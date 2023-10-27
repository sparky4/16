/////////////////////////////////////////////////////////////////
//
// Pull-Down Menu Interface
// by John Romero (C) 1991 Id Software
//
/////////////////////////////////////////////////////////////////
#include "ted5.h"
#pragma hdrstop

#define NUMFLASHES	10
#define ALT		0x38
#define CTRL		0x1d

#define CGASIZE		0x4000
#define EGA1SIZE	0x2000
#define EGA2SIZE	0x9600
#define EGA3SIZE	60000
#define VGASIZE		64000

void (*HookRoutine)(int x,int y);
void (*ItemRoutine)(void);
char MenuStr[20][80],tempstr[80];
memptr Background[10];
int WhichBack;
struct { int savex,savey,savew,saveh;  } Back[10];
MBarDef *MBarPtr;
MInfoType MenuInfo[10];
int ScreenWidth,OpenMenu,NumMenus,ItemOn,KeybdOn;

void HandleOpenMenu(void (*UserRoutine)(void),int which);
void HandleCloseMenu(void (*UserRoutine)(void));
void HandleHighlight(void (*UserRoutine)(void));
int  DetectMenu(int x,int y);
void ChangeItem(int newitem);
void ClearScreen(void);
void RedrawDesktop(void);
char *PassScancode(int sc);

/////////////////////////////////////////////////////////////////
//
// Desktop Event Loop
//
/////////////////////////////////////////////////////////////////
void DeskEventLoop(void (*UserRoutine)(void),void (*ConstantRoutine)(void))
{
 int buttonstatus=0,oldmenu,olditem;
 enum clicks {upup,updown,downup,downdown};

 if (KeybdOn)
   {
    HandleOpenMenu(UserRoutine,2);
    ChangeItem(1);
   }

 MouseShow();
 while(1)
   {
    int temp=MouseButton()&1,temp1=(MouseButton()>>1)&1;

    sx=0;
    sy=3;

    if (!OpenMenu)
      ConstantRoutine();
    switch(buttonstatus=((buttonstatus<<1)|temp|temp1)&3)
      {
       case upup: break;
       case updown: HandleOpenMenu(UserRoutine,0);
		    break;
       case downdown: HandleHighlight(UserRoutine);
		      break;
       case downup: HandleCloseMenu(UserRoutine);
		    if (ItemOn && ItemRoutine)
		      ItemRoutine();
		    ItemOn=0;
		    break;
      }

    if (bioskey(1))
      {
       char key=bioskey(1)>>8;
       int nitems;

       if (OpenMenu)
	 {
	  bioskey(0);

	  nitems=(MBarPtr+OpenMenu-1)->num_items;

	  switch(key)
	    {
	     case 0x48: if (OpenMenu)
			  if (ItemOn)
			    if (ItemOn==1)
			      ChangeItem(nitems);
			    else
			      ChangeItem(ItemOn-1);
			break;
	     case 0x50: if (OpenMenu)
			  if (ItemOn)
			    if (ItemOn==nitems)
			      ChangeItem(1);
			    else
			      ChangeItem(ItemOn+1);
			break;
	     case 0x1c: oldmenu=OpenMenu;
			olditem=ItemOn;
			HandleCloseMenu(UserRoutine);
			if (ItemOn)
			  ItemRoutine();

			break;
	     case 0x4b: if (OpenMenu)
			  {
			   int newmenu;

			   ItemOn=0;
			   if (OpenMenu==1)
			     newmenu=NumMenus;
			   else
			     newmenu=OpenMenu-1;

			   HandleCloseMenu(UserRoutine);
			   HandleOpenMenu(UserRoutine,newmenu);

			   ChangeItem(1);
			  }
			break;
	     case 0x4d: if (OpenMenu)
			  {
			   int newmenu;

			   ItemOn=0;
			   if (OpenMenu==NumMenus)
			     newmenu=1;
			   else
			     newmenu=OpenMenu+1;

			   HandleCloseMenu(UserRoutine);
			   HandleOpenMenu(UserRoutine,newmenu);

			   ChangeItem(1);
			  }
			break;
	     case 0x01: if (!OpenMenu)
			  {
			   HandleOpenMenu(UserRoutine,oldmenu);
			   ChangeItem(olditem);
			  }
			break;
	    }
	 }
       else
	 {
	  int i,j,numitems,run=0;
	  MenuDef *items;

	  for (i=0;i<NumMenus;i++)
	    {
	     numitems=(MBarPtr+i)->num_items;
	     items=(MBarPtr+i)->menu_def;

	     for (j=0;j<numitems;j++)
	       if ((items+j)->hotkey==key && (items+j)->hotkey)
		 {
		  if (((items+j)->shiftflag) &&
		      (!keydown[(items+j)->shiftflag]))
		    continue;

		  ItemRoutine=(items+j)->routine;
		  ItemRoutine();
		  run=1;
		  break;
		 }
	     if (run)
	       {
		if (bioskey(1))
		  bioskey(0);
		break;
	       }
	    }
	  if (!run)
	    UserRoutine();
	 }
      }
   }
}



/////////////////////////////////////////////////////////////////
//
// See if a menu was opened
//
/////////////////////////////////////////////////////////////////
void HandleOpenMenu(void (*UserRoutine)(void),int which)
{
 int x,y,loop,flag,tempx,tempw,maxw,loopsize;
 MenuDef *items;

 if (!which)
   {
    MouseCoords(&x,&y);
    if (y>8 && !OpenMenu)
      {
       UserRoutine();
       return;
      }

    flag=DetectMenu(x,y);
    if (!flag) return;
   }
 else
   flag=which;


 tempx=MenuInfo[flag-1].menux;
 tempw=MenuInfo[flag-1].menuwidth;
 items=(MBarPtr+flag-1)->menu_def;

 sx=tempx+1;
 sy=0;
 maxw=0;

 MouseHide();


 //
 // BUILD MENU STRINGS
 //
 loopsize=(MBarPtr+flag-1)->num_items;

 for (loop=0;loop<loopsize;loop++)
   {
    int len;

    memset(MenuStr[loop],0,80);

    MenuStr[loop][0]=4;	// leftedge
    strcat(MenuStr[loop],(items+loop)->item_name);

    len=strlen(MenuStr[loop]);
    memset(&MenuStr[loop][len],' ',tempw-len+1);

    switch((items+loop)->shiftflag)
      {
       case ALT: strcat(MenuStr[loop]," ALT-"); break;
       case CTRL:strcat(MenuStr[loop],"CTRL-");
      }

    strcat(MenuStr[loop],PassScancode((items+loop)->hotkey));

    len=strlen(MenuStr[loop]);
    if (len>maxw)
      maxw=len;
   }

 if (tempx+maxw>ScreenWidth)
   tempx=ScreenWidth-1-maxw;
 SaveBackground(tempx*8,0,(maxw+1)*8,(MBarPtr+flag-1)->num_items*8+16);

 //
 // PRINT MENU STRINGS
 //
 xormask=0xffff;
 print((MBarPtr+flag-1)->menu_name);
 xormask=0;

 for (loop=0;loop<loopsize;loop++)
   {
    int len;

    len=strlen(MenuStr[loop]);
    if (len<maxw)
      memset(&MenuStr[loop][len],' ',maxw-len);
    strcat(MenuStr[loop],"\x5");

    sx=tempx;
    sy=loop+1;
    print(MenuStr[loop]);
   }

  sx=tempx;
  sy=loopsize+1;
  print("\x6");
  bar(sx,sy,sx+maxw-2,sy,7);
  sx=tempx+maxw;
  print("\x8");

 MouseShow();
 OpenMenu=flag;
 ItemOn=0;
}



/////////////////////////////////////////////////////////////////
//
// Detect whether a menu is clicked on or not
//
/////////////////////////////////////////////////////////////////
int DetectMenu(int x,int y)
{
 int flag,loop;

 if (y>10)
   return 0;

 flag=0;
 for (loop=0;loop<NumMenus;loop++)
   if (x/8>=MenuInfo[loop].menux && x/8<MenuInfo[loop].menux+MenuInfo[loop].menunamelen)
     {
      flag=loop+1;
      break;
     }
 return flag;
}



/////////////////////////////////////////////////////////////////
//
// Highlight & De-highlight items
//
/////////////////////////////////////////////////////////////////
void HandleHighlight(void (*UserRoutine)(void))
{
 int x,y,nitems,tempx,tempw,tempy,opmen;

 if (!OpenMenu)
   {
    UserRoutine();
    return;
   }

 MouseCoords(&x,&y);
 nitems=(MBarPtr+OpenMenu-1)->num_items;
 tempx=MenuInfo[OpenMenu-1].menux;
 tempw=strlen(MenuStr[0])-5;
 if (tempx+tempw+5>ScreenWidth)
   tempx=ScreenWidth-tempw-5;

 opmen=DetectMenu(x,y);
 if (opmen && opmen!=OpenMenu)
   {
    OpenMenu=0;
    RestoreBackground();
    HandleOpenMenu(UserRoutine,0);
    return;
   }

 //
 // IS USER IN A MENU?
 //

 if (x/8>tempx && x/8<tempx+tempw+4 && y>8 && y<(nitems+1)*8)
   {
    if (y/8==ItemOn)
      return;	// EXIT IF ON SAME ITEM

    //
    // IF AN ITEM IS CURRENTLY SELECTED, DEHIGHLIGHT IT
    // AND HIGHLIGHT A NEW ITEM
    //

    ChangeItem(y/8);
   }
 else
   //
   // USER MOVED POINTER OUTSIDE OF MENU; DEHIGHLIGHT ITEM
   //

   if (ItemOn)
     {
      MouseHide();

      sx=tempx;
      sy=ItemOn;
      print(MenuStr[ItemOn-1]);

      ItemOn=0;
      MouseShow();
     }
}



/////////////////////////////////////////////////////////////////
//
// Set new item highlighted
//
/////////////////////////////////////////////////////////////////
void ChangeItem(int newitem)
{
 int tempx,tempw;
 MenuDef *items;
 char tempstr[80]="";

 tempx=MenuInfo[OpenMenu-1].menux;
 if (tempx+strlen(MenuStr[0])>ScreenWidth)
   tempx=ScreenWidth-strlen(MenuStr[0]);

 MouseHide();
 if (ItemOn)
   {
    sx=tempx;
    sy=ItemOn;
    print(MenuStr[ItemOn-1]);
   }

 xormask=0xffff;
 ItemOn=newitem;
 strncpy(tempstr,&MenuStr[ItemOn-1][1],strlen(MenuStr[ItemOn-1])-2);
 sx=tempx+1;
 sy=ItemOn;
 print(tempstr);
 xormask=0;

 MouseShow();
}



/////////////////////////////////////////////////////////////////
//
// See if a menu was closed
//
/////////////////////////////////////////////////////////////////
void HandleCloseMenu(void (*UserRoutine)(void))
{
 MenuDef *items;
 int loop,tempx;
 char tempstr[80]="";

 if (!OpenMenu)
   {
    UserRoutine();
    return;
   }

 if (ItemOn)
   {
    items=(MBarPtr+OpenMenu-1)->menu_def;
    tempx=MenuInfo[OpenMenu-1].menux;
    if (tempx+strlen(MenuStr[0])>ScreenWidth)
      tempx=ScreenWidth-strlen(MenuStr[0]);

    for (loop=0;loop<NUMFLASHES;loop++)
      {
       xormask^=0xffff;
       MouseHide();

       strncpy(tempstr,&MenuStr[ItemOn-1][1],strlen(MenuStr[ItemOn-1])-2);
       sx=tempx+1;
       sy=ItemOn;
       print(tempstr);

       MouseShow();
       WaitVBL(3);
      }
    xormask=0;
    ItemRoutine=(items+ItemOn-1)->routine;
   }

 OpenMenu=0;
 RestoreBackground();
}


/////////////////////////////////////////////////////////////////
//
// Save the background
//
/////////////////////////////////////////////////////////////////
void SaveBackground(int x,int y,int w,int h)
{
 long size;
 unsigned loc,loop,loop1,seg,planelen;

 Back[WhichBack].savex=x;
 Back[WhichBack].savey=y;
 Back[WhichBack].savew=w;
 Back[WhichBack].saveh=h;

 MouseHide();
 switch (videomode)
   {
    case CGA:
      MMAllocate(&Background[WhichBack],(w/4)*h);

      for(loop=y;loop<y+h;loop++)
	{
	 loc=(loop/2)*80+0x2000*(loop&1)+x/4;
	 movedata(0xb800,loc,(unsigned)Background[WhichBack],(loop-y)*(w/4),w/4);
	}
      break;

    case EGA2:
    case EGA1:
      {
       unsigned tempw=w/8,tempx=x/8,psize;

       planelen=tempw*h;
       MMAllocate(&Background[WhichBack],5L*planelen);
       outport(GCindex,GCmode);
       for (loop=0;loop<4;loop++)
	 {
	  psize=loop*planelen;

	  outport(GCindex,GCreadmap | loop*256);
	  for (loop1=y;loop1<y+h;loop1++)
	    movedata(0xa000,((unsigned)loop1*ScreenWidth)+tempx,
	      ((unsigned)Background[WhichBack])+(psize/16),
	      (psize&15)+((loop1-y)*tempw),tempw);
	 }
      }
      break;

    case VGA:
      MMAllocate(&Background[WhichBack],(long)w*h);
      for(loop=y;loop<y+h;loop++)
	movedata(0xa000,loop*320+x,(unsigned)Background[WhichBack],(loop-y)*w,w);
   }
 MouseShow();
 if (WhichBack<10)
   WhichBack++;
}



/////////////////////////////////////////////////////////////////
//
// Restore the background
//
/////////////////////////////////////////////////////////////////
void setfarmem(char huge *mem,unsigned char val,unsigned len)
{
 int i;
 for (i=0;i<len;i++)
   *(mem+i)=val;
}

void RestoreBackground(void)
{
 long size;
 unsigned loc,loop1,x,y,w,h,loop,planelen;

 if (!Back[WhichBack-1].savex &&
     !Back[WhichBack-1].savey &&
     !Back[WhichBack-1].savew &&
     !Back[WhichBack-1].saveh)
   Quit("Can't RestoreBackground with no information!");

 x=Back[WhichBack-1].savex;
 y=Back[WhichBack-1].savey;
 w=Back[WhichBack-1].savew;
 h=Back[WhichBack-1].saveh;

 MouseHide();
 switch (videomode)
   {
    case CGA:
      for(loop=y;loop<y+h;loop++)
	{
	 loc=(loop/2)*80+0x2000*(loop&1)+x/4;
	 movedata((unsigned)Background[WhichBack-1],(loop-y)*(w/4),0xb800,loc,w/4);
	}
      break;

    case EGA2:
    case EGA1:
      planelen=(w/8)*h;
      outport(GCindex,GCmode);
      for (loop=0;loop<4;loop++)
	{
	 outport(SCindex,SCmapmask | (1<<loop)*256);
	 for (loop1=y;loop1<y+h;loop1++)
	   movedata(((unsigned)Background[WhichBack-1])+((loop*planelen)/16),
	     ((loop*planelen)&15)+(loop1-y)*(w/8),
	     0xa000,loop1*ScreenWidth+x/8,w/8);
	}
      break;

    case VGA:
      for(loop=y;loop<y+h;loop++)
	movedata((unsigned)Background[WhichBack-1],(loop-y)*w,0xa000,loop*320+x,w);
   }

 MMFreePtr(&Background[WhichBack-1]);

 MouseShow();

 WhichBack--;
 Back[WhichBack].savex=0;
 Back[WhichBack].savey=0;
 Back[WhichBack].savew=0;
 Back[WhichBack].saveh=0;
}


/////////////////////////////////////////////////////////////////
//
// Draw the Desktop
//
/////////////////////////////////////////////////////////////////
void RedrawDesktop(void)
{
 int flag,count,i;

 ClearScreen();

 sx=sy=0;
 bar(0,0,ScreenWidth-1,0,' ');

 flag=0;
 count=0;
 NumMenus=0;
 while((MBarPtr+count)->num_items && !flag)
   {
    int len,max_width,loop;
    MenuDef *the_item;
    char string[80];

    //
    // First, determine xcoord & namewidth & print
    //

    strcpy(string,(MBarPtr+count)->menu_name);
    len=strlen(string);
    MenuInfo[NumMenus].menux=sx;
    MenuInfo[NumMenus].menunamelen=len+2;
    if (len+sx>ScreenWidth-1)
      {
       string[ScreenWidth-1-sx]=0;
       MenuInfo[NumMenus].menunamelen=ScreenWidth-1-sx;
       MenuInfo[NumMenus].menux=ScreenWidth-MenuInfo[NumMenus].menunamelen;
       flag++;
      }

    drawchar(sx++,sy,' ');
    print(string);
    drawchar(sx++,sy,' ');

    //
    // Now, figure out length of widest item
    //

    max_width=0;
    the_item=(MBarPtr+count)->menu_def;

    for (loop=0;loop<(MBarPtr+count)->num_items;loop++)
      {
       int len;

       len=strlen((the_item+loop)->item_name);
       if (len>max_width)
	 max_width=len;
      }

    MenuInfo[NumMenus].menuwidth=max_width+1;

    count++;
    NumMenus++;
   }

 //
 // clear bottom line of menubar
 //
 switch(videomode)
 {
  case CGA:
    {
     unsigned huge *CGAmem=MK_FP(0xb800,240+0x2000);
     for(i=0;i<40;i++)
       *(CGAmem+i)=0;
    }
    break;
  case EGA1:
  case EGA2:
    {
    char huge *EGAmem=MK_FP(0xa000,0);
    outport(GCindex,GCmode);
    outport(SCindex,0x0f00 | SCmapmask);
    for (i=0;i<ScreenWidth;i++)
      *(EGAmem+7*ScreenWidth+i)=0;
    }
    break;
  case VGA:
    {
    unsigned huge *VGAmem=MK_FP(0xa000,320*7);
    for (i=0;i<160;i++)
      *(VGAmem+i)=0;
    }
 }
}



/////////////////////////////////////////////////////////////////
//
// Init Desktop
//
/////////////////////////////////////////////////////////////////
void InitDesktop(MBarDef *menubar,int initmouse)
{
 switch(videomode)
   {
    case CGA: ScreenWidth=40; break;
    case EGA1: ScreenWidth=40; break;
    case EGA2: ScreenWidth=80; break;
    case VGA: ScreenWidth=40;
   }

 MBarPtr=menubar;
 RedrawDesktop();
 if (initmouse)
   MouseInit();
 OpenMenu=KeybdOn=0;
 if (!MouseStatus)
   KeybdOn=1;
}




////////////////////////////////////////////////////////////////////
//
// Clear the current screen
//
////////////////////////////////////////////////////////////////////
void ClearScreen(void)
{
 unsigned size,segment;

 segment=0xa000;
 switch(videomode)
   {
    case CGA: segment=0xb800; size=CGASIZE/2; break;
    case EGA1: size=EGA1SIZE/2; break;
    case EGA2: size=EGA2SIZE/2; break;
    case VGA: size=VGASIZE/2;
   }

 MouseHide();

 asm	push	di
 asm	push	es
 asm	xor	di,di
 asm	mov	ax,segment
 asm	mov	es,ax
 asm	mov	cx,size
 asm	cld
 asm	xor	ax,ax
 asm	rep stosw
 asm	pop	es
 asm	pop	di

 MouseShow();
}



/////////////////////////////////////////////////////////
//
// print a representation of the scan code key
//
/////////////////////////////////////////////////////////
char *PassScancode(int sc)
{
 char smallstr[2];
 char static chartable[128] =
 {' ',' ','1','2','3','4','5','6','7','8','9','0','-','+','?','?',
  'Q','W','E','R','T','Y','U','I','O','P','[',']','|','?','A','S',
  'D','F','G','H','J','K','L',';','"',' ',' ',' ','Z','X','C','V',
  'B','N','M',',','.','/','?',' ',' ',' ',' ',' ',' ',' ',' ',' ',
  ' ',' ',' ',' ',' ',' ',' ',' ', 15,' ','-', 14,'5', 31,'+',' ',
   19,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
  ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
  ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};

 sc = sc & 0x7f;

 if (sc==1)
   strcpy(tempstr,"ESC");
 else if (sc==0x48)
   strcpy(tempstr,"\xb");
 else if (sc==0x50)
   strcpy(tempstr,"\xc");
 else if (sc==0xe)
   strcpy(tempstr,"BKSP");
 else if (sc==0xf)
   strcpy(tempstr,"TAB");
 else if (sc==0x1d)
   strcpy(tempstr,"CTRL");
 else if (sc==0x2A)
   strcpy(tempstr,"LSHIFT");
 else if (sc==0x39)
   strcpy(tempstr,"SPACE");
 else if (sc==0x3A)
   strcpy(tempstr,"CAPSLK");
 else if (sc>=0x3b && sc<=0x44)
 {
   char str[3];
   strcpy(tempstr,"F");
   itoa (sc-0x3a,str,10);
   strcat(tempstr,str);
 }
 else if (sc==0x57)
   strcpy(tempstr,"F11");
 else if (sc==0x59)
   strcpy(tempstr,"F12");
 else if (sc==0x46)
   strcpy(tempstr,"SCRLLK");
 else if (sc==0x1c)
   strcpy(tempstr,"ENTER");
 else if (sc==0x36)
   strcpy(tempstr,"RSHIFT");
 else if (sc==0x37)
   strcpy(tempstr,"PRTSC");
 else if (sc==0x38)
   strcpy(tempstr,"ALT");
 else if (sc==0x47)
   strcpy(tempstr,"HOME");
 else if (sc==0x49)
   strcpy(tempstr,"PGUP");
 else if (sc==0x4f)
   strcpy(tempstr,"END");
 else if (sc==0x51)
   strcpy(tempstr,"PGDN");
 else if (sc==0x52)
   strcpy(tempstr,"INS");
 else if (sc==0x53)
   strcpy(tempstr,"DEL");
 else if (sc==0x45)
   strcpy(tempstr,"NUMLK");
 else
   {
    smallstr[0]=chartable[sc];
    smallstr[1]=0;
    strcpy(tempstr,smallstr);
   }

 return tempstr;
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
//
//
// DIALOG MANAGER CODE
//
//
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
//
// Dialog Boxes!
//
/////////////////////////////////////////////////////////
int DoDialog(DialogDef *TheDialog)
{
 btype *TheButton;
 int i,ox,oy,Float=0,Released=0,Clicked=0,xc[30],yc[30],wid[30];

 for (i=0;i<30;i++)
   xc[i]=yc[i]=wid[i]=0;

 MouseHide();
 SaveBackground((screencenterx-TheDialog->width/2)*8,
   (screencentery-TheDialog->height/2)*8,(TheDialog->width+2)*8,
   (TheDialog->height+2)*8);

 xormask=0;
 centerwindow(TheDialog->width,TheDialog->height);
 ox=sx;
 oy=sy;
 print(TheDialog->text);
 for (i=0;i<TheDialog->numbuttons;i++)
   {
    int xx,yy,j;

    TheButton=TheDialog->buttons;

    xc[i]=sx=ox+(TheButton+i)->xoff;
    yc[i]=sy=oy+(TheButton+i)->yoff;
    xx=sx-1;
    yy=sy-1;
    print((TheButton+i)->text);
    wid[i]=strlen((TheButton+i)->text);

    if ((TheButton+i)->border)
      DrawBorder(xx,yy,wid[i]+1,2,(TheButton+i)->border);
   }

 if (TheDialog->hook)
   {
    HookRoutine=(void (*)(int x,int y))TheDialog->hook;
    HookRoutine(ox,oy);
   }
 MouseShow();

 clearkeys();
 do
 {
  char temp;
  int mx,my;

  temp=((temp<<1)|(MouseButton()&1))&3;
  MouseCoords(&mx,&my);
  mx/=8;
  my/=8;

  //
  // ENTER press
  //
  if (keydown[0x1c])
    for(i=0;i<TheDialog->numbuttons;i++)
      {
       TheButton=TheDialog->buttons;
       if ((TheButton+i)->border==2)
	 {
	  Clicked=i+1;
	  Released=1;
	  temp=Float=0;
	  while(keydown[0x1c]);
	  clearkeys();
	 }
      }

  //
  // ESC press
  //
  if (keydown[1])
    {
     temp=Float=Clicked=0;
     Released=1;
     while(keydown[1]);
    }

  switch(temp)
  {
   case 0: // upup (no press)
     break;
   case 3: // downdown (held down)
     if (!Float && Clicked && (mx<xc[Clicked-1] ||
	 mx>xc[Clicked-1]+wid[Clicked-1]-1 || my!=yc[Clicked-1]))
       {
	xormask=0;
	sx=xc[Clicked-1];
	sy=yc[Clicked-1];
	MouseHide();
	print((TheButton+Clicked-1)->text);
	MouseShow();
	xormask=1;
	Float=1;
       }
     else
     if (Float && mx>=xc[Clicked-1] &&
	 mx<xc[Clicked-1]+wid[Clicked-1] && my==yc[Clicked-1])
       {
	xormask=1;
	sx=xc[Clicked-1];
	sy=yc[Clicked-1];
	MouseHide();
	print((TheButton+Clicked-1)->text);
	MouseShow();
	xormask=0;
	Float=0;
       }
     break;
   case 1: // updown (press)
     for (i=0;i<TheDialog->numbuttons;i++)
       {
	if (mx>=xc[i] && mx<xc[i]+wid[i] && my==yc[i])
	  {
	   Clicked=i+1;
	   xormask=1;
	   sx=xc[i];
	   sy=yc[i];
	   MouseHide();
	   print((TheButton+i)->text);
	   MouseShow();
	   xormask=0;
	   break;
	  }
       }
     break;
   case 2: // downup (release)
     if (Clicked && !Float)
       Released++;
  }
 } while (!Released);

 RestoreBackground();
 return Clicked;
}


/////////////////////////////////////////////////////////
//
// Just CHECK a Dialog Box's BUTTONS
//
/////////////////////////////////////////////////////////
int CheckButtons(DialogDef *TheDialog)
{
 btype *TheButton;
 int i,ox,oy,Float=0,Released=0,Clicked=0,xc[30],yc[30],wid[30];

 for (i=0;i<30;i++)
   xc[i]=yc[i]=wid[i]=0;

 ox=screencenterx-TheDialog->width/2+1;
 oy=screencentery-TheDialog->height/2+1;
 for (i=0;i<TheDialog->numbuttons;i++)
   {
    int xx,yy,j;

    TheButton=TheDialog->buttons;

    xc[i]=ox+(TheButton+i)->xoff;
    yc[i]=oy+(TheButton+i)->yoff;
    wid[i]=strlen((TheButton+i)->text);
   }


 clearkeys();
 do
 {
  char temp;
  int mx,my;

  temp=((temp<<1)|(MouseButton()&1))&3;
  MouseCoords(&mx,&my);
  mx/=8;
  my/=8;

  //
  // ENTER press
  //
  if (keydown[0x1c])
    for(i=0;i<TheDialog->numbuttons;i++)
      {
       TheButton=TheDialog->buttons;
       if ((TheButton+i)->border==2)
	 {
	  Clicked=i+1;
	  Released=1;
	  temp=Float=0;
	  while(keydown[0x1c]);
	  clearkeys();
	 }
      }

  //
  // ESC press
  //
  if (keydown[1])
    {
     temp=Float=Clicked=0;
     Released=1;
     while(keydown[1]);
    }

  switch(temp)
  {
   case 0: // upup (no press)
     break;
   case 3: // downdown (held down)
     if (!Float && Clicked && (mx<xc[Clicked-1] ||
	 mx>xc[Clicked-1]+wid[Clicked-1]-1 || my!=yc[Clicked-1]))
       {
	xormask=0;
	sx=xc[Clicked-1];
	sy=yc[Clicked-1];
	MouseHide();
	print((TheButton+Clicked-1)->text);
	MouseShow();
	xormask=1;
	Float=1;
       }
     else
     if (Float && mx>=xc[Clicked-1] &&
	 mx<xc[Clicked-1]+wid[Clicked-1] && my==yc[Clicked-1])
       {
	xormask=1;
	sx=xc[Clicked-1];
	sy=yc[Clicked-1];
	MouseHide();
	print((TheButton+Clicked-1)->text);
	MouseShow();
	xormask=0;
	Float=0;
       }
     break;
   case 1: // updown (press)
     for (i=0;i<TheDialog->numbuttons;i++)
       {
	if (mx>=xc[i] && mx<xc[i]+wid[i] && my==yc[i])
	  {
	   Clicked=i+1;
	   xormask=1;
	   sx=xc[i];
	   sy=yc[i];
	   MouseHide();
	   print((TheButton+i)->text);
	   MouseShow();
	   xormask=0;
	   break;
	  }
       }
     break;
   case 2: // downup (release)
     if (Clicked && !Float)
       Released++;
  }
 } while (!Released);
 clearkeys();

 if (Clicked)
 {
  sx=xc[Clicked-1];
  sy=yc[Clicked-1];
  MouseHide();
  print((TheButton+Clicked-1)->text);
  MouseShow();
 }

 return Clicked;
}


/////////////////////////////////////////////////////////
//
// Just CHECK a Dialog Box's BUTTONS
// BUT!...RETURN IF MOUSE BUTTON IS PRESSED OUTSIDE DIALOG BUTTON!
//
/////////////////////////////////////////////////////////
int CheckButtonsRet(DialogDef *TheDialog)
{
 btype *TheButton;
 int i,ox,oy,Float=0,Released=0,Clicked=0,xc[30],yc[30],wid[30];

 for (i=0;i<30;i++)
   xc[i]=yc[i]=wid[i]=0;

 ox=screencenterx-TheDialog->width/2+1;
 oy=screencentery-TheDialog->height/2+1;
 for (i=0;i<TheDialog->numbuttons;i++)
   {
    int xx,yy,j;

    TheButton=TheDialog->buttons;

    xc[i]=ox+(TheButton+i)->xoff;
    yc[i]=oy+(TheButton+i)->yoff;
    wid[i]=strlen((TheButton+i)->text);
   }

 do
 {
  char temp;
  int mx,my;

  temp=((temp<<1)|(MouseButton()&1))&3;
  if (MouseButton()&2)
    return -1;

  MouseCoords(&mx,&my);
  mx/=8;
  my/=8;

  //
  // ENTER press
  //
  if (keydown[0x1c])
    for(i=0;i<TheDialog->numbuttons;i++)
      {
       TheButton=TheDialog->buttons;
       if ((TheButton+i)->border==2)
	 {
	  Clicked=i+1;
	  Released=1;
	  temp=Float=0;
	  while(keydown[0x1c]);
	  clearkeys();
	 }
      }

  //
  // ESC press
  //
  if (keydown[1])
    {
     temp=Float=Clicked=0;
     Released=1;
     while(keydown[1]);
    }

  //
  // arrows or PgUp/PgDn/Home/End
  //
  if (keydown[0x48] || keydown[0x50] || keydown[0x4b] || keydown[0x4d] ||
      keydown[0x49] || keydown[0x51] || keydown[0x47] || keydown[0x4f] ||
      keydown[0x39] || keydown[0x2e])
    return -1;

  switch(temp)
  {
   case 0: // upup (no press)
     break;
   case 3: // downdown (held down)
     if (!Float && Clicked && (mx<xc[Clicked-1] ||
	 mx>xc[Clicked-1]+wid[Clicked-1]-1 || my!=yc[Clicked-1]))
       {
	xormask=0;
	sx=xc[Clicked-1];
	sy=yc[Clicked-1];
	MouseHide();
	print((TheButton+Clicked-1)->text);
	MouseShow();
	xormask=1;
	Float=1;
       }
     else
     if (Float && mx>=xc[Clicked-1] &&
	 mx<xc[Clicked-1]+wid[Clicked-1] && my==yc[Clicked-1])
       {
	xormask=1;
	sx=xc[Clicked-1];
	sy=yc[Clicked-1];
	MouseHide();
	print((TheButton+Clicked-1)->text);
	MouseShow();
	xormask=0;
	Float=0;
       }
     break;
   case 1: // updown (press)
     for (i=0;i<TheDialog->numbuttons;i++)
       {
	if (mx>=xc[i] && mx<xc[i]+wid[i] && my==yc[i])
	  {
	   Clicked=i+1;
	   xormask=1;
	   sx=xc[i];
	   sy=yc[i];
	   MouseHide();
	   print((TheButton+i)->text);
	   MouseShow();
	   xormask=0;
	   break;
	  }
       }

     if (!Clicked)	// MOD TO ORIGINAL CHECKBUTTONS
       return -1;

     break;
   case 2: // downup (release)
     if (Clicked && !Float)
       Released++;
  }
 } while (!Released);
 clearkeys();
 return Clicked;
}


/////////////////////////////////////////////////////////
//
// Just DRAW a Dialog Box
//
/////////////////////////////////////////////////////////
void DrawDialog(DialogDef *TheDialog,int saveback)
{
 btype *TheButton;
 int i,ox,oy,xc[30],yc[30],wid[30];

 for (i=0;i<30;i++)
   xc[i]=yc[i]=wid[i]=0;

 MouseHide();
 if (saveback)
   SaveBackground((screencenterx-TheDialog->width/2)*8,
     (screencentery-TheDialog->height/2)*8,(TheDialog->width+2)*8,
     (TheDialog->height+2)*8);

 xormask=0;
 centerwindow(TheDialog->width,TheDialog->height);
 ox=sx;
 oy=sy;
 print(TheDialog->text);
 for (i=0;i<TheDialog->numbuttons;i++)
   {
    int xx,yy,j;

    TheButton=TheDialog->buttons;

    xc[i]=sx=ox+(TheButton+i)->xoff;
    yc[i]=sy=oy+(TheButton+i)->yoff;
    xx=sx-1;
    yy=sy-1;
    print((TheButton+i)->text);
    wid[i]=strlen((TheButton+i)->text);

    if ((TheButton+i)->border)
      DrawBorder(xx,yy,wid[i]+1,2,(TheButton+i)->border);
   }

 if (TheDialog->hook)
   {
    HookRoutine=(void (*)(int x,int y))TheDialog->hook;
    HookRoutine(ox,oy);
   }

 MouseShow();
}


/////////////////////////////////////////////////////////
//
// Error Dialog Box
//
/////////////////////////////////////////////////////////
char errstring[200],bstring[20];
btype ERROKb={bstring,0,0,2};
DialogDef ERRR={errstring,0,0,1,&ERROKb,NULL};

void ErrDialog(char *string,char *bstr)
{
 int maxw=0,width=0,height=1,i;

 if (strlen(string)>200)
   Quit("Programmer Error: ErrDialog string is too long!");

 for (i=0;i<strlen(string);i++)
   {
    width++;
    if (width>maxw)
      maxw=width;

    if (string[i]=='\n')
      {
       height++;
       width=0;
      }
   }
 height+=3;	// add for button!

 if (strlen(bstr)>maxw)
   Quit("Programmer Error: ErrDialog BUTTONstring is longer than dialog!");

 strcpy(bstring,bstr);
 strcpy(errstring,string);
 ERRR.width=maxw;
 ERRR.height=height;
 ERRR.numbuttons=1;
 if (!bstr[0])
   {
    ERRR.numbuttons=0;
    ERRR.height-=3;
   }

 ERROKb.xoff=(maxw/2)-(strlen(bstr)/2);
 ERROKb.yoff=height-2;

 if (bstr[0])
   DoDialog(&ERRR);
 else
   DrawDialog(&ERRR,1);
}


/////////////////////////////////////////////////////////
//
// Draw a border
//
/////////////////////////////////////////////////////////
void DrawBorder(int x,int y,int w,int h,int b)
{
 int xx=x,yy=y,j;

 if (b==2)
   {
    drawchar(xx,yy,15);
    drawchar(xx+w,yy,17);
    drawchar(xx,yy+h,20);
    drawchar(xx+w,yy+h,22);

    for (j=yy+1;j<yy+h;j++)
      {
       drawchar(xx,j,18);
       drawchar(xx+w,j,19);
      }
    for (j=xx+1;j<xx+w;j++)
      {
       drawchar(j,yy,16);
       drawchar(j,yy+h,21);
      }
   }

 if (b==1)
   {
    drawchar(xx,yy,23);
    drawchar(xx+w,yy,25);
    drawchar(xx,yy+h,28);
    drawchar(xx+w,yy+h,30);

    for (j=yy+1;j<yy+h;j++)
      {
       drawchar(xx,j,26);
       drawchar(xx+w,j,27);
      }
    for (j=xx+1;j<xx+w;j++)
      {
       drawchar(j,yy,24);
       drawchar(j,yy+h,29);
      }
   }

}


/////////////////////////////////////////////////////////
//
// Get the XY coords of a dialog's button
//
/////////////////////////////////////////////////////////
void GetButtonXY(DialogDef *TheDialog,int button,unsigned *x,unsigned *y)
{
 btype *TheButton;

 TheButton=TheDialog->buttons;

 *x=(TheButton+button)->xoff+screencenterx-TheDialog->width/2+1;
 *y=(TheButton+button)->yoff+screencentery-TheDialog->height/2+1;
}


/////////////////////////////////////////////////////////
//
// Get the XY coords of a dialog
//
/////////////////////////////////////////////////////////
void GetDialogXY(DialogDef *TheDialog,unsigned *x,unsigned *y)
{
 *x=screencenterx-TheDialog->width/2+1;
 *y=screencentery-TheDialog->height/2+1;
}


////////////////////////////////////////////////////
//
// Allow user to select a list item (like the menus)
//
////////////////////////////////////////////////////
int CheckList(int x,int y,int w,int h,void (*oncode)(),void (*offcode)(),int blink)
{
 enum {upup,updown,downup,downdown} clicks;
 static char bpress=0;
 unsigned mx,my,i;
 int high=-1;


 while(1)
 {
  MouseCoords(&(int)mx,&(int)my);
  mx/=8;
  my/=8;
  bpress=((bpress<<1)|(MouseButton()&1))&3;
  switch(bpress)
  {
   case upup:
     return -1;
   case updown:
     if (mx>=x && mx<x+w && my>=y && my<y+h)
       {
	high=my-y;
	MouseHide();
	oncode(x,my,high);
	MouseShow();
       }
     else
       return -1;

     break;
   case downup:
     if (high!=-1)
       {
	if (blink)
	  for(i=0;i<5;i++)
	  {
	   MouseHide();
	   oncode(x,my,high);
	   MouseShow();
	   WaitVBL(3);

	   MouseHide();
	   offcode(x,my,high);
	   WaitVBL(3);
	   MouseShow();
	  }
       }
     return high;

   case downdown:
     if (mx>=x && mx<x+w && my>=y && my<y+h)
       {
	if (my-y != high && high!=-1)
	  {
	   MouseHide();
	   offcode(x,high+y,high);
	   MouseShow();

	  }

	if (my-y != high)
	  {
	   high=my-y;
	   MouseHide();
	   oncode(x,y+high,high);
	   MouseShow();
	  }
       }
     else
       {
	if (high!=-1)
	  {
	   MouseHide();
	   offcode(x,high+y,high);
	   MouseShow();
	  }
	high=-1;
	return -1;
       }
  }
 }
}


/////////////////////////////////////////////////
//
// Message dialog
// Returns: -1=ESC, 0=No, 1=Yes
//
/////////////////////////////////////////////////
char MessStr[200];
btype MessOKb[]={{" No ",0,0,1},{" Yes ",0,0,2}};
DialogDef Mess={MessStr,0,0,2,&MessOKb[0],NULL};

int Message(char *string)
{
 int maxw=0,width=0,height=1,i;

 if (strlen(string)>200)
   Quit("Programmer Error: Message string is too long!");

 for (i=0;i<strlen(string);i++)
   {
    width++;
    if (width>maxw)
      maxw=width;

    if (string[i]=='\n')
      {
       height++;
       width=0;
      }
   }
 height+=3;	// add for buttons!

 strcpy(MessStr,string);
 Mess.width=maxw;
 Mess.height=height;

 MessOKb[1].xoff=(maxw/4)-(strlen(MessOKb[0].text)/2);
 MessOKb[1].yoff=height-2;
 MessOKb[0].xoff=(maxw/4)*3-(strlen(MessOKb[1].text)/2);
 MessOKb[0].yoff=height-2;

 return DoDialog(&Mess);
}


/////////////////////////////////////////////////
//
// GetPath dialog
// *path is returned
// (exit:0 if OK,-1 if Not Successful,-2 if Canceled)
//
/////////////////////////////////////////////////
#define LISTX	1
#define LISTY	3

char dstr[80];
btype GPb={"Cancel",15,3,1};
DialogDef GPd={dstr,22,14,1,&GPb,NULL};
char NameList[MAXFDNAMES][13];
int base;
struct ffblk f;

int GetPath(char *string,char *filter,char *path)
{
 char pname[64];
 unsigned int numnames=0,max,dx,dy,redraw,exit;
 int select,zset,i;


 strcpy(pname,filter);
 for (zset=0,i=strlen(pname);i>=0;i--)
   if (pname[i]=='\\')
     {
      pname[i+1]=0;
      zset++;
      break;
     }
 if (!zset)
   pname[0]=0;

 strcpy(dstr,string);
 //
 // FIRST, GET THE NAMES FROM THE DIRECTORY
 //
 if (findfirst(filter,&f,FA_ARCH))
   return -1;
 strcpy(NameList[numnames++],f.ff_name);

 while(!findnext(&f) && numnames<MAXFDNAMES)
   strcpy(NameList[numnames++],f.ff_name);

 DrawDialog(&GPd,1);
 MouseHide();
 GetDialogXY(&GPd,&dx,&dy);
 DrawBorder(dx+LISTX-1,dy+LISTY-1,13,11,1);
 MouseShow();

 base=exit=0;

 //
 // THIS LOOP DRAWS THE DIALOG
 //
 do
 {
  redraw=0;
  MouseHide();
  max=10;
  if (numnames<10)
    max=numnames;
  for (i=0;i<max;i++)
    {
     bar(dx+LISTX,dy+LISTY+i,dx+LISTX+11,dy+LISTY+i,' ');
     FDoff(dx+LISTX,dy+LISTY+i,i);
    }
  MouseShow();

  //
  // THIS LOOP CHECKS INPUT
  //
  do
  {
   select=CheckList(dx+LISTX,dy+LISTY,12,max,FDon,FDoff,0);
   if (select>=0)
     {
      redraw=exit=1;
      continue;
     }

   GetButtonXY(&GPd,0,&sx,&sy);
   select=CheckList(sx,sy,strlen(GPb.text),1,CancelOn,CancelOff,0);
   if (!select || keydown[1])
     {
      RestoreBackground();
      while(keydown[1]);
      clearkeys();
      return -2;
     }

   //
   // CHECK ARROWS & PGUP/DN
   //
   if (keydown[0x48] && base)
     {
      base--;
      redraw=1;
      if (!keydown[0x1d])
	while(keydown[0x48]);
     }
   else
   if (keydown[0x50] && base+10<numnames)
     {
      base++;
      redraw=1;
      if (!keydown[0x1d])
	while(keydown[0x50]);
     }
   else
   if (keydown[0x49])
     {
      base-=10;
      if (base<0)
	base=0;
      redraw=1;
      if (!keydown[0x1d])
	while(keydown[0x49]);
     }
   else
   if (keydown[0x51])
     {
      base+=10;
      if (base+10>numnames)
	base=numnames-10;
      redraw=1;
      if (!keydown[0x1d])
	while(keydown[0x51]);
     }

  } while(!redraw);
 } while(!exit);

 //
 // RETURN PATHNAME
 //
 RestoreBackground();
 strcpy(path,pname);
 strcat(path,NameList[select+base]);
 findfirst(path,&f,FA_ARCH);
 return 0;
}


static void FDon(int x,int y,int w)
{
 xormask=1;
 FDoff(x,y,w);
 xormask=0;
}

static void FDoff(int x,int y,int w)
{
 MouseHide();
 sx=x;
 sy=y;
 print(NameList[w+base]);
 MouseShow();
}


static void CancelOn(int x,int y)
{
 xormask=1;
 CancelOff(x,y);
 xormask=0;
}

static void CancelOff(int x,int y)
{
 MouseHide();
 sx=x;
 sy=y;
 print("Cancel");
 MouseShow();
}


/////////////////////////////////////////////////
//
// GetList dialog
// Fill "NameList[?][13]" with your strings
// (exit:>=0 is selection,-1 if Not Successful,-2 if Canceled)
//
/////////////////////////////////////////////////
int GetList(char *string,int numnames)
{
 unsigned int max,dx,dy,redraw,exit,i;
 int select;


 strcpy(dstr,string);

 DrawDialog(&GPd,1);
 MouseHide();
 GetDialogXY(&GPd,&dx,&dy);
 DrawBorder(dx+LISTX-1,dy+LISTY-1,13,11,1);
 MouseShow();

 base=exit=0;

 //
 // THIS LOOP DRAWS THE DIALOG
 //
 do
 {
  redraw=0;
  MouseHide();
  max=10;
  if (numnames<10)
    max=numnames;
  for (i=0;i<max;i++)
    {
     bar(dx+LISTX,dy+LISTY+i,dx+LISTX+11,dy+LISTY+i,' ');
     FDoff(dx+LISTX,dy+LISTY+i,i);
    }
  MouseShow();

  //
  // THIS LOOP CHECKS INPUT
  //
  do
  {
   select=CheckList(dx+LISTX,dy+LISTY,12,max,FDon,FDoff,0);
   if (select>=0)
     {
      redraw=exit=1;
      continue;
     }

   GetButtonXY(&GPd,0,&sx,&sy);
   select=CheckList(sx,sy,strlen(GPb.text),1,CancelOn,CancelOff,0);
   if (!select || keydown[1])
     {
      RestoreBackground();
      while(keydown[1]);
      clearkeys();
      return -2;
     }

   //
   // CHECK ARROWS & PGUP/DN
   //
   if (keydown[0x48] && base)
     {
      base--;
      redraw=1;
      if (!keydown[0x1d])
	while(keydown[0x48]);
     }
   else
   if (keydown[0x50] && base+10<numnames)
     {
      base++;
      redraw=1;
      if (!keydown[0x1d])
	while(keydown[0x50]);
     }
   else
   if (keydown[0x49])
     {
      base-=10;
      if (base<0)
	base=0;
      redraw=1;
      if (!keydown[0x1d])
	while(keydown[0x49]);
     }
   else
   if (keydown[0x51])
     {
      base+=10;
      if (base+10>numnames)
	base=numnames-10;
      redraw=1;
      if (!keydown[0x1d])
	while(keydown[0x51]);
     }

  } while(!redraw);
 } while(!exit);

 //
 // RETURN SELECTION
 //
 RestoreBackground();
 return select+base;
}
