////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
// TED5-1
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////
#include "ted5.h"
#pragma hdrstop

void InputMap(char *lvlname,unsigned *levelw,unsigned *levelh,int exitok);
void MNameOn(int x,int y,int i);
void MNameOff(int x,int y,int i);
void DnarOn(int x,int y);
void DnarOff(int x,int y);
void UparOn(int x,int y);
void UparOff(int x,int y);
void ExitOn(int x,int y);
void ExitOff(int x,int y);
void TInfoNon(int x,int y,int b);
void TInfoNoff(int x,int y,int b);
void TInfoMNon(int x,int y,int b);
void TInfoMNoff(int x,int y,int b);
void TIDoneOn(int x,int y);
void TIDoneOff(int x,int y);


////////////////////////////////////////////////////
//
// SAVE the current map
//
////////////////////////////////////////////////////
void SaveMap(int delmap)
{
 memptr block;
 long fsize,size,nsize,change;
 MapHeaderStr TempHeader;
 int i,XMStemp;
 char string[100],TEDid[]=IDSTRING;



 RemoveUndoBuffers();
 if (delmap)
   {
	LoadFile(mapname,(char huge *)&TempHeader,
	MapFileHeader->dataoffsets[whichmap],sizeof(MapHeaderStr));
	strcpy(string,"Deleting Map, '");
	strcat(string,TempHeader.name);
   }
 else
   {
	strcpy(string,"Saving Map, '");
	strcat(string,MapHeader.name);
   }

 strcat(string,"'.");
 ErrDialog(string,"");


 BackupFile(mapheadname);
 BackupFile(SM_loadname);


 if (delmap || DirtyFlag)
   {
	//
	// SAVE MAP FILE HEADER
	//
	if (delmap)
	  MapFileHeader->dataoffsets[whichmap]=-1;

	SaveFile(mapheadname,MK_FP(MapFileHeader,0),0,sizeof(MapFileHeaderStr));
	fsize=sizeof(MapFileHeaderStr);

	//
	// COMPRESS & SAVE TILEINFOS
	//
	MMAllocate(&block,tilenum);
	for (i=0;i<numtplanes;i++)
	  {
	   MapFileHeader->tileinfooff[i]=fsize;
	   nsize=RLEBCompress(MK_FP(Tinfo[i],0),tilenum,MK_FP(block,0),MapFileHeader->RLEWtag);
	   MapFileHeader->tileinfolen[i]=nsize;
	   SaveFile(mapheadname,MK_FP(block,0),fsize,nsize);
	   fsize+=nsize;
	  }
	MMFreePtr(&block);

	MMAllocate(&block,tilemnum);
	for (i=0;i<numtmplanes;i++)
	  {
	   MapFileHeader->tileinfomoff[i]=fsize;
	   nsize=RLEBCompress(MK_FP(TMinfo[i],0),tilemnum,MK_FP(block,0),MapFileHeader->RLEWtag);
	   MapFileHeader->tileinfomlen[i]=nsize;
	   SaveFile(mapheadname,MK_FP(block,0),fsize,nsize);
	   fsize+=nsize;
	  }
	MMFreePtr(&block);

	MapFileHeader->oldtilenum=tilenum;
	MapFileHeader->oldtilemnum=tilemnum;

	SaveFile(mapheadname,MK_FP(MapFileHeader,2),2,sizeof(MapFileHeaderStr)-2);
	//
	// SAVE ALREADY COMPRESSED MAPS
	//

	//
	// IF MAPS ARE IN XMS ALREADY, ALLOCATE A NEW BUFFER
	// TO SHUFFLE ALREADY-COMPRESSED MAPS INTO
	//
	if (XMSmaps)
	  {
	   int planes;
	   long len=filelen(SM_loadname);


	   planes=(MapFileHeader->maptype&BPLANE>0)+
		  (MapFileHeader->maptype&FPLANE>0)+
		  (MapFileHeader->maptype&IPLANE>0);
	   len+=2L*mapwidth*mapheight*planes;

	   if (1024L*XMSTotalFree()<len)
	 {
	  XMSFreeMem(XMSmaps);
	  XMSmaps=0;
	 }
	   else
	 XMStemp=XMSAllocate(len);
	  }

	//
	// NOTE: I AM STORING "TED5" AT THE START OF THE FILE BECAUSE
	// SAVING THE FILE AT OFFSET 0 WILL TRASH IT (I HAVE TO RE-SAVE THE HEADER!)
	//
	if (XMSmaps)
	  XMSmove(0,(long)&TEDid,XMStemp,0,strlen(TEDid));
	else
	  SaveFile(SM_name,(char huge *)TEDid,0,strlen(TEDid));
	fsize=strlen(TEDid);

	for (i=0;i<100;i++)
    {
     long oldoff;

     if (MapFileHeader->dataoffsets[i]==-1 || i==whichmap)
       continue;

     oldoff=MapFileHeader->dataoffsets[i];

     if (XMSmaps)
	   XMSmove(XMSmaps,oldoff,0,(long)&TempHeader,sizeof(MapHeaderStr));
     else
       LoadFile(SM_loadname,(char huge *)&TempHeader,oldoff,sizeof(MapHeaderStr));

     strcpy(TempHeader.name,MapNames[i]);
     MapFileHeader->dataoffsets[i]=fsize;
     size=TempHeader.mapbkgndlen+TempHeader.mapfrgndlen+TempHeader.mapinfolen;
     change=TempHeader.mapbkgndpl-fsize-sizeof(MapHeaderStr);
     TempHeader.mapbkgndpl-=change;
     TempHeader.mapfrgndpl-=change;
     TempHeader.mapinfopl-=change;

     if (XMSmaps)
       XMSmove(0,(long)&TempHeader,XMStemp,fsize,sizeof(MapHeaderStr));
     else
       SaveFile(SM_name,(char huge *)&TempHeader,fsize,sizeof(MapHeaderStr));
     fsize+=sizeof(MapHeaderStr);

     if (XMSmaps)
       {
	XMSmove(XMSmaps,oldoff+sizeof(MapHeaderStr),XMStemp,fsize,size);
	fsize+=size;
	XMSmove(0,(long)"!ID!",XMStemp,fsize,4);
	fsize+=4;
       }
     else
       {
	MMAllocate(&block,size);
	LoadFile(SM_loadname,MK_FP(block,0),oldoff+sizeof(MapHeaderStr),size);
	SaveFile(SM_name,MK_FP(block,0),fsize,size);
	fsize+=size;
	SaveFile(SM_name,"!ID!",fsize,4);
	fsize+=4;
	MMFreePtr(&block);
       }
    }

    //
    // SAVE CURRENT MAP AT END OF FILE
    //
    if (!delmap)
      {
       MapFileHeader->dataoffsets[whichmap]=fsize;
       MapFileHeader->datalengths[whichmap]=sizeof(MapHeaderStr);
       if (XMSmaps)
	 XMSmove(0,(long)&MapHeader,XMStemp,fsize,sizeof(MapHeaderStr));
       else
	 SaveFile(SM_name,(char huge *)&MapHeader,fsize,sizeof(MapHeaderStr));
       fsize+=sizeof(MapHeaderStr);

       size=MapHeader.width*MapHeader.height*sizeof(int);
       MMAllocate(&block,size);
       if (MapFileHeader->maptype & BPLANE)
	 {
	  MapHeader.mapbkgndpl=fsize;
	  nsize=RLEWCompress(MK_FP(MapBkgnd,0),size,MK_FP(block,0),MapFileHeader->RLEWtag);

	  MapHeader.mapbkgndlen=nsize+2;

	  if (XMSmaps)
	    XMSmove(0,(long)&size,XMStemp,fsize,2);
	  else
	    SaveFile(SM_name,(char huge *)&size,fsize,2);

	  fsize+=2;

	  if (XMSmaps)
	    XMSmove(0,(long)MK_FP(block,0),XMStemp,fsize,nsize);
	  else
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

	  if (XMSmaps)
	    XMSmove(0,(long)&size,XMStemp,fsize,2);
	  else
	    SaveFile(SM_name,(char huge *)&size,fsize,2);

	  fsize+=2;

	  if (XMSmaps)
	    XMSmove(0,(long)MK_FP(block,0),XMStemp,fsize,nsize);
	  else
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

	  if (XMSmaps)
	    XMSmove(0,(long)&size,XMStemp,fsize,2);
	  else
	    SaveFile(SM_name,(char huge *)&size,fsize,2);

	  fsize+=2;

	  if (XMSmaps)
	    XMSmove(0,(long)MK_FP(block,0),XMStemp,fsize,nsize);
	  else
	    SaveFile(SM_name,MK_FP(block,0),fsize,nsize);

	  fsize+=nsize;
	 }
       else
	 MapHeader.mapinfolen=0;

       if (XMSmaps)
	 XMSmove(0,(long)"!ID!",XMStemp,fsize,4);
       else
	 SaveFile(SM_name,"!ID!",fsize,4);

       fsize+=4;

       MMFreePtr(&block);

       // RE-SAVE HEADER
       if (XMSmaps)
			XMSmove(0,(long)&MapHeader,XMStemp,
				MapFileHeader->dataoffsets[whichmap],sizeof(MapHeaderStr));
	   else
			SaveFile(SM_name,(char huge *)&MapHeader,
				MapFileHeader->dataoffsets[whichmap],sizeof(MapHeaderStr));

       //
       // RE-SAVE FILE HEADER
	   // NOTE: The "2" is so SaveFile doesn't truncate the fucking file!
       //
       SaveFile(mapheadname,MK_FP(MapFileHeader,2),2,sizeof(MapFileHeaderStr)-2);
	  }

    if (XMSmaps)
      {
       #define BCSIZE	0x4000
       long size=fsize,clen,coff;

       //
       // SAVE ENTIRE MAPFILE IN XMS TO DISK!
       //
       XMSFreeMem(XMSmaps);
       XMSmaps=XMStemp;

       MMAllocate(&block,BCSIZE);
       coff=0;
       do
       {
		clen=BCSIZE;
		if (size<BCSIZE)
			clen=size;

		XMSmove(XMSmaps,coff,0,(long)MK_FP(block,0),clen);
		SaveFile(SM_loadname,MK_FP(block,0),coff,clen);
		size-=BCSIZE;
		coff+=clen;

	   } while(size>0);

       MMFreePtr(&block);
      }
    else
      {
       unlink(SM_loadname);
       rename(SM_name,SM_loadname);
      }
   }

 AllocateUndoBuffers();
 UndoRegion.x=-1;
 SaveUndo(0,0,mapwidth,mapheight);
 RestoreBackground();
}






////////////////////////////////////////////////////
//
// BACKUP FILES!
//
////////////////////////////////////////////////////
void BackupFile(char *filename)
{
 #define BUFSIZE	0x8000
 memptr block;
 long size,clen,coff;
 int i;
 char backupname[14];


 size = filelen(filename);
 if (!size)
   return;

 strcpy(backupname,filename);
 for (i=strlen(backupname);i;i--)
   if (backupname[i] == '.')
	 backupname[i+1] = 0;
 strcat(backupname,"BAK");

 MMAllocate(&block,BUFSIZE);
 coff=0;
 do
 {
  clen=BUFSIZE;
  if (size<BUFSIZE)
	  clen=size;

  LoadFile(filename,(char far *)block,coff,clen);
  SaveFile(backupname,MK_FP(block,0),coff,clen);
  size-=BUFSIZE;
  coff+=clen;

 } while(size>0);

 MMFreePtr(&block);
}


////////////////////////////////////////////////////
//
// SAVE OUT TEDINFO FILE
//
////////////////////////////////////////////////////
void SaveTEDInfo(void)
{
 BackupFile(infoname);

 TEDInfo->oscrx=xbase;
 TEDInfo->oscry=ybase;
 _fstrcpy(TEDInfo->launchname,(char far *)launchname);
 SaveFile(infoname,MK_FP(TEDInfo,0),0,sizeof(InfoStruct));
}


////////////////////////////////////////////////////
//
// WRITE OUT THE OUTPUT HEADER FILE
//
////////////////////////////////////////////////////
char * _nfstrcpy(char *dest,char far *source)
{
 int i;

 for (i=0;i<_fstrlen(source);i++)
   *(dest+i)=*(source+i);

 return dest;
}


void SaveOutputHeader(void)
{
 char outhead[14]="MAPHEAD.",dot_h[14]="MAPS",t[15],tm[15],temp[40],temp1[40];
 FILE *fp;
 OutputHeadStr OutHead;
 int i,Thelast;
 long fsize;


 ErrDialog("Saving header...","");

 strcat(outhead,ext);

 BackupFile(outhead);

 memset(&OutHead,0,sizeof(OutputHeadStr));

 OutHead.RLEWtag=MapFileHeader->RLEWtag;

 for (i=0;i<100;i++)
   OutHead.dataoffsets[i]=MapFileHeader->dataoffsets[i];

 fsize=sizeof(OutputHeadStr);
 SaveFile(outhead,(char huge *)&OutHead,0,fsize);
 for (i=0;i<numtplanes;i++)
   {
    SaveFile(outhead,MK_FP(Tinfo[i],0),fsize,tilenum);
    fsize+=tilenum;
   }
 for (i=0;i<numtmplanes;i++)
   {
    SaveFile(outhead,MK_FP(TMinfo[i],0),fsize,tilemnum);
    fsize+=tilemnum;
   }

 RestoreBackground();

 //
 // DUMP OUT A MAPSext.H HEADER FILE
 //
 if (writeH)
   {
    ErrDialog("Saving .h file...","");

    strcat(dot_h,ext);
    strcat(dot_h,".H");
    unlink(dot_h);

    if ((fp=fopen(dot_h,"wt"))==NULL)
      return;

    fprintf(fp,"///////////////////////////////////////\n");
    fprintf(fp,"//\n");
    fprintf(fp,"// TED5 Map Header for %s\n",ext);
    fprintf(fp,"//\n");
    fprintf(fp,"///////////////////////////////////////\n");

    Thelast=99;
	for (i=99;i>=0;i--)
      if (MapFileHeader->dataoffsets[i]>=0)
      {
       Thelast=i;
       break;
      }


    fprintf(fp,"\n");
    fprintf(fp,"//\n");
    fprintf(fp,"// Map Names\n");
    fprintf(fp,"//\n");

    fprintf(fp,"typedef enum {\n");
    for (i=0;i<100;i++)
      if (MapFileHeader->dataoffsets[i]>=0)
	{
	 char temp[28];
	 int j;


	 strcpy(temp,&MapNames[i][0]);
	 strcat(temp,"_MAP");

	 for (j=0;j<strlen(temp);j++)
	   if (temp[j]==' ')
	     temp[j]='_';

	 strcat(temp,",");
	 while(strlen(temp)<25)
	   strcat(temp," ");

	 fprintf(fp,"\t\t%s// %d\n",strupr(temp),i);
	}
      else
	if (i<Thelast)
	  fprintf(fp,"\t\tEMPTYMAP%d,\n",i);

    fprintf(fp,"\t\tLASTMAP\n\t     } mapnames;\n");

    switch(tsize)
    {
     case 1:
       strcpy(t,"NUMTILE8");
       strcpy(tm,"NUMTILE8M");
	   break;
     case 2:
       strcpy(t,"NUMTILE16");
       strcpy(tm,"NUMTILE16M");
       break;
     case 3:
       strcpy(t,"NUMTILE32");
       strcpy(tm,"NUMTILE32M");
    }


    if (numtplanes)
    {
     fprintf(fp,"\n");
     fprintf(fp,"//\n");
     fprintf(fp,"// TILEINFO offsets\n");
     fprintf(fp,"//\n");

     memset(temp,0,sizeof(temp));
     memset(temp1,0,sizeof(temp1));
     fprintf(fp,"#define %s\x9\x9%d\n",
       strupr(_nfstrcpy(temp,MapFileHeader->tnames[0])),
       sizeof(OutputHeadStr));
     for (i=1;i<numtplanes;i++)
       {
	memset(temp,0,sizeof(temp));
	memset(temp1,0,sizeof(temp1));

	fprintf(fp,"#define %s\x9\x9(%s+%s)\n",
	  strupr(_nfstrcpy(temp,MapFileHeader->tnames[i])),
	  strupr(_nfstrcpy(temp1,MapFileHeader->tnames[i-1])),
	  t);
       }
    }

    if (numtmplanes)
    {
     fprintf(fp,"\n");
     fprintf(fp,"//\n");
     fprintf(fp,"// TILEINFOM offsets\n");
     fprintf(fp,"//\n");

     memset(temp,0,sizeof(temp));
     memset(temp1,0,sizeof(temp1));
     fprintf(fp,"#define %s\x9\x9(%s+%s)\n",
       strupr(_nfstrcpy(temp,MapFileHeader->tmnames[0])),
       strupr(_nfstrcpy(temp1,MapFileHeader->tnames[numtplanes-1])),
       t);
     for (i=1;i<numtmplanes;i++)
       {
	memset(temp,0,sizeof(temp));
	memset(temp1,0,sizeof(temp1));

	fprintf(fp,"#define %s\x9\x9(%s+%s)\n",
	  strupr(_nfstrcpy(temp,MapFileHeader->tmnames[i])),
	  strupr(_nfstrcpy(temp1,MapFileHeader->tmnames[i-1])),
	  tm);
       }
    }

    fclose(fp);
    writeH=0;
    RestoreBackground();
   }

}


////////////////////////////////////////////////////
//
// Initialize the TILEINFO/M arrays
//
////////////////////////////////////////////////////
btype TIb[]={{"                  ",1,2,1},
	     {"                  ",1,6,1},
	     {" Done ",12,9,2}};
DialogDef TId={" How many TILEINFO planes?\n\n\n\n"
	       " How many TILEINFOM planes?",
	       27,11,3,&TIb[0],NULL};

void InitTileinfo(void)
{
 unsigned temp,which,oktoexit=0,ox,oy,i,j;
 long fsize;

 numtplanes=numtmplanes=0;
 DrawDialog(&TId,1);
 GetButtonXY(&TId,0,&sx,&sy);
 printint(numtplanes);
 if (MapFileHeader->maptype&FPLANE)
   {
    GetButtonXY(&TId,1,&sx,&sy);
    printint(numtmplanes);
   }
 do
 {
  which=CheckButtons(&TId);
  switch(which)
  {
   case 1:
     MouseHide();
     GetButtonXY(&TId,0,&sx,&sy);
     ox=sx;
     oy=sy;
     print(TIb[0].text);
     sx=ox;
     sy=oy;
     if ((temp=inputint(9))!=ESCOUT && temp<11 && temp)
       numtplanes=temp;
     sx=ox;
     sy=oy;
     print(TIb[0].text);
     sx=ox;
     sy=oy;
     printint(numtplanes);
     MouseShow();

   case 2:
     MouseHide();
     GetButtonXY(&TId,1,&sx,&sy);
     ox=sx;
     oy=sy;
     print(TIb[1].text);
     sx=ox;
     sy=oy;
     if (MapFileHeader->maptype&FPLANE)
       {
	if ((temp=inputint(9))!=ESCOUT && temp<11 && temp)
	  numtmplanes=temp;
	sx=ox;
	sy=oy;
	print(TIb[1].text);
	sx=ox;
	sy=oy;
	printint(numtmplanes);
       }
     MouseShow();
     break;

   case 3:
     MouseHide();
     oktoexit=1;
     GetButtonXY(&TId,2,&sx,&sy);
     print(TIb[2].text);
     MouseShow();
  }

 } while(!oktoexit);

 RestoreBackground();

 //
 // allocate arrays
 //
 fsize=sizeof(MapFileHeaderStr);

 for (i=0;i<numtplanes;i++)
   {
    MMAllocate((memptr *)&Tinfo[i],tilenum);
    MapFileHeader->tileinfooff[i]=fsize;	// THIS DOESN'T REALLY MATTER
    fsize+=tilenum;				// ....YET!
    for(j=0;j<tilenum;j++)
      *(Tinfo[i]+j)=0;
   }

 for (i=0;i<numtmplanes;i++)
   {
    MMAllocate((memptr *)&TMinfo[i],tilemnum);
    MapFileHeader->tileinfomoff[i]=fsize;
    fsize+=tilemnum;
    for(j=0;j<tilemnum;j++)
      *(TMinfo[i]+j)=0;
   }

 MapFileHeader->numtplanes=numtplanes;
 MapFileHeader->numtmplanes=numtmplanes;

 MapFileHeader->oldtilenum=tilenum;
 MapFileHeader->oldtilemnum=tilemnum;
 writeH=1;
}


////////////////////////////////////////////////////
//
// Item - Edit the TILEINFO names
//
////////////////////////////////////////////////////
btype TINb={" Done ",8,15,1};
DialogDef TINd={"  Enter plane names:\n"
		" TILEINFO   TILEINFOM",
		22,17,1,&TINb,NULL};

void Item_EditTinfoNames(void)
{
 unsigned ox,oy,i,oktoexit=0;
 int which;

 MouseHide();
 DrawDialog(&TINd,1);
 GetDialogXY(&TINd,&sx,&sy);
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
     char temp[8];

     MouseHide();
redo:
     sx=ox+1;
     sy=oy+which+3;
     print("         ");
     sx=ox+1;
     sy=oy+which+3;
     if (input(temp,7))
       {
	for(i=0;i<8;i++)
	  MapFileHeader->tnames[which][i]=temp[i];
	which++;
	writeH=DirtyFlag=1;
	if (which<numtplanes)
	  goto redo;
       }
     else
       TInfoNoff(ox+1,oy+3+which,which);
    MouseShow();
    }

  if ((which=CheckList(ox+12,oy+3,8,numtmplanes,TInfoMNon,TInfoMNoff,0))>=0)
    {
     char temp[8];

     MouseHide();
redo1:
     sx=ox+12;
     sy=oy+which+3;
     print("         ");
     sx=ox+12;
     sy=oy+which+3;
     if (input(temp,7))
       {
	for(i=0;i<8;i++)
	  MapFileHeader->tmnames[which][i]=temp[i];
	which++;
	writeH=DirtyFlag=1;
	if (which<numtmplanes)
	  goto redo1;
       }
     else
       TInfoMNoff(ox+12,oy+3+which,which);
    MouseShow();
    }

  GetButtonXY(&TINd,0,&sx,&sy);
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

void TInfoNon(int x,int y,int b)
{
 xormask=1;
 sx=x;
 sy=y;
 if (!MapFileHeader->tnames[b][0])
   print("         ");
 else
   fprint(MapFileHeader->tnames[b]);
 xormask=0;
}
void TInfoNoff(int x,int y,int b)
{
 sx=x;
 sy=y;
 if (!MapFileHeader->tnames[b][0])
   print("         ");
 else
   fprint(MapFileHeader->tnames[b]);
}
void TInfoMNon(int x,int y,int b)
{
 xormask=1;
 sx=x;
 sy=y;
 if (!MapFileHeader->tmnames[b][0])
   print("         ");
 else
   fprint(MapFileHeader->tmnames[b]);
 xormask=0;
}
void TInfoMNoff(int x,int y,int b)
{
 sx=x;
 sy=y;
 if (!MapFileHeader->tmnames[b][0])
   print("         ");
 else
   fprint(MapFileHeader->tmnames[b]);
}
void TIDoneOn(int x,int y)
{
 xormask=1;
 sx=x;
 sy=y;
 print(" Done ");
 xormask=0;
}
void TIDoneOff(int x,int y)
{
 sx=x;
 sy=y;
 print(" Done ");
}

////////////////////////////////////////////////////
//
// Create a new map!
//
////////////////////////////////////////////////////
void CreateMap(int exitok)
{
 unsigned lwidth,lheight,i;
 int mapnum;
 long size;
 char lname[16];

 if ((mapnum=SelectMap(exitok,NOTCREATED,"TO CREATE"))==-1)
   return;

 InputMap(lname,&lwidth,&lheight,exitok);
 if (!lname[0])
   return;

 RemoveUndoBuffers();
 SaveCutBuffers();

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

 size=2L*(lwidth*lheight);

 if (MapFileHeader->maptype&BPLANE)
   {
    MMAllocate((memptr *)&MapBkgnd,size);
    MMAllocate((memptr *)&CutBkgnd,size);
    for(i=0;i<lwidth*lheight;i++)
      *(MapBkgnd+i)=*(CutBkgnd+i)=0;
   }
 if (MapFileHeader->maptype&FPLANE)
   {
    MMAllocate((memptr *)&MapFrgnd,size);
    MMAllocate((memptr *)&CutFrgnd,size);
    for(i=0;i<lwidth*lheight;i++)
      *(MapFrgnd+i)=*(CutFrgnd+i)=0;
   }
 if (MapFileHeader->maptype&IPLANE)
   {
    MMAllocate((memptr *)&MapInfoPl,size);
    MMAllocate((memptr *)&CutInfoPl,size);
    for(i=0;i<lwidth*lheight;i++)
      *(MapInfoPl+i)=*(CutInfoPl+i)=0;
   }

 whichmap=mapnum;
 strcpy(MapHeader.name,lname);
 MapHeader.width=mapwidth=lwidth;
 MapHeader.height=mapheight=lheight;
 strcpy(MapNames[mapnum],MapHeader.name);
 AllocateUndoBuffers();
 DirtyFlag=1;
 SaveMap(0);
 DirtyFlag=0;
 RestoreCutBuffers();
 SelectMode=PasteMode=xbase=ybase=0;
 SelX1=SelX2=SelY1=SelY2=-1;
 writeH=1;
}


////////////////////////////////////////////////////
//
// Load a map
//
////////////////////////////////////////////////////
void LoadMap(int mapnum)
{
 unsigned long csize,size=0;
 memptr block;

 SaveCutBuffers();
 //
 // DEALLOCATE ALL CURRENT MAP MEMORY
 //
 RemoveUndoBuffers();

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
 if (XMSmaps)
   XMSmove(XMSmaps,MapFileHeader->dataoffsets[mapnum],0,(long)&MapHeader,sizeof(MapHeaderStr));
 else
   LoadFile(mapname,(char huge *)&MapHeader,MapFileHeader->dataoffsets[mapnum],sizeof(MapHeaderStr));

 //
 // LOAD & DECOMPRESS MAP PLANES
 //
 if (MapFileHeader->maptype & BPLANE)
   {
    if (XMSmaps)
      XMSmove(XMSmaps,MapHeader.mapbkgndpl,0,(long)&size,2);
    else
      LoadFile(mapname,(char huge *)&size,MapHeader.mapbkgndpl,2);

    MMAllocate((memptr *)&MapBkgnd,size);
    MMAllocate((memptr *)&CutBkgnd,size);
    csize=MapHeader.mapbkgndlen-2;
    MMAllocate(&block,csize);

    if (XMSmaps)
      XMSmove(XMSmaps,MapHeader.mapbkgndpl+2,0,(long)MK_FP(block,0),csize);
    else
      LoadFile(mapname,MK_FP(block,0),MapHeader.mapbkgndpl+2,csize);

    RLEWExpand(MK_FP(block,0),MK_FP(MapBkgnd,0),size,MapFileHeader->RLEWtag);
    MMFreePtr(&block);
   }
 if (MapFileHeader->maptype & FPLANE)
   {
    if (XMSmaps)
      XMSmove(XMSmaps,MapHeader.mapfrgndpl,0,(long)&size,2);
    else
      LoadFile(mapname,(char huge *)&size,MapHeader.mapfrgndpl,2);

    MMAllocate((memptr *)&MapFrgnd,size);
    MMAllocate((memptr *)&CutFrgnd,size);
    csize=MapHeader.mapfrgndlen-2;
    MMAllocate(&block,csize);

    if (XMSmaps)
      XMSmove(XMSmaps,MapHeader.mapfrgndpl+2,0,(long)MK_FP(block,0),csize);
    else
      LoadFile(mapname,MK_FP(block,0),MapHeader.mapfrgndpl+2,csize);

    RLEWExpand(MK_FP(block,0),MK_FP(MapFrgnd,0),size,MapFileHeader->RLEWtag);
    MMFreePtr(&block);
   }
 if (MapFileHeader->maptype & IPLANE)
   {
    if (XMSmaps)
      XMSmove(XMSmaps,MapHeader.mapinfopl,0,(long)&size,2);
    else
      LoadFile(mapname,(char huge *)&size,MapHeader.mapinfopl,2);

    MMAllocate((memptr *)&MapInfoPl,size);
    MMAllocate((memptr *)&CutInfoPl,size);
    csize=MapHeader.mapinfolen-2;
    MMAllocate(&block,csize);

    if (XMSmaps)
      XMSmove(XMSmaps,MapHeader.mapinfopl+2,0,(long)MK_FP(block,0),csize);
    else
      LoadFile(mapname,MK_FP(block,0),MapHeader.mapinfopl+2,csize);

    RLEWExpand(MK_FP(block,0),MK_FP(MapInfoPl,0),size,MapFileHeader->RLEWtag);
    MMFreePtr(&block);
   }

 mapwidth=MapHeader.width;
 mapheight=MapHeader.height;
 strcpy(MapNames[mapnum],MapHeader.name);

 AllocateUndoBuffers();
 UndoRegion.x=-1;
 SaveUndo(0,0,mapwidth,mapheight);
 RestoreCutBuffers();

 SelectMode=PasteMode=xbase=ybase=0;
 SelX1=SelX2=SelY1=SelY2=-1;
}


////////////////////////////////////////////////////
//
// Select a new map!
//
////////////////////////////////////////////////////
btype ClvlB[]={{"\xb",28,5,1},
	       {"\xc",28,14,1},
	       {" Exit ",12,17,1}};
DialogDef ClvlD={"          SELECT MAP",
		 30,19,2,&ClvlB[0],NULL};
int smbase=0;

int SelectMap(int exitok,int createflg,char *title)
{
 MapHeaderStr TempMapHeader;
 unsigned ox,oy,i,dx,dy;
 int select=-1,redraw;

 if (exitok)
   {
    ClvlD.numbuttons=3;
    ClvlD.height=19;
   }
 else
   {
    ClvlD.numbuttons=2;
    ClvlD.height=16;
   }

 DrawDialog(&ClvlD,1);
 GetDialogXY(&ClvlD,&dx,&dy);
 sy=dy+1;
 sx=screencenterx-strlen(title)/2;
 print(title);

 dx+=3;
 dy+=5;

 MouseHide();
 GetButtonXY(&ClvlD,0,&sx,&sy);
 sx-=27;
 sy-=2;
 print("Map   Name\n");
 ox=sx+1;
 oy=sy+1;
 DrawBorder(sx,sy,26,11,2);
 MouseShow();


 do
 {
  int mx,my;

  //
  // DRAW MAP NAMES
  //
  MouseHide();
  for (i=0;i<10;i++)
  {
   sy=oy+i;
   sx=ox;
   if (i+smbase==whichmap)
     print("*");
   else
     print(" ");

   printint(i+smbase);
   print(" ");
   sx=ox+5;
   MNameOff(sx,sy,i);
  }
  MouseShow();
  redraw=0;

  do
  {
   GetButtonXY(&ClvlD,0,&(unsigned)mx,&(unsigned)my);
   if (!CheckList(mx,my,1,1,UparOn,UparOff,0) || keydown[0x48])
     {
      MouseHide();
      GetButtonXY(&ClvlD,0,&sx,&sy);
      print(ClvlB[0].text);

      smbase-=10;
      if (smbase<0)
	smbase=0;
      redraw=1;
      MouseShow();
      while(keydown[0x48]);
      continue;
     }

   GetButtonXY(&ClvlD,1,&(unsigned)mx,&(unsigned)my);
   if (!CheckList(mx,my,1,1,DnarOn,DnarOff,0) || keydown[0x50])
     {
      MouseHide();
      GetButtonXY(&ClvlD,1,&sx,&sy);
      print(ClvlB[1].text);

      smbase+=10;
      if (smbase>90)
	smbase=90;
      redraw=1;
      MouseShow();
      while(keydown[0x50]);
      continue;
     }

   if (exitok)
     {
      GetButtonXY(&ClvlD,2,&(unsigned)mx,&(unsigned)my);
      if (!CheckList(mx,my,6,1,ExitOn,ExitOff,0))
	{
	 RestoreBackground();
	 return -1;
	}
     }

   if (exitok && keydown[1])
     {
      RestoreBackground();
      return -1;
     }

   select=CheckList(ox+5,oy,20,10,MNameOn,MNameOff,0);
   if (select>=0)
     {
      MouseHide();
      sy=select+oy;
      sx=ox+5;
      MNameOff(sx,sy,select);
      MouseShow();
      if ((MapFileHeader->dataoffsets[select+smbase]!=-1 && createflg==NOTCREATED)||
	  (MapFileHeader->dataoffsets[select+smbase]==-1 && createflg==CREATED))
	select=-1;
     }

  } while(!redraw && select<0);
 } while(select<0);

 RestoreBackground();
 return select+smbase;
}

//
// Print arrows
//
void DnarOn(int x,int y)
{
 sx=x;
 sy=y;
 xormask=1;
 drawchar(sx,sy,0xc);
 xormask=0;
}
void DnarOff(int x,int y)
{
 sx=x;
 sy=y;
 drawchar(sx,sy,0xc);
}
void UparOn(int x,int y)
{
 sx=x;
 sy=y;
 xormask=1;
 drawchar(sx,sy,0xb);
 xormask=0;
}
void UparOff(int x,int y)
{
 sx=x;
 sy=y;
 drawchar(sx,sy,0xb);
}
void ExitOn(int x,int y)
{
 sx=x;
 sy=y;
 xormask=1;
 print(" Exit ");
 xormask=0;
}
void ExitOff(int x,int y)
{
 sx=x;
 sy=y;
 print(" Exit ");
}

//
// Highlight Map Name
//
void MNameOn(int x,int y,int i)
{
 xormask=1;
 MNameOff(x,y,i);
 xormask=0;
}

//
// De-Highlight Map Name
//
void MNameOff(int x,int y,int i)
{
 MapHeaderStr TempMapHeader;

 sx=x;
 sy=y;
 if (MapFileHeader->dataoffsets[i+smbase]!=-1)
   {
    print("                   ");
    sx=x;
    print(MapNames[i+smbase]);
   }
 else
   print("--not created yet--");
}



////////////////////////////////////////////////////
//
// Pick more planes for the map set
//
////////////////////////////////////////////////////
btype PickPlaneb[]={{"X",1,3,1},
		    {"X",1,6,1},
		    {" Done ",1,9,2}};
DialogDef PickPlaneD={"Which other planes\n"
		     "to enable?\n\n"
		     "    Foreground\n\n\n"
		     "    Information"
		     ,18,11,3,&PickPlaneb[0],NULL};

int PickMorePlanes(void)
{
 int which;
 char planes=7;	// 1=bkgnd/2=frgnd/4=info

 DrawDialog(&PickPlaneD,1);
 do
 {
  which=CheckButtons(&PickPlaneD);
  switch(which)
  {
   case 0:
     RestoreBackground();
     return 1;
   case 1:
     GetButtonXY(&PickPlaneD,0,&sx,&sy);
     planes^=2;
     MouseHide();
     if (planes&2)
       print("X");
     else
       print(" ");
     MouseShow();
     break;
   case 2:
     GetButtonXY(&PickPlaneD,1,&sx,&sy);
     planes^=4;
     MouseHide();
     if (planes&4)
       print("X");
     else
       print(" ");
     MouseShow();
  }
 } while(which!=3);

 RestoreBackground();
 MapFileHeader->maptype=planes;
 return 0;
}

////////////////////////////////////////////////////
//
// Get map dimensions
//
////////////////////////////////////////////////////
btype LvlSpecb[]={{"                  ",1,3,1},
		  {"                  ",1,7,1},
		  {"                  ",1,11,1},
		  {" Done ",7,14,2}};
DialogDef LvlSpec={"   MAP DIMENSIONS\n"
		   " Map Name:\n\n\n\n"
		   " Map Width:\n\n\n\n"
		   " Map Height:\n",
		   20,16,4,&LvlSpecb[0],NULL};

void InputMap(char *lvlname,unsigned *levelw,unsigned *levelh,int exitok)
{
 unsigned butn,ok=0,nameok=0,widok=0,heightok=0,
     ox,oy,width=0,height=0,oldwidth=0,oldheight=0;
 char oldname[16]="",name[16]="";

 MouseHide();
 DrawDialog(&LvlSpec,1);
 GetButtonXY(&LvlSpec,1,&sx,&sy);
 printint(width);
 GetButtonXY(&LvlSpec,2,&sx,&sy);
 printint(height);
 MouseShow();

 //
 // START INPUT IMMEDIATELY
 //
 butn=1;
 goto firsttime;

#pragma warn -rch
 while (!ok)
#pragma warn +rch
 {
  butn=CheckButtons(&LvlSpec);
  if (!butn)
     if (exitok)
      {
       RestoreBackground();
       lvlname[0]=0;
       return;
      }
     else
      continue;

firsttime:

  xormask=0;
  GetButtonXY(&LvlSpec,butn-1,&sx,&sy);
  MouseHide();
  ox=sx;
  oy=sy;
  print(LvlSpecb[butn-1].text);
  sx=ox;
  sy=oy;
  switch(butn)
  {
   case 1:
     name[0]=0;
     if (!input(name,15))
       {
	strcpy(name,oldname);
	sx=ox;
	sy=oy;
	print(LvlSpecb[butn-1].text);
       }
     strcpy(oldname,name);
     sx=ox;
     sy=oy;
     if (name[0])
		 {
	print(name);
	nameok=1;
       }
     else
       nameok=0;

     if (nameok)
       {
	butn=2;
	GetButtonXY(&LvlSpec,butn-1,&sx,&sy);
	ox=sx;
	oy=sy;
	print(LvlSpecb[butn-1].text);
	sx=ox;
	sy=oy;
       }
     else
     break;

   case 2:
     width=0;
     if ((width=inputint(9))==ESCOUT)
       {
	widok=0;
	width=oldwidth;
       }
     if (width)
       widok=1;

     oldwidth=width;
     sx=ox;
     sy=oy;
     print(LvlSpecb[butn-1].text);
	  sx=ox;
     sy=oy;
     printint(width);
     if (widok)
       {
	butn=3;
	GetButtonXY(&LvlSpec,butn-1,&sx,&sy);
	ox=sx;
	oy=sy;
	print(LvlSpecb[butn-1].text);
	sx=ox;
	sy=oy;
       }
     else
     break;

   case 3:
     height=0;
     if ((height=inputint(9))==ESCOUT)
       {
	heightok=0;
	height=oldheight;
       }
     if (height)
       heightok=1;

     oldheight=height;
     sx=ox;
     sy=oy;
     print(LvlSpecb[butn-1].text);
     sx=ox;
     sy=oy;
     printint(height);
     break;

   case 4:
     if (2L*height*width>0x10000)	// TOO BIG! TRIM IT!
       {
	errsound();
	errsound();
	widok=heightok=0;
	break;
       }
     if (nameok && widok && heightok)
       ok=1;
  }

  MouseShow();
 }

 RestoreBackground();

 *levelw=width;
 *levelh=height;
 strcpy(lvlname,name);
}

////////////////////////////////////////////////////
//
// Item - Input INFOPLANE value
//
////////////////////////////////////////////////////
btype EnterIVb={"          ",1,3,1};
DialogDef EnterIV={"Enter a value for\nthe INFO plane:\n\n\n",18,5,1,&EnterIVb,NULL};

void Item_InputInfoplane(void)
{
 unsigned val;


 if (TsearchMode || BfillMode || FillMode || PasteMode || SelectMode)
   return;

 clearkeys();
 DrawDialog(&EnterIV,1);
 MouseHide();
 GetButtonXY(&EnterIV,0,&sx,&sy);
 if ((val=inputint(9))!=ESCOUT)
   whichi=val+tilenum;
 RestoreBackground();
 DrawInfoBar();
 MouseShow();
}

////////////////////////////////////////////////////
//
// Item - Select Tile
//
////////////////////////////////////////////////////
void Item_SelectTile(void)
{
 SelectTiles(0);
}

////////////////////////////////////////////////////
//
// Item - Block Fill
//
////////////////////////////////////////////////////
void Item_BlockFill(void)
{
 ZeroModes();
 BfillMode=1;
 DrawInfoBar();
}

//
// DO THE ACTUAL BLOCK FILL
//
void DoBlockFill(void)
{
 unsigned loc,i,j,ctrl=0,newt,newm,newi,tton,tmon,tion,from;

 if (keydown[0x1d])
   ctrl++;

 MouseHide();

 CopyUndoRegion();
 UndoRegion.x=SelX1;
 UndoRegion.y=SelY1;
 UndoRegion.w=SelX2-SelX1+1;
 UndoRegion.h=SelY2-SelY1+1;

 if (ctrl)
   {
    for (j=SelY1;j<=SelY2;j++)
      for (i=SelX1;i<=SelX2;i++)
	{
	 loc=j*mapwidth+i;
	 from=(TileCopy.y+((j-SelY1)%TileCopy.h))*mapwidth+
	      TileCopy.x+((i-SelX1)%TileCopy.w);

	 switch(TileCopy.MapOrTileSelect)
	 {
	  case 0: // COPY BUFFER
		 tton=TileCopy.PlanesCopied&BPLANE;
	    tmon=TileCopy.PlanesCopied&FPLANE;
	    tion=TileCopy.PlanesCopied&IPLANE;

	    newt=CutBkgnd[from];
	    newm=CutFrgnd[from];
	    newi=CutInfoPl[from];

	    break;
	  case 1: // TILES
	    tton=1;
	    tmon=tion=0;

	    newt=((j%TileCopy.h)+TileCopy.y)*selectcols+
		 TileCopy.x+(i%TileCopy.w);
	    if (XMSlookup[newt]<0)
	      tton=0;
	    break;
	  case 2: // MASKED
	    tton=tion=0;
	    tmon=1;

	    newm=((j%TileCopy.h)+TileCopy.y)*selectcols+
		 TileCopy.x+(i%TileCopy.w)+tilenum+maxiconrows*selectcols;
	    if (XMSlookup[newm]<0)
	      tmon=0;
	    else
	      newm-=tilenum;
	 }

	 if (tton)
	   *(MapBkgnd+loc)=newt;
	 if (tmon)
	   *(MapFrgnd+loc)=newm;
	 if (tion)
	   *(MapInfoPl+loc)=newi;

	 if (j>=ybase && j<ybase+screenh &&
	     i>=xbase && i<xbase+screenw)
	   {
	    int tempt,tempm,tempi;

	    tempt=*(MapBkgnd+loc);
	    tempm=*(MapFrgnd+loc)+tilenum;
	    tempi=*(MapInfoPl+loc)+tilenum;

	    CombineTiles(viewton?tempt:-BkgndColor,viewmon*tempm,viewion*tempi,tsize);
	    if (GridMode)
	      Overlay(tsize);
	    DrawTile((i-xbase)<<(tsize-1),((j-ybase)<<(tsize+2))+8,tsize);
	   }
	}
   }
 else
   for (j=SelY1;j<=SelY2;j++)
     for (i=SelX1;i<=SelX2;i++)
       {
	loc=j*mapwidth+i;

	if (planeton)
	  *(MapBkgnd+loc)=whicht;
	if (planemon)
	  *(MapFrgnd+loc)=whichtm-tilenum;
	if (planeion)
	  *(MapInfoPl+loc)=whichi-tilenum;

	if (j>=ybase && j<ybase+screenh &&
	    i>=xbase && i<xbase+screenw)
	  {
	   int tempt,tempm,tempi;

	   tempt=*(MapBkgnd+loc);
	   tempm=*(MapFrgnd+loc)+tilenum;
	   tempi=*(MapInfoPl+loc)+tilenum;

	   CombineTiles(viewton?tempt:-BkgndColor,viewmon*tempm,viewion*tempi,tsize);
	   if (GridMode)
	     Overlay(tsize);
	   DrawTile((i-xbase)<<(tsize-1),((j-ybase)<<(tsize+2))+8,tsize);
	  }
       }

 DirtyFlag=1;
 MouseShow();
}


////////////////////////////////////////////////////
//
// Remove the XMS undo buffers
//
////////////////////////////////////////////////////
void RemoveUndoBuffers(void)
{
 if (XMSundoB)
   XMSFreeMem(XMSundoB);
 if (XMSundoF)
   XMSFreeMem(XMSundoF);
 if (XMSundoI)
   XMSFreeMem(XMSundoI);

 XMSundoB=XMSundoF=XMSundoI=0;
}


////////////////////////////////////////////////////
//
// Allocate XMS undo buffers
// NOTE: uses "mapwidth" & "mapheight"
//
////////////////////////////////////////////////////
void AllocateUndoBuffers(void)
{
 long size=2L*mapwidth*mapheight;

 if (MapFileHeader->maptype&BPLANE)
   XMSundoB=XMSAllocate(size);
 if (MapFileHeader->maptype&FPLANE)
   XMSundoF=XMSAllocate(size);
 if (MapFileHeader->maptype&IPLANE)
   XMSundoI=XMSAllocate(size);
}


////////////////////////////////////////////////////
//
// Save Undo buffers
//
////////////////////////////////////////////////////
void SaveUndo(int x,int y,int w,int h)
{
 unsigned hsize=w*2,j;

 for (j=y;j<y+h;j++)
   {
    unsigned off=2*(j*mapwidth+x);

    if (MapFileHeader->maptype&BPLANE)
      XMSmove(0,(long)MK_FP(MapBkgnd,off),XMSundoB,off,hsize);
    if (MapFileHeader->maptype&FPLANE)
      XMSmove(0,(long)MK_FP(MapFrgnd,off),XMSundoF,off,hsize);
    if (MapFileHeader->maptype&IPLANE)
      XMSmove(0,(long)MK_FP(MapInfoPl,off),XMSundoI,off,hsize);
   }
}


////////////////////////////////////////////////////
//
// Restore Undo buffers
//
////////////////////////////////////////////////////
void RestoreUndo(void)
{
 long size=2L*UndoRegion.w;
 unsigned j;

 sound(2000);
 for (j=UndoRegion.y;j<UndoRegion.y+UndoRegion.h;j++)
   {
    unsigned loc=2*(j*mapwidth+UndoRegion.x);

    if (MapFileHeader->maptype&BPLANE)
      XMSmove(XMSundoB,loc,0,(long)MK_FP(MapBkgnd,loc),size);
    if (MapFileHeader->maptype&FPLANE)
      XMSmove(XMSundoF,loc,0,(long)MK_FP(MapFrgnd,loc),size);
    if (MapFileHeader->maptype&IPLANE)
      XMSmove(XMSundoI,loc,0,(long)MK_FP(MapInfoPl,loc),size);
   }

 nosound();
}


////////////////////////////////////////////////////
//
// Copy the current UNDO region to the undo buffers
//
////////////////////////////////////////////////////
void CopyUndoRegion(void)
{
 if (UndoRegion.x==-1)
	return;

 SaveUndo(UndoRegion.x,UndoRegion.y,UndoRegion.w,UndoRegion.h);
}


////////////////////////////////////////////////////
//
// Item - Undo
//
////////////////////////////////////////////////////
void Item_Undo(void)
{
 if (UndoRegion.x==-1)
	{
	 ErrDialog("You don't have anything to UNDO!"," Oh, yeah. ");
	 return;
	}

 RestoreUndo();
 DrawMap();
}


////////////////////////////////////////////////////
//
// Item - Tile Search!
//
////////////////////////////////////////////////////
void Item_TileSearch(void)
{
 int num=planeton+planemon+planeion;

 ZeroModes();
 if (num>1)
	{
	 ErrDialog("TILE SEARCH will only work\n"
			"with ONE active plane! Make\n"
			"sure that the tile you're\n"
			"searching for is selected!"," OK ");
	 return;
	}

 TsearchMode=1;
 DrawInfoBar();
 DrawMap();
}

////////////////////////////////////////////////////
//
// Item - Launch
//
////////////////////////////////////////////////////
char tempparm[40];

void Item_Launch(void)
{
 TempStruct LaunchInfo;
 char ename[64],*envstr[15],temp[40],temp1[40],temp2[40],tiname[14]="TEDINFO.TMP",i,j;
 long size;
 int  rottlaunch;
 int  startp;
 unsigned mx,my;


 rottlaunch=0;
 if (!TEDInfo->launchname[0])
	{
	 ErrDialog("You didn't specify a launching\n"
			"name on the command line? What\n"
			"do I launch, Einstein? Geez!"," What a goober. ");
	 return;
	}

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

 //
 // ICON CHANGE ON LAUNCH?
 //
 if (keydown[0x2A])
	 {
	 rottlaunch=1;
	 startp=5;
	 mx=xbase+(pixelx>>(tsize+2));
	 my=ybase+((pixely-8)>>(tsize+2));
	 }
 else
	 startp=3;
 if (pixely>=8 && pixely<infoy*8 && TEDInfo->permicon
	  && MapFileHeader->maptype&IPLANE && !usingbat && keydown[0x1d])
	{
	 unsigned i,j;

	 mx=xbase+(pixelx>>(tsize+2));
	 my=ybase+((pixely-8)>>(tsize+2));

	 for (j=0;j<mapheight;j++)
		for (i=0;i<mapwidth;i++)
	if (MapInfoPl[j*mapwidth+i]==TEDInfo->permicon)
	  {
		MapInfoPl[j*mapwidth+i]=0;
		TEDInfo->lastx=i;
		TEDInfo->lasty=j;
		break;
	  }

	 MapInfoPl[my*mapwidth+mx]=TEDInfo->permicon;
	 DrawMap();
	 DirtyFlag=1;
	}

 TEDInfo->oscrx=xbase;
 TEDInfo->oscry=ybase;
 TEDInfo->pflags=((planeton&1)<<6)|
		 ((planemon&1)<<5)|
		 ((planeion&1)<<4)|
		 ((viewton&1)<<2)|
		 ((viewmon&1)<<1)|
		 (viewion&1);

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

 envstr[0] = ename;
 envstr[1] = "/TEDLEVEL";
 itoa(whichmap,temp,10);
 envstr[2] = temp;
 if (rottlaunch)
	 {
	 itoa(mx,temp1,10);
	 envstr[3] = temp1;
	 itoa(my,temp2,10);
	 envstr[4] = temp2;
	 }
 //
 // CHOP UP THE PARMSTRING FOR 'EXECV'
 //
 strcpy(tempparm,parmstring);
 envstr[startp]=&tempparm[0];
 for(j=startp+1,i=0;i<strlen(tempparm);i++)
	if (tempparm[i]==' ')
	{
	 envstr[j]=&tempparm[i+1];
	 tempparm[i]=0;
	 j++;
	}
 envstr[j]=0;

 RemoveUndoBuffers();
 ShutdownKBD();

 //
 // ARE WE EXITING WITH A BATCH FILE?
 //

 if (usingbat)
	{
	 setvideo(TEXT);
	 exit(whichmap+2);
   }

 fcloseall();
 MMShutdown();
 execv(ename,envstr);

 SetupKBD();
 RestoreBackground();
 MouseShow();
 ErrDialog("LAUNCHing failed. TED5 will\n"
	   "now restart."," I have gubs ");
 ShutdownKBD();
 execlp("TED5.EXE","TED5.EXE","/LAUNCH",NULL);

 printf("Couldn't find TED5 for some reason!");
 exit(1);
}


////////////////////////////////////////////////////
//
// Item - TILEINFO Copy
//
////////////////////////////////////////////////////
DialogDef TICd=
{"TILEINFOM Copy Kludge\n\n"
 "From:\n"
 "  To:\n"
 "Rows:",22,6};


void Item_TINFOCopy(void)
{
 unsigned dx,dy,i,from,to,amt;
 memptr block;


 MouseHide();
 DrawDialog(&TICd,1);
 GetDialogXY(&TICd,&dx,&dy);


 sx=dx+5;
 sy=dy+2;
 from=inputint(9);
 if (from==ESCOUT)
 {
  RestoreBackground();
  MouseShow();
  return;
 }


 sx=dx+5;
 sy=dy+3;
 to=inputint(9);
 if (to==ESCOUT)
 {
  RestoreBackground();
  MouseShow();
  return;
 }


 sx=dx+5;
 sy=dy+4;
 amt=inputint(9);
 if (amt==ESCOUT)
 {
  RestoreBackground();
  MouseShow();
  return;
 }
 amt*=18;


 MMAllocate(&block,amt);

 for (i=0;i<numtmplanes;i++)
 {
  movedata((unsigned)TMinfo[i],from,(unsigned)block,0,amt);
  movedata((unsigned)block,0,(unsigned)TMinfo[i],to,amt);
 }

 MMFreePtr(&block);
 RestoreBackground();
 DirtyFlag=writeH=1;


 MouseShow();
}
