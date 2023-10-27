////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
// TED5-2 : Menu Item Routines
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////
#include "ted5.h"
#pragma hdrstop

////////////////////////////////////////////////////
//
// Toggle the INFOBAR on and off
//
////////////////////////////////////////////////////
void Item_ToggleInfo(void)
{
 infobaron^=1;
 MouseHide();
 InitDesktop(TED5MenuBar,0);
 FigureScreenEdges();
 if (ybase+screenh>mapheight && mapheight>screenh)
   ybase--;
 DrawMap();
 DrawInfoBar();
 MouseShow();
}


////////////////////////////////////////////////////
//
// Tile Select
//
////////////////////////////////////////////////////
btype SelTb[]={{" Tiles ",2,21,1},
	       {" Masked ",11,21,1},
	       {" Icons ",21,21,1},
	       {" Exit ",30,21,2}};
DialogDef SelTd={"",38,23,4,&SelTb[0],NULL};

void SelectTiles(int screen)
{
 int exitok=0,which,i,numrows,numcols,b0,b1,redraw=0;

 //
 // if parameter passed, change the screen
 //
 if (screen)
   whichscreen=screen-1;

 if (PasteMode || SelectMode)   // RE-BLIT MAP IF WE ARE ENTERING WHILE
   redraw=1;                    // FLOATING AN IMAGE

 PasteMode=SelectMode=0;
 SelX1=SelX2=SelY1=SelY2=-1;

 switch(videomode)
 {
  case CGA:
  case EGA1:
  case VGA:
	 SelTd.height=23;
    for(i=0;i<4;i++)
      SelTb[i].yoff=21;
    break;
  case EGA2:
    SelTd.height=58;
    for(i=0;i<4;i++)
      SelTb[i].yoff=56;
 }

 DrawDialog(&SelTd,1);
 DrawTileSelect(0,&numrows,&numcols);
 selectcols=numcols;    // VERY IMPORTANT TO PASTE-FROM-TILESELECT MODE
 DrawCurrentTiles();
 while(keydown[0x39]);  // WAIT FOR SPACE-UP

 do
 {
  which=CheckButtonsRet(&SelTd);
  b0=MouseButton()&1;
  b1=(MouseButton()>>1)&1;

  if (which<0)
    {
     //
     // GRAB CURRENT TILE FROM MATRIX
     //
     if (b0 || b1)
       {
	int mx,my;

	MouseCoords(&mx,&my);
	mx/=8;
	if (mx>=left && mx<left+(numcols<<(tsize-1)) &&
	    my>=16 && my<=16+(numrows<<(2+tsize)))
	  {
	   int tile;

	   tile=((my-16)>>(tsize+2))*numcols+
		((mx-left)>>(tsize-1));

	   if (SelectMode)
	     {
	      int thebase;
	      switch(whichscreen)
			{
	       case TILES: thebase=tilebase/numcols; break;
	       case MASKED: thebase=tilembase/numcols;
	      }

	      if (b0)
		{
		 sound(1000);
		 SelY1=((my-16)>>(tsize+2))+thebase;
		 SelX1=((mx-left)>>(tsize-1));
		 if (SelX2==-1 && SelY2==-1)
		   {
		    SelX2=SelX1;
		    SelY2=SelY1;
		   }
		 DrawTileSelect(0,&numrows,&numcols);
		 nosound();
		}
	      else
	      if (b1)
		{
		 sound(1000);
		 SelY2=((my-16)>>(tsize+2))+thebase;
		 SelX2=((mx-left)>>(tsize-1));
		 if (SelX1==-1 && SelY1==-1)
		   {
		    SelX1=SelX2;
		    SelY1=SelY2;
		   }
		 DrawTileSelect(0,&numrows,&numcols);
		 nosound();
		}
	     }
	   else
	     {
	      switch(whichscreen)
	      {
	       case TILES:
		 if (XMSlookup[tile+tilebase]>=0)
		   whicht=tile+tilebase;
		 else
		   errsound();
		 break;
	       case MASKED:
		 whichtm=tile+tilembase+numcols*maxiconrows+tilenum;
		 if (XMSlookup[whichtm]==-1)
		   whichtm=tilenum;
		 break;
	       case ICONS:
		 if (XMSlookup[tile+tilenum]>=0 || !tile)
		   whichi=tile+tilenum;
		 else
		   errsound();
		 break;
	      }
	      DrawCurrentTiles();
	     }
	  }
       }

     if (keydown[0x48])                 // UP
       DrawTileSelect(-1,&numrows,&numcols);
     else
     if (keydown[0x50])                 // DOWN
       DrawTileSelect(1,&numrows,&numcols);
     else
     if (keydown[0x47])                 // HOME
       {
	if (whichscreen==TILES)
	  DrawTileSelect(-tilebase/numcols,&numrows,&numcols);
	else
	if (whichscreen==MASKED)
	  DrawTileSelect(-tilembase/numcols,&numrows,&numcols);
       }
     else
     if (keydown[0x4f])                 // END
       {
	if (whichscreen==TILES)
	  DrawTileSelect(tilenum,&numrows,&numcols);
	else
	if (whichscreen==MASKED)
	  DrawTileSelect(tilemnum,&numrows,&numcols);
       }
     else
     if (keydown[0x49])                 // PgUP
       {
	DrawTileSelect(-numrows,&numrows,&numcols);
	if (!keydown[0x1d])     // if not CTRL down, wait for keyup
	  while(keydown[0x49]);
       }
	  else
     if (keydown[0x51])                 // PgDN
       {
	DrawTileSelect(numrows,&numrows,&numcols);
	if (!keydown[0x1d])     // if not CTRL down, wait for keyup
	  while(keydown[0x51]);
       }
     else
     if (keydown[0x39])                 // SPACEBAR
       {
	RestoreBackground();
	while(keydown[0x39]);
	SelectMode=0;
	if (redraw)
	  DrawMap();
	DrawInfoBar();
	return;
       }
     else                               // 'C' TO COPY
     if (keydown[0x2e] && (whichscreen==TILES || whichscreen==MASKED))
       {
	char temp[]="COPY MODE";

	sx=screencenterx-strlen(temp)/2;
	SelectMode=sy=1;
	print(temp);

	while(keydown[0x2e]);
       }
    }
  else
    switch(which)
    {
     case 0:
       if (!SelectMode)
	 exitok=1;
       else
	 {
	  char temp[]="         ";

	  sx=screencenterx-strlen(temp)/2;
	  SelectMode=0;
	  SelX1=SelX2=SelY1=SelY2=-1;
	  sy=1;
	  print(temp);
	  DrawTileSelect(0,&numrows,&numcols);
	 }
       break;

     case 4:
       if (!SelectMode)
	 exitok=1;
       //
       // 'ENTER' TO FINALIZE COPY
       //
       else
	 {
	  char temp[]="         ";

	  if (MouseButton())    // IF CLICKED ON 'EXIT' WITH MOUSE
	    break;

	  sound(500);
	  sx=screencenterx-strlen(temp)/2;
	  PasteOK=sy=1;
	  print(temp);

	  TileCopy.x=SelX1;
	  TileCopy.y=SelY1;
	  TileCopy.w=SelX2-SelX1+1;
	  TileCopy.h=SelY2-SelY1+1;
	  TileCopy.MapOrTileSelect=(whichscreen==TILES)+2*(whichscreen==MASKED);

	  while(keydown[0x1c]);
	  nosound();
	  SelectMode=0;
	  SelX1=SelX2=SelY1=SelY2=-1;
	  DrawTileSelect(0,&numrows,&numcols);
	 }
       break;
     //
     // NORMAL TILE SELECT
     //
     case 1:
       if (whichscreen!=TILES)
	 {
	  SelX1=SelX2=SelY1=SelY2=-1;
	  SelectMode=0;
	  whichscreen=TILES;
	  DrawDialog(&SelTd,0);
	  DrawCurrentTiles();
	  DrawTileSelect(0,&numrows,&numcols);
	 }
       else
	 errsound();
       GetButtonXY(&SelTd,0,&sx,&sy);
       MouseHide();
       print(SelTb[0].text);
       MouseShow();
       break;
     //
     // MASKED TILE SELECT
     //
     case 2:
       if (tilemnum && whichscreen!=MASKED)
	 {
	  SelX1=SelX2=SelY1=SelY2=-1;
	  SelectMode=0;
	  whichscreen=MASKED;
	  DrawDialog(&SelTd,0);
	  DrawCurrentTiles();
	  DrawTileSelect(0,&numrows,&numcols);
	 }
       else
	 errsound();
       GetButtonXY(&SelTd,1,&sx,&sy);
       MouseHide();
       print(SelTb[1].text);
       MouseShow();
       break;
     //
     // ICON SELECT
     //
     case 3:
       if (tilemnum && whichscreen!=ICONS)
	 {
	  whichscreen=ICONS;
	  DrawDialog(&SelTd,0);
	  DrawCurrentTiles();
	  DrawTileSelect(0,&numrows,&numcols);
	 }
       else
	 errsound();
       GetButtonXY(&SelTd,2,&sx,&sy);
       MouseHide();
		 print(SelTb[2].text);
       MouseShow();
    }

 } while(!exitok);
 RestoreBackground();
 SelectMode=0;
 SelX1=SelX2=SelY1=SelY2=-1;
 DrawInfoBar();
 if (redraw)
   DrawMap();
}


//
// DRAW THE CURRENT TILES IN THE TILE-SELECT WINDOW
//
void DrawCurrentTiles(void)
{
 MouseHide();
 GetButtonXY(&SelTd,0,&sx,&sy);
 sy-=1+tsize;
 CombineTiles(whicht,0,0,tsize);
 DrawTile(sx,sy*8,tsize);
 sx+=2;
 printhex(whicht);
 sy++;
 sx-=5;
 printint(whicht);
 print("   ");

 if (tilemnum)
   {
    GetButtonXY(&SelTd,1,&sx,&sy);
    sy-=1+tsize;
    CombineTiles(-BkgndColor,whichtm,0,tsize);
    DrawTile(sx,sy*8,tsize);
    sx+=2;
    (whichtm==tilenum)?print(" No  "):printhex(whichtm-tilenum);

    sy++;
    sx-=5;
    (whichtm==tilenum)?print(" Tile "):printint(whichtm-tilenum);
    print("    ");

	 GetButtonXY(&SelTd,2,&sx,&sy);
    sy-=1+tsize;
    CombineTiles(-ICONBACK,whichi,0,tsize);
    DrawTile(sx,sy*8,tsize);
    sx+=2;
    (whichi==tilenum)?print(" No  "):printhex(whichi-tilenum);
    sy++;
    sx-=5;
    (whichi==tilenum)?print(" Icon "):printint(whichi-tilenum);
    print("    ");
   }
 MouseShow();
}


//
// CHECK TILESELECT EDGES
//
void CheckTSelectEdges(int x,int y,int basey)
{
 int xx,yy,temp;

 xx=left+(x<<(tsize-1));
 yy=(y<<(tsize-1))+2;

 if (SelX2<SelX1)
   {
    temp=SelX1;
    SelX1=SelX2;
    SelX2=temp;
   }

 if (SelY2<SelY1)
   {
    temp=SelY1;
    SelY1=SelY2;
    SelY2=temp;
   }

 if (x==SelX1 && y==SelY1-basey)
   {
    switch(tsize)
    {
     case 1: drawchar(xx,yy,1); return;
     case 2: drawchar(xx  ,yy,1);
		  drawchar(xx+1,yy,2);
	     drawchar(xx  ,yy+1,4);
	     break;
     case 3: drawchar(xx,yy,1);

	     drawchar(xx,yy+1,4);
	     drawchar(xx,yy+2,4);
	     drawchar(xx,yy+3,4);

	     drawchar(xx+1,yy,2);
	     drawchar(xx+2,yy,2);
	     drawchar(xx+3,yy,2);

	     break;
    }
   }
 if (x==SelX2 && y==SelY2-basey)
   {
    switch(tsize)
    {
     case 1: drawchar(xx,yy,8); return;
     case 2: drawchar(xx+1,yy+1,8);
	     drawchar(xx+1,yy,5);
	     drawchar(xx,yy+1,7);
	     break;
     case 3: drawchar(xx+3,yy+3,8);

	     drawchar(xx+3,yy  ,5);
	     drawchar(xx+3,yy+1,5);
	     drawchar(xx+3,yy+2,5);

	     drawchar(xx  ,yy+3,7);
	     drawchar(xx+1,yy+3,7);
	     drawchar(xx+2,yy+3,7);
	     break;
    }
   }
 if (x==SelX2 && y==SelY1-basey)
   {
    switch(tsize)
    {
     case 1: drawchar(xx,yy,3); return;
     case 2: drawchar(xx+1,yy,3);
	     drawchar(xx,yy,2);
	     drawchar(xx+1,yy+1,5);
		  break;
     case 3: drawchar(xx+3,yy,3);

	     drawchar(xx+2,yy,2);
	     drawchar(xx+1,yy,2);
	     drawchar(xx  ,yy,2);

	     drawchar(xx+3,yy+1,5);
	     drawchar(xx+3,yy+2,5);
	     drawchar(xx+3,yy+3,5);
	     break;
    }
   }
 if (x==SelX1 && y==SelY2-basey)
   {
    switch(tsize)
    {
     case 1: drawchar(xx,yy,6); return;
     case 2: drawchar(xx,yy+1,6);
	     drawchar(xx+1,yy+1,7);
	     drawchar(xx,yy,4);
	     break;
     case 3: drawchar(xx,yy+3,6);

	     drawchar(xx  ,yy+2,4);
	     drawchar(xx  ,yy+1,4);
	     drawchar(xx  ,yy,4);

	     drawchar(xx+1,yy+3,7);
	     drawchar(xx+2,yy+3,7);
	     drawchar(xx+3,yy+3,7);
	     break;
    }
   }

 if (x==SelX1 && y>SelY1-basey && y<SelY2-basey)
   {
    switch(tsize)
    {
     case 3: drawchar(xx,yy+3,4);
	     drawchar(xx,yy+2,4);
     case 2: drawchar(xx,yy+1,4);
     case 1: drawchar(xx,yy,4);
	     break;
    }
	}
 else
 if (y==SelY1-basey && x>SelX1 && x<SelX2)
   {
    switch(tsize)
    {
     case 3: drawchar(xx+3,yy,2);
	     drawchar(xx+2,yy,2);
     case 2: drawchar(xx+1,yy,2);
     case 1: drawchar(xx  ,yy,2);
	     break;
    }
   }
 else
 if (x==SelX2 && y>SelY1-basey && y<SelY2-basey)
   {
    switch(tsize)
    {
     case 1: drawchar(xx,yy,5); return;
     case 2: drawchar(xx+1,yy,5);
	     drawchar(xx+1,yy+1,5);
	     break;
     case 3: drawchar(xx+3,yy,5);
	     drawchar(xx+3,yy+1,5);
	     drawchar(xx+3,yy+2,5);
	     drawchar(xx+3,yy+3,5);
	     break;
    }
   }
 else
 if (y==SelY2-basey && x>SelX1 && x<SelX2)
   {
    switch(tsize)
    {
     case 1: drawchar(xx,yy,7); return;
     case 2: drawchar(xx  ,yy+1,7);
	     drawchar(xx+1,yy+1,7);
	     break;
     case 3: drawchar(xx  ,yy+3,7);
	     drawchar(xx+1,yy+3,7);
	     drawchar(xx+2,yy+3,7);
	     drawchar(xx+3,yy+3,7);
	     break;
    }
   }
}



//
// FILL THE TILE SELECT SCREEN WITH TILES
//
void DrawTileSelect(int deltarow,int *retrows,int *retcols)
{
 char tile[32*32];
 int numcols,numrows,i,j;

 MouseHide();
 switch(videomode)
 {
  case CGA:
  case EGA1:
  case VGA:
    numrows=18-10*(tsize==2)-15*(tsize==3);
    left=2;
    break;
  case EGA2:
    numrows=49-23*(tsize==2)-34*(tsize==3);
    left=22;
 }

 switch(tsize)
 {
  case 1: numcols=36; break;
  case 2: numcols=18; break;
  case 3: numcols=9;
 }

 if (whichscreen==TILES && tilebase+numrows*numcols>tilenum)
   {
    tilebase=0;
    numrows=tilenum/numcols;
   }

 if (whichscreen==MASKED && tilembase+numrows*numcols>tilemnum-maxiconrows*numcols)
   {
    tilembase=0;
    numrows=tilemnum/numcols-maxiconrows;
   }

 if (whichscreen==ICONS && numrows*numcols>maxiconrows*numcols)
   {
    tilembase=0;
    numrows=maxiconrows;
   }

 switch((deltarow<0?-1:deltarow>0?1:0))
 {
  case -1:
   switch(whichscreen)
   {
    case TILES:
      tilebase+=deltarow*numcols;
      if (tilebase<0)
	tilebase=0;
      break;
    case MASKED:
      tilembase+=deltarow*numcols;
      if (tilembase<0)
	tilembase=0;
   }
   break;
  case 1:
   switch(whichscreen)
   {
    case TILES:
      tilebase+=deltarow*numcols;
      if (tilebase+numrows*numcols>tilenum)
	tilebase=tilenum-numcols*numrows;
      break;
    case MASKED:
      tilembase+=deltarow*numcols;
      if (tilembase+numrows*numcols>tilemnum-maxiconrows*numcols)
	tilembase=(tilemnum-maxiconrows*numcols)-numcols*numrows;
   }
 }

 switch(whichscreen)
 {
  case TILES:
    for(j=0;j<numrows;j++)
      for(i=0;i<numcols;i++)
      {
       CombineTiles(tilebase+j*numcols+i,0,0,tsize);
       if (GridMode)
	 Overlay(tsize);
       DrawTile(i*2+left,j*16+16,tsize);
       CheckTSelectEdges(i,j,tilebase/numcols);
      }
    break;

  case MASKED:
    for(j=0;j<numrows;j++)
      for(i=0;i<numcols;i++)
      {
       CombineTiles(-BkgndColor,tilenum+numcols*maxiconrows+tilembase+j*numcols+i,0,tsize);
       if (GridMode)
	 Overlay(tsize);
       DrawTile(i*2+left,j*16+16,tsize);
       CheckTSelectEdges(i,j,tilembase/numcols);
      }
    break;

  default:
    for(j=0;j<numrows;j++)
      for(i=0;i<numcols;i++)
      {
       CombineTiles(-ICONBACK,tilenum+j*numcols+i,0,tsize);
       if (GridMode)
	 Overlay(tsize);
       DrawTile(i*2+left,j*16+16,tsize);
      }
 }
 MouseShow();

 *retrows=numrows;
 *retcols=numcols;
}



void errsound(void)
{
 sound(1000);
 delay(50);
 nosound();
}


////////////////////////////////////////////////////
//
// Item - About
//
////////////////////////////////////////////////////
void Item_About(void)
{
 char dstr[200]=IDSTSTR;

 strcat(dstr,"\n\n   IdWare by John Romero\n");
 strcat(dstr,"        ");
 strcat(dstr,__DATE__);
 strcat(dstr,"\n    (C)1991 Id Software\n\n"
	     " DeluxePaint for Tile Maps! ");

 ErrDialog(dstr," OK ");
}

////////////////////////////////////////////////////
//
// Item - Video Mode Switch
//
////////////////////////////////////////////////////
btype VMS1b[]={{" CGA 320x200 ",5,2,1},
	       {" EGA 320x200 ",5,5,1},
			 {" EGA 640x350 ",5,8,1},
	       {" VGA 320x200 ",5,11,1}};
DialogDef VMS1d={"Select the resolution:",22,14,4,&VMS1b[0],NULL};

void Item_ModeSwitch(void)
{
 int i,which,rtn;

 for(i=0;i<VMS1d.numbuttons;i++)
   {
    VMS1b[i].border=1;
    if (i==lastvideo)
      VMS1b[i].border=2;
   }

// which=DoDialog(&VMS1d);
 //
 // Keep track of last video mode!
 //
 if (videomode==EGA1)
	 {
	 which=3;
	 lastvideo=videomode;
	 }
 else
	 {
	 which=2;
	 lastvideo=videomode;
	 }

 MouseHide();
 switch(which)
 {
  case 0:
	 MouseShow();
	 return;
  case 1:
	 if (!CgaXMS)
		{
		 rtn=LoadGraphStuff(1,CGA);
		 if (!rtn)
	 {
	  MouseShow();
	  ErrDialog("There aren't any CGA\n"
			 "graphics files available!"," OK ");
	  return;
	 }
		 if (rtn<0)
	 return;
		 MouseShow();
		}
	 else
		{
		 xmshandle=CgaXMS;
		 XMSlookup=CgaXMSlookup;
		}
	 setvideo(CGA);
	 break;
  case 2:
	 if (!EgaXMS)
		{
		 rtn=LoadGraphStuff(1,EGA1);
		 if (!rtn)
	 {
	  MouseShow();
	  ErrDialog("There aren't any EGA\n"
			 "graphics files available!"," OK ");
	  return;
	 }
		 if (rtn<0)
	 return;
		 MouseShow();
		}
	 else
      {
       xmshandle=EgaXMS;
       XMSlookup=EgaXMSlookup;
      }
    setvideo(EGA1);
    break;
  case 3:
    if (!EgaXMS)
      {
       rtn=LoadGraphStuff(1,EGA1);
       if (!rtn)
	 {
	  MouseShow();
	  ErrDialog("There aren't any EGA\n"
		    "graphics files available!"," OK ");
	  return;
	 }
       if (rtn<0)
	 return;
       MouseShow();
      }
    else
      {
       xmshandle=EgaXMS;
       XMSlookup=EgaXMSlookup;
      }
    setvideo(EGA2);
    break;
  case 4:
    if (!VgaXMS)
      {
       rtn=LoadGraphStuff(1,VGA);
       if (!rtn)
	 {
	  MouseShow();
	  ErrDialog("There aren't any VGA\n"
		    "graphics files available!"," OK ");
	  return;
	 }
       if (rtn<0)
	 return;
       MouseShow();
		}
	 else
      {
       xmshandle=VgaXMS;
       XMSlookup=VgaXMSlookup;
      }
    setvideo(VGA);
 }

 InitDesktop(TED5MenuBar,0);
 DrawInfoBar();
 FigureScreenEdges();

 if (xbase+screenw>mapwidth)
   xbase=mapwidth-screenw;
 if (mapwidth<screenw)
   xbase=0;
 if (ybase+screenh>mapheight)
   ybase=mapheight-screenh;
 if (mapheight<screenh)
   ybase=0;

 DrawMap();
 MouseShow();
}

////////////////////////////////////////////////////
//
// Item - Map Stats
//
////////////////////////////////////////////////////
btype MapStatsb={" OK ",11,11,2};
DialogDef MapStatsd={"        MAP STATS\n\n"
		     "Width   :\n"
		     "Height  :\n"
		     "Name    :\n"
		     "Planes  :",
		     24,13,1,&MapStatsb,NULL};

void Item_MapStats(void)
{
 int ox,oy;
 unsigned i,_seg *unique;

 DrawDialog(&MapStatsd,1);
 MouseHide();
 GetDialogXY(&MapStatsd,&sx,&sy);
 ox=sx+9;
 oy=sy+2;
 sx=ox;
 sy=oy;
 printint(mapwidth);
 sx=ox;
 sy=oy+1;
 printint(mapheight);
 sx=ox;
 sy=oy+2;
 print(MapHeader.name);
 sx=ox;
 sy=oy+3;

 if (MapFileHeader->maptype&BPLANE)
   print("BACK");
 if (MapFileHeader->maptype&FPLANE)
   print(",FORE");
 if (MapFileHeader->maptype&IPLANE)
   print(",INFO");

 //
 // Count amount of unique background tiles
 //
 sx=ox-9;
 sy=oy+4;
 if (MapFileHeader->maptype&BPLANE)
   {
    int amount=0;

    print("Unique Bkgnd Tiles:");
    MMAllocate((memptr *)&unique,tilenum*2);
    _fmemset(unique,0,tilenum*2);
    for (i=0;i<mapwidth*mapheight;i++)
      unique[MapBkgnd[i]]=1;
    for (i=0;i<tilenum;i++)
      amount+=unique[i];
    MMFreePtr((memptr *)&unique);
	 printint(amount);
   }

 //
 // Count amount of unique foreground tiles
 //
 sx=ox-9;
 sy=oy+5;
 if (MapFileHeader->maptype&FPLANE)
   {
    int amount=0;

    print("Unique Frgnd Tiles:");
    MMAllocate((memptr *)&unique,tilemnum*2);
    _fmemset(unique,0,tilemnum*2);
    for (i=0;i<mapwidth*mapheight;i++)
      if (MapFrgnd[i])
	unique[MapFrgnd[i]]=1;
    for (i=0;i<tilemnum;i++)
      amount+=unique[i];
    MMFreePtr((memptr *)&unique);
    printint(amount);
   }

 //
 // Count amount of icons
 //
 sx=ox-9;
 sy=oy+6;
 if (MapFileHeader->maptype&IPLANE)
   {
    int amount=0;

    print("Amount of Icons:");
    for (i=0;i<mapwidth*mapheight;i++)
      if (MapInfoPl[i])
	amount++;
    printint(amount);
   }

 //
 // Display amount of memory the map needs
 //
 sx=ox-9;
 sy=oy+7;
 {
  long length=0;

  if (MapFileHeader->maptype&BPLANE)
    length+=2L*mapwidth*mapheight;
  if (MapFileHeader->maptype&FPLANE)
	 length+=2L*mapwidth*mapheight;
  if (MapFileHeader->maptype&IPLANE)
    length+=2L*mapwidth*mapheight;
  length+=sizeof(MapHeaderStr);

  print("Size of map:");
  length=(length+1023)/1024;
  printint(length);
  print("K");
 }

 MouseShow();
 CheckButtons(&MapStatsd);
 RestoreBackground();
}


////////////////////////////////////////////////////
//
// Item - Edit New Map
//
////////////////////////////////////////////////////
btype DOSAVEb[]={{" Yes ",3,3,2},
		 {" No! ",15,3,1}};
DialogDef DOSAVEd={"Your map has changed!\n"
		   "       Save it?",
		   21,5,2,&DOSAVEb[0],NULL};

void Item_EditMap(void)
{
 int which,olddirt;

 olddirt=DirtyFlag;
 if (DirtyFlag)
   {
    which=DoDialog(&DOSAVEd);
    if (!which)
      return;
    if (which==1)
		Item_SaveMap();
    DirtyFlag=0;
   }

 if ((which=SelectMap(1,CREATED,"TO EDIT"))==-1)
   {
	 DirtyFlag=olddirt;
    return;
   }

 TEDInfo->level=whichmap=which;
 LoadMap(whichmap);
 MouseHide();
 InitDesktop(TED5MenuBar,0);
 DrawInfoBar();
 FigureScreenEdges();
 DrawMap();
 MouseShow();
}

////////////////////////////////////////////////////
//
// Item - Save Map
//
////////////////////////////////////////////////////
void Item_SaveMap(void)
{
 SaveMap(0);
 DirtyFlag=0;
}

////////////////////////////////////////////////////
//
// Item - Create Map
//
////////////////////////////////////////////////////
void Item_CreateMap(void)
{
 if (DirtyFlag)
   {
    int button;

    button=DoDialog(&DoCreated);
    if (!button)
      return;
	 if (button==1)
      Item_SaveMap();
    DirtyFlag=0;
   }

 CreateMap(1);
 MouseHide();
 InitDesktop(TED5MenuBar,0);
 DrawInfoBar();
 FigureScreenEdges();
 DrawMap();
 MouseShow();
}

////////////////////////////////////////////////////
//
// Item - Delete Map
//
////////////////////////////////////////////////////
btype AreSureB[]={{" Yes ",1,4,1},
		  {" No ",11,4,2}};
DialogDef AreSureD={"Are you sure you\n"
			"you want to delete",
			18,6,2,&AreSureB[0],NULL};

void Item_DeleteMap(void)
{
 MapHeaderStr TempHead;
 int whichdel,which,temp;

 if ((whichdel=SelectMap(1,CREATED,"TO DELETE"))==-1)
   return;

 if (whichmap==whichdel)
   {
	ErrDialog("I'm just not gonna stand for\n"
		 "you deleting the map you're\n"
		 "currently editing. I'm not\n"
		 "gonna doit. Nope."," Gee... ");
	return;
   }

 LoadFile(mapname,(char huge *)&TempHead,
	MapFileHeader->dataoffsets[whichdel],sizeof(MapHeaderStr));
 MouseHide();
 DrawDialog(&AreSureD,1);
 GetDialogXY(&AreSureD,&sx,&sy);
 sy+=2;
 sx=screencenterx-(strlen(TempHead.name)+1)/2;
 print(TempHead.name);
 print("?");
 MouseShow();

 which=CheckButtons(&AreSureD);
 switch(which)
 {
  case 1:
	temp=whichmap;
	whichmap=whichdel;
	RestoreBackground();
	SaveMap(1);
	whichmap=temp;
	return;
 }
 RestoreBackground();
}

////////////////////////////////////////////////////
//
// Item - Amputate Maps
//
////////////////////////////////////////////////////
void Item_Amputate(void)
{
	char tstr[40];
	MapHeaderStr TempHead;
	int which1,which2,whichtemp,i,button;
	long temp;


	if ((which1 = SelectMap(1,CREATED,"TO START AMPUTATE"))==-1)
		return;

	LoadFile(mapname,(char huge *)&TempHead,
		MapFileHeader->dataoffsets[which1],sizeof(MapHeaderStr));

	if ((which2 = SelectMap(1,ANYLIST,"TO END AMPUTATE"))==-1)
		return;

	if (which2 < which1)
	{
		whichtemp = which1;
		which1 = which2;
		which2 = whichtemp;
	}

	if (whichmap >= which1 && whichmap <= which2)
	{
		ErrDialog (	"The currently loaded map\n"
					"is within that range!\n"
					"NON-AMPUTATENESS!!","Wah!");
		return;
	}

	DrawDialog(&AreSureD,1);
	button = CheckButtons(&AreSureD);
	switch (button)
	{
		case 1:
			for (i = which1;i <= which2;i++)
			{
				MapFileHeader->dataoffsets[i] = -1;
				MapFileHeader->datalengths[i] = 0;
			}

			DirtyFlag = writeH = 1;
			SaveMap(0);
	}
}


////////////////////////////////////////////////////
//
// Item - Switch Maps
//
////////////////////////////////////////////////////
void Item_SwitchMap(void)
{
 char tstr[40];
 MapHeaderStr TempHead;
 int which1,which2;
 long temp;

 while(1)
 {
  if ((which1=SelectMap(1,CREATED,"TO SWAP"))==-1)
	return;
  LoadFile(mapname,(char huge *)&TempHead,
	MapFileHeader->dataoffsets[which1],sizeof(MapHeaderStr));
  strcpy(tstr,"TO SWAP WITH '");
  strcat(tstr,TempHead.name);
  strcat(tstr,"'");
  if ((which2=SelectMap(1,ANYLIST,tstr))==-1)
	return;

  if (which1==whichmap) // MAKE SURE THE CURRENTLY EDITED MAP GETS CHANGED!
	whichmap=which2;
  else
  if (which2==whichmap)
	whichmap=which1;

  temp=MapFileHeader->dataoffsets[which1];

  strcpy(tstr,MapNames[which1]);
  strcpy(MapNames[which1],MapNames[which2]);
  strcpy(MapNames[which2],tstr);

  MapFileHeader->dataoffsets[which1]=MapFileHeader->dataoffsets[which2];
  MapFileHeader->dataoffsets[which2]=temp;
  writeH=DirtyFlag=1;
 }
}

////////////////////////////////////////////////////
//
// Item - Quit
//
////////////////////////////////////////////////////
btype Qbuttons[]={{" Yes ",4,2,2},{" No ",12,2,1}},
      DoSaveb[]={{" Yes ",7,4,2},{" No ",14,4,1}};
DialogDef Qdialog={"Quit - Are you sure?",20,4,2,&Qbuttons[0],NULL},
	  DoSaved={"The map has been modified.\n"
		   "Do you want to SAVE it\n"
		   "before exiting TED5?",26,6,2,&DoSaveb[0],NULL},
	  DoCreated={"The map has been modified.\n"
		     "Do you want to SAVE it\n"
		     "before CREATING a new one?",26,6,2,&DoSaveb[0],NULL};

void Item_Quit(void)
{
 int button;

 button=DoDialog(&Qdialog);

 if (button==1)
   {
	 TEDInfo->lastvid=videomode;
    TEDInfo->level=whichmap;
    TEDInfo->OldCgaXMS=0;
    TEDInfo->OldEgaXMS=0;
    TEDInfo->OldVgaXMS=0;
    TEDInfo->OldCgaXMSsize=0;
    TEDInfo->OldEgaXMSsize=0;
    TEDInfo->OldVgaXMSsize=0;

    TEDInfo->pflags=((planeton&1)<<6)|
		    ((planemon&1)<<5)|
		    ((planeion&1)<<4)|
		    ((viewton&1)<<2)|
		    ((viewmon&1)<<1)|
		    (viewion&1);

    SaveTEDInfo();
    if (DirtyFlag)
      {
       button=DoDialog(&DoSaved);
       if (button==1)
	 Item_SaveMap();
       if (!button)
	 return;        // ESC exits
      }
    SaveOutputHeader();
    Quit("");
   }
}

////////////////////////////////////////////////////
//
// Item - Edit Map Names
//
////////////////////////////////////////////////////
char EMNstring[16];
btype EMNb[]={{EMNstring,1,4,1},
	      {"                ",1,8,1},
	      {" Exit ",7,11,1}};
DialogDef EMNd={"     MAP RENAME\n\nChange...\n\n\n\nTo..."
		,20,13,3,&EMNb[0],NULL};

void Item_EditMapNames(void)
{
 int which,mapnum,redraw=0,omapnum,oxb,oyb;
 MapHeaderStr TempHeader;
 char temp[16];


 CheckForMapSave();
 omapnum=whichmap;
 oxb=xbase;
 oyb=ybase;

 if ((mapnum=SelectMap(1,CREATED,"TO RENAME"))<0)
   return;

 whichmap=mapnum;
 LoadMap(whichmap);
 strcpy(EMNstring,MapHeader.name);

 MouseHide();
 DrawDialog(&EMNd,1);
 GetDialogXY(&EMNd,&sx,&sy);
 MouseShow();
 which=2;
#pragma warn -rch
 goto badboy;

 do
 {
  which=CheckButtons(&EMNd);

badboy:
#pragma warn +rch
  switch(which)
  {
   case 1:
     RestoreBackground();
     if ((mapnum=SelectMap(1,CREATED,"TO RENAME"))<0)
       {
	if (redraw)
	  DrawInfoBar();

	whichmap=omapnum;
	LoadMap(whichmap);
	xbase=oxb;
	ybase=oyb;
	return;
       }

     whichmap=mapnum;
     LoadMap(whichmap);
     strcpy(EMNstring,MapHeader.name);
     MouseHide();
     DrawDialog(&EMNd,1);
     MouseShow();

   case 2:
     MouseHide();
     GetButtonXY(&EMNd,1,&sx,&sy);
     print(EMNb[1].text);
     GetButtonXY(&EMNd,1,&sx,&sy);
     if (input(temp,15))
       {
	writeH=1;
	strcpy(MapHeader.name,temp);
	strcpy(MapNames[mapnum],temp);
	DirtyFlag=1;
	Item_SaveMap();
       }
     else
       {
	GetButtonXY(&EMNd,1,&sx,&sy);
	print(EMNb[1].text);
       }
     MouseShow();
     break;

   case 3:
     which=0;
  }
 } while(which);

 RestoreBackground();
 if (redraw)
   DrawInfoBar();

 whichmap=omapnum;
 LoadMap(whichmap);
 xbase=oxb;
 ybase=oyb;
}

////////////////////////////////////////////////////
//
// Item - Paste Mode
//
////////////////////////////////////////////////////
void Item_Paste(void)
{
 if (!TileCopy.w)
   return;

 ZeroModes();
 PasteMode=1;
 DrawInfoBar();
 px=(pixelx>>(tsize+2))+xbase;
 py=((pixely-8)>>(tsize+2))+ybase;
}

////////////////////////////////////////////////////
//
// Item - Copy Mode
//
////////////////////////////////////////////////////
void Item_Copy(void)
{
 ZeroModes();
 SelectMode=1;
 SelX1=SelY1=SelX2=SelY2=-1;
 DrawInfoBar();
}

////////////////////////////////////////////////////
//
// Item - LastVideo
//
////////////////////////////////////////////////////
void Item_LastVideo(void)
{
 int temp=videomode;


 videomode=lastvideo;
 lastvideo=temp;
 if (temp==EGA1)
	 videomode=EGA2;
 else
	 videomode=EGA1;

 switch(videomode)
 {
  case CGA:
	 xmshandle=CgaXMS;
	 XMSlookup=CgaXMSlookup;
	 break;
  case EGA1:
  case EGA2:
	 xmshandle=EgaXMS;
	 XMSlookup=EgaXMSlookup;
	 break;
  case VGA:
	 xmshandle=VgaXMS;
	 XMSlookup=VgaXMSlookup;
 }

 MouseHide();
 setvideo(videomode);
 InitDesktop(TED5MenuBar,0);
 DrawInfoBar();
 FigureScreenEdges();

 if (xbase+screenw>mapwidth)
	xbase=mapwidth-screenw;
 if (mapwidth<screenw)
	xbase=0;
 if (ybase+screenh>mapheight)
	ybase=mapheight-screenh;
 if (mapheight<screenh)
	ybase=0;

 DrawMap();
 MouseShow();
}

////////////////////////////////////////////////////
//
// Item - Switch to the Last Map
//
////////////////////////////////////////////////////
void Item_LastMap(void)
{
 int temp;

 if (!CheckForMapSave())
	return;

 if (lastmap==-1)
	{
	 int which=SelectMap(1,CREATED,"TO EDIT");
    if (which==-1)
      return;
    lastmap=which;
   }

 temp=whichmap;
 whichmap=lastmap;
 lastmap=temp;

 LoadMap(whichmap);
 InitDesktop(TED5MenuBar,0);
 MouseShow();
 DrawMap();
 DrawInfoBar();
}

////////////////////////////////////////////////////
//
// Item - Flood Fill!
//
////////////////////////////////////////////////////
void Item_FloodFill(void)
{
 ZeroModes();
 FillMode=1;
 DrawInfoBar();
}

void DoFloodFill(int x,int y,int whichb)
{
#define NUMSPORES       500

 int obx[NUMSPORES],oby[NUMSPORES],i,used,j,width,k,highest,height,
     vectx[4]={0,1,0,-1},vecty[4]={-1,0,1,0},orgt,orgm,orgi,Ton,Mon,Ion;
 unsigned ptr,newoff,whichplanes=0,ctrl=0;
 int nx,ny,newt,newm,newi;


 if (keydown[0x1d] || whichb)
   ctrl++;

 whichplanes=planeton+planemon+planeion;
 if (whichplanes>1)
   {
	 ErrDialog("I will only allow Flood Filling\n"
	      "one plane at a time; you have\n"
	      "more than one plane selected."," OK ");
    return;
   }

 for (i=0;i<NUMSPORES;i++)
   obx[i]=oby[i]=-1;

 obx[0]=x;
 oby[0]=y;
 newoff=ptr=oby[0]*mapwidth+obx[0];
 width=MapHeader.width;
 height=MapHeader.height;
 used=1;
 highest=1;
 if (obx[0]<0 || oby[0]<0 || obx[0]>width || oby[0]>height)
   {
    FillMode=0;
    DrawInfoBar();
    return;
   }

 Ton=planeton;
 Mon=planemon;
 Ion=planeion;

 orgt=*(MapBkgnd+ptr);
 orgm=*(MapFrgnd+ptr);
 orgi=*(MapInfoPl+ptr);

 if (((Ton?whicht==orgt:0) ||
     (Mon?whichtm-tilenum==orgm:0) ||
     (Ion?whichi-tilenum==orgi:0)) && !ctrl)
   {
    FillMode=0;
    DrawInfoBar();
    return;
   }

 MouseHide();
 CopyUndoRegion();
 UndoRegion.x=UndoRegion.y=0;
 UndoRegion.w=mapwidth;
 UndoRegion.h=mapheight;

 if (ctrl)
   {
    unsigned from=(TileCopy.y+(y%TileCopy.h))*mapwidth+
	  TileCopy.x+(x%TileCopy.w);

    switch(TileCopy.MapOrTileSelect)
    {
     case 0: // COPY BUFFER
       Ton=TileCopy.PlanesCopied&BPLANE;
       Mon=TileCopy.PlanesCopied&FPLANE;
       Ion=TileCopy.PlanesCopied&IPLANE;

       newt=CutBkgnd[from];
       newm=CutFrgnd[from];
       newi=CutInfoPl[from];

       break;
     case 1: // TILES
       Ton=1;
       Mon=Ion=0;

       newt=((y%TileCopy.h)+TileCopy.y)*selectcols+
	    TileCopy.x+(x%TileCopy.w);
       if (XMSlookup[newt]<0)
	 Ton=0;
       break;
     case 2: // MASKED
       Ton=Ion=0;
       Mon=1;

       newm=((y%TileCopy.h)+TileCopy.y)*selectcols+
	    TileCopy.x+(x%TileCopy.w)+tilenum+maxiconrows*selectcols;
       if (XMSlookup[newm]<0)
	 Mon=0;
       else
	 newm-=tilenum;
    }
   }
 else
   {
    newt=whicht;
    newm=whichtm-tilenum;
    newi=whichi-tilenum;
   }

 if (Ton)
   *(MapBkgnd+newoff)=newt;
 if (Mon)
   *(MapFrgnd+newoff)=newm;
 if (Ion)
   *(MapInfoPl+newoff)=newi;

 do
   {
    for (i=0;i<=highest;i++)
      //
      // SEE IF SPORE EXISTS
      //
      if (obx[i]!=-1)
	{
	 //
	 // DRAW TILE AT SPORE IF IT'S ONSCREEN
	 //
	 if (oby[i]>=ybase && oby[i]<ybase+screenh &&
	     obx[i]>=xbase && obx[i]<xbase+screenw)
	     {
	      unsigned oldt,oldm,oldi,off;

	      off=oby[i]*width+obx[i];

	      oldt=*(MapBkgnd+off);
	      oldm=*(MapFrgnd+off)+tilenum;
	      oldi=*(MapInfoPl+off)+tilenum;

	      CombineTiles(viewton?oldt:-BkgndColor,
			   viewmon?oldm:0,
			   viewion?oldi:0,
			   tsize);
	      DrawTile((obx[i]-xbase)<<(tsize-1),((oby[i]-ybase)<<(tsize+2))+8,tsize);
	     }

	 //
	 // FREE THIS SPORE
	 //
	 x=obx[i];
	 y=oby[i];
	 obx[i]=-1;
	 used--;

	 //
	 // SEARCH 4 QUADRANTS FOR A SPORE TO FILL
	 // (ONLY 4 QUADS SO WE DON'T FILL THRU DIAGONALS)
	 //
	 if (!ctrl)
	 for (j=0;j<4;j++)
	   {
	    newoff=(y+vecty[j])*mapwidth+x+vectx[j];

	    if ((Ton?*(MapBkgnd+newoff)==orgt:1) &&
		(Mon?*(MapFrgnd+newoff)==orgm:1) &&
		(Ion?*(MapInfoPl+newoff)==orgi:1))
	      {
	       for (k=0;k<NUMSPORES;k++)
		 if (obx[k]==-1)
		   {
		    obx[k]=x+vectx[j];
		    oby[k]=y+vecty[j];
		    if (obx[k]<0 || obx[k]>width || oby[k]<0 || oby[k]>height)
		      {
		       obx[k]=-1;
		       break;
		      }

		    used++;
		    if (Ton)
		      *(MapBkgnd+newoff)=whicht;
		    if (Mon)
		      *(MapFrgnd+newoff)=whichtm-tilenum;
		    if (Ion)
		      *(MapInfoPl+newoff)=whichi-tilenum;
		    DirtyFlag=1;
		    if (k>highest)
		      highest=k;
		    break;
		   }
	      }

	    if (keydown[1])     // ESC OUT
	      {
	       while(keydown[1]);
	       goto done;
	      }
	   }
	 else
	 for (j=0;j<4;j++)
	   {
	    unsigned from;

	    ny=y+vecty[j];
	    nx=x+vectx[j];

	    newoff=ny*mapwidth+nx;

	    if ((Ton?*(MapBkgnd+newoff)==orgt:1) &&
		(Mon?*(MapFrgnd+newoff)==orgm:1) &&
		(Ion?*(MapInfoPl+newoff)==orgi:1))
	      {
	       for (k=0;k<NUMSPORES;k++)
		 if (obx[k]==-1)
		   {
		    obx[k]=nx;
		    oby[k]=ny;
		    if (obx[k]<0 || obx[k]>width || oby[k]<0 || oby[k]>height)
		      {
		       obx[k]=-1;
		       break;
		      }

		    from=(TileCopy.y+(ny%TileCopy.h))*mapwidth+
			 TileCopy.x+(nx%TileCopy.w);

		    switch(TileCopy.MapOrTileSelect)
		    {
		     case 0: // COPY BUFFER
		       Ton=TileCopy.PlanesCopied&BPLANE;
		       Mon=TileCopy.PlanesCopied&FPLANE;
		       Ion=TileCopy.PlanesCopied&IPLANE;

		       newt=CutBkgnd[from];
		       newm=CutFrgnd[from];
		       newi=CutInfoPl[from];

		       break;
		     case 1: // TILES
		       Ton=1;
		       Mon=Ion=0;

		       newt=((ny%TileCopy.h)+TileCopy.y)*selectcols+
			    TileCopy.x+(nx%TileCopy.w);
				 if (XMSlookup[newt]<0)
			 Ton=0;
		       break;
		     case 2: // MASKED
		       Ton=Ion=0;
		       Mon=1;

		       newm=((ny%TileCopy.h)+TileCopy.y)*selectcols+
			    TileCopy.x+(nx%TileCopy.w)+tilenum+maxiconrows*selectcols;
		       if (XMSlookup[newm]<0)
			 Mon=0;
		       else
			 newm-=tilenum;
		    }

		    if (Ton)
		      *(MapBkgnd+newoff)=newt;
		    if (Mon)
		      *(MapFrgnd+newoff)=newm;
		    if (Ion)
		      *(MapInfoPl+newoff)=newi;
		    used++;

		    DirtyFlag=1;
		    if (k>highest)
		      highest=k;
		    break;
		   }
	      }

	    if (keydown[1])     // ESC OUT
	      {
	       while(keydown[1]);
	       goto done;
	      }
	   }
	}
   } while(used);

done:
 DrawMap();
 MouseShow();
 FillMode=0;
 DrawInfoBar();
}



////////////////////////////////////////////////////
//
// If map has been changed, ask user if they want
// to SAVE it before continuing.
//
////////////////////////////////////////////////////
btype oktosaveB[]={{" Yes ",10,2,2},{" No ",20,2,1}};
DialogDef oktosaveD={"The map has been modified!\n"
		     "Save it?",26,4,2,&oktosaveB[0],NULL};

int CheckForMapSave(void)
{
 if (DirtyFlag)
   {
    int which=DoDialog(&oktosaveD);
    if (!which)
      return 0;
    if (which==1)
      Item_SaveMap();
   }
 return 1;
}


////////////////////////////////////////////////////
//
// Zero all special mode flags
//
////////////////////////////////////////////////////
void ZeroModes(void)
{
 SelX1=SelX2=SelY1=SelY2=-1;
 if (BfillMode || SelectMode || PasteMode || FillMode)
 {
  DrawMap();
  DrawInfoBar();
 }

 BfillMode=SelectMode=PasteMode=FillMode=0;
}


////////////////////////////////////////////////////
//
// Item - Count Map Tiles
//
////////////////////////////////////////////////////
btype CMTb[]={
	      {" Exit ",9,20,2},
	      {" TILE ",1,20,1},
	      {" MASKED ",17,20,1},
	      {" Rebuild ",27,20,1}
	     };
DialogDef CMTd={"        Count Unused Map Tiles",38,22,4,&CMTb[0],0};
unsigned char _seg *tarray,_seg *tmarray,thesparse;


void Item_CountTiles(void)
{
 enum {TILE,TILEM};

 unsigned i,j,dx,dy,max,exit=0,numt,numtm,oxbase,oybase,nsize,nmsize,redraw;
 int which,dumrow,dumcol;
 char pb,pf,pi;


 CheckForMapSave();

 oxbase=xbase;
 oybase=ybase;

 //
 // COMPUTE SIZE OF EACH TILE
 //
 switch(videomode)
 {
  case CGA: nsize=16; nmsize=32; break;
  case EGA1:
  case EGA2: nsize=32; nmsize=40; break;
  case VGA: nsize=64; nmsize=128;
 }
 nsize=nsize<<((tsize-1)<<1);
 nmsize=nmsize<<((tsize-1)<<1);


 MouseHide();
 DrawDialog(&CMTd,1);
 GetDialogXY(&CMTd,&dx,&dy);
 sx=dx+4;
 sy=dy+2;
 print("Counting tiles in map:");
 MouseShow();

 MMAllocate((memptr *)&tarray,tilenum);
 MMAllocate((memptr *)&tmarray,tilemnum);
 _fmemset(tarray,0,tilenum);
 _fmemset(tmarray,0,tilemnum);

 //
 // Now, load each map in & count the tiles
 //
 pb=MapFileHeader->maptype&BPLANE;
 pf=MapFileHeader->maptype&FPLANE;
 pi=MapFileHeader->maptype&IPLANE;

 for (i=0;i<100;i++)
   if (MapFileHeader->dataoffsets[i]>=0)
     {
      sx=dx+26;
      sy=dy+2;
      printint(i);

      LoadMap(i);
      max=mapwidth*mapheight;
      for (j=0;j<max;j++)
	{
	 if (pb)
	    tarray[MapBkgnd[j]]=1;
	 if (pf)
	    tmarray[MapFrgnd[j]]=1;
	 if (pi)
	    tmarray[MapInfoPl[j]]=1;
	}
     }

 //
 // Find a SPARSE tile
 //
 for (i=0;i<tilenum;i++)
   if (XMSlookup[i]<0)
     {
      thesparse=i;
      break;
     }

 //
 // Count unused amount
 //
 numt=0;
 for (i=0;i<tilenum;i++)
   if (XMSlookup[i]>=0 && !tarray[i])
     numt++;

 numtm=0;
 for (i=0;i<tilemnum;i++)
   if (XMSlookup[i+tilenum]>=0 && !tmarray[i])
     numtm++;


 //
 // INPUT FROM DIALOG
 //
 do
 {
  MouseHide();
  xormask=0;
  sx=dx+2;
  sy=dy+17;
  print("Unused TILES:");
  printint(numt);
  print(", unused MASKED:");
  printint(numtm);
  print(" ");
  sx=dx+2;
  sy=dy+18;
  print("TILE Memory:");
  printint((1023l+(long)numt*nsize)/1024);
  print("K, MASKED Memory:");
  printint((1023l+(long)numtm*nmsize)/1024);
  print("K ");
  MouseShow();
  DrawUnused(0);
  redraw=0;

  do
  {
   if (!MouseButton())
     which=CheckButtonsRet(&CMTd);
   MouseCoords(&pixelx,&pixely);
   pixelx/=8;
   pixely/=8;
   if (MouseButton() && pixelx>dx && pixelx<dx+37 && pixely>dy && pixely<dy+18)
     {
      int tile=((pixelx-dx-1)>>(tsize-1))+((pixely-dy-1)>>(tsize-1))*(9<<(3-tsize));

      switch(whichscreen)
      {
       case TILES:
	 if (!tarray[tile+tilebase] && XMSlookup[tile+tilebase]>=0)
	   {
	    tarray[tile+tilebase]=1;
	    numt--;
	    redraw=1;
	   }
	 break;
       case MASKED:
	 if (!tmarray[tile+tilembase] && XMSlookup[tilenum+tile+tilembase]>=0)
	   {
	    tmarray[tile+tilembase]=1;
	    numtm--;
	    redraw=1;
	   }
      }
      continue;
     }

   switch(which)
   {
    case 0:
    case 1:
      exit=1;
      continue;

    case 2:
      MouseHide();
      GetButtonXY(&CMTd,which-1,&sx,&sy);
      print(CMTb[which-1].text);
      whichscreen=TILES;
      DrawUnused(0);
      MouseShow();
      continue;

    case 3:
      MouseHide();
      GetButtonXY(&CMTd,which-1,&sx,&sy);
      print(CMTb[which-1].text);
      whichscreen=MASKED;
      DrawUnused(0);
      MouseShow();
      continue;

    //
    // REBUILD GRAPHICS FILE & HEADER
    //
    case 4:
      {
       char newgfx[13]="NEW?GA.",
	    newhead[13]="NEW?HEAD.",
	    newhobj[13]="NEW?HEAD.",
	    oldhead[13]="?GAHEAD.";

       char _seg *gfxhead;


       newgfx[3]=format[0];
       newhead[3]=format[0];
       newhobj[3]=format[0];
       oldhead[0]=format[0];


       LoadIn(oldhead,(memptr *)&gfxhead);
       for (i=0;i<tilenum;i++)
       {
	if (tarray[i])
	  *(gfxhead+i*3)=*(gfxhead+i*3);
       }


      }
      continue;
   }
   //
   // KEY CHECKS
   //
   if (keydown[1])
     {
      exit=1;
      continue;
     }
   else
   if (keydown[0x50])
     DrawUnused(1);
   else
   if (keydown[0x48])
     DrawUnused(-1);
   else
   if (keydown[0x49])
     {
      DrawUnused(-8);
      while(keydown[0x49]);
     }
   else
   if (keydown[0x51])
     {
      DrawUnused(8);
      while(keydown[0x51]);
     }

  } while(!redraw && !exit);
 } while(!exit);

 RestoreBackground();
 LoadMap(whichmap);
 tilebase=tilembase=0;

 xbase=oxbase;
 ybase=oybase;
}


//
// FILL THE "UNUSED TILES" SCREEN WITH TILES
//
void DrawUnused(int deltarow)
{
 char tile[32*32];
 int numcols,numrows,i,j,top;

 if (whichscreen==ICONS)
   whichscreen=TILES;

 MouseHide();

 numrows=18-10*(tsize==2)-15*(tsize==3);
 left=2;
 top=2*8;
 if (videomode==EGA2)
   {
    left=22;
    top=20*8;
   }

 switch(tsize)
 {
  case 1: numcols=36; break;
  case 2: numcols=18; break;
  case 3: numcols=9;
 }

 if (whichscreen==TILES && tilebase+numrows*numcols>tilenum)
   {
    tilebase=0;
    numrows=tilenum/numcols;
   }

 if (whichscreen==MASKED && tilembase+numrows*numcols>tilemnum)
   {
    tilembase=0;
    numrows=tilemnum/numcols;
   }

 switch((deltarow<0?-1:deltarow>0?1:0))
 {
  case -1:
   switch(whichscreen)
   {
    case TILES:
      tilebase+=deltarow*numcols;
      if (tilebase<0)
	tilebase=0;
      break;
    case MASKED:
      tilembase+=deltarow*numcols;
      if (tilembase<0)
	tilembase=0;
   }
   break;
  case 1:
   switch(whichscreen)
   {
    case TILES:
      tilebase+=deltarow*numcols;
      if (tilebase+numrows*numcols>tilenum)
	tilebase=tilenum-numcols*numrows;
      break;
    case MASKED:
      tilembase+=deltarow*numcols;
      if (tilembase+numrows*numcols>tilemnum)
	tilembase=tilemnum-numcols*numrows;
   }
 }

 switch(whichscreen)
 {
  case TILES:
    for(j=0;j<numrows;j++)
      for(i=0;i<numcols;i++)
      {
       if (!tarray[tilebase+j*numcols+i] && XMSlookup[tilebase+j*numcols+i]>=0)
	 CombineTiles(tilebase+j*numcols+i,0,0,tsize);
       else
	 CombineTiles(thesparse,0,0,tsize);
       DrawTile(i*2+left,j*16+top,tsize);
      }
    break;

  case MASKED:
    for(j=0;j<numrows;j++)
      for(i=0;i<numcols;i++)
      {
       if (!tmarray[tilembase+j*numcols+i] && XMSlookup[tilenum+tilembase+j*numcols+i]>=0)
	 CombineTiles(-BkgndColor,tilenum+tilembase+j*numcols+i,0,tsize);
       else
	 CombineTiles(thesparse,0,0,tsize);
       DrawTile(i*2+left,j*16+top,tsize);
      }
 }
 MouseShow();
}
