#include "ted5.h"
#pragma hdrstop

extern char far TEDCHAR,far VGAPAL;

void Quit(char *string);
void drawchar(int x,int y,int chr);
void centerwindow (int width, int height);

int win_xl,win_yl,win_xh,win_yh;
int screencenterx = 19,screencentery = 11;
unsigned char keydown[256];
unsigned leftedge,yshift,xormask,MouseStatus,sx,sy;
void interrupt (*oldint9) ()=NULL;
enum {false,true} boolean;
memptr CGAfont,VGAfont;
unsigned doubled[256];
//
// Special vars to handle EGA3 mouse mode
//
int EGA3mx,EGA3my;


////////////////////////////////////////////////////////////////////
//
// Return a file's length
//
////////////////////////////////////////////////////////////////////
long filelen(char *filename)
{
 long size;
 int handle;

 if ((handle=open(filename,O_BINARY))==-1)
   return 0;

 size=filelength(handle);
 close(handle);
 return size;
}

////////////////////////////////////////////////////////////////////
//
// WaitVBL
//
////////////////////////////////////////////////////////////////////
void WaitVBL(int times)
{
asm	mov	cx,times
asm	mov	dx,crtcaddr
asm	add	dx,6

waitvbl1:
asm	in	al,dx
asm	test	al,00001000b	//;look for vbl
asm	jnz	waitvbl1

waitvbl2:
asm	in	al,dx
asm	test	al,00001000b	//;look for vbl
asm	jz	waitvbl2

asm	loop	waitvbl1
}


////////////////////////////////////////////////////////////////////
//
// Int9ISR
// Called for every keypress.  Keeps track of which keys are down, and passes
// the key on to DOS after clearing the dos buffer (max 1 char in buffer).
//
////////////////////////////////////////////////////////////////////
void interrupt Int9ISR ()
{
 int key = inportb (0x60);		/* get the key pressed */

 if (key>127)
   keydown [key-128] = false;		/* break scan code */
 else
 {
   keydown [key] = true;		/* make scan code */
   poke (0x40,0x1c,peek(0x40,0x1a));	/* clear the bios key buffer */
 }
asm {
   push ax
   push	bx
   push	cx
   push	dx
   push	si
   push	di
   push	bp
 }
 oldint9 ();				/* give it to DOS */
asm {
   pop	bp
   pop  di
   pop	si
   pop	dx
   pop	cx
   pop	bx
   pop	ax
 }
 outport (0x20,0x20);			/* tell the int manager we got it */
}


////////////////////////////////////////////////////////////////////
//
// SetupKBD
// Clears the keydown array and installs the INT 9 ISR if it isn't already
// hooked up.
//
////////////////////////////////////////////////////////////////////
void SetupKBD ()
{
 void far *vect = getvect (9);
 int i;

 for (i=0;i<128;i++)			/* clear our key down table */
   keydown[i]= false;

 poke (0x40,0x1c,peek(0x40,0x1a));	/* clear the bios key buffer */

 if ( &Int9ISR != vect ) 		/* is our handler allready set up? */
 {
   oldint9 = vect;
   setvect (9,Int9ISR);
 }
}


////////////////////////////////////////////////////////////////////
//
// ShutdownKBD
// Sets the int 9 vector back to oldint 9
//
////////////////////////////////////////////////////////////////////
void ShutdownKBD ()
{
 if (oldint9 != NULL)
   setvect (9,oldint9);
}


////////////////////////////////////////////////////////////////////
//
// clearkeys
// Clears out the bios buffer and zeros out the keydown array
//
////////////////////////////////////////////////////////////////////
void clearkeys (void)
{
  int i;
  while (bioskey (1))
    bioskey(0);

  for (i=0;i<128;i++)
    keydown [i]=0;
}


////////////////////////////////////////////////////////////////////
//
// Mouse Routines
//
////////////////////////////////////////////////////////////////////
int MouseInit(void)
{
 union REGS regs;
 unsigned char far *vector;

 if ((vector=MK_FP(peek(0,0x33*4+2),peek(0,0x33*4)))==NULL) return 0;

 if (*vector == 207)
   return MouseStatus = 0;

 _AX=0;
 geninterrupt(0x33);
 EGA3mx=800/2;
 EGA3my=600/2;

 //
 // Set CGA mouse cursor (normal one sucks)
 //
 if (videomode==CGA)
   {
    static unsigned CGAcursor[]=
      {
       0x0fff,0x03ff,0x00ff,0x003f,0x000f,0x0003,0x0000,0x000f,0x0c03,0x3c03,
       0xff03,0xffff,0xffff,0xffff,0xffff,0xffff,

       0xf000,0xcc00,0xc300,0xc0c0,0xc030,0xc00c,0xc03f,0xcc30,0xf30c,0xc30c,
       0x00fc,0x0000,0x0000,0x0000,0x0000,0x0000
      };

    _BX=0;
    _CX=0;
    _DX=FP_OFF(CGAcursor);
    _ES=_DS;
    _AX=9;
    geninterrupt(0x33);
   }

 return MouseStatus = 1;
}


void MouseOrigin(int x,int y)
{
 if (!MouseStatus) return;

 _CX=x;
 _DX=y;
 _AX=4;
 geninterrupt(0x33);
}


void MouseLimits(int xmin,int xmax,int ymin,int ymax)
{
 if (!MouseStatus) return;

 _CX=xmin;
 _DX=xmax;
 _AX=7;
 geninterrupt(0x33);
 _CX=ymin;
 _DX=ymax;
 _AX=8;
 geninterrupt(0x33);
}


void MouseHide(void)
{
 if (!MouseStatus) return;

 _AX=2;
 geninterrupt(0x33);
}



void MouseShow(void)
{
 if (!MouseStatus) return;

 _AX=1;
 geninterrupt(0x33);
}



int MouseButton(void)
{
 union REGS regs;

 if (!MouseStatus) return 0;

 regs.x.ax=3;
 int86(0x33,&regs,&regs);
 return(regs.x.bx);
}



void MouseCoords(int *x,int *y)
{
 union REGS regs;

 if (!MouseStatus)
   return;

 regs.x.ax=3;
 int86(0x33,&regs,&regs);
 *x=regs.x.cx;
 *y=regs.x.dx;

 *x/=2;
 if (videomode==EGA2)
   *x*=2;
}

/////////////////////////
//
// print
// Prints a string at sx,sy.  No clipping!!!
//
/////////////////////////

void print (const char *str)
{
  char ch;

  while ((ch=*str++) != 0)
    if (ch == '\n')
    {
      sy++;
      sx=leftedge;
    }
    else if (ch == '\r')
      sx=leftedge;
    else
      drawchar (sx++,sy,ch);
}
void fprint (const char huge *str)
{
  char ch;

  while ((ch=*str++) != 0)
    if (ch == '\n')
    {
      sy++;
      sx=leftedge;
    }
    else if (ch == '\r')
      sx=leftedge;
    else
      drawchar (sx++,sy,ch);
}

////////////////////////////////////////////////////////////////////
//
// print hex byte
//
////////////////////////////////////////////////////////////////////
void printhexb(unsigned char value)
{
 int loop;
 char hexstr[16]="0123456789ABCDEF",str[2]="";

 for (loop=0;loop<2;loop++)
   {
    str[0]=hexstr[(value>>(1-loop)*4)&15];
    print(str);
   }
}

////////////////////////////////////////////////////////////////////
//
// print hex
//
////////////////////////////////////////////////////////////////////
void printhex(unsigned value)
{
 print("$");
 printhexb(value>>8);
 printhexb(value&0xff);
}


////////////////////////////////////////////////////////////////////
//
// print int
//
////////////////////////////////////////////////////////////////////
void printint(unsigned value)
{
 char temp[10];

 ultoa((unsigned long)value,temp,10);
 print(temp);
}


////////////////////////////////////////////////////////////////////
//
// print bin
//
////////////////////////////////////////////////////////////////////
void printbin(unsigned value)
{
 int loop;

 print("%");
 for (loop=0;loop<16;loop++)
    if ((value>>15-loop)&1) print("1"); else print("0");
}


////////////////////////////////////////////////////////////////////
//
// input unsigned
//
////////////////////////////////////////////////////////////////////
unsigned inputint(int numchars)
{
 char string[18]="",digit,hexstr[16]="0123456789ABCDEF";
 unsigned value,loop,loop1;

 if (!input(string,numchars))
   return ESCOUT;

 if (string[0]=='$')
   {
    int digits;

    digits=strlen(string)-2;
    if (digits<0) return 0;

    for (value=0,loop1=0;loop1<=digits;loop1++)
      {
       digit=toupper(string[loop1+1]);
       for (loop=0;loop<16;loop++)
	  if (digit==hexstr[loop])
	    {
	     value|=(loop<<(digits-loop1)*4);
	     break;
	    }
      }
   }
 else if (string[0]=='%')
   {
    int digits;

    digits=strlen(string)-2;
    if (digits<0) return 0;

    for (value=0,loop1=0;loop1<=digits;loop1++)
      {
       if (string[loop1+1]<'0' || string[loop1+1]>'1') return 0;
       value|=(string[loop1+1]-'0')<<(digits-loop1);
      }
   }
 else value=atoi(string);
 return value;
}


////////////////////////////////////////////////////////////////////
//
// line input routine
//
////////////////////////////////////////////////////////////////////
int input(char *string,int max)
{
 char key;
 int count=0,loop;

 do {
     key=get()&0xff;
     if ((key==127 || key==8)&&count>0)
       {
	count--;
	drawchar(sx,sy,' ');
	sx--;
       }

     if (key>=' ' && key<='z' && count<max)
       {
	*(string+count++)=key;
	drawchar(sx++,sy,key);
       }

    } while (key!=27 && key!=13);

 for (loop=count;loop<max;loop++) *(string+loop)=0;

 while(keydown[1]);	// don't let ESC repeat

 if (key==13)
   {
    while(keydown[0x1c]);
    return 1;
   }
 return 0;
}

void bar (int xl, int yl, int xh, int yh, int ch)
{
  int x,y;

  for (y=yl;y<=yh;y++)
    for (x=xl;x<=xh;x++)
      drawchar (x,y,ch);
}

/////////////////////////
//
// get
// Flash a cursor at sx,sy and waits for a user bioskey
//
/////////////////////////

int get (void)
{
 int cycle,key;

 do
 {
   cycle = 9;
   while (!(key = bioskey(1)) && cycle<11)
   {
     drawchar (sx,sy,cycle++);
     WaitVBL (5);
   }
 } while (key == 0);
 drawchar (sx,sy,' ');
 return bioskey(0);		// take it out of the buffer
}


////////////////////////////////////////////////////////////////////
//
// drawchar
//
////////////////////////////////////////////////////////////////////
void drawchar(int x,int y,int chr)
{
 switch(videomode)
 {
  case CGA: CGAcharout(x,y,chr); break;
  case EGA1:
  case EGA2: EGAcharout(x,y,chr,videomode); break;
  case VGA: VGAcharout(x,y,chr);
 }
}

////////////////////////////////////////////////////////////////////
//
// Set a video mode
//
////////////////////////////////////////////////////////////////////
void setvideo(video vid)
{
 //
 // create CGA font (if not already created)
 //
 if (vid==CGA && !CGAfont)
   {
    unsigned char huge *oldfont=MK_FP(FP_SEG(&TEDCHAR),FP_OFF(&TEDCHAR));
    unsigned i,j,huge *newfont;

    centerwindow(20,1);
    print("Creating CGA font...");

    MMAllocate(&CGAfont,2048);
    newfont=MK_FP(CGAfont,0);

    for(i=0;i<127;i++)
      for(j=0;j<8;j++)
	(unsigned)*(newfont+i*8+j)=doubled[*(oldfont+i*8+j)]; //color=|0xaaaa;
   }
 else
 //
 // OR create VGA font (if not already created)
 //
 if (vid==VGA && !VGAfont)
   {
    unsigned char huge *oldfont=MK_FP(FP_SEG(&TEDCHAR),FP_OFF(&TEDCHAR)),
	huge *newfont;
    unsigned i,j,k;
    unsigned char bitbyte[2]={0,0xff}; // colors={0x7e,0x78};

    centerwindow(20,1);
    print("Creating VGA font...");

    MMAllocate(&VGAfont,0x2000);
    newfont=MK_FP(VGAfont,0);

    for(i=0;i<128;i++)
      for(j=0;j<8;j++)
	for(k=0;k<8;k++)
	  *(newfont+i*64+j*8+k)=bitbyte[(*(oldfont+i*8+j)>>(7-k))&1];
   }
 //
 // Now, change video modes!
 //
 switch(vid)
 {
  case TEXT: _AX=3; break;
  case CGA:
    screencenterx=19;
    screencentery=11;
    scrnbot=199;
    scrnrgt=319;
    _AX=4;
    break;
  case EGA1:
    screencenterx=19;
    screencentery=11;
    scrnbot=199;
    scrnrgt=319;
    _AX=0x0d;
    break;
  case EGA2:
    screencenterx=39;
    screencentery=29;
    scrnbot=479;
    scrnrgt=638;
    _AX=0x12;
    break;
  case VGA:
    _AX=0x13;
    screencenterx=19;
    screencentery=11;
    scrnbot=199;
    scrnrgt=319;
 }
 geninterrupt(0x10);
 videomode=vid;

 //
 // Set CGA mouse cursor (normal one sucks)
 //
 if (vid==CGA)
   {
    static unsigned CGAcursor[]=
      {
       0x0fff,0x03ff,0x00ff,0x003f,0x000f,0x0003,0x0000,0x000f,0x0c03,0x3c03,
       0xff03,0xffff,0xffff,0xffff,0xffff,0xffff,

       0xf000,0xcc00,0xc300,0xc0c0,0xc030,0xc00c,0xc03f,0xcc30,0xf30c,0xc30c,
       0x00fc,0x0000,0x0000,0x0000,0x0000,0x0000
      };

    _BX=0;
    _CX=0;
    _DX=FP_OFF(CGAcursor);
    _ES=_DS;
    _AX=9;
    geninterrupt(0x33);
   }
 else
 //
 // Move EGA font into LATCH memory!
 //
 if (vid==EGA1 || vid==EGA2)
   {
    unsigned i,s=FP_SEG(&TEDCHAR),o=FP_OFF(&TEDCHAR);

    outport(GCindex,GCmode);
    for (i=0;i<4;i++)
      {
       outport(SCindex,SCmapmask | (1<<i)*256);
       movedata(s,o+i*0x400,0xaf00,0,0x400);
      }
   }
 else
 //
 // OR set the VGA palette
 //
 if (vid==VGA)	// set VGA palette
   {
    _BX=0;
    _CX=0x100;
    _DX=FP_OFF(&VGAPAL);
    _ES=FP_SEG(&VGAPAL);
    _AX=0x1012;
    geninterrupt(0x10);
   }
}


////////////////////////////////////////////////////////////////////
//
// Quit the fuck outta here!
//
////////////////////////////////////////////////////////////////////
void Quit(char *string)
{
 Unhook();
 setvideo(TEXT);
 poke(0,0x41a,peek(0,0x41c));	// clear keyboard
 if (string[0])
   {
    printf("TED5 ERROR: %s",string);
    exit(1);
   }
 printf("Thanks for using TED5!");
 nosound();
 exit(0);
}


//////////////////////////
//
// drawwindow
// draws a bordered window and homes the cursor
//
//////////////////////////

void drawwindow (int xl, int yl, int xh, int yh)
{
 int x,y;
 win_xl=xl;
 win_yl=yl;
 win_xh=xh;
 win_yh=yh;		// so the window can be erased

 drawchar (xl,yl,1);
 for (x=xl+1;x<xh;x++)
   drawchar (x,yl,2);
 drawchar (xh,yl,3);
 for (y=yl+1;y<yh;y++)
 {
   drawchar (xl,y,4);
   for (x=xl+1;x<xh;x++)
     drawchar (x,y,' ');
   drawchar (xh,y,5);
 }
 drawchar (xl,yh,6);
 for (x=xl+1;x<xh;x++)
   drawchar (x,yh,7);
 drawchar (xh,yh,8);

 sx = leftedge = xl+1;
 sy = yl+1;
}

////////////////////////////
//
// erasewindow
// clears out the last window and it's border to spaces
//
///////////////////////////

void erasewindow (void)
{
  bar (win_xl,win_yl,win_xh,win_yh,' ');
}

/////////////////////////////
//
// centerwindow
// Centers a drawwindow of the given size
//
/////////////////////////////

void centerwindow (int width, int height)
{
  int xl = screencenterx-width/2;
  int yl = screencentery-height/2;

  drawwindow (xl,yl,xl+width+1,yl+height+1);
}

///////////////////////////////
//
// expwin {h / v}
// Grows the window outward
//
///////////////////////////////
void expwinh (int width, int height)
{
  if (width > 2)
    expwinh (width-2,height);

  WaitVBL (1);
  centerwindow (width,height);
}

void expwinv (int width, int height)
{
  if (height >2)
    expwinv (width,height-2);

  WaitVBL (1);
  centerwindow (width,height);
}
void expwin (int width, int height)
{
  if (width > 2)
  {
    if (height >2)
      expwin (width-2,height-2);
    else
      expwinh (width-2,height);
  }
  else
    if (height >2)
      expwinv (width,height-2);

  WaitVBL (1);
  centerwindow (width,height);
}


////////////////////////////////////////////////////////////
//
// Save a *LARGE* file from a FAR buffer!
// by John Romero (C) 1990 Gamer's Edge
//
////////////////////////////////////////////////////////////
void SaveFile(char *filename,char huge *buffer,long offset,long size)
{
 unsigned int handle,buf1,buf2,offlo,offhi,sizelo,sizehi;

 buf1=FP_OFF(buffer);
 buf2=FP_SEG(buffer);
 offlo=offset&0xffff;
 offhi=offset>>16;
 sizelo=size&0xffff;
 sizehi=size>>16;

asm		mov	ax,offlo
asm		or	ax,offhi
asm		jz	CREATEIT

asm		mov	dx,filename
asm		mov	ax,3d02h		// OPEN w/handle (read only)
asm		int	21h
asm		jnc	L0

 return;

L0:

asm		mov	handle,ax

asm		mov	bx,handle
asm		mov     dx,offlo
asm		mov	cx,offhi
asm		mov	ax,4200h
asm		int	21h			// SEEK (to file offset)
asm		jc	out

asm		jmp	DOSAVE

CREATEIT:

asm		mov	dx,filename
asm		mov	ax,3c00h		// CREATE w/handle (read only)
asm		xor	cx,cx
asm		int	21h
asm		jc	out

asm		mov	handle,ax

DOSAVE:

asm		cmp	WORD PTR sizehi,0		// larger than 1 segment?
asm		je	L2

L1:

asm		push	ds
asm		mov	bx,handle
asm		mov	cx,8000h
asm		mov	dx,buf1
asm		mov	ax,buf2
asm		mov	ds,ax
asm		mov	ah,40h			// WRITE w/handle
asm		int	21h
asm		pop	ds

asm		add	WORD PTR buf2,800h		// bump ptr up 1/2 segment
asm		sub	WORD PTR sizelo,8000h		// done yet?
asm		sbb	WORD PTR sizehi,0
asm		cmp	WORD PTR sizehi,0
asm		ja	L1
asm		cmp	WORD PTR sizelo,8000h
asm		jae	L1

L2:

asm		push	ds
asm		mov	bx,handle
asm		mov	cx,sizelo
asm		mov	dx,buf1
asm		mov	ax,buf2
asm		mov	ds,ax
asm		mov	ah,40h			// WRITE w/handle
asm		int	21h
asm		pop	ds

out:

asm		mov	bx,handle		// CLOSE w/handle
asm		mov	ah,3eh
asm		int	21h

}

////////////////////////////////////////////////////////////
//
// Load a *LARGE* file into a FAR buffer!
// by John Romero (C) 1990 Gamer's Edge
//
////////////////////////////////////////////////////////////
unsigned long LoadFile(char *filename,char huge *buffer,long offset,long size)
{
 unsigned handle,flength1=0,flength2=0,buf1,buf2,len1,len2,
	  rsize1,rsize2,roffset1,roffset2;

 rsize1=size&0xffff;
 rsize2=size>>16;
 roffset1=offset&0xffff;
 roffset2=offset>>16;
 buf1=FP_OFF(buffer);
 buf2=FP_SEG(buffer);

asm		mov	dx,filename
asm		mov	ax,3d00h		// OPEN w/handle (read only)
asm		int	21h
asm		jnc	L_0

 return 0;

L_0:

asm		mov	handle,ax
asm		mov	bx,ax
asm		xor	cx,cx
asm		xor	dx,dx
asm		mov	ax,4202h
asm		int	21h			// SEEK (find file length)
asm		jc	out

asm		mov	flength1,ax
asm		mov	len1,ax
asm		mov	flength2,dx
asm		mov	len2,dx

asm		mov	ax,rsize1		// was a size specified?
asm		or	ax,rsize1
asm		jz	LOADALL

asm		mov	ax,rsize1		// only load size requested
asm		mov	len1,ax
asm		mov	ax,rsize2
asm		mov	len2,ax

LOADALL:

asm		mov	bx,handle
asm		mov     dx,roffset1
asm		mov	cx,roffset2
asm		mov	ax,4200h
asm		int	21h			// SEEK (to file offset)
asm		jc	out

asm		cmp	WORD PTR len2,0			// MULTI-SEGMENTAL?
asm		je      L_2

L_1:

asm		push	ds
asm		mov	bx,handle
asm		mov	cx,8000h		// read 32K chunks
asm		mov	dx,buf1
asm		mov	ax,buf2
asm		mov	ds,ax
asm		mov	ah,3fh			// READ w/handle
asm		int	21h
asm		pop	ds
asm		jc	out

asm		add	WORD PTR buf2,800h
asm		sub	WORD PTR len1,8000h
asm		sbb	WORD PTR len2,0
asm		cmp	WORD PTR len2,0
asm		ja	L_1
asm		cmp	WORD PTR len1,8000h
asm		jae	L_1

L_2:

asm		push	ds
asm		mov	bx,handle
asm		mov	cx,len1
asm		mov	dx,buf1
asm		mov	ax,buf2
asm		mov	ds,ax
asm		mov	ah,3fh			// READ w/handle
asm		int	21h
asm		pop	ds
asm		jmp	exit

out:

asm		mov	WORD PTR flength2,0
asm		mov	WORD PTR flength1,0

exit:

asm		mov	bx,handle		// CLOSE w/handle
asm		mov	ah,3eh
asm		int	21h

 return (flength2*0x10000+flength1);

}


////////////////////////////////////////////////////////////
//
// Allocate memory and load file in
//
////////////////////////////////////////////////////////////
void LoadIn(char *filename,void _seg **baseptr)
{
 char errstr[80];
 int handle;
 long len;

 if ((handle=open(filename,O_BINARY))==-1)
   {
    strcpy(errstr,"Error loading file ");
    strcat(errstr,filename);
    Quit(errstr);
   }

 len=filelength(handle);
 close(handle);
 MMAllocate((memptr *)baseptr,len);
 LoadFile(filename,MK_FP(*baseptr,0),0,0);
}


