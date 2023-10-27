////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
// TED5-4
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////
#include "ted5.h"
#pragma hdrstop


void SignalSound(void)
{
 int i;

 for(i=0;i<10;i++)
 {
  sound(500+i*500);
  delay(5);
  nosound();
 }
}


////////////////////////////////////////////////////
//
// Create an OBJ linkable file from any type of datafile
//
// Exit:
//  0 = everything's a-ok!
// -1 = file not found
// -2 = file >64K
//
////////////////////////////////////////////////////
int MakeOBJ(char *filename,char *destfilename,char *public,segtype whichseg,char *farname)
{
 char THEADR[17]={0x80,14,0,12,32,32,32,32,32,32,32,32,32,32,32,32,0},
      COMENT[18]={0x88,0,0,0,0,'M','a','k','e','O','B','J',' ','v','1','.','1',0},
      LNAMES[42]={0x96,0,0,
		  6,'D','G','R','O','U','P',
		  5,'_','D','A','T','A',
		  4,'D','A','T','A',
		  0,
		  5,'_','T','E','X','T',
		  4,'C','O','D','E',
		  8,'F','A','R','_','D','A','T','A'},
      SEGDEF[9]={0x98,7,0,0x48,0,0,2,3,4},	// for .DATA
      SEGDEF1[9]={0x98,7,0,0x48,0,0,5,6,4},	// for .CODE
      SEGDEF2[9]={0x98,7,0,0x60,0,0,8,7,4},	// for .FARDATA
      GRPDEF[7]={0x9a,4,0,1,0xff,1,0x61},
      MODEND[5]={0x8a,2,0,0,0x74};

 unsigned i,j,flag,handle;
 long fsize,offset,loffset,temp,amtleft,amount,offset1;
 char _seg *dblock,*block;


 //
 // Need to compute the CHECKSUM in the COMENT field
 // (so if the "MakeOBJ..." string is modified, the CHECKSUM
 //  will be correct).
 //
 COMENT[1]=sizeof(COMENT)-3;
 for (flag=i=0;i<sizeof(COMENT);i++)
    flag+=COMENT[i];
 COMENT[sizeof(COMENT)-1]=(flag^0xff)+1;

 if ((handle=open(filename,O_BINARY))==NULL)
   return -1;

 fsize=filelength(handle);
 close(handle);
 if (fsize>0x10000L)		// BIGGER THAN 1 SEG = ERROR!
   return -2;

 LoadIn(filename,(memptr *)&block);	// LOAD FILE IN
 offset=0;

 MMAllocate((memptr *)&dblock,0x10000L);

 ////////////////////////////////////////////////////
 //
 // INSERT HEADER RECORD
 //
 movedata(_DS,FP_OFF(&THEADR),(unsigned)dblock,offset,sizeof(THEADR));
 movedata(FP_SEG(filename),FP_OFF(filename),
	  (unsigned)dblock,offset+4,strlen(filename));
 offset+=sizeof(THEADR);


 ////////////////////////////////////////////////////
 //
 // INSERT COMMENT RECORD
 //
 movedata(_DS,FP_OFF(COMENT),(unsigned)dblock,offset,sizeof(COMENT));
 offset+=sizeof(COMENT);


 ////////////////////////////////////////////////////
 //
 // INSERT START OF LIST-OF-NAMES RECORD
 //
 loffset=offset;
 movedata(_DS,FP_OFF(LNAMES),(unsigned)dblock,offset,sizeof(LNAMES));
 offset+=sizeof(LNAMES);

 // If it's a .FARDATA segment, we need to insert the segment name!
 if (whichseg==FARDATA)
   {
    *(dblock+offset)=strlen(farname);
    movedata(FP_SEG(farname),FP_OFF(farname),
	(unsigned)dblock,offset+1,strlen(farname));
    offset+=strlen(farname)+1;
   }

 // Now, finish the List-Of-Names record by creating
 // the CHECKSUM and LENGTH
 temp=offset;
 offset=offset-loffset-2;
 *(int huge *)(dblock+loffset+1)=offset;
 offset=temp;

 // Now, figure out the CHECKSUM of the record
 for (flag=i=0;i<(offset-loffset);i++)
   flag+=*(dblock+i+loffset);
 *(dblock+offset)=(flag^0xff)+1;
 offset++;


 ////////////////////////////////////////////////////
 //
 // CREATE SEGMENT DEFINITION RECORD
 //
 loffset=offset;
 temp=fsize;
 switch(whichseg)
 {
  case DATA:
    movedata(FP_SEG(&SEGDEF),FP_OFF(&SEGDEF),
	     (unsigned)dblock,offset,sizeof(SEGDEF));
    *(int huge *)(dblock+offset+4)=temp;
    offset+=sizeof(SEGDEF);
    break;
  case CODE:
    movedata(FP_SEG(&SEGDEF1),FP_OFF(&SEGDEF1),
	     (unsigned)dblock,offset,sizeof(SEGDEF1));
    *(int huge *)(dblock+offset+4)=temp;
    offset+=sizeof(SEGDEF1);
    break;
  case FARDATA:
    movedata(FP_SEG(&SEGDEF2),FP_OFF(&SEGDEF2),
	     (unsigned)dblock,offset,sizeof(SEGDEF2));
    *(int huge *)(dblock+offset+4)=temp;
    offset+=sizeof(SEGDEF2);
    break;
 }

 // CHECKSUM
 for (flag=0,i=loffset;i<offset;i++)
   flag+=*(dblock+i);
 *(dblock+offset)=(flag^0xff)+1;
 offset++;


 ////////////////////////////////////////////////////
 //
 // CREATE GROUP DEFINITION RECORD
 //
 switch(whichseg)
 {
  case DATA:
  case CODE:
    movedata(FP_SEG(&GRPDEF),FP_OFF(&GRPDEF),
	     (unsigned)dblock,offset,sizeof(GRPDEF));
    offset+=sizeof(GRPDEF);
 }


 ////////////////////////////////////////////////////
 //
 // CREATE PUBLIC DEFINITION RECORD
 //
 loffset=offset;
 *(dblock+offset)=0x90;		// PUBDEF ID
 offset+=3;			// point to public base, skip length
 *(dblock+offset)=1;		// group index=1
 *(dblock+offset+1)=1;		// segment index=1
 offset+=2;			// point to public name

 temp=0;
 movedata(FP_SEG(public),FP_OFF(public),
	  (unsigned)dblock,offset+1,strlen(public));
 *(dblock+offset)=strlen(public);
 offset+=strlen(public)+1;
 *(int huge *)(dblock+offset)=0;	// public offset within segment
 offset+=2;
 *(dblock+offset)=0;		// type index
 offset++;

 // LENGTH
 temp=offset-loffset-2;
 *(int huge *)(dblock+loffset+1)=temp;
 offset++;

 // CHECKSUM
 for (flag=0,i=loffset;i<offset;i++)
   flag+=*(dblock+i);
 *(dblock+offset)=(flag^0xff)+1;


 ////////////////////////////////////////////////////
 //
 // DATA RECORD(S). YUCK.
 //

 amtleft=fsize;
 amount=1024;
 for (i=0;i<(fsize+1023)/1024;i++)
   {
    offset1=offset;
    if (amtleft<1024)
      amount=amtleft;
    //
    // RECORD HEADER
    //
    *(dblock+offset)=0xa0;			// LEDATA ID
    *(int huge *)(dblock+offset+1)=amount+4;	// length of record
    offset+=3;
    *(dblock+offset)=1;				// segment index
    *(int huge *)(dblock+offset+1)=i*1024;	// index into segment
    offset+=3;
    //
    // LOAD DATA IN
    //
    LoadFile(filename,(char huge *)dblock+offset,i*1024,amount);
    offset+=amount;
    //
    // CHECKSUM!
    //
    for (flag=0,j=offset1;j<offset;j++)
      flag+=*(dblock+j);
    *(dblock+offset)=(flag^0xff)+1;
    offset++;

    amtleft-=1024;
   }

 ////////////////////////////////////////////////////
 //
 // MODULE END! YES!
 //
 movedata(FP_SEG(&MODEND),FP_OFF(&MODEND),(unsigned)dblock,offset,sizeof(MODEND));
 offset+=sizeof(MODEND);

 //
 // Save the little puppy out!
 //
 SaveFile(destfilename,(char huge *)dblock,0,offset);
 MMFreePtr((memptr *)&dblock);
 MMFreePtr((memptr *)&block);
 return 0;
}


////////////////////////////////////////////////////
//
// DUMP THE PASTE BUFFER OUT TO EITHER AN
// "APPLE PREFERRED" OR "ILBM" GRAPHICS FORMAT FILE
//
// NOTE: THIS IS ONLY AVAILABLE IN EGA, BECAUSE I
// DON'T FEEL LIKE WRITING A CGA ILBM PIXEL-SPLICER --
// AND APPLE PREFERRED ONLY HANDLES 4-BIT COLOR ANYWAY!
// (YES, I KNOW, I COULD ONLY USE 4 OUT OF 16 COLORS...SHUT UP!)
//
////////////////////////////////////////////////////
btype dumpB[]={{" DeluxePaint II ILBM ",4,2,2},
	       {"   Apple Preferred   ",4,5,1}},
      fnameB={"         ",4,3,1};
DialogDef dumpD={" Which format do you require?",30,7,2,&dumpB[0],NULL},
	  fnameD={"Enter filename to\nsave (no suffix)",17,5,1,&fnameB,NULL};

void Item_GraphicDump(void)
{
 char filename[14],ext[5],_seg *block,_seg *block1;
 ApPrefStr PrefHeader;
 int which,i,j,k,m,n,pwidth,lwidth,dx,dy;
 long tilelen,fsize=0,bufsize;

 if (!PasteOK)
   {
    ErrDialog("You need to use the Copy command\n"
	      "to copy part of the map or tiles\n"
	      "so I know what I need to dump!"," OK ");
    return;
   }

 switch(videomode)
 {
  case CGA:
  case VGA:
    ErrDialog("Sorry, but this function is only\n"
	      "available for EGA mode.  If you\n"
	      "have a REAL NEED for this to work\n"
	      "in CGA or VGA, talk to Id Software!"," OK ");
    return;
 }

 if (!(which=DoDialog(&dumpD)))
   return;

 //
 // SET TILE LENGTH
 //
 switch(tsize)
 {
  case 1: tilelen=32L; break;
  case 2: tilelen=128L; break;
  case 3: tilelen=512L;
 }

 //
 // GET FILENAME TO SAVE UNDER
 //
 DrawDialog(&fnameD,1);
 MouseHide();
 GetButtonXY(&fnameD,0,&sx,&sy);
 if (!input(filename,8))
   {
    RestoreBackground();
    MouseShow();
    return;
   }
 for (i=0;i<strlen(filename);i++)
   if (filename[i]=='.')
     {
      filename[i]=0;
      break;
     }
 RestoreBackground();
 MouseShow();

 //
 // SETUP FOR EACH TYPE
 //
 switch(which)
 {
  case 1:	// ILBM
    {
     long size;
     char form[5]="FORM",ilbm[9]="ILBMBMHD",body[5]="BODY";


     size=48L;
     MMAllocate((memptr *)&block,size);

     movedata(FP_SEG(form),FP_OFF(form),(unsigned)block,fsize,4);
     fsize+=4;
     size=40L+tilelen*TileCopy.w*TileCopy.h;
     *(block+fsize)=(size>>24)&0xff;
     *(block+fsize+1)=(size>>16)&0xff;
     *(block+fsize+2)=(size>>8)&0xff;
     *(block+fsize+3)=size&0xff;
     fsize+=4;
     movedata(FP_SEG(ilbm),FP_OFF(ilbm),(unsigned)block,fsize,8);
     fsize+=8;
     *(block+fsize)=0;
     *(block+fsize+1)=0;
     *(block+fsize+2)=0;
     *(block+fsize+3)=20;
     fsize+=4;
     *(block+fsize)=(TileCopy.w<<(tsize+2))/256;   // pixel width
     *(block+fsize+1)=(TileCopy.w<<(tsize+2))&0xff;
     *(block+fsize+2)=(TileCopy.h<<(tsize+2))/256; // pixel height
     *(block+fsize+3)=(TileCopy.h<<(tsize+2))&0xff;
     *(int huge *)(block+fsize+4)=0;		// Xorg
     *(int huge *)(block+fsize+6)=0;		// Yorg
     *(block+fsize+8)=4;			// planes
     *(block+fsize+9)=0;			// mask (stencil!)
     *(block+fsize+10)=0;			// compression (none)
     *(block+fsize+11)=0;			// pad (?)
     *(int huge *)(block+fsize+12)=0;		// trans (?)
     *(int huge *)(block+fsize+14)=0x101;	// aspt (aspect?)
     *(int huge *)(block+fsize+16)=0x4001;	// page width
     *(int huge *)(block+fsize+18)=0xc800;	// page height
     fsize+=20;
     movedata(FP_SEG(body),FP_OFF(body),(unsigned)block,fsize,4);
     fsize+=4;
     size=tilelen*TileCopy.w*TileCopy.h;
     *(block+fsize)=(size>>24)&0xff;
     *(block+fsize+1)=(size>>16)&0xff;
     *(block+fsize+2)=(size>>8)&0xff;
     *(block+fsize+3)=size&0xff;
     fsize+=4;

     strcpy(ext,".LBM");
    }
    break;

  case 2:	// APPLE PREFERRED
    {
     int Ctable[16]={0x0000,0x000a,0x00a0,0x00aa,0x0a00,0x0a0a,0x0a50,0x0aaa,
		     0x0555,0x055f,0x05f5,0x05ff,0x0f55,0x0f5f,0x0ff5,0x0fff};
     long size,pixwid;


     PrefHeader.length=sizeof(ApPrefStr)+4L*(TileCopy.h<<(tsize+2))+
	TileCopy.w*TileCopy.h*tilelen+(((TileCopy.w*TileCopy.h*tilelen)+63)/64);
     strncpy(PrefHeader.Kind,"\x4MAIN",5);
     PrefHeader.MasterMode=0;
     PrefHeader.PixelsPerLine=TileCopy.w<<(tsize+2);
     PrefHeader.NumColorTables=1;
     for (i=0;i<16;i++)
       PrefHeader.ColorTable[i]=Ctable[i];
     PrefHeader.NumScanLines=TileCopy.h<<(tsize+2);

     size=sizeof(ApPrefStr)+4L*(TileCopy.h<<(tsize+2));
     MMAllocate((memptr *)&block,size);
     movedata(FP_SEG(&PrefHeader),FP_OFF(&PrefHeader),(unsigned)block,fsize,sizeof(ApPrefStr));
     fsize+=sizeof(ApPrefStr);

     pixwid=TileCopy.w*(2<<tsize);
     for (i=0;i<(TileCopy.h<<(tsize+2));i++)
       {
	*(int huge *)(block+fsize)=pixwid+(pixwid+63)/64;
	*(int huge *)(block+fsize+2)=0;
	fsize+=4;
       }

     strcpy(ext,".APP");
     MMAllocate((memptr *)&block1,tilelen*TileCopy.w);
    }
 }

 //
 // SAVE HEADER OUT
 //
 strcat(filename,ext);
 SaveFile(filename,(char huge *)block,0,fsize);
 MMFreePtr((memptr *)&block);

 //
 // NOW, WRITE THE DATA OUT! EEEE!
 //
 bufsize=tilelen*TileCopy.w;
 MMAllocate((memptr *)&block,bufsize);

 pwidth=(1<<(tsize-1))*TileCopy.w;
 lwidth=pwidth*4;

 ErrDialog("One moment. I am busy.\n     Countdown:","");
 dx=sx;
 dy=sy;

 for (j=0;j<TileCopy.h;j++)
   {
    sx=dx;
    sy=dy;
    printint(TileCopy.h-j);
    print(" ");

    for (i=0;i<TileCopy.w;i++)
      {
       unsigned tilet,tilem,tilei,loc;

       //
       // ESC out?
       //
       if (keydown[1])
	 {
	  RestoreBackground();
	  while(keydown[1]);
	  ErrDialog("You aborted out of the\n"
		    "graphic dump conversion!"," Yes, I know ");

	  if (which==2)
	    MMFreePtr((memptr *)&block1);
	  return;
	 }
       //
       // GET THE CORRECT TILE MOVED INTO "TDATA"
       //
       switch(TileCopy.MapOrTileSelect)
       {
	case 0:	// MAP COPY
	  loc=(j+TileCopy.y)*mapwidth+TileCopy.x+i;
	  tilet=(TileCopy.PlanesCopied&BPLANE)?CutBkgnd[loc]:-BkgndColor;
	  tilem=(TileCopy.PlanesCopied&FPLANE)?CutFrgnd[loc]+tilenum:0;
	  tilei=(TileCopy.PlanesCopied&IPLANE)?CutInfoPl[loc]+tilenum:0;
	  CombineTiles(tilet,tilem,tilei,tsize);
	  break;
	case 1:	// BKGND TILE COPY
	  loc=(TileCopy.y+j)*selectcols+TileCopy.x+i;
	  CombineTiles(loc,0,0,tsize);
	  break;
	case 2:	// FRGND TILE COPY
	  loc=(TileCopy.y+j)*selectcols+TileCopy.x+i+tilenum;
	  CombineTiles(-BkgndColor,loc,0,tsize);
       }

       //
       // NOW, MUNGE "TDATA" INTO BIT-PLANES!
       //
       // INTERNAL:		P0	P1	P2	P3
       //	SCANLINE x	......	......	......	......
       //
       for (k=0;k<8<<(tsize-1);k++)
	 for (m=0;m<4;m++)
	   movedata(FP_SEG(&tdata),FP_OFF(&tdata)+(m*tsize*(8<<(tsize-1)))+k*(1<<(tsize-1)),
		(unsigned)block,k*lwidth+m*pwidth+i*(1<<(tsize-1)),1<<(tsize-1));
      }
    //
    // ALRIGHT. NOW SAVE THIS CHUNK OUT IN THE CORRECT FORMAT
    //
    switch(which)
    {
     case 1:	// ILBM
       SaveFile(filename,MK_FP(block,0),fsize,bufsize);
       fsize+=bufsize;
       break;
     case 2:	// APPLE PREFERRED
       {
	int len,clen;

	//
	// FIRST, I NEED TO CONVERT THE ILBM PLANAR FORMAT
	// TO SUPER-RES NIBBLES
	//
	for (k=0;k<8<<(tsize-1);k++)
	  for (m=0;m<(1<<(tsize-1))*TileCopy.w;m++)
	    {
	     unsigned char src[4],dest[4]={0,0,0,0};

	     for (n=0;n<4;n++)
	       src[n]=block[k*lwidth+pwidth*n+m];

	     asm	mov	al,[BYTE PTR src+0]
	     asm	mov	ah,[BYTE PTR src+1]
	     asm	mov	bl,[BYTE PTR src+2]
	     asm	mov	bh,[BYTE PTR src+3]

	     // dest[0]
	     asm	shl	bh,1
	     asm	rcl	cl,1
	     asm	shl	bl,1
	     asm	rcl	cl,1
	     asm	shl	ah,1
	     asm	rcl	cl,1
	     asm	shl	al,1
	     asm	rcl	cl,1

	     asm	shl	bh,1
	     asm	rcl	cl,1
	     asm	shl	bl,1
	     asm	rcl	cl,1
	     asm	shl	ah,1
	     asm	rcl	cl,1
	     asm	shl	al,1
	     asm	rcl	cl,1

	     // dest[1]
	     asm	shl	bh,1
	     asm	rcl	ch,1
	     asm	shl	bl,1
	     asm	rcl	ch,1
	     asm	shl	ah,1
	     asm	rcl	ch,1
	     asm	shl	al,1
	     asm	rcl	ch,1

	     asm	shl	bh,1
	     asm	rcl	ch,1
	     asm	shl	bl,1
	     asm	rcl	ch,1
	     asm	shl	ah,1
	     asm	rcl	ch,1
	     asm	shl	al,1
	     asm	rcl	ch,1

	     // dest[2]
	     asm	shl	bh,1
	     asm	rcl	dl,1
	     asm	shl	bl,1
	     asm	rcl	dl,1
	     asm	shl	ah,1
	     asm	rcl	dl,1
	     asm	shl	al,1
	     asm	rcl	dl,1

	     asm	shl	bh,1
	     asm	rcl	dl,1
	     asm	shl	bl,1
	     asm	rcl	dl,1
	     asm	shl	ah,1
	     asm	rcl	dl,1
	     asm	shl	al,1
	     asm	rcl	dl,1

	     // dest[3]
	     asm	shl	bh,1
	     asm	rcl	dh,1
	     asm	shl	bl,1
	     asm	rcl	dh,1
	     asm	shl	ah,1
	     asm	rcl	dh,1
	     asm	shl	al,1
	     asm	rcl	dh,1

	     asm	shl	bh,1
	     asm	rcl	dh,1
	     asm	shl	bl,1
	     asm	rcl	dh,1
	     asm	shl	ah,1
	     asm	rcl	dh,1
	     asm	shl	al,1
	     asm	rcl	dh,1

	     asm	mov	[BYTE PTR dest+0],cl
	     asm	mov	[BYTE PTR dest+1],ch
	     asm	mov	[BYTE PTR dest+2],dl
	     asm	mov	[BYTE PTR dest+3],dh

	     movedata(FP_SEG(&dest),FP_OFF(&dest),
		(unsigned)block1,k*lwidth+m*4,4);
	    }

	//
	// NOW, TIME TO WRITE THE DATA OUT IN 64-BYTE CHUNKS! YUCK!
	//
	for (k=0;k<8<<(tsize-1);k++)
	  {
	   char clen;
	   int off=k*lwidth;;

	   len=4*pwidth;
	   while(len>0)
	   {
	    if (len>64)
	      clen=64;
	    else
	      clen=len;

	    clen--;
	    SaveFile(filename,(char huge *)&clen,fsize++,1);
	    clen++;

	    SaveFile(filename,MK_FP(block1,off),fsize,clen);
	    fsize+=clen;
	    off+=clen;
	    len-=clen;
	   }
	  }
       }
    }
   }

 if (which==2)
   MMFreePtr((memptr *)&block1);

 MMFreePtr((memptr *)&block);
 RestoreBackground();

 ErrDialog("Graphic successfully dumped!"," Yeah! ");
}


////////////////////////////////////////////////////
//
// Item - Edit TILEINFO/M values
//
////////////////////////////////////////////////////
btype ETVb[]={{" Tileinfo ",2,21,1},
	      {" TileinfoM ",16,21,1},
	      {" Exit ",30,21,2}};
DialogDef ETVd={"       Edit TILEINFO/M values",38,23,3,&ETVb[0],NULL};
int CurTIvalue;

void Item_EditTinfoValues(void)
{
 int max,i,which,exitok=0,mx,my,b0,b1,CTRLdown;
 static int whichtinfo=0;

 //
 // IS THE "CTRL" KEY DOWN?
 //
 CTRLdown=keydown[0x1d];
 if (CTRLdown)
   {
    if (planeton)
      whichtinfo=0;
    else
      whichtinfo=1;
   }

 switch(whichtinfo)
 {
  case 0: max=tilenum; break;
  case 1: max=tilemnum;
 }

 switch(videomode)
 {
  case CGA:
  case EGA1:
  case VGA:
    ETVd.height=23;
    for(i=0;i<3;i++)
      ETVb[i].yoff=21;
    break;
  case EGA2:
    ETVd.height=58;
    for(i=0;i<3;i++)
      ETVb[i].yoff=56;
 }

 //
 // DRAW THE SCREEN
 //
 DrawDialog(&ETVd,1);
 if (CTRLdown)
   {
    DrawTinfoScreen(whichtinfo,0,-max);	// ALIGN TO TOP
    switch(whichtinfo)
    {
     case 0: DrawTinfoScreen(whichtinfo,0,whicht); break;
     case 1: DrawTinfoScreen(whichtinfo,0,whichtm-tilenum);
    }
   }
 else
   DrawTinfoScreen(whichtinfo,0,0);


 do
 {
  which=CheckButtonsRet(&ETVd);
  if (which>=0)
    switch(which)
    {
     case 0:
       RestoreBackground();
       return;
     case 1:
       if (whichtinfo)
	 {
	  max=tilenum;
	  whichtinfo=0;
	  DrawDialog(&ETVd,0);
	  DrawTinfoScreen(whichtinfo,0,0);
	 }
       else
	 {
	  GetButtonXY(&ETVd,0,&sx,&sy);
	  MouseHide();
	  print(ETVb[0].text);
	  MouseShow();
	  errsound();
	 }
       break;
     case 2:
       if (!tilemnum)
	 {
	  GetButtonXY(&ETVd,1,&sx,&sy);
	  MouseHide();
	  print(ETVb[1].text);
	  MouseShow();
	  errsound();
	  break;
	 }

       if (!whichtinfo)
	 {
	  max=tilemnum;
	  whichtinfo=1;
	  DrawDialog(&ETVd,0);
	  DrawTinfoScreen(whichtinfo,0,0);
	 }
       else
	 {
	  GetButtonXY(&ETVd,1,&sx,&sy);
	  MouseHide();
	  print(ETVb[1].text);
	  MouseShow();
	  errsound();
	 }
       break;

     case 3:
       exitok=1;
    }
  else
    {
     MouseCoords(&mx,&my);
     mx/=8;
     my/=8;
     b0=MouseButton()&1;
     b1=MouseButton()&2;

     //
     // CHECK FOR BUTTON PRESSES
     //
     if (b0)
       UseTinfoValue(whichtinfo,mx,my,1);
     else
     if (b1)
       UseTinfoValue(whichtinfo,mx,my,0);

     //
     // SPACE = ENTER VALUES HORIZONTALLY
     //
     if (keydown[0x39])
     {
      while(keydown[0x39]);
      clearkeys();
      EnterTinfoValue(whichtinfo,mx,my,0);
     }

     //
     // CHECK FOR SCROLLING
     //
     if (keydown[0x48])		// UP
       {
	DrawTinfoScreen(whichtinfo,0,-1);
	if (keydown[0x1d])
	  while(keydown[0x48]);
       }
     else
     if (keydown[0x50])		// DOWN
       {
	DrawTinfoScreen(whichtinfo,0,1);
	if (keydown[0x1d])
	  while(keydown[0x50]);
       }
     else
     if (keydown[0x49])		// PGUP
       {
	DrawTinfoScreen(whichtinfo,0,-8);
	if (!keydown[0x1d])
	  while(keydown[0x49]);
       }
     else
     if (keydown[0x51])		// PGDN
       {
	DrawTinfoScreen(whichtinfo,0,8);
	if (!keydown[0x1d])
	  while(keydown[0x51]);
       }
     else
     if (keydown[0x47])		// HOME
       {
	DrawTinfoScreen(whichtinfo,0,-max);
	while(keydown[0x47]);
       }
     else
     if (keydown[0x4f])		// END
       {
	DrawTinfoScreen(whichtinfo,0,max);
	while(keydown[0x4f]);
       }
     else
     if (keydown[0x4d])		// RIGHT
       {
	DrawTinfoScreen(whichtinfo,1,0);
	if (!keydown[0x1d])
	  while(keydown[0x4d]);
       }
     else
     if (keydown[0x4b])		// LEFT
       {
	DrawTinfoScreen(whichtinfo,-1,0);
	if (!keydown[0x1d])
	  while(keydown[0x4b]);
       }

    }

 } while(!exitok);

 RestoreBackground();
}


//
// PICKUP/DROP TILEINFO VALUE AT CURSOR
//
void UseTinfoValue(int whichtinfo,int mx,int my,int PickupOrDrop)
{
 int whichx=-1,whichy=-1;
 unsigned dialogx,dialogy;


 GetDialogXY(&ETVd,&dialogx,&dialogy);

 if (mx>=dialogx+10 && mx<=dialogx+45)
   whichx=(mx-(dialogx+10))/7;

 if (my>=4 && my<=((videomode==EGA2)?55:19))
   whichy=(my-4)>>(tsize-1);

 switch(whichtinfo)
 {
  case 0:
    if (whichx>=numtplanes)
      whichx=-1;
    if (whichy>=tilenum)
      whichy=-1;
    break;

  case 1:
    if (whichx>=numtmplanes)
      whichx=-1;
    if (whichy>=tilemnum)
      whichy=-1;
 }

 if (whichx>=0 && whichy>=0)
 {
  if (!PickupOrDrop)
    switch(whichtinfo)
    {
     case 0:	// TILE
       CurTIvalue=*(Tinfo[whichx+TIxbase]+whichy+TIybase);
       break;
     case 1:	// MASKED
       CurTIvalue=*(TMinfo[whichx+TIxmbase]+whichy+TIymbase);
    }
  else
    switch(whichtinfo)
    {
     case 0:	// TILE
       *(Tinfo[whichx+TIxbase]+whichy+TIybase)=CurTIvalue;
       DirtyFlag=1;
       break;
     case 1:	// MASKED
       *(TMinfo[whichx+TIxmbase]+whichy+TIymbase)=CurTIvalue;
       DirtyFlag=1;
    }

  DrawTinfoScreen(whichtinfo,0,0);
 }
 while(MouseButton());
}


//
// ENTER TILEINFO/M CELL VALUES
//
void EnterTinfoValue(int whichtinfo,int mx,int my,int H_or_V)
{
 int whichx=-1,whichy=-1,val,outok=0,tempx,tempy,maxx,maxy;
 unsigned dialogx,dialogy;


 GetDialogXY(&ETVd,&dialogx,&dialogy);

 if (mx>=dialogx+10 && mx<=dialogx+45)
   whichx=(mx-(dialogx+10))/7;

 if (my>=4 && my<=((videomode==EGA2)?55:19))
   whichy=(my-4)>>(tsize-1);

 switch(whichtinfo)
 {
  case 0:
    if (whichx>=numtplanes)
      whichx=-1;
    if (whichy>=tilenum)
      whichy=-1;
    break;

  case 1:
    if (whichx>=numtmplanes)
      whichx=-1;
    if (whichy>=tilemnum)
      whichy=-1;
 }

 MouseHide();

 if (whichx>=0 && whichy>=0)
   do
   {
    //
    // INPUT VALUE
    //
    sx=whichx*7+dialogx+10;
    sy=(whichy<<(tsize-1))+4;
    print("    ");
    sx-=4;
    if ((val=inputint(3))!=(int)ESCOUT)
      switch(whichtinfo)
      {
       case 0:	// TILE
	 *(Tinfo[whichx+TIxbase]+whichy+TIybase)=val&0xff;
	 DirtyFlag=1;
	 break;
       case 1:	// MASKED
	 *(TMinfo[whichx+TIxmbase]+whichy+TIymbase)=val&0xff;
	 DirtyFlag=1;
      }
    else
      outok=1;

    //
    // INPUT INTO THE NEXT FIELD!
    //
    if (!outok)
      {
       tempx=(whichtinfo?TIxmbase:TIxbase);
       tempy=(whichtinfo?TIymbase:TIybase);
       maxx=(whichtinfo?numtmplanes:numtplanes);
       maxy=(whichtinfo?tilemnum:tilenum);

       switch(H_or_V)
       {
	case 0:	// HORIZONTAL
	  whichx++;
	  if (tempx+whichx>=maxx)
	    outok=1;
	  else
	  if (whichx>TINFOWIDTH)
	    {
	     whichx=TINFOWIDTH;
	     tempx++;
	     if (tempx>=maxx)
	       outok=1;
	    }

	  switch(whichtinfo)
	  {
	   case 0: TIxbase=tempx; break;
	   case 1: TIxmbase=tempx;
	  }
	  break;

	case 1:	// VERTICAL
	  whichy++;
	  if (tempy+whichy>=maxy)
	    outok=1;
	  else
	  if (whichy>(videomode==EGA2?TINFOHEIGHTEGA2:TINFOHEIGHT))
	    {
	     whichy=(videomode==EGA2?TINFOHEIGHTEGA2:TINFOHEIGHT);
	     tempy++;
	     if (tempy>=maxy)
	       outok=1;
	    }

	  switch(whichtinfo)
	  {
	   case 0: TIybase=tempy; break;
	   case 1: TIymbase=tempy;
	  }
       }
      }

    DrawTinfoScreen(whichtinfo,0,0);

   } while (!outok);

 MouseShow();
}



//
// Draw the Tileinfo screen
//
void DrawTinfoScreen(int thescreen,int deltax,int deltay)
{
 int temp,temp1,i,j,width,height,dialogx;
 char _seg *Values[10];


 MouseHide();
 switch(videomode)
 {
  case CGA:
  case EGA1:
  case VGA:
    dialogx=1;
    height=16>>(tsize-1); break;
  case EGA2:
    dialogx=21;
    height=52>>(tsize-1);
 }

 switch(thescreen)
 {
  case 0:	// TILEINFO
    if (height>tilenum)
      height=tilenum;
    else
      {
       TIybase+=deltay;
       if (TIybase<0)
	 TIybase=0;
       else
       if (TIybase+height>tilenum)
	 TIybase=tilenum-height;
      }
    temp=TIybase;
    break;

  case 1:	// TILEINFOM
    if (height>tilemnum)
      height=tilemnum;
    else
      {
       TIymbase+=deltay;
       if (TIymbase<0)
	 TIymbase=0;
       else
       if (TIymbase+height>tilemnum)
	 TIymbase=tilemnum-height;
      }
    temp=TIymbase;
 }

 //
 // DRAW TILES AND THEIR VALUES
 //
 for (i=0;i<height;i++)
   {
    sx=dialogx;
    sy=(i<<(tsize-1))+4;
    printhex(i+temp);
    sx=dialogx;
    sy++;
    print("    ");
    sx-=4;
    printint(i+temp);

    switch(thescreen)
    {
     case 0:
       CombineTiles(temp+i,0,0,tsize);
       break;
     case 1:
       CombineTiles(-BkgndColor,temp+i+tilenum,0,tsize);
    }

    DrawTile(dialogx+5,(i+2)<<(3+(tsize-1)),tsize);
   }

 //
 // DRAW TILEINFO ARRAYS
 //
 width=4;

 switch(thescreen)
 {
  case 0:
    if (width>numtplanes)
      width=numtplanes;
    else
      {
       TIxbase+=deltax;
       if (TIxbase<0)
	 TIxbase=0;
       else
       if (TIxbase+width>numtplanes)
	 TIxbase=numtplanes-width;
      }
    temp1=TIxbase;
    for (i=0;i<10;i++)
      Values[i]=Tinfo[i];

    break;

  case 1:
    if (width>numtmplanes)
      width=numtmplanes;
    else
      {
       TIxmbase+=deltax;
       if (TIxmbase<0)
	 TIxmbase=0;
       else
       if (TIxmbase+width>numtmplanes)
	 TIxmbase=numtmplanes-width;
      }
    temp1=TIxmbase;
    for (i=0;i<10;i++)
      Values[i]=TMinfo[i];
 }

 for (j=0;j<width;j++)
   {
    sx=(dialogx+10)+j*7;
    sy=3;
    switch(thescreen)
    {
     case 0:
       print("       ");
       sx-=7;
       fprint(MapFileHeader->tnames[j+TIxbase]);
       break;
     case 1:
       print("       ");
       sx-=7;
       fprint(MapFileHeader->tmnames[j+TIxmbase]);
    }
    for (i=0;i<height;i++)
      {
       sx=(dialogx+10)+j*7;
       sy=(i<<(tsize-1))+4;
       print("$");
       printhexb(Values[j+temp1][i+temp]);
       sx=(dialogx+10)+j*7;
       sy++;
       print("   ");
       sx-=3;
       printint(Values[j+temp1][i+temp]&0xff);
      }
   }
 MouseShow();
}



////////////////////////////////////////////////////
//
// Item - Add or Delete TILEINFO/M planes
//
////////////////////////////////////////////////////
btype AODb[]={{" Add ",1,3,1},
	      {" Delete ",8,3,1},
	      {" Exit ",18,3,2}},
      TOMb[]={{" Tileinfo  ",1,2,1},
	      {" TileinfoM ",1,5,1}},
      AreSureB2[]={{" Yes ",1,2,1},
		   {" No ",8,2,2}},
      TINb2={" Done ",8,15,1};;
DialogDef AODd={"       TILEINFO/M\n     Pick a function",25,5,3,&AODb[0],NULL},
	  TOMd={"Add to what?",13,7,2,&TOMb[0],NULL},
	  NId={"Gimme a name!",13,4,0,NULL,NULL},
	  AreSureD2={"Are you sure?",13,4,2,&AreSureB2[0],NULL},
	  TINd2={"   Which to delete?\n"
		" TILEINFO   TILEINFOM",
		22,17,1,&TINb2,NULL};

void Item_AddDelTinfo(void)
{
 char temp[16];
 int which,i;
 unsigned dx,dy;

 while(1)
 {
  which=DoDialog(&AODd);
  switch(which)
  {
   case 0:	// ESC or EXIT
   case 3:
     return;
   //
   // ADD
   //
   case 1:
     which=DoDialog(&TOMd);
     if (!which)
       break;

     switch(which)
     {
      case 1:
	if (numtplanes==10)
	  {
	   ErrDialog("You already have 10 TILEINFO\n"
		     "planes defined!"," OK ");
	   return;
	  }
	break;

      case 2:
	if (!tilemnum)
	  {
	   ErrDialog("You crazy shit! You don't\n"
		     "have any masked tiles!"," Duh! ");
	   return;
	  }

	if (numtmplanes==10)
	  {
	   ErrDialog("You already have 10 TILEINFOM\n"
		     "planes defined!"," OK ");
	   return;
	  }
	break;
     }

     DrawDialog(&NId,1);
     MouseHide();
     GetDialogXY(&NId,&dx,&dy);
     sx=dx+1;
     sy=dy+1;
     DrawBorder(sx,sy,9,2,1);
     sx=dx+2;
     sy=dy+2;
     if (!input(temp,7))
       {
	RestoreBackground();
	MouseShow();
	break;
       }

     //
     // MOVE THE NAME AND ALLOCATE THE MEMORY!
     //
     switch(which)
     {
      case 1:
	movedata(FP_SEG(temp),FP_OFF(temp),
		 (unsigned)MapFileHeader,FP_OFF(MapFileHeader->tnames[numtplanes]),8);
	MMAllocate((memptr *)&Tinfo[numtplanes],tilenum);
	for (i=0;i<tilenum;i++)
	  *(Tinfo[numtplanes]+i)=0;
	MapFileHeader->numtplanes=++numtplanes;
	writeH=DirtyFlag=1;
	break;

      case 2:
	movedata(FP_SEG(temp),FP_OFF(temp),
		 (unsigned)MapFileHeader,FP_OFF(MapFileHeader->tmnames[numtmplanes]),8);
	MMAllocate((memptr *)&TMinfo[numtmplanes],tilemnum);
	for (i=0;i<tilemnum;i++)
	  *(TMinfo[numtmplanes]+i)=0;
	MapFileHeader->numtmplanes=++numtmplanes;
	writeH=DirtyFlag=1;
     }

     RestoreBackground();
     MouseShow();
     break;

   //
   // DELETE
   //
   case 2:
     {
      unsigned ox,oy,i,oktoexit=0;
      int which;

      redo:

      MouseHide();
      DrawDialog(&TINd2,1);
      GetDialogXY(&TINd2,&sx,&sy);
      ox=sx;
      oy=sy;
      DrawBorder(sx,sy+2,10,11,1);
      sx=ox;
      sy=oy;
      DrawBorder(sx+11,sy+2,10,11,1);

      for (i=0;i<10;i++)
      {
       sx=ox+1;
       sy=oy+i+3;
       fprint(MapFileHeader->tnames[i]);
       sx=ox+12;
       sy=oy+i+3;
       fprint(MapFileHeader->tmnames[i]);
      }
      MouseShow();

      do
      {
       if ((which=CheckList(ox+1,oy+3,8,numtplanes,TInfoNon,TInfoNoff,0))>=0)
	 {
	  int reply;

	  RestoreBackground();
	  reply=DoDialog(&AreSureD2);
	  switch(reply)
	  {
	   case 0:
	   case 2:
	     goto redo;
	  }

	  if (which!=numtplanes-1)
	    {
	     for (i=0;i<8;i++)
	       {
		MapFileHeader->tnames[which][i]=MapFileHeader->tnames[numtplanes-1][i];
		MapFileHeader->tnames[numtplanes-1][i]=0;
	       }
	     for (i=0;i<tilenum;i++)
	       *(Tinfo[which]+i)=*(Tinfo[numtplanes-1]+i);
	     MMFreePtr((memptr *)&Tinfo[numtplanes-1]);
	    }
	  else
	    {
	     MMFreePtr((memptr *)&Tinfo[which]);
	     for (i=0;i<8;i++)
	       MapFileHeader->tnames[which][i]=0;
	    }
	  writeH=DirtyFlag=1;
	  MapFileHeader->numtplanes=--numtplanes;
	  goto redo;
	 }

       if ((which=CheckList(ox+12,oy+3,8,numtmplanes,TInfoMNon,TInfoMNoff,0))>=0)
	 {
	  int reply;

	  RestoreBackground();
	  reply=DoDialog(&AreSureD2);
	  switch(reply)
	  {
	   case 0:
	   case 2:
	     goto redo;
	  }

	  if (which!=numtmplanes-1)
	    {
	     for (i=0;i<8;i++)
	       {
		MapFileHeader->tmnames[which][i]=MapFileHeader->tmnames[numtplanes-1][i];
		MapFileHeader->tmnames[numtmplanes-1][i]=0;
	       }
	     for (i=0;i<tilemnum;i++)
	       *(TMinfo[which]+i)=*(TMinfo[numtmplanes-1]+i);
	     MMFreePtr((memptr *)&TMinfo[numtmplanes-1]);
	    }
	  else
	    {
	     MMFreePtr((memptr *)&TMinfo[which]);
	     for (i=0;i<8;i++)
	       MapFileHeader->tmnames[which][i]=0;
	    }
	  writeH=DirtyFlag=1;
	  MapFileHeader->numtmplanes=--numtmplanes;
	  goto redo;
	 }

       GetButtonXY(&TINd2,0,&sx,&sy);
       if (!CheckList(sx,sy,6,1,TIDoneOn,TIDoneOff,1))
	 oktoexit++;

       if (keydown[1])
	 {
	  while(keydown[1]);
	  oktoexit++;
	 }
      }while(!oktoexit);

      RestoreBackground();
     }
  }
 }
}



////////////////////////////////////////////////////
//
// Item - Project Re-Select
//
////////////////////////////////////////////////////
btype NGBb[]={{" Do It! ",1,4,2},
	      {" No! Help! ",16,4,1}};
DialogDef NGBd={"Are you sure you want to\n"
		"switch projects? Abort now\n"
		"or forever hold your peace!",
		28,6,2,&NGBb[0],NULL};

void Item_ProjectReSelect(void)
{
 int i,which;

 //
 // ARE YOU SURE?!
 //
 which=DoDialog(&NGBd);
 if (!which || which==2)
   return;


 TEDInfo->lastvid=videomode;
 TEDInfo->level=whichmap;
 SaveTEDInfo();
 SaveOutputHeader();

 if (!CheckForMapSave())
   return;

 //
 // RELEASE ALL MEMORY
 //         ---
 if (MapBkgnd)
   {
    MMFreePtr((memptr *)&MapBkgnd);
    MMFreePtr((memptr *)&CutBkgnd);
   }
 if (MapFrgnd)
   {
    MMFreePtr((memptr *)&MapFrgnd);
    MMFreePtr((memptr *)&CutFrgnd);
   }
 if (MapInfoPl)
   {
    MMFreePtr((memptr *)&MapInfoPl);
    MMFreePtr((memptr *)&CutInfoPl);
   }

 MMFreePtr((memptr *)&TEDInfo);
 MMFreePtr((memptr *)&MapFileHeader);

 if (CgaXMS)
   {
    MMFreePtr((memptr *)&CgaXMSlookup);
    XMSFreeMem(CgaXMS);
   }
 if (EgaXMS)
   {
    MMFreePtr((memptr *)&EgaXMSlookup);
    XMSFreeMem(EgaXMS);
   }
 if (VgaXMS)
   {
    MMFreePtr((memptr *)&VgaXMSlookup);
    XMSFreeMem(VgaXMS);
   }
 if (XMSmaps)
   XMSFreeMem(XMSmaps);

 XMSmaps=CgaXMS=EgaXMS=VgaXMS=xmshandle=0;


 for (i=0;i<numtplanes;i++)
   if (Tinfo[i])
     MMFreePtr((memptr *)&Tinfo[i]);
 for (i=0;i<numtmplanes;i++)
   if (TMinfo[i])
     MMFreePtr((memptr *)&TMinfo[i]);

 //
 // FORCE RE-INIT
 //
 writeH=TIxbase=TIxmbase=TIybase=TIymbase=PasteOK=DirtyFlag=ext[0]=0;
 lastmap=-1;

 FindGraphFile();
 LoadInfoFile();
 LoadMapHeader();
 LoadGraphStuff(0,videomode);
 MouseHide();
 RedrawDesktop();
 MouseShow();
 DrawInfoBar();
 DrawMap();
}


////////////////////////////////////////////////////
//
// Item - Toggle GRIDMODE on/off
//
////////////////////////////////////////////////////
void Item_GridMode(void)
{
 GridMode^=1;

 if (PasteMode)
   DrawFloatPaste();

 DrawMap();
 DrawInfoBar();
}


////////////////////////////////////////////////////
//
// Item - Toggle SNAP-PASTE on/off
//
////////////////////////////////////////////////////
void Item_SnapTog(void)
{
 if (!PasteMode)
   return;

 EraseFloatPaste();
 SnapMode^=1;

 snapx=(pixelx>>(tsize+2))+xbase;
 snapy=((pixely-8)>>(tsize+2))+ybase;
 snapxsize=TileCopy.w;
 snapysize=TileCopy.h;

 snapx=snapx-(snapx/snapxsize)*snapxsize;
 snapy=snapy-(snapy/snapysize)*snapysize;

 DrawInfoBar();
 DrawFloatPaste();
}


////////////////////////////////////////////////////
//
// Item - View Map & Goto
//
////////////////////////////////////////////////////
void Item_ViewMap(void)
{
 Do_ViewMap(0);
}


////////////////////////////////////////////////////
//
// Item - Review Map & Goto
//
////////////////////////////////////////////////////
void Item_ReviewMap(void)
{
 Do_ViewMap(1);
}


////////////////////////////////////////////////////
//
// Code to actually do the ViewMap (and save it in
// EGA memory when finished).
// Entry:
// 0 = ViewMap, as normal & save when done (but only
//	if a full map was viewed)
// 1 = RestoreMap for GOTO
//
////////////////////////////////////////////////////
void Do_ViewMap(int how)
{
 int _seg *a_bg,_seg *a_fg,_seg *a_in,CopyArea,bl_width,bl_height,bl_x,bl_y,p_info;

 char huge *EGA=MK_FP(0xa000,0);
 char _seg *block,_seg *gblock[4];
 int i,j,k,m,n,pwidth,lwidth,maptype,step,pixnum[4]={0,8,16,32},curpix,
	  maxpack,curline,lybase,t8=8<<(tsize-1),t1=1<<(tsize-1),scrn_h,scrn_w;
 long tilelen,bufsize;
 int savevideo;



 savevideo=videomode;
 MouseHide();
 setvideo(EGA1);
 MouseShow();
 if (videomode!=EGA1 && videomode!=EGA2)
 {
  ErrDialog("This function is currently\n"
		 "only usable in EGA mode!"," OK ");
  return;
 }

 //
 // SCREEN DIMENSIONS
 //
 scrn_w=320;
 scrn_h=200;


 bl_x=bl_y=0;
 bl_width=mapwidth;
 bl_height=mapheight;
 a_bg=MapBkgnd;
 a_fg=MapFrgnd;
 a_in=MapInfoPl;
 p_info=MapFileHeader->maptype;
 CopyArea=0;

 if ((TileCopy.w>screenw || TileCopy.h>screenh) && !how)
	if (Message("\"Yes\" to display full map,\n"
			 "\"No\" to display COPY buffer.")==1)
	{
	 bl_x=TileCopy.x;
	 bl_y=TileCopy.y;
	 bl_width=TileCopy.w;
	 bl_height=TileCopy.h;
	 a_bg=CutBkgnd;
	 a_fg=CutFrgnd;
	 a_in=CutInfoPl;
	 p_info=TileCopy.PlanesCopied;
	 CopyArea=1;
	}

 //
 // VALIDATE WIDTH & HEIGHT
 //
 if (bl_height<screenh && bl_width<screenw)
 {
  ErrDialog("The area fits within the\n"
		 "screen! Forget it!"," Wah! ");
  return;
 }


 //
 // FIGURE OUT THE BEST RATIO FOR CONVERSION
 //
 if (!how)
	switch(bl_width>=bl_height)
	{
	 case 1: // WIDTH > HEIGHT
		step=(float)(pixnum[tsize]*bl_width)/scrn_w+.5;
		if (pixnum[tsize]*bl_width/step>scrn_w)
	step++;
		if ((float)(pixnum[tsize]*bl_height)/step+.5<scrn_h)
	break;

	 case 0: // HEIGHT > WIDTH
		step=(float)(pixnum[tsize]*bl_height)/scrn_h+.5;
		if (pixnum[tsize]*bl_height/step>scrn_h)
	step++;
	}
 else
	step=VMapData.step;


 //
 // POP LAST MAP ON SCREEN
 //
 if (how)
 {
  unsigned EGAseg=VMapData.EGAseg;

  if (!VMapData.built_flag)
  {
	ErrDialog("You haven't previously\n"
		  "VIEWed a map!"," OK ");
	return;
  }

  //
  // RESTORE MAP IN MEMORY!
  //
  MouseHide();
  outport(GCindex,GCmode | 0x100);
  outport(SCindex,SCmapmask | 0xf00);
  asm	push	ds
  asm	mov	ax,[EGAseg]
  asm	mov	ds,ax
  asm	mov	ax,0xa000
  asm	mov	es,ax
  asm	xor	si,si
  asm	xor	di,di
  asm	mov	cx,0x2000
  asm	rep movsb
  asm	pop	ds
  step=VMapData.step;
  maxpack=VMapData.maxpack;
  MouseShow();
 }
 //
 // BUILD MAP
 //
 else
 {
  //
  // CLEAR EGA SCREEN
  //
  outport(GCindex,GCmode);
  outport(SCindex,SCmapmask | 0xf00);
  _fmemset(MK_FP(0xa000,0),0,0x2000);

  //
  // SET TILE LENGTH
  //
  switch(tsize)
  {
	case 1: tilelen=32L; break;
	case 2: tilelen=128L; break;
	case 3: tilelen=512L;
  }

  //
  // ALLOCATE TEMP BUFFERS & BEGIN!
  //
  bufsize=tilelen*bl_width;
  pwidth=t1*bl_width;
  lwidth=pwidth*4;
  MMAllocate((memptr *)&block,bufsize);
  maxpack=pwidth/step+1;
  for (i=0;i<4;i++)
	 MMAllocate((memptr *)&gblock[i],maxpack);

  outport(GCindex,GCmode);
  curline=0;
  for (j=bl_y;j<bl_y+bl_height;j++)
  {
	lybase=j*t8;
	for (i=bl_x;i<bl_x+bl_width;i++)
	{
	 unsigned tilet,tilem,tilei,loc;

	 //
	 // ESC out?
	 //
	 if (keydown[1])
	 {
	  while(keydown[1]);
	  setvideo(savevideo);
	  RedrawDesktop();
	  DrawMap();
	  DrawInfoBar();
	  return;
	 }
	 //
	 // GET THE CORRECT TILE MOVED INTO "TDATA"
	 //
	 loc=j*mapwidth+i;
	 tilet=viewton?((p_info&BPLANE)?a_bg[loc]:-BkgndColor):-BkgndColor;
	 tilem=viewmon?((p_info&FPLANE)?a_fg[loc]+tilenum:0):0;
	 tilei=viewion?((p_info&IPLANE)?a_in[loc]+tilenum:0):0;
	 CombineTiles(tilet,tilem,tilei,tsize);

	 //
	 // NOW, MUNGE "TDATA" INTO BIT-PLANES!
	 //
	 // INTERNAL:		P0	P1	P2	P3
	 //	SCANLINE x	......	......	......	......
	 //
	 for (k=0;k<t8;k++)
		if (!((lybase+k)%step))
	for (m=0;m<4;m++)
	  movedata(FP_SEG(&tdata),FP_OFF(&tdata)+(m*tsize*t8)+k*t1,
			 (unsigned)block,k*lwidth+m*pwidth+(i-bl_x)*t1,t1);
	}

	//
	// CONVERT BIT-PLANE LINES TO COMPRESSED FORM...
	//
	for (k=0;k<8<<(tsize-1);k++)
	{
	 if (!((lybase+k)%step))
	 {
	  int midx=k*lwidth;


	  curpix=0;
	  _fmemset(gblock[0],0,maxpack);
	  _fmemset(gblock[1],0,maxpack);
	  _fmemset(gblock[2],0,maxpack);
	  _fmemset(gblock[3],0,maxpack);
	  for (m=0;m<pwidth*8;m++)
	  {
		if (!(m%step))
		{
		 int temp=curpix/8,
		temp1=7-(curpix%8),
		temp2=7-(m%8),
		idx;


		 idx=midx+m/8;

		 #if 1
		 asm	push	ds

		 asm	mov	ds,[block]
		 asm	mov	si,[idx]
		 asm	mov	cl,[BYTE PTR temp2]
		 asm	mov	ch,[BYTE PTR temp1]
		 asm	mov	di,[temp]
		 asm	mov	dx,[pwidth]
		 asm	dec	dx

		 asm	mov	es,[WORD PTR gblock]

		 asm	lodsb
		 asm	shr	al,cl
		 asm	and	al,1
		 asm	xchg	ch,cl
		 asm	shl	al,cl
		 asm	xchg	ch,cl
		 asm	or	[es:di],al

		 asm	add	si,dx

		 asm	mov	es,[WORD PTR gblock+2]

		 asm	lodsb
		 asm	shr	al,cl
		 asm	and	al,1
		 asm	xchg	ch,cl
		 asm	shl	al,cl
		 asm	xchg	ch,cl
		 asm	or	[es:di],al

		 asm	add	si,dx

		 asm	mov	es,[WORD PTR gblock+4]

		 asm	lodsb
		 asm	shr	al,cl
		 asm	and	al,1
		 asm	xchg	ch,cl
		 asm	shl	al,cl
		 asm	xchg	ch,cl
		 asm	or	[es:di],al

		 asm	add	si,dx

		 asm	mov	es,[WORD PTR gblock+6]

		 asm	lodsb
		 asm	shr	al,cl
		 asm	and	al,1
		 asm	xchg	ch,cl
		 asm	shl	al,cl
		 asm	xchg	ch,cl
		 asm	or	[es:di],al

		 asm	add	si,dx

		 asm	pop	ds

		 #else
		 *(gblock[0]+temp)|=((block[idx]>>temp2)&1)<<temp1;
		 *(gblock[1]+temp)|=((block[idx+pwidth]>>temp2)&1)<<temp1;
		 *(gblock[2]+temp)|=((block[idx+pwidth*2]>>temp2)&1)<<temp1;
		 *(gblock[3]+temp)|=((block[idx+pwidth*3]>>temp2)&1)<<temp1;
		 #endif

		 curpix++;
		}
	  }

	  //
	  // BLAST ONTO THE SCREEN
	  //

	  MouseHide();
	  for (n=0;n<4;n++)
	  {
		outport(SCindex,SCmapmask | (0x100<<n));
		for (m=0;m<maxpack;m++)
	*(EGA+EGA1lookup[curline]+m)=*(gblock[n]+m);
     }
     MouseShow();

     curline++;
    }
   }
  }

  MMFreePtr((memptr *)&block);
  for(m=0;m<4;m++)
    MMFreePtr((memptr *)&gblock[m]);


  //
  // SAVE MAP IN MEMORY!
  //
  if (!CopyArea)
  {
   unsigned EGAseg=0xa800;


   MouseHide();
   outport(GCindex,GCmode | 0x100);
   outport(SCindex,SCmapmask | 0xf00);
   asm	push	ds
   asm	mov	ax,[EGAseg]
   asm	mov	es,ax
   asm	mov	ax,0xa000
   asm	mov	ds,ax
   asm	xor	si,si
   asm	xor	di,di
   asm	mov	cx,0x2000
   asm	rep movsb
   asm	pop	ds
   VMapData.step=step;
   VMapData.built_flag=1;
   VMapData.EGAseg=EGAseg;
   VMapData.maxpack=maxpack;
   MouseShow();
  }
 }


 //
 // DONE WITH MAP BUILD-N-DISPLAY
 //
 SignalSound();
 clearkeys();

 //
 // IF MOUSE BUTTON PRESSED WITHIN REGION, GO THERE!
 //
 while(!bioskey(1))
   if (MouseButton())
   {
    int x,y;

    MouseCoords(&x,&y);
    if (x<maxpack*8 && y<(mapheight*t8/step) && !CopyArea)
    {
     xbase=(x*step)/t8-screenw/2;
     ybase=(y*step)/t8-screenh/2;
     if (xbase+screenw>mapwidth)
       xbase=mapwidth-screenw;
     if (ybase+screenh>mapheight)
       ybase=mapheight-screenh;
     if (xbase<0)
       xbase=0;
     if (ybase<0)
       ybase=0;
    }
    while(MouseButton());
    break;
   }

 clearkeys();
 MouseHide();
 setvideo(savevideo);
 RedrawDesktop();
 DrawMap();
 DrawInfoBar();
 MouseShow();
}


////////////////////////////////////////////////////
//
// MAP IMPORTING FUNCTIONS FOLLOW:
//
////////////////////////////////////////////////////
char _seg *oldnames,oldmapname[64],oldmapheadname[64],
	oldSM_name[64],oldSM_loadname[64];

int IM_swapin(void)
{
 int i;


 _fstrcpy(mapheadname,TEDInfo->ImportPath);
 strcat(mapheadname,oldmapheadname);
 if (access(mapheadname,0))
   {
    strcpy(mapheadname,oldmapheadname);
    return 0;
   }

 _fstrcpy(mapname,TEDInfo->ImportPath);
 strcat(mapname,oldmapname);


 _fstrcpy(SM_name,TEDInfo->ImportPath);
 strcat(SM_name,oldSM_name);

 _fstrcpy(SM_loadname,TEDInfo->ImportPath);
 strcat(SM_loadname,oldSM_loadname);

 MMAllocate((memptr *)&oldnames,100*16);
 movedata(FP_SEG(MapNames),FP_OFF(MapNames),(unsigned)oldnames,0,100*16);

 MMFreePtr((memptr *)&MapFileHeader);

 LoadIn(mapheadname,(memptr *)&MapFileHeader);

 for (i=0;i<100;i++)
   if (MapFileHeader->dataoffsets[i]>=0)
   {
    MapHeaderStr TempHead;

    LoadFile(mapname,(char huge *)&TempHead,MapFileHeader->dataoffsets[i],sizeof(MapHeaderStr));
    strcpy(MapNames[i],TempHead.name);
   }
 return 1;
}


void IM_swapout(void)
{
 if (oldnames)		// GET RID OF MAPNAMES
 {
  strcpy(mapname,oldmapname);
  strcpy(mapheadname,oldmapheadname);
  strcpy(SM_name,oldSM_name);
  strcpy(SM_loadname,oldSM_loadname);

  movedata((unsigned)oldnames,0,FP_SEG(MapNames),FP_OFF(MapNames),100*16);
  MMFreePtr((memptr *)&oldnames);
  MMFreePtr((memptr *)&MapFileHeader);
  LoadIn(mapheadname,(memptr *)&MapFileHeader);

 }
}

void IM_LoadMap(void)
{
 unsigned long csize,size=0;
 memptr block;

 //
 // DEALLOCATE ALL CURRENT MAP MEMORY
 //
 if (MapBkgnd)
   {
    MMFreePtr((memptr *)&MapBkgnd);
    MMFreePtr((memptr *)&CutBkgnd);
   }
 if (MapFrgnd)
   {
    MMFreePtr((memptr *)&MapFrgnd);
    MMFreePtr((memptr *)&CutFrgnd);
   }
 if (MapInfoPl)
   {
    MMFreePtr((memptr *)&MapInfoPl);
    MMFreePtr((memptr *)&CutInfoPl);
   }

 //
 // LOAD MAP HEADER
 //
 LoadFile(mapname,(char huge *)&MapHeader,MapFileHeader->dataoffsets[whichmap],sizeof(MapHeaderStr));

 //
 // LOAD & DECOMPRESS MAP PLANES
 //
 if (MapFileHeader->maptype & BPLANE)
   {
    LoadFile(mapname,(char huge *)&size,MapHeader.mapbkgndpl,2);

    MMAllocate((memptr *)&MapBkgnd,size);
    MMAllocate((memptr *)&CutBkgnd,size);
    csize=MapHeader.mapbkgndlen-2;
    MMAllocate(&block,csize);

    LoadFile(mapname,MK_FP(block,0),MapHeader.mapbkgndpl+2,csize);

    RLEWExpand(MK_FP(block,0),MK_FP(MapBkgnd,0),size,MapFileHeader->RLEWtag);
    MMFreePtr(&block);
   }
 if (MapFileHeader->maptype & FPLANE)
   {
    LoadFile(mapname,(char huge *)&size,MapHeader.mapfrgndpl,2);

    MMAllocate((memptr *)&MapFrgnd,size);
    MMAllocate((memptr *)&CutFrgnd,size);
    csize=MapHeader.mapfrgndlen-2;
    MMAllocate(&block,csize);

    LoadFile(mapname,MK_FP(block,0),MapHeader.mapfrgndpl+2,csize);

    RLEWExpand(MK_FP(block,0),MK_FP(MapFrgnd,0),size,MapFileHeader->RLEWtag);
    MMFreePtr(&block);
   }
 if (MapFileHeader->maptype & IPLANE)
   {
    LoadFile(mapname,(char huge *)&size,MapHeader.mapinfopl,2);

    MMAllocate((memptr *)&MapInfoPl,size);
    MMAllocate((memptr *)&CutInfoPl,size);
    csize=MapHeader.mapinfolen-2;
    MMAllocate(&block,csize);

    LoadFile(mapname,MK_FP(block,0),MapHeader.mapinfopl+2,csize);

    RLEWExpand(MK_FP(block,0),MK_FP(MapInfoPl,0),size,MapFileHeader->RLEWtag);
    MMFreePtr(&block);
   }
}


void IM_SaveMap(void)
{
 memptr block;
 long fsize,size,nsize,change;
 MapHeaderStr TempHeader;
 int i,XMStemp;
 char string[100],TEDid[]=IDSTRING;



 strcpy(string,"Saving Map, '");
 strcat(string,MapHeader.name);

 strcat(string,"'.");
 ErrDialog(string,"");

 //
 // SAVE MAP FILE HEADER
 //
 SaveFile(mapheadname,MK_FP(MapFileHeader,0),0,sizeof(MapFileHeaderStr));
 fsize=sizeof(MapFileHeaderStr);

 //
 // COMPRESS & SAVE TILEINFOS
 //
 for (i=0;i<numtplanes;i++)
   {
    MMAllocate(&block,tilenum);
    MapFileHeader->tileinfooff[i]=fsize;
    nsize=RLEBCompress(MK_FP(Tinfo[i],0),tilenum,MK_FP(block,0),MapFileHeader->RLEWtag);
    MapFileHeader->tileinfolen[i]=nsize;
    SaveFile(mapheadname,MK_FP(block,0),fsize,nsize);
    fsize+=nsize;
    MMFreePtr(&block);
   }

 for (i=0;i<numtmplanes;i++)
   {
    MMAllocate(&block,tilemnum);
    MapFileHeader->tileinfomoff[i]=fsize;
    nsize=RLEBCompress(MK_FP(TMinfo[i],0),tilemnum,MK_FP(block,0),MapFileHeader->RLEWtag);
    MapFileHeader->tileinfomlen[i]=nsize;
    SaveFile(mapheadname,MK_FP(block,0),fsize,nsize);
    fsize+=nsize;
    MMFreePtr(&block);
   }

 MapFileHeader->oldtilenum=tilenum;
 MapFileHeader->oldtilemnum=tilemnum;

 SaveFile(mapheadname,MK_FP(MapFileHeader,2),2,sizeof(MapFileHeaderStr)-2);
 //
 // SAVE ALREADY COMPRESSED MAPS
 //

 //
 // NOTE: I AM STORING "TED5" AT THE START OF THE FILE BECAUSE
 // SAVING THE FILE AT OFFSET 0 WILL TRASH IT (I HAVE TO RE-SAVE THE HEADER!)
 //
 SaveFile(SM_name,(char huge *)TEDid,0,strlen(TEDid));
 fsize=strlen(TEDid);

 for (i=0;i<100;i++)
 {
  long oldoff;

  if (MapFileHeader->dataoffsets[i]==-1 || i==whichmap)
    continue;

  oldoff=MapFileHeader->dataoffsets[i];

  LoadFile(SM_loadname,(char huge *)&TempHeader,oldoff,sizeof(MapHeaderStr));

  strcpy(TempHeader.name,MapNames[i]);
  MapFileHeader->dataoffsets[i]=fsize;
  size=TempHeader.mapbkgndlen+TempHeader.mapfrgndlen+TempHeader.mapinfolen;
  change=TempHeader.mapbkgndpl-fsize-sizeof(MapHeaderStr);
  TempHeader.mapbkgndpl-=change;
  TempHeader.mapfrgndpl-=change;
  TempHeader.mapinfopl-=change;

  SaveFile(SM_name,(char huge *)&TempHeader,fsize,sizeof(MapHeaderStr));
  fsize+=sizeof(MapHeaderStr);

  MMAllocate(&block,size);
  LoadFile(SM_loadname,MK_FP(block,0),oldoff+sizeof(MapHeaderStr),size);
  SaveFile(SM_name,MK_FP(block,0),fsize,size);
  fsize+=size;
  SaveFile(SM_name,"!ID!",fsize,4);
  fsize+=4;
  MMFreePtr(&block);
 }

 //
 // SAVE CURRENT MAP AT END OF FILE
 //
 MapFileHeader->dataoffsets[whichmap]=fsize;
 MapFileHeader->datalengths[whichmap]=sizeof(MapHeaderStr);
 SaveFile(SM_name,(char huge *)&MapHeader,fsize,sizeof(MapHeaderStr));
 fsize+=sizeof(MapHeaderStr);

 size=MapHeader.width*MapHeader.height*sizeof(int);
 MMAllocate(&block,size);
 if (MapFileHeader->maptype & BPLANE)
   {
    MapHeader.mapbkgndpl=fsize;
    nsize=RLEWCompress(MK_FP(MapBkgnd,0),size,MK_FP(block,0),MapFileHeader->RLEWtag);
    MapHeader.mapbkgndlen=nsize+2;

    SaveFile(SM_name,(char huge *)&size,fsize,2);
    fsize+=2;
    SaveFile(SM_name,MK_FP(block,0),fsize,nsize);
    fsize+=nsize;
   }
 else
   MapHeader.mapbkgndlen=0;

 if (MapFileHeader->maptype & FPLANE)
   {
    MapHeader.mapfrgndpl=fsize;
    nsize=RLEWCompress(MK_FP(MapFrgnd,0),size,MK_FP(block,0),MapFileHeader->RLEWtag);
    MapHeader.mapfrgndlen=nsize+2;

    SaveFile(SM_name,(char huge *)&size,fsize,2);
    fsize+=2;
    SaveFile(SM_name,MK_FP(block,0),fsize,nsize);
    fsize+=nsize;
   }
 else
   MapHeader.mapfrgndlen=0;

 if (MapFileHeader->maptype & IPLANE)
   {
    MapHeader.mapinfopl=fsize;
    nsize=RLEWCompress(MK_FP(MapInfoPl,0),size,MK_FP(block,0),MapFileHeader->RLEWtag);
    MapHeader.mapinfolen=nsize+2;

    SaveFile(SM_name,(char huge *)&size,fsize,2);
    fsize+=2;
    SaveFile(SM_name,MK_FP(block,0),fsize,nsize);
    fsize+=nsize;
   }
 else
   MapHeader.mapinfolen=0;

 SaveFile(SM_name,"!ID!",fsize,4);

 fsize+=4;

 MMFreePtr(&block);

 // RE-SAVE HEADER
 SaveFile(SM_name,(char huge *)&MapHeader,
     MapFileHeader->dataoffsets[whichmap],sizeof(MapHeaderStr));

 //
 // RE-SAVE FILE HEADER
 // NOTE: The "2" is so MSDOS doesn't truncate the fucking file!
 //
 SaveFile(mapheadname,MK_FP(MapFileHeader,2),2,sizeof(MapFileHeaderStr)-2);

 unlink(SM_loadname);
 rename(SM_name,SM_loadname);

 RestoreBackground();
}


////////////////////////////////////////////////////
//
// Item - Import Maps
//
////////////////////////////////////////////////////
btype IMPMb[]={{"New Path",3,2,1},
	       {" Import ",3,5,1},
	       {"  Exit  ",3,8,2}};
DialogDef IMPMd={"Map Importing",13,11,3,&IMPMb[0],NULL};
btype NPb={"                                    ",1,3,1};
DialogDef NPd={"Current Path:",38,5,1,&NPb,NULL};


void Item_ImportMaps(void)
{
 char imfile[64],tempstr[40],impath[64];
 int oldwhichmap,which,mapnum,i;
 int oldxb,oldyb;


 CheckForMapSave();

 //
 // THE IMPORT FUNCTION WILL RID THE SYSTEM OF XMSMAPS
 //
 if (XMSmaps)
 {
  XMSFreeMem(XMSmaps);
  XMSmaps=0;
 }

 //
 // SAVE PATHS
 //
 _fstrcpy(impath,TEDInfo->ImportPath);

 strcpy(oldmapname,mapname);
 strcpy(oldmapheadname,mapheadname);
 strcpy(oldSM_name,SM_name);
 strcpy(oldSM_loadname,SM_loadname);

 oldxb=xbase;
 oldyb=ybase;
 #pragma warn -sus
 oldnames=0;
 #pragma warn +sus
 oldwhichmap=whichmap;

 DrawDialog(&IMPMd,1);
 while(1)
 {
  which=CheckButtons(&IMPMd);
  switch(which)
  {
   //
   // NEW PATH
   //
   case 1:
     MouseHide();
     DrawDialog(&NPd,1);
     GetDialogXY(&NPd,&sx,&sy);
     sy++;
     sx++;
     if (!impath[0])
       print("- current dir -");
     else
       print(impath);
     GetButtonXY(&NPd,0,&sx,&sy);
     if (input(tempstr,36))
       {
	strcpy(impath,tempstr);
	if (impath[strlen(impath)-1]!='\\')
	  strcat(impath,"\\");

	MouseShow();
	if (access(mapheadname,0))
	{
	 ErrDialog("Can't find any TED5\n"
		   "map files at that path."," OK ");
	}
	else
	{
	 RestoreBackground();
	 ErrDialog("Verifying path...","");
	 _fstrcpy(TEDInfo->ImportPath,impath);
	 DirtyFlag=1;
	}

	MouseHide();
       }
     MouseShow();
     RestoreBackground();
     break;

   //
   // IMPORT
   //
   case 2:
     if (!oldnames)
     {
      ErrDialog("Loading File Info...","");
      if (!IM_swapin())
      {
       RestoreBackground();
       ErrDialog("Having problems with your path!"," OK ");
       break;
      }
      RestoreBackground();
     }

     mapnum=SelectMap(1,CREATED,"TO IMPORT");
     if (mapnum>=0)
     {
      char check[100]="Are you SURE you want to\n"
		      "Import ";

      strcat(check,MapNames[mapnum]);
      strcat(check,"?");
      if (Message(check)<2)
	break;

      whichmap=mapnum;
      ErrDialog("Importing. One moment.","");
      IM_LoadMap();
      IM_swapout();
      IM_SaveMap();
      IM_swapin();
      RestoreBackground();
     }
     break;

   //
   // EXIT
   //
   case 0:
   case 3:
     RestoreBackground();
     whichmap=oldwhichmap;
     IM_swapout();

     LoadMap(whichmap);
     xbase=oldxb;
     ybase=oldyb;
     DrawMap();

     return;
  }
 }
}


////////////////////////////////////////////////////
//
// Item - Visit DOS
//
////////////////////////////////////////////////////
int olddisk;
char oldpath[64]="\\";

void Item_VisitDOS(void)
{
 TempStruct LaunchInfo;
 char ename[64],temp[40],tiname[14]="TEDINFO.TMP";

 long size;


 //
 // Save the handles for all XMS memory so we don't
 // have to re-install this shit!
 //
 TEDInfo->OldCgaXMS=CgaXMS;
 TEDInfo->OldEgaXMS=EgaXMS;
 TEDInfo->OldVgaXMS=VgaXMS;

 TEDInfo->OldCgaXMSsize=CgaXMSsize;
 TEDInfo->OldEgaXMSsize=EgaXMSsize;
 TEDInfo->OldVgaXMSsize=VgaXMSsize;

 size=4L*(tilenum+tilemnum);
 if (CgaXMS)
   {
    if (1024L*XMSTotalFree()<size)
      {
       XMSFreeMem(CgaXMS);
       TEDInfo->OldCgaXMS=TEDInfo->OldCgaXMSsize=0;
      }
    else
      {
       TEDInfo->CgaXMSlook=XMSAllocate(size);
       XMSmove(0,(long)MK_FP(CgaXMSlookup,0),TEDInfo->CgaXMSlook,0,size);
      }
   }

 if (EgaXMS)
   {
    if (1024L*XMSTotalFree()<size)
      {
       XMSFreeMem(EgaXMS);
       TEDInfo->OldEgaXMS=TEDInfo->OldEgaXMSsize=0;
      }
    else
      {
       TEDInfo->EgaXMSlook=XMSAllocate(size);
       XMSmove(0,(long)MK_FP(EgaXMSlookup,0),TEDInfo->EgaXMSlook,0,size);
      }
   }

 if (VgaXMS)
   {
    if (1024L*XMSTotalFree()<size)
      {
       XMSFreeMem(VgaXMS);
       TEDInfo->OldVgaXMS=TEDInfo->OldVgaXMSsize=0;
      }
    else
      {
       TEDInfo->VgaXMSlook=XMSAllocate(size);
       XMSmove(0,(long)MK_FP(VgaXMSlookup,0),TEDInfo->VgaXMSlook,0,size);
      }
   }

 //
 // SAVE CURRENT VIDEOMODE FOR LAUNCH RETURN
 //
 LaunchInfo.lastmode=videomode;
 strcpy(LaunchInfo.ext,ext);
 SaveFile(tiname,(char huge *)&LaunchInfo,0,sizeof(TempStruct));

 TEDInfo->oscrx=xbase;
 TEDInfo->oscry=ybase;
 _fmemcpy((void far *)TEDInfo->parmstring,(void far *)parmstring,64);

 Item_SaveMap();
 SaveTEDInfo();
 SaveOutputHeader();

 if (XMSmaps)
   XMSFreeMem(XMSmaps);

 strcpy(temp,"Launching ");
 _fstrcat((char far *)temp,(char far *)TEDInfo->launchname);
 strcat(temp,"...");
 ErrDialog(temp,"");
 clearkeys();
 nosound();
 MouseHide();

 _fmemcpy((char far *)ename,(char far *)TEDInfo->launchname,14);

 RemoveUndoBuffers();
 ShutdownKBD();

 //
 // ARE WE EXITING WITH A BATCH FILE?
 //

 setvideo(TEXT);
 printf("The TED5 DOS shell. Type 'EXIT' to return to TED5.");

 //
 // SAVE CURRENT DRIVE & PATH
 //
 olddisk=getdisk();
 getcurdir(0,oldpath+1);

 MMShutdown();
 fcloseall();
 spawnlp(P_WAIT,"COMMAND",NULL);

 //
 // RESET OLD PATH
 //
 setdisk(olddisk);
 chdir(oldpath);

 execlp("TED5.EXE","TED5.EXE","/LAUNCH",NULL);

 printf("Can't find TED5 for some reason!");
 exit(1);
}


////////////////////////////////////////////////////
//
// Item - Paste Overlay toggle
//
////////////////////////////////////////////////////
void Item_POtog(void)
{
 F3_flag^=1;

 if (PasteMode)
 {
  EraseFloatPaste();
  DrawFloatPaste();
 }
}
