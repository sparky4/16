////////////////////////////////////////////////////
//
// TED 5
// MultiVideo mode, MultiTile size, MultiMap editor
// by John Romero (C) 1991 Id Software
//
// Development Log:
// -------------------------------------------------
// Mar 19 91 - Starting this thing! Getting it to
//      compile with the JHUFF and MEM stuff.
//
// Mar 20 91 - Got the whole deal compiling and wrote
//      the EGA drawchar routine and got most of
//      Dialogs written!
//
// Mar 25 91 - Forgot some days in there! Got the XMS
//      stuff going and started the big initialization
//      part. Past init will be lots easier!
//
// Mar 30 91 - Got map selection and dimensioning and
//      allocation done. Added more MENU abilities to
//      allow more flexible dialogs.
//
// Apr  7 91 - Got project initialization done. Got
//      tiles into XMS. Wrote XMS routines for all
//      memory functions needed.
//
// Apr 12 91 - FINALLY got the maps loading and saving
//      correctly - NO THANKS TO MR.C OVER THERE! You
//      see, he can't remember whether his expansion
//      routines use the compressed or expanded length
//      and that was causing me quite a problem. Got
//      it solved and the map shuffling shit works
//      PERFECTLY now!  Got tile select in, map stats,
//      and other stuff...
//
// Apr 19 91 - Got all 3 planes working (drawing,etc.)
//      TileSelect screens jam...lots of little options
//      thrown in... !!! I got my '75 Cougar !!!
//
// Apr 25 91 - Got map dimension changing done! Fixed
//      some small bugs...had a WHOPPER of a fucking
//      bug today ... I was overwriting the stack
//      when the first background save happened. I'm SURE
//      I allocated enough memory -- I don't know...
//
// Apr 27 91 - Map Edge Select works (for Copy) & now I
//      draw a frame around the entire region.
//
// May 01 91 - Added a nice feature - retaining the Copy
//      buffer wherever it may get toasted. Used XMS so
//      I didn't chew mainmem. Also made it so you can
//      turn the plane READ/WRITEs ON/OFF (obvious advantage).
//      Got Copy/Paste from everywhere with FloatingPaste!
//
// May 03 91 - Got entering values for infoplane stuff & all
//      that shit encompasses. Trying to get a cursor drawn
//      (with backgrnd save/restore) in CGA & EGA3 modes.
//      Wish me luck! ... FUCK IT! I removed EGA3 mode -- it
//      was too much shit & kludgery to get a cursor going.
//
// May 08 91 - Well! Let's see...I got Huffman map compression
//      done; wrote a MakeOBJ function that can be used within
//      the Memory Manager or Standard Borland C. Added .OBJ
//      generation to IGRAB. Started the function to generate
//      graphic map dumps. Modified the MapHeader stuff more
//      when John needed "extras".
//
// May 09 91 - Finished ILBM & Apple Preferred map dumps!
//      Fixed a couple hidden bugs, made some things nicer,
//      started on Flood Fill...
//
// May 11 91 - Got the TILEINFO/M stuff structured and loading
//      and saving (with compression). Putting all the "hidden"
//      commands in the menus...
//
// May 13 91 - Stuck in a PROJECT SELECT option (for the Keen2
//      trilogy, it'll be nice).
//
// May 16 91 - Got all of TILEINFO stuff finished. Tom and
//      Jason's first day! Tom BETAed IGRAB & TED5 big time.
//      Had to basically dump the GraphHeaderStr and just
//      make the ?GAHEAD.ext file all dataoffsets! (The simpler,
//      the better!)
//
// May 19 91 - Almost got the Flood Fill mode working...Tom
//      started drawing maps a little early & now I need to add
//      a little kludge-fix to reformat his old maps...
//
// May 22 91 - Got AWESOME UNDO working ... fixed the nagging little
//      paste/scrolling bug fixed (separated the PasteErase
//      & PasteDraw routines) ... finished Block Fill ... fixed
//      the floating paste/tile showthru bug ...
//
// May 27 91 - Basically finished up. Tom is BETAing now. I need
//      to make SelectMap accept the arrows & make an array for
//      the map names instead of the sorry way I'm doing it now.
//
// Jun 2  91 - Got level launching working!  Been working on
//      TED5 and IGRAB docs and MUSE most of the time.
//
// Jun 18 91 - v0.13 because John C.'s system didn't have
//      the SCmapmask set to 15 in CopyEGA!
//
// Jun 20 91 - v0.14: It's annoying to LAUNCH the game and come
//      back at the top-left corner!
//
// Jun 26 91 - v0.17: Made LAUNCH with ICON CHANGE ctrl-alt-L
//      because Tomshit was having problems...adding Unused Tile
//      Scan...fixing a couple minor Tom-irritants...
//
// Jul 12 91 - v0.23: Made VGA & CGA work! Fixed a small
//      bug with plane-on starting...added a GridMode for the
//      FloatingPaste...XMS Map-Cache...now saves the Launchname
//      in TEDINFO? so user program doesn't need to send anything
//      back but /LAUNCHED...finally fixed nagging FloatingPaste
//      multi-plane viewing bug...
//
// Jul 19 91 - v0.24: Changed TEDINFO structure and added GFXINFO
//      to IGRAB. Got most all bugs out...lowercase input now...
//
// Jul 23 91 - v0.26: Only write out .H if it needs to...got the
//      SnapPaste feature in that Tom wanted...
//
// Aug 08 91 - v0.28: I can't believe this shit! Mr. Tom decided
//      to enter the value "0xF00D" into the map, which was my secret
//      value for passing an ESC press back from "inputint". I made
//      it a #define now!
//
// Aug 09 91 - v0.29: I FUCKING FRIED THE LIB_A.ASM & LIB.C FILES!
//      THESE USED TO HOLD TED5'S TILE-DRAWING AND CGA/VGA ROUTINES!
//      AAAARRRRGGGG!!!! Trying to recontruct the program. It'll
//      take a bit (I was trying to fix a bug in EditMapNames).
//
// Aug 14 91 - v0.30: Just got finished fixing the FRIED SOURCE HELL!
//
// Aug 16 91 - v0.31: Heh heh, I added the ViewMap function!
//
// Aug 19 91 - v0.32: Added the MapImport function and it works! YES!
//
// Aug 22-23 - v0.35: Fixed a WHOLE BUNCH of little things and added
//      some cool features like easier TILEINFO entering, default
//      Import drive, F3 to toggle Paste Overlay, etc. Very nice now.
//
// Aug 24 91 - v0.38: Fixed a bug in passing multiple command-line
//      parameters to LAUNCHed program. Let user enter new parms and
//      a new launch name while in TED5.  Added the CarmackCompression
//      and trashed the Huffman shit.
//
// Sep 07 91 - v0.40: Only ViewMap planes viewable instead of all; flag
//	currently loaded map in SelectMap dialog; check NumIconRows for
//	a bogus value for old TED5 compatibility; show map dimensions in
//	ChangeMapEdges dialog; CTRL-ALT-arrows will go to map edges.
//
// Sep 12 91 - v0.41: Let user PERMANENTLY change the Launch icon. Also,
//	the plane flags are always saved!
//
// Sep 27 91 - v0.43: Added REVIEW_MAP function. Fixed a hard-to-find bug
//	in the BlockFill-with-pattern function.
//
// Sep 29 91 - v0.44: Fixed small annoying bug w/Flood-fill w/pattern.
//
// Oct 12 91 - v0.45: Added the PrintReport feature after Carmacizing
//	all the maps.
//
// Nov 24 91 - v0.48: Saving the Import Path, added TILEINFOM copying
//	from one area to another, getting rid of the "turning the plane
//	viewing ON automatically sets it for writing."
//
// May 22 92 - v0.49: Added graphics-select switch to get rid of a dialog.
//
// Oct 20 95 - v0.50: Fixed EGA scrolling bug, added screensaver which comes
//                    on automatically after 2 minutes.  Timelimit adjustable
//                    through TIMELIMIT command line MED
//
////////////////////////////////////////////////////
#include "ted5.h"
#pragma hdrstop

extern unsigned _stklen=0x2000;

////////////////////////////////////////////////////
//
// Variables
//
////////////////////////////////////////////////////
extern char far TEDCHAR,far VGAPAL,tdata;
extern unsigned doubled[256];

UndoStr UndoRegion;
CopyStr TileCopy;
MapFileHeaderStr _seg *MapFileHeader;
char _seg *Tinfo[10],_seg *TMinfo[10],_seg *GraphHeader;
long _seg *XMSlookup,_seg *EgaXMSlookup,_seg *CgaXMSlookup,_seg *VgaXMSlookup;
int _seg *MapBkgnd,_seg *MapFrgnd,_seg *MapInfoPl,
    _seg *CutBkgnd,_seg *CutFrgnd,_seg *CutInfoPl;
MapHeaderStr MapHeader;

TempStruct LaunchInfo;
InfoStruct _seg *TEDInfo;
GfxStruct _seg *GFXInfo;
video lastvideo,videomode;
screentype whichscreen=TILES;
VMapStr VMapData;

char launchname[64],ext[4],format[2],projname[64],mapname[64],planes,
	infoname[64],mapheadname[64],MapNames[100][16],parmstring[64];
char SM_name[64],SM_loadname[64],BkgndColor,GfxToUse;

unsigned temp,whichmap,numtplanes,tilenum,tilemnum,numtmplanes,left,
	DirtyFlag,tilelen,tilemlen,whicht,whichtm,whichi,
	tsize,infoy,infomaxw,mapwidth,mapheight,screenw,usingbat,
	screenh,planeton,planemon,planeion,maxiconrows,lasticon,firsticon,
	viewton,viewmon,viewion,XMSundoB,XMSundoF,XMSundoI,launched,
	XMSmaps,EgaXMS,CgaXMS,VgaXMS,xmshandle,GridMode,SnapMode,snapx,
	snapy,snapxsize,snapysize,writeH,NoAbout,F3_flag;
int tilebase=0,tilembase=0,infobaron=1,xbase,ybase,scrnbot,scrnrgt,
	FillMode=0,PasteMode=0,SelectMode=0,SelX1=-1,SelY1=-1,PasteOK=0,SelX2=-1,
	SelY2=-1,pixelx,pixely,selectcols,px,py,lastmap=-1,TIybase,TIymbase,TIxbase,
	TIxmbase,BfillMode,Plotting,TsearchMode,NoXMSFlag;
long CgaXMSsize,EgaXMSsize,VgaXMSsize;
long tics, tictime=1092L*2L;


//
// harderr-called routine
//
int ignore(void)
{
 hardresume(0);
 return 0;
}

////////////////////////////////////////////////////
//
// Start of The Beast From Hell
//
////////////////////////////////////////////////////
void main(void)
{
 ParseCmdline();
 SetupKBD();
 MMStartup();
 setvideo(EGA1);
 lastvideo=EGA1;
 InitTed5();
 harderr(ignore);
 tics=biostime(0,0);
 DeskEventLoop(HandleEvent,Continuous);
}

////////////////////////////////////////////////////
//
// Parse the commandline
//
////////////////////////////////////////////////////
void ParseCmdline(void)
{
 int i;

 for (i=1;i<_argc;i++)
 {
  _argv[i]=strupr(_argv[i]);

  if (_argv[i][0]=='-' || _argv[i][0]=='/')
	_argv[i]++;

  if (!strcmp(_argv[i],"?"))
	{
	 printf(TITLESTR" by John Romero (C) 1991 Id Software, Inc.\n\n");
	 printf("Command Line parameters:\n");
	 printf("/?              : gets this stuff\n");
	 printf("/EXT=???        : set the project extension\n");
	 printf("<filename>      : set the Launch filename\n");
	 printf("/PARMS=<string> : set parms to Launch with\n");
	 printf("/NOXMSMAPS      : don't cache maps in XMS\n");
    printf("/GFX=???        : set gfx to use - E,C,V\n");
	 printf("/TIME=???       : half-minutes until screenblanker (default 2 minutes)\n");
    exit(0);
	}
  else
  if (!strncmp(_argv[i],"EXT=",4))
	strcpy(ext,&_argv[i][4]);
  else
  if (!strncmp(_argv[i],"GFX=",4))
	GfxToUse = _argv[i][4];
  else
  if (!strcmp(_argv[i],"LAUNCH"))
	launched=1;
  else
  if (!strcmp(_argv[i],"BAT"))
	usingbat=1;
  else
  if (!strncmp(_argv[i],"PARMS=",6))
	strcpy(parmstring,&_argv[i][6]);
  else
  if (!strncmp(_argv[i],"TIME=",5))
	  tictime=(atol(&_argv[i][5]))*546L;
  else
  if (!strcmp(_argv[i],"NOXMSMAPS"))
	NoXMSFlag=1;
  else
  if (!strcmp(_argv[i],"NOABOUT"))
	NoAbout=1;
  else
	strcpy(launchname,_argv[i]);
 }
}


////////////////////////////////////////////////////
//
// Event handler - called when button is pressed
// outside menu bar
//
////////////////////////////////////////////////////
void HandleEvent(void)
{
 int pixelx,pixely,mx,my,b0,b1;
 unsigned loc;

 b0=MouseButton()&1;
 b1=(MouseButton()>>1)&1;

 MouseCoords(&mx,&my);
 pixely=my;
 pixelx=mx;

 //
 // PLOT OR PICK-UP TILE
 //
 if (my>=8 && my<infoy*8 && (b0 || b1))
   {
	mx=xbase+(mx>>(tsize+2));
    my=ybase+((my-8)>>(tsize+2));

    loc=my*mapwidth+mx;

    if (mx>=mapwidth || my>=mapheight)
      errsound();
    else
      {
       if (b1)
	 {
	  //
	  // SELECT BOTTOM-RIGHT EDGE
	  //
	  if (SelectMode || BfillMode)
	    {
	     SelX2=mx;
	     SelY2=my;
	     if ((SelX1==-1 && SelY1==-1) ||
		 (SelX2<SelX1 || SelY2<SelY1))
	       {
		SelX1=mx;
		SelY1=my;
	       }
	     DrawMap();
	     sound(2000);
	    }
	  //
	  // FLOOD FILL!
	  //
	  else
	  if (FillMode)
	    {
	     while(MouseButton());
	     DoFloodFill(mx,my,1);
	    }
	  //
	  // PICK UP TILE(S)
	  //
	  else
	    {
	     if (planeton)
	       whicht=*(MapBkgnd+loc);
	     if (planemon)
	       whichtm=*(MapFrgnd+loc)+tilenum;
		 if (planeion)
	       whichi=*(MapInfoPl+loc)+tilenum;
	     DrawInfoBar();

	     //
	     // IF WE'RE IN TILESEARCH MODE, SHOW IT!
	     //
	     if (TsearchMode)
	       DrawMap();
	    }
	  while(MouseButton()>>1);
	  nosound();
	 }
       if (b0)
	 {
	  //
	  // SELECT TOP-LEFT EDGE
	  //
	  if (SelectMode || BfillMode)
	    {
	     SelX1=mx;
	     SelY1=my;
	     if ((SelX2==-1 && SelY2==-1) ||
		 (SelX1>SelX2 || SelY1>SelY2))
	       {
		SelX2=mx;
		SelY2=my;
	       }

	     DrawMap();
	     sound(2000);
	     while(MouseButton());
	    }
	  //
	  // FLOOD FILL!
	  //
	  else
	  if (FillMode)
	    {
	     while(MouseButton());
	     DoFloodFill(mx,my,0);
	    }
	  //
	  // PASTE A CHUNK O' TILES/MAP
	  //
	  else
	  if (PasteMode)
	    {
	     if (TileCopy.MapOrTileSelect)      // TILE-SELECT AREA?
	       {
		int i,j;

		if (SnapMode)
		  {
		   mx=(mx/snapxsize)*snapxsize+snapx;
		   my=(my/snapysize)*snapysize+snapy;
		  }

		if (mx+TileCopy.w>mapwidth ||
		    my+TileCopy.h>mapheight)
		sound(500);
		else
		  {
		   CopyUndoRegion();
		   UndoRegion.x=mx;
		   UndoRegion.y=my;
		   UndoRegion.w=TileCopy.w;
		   UndoRegion.h=TileCopy.h;

		   sound(500);
		   switch(TileCopy.MapOrTileSelect)
		   {
		    case 1: // TILES
		      for (j=0;j<TileCopy.h;j++)
			for (i=0;i<TileCopy.w;i++)
			  {
			   int val=(j+TileCopy.y)*selectcols+TileCopy.x+i;
			   if (XMSlookup[val]!=-1)
			     MapBkgnd[(my+j)*mapwidth+mx+i]=val;
			  }
		      break;
		    case 2: // MASKED
		      for (j=0;j<TileCopy.h;j++)
			for (i=0;i<TileCopy.w;i++)
			  {
			   int val=(j+TileCopy.y)*selectcols+TileCopy.x+i+tilenum+maxiconrows*selectcols;
			   if (XMSlookup[val]!=-1)
			     MapFrgnd[(my+j)*mapwidth+mx+i]=val-tilenum;
			  }
		   }
		   nosound();
		   DrawMap();
		   DirtyFlag=1;
		  }
		while(MouseButton());
		nosound();
	       }
	     else
	       {
		int i,j;

		if (SnapMode)
		  {
		   mx=(mx/snapxsize)*snapxsize+snapx;
		   my=(my/snapysize)*snapysize+snapy;
		  }

		if (mx+TileCopy.w-1<mapwidth && my+TileCopy.h-1<mapheight)
		  {
		   sound(500);
		   CopyUndoRegion();
		   UndoRegion.x=mx;
		   UndoRegion.y=my;
		   UndoRegion.w=TileCopy.w;
		   UndoRegion.h=TileCopy.h;

		   for (j=0;j<TileCopy.h;j++)
		     for (i=0;i<TileCopy.w;i++)
		       {
			int theM,theI;


			if (TileCopy.PlanesCopied&BPLANE)
			  MapBkgnd[(j+my)*mapwidth+mx+i]=
			    CutBkgnd[(j+TileCopy.y)*mapwidth+TileCopy.x+i];
			if (TileCopy.PlanesCopied&FPLANE)
			  {
			   theM=CutFrgnd[(j+TileCopy.y)*mapwidth+TileCopy.x+i];
			   if (theM || F3_flag)
			     MapFrgnd[(j+my)*mapwidth+mx+i]=theM;
			  }
			if (TileCopy.PlanesCopied&IPLANE)
			  {
			   theI=CutInfoPl[(j+TileCopy.y)*mapwidth+TileCopy.x+i];
			   if (theI || F3_flag)
				 MapInfoPl[(j+my)*mapwidth+mx+i]=theI;
			  }
		       }
		   nosound();
		   DrawMap();
		   DirtyFlag=1;
		  }
		else
		  sound(500);

		while(MouseButton());
	       }
	    }
	  //
	  // PLOT TILE(S)
	  //
	  else
	    {
	     unsigned oldt,oldm,oldi;

	     //
	     // ARE WE STARTING THE PLOTTING REGION?
	     //
	     if (!Plotting)
	       {
		CopyUndoRegion();
		UndoRegion.x=mx;
		UndoRegion.y=my;
		UndoRegion.w=UndoRegion.h=1;
		Plotting=1;
	       }

	     //
	     // FLOAT THE PLOTTING REGION
	     //
	     if (mx<UndoRegion.x)
	       UndoRegion.x=mx;
	     if (my<UndoRegion.y)
	       UndoRegion.y=my;
	     if (mx+1>UndoRegion.x+UndoRegion.w)
	       UndoRegion.w=mx-UndoRegion.x+1;
	     if (my+1>UndoRegion.y+UndoRegion.h)
	       UndoRegion.h=my-UndoRegion.y+1;

	     if (planeton)
		   *(MapBkgnd+loc)=whicht;
	     if (planemon)
	       *(MapFrgnd+loc)=whichtm-tilenum;
	     if (planeion)
	       *(MapInfoPl+loc)=whichi-tilenum;

	     oldt=MapBkgnd[loc];
	     oldm=MapFrgnd[loc]+tilenum;
	     oldi=MapInfoPl[loc]+tilenum;

	     CombineTiles(viewton?oldt:-BkgndColor,viewmon*oldm,oldi*viewion,tsize);
	     if (GridMode)
	       Overlay(tsize);
	     MouseHide();
	     DrawTile((mx-xbase)<<(tsize-1),(my-ybase)*(4<<tsize)+8,tsize);
	     CheckInfoValues(mx-xbase,my-ybase,oldi);
	     MouseShow();
	     DirtyFlag=1;
	    }
	 }
       nosound();
      }
   }

 //
 // CLICK ON TILES
 //
 if (b0 && pixely>infoy*8)
   {
    if (pixelx<7*8)
      SelectTiles(1);
    else
    if (pixelx<8*14 && tilemnum)
      SelectTiles(2);
    else
    if (pixelx<8*20 && tilemnum)
      SelectTiles(3);
   }

}

#define PEL_WRITE_ADR   0x3c8
#define PEL_READ_ADR    0x3c7
#define PEL_DATA        0x3c9
#define PEL_MASK        0x3c6

void ScreenBlank ( void )
{
  int done;
  int x;
  int y;
  int r;
  int g;
  int b;
  int c;
  int oldx,oldy;
  int i, j;
  int xdir,ydir;
  int cury;
  int size;
  int type;
  struct {
	  int x,y;
	  } snake[256];

  unsigned char far * screen=(unsigned char far *)0xa0000000l;


  randomize();
  _AX=0x13;
  asm int 10h
  done=0;
  size=random(10)+2;
  clearkeys();

  for (i=255;i>=0;i--)
	  {
	  snake[i].x=160;
	  snake[i].y=100;
	  }
  type=random(11);
  r=random(200)+56;
  g=random(200)+56;
  b=random(200)+56;
  outp (PEL_WRITE_ADR,0);
  for (i=0;i<=255;i++)
		{
		outp (PEL_DATA, (unsigned char)((i*r)>>10));
		outp (PEL_DATA, (unsigned char)((i*g)>>10));
		outp (PEL_DATA, (unsigned char)((i*b)>>10));
		}
  xdir=0;
  while (!xdir)
	  xdir=random(size<<1)-size;
  ydir=0;
  while (!ydir)
	  ydir=random(size<<1)-size;
  while (!done)
	  {
	  if ((random(100)-80)>0)
		  {
		  xdir+=random(3)-1;
		  ydir+=random(3)-1;
		  }
	  for (i=255;i>0;i--)
		  snake[i]=snake[i-1];
	  snake[0].x+=xdir;
	  snake[0].y+=ydir;
	  if ((snake[0].x<size+1) || (snake[0].x>320-(size+1)))
		  {
		  xdir=-xdir;
		  snake[0].x+=xdir<<1;
		  }
	  else if (abs(xdir)>size-1)
		  xdir=-(xdir>>1);
	  if ((snake[0].y<size+1) || (snake[0].y>200-(size+1)))
		  {
		  ydir=-ydir;
		  snake[0].y+=ydir<<1;
		  }
	  else if (abs(ydir)>size-1)
		  ydir=-(ydir>>1);
	  for (x=255;x>=0;x--)
		  {
		  if (type<7)
			  {
			  for (j=snake[x].y,cury=320*snake[x].y;j<snake[x].y+size;j++,cury+=320)
				  _fmemset((screen+cury+snake[x].x),255-x,size);
			  }
		  else
			  {
			  _fmemset((screen+(320*snake[x].y)+snake[x].x),255-x,size);
			  }
		  }
	  oldx=pixelx;
	  oldy=pixely;
	  MouseCoords(&pixelx,&pixely);
	  if ((oldx!=pixelx) || (oldy!=pixely))
		  done=1;
	  for (x=0;x<127;x++)
		  if (keydown[x])
			  done=1;
	  }
}

////////////////////////////////////////////////////
//
// Routine called continuously by DeskEventLoop
//
////////////////////////////////////////////////////
void Continuous(void)
{
 static int oldx, oldy;
 if (biostime(0,0)-tics>tictime)
	 {
	 MouseHide();
	 ScreenBlank();
	 clearkeys();
	 videomode=lastvideo;
	 Item_ModeSwitch();
	 RedrawDesktop();
	 DrawMap();
	 DrawInfoBar();
	 MouseShow();
	 tics=biostime(0,0);
	 }
 oldx=pixelx;
 oldy=pixely;
 MouseCoords(&pixelx,&pixely);
 if ((oldx!=pixelx) || (oldy!=pixely))
	 {
	 tics=biostime(0,0);
	 PrintCoords();
	 }

 if (!MouseButton())
   Plotting=0;

 //
 // PLANE "WRITE" SELECTION
 //
 if (keydown[2] && viewton)
   {
    planeton^=1;
    DrawInfoBar();
    DrawMap();
    PrintCoords();
    tics=biostime(0,0);
    while(keydown[2]);
   }
 if (keydown[3] && tilemnum && (MapFileHeader->maptype&FPLANE) && viewmon)
   {
    planemon^=1;
    DrawInfoBar();
    DrawMap();
    PrintCoords();
    tics=biostime(0,0);
    while(keydown[3]);
   }
 if (keydown[4] && (MapFileHeader->maptype&IPLANE) && viewion)
   {
    planeion^=1;
    DrawInfoBar();
    DrawMap();
    PrintCoords();
    tics=biostime(0,0);
    while(keydown[4]);
   }

 //
 // PLANE "VIEW" SELECTION
 //
 if (keydown[5])
   {
    viewton^=1;
    if (!viewton)
	  planeton=0;
    DrawInfoBar();
    DrawMap();
    PrintCoords();
    tics=biostime(0,0);
    while(keydown[5]);
   }
 if (keydown[6] && tilemnum && (MapFileHeader->maptype&FPLANE))
   {
    viewmon^=1;
    if (!viewmon)
      planemon=0;
    DrawInfoBar();
    DrawMap();
    PrintCoords();
    tics=biostime(0,0);
    while(keydown[6]);
   }
 if (keydown[7] && (MapFileHeader->maptype&IPLANE))
   {
    viewion^=1;
    if (!viewion)
      planeion=0;
    DrawInfoBar();
    DrawMap();
    PrintCoords();
    tics=biostime(0,0);
    while(keydown[7]);
   }

 //
 // Cancel COPY or PASTE or FLOOD FILL or BLOCK FILL
 //
 if (keydown[1] && (PasteMode || SelectMode || FillMode ||
	 BfillMode || TsearchMode))
   {
    while(keydown[1]);

    if (PasteMode)
      {
       EraseFloatPaste();
       px=py=-1;
      }

    SnapMode=TsearchMode=BfillMode=FillMode=PasteMode=SelectMode=0;
    SelX1=SelX2=SelY1=SelY2=-1;
    DrawMap();
    DrawInfoBar();
    tics=biostime(0,0);
   }

 //
 // END OF COPY || BLOCK FILL
 //
 if (keydown[0x1c] && (SelectMode || BfillMode))
   {
    int temp,j,i;

    tics=biostime(0,0);
    while(keydown[0x1c]);

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

    //
    // BLOCK FILL?
    //
    if (BfillMode)
      {
       BfillMode=0;
       DrawInfoBar();
       DoBlockFill();
       SelX1=SelX2=SelY1=SelY2=-1;
      }
    else
    //
    // COPY MODE?
    //
    if (SelectMode)
      {
       SelectMode=0;

       TileCopy.x=SelX1;
       TileCopy.y=SelY1;
       TileCopy.w=SelX2-SelX1+1;
	   TileCopy.h=SelY2-SelY1+1;
       TileCopy.MapOrTileSelect=0;
       TileCopy.PlanesCopied=planeton*BPLANE | planemon*FPLANE | planeion*IPLANE;

       //
       // DO THE COPY!
       //
       sound(600);
       for (j=SelY1;j<=SelY2;j++)
	 for (i=SelX1;i<=SelX2;i++)
	   {
	    if (planeton)
	      CutBkgnd[j*mapwidth+i]=MapBkgnd[j*mapwidth+i];
	    if (planemon)
	      CutFrgnd[j*mapwidth+i]=MapFrgnd[j*mapwidth+i];
	    if (planeion)
	      CutInfoPl[j*mapwidth+i]=MapInfoPl[j*mapwidth+i];
	   }

       DrawInfoBar();
       SelX1=SelX2=SelY1=SelY2=-1;
       PasteOK=1;
       px=py=-1;
       nosound();
       DrawMap();
      }
    clearkeys();
   }

 //
 // See if we want to scroll the map!
 //
 CheckMapScroll();
 CheckFloatPaste((pixelx>>(tsize+2))+xbase,((pixely-8)>>(tsize+2))+ybase);
}

////////////////////////////////////////////////////
//
// Draw the current map on the screen at xbase,ybase
//
////////////////////////////////////////////////////
void DrawMap(void)
{
 int i,j,imax,jmax;


 EraseFloatPaste();
 jmax=screenh;
 if (jmax>mapheight)
   jmax=mapheight;

 imax=screenw;
 if (imax>mapwidth)
   imax=mapwidth;

 MouseHide();
 for(j=0;j<jmax;j++)
   for(i=0;i<imax;i++)
     {
      unsigned tilet,tilem,tilei,loc;

      loc=(ybase+j)*mapwidth+xbase+i;

      tilet=*(MapBkgnd+loc);
      tilem=*(MapFrgnd+loc)+tilenum;
      tilei=*(MapInfoPl+loc)+tilenum;

      CombineTiles(tilet*viewton-BkgndColor*(!viewton),tilem*viewmon,tilei*viewion,tsize);
      if (GridMode)
	Overlay(tsize);
      DrawTile(i<<(tsize-1),j*(4<<tsize)+8,tsize);
      CheckInfoValues(i,j,tilei);
      CheckSelectEdges(i+xbase,j+ybase,i,j);
     }
 MouseShow();
 DrawFloatPaste();
}


////////////////////////////////////////////////////
//
// Figure out SCREENW,SCREENH from videomode & tsize
//
////////////////////////////////////////////////////
void FigureScreenEdges(void)
{
 switch(videomode)
 {
  case CGA:
  case EGA1:
  case VGA:
    screenw=40>>(tsize-1);
    screenh=22>>(tsize-1);
    break;
  case EGA2:
    screenw=80>>(tsize-1);
    screenh=57>>(tsize-1);
    break;
 }
 if (!infobaron)
   {
    infoy=100;  // WAY OFF THE BOTTOM!
    screenh+=2*(tsize==1)+(tsize==2);
   }
}


////////////////////////////////////////////////////
//
// Print the coords on the INFOBAR
//
////////////////////////////////////////////////////
void PrintCoords(void)
{
 static zeroed=0;
 int mx,my;

 if (!infobaron)
   return;

 xormask=0;
 MouseCoords(&mx,&my);
 if (my<8 || my>=infoy*8)
   {
    if (zeroed)
      return;
    sx=infomaxw-9;
    sy=infoy;
    print("??? ?????");
    sx=infomaxw-9;
    sy=infoy+1;
    print("??? ?????");
    zeroed=1;
    return;
   }

 zeroed=0;
 mx=mx>>(tsize+2);
 sx=infomaxw-9;
 sy=infoy;
 printint(mx+xbase);
 print("  ");
 sx=infomaxw-5;
 printhex(mx+xbase);

 my=(my-8)>>(tsize+2);
 sx=infomaxw-9;
 sy=infoy+1;
 printint(my+ybase);
 print("  ");
 sx=infomaxw-5;
 printhex(my+ybase);
}


////////////////////////////////////////////////////
//
// Draw the INFOBAR
//
////////////////////////////////////////////////////
void DrawInfoBar(void)
{
 int ox,oy;


 if (PasteMode)
   {
    px=py=-1;
    CheckFloatPaste((pixelx>>(tsize+2))+xbase,((pixely-8)>>(tsize+2))+ybase);
   }

 if (!infobaron)
   return;

 EraseFloatPaste();

 ox=sx=0;
 switch(videomode)
 {
  case CGA:
  case EGA1:
  case VGA:
    oy=sy=23-2*(tsize==3);
    infomaxw=40;
    break;
  case EGA2:
    oy=sy=58-2*(tsize==3)-1*(tsize==2);
    infomaxw=80;
 }

 MouseHide();
 infoy=oy;
 bar(0,infoy,infomaxw-1,infoy+1,' ');

 if (SelectMode)
   {
    sx=leftedge=1;
    sy=infoy;
    print("Copy Mode\nESC to exit");
   }
 else
 if (TsearchMode)
   {
    sx=leftedge=1;
    sy=infoy;
    print("Tile Search Mode\nESC to exit");
   }
 else
 if (BfillMode)
   {
    sx=leftedge=1;
    sy=infoy;
    print("Block Fill Mode\nESC to exit");
   }
 else
 if (PasteMode)
   {
    sx=leftedge=1;
    sy=infoy;
    print("Paste Mode\nESC to exit");
   }
 else
 if (FillMode)
   {
    sx=leftedge=1;
	sy=infoy;
    print("Flood Fill Mode\nESC to exit");
   }
 else
   {
    CombineTiles(whicht,0,0,tsize);
    DrawTile(ox,oy*8+8*(tsize==1),tsize);
    sx=ox+2+2*(tsize==3);
    sy=oy;
    printhex(whicht);
    sx-=5;
    sy++;
    printint(whicht);
    print("    ");

    if (tilemnum)
      {
       ox+=7;
       CombineTiles(-BkgndColor,whichtm,0,tsize);
       DrawTile(ox,oy*8+8*(tsize==1),tsize);
       sx=ox+2+2*(tsize==3);
       sy=oy;
       (whichtm==tilenum)?print(" No  "):printhex(whichtm-tilenum);
       sx-=5;
       sy++;
       (whichtm==tilenum)?print("Tile"):printint(whichtm-tilenum);
       print("    ");

       ox+=7;
       CombineTiles(-ICONBACK,(whichi>lasticon)?firsticon:whichi,0,tsize);
       DrawTile(ox,oy*8+8*(tsize==1),tsize);
       sx=ox+2+2*(tsize==3);
       sy=oy;
       (whichi==tilenum)?print(" No  "):printhex(whichi-tilenum);
       sx-=5;
       sy++;
       (whichi==tilenum)?print("Icon"):printint(whichi-tilenum);
       print("    ");
      }
   }


 sx=infomaxw-11;
 sy=infoy;
 print("X=");
 sy++;
 sx-=2;
 print("Y=");

 sx=infomaxw-((videomode==EGA2)?19:18);
 sy=infoy+1;
 print("123");
 if (SnapMode)
   {
    sx=infomaxw-((videomode==EGA2)?21:20);
    sy=infoy+1;
    print("S");
   }
 if (GridMode)
   {
    sx=infomaxw-((videomode==EGA2)?20:19);
    sy=infoy+1;
    print("G");
   }
 sx=infomaxw-((videomode==EGA2)?19:18);
 sy=infoy;
 (planeton)?print("B"):print(" ");
 (planemon)?print("F"):print(" ");
 (planeion)?print("I"):print(" ");

 sx=infomaxw-15;
 sy=infoy+1;
 print("456");
 sx=infomaxw-15;
 sy=infoy;
 (viewton)?print("b"):print(" ");
 (viewmon)?print("f"):print(" ");
 (viewion)?print("i"):print(" ");

 if (videomode==EGA2)
   {
    sx=screencenterx-strlen(MapHeader.name)/2;
    sy=infoy;
    print(MapHeader.name);
   }

 DrawFloatPaste();
 MouseShow();
}


////////////////////////////////////////////////////
////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
// Initialize TED5:
//
// * Project Select if multiple projects
// * If Project has NO LEVELS:
//   * Init Level 1
//   * Init TILEINFO
//   * Init TEDINFO file
// * Load TILES into XMS
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////
////////////////////////////////////////////////////
char bstrings[10][15];
btype ProjButns[10];
DialogDef ProjSelect={"Select the project to work on:",
		      30,0,0,&ProjButns[0],DrawProjBord};

void InitTed5(void)
{
 char pname[15];
 unsigned i,loop,which;

 MouseInit();
 MouseShow();
 if (!MouseStatus)
   {
	ErrDialog("Sorry, but TED5 will NOT\n"
		  "operate without a mouse!\n"," Press ENTER ");
	Quit("Only REAL developers have a mouse!");
   }
 MouseOrigin(0,0);

 ErrDialog("Initializing. One moment.","");
 //
 // Create bit-doubling lookup table
 // for CGA font creation
 //
 for (loop=0;loop<256;loop++)
   {
	unsigned result,temp=loop;

	asm         mov     ax,temp
	asm         mov     ah,al
	asm         mov     cx,8
	LOOP0:
	asm         shl     al,1
	asm         rcl     bx,1
	asm         shl     ah,1
	asm         rcl     bx,1
	asm         loop    LOOP0

	asm         xchg    bh,bl
	asm         mov     result,bx

	doubled[loop]=result;
   }

 //
 // Init everything!
 //
 InitXMS();
 FindGraphFile();
 LoadInfoFile();
 LoadMapHeader();

 RestoreBackground();

 LoadGraphStuff(0,TEDInfo->lastvid);
 DrawInfoBar();
 DrawMap();
 if (!launched && !NoAbout)
   Item_About();
 launched=0;
}


////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
// Load the MAPTEMP.ext header in...
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////
btype SelectTsizeb[]={{"  8x8  ",1,2,1},
		      {" 16x16 ",1,5,1},
			  {" 32x32 ",1,8,1}};
DialogDef SelectTsize={" Which tile size to use?"
		       ,26,10,3,&SelectTsizeb[0],STnot};

void LoadMapHeader(void)
{
 unsigned size,i,j,pflag;
 char types=0;


 strcpy(mapheadname,"MAPTHEAD.");
 strcat(mapheadname,ext);
 strcpy(mapname,"MAPTEMP.");
 strcat(mapname,ext);
 strcpy(SM_name,"MAPTEMP1.");
 strcat(SM_name,ext);
 strcpy(SM_loadname,"MAPTEMP.");
 strcat(SM_loadname,ext);

 if (access(mapheadname,0))
   {
    int i;
    //
    // Gotta create a new map file!
    //
    MMAllocate((memptr *)&MapFileHeader,sizeof(MapFileHeaderStr));
    for (i=0;i<sizeof(MapFileHeaderStr);i++)
      *((char _seg *)MapFileHeader+i)=0;

    MapFileHeader->RLEWtag=0xabcd;
    for (i=0;i<100;i++)
      {
       MapFileHeader->dataoffsets[i]=-1;
       MapFileHeader->datalengths[i]=0;
       memset(MapNames[i],0,16);
      }

    if (!GFXInfo->num8 &&
	!GFXInfo->num8m &&
	!GFXInfo->num16 &&
	!GFXInfo->num16m &&
	!GFXInfo->num32 &&
	!GFXInfo->num32m)
      {
       ErrDialog("Uhh...you 'neglected' to\n"
		 "grab tiles to use. Running\n"
		 "TED5 is quite useless at\n"
		 "this point, I'm afraid.","Duh!");
       Quit("Get some tiles ... quick! Me hungry!");
      }

    if (!GFXInfo->num8 &&
	!GFXInfo->num16 &&
	!GFXInfo->num32)
      {
       ErrDialog("You may have grabbed some\n"
		 "MASKED tiles, but I require\n"
		 "NON-MASKED tiles as a\n"
		 "minimum requirement!","Geez...");
       Quit("Please grab some normal tiles!");
      }

    types+=(GFXInfo->num8>0)+(GFXInfo->num16>0)+(GFXInfo->num32>0);

    redo:

    if (types>1)
      {
       int which;

       which=DoDialog(&SelectTsize);
       switch(which)
       {
	case 0:
	  Quit("");
	case 1:
	  if (!GFXInfo->num8)
	    which=0;
	  break;
	case 2:
	  if (!GFXInfo->num16)
	    which=0;
	  break;
	case 3:
	  if (!GFXInfo->num32)
	    which=0;
	  break;
       }

       MapFileHeader->tsize=TEDInfo->tsize=tsize=which;
	  }
    else
      {
       if (GFXInfo->num8)
	 TEDInfo->tsize=1;
       else
       if (GFXInfo->num16)
	 TEDInfo->tsize=2;
       else
       if (GFXInfo->num32)
	 TEDInfo->tsize=3;

       MapFileHeader->tsize=tsize=TEDInfo->tsize;
      }

    //
    // pick the planes that all maps will use
    //
    if (PickMorePlanes())
      goto redo;

    //
    // initialize TILEINFO/TILEINFOM
    //
    switch(tsize)
    {
     case 1:
       tilenum=GFXInfo->num8;
       tilemnum=GFXInfo->num8m;
       break;
     case 2:
       tilenum=GFXInfo->num16;
       tilemnum=GFXInfo->num16m;
       break;
     case 3:
       tilenum=GFXInfo->num32;
       tilemnum=GFXInfo->num32m;
    }
    InitTileinfo();
    if (numtplanes || numtmplanes)      // only input if applicable
      Item_EditTinfoNames();            // void where prohibited
    //
    // now create a map!
    //
    CreateMap(0);
	FigureScreenEdges();
    MapFileHeader->NumIconRows=maxiconrows=InputIconAmount();
   }
 //
 // MAP FILE ALREADY IN PLACE. LOAD STUFF IN...
 //
 else
   {
    memptr block,tempblock;

    LoadIn(mapheadname,(memptr *)&MapFileHeader);

    //
    // See if the NumIconRows is toasty (old TED5 compatibility)
    //
    if (MapFileHeader->NumIconRows>50)
      MapFileHeader->NumIconRows=4;

    //
    // has the TEDINFO?.ext file been changed?
    // if so, reconstruct pertinent data...
    //
    if (!TEDInfo->tsize)
      {
       tsize=TEDInfo->tsize=MapFileHeader->tsize;
       switch(tsize)
       {
	case 1:
	  tilenum=GFXInfo->num8;
	  tilemnum=GFXInfo->num8m;
	  break;
	case 2:
	  tilenum=GFXInfo->num16;
	  tilemnum=GFXInfo->num16m;
	  break;
	case 3:
	  tilenum=GFXInfo->num32;
	  tilemnum=GFXInfo->num32m;
       }
      }

    maxiconrows=MapFileHeader->NumIconRows;

    //
    // Read-in all the Map Names
	//
    for (i=0;i<100;i++)
      if (MapFileHeader->dataoffsets[i]!=-1)
	{
	 MapHeaderStr TempHead;

	 LoadFile(mapname,(char huge *)&TempHead,
	   MapFileHeader->dataoffsets[i],sizeof(MapHeaderStr));
	 strcpy(MapNames[i],TempHead.name);
	}

    FigureScreenEdges();

    if (!TEDInfo->level)
      {
       for(i=0;i<100;i++)
	 if (MapFileHeader->dataoffsets[i]!=-1)
	   {
	    whichmap=TEDInfo->level=i;
	    break;
	   }
      }
    else
      whichmap=TEDInfo->level;

    LoadMap(TEDInfo->level);

    //
    // IF WE WERE LAUNCHED AND CHARACTER POSITION WAS CHANGED,
    // PUT IT BACK!
    //
    if (launched && (TEDInfo->lastx || TEDInfo->lasty))
      {
       int i;

       for (i=0;i<mapwidth*mapheight;i++)
	 if (MapInfoPl[i]==TEDInfo->permicon)
	   {
	    MapInfoPl[i]=0;
	    MapInfoPl[TEDInfo->lasty*mapwidth+TEDInfo->lastx]=TEDInfo->permicon;
	    TEDInfo->lastx=TEDInfo->lasty=0;
	    DirtyFlag=1;
	    break;
	   }
      }

    //
    // POSITION SCREEN
    //
    xbase=TEDInfo->oscrx;
    ybase=TEDInfo->oscry;
    if (xbase+screenw>mapwidth)
      xbase=mapwidth-screenw;
    if (ybase+screenh>mapheight)
      ybase=mapheight-screenh;

    if (launched)
      _fmemcpy((void far *)parmstring,(void far *)TEDInfo->parmstring,64);

    //
    // LOAD TILEINFO/M AND ADJUST IF IT CHANGED
    //
    numtplanes=MapFileHeader->numtplanes;
    numtmplanes=MapFileHeader->numtmplanes;

    pflag=0;
    for (i=0;i<numtplanes;i++)
      {
       //
       // SPACE FOR OLD TILEINFO TO DECOMPRESS INTO
       //
       MMAllocate(&tempblock,MapFileHeader->oldtilenum);
       //
       // SPACE FOR OLD TILEINFO TO LOAD INTO
       //
       MMAllocate(&block,MapFileHeader->tileinfolen[i]);
       LoadFile(mapheadname,MK_FP(block,0),MapFileHeader->tileinfooff[i],MapFileHeader->tileinfolen[i]);
       //
       // DECOMPRESS FROM "BLOCK" TO "TEMPBLOCK"
       //
       RLEBExpand(MK_FP(block,0),MK_FP(tempblock,0),
	   MapFileHeader->oldtilenum,MapFileHeader->RLEWtag);
       MMFreePtr(&block);
       //
       // ALLOCATE TINFO ARRAY
       //
       MMAllocate((memptr *)&Tinfo[i],tilenum);
       //
       // MOVE FROM "TEMPBLOCK" TO "TINFO[I]" ARRAY
       //
	   if (MapFileHeader->oldtilenum<tilenum)
	 {
	  movedata((unsigned)tempblock,0,(unsigned)Tinfo[i],0,MapFileHeader->oldtilenum);
	  //
	  // IF NEW TILEINFO IS MORE, FILL END WITH 0s
	  //
	  for (j=MapFileHeader->oldtilenum;j<tilenum;j++)
	    *(Tinfo[i]+j)=0;
	  DirtyFlag=pflag=1;
	 }
       else
	 {
	  movedata((unsigned)tempblock,0,(unsigned)Tinfo[i],0,tilenum);
	  if (MapFileHeader->oldtilenum>tilenum)
	    DirtyFlag=pflag=2;
	 }

       MMFreePtr(&tempblock);
      }

    switch(pflag)
    {
     case 1:
       ErrDialog("The new TILEINFO data has\n"
		 "been expanded to accomodate\n"
		 "the newly grabbed tiles."," OK ");
       break;
     case 2:
       ErrDialog("The new TILEINFO data has\n"
		 "been shrunk due to a reduced\n"
		 "amount of tiles."," OK ");
    }

    pflag=0;
    if (tilemnum && (MapFileHeader->maptype&FPLANE))
      for (i=0;i<numtmplanes;i++)
	{
	 MMAllocate(&tempblock,MapFileHeader->oldtilemnum);
	 MMAllocate(&block,MapFileHeader->tileinfomlen[i]);
	 LoadFile(mapheadname,MK_FP(block,0),MapFileHeader->tileinfomoff[i],MapFileHeader->tileinfomlen[i]);
	 RLEBExpand(MK_FP(block,0),MK_FP(tempblock,0),
	     MapFileHeader->oldtilemnum,MapFileHeader->RLEWtag);
	 MMFreePtr(&block);
	 MMAllocate((memptr *)&TMinfo[i],tilemnum);
	 if (MapFileHeader->oldtilemnum<tilemnum)
	   {
	    movedata((unsigned)tempblock,0,(unsigned)TMinfo[i],0,MapFileHeader->oldtilemnum);
	    for (j=MapFileHeader->oldtilemnum;j<tilemnum;j++)
	      *(TMinfo[i]+j)=0;
	    DirtyFlag=pflag=1;
	   }
	 else
	   {
	    movedata((unsigned)tempblock,0,(unsigned)TMinfo[i],0,tilemnum);
	    if (MapFileHeader->oldtilemnum>tilemnum)
	      DirtyFlag=pflag=2;
	   }

	 MMFreePtr(&tempblock);
	}

    switch(pflag)
    {
     case 1:
       ErrDialog("The new TILEINFOM data has\n"
		 "been expanded to accomodate\n"
		 "the newly grabbed masked tiles."," OK ");
       break;
     case 2:
       ErrDialog("The new TILEINFOM data has\n"
		 "been shrunk due to a reduced\n"
		 "amount of tiles."," OK ");
    }

    #if 0
    //
    // TURN ON PLANES
    //
    viewton=planeton=1;
    if (MapFileHeader->maptype&FPLANE)
      viewmon=1;
    if (MapFileHeader->maptype&IPLANE)
      viewion=1;
    planemon=planeion=0;
    #endif

   }

 //
 // LOAD THE MAPFILE INTO XMS IF ENOUGH ROOM
 //
 if (1024L*XMSTotalFree()>2L*filelen(mapname) && !NoXMSFlag)
   {
    #define LBCSIZE     0x4000
    memptr block;
    long size,clen,coff;


    size=filelen(mapname);
    MMAllocate(&block,LBCSIZE);
    XMSmaps=XMSAllocate(size);

    //
    // LOAD ENTIRE MAPFILE FROM DISK TO XMS!
    //
    coff=0;
    do
    {
     clen=LBCSIZE;
     if (size<LBCSIZE)
       clen=size;

     LoadFile(mapname,MK_FP(block,0),coff,clen);
     XMSmove(0,(long)MK_FP(block,0),XMSmaps,coff,clen);
     size-=LBCSIZE;
     coff+=clen;
    } while(size>0);

    MMFreePtr(&block);
   }

}

void STnot(int x,int y)
{
 sx=x+10;
 sy=y+2;
 if (!GFXInfo->num8)
   print("<-Not available");
 else
   {
    printint(GFXInfo->num8);
    print(" total tiles");
   }
 sx=x+10;
 sy=y+5;
 if (!GFXInfo->num16)
   print("<-Not available");
 else
   {
    printint(GFXInfo->num16);
    print(" total tiles");
   }
 sx=x+10;
 sy=y+8;
 if (!GFXInfo->num32)
   print("<-Not available");
 else
   {
    printint(GFXInfo->num32);
    print(" total tiles");
   }
}


////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
// Load the graphheader file & ?GAGRAPH.ext file and stick it in XMS!
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////
int LoadGraphStuff(int rtn,video newvid)
{
 #define NUMFASTDECOMP  100     // # of tiles in fast decompress buffer

 char gname[14]="?GAHEAD.",gname1[14]="?GAGRAPH.",_seg *packed,_seg *unpack,
      dictname[14]="?GADICT.",hufftable[1020],_seg *CacheBlock;
 unsigned index,indexm,num,numm,i,realnum,realnumm,ox,oy,cacheon=0,_seg *tile_len,
      _seg *tilem_len,cacheon_h;
 long expsize,expmsize,xmsoff=0,unpackoff,unpackmax,unpacksize,
      unpackxms;
 video tempvid,pickedvid;

 char huge *offsets;



 strcat(gname,ext);
 strcat(gname1,ext);
 strcat(dictname,ext);
 gname[0]=format[0];
 gname1[0]=format[0];
 dictname[0]=format[0];

 if (!launched)
   switch(format[0])
   {
	case 'C': pickedvid=CGA; break;
	case 'E': pickedvid=EGA1; break;
	case 'V': pickedvid=VGA;
   }
 else
   pickedvid=TEDInfo->lastvid;

 //
 // VALIDATE GRAPHICS MODE
 //
 tempvid=rtn?newvid:pickedvid;
 switch(tempvid)
 {
  case CGA:
	gname[0]='C';
	if (access(gname,0))
	  {
	   if (rtn)
		 return 0;
	  }
	else
	  {
	   TEDInfo->lastvid=CGA;
	   dictname[0]=format[0]=gname1[0]='C';
	   setvideo(CGA);
	   InitDesktop(TED5MenuBar,1);
	   MouseShow();
	   break;
	  }
  case EGA1:
  case EGA2:
	gname[0]='E';
	if (access(gname,0))
	  {
	   if (rtn)
	 return 0;
	  }
	else
	  {
	   TEDInfo->lastvid=tempvid;
	   dictname[0]=format[0]=gname1[0]='E';
	   setvideo(tempvid);
	   InitDesktop(TED5MenuBar,1);
	   MouseShow();
	   break;
	  }
  case VGA:
	gname[0]='V';
	if (access(gname,0))
	  {
	   if (rtn)
	 return 0;

       gname[0]=format[0];
      }
    else
      {
       TEDInfo->lastvid=VGA;
       dictname[0]=format[0]=gname1[0]='V';
       setvideo(VGA);
       InitDesktop(TED5MenuBar,1);
       MouseShow();
       break;
      }
 }


 //
 // FIND HEADER & LOAD IT
 //
 if (access(gname,0))
   {
    char errstr[100]="Can't find the ";

    strcat(errstr,format);
    strcat(errstr,"GAHEAD.");
    strcat(errstr,ext);
    strcat(errstr,"\nfile! Maybe you didn't\n"
		  "copy it from the graphics\n"
		  "subdirectory?");

    ErrDialog(errstr," OK ");
    if (rtn)
      return 0;
    Quit("You're stupid! Copy the damn file!");
   }

 LoadIn(gname,(memptr *)&GraphHeader);

 switch(MapFileHeader->tsize)
 {
  case 1:
    index=GFXInfo->off8;
    indexm=GFXInfo->off8m;
    num=GFXInfo->num8;
    numm=GFXInfo->num8m;
    if (indexm==index+1)
      {
       ErrDialog("I'm sorry, but you need to\n"
		 "capture your 8x8 tiles\n"
		 "individually, and not in a\n"
		 "big chunk."," Alright ");
       if (rtn)
	 return 0;
       Quit("Regrab-time, bag o' shit!");
      }

    switch(tempvid)
    {
     case CGA: expsize=16; expmsize=32; break;
     case EGA1:
     case EGA2: expsize=32; expmsize=40; break;
	 case VGA: expsize=64; expmsize=128;
    }
    break;
  case 2:
    index=GFXInfo->off16;
    indexm=GFXInfo->off16m;
    num=GFXInfo->num16;
    numm=GFXInfo->num16m;
    switch(tempvid)
    {
     case CGA: expsize=64; expmsize=128; break;
     case EGA1:
     case EGA2: expsize=128; expmsize=128+32; break;
     case VGA: expsize=256; expmsize=512;
    }
    break;
  case 3:
    index=GFXInfo->off32;
    indexm=GFXInfo->off32m;
    num=GFXInfo->num32;
    numm=GFXInfo->num32m;
    switch(tempvid)
	{
     case CGA: expsize=256; expmsize=512; break;
     case EGA1:
     case EGA2: expsize=512; expsize=512+4*32; break;
     case VGA: expsize=1024; expmsize=2048;
    }
 }

 //
 // MOVE TILES INTO XMS MEMORY!
 //

 MMAllocate((memptr *)&packed,expmsize);
 unpackmax=expmsize*NUMFASTDECOMP;
 MMAllocate((memptr *)&unpack,unpackmax);


 tilelen=expsize;
 tilemlen=expmsize;
 offsets=MK_FP(GraphHeader,0);

 //
 // LOAD DICTIONARY IN & INITIALIZE IT
 //
 if (!launched)
   {
    char _seg *block;

    if (access(dictname,0))
      {
       char errst[200]="I can't find the \n";

       strcat(errst,dictname);
       strcat(errst," file!");

       ErrDialog(errst," OK ");
       if (rtn)
	 return 0;
       Quit("Look in the graphics grab directory!");
      }

    LoadIn(dictname,(memptr *)&block);
    movedata((unsigned)block,0,FP_SEG(hufftable),FP_OFF(hufftable),1020);
    MMFreePtr((memptr *)&block);
    OptimizeNodes((huffnode *)hufftable);
   }

 //
 // Count up the REAL number of tiles there are!
 // Build tables for tile lengths
 //
 MMAllocate((memptr *)&tile_len,num*2);
 for (realnum=i=0;i<num;i++)
 {
  int j;



  if (OFF3(offsets,i+index)!=0xffffff)
  {
   realnum++;
   if (OFF3(offsets,i+index+1)!=0xffffff)
     tile_len[i]=OFF3(offsets,i+index+1)-OFF3(offsets,i+index);
   else
     for (j=i+1;j<num+numm;j++)
       if (OFF3(offsets,j+index)!=0xffffff)
       {
	tile_len[i]=OFF3(offsets,j+index)-OFF3(offsets,i+index);
	break;
       }
  }
  else
    tile_len[i]=0;

 }
 MMAllocate((memptr *)&tilem_len,numm*2);
 for (realnumm=i=0;i<numm;i++)
 {
  int j;


  if (OFF3(offsets,i+indexm)!=0xffffff)
  {
   realnumm++;
   if (OFF3(offsets,i+indexm+1)!=0xffffff)
     tilem_len[i]=OFF3(offsets,i+indexm+1)-OFF3(offsets,i+indexm);
   else
     for (j=i+1;j<numm+1;j++)
       if (OFF3(offsets,j+indexm)!=0xffffff)
       {
	tilem_len[i]=OFF3(offsets,j+indexm)-OFF3(offsets,i+indexm);
	break;
       }
  }
  else
    tilem_len[i]=0;
 }

 //
 // DON'T REALLOCATE THIS IF WE'RE COMING BACK FROM A LAUNCH!
 //
 if (!launched)
   {
    long size=expsize*realnum+expmsize*realnumm,savings=0;


    if (1024L*XMSTotalFree()<size)
      {
       savings=CgaXMSsize+EgaXMSsize+VgaXMSsize;
       if (1024L*XMSTotalFree()<size-savings)
	 {
	  MouseShow();
	  ErrDialog("Not enough memory to load\n"
		    "requested graphics.\n"," OK ");

	  MMFreePtr((memptr *)&GraphHeader);
	  MMFreePtr((memptr *)&packed);
	  MMFreePtr((memptr *)&unpack);

	  if (rtn)
	    return -1;
	  else
	    Quit("Get more Extended memory!");
	 }

       if (CgaXMS)
	 {
	  XMSFreeMem(CgaXMS);
	  MMFreePtr((memptr *)&CgaXMSlookup);
	 }
       if (EgaXMS)
	 {
	  XMSFreeMem(EgaXMS);
	  MMFreePtr((memptr *)&EgaXMSlookup);
	 }
	   if (VgaXMS)
	 {
	  XMSFreeMem(VgaXMS);
	  MMFreePtr((memptr *)&VgaXMSlookup);
	 }
       CgaXMSsize=EgaXMSsize=VgaXMSsize=CgaXMS=EgaXMS=VgaXMS=0;
      }

    switch(tempvid)
    {
     case CGA:
       xmshandle=CgaXMS=XMSAllocate(size);
       CgaXMSsize=size;
       break;
     case EGA1:
     case EGA2:
       xmshandle=EgaXMS=XMSAllocate(size);
       EgaXMSsize=size;
       break;
     case VGA:
       xmshandle=VgaXMS=XMSAllocate(size);
       VgaXMSsize=size;
    }

    ErrDialog("GRAPHICS INSTALLATION\n"
	      "Decompressing and\n"
	      "moving tiles into\n"
	      "Extended memory:","");
   }
 else
   {
    long size;


    CgaXMS=TEDInfo->OldCgaXMS;
    EgaXMS=TEDInfo->OldEgaXMS;
    VgaXMS=TEDInfo->OldVgaXMS;

    CgaXMSsize=TEDInfo->OldCgaXMSsize;
    EgaXMSsize=TEDInfo->OldEgaXMSsize;
    VgaXMSsize=TEDInfo->OldVgaXMSsize;

    size=(num+numm)*4;
    if (CgaXMS)
      {
	   MMAllocate((memptr *)&CgaXMSlookup,size);
       XMSmove(TEDInfo->CgaXMSlook,0,0,(long)MK_FP(CgaXMSlookup,0),size);
       XMSFreeMem(TEDInfo->CgaXMSlook);
       TEDInfo->CgaXMSlook=0;
      }
    if (EgaXMS)
      {
       MMAllocate((memptr *)&EgaXMSlookup,size);
       XMSmove(TEDInfo->EgaXMSlook,0,0,(long)MK_FP(EgaXMSlookup,0),size);
       XMSFreeMem(TEDInfo->EgaXMSlook);
       TEDInfo->EgaXMSlook=0;
      }
    if (VgaXMS)
      {
       MMAllocate((memptr *)&VgaXMSlookup,size);
       XMSmove(TEDInfo->VgaXMSlook,0,0,(long)MK_FP(VgaXMSlookup,0),size);
       XMSFreeMem(TEDInfo->VgaXMSlook);
       TEDInfo->VgaXMSlook=0;
      }

    switch(tempvid)
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

    ErrDialog("RE-INITIALIZING...","");
   }

 ox=sx;
 oy=sy;

 //
 // INSTALL GRAPHICS IF NOT A LAUNCH...
 //
 if (!launched)
 {
  switch(tempvid)
  {
   case CGA:
     MMAllocate((memptr *)&CgaXMSlookup,(num+numm)*4);
     XMSlookup=CgaXMSlookup;
     break;
   case EGA1:
   case EGA2:
     MMAllocate((memptr *)&EgaXMSlookup,(num+numm)*4);
     XMSlookup=EgaXMSlookup;
     break;
   case VGA:
     MMAllocate((memptr *)&VgaXMSlookup,(num+numm)*4);
     XMSlookup=VgaXMSlookup;
  }


  //
  // SET UP MEMORY CACHE IF ENOUGH IS AVAILABLE...
  //
  cacheon_h=0;
  if (filelen(gname1)<16L*MMTotalFree())
  {
   LoadIn(gname1,(memptr *)&CacheBlock);
   cacheon=1;
  }
  else
  //
  // DAMN! TRY XMS AS A LAST RESORT...
  //
  if (filelen(gname1)<1024l*XMSTotalFree())
  {
   #define TMPBUFSIZE 32000l
   long amtleft,tmpoff=0,tmpsize;
   memptr tblock;


   amtleft=filelen(gname1);
   MMAllocate(&tblock,TMPBUFSIZE);
   cacheon_h=XMSAllocate(amtleft);
   tmpsize=TMPBUFSIZE;
   while(amtleft>0)
   {
	LoadFile(gname1,MK_FP(tblock,0),tmpoff,tmpsize);
    XMSmove(0,(long)MK_FP(tblock,0),cacheon_h,tmpoff,tmpsize);
    amtleft-=TMPBUFSIZE;
    tmpoff+=TMPBUFSIZE;
    tmpsize=(amtleft>TMPBUFSIZE)?TMPBUFSIZE:amtleft;
   }

   MMFreePtr(&tblock);
   cacheon=2;
  }

  clearkeys();

  //
  // MOVE NONMASKED TILES INTO XMS MEMORY!
  //      ---------
  unpacksize=unpackoff=0;
  unpackxms=xmsoff;
  for (i=0;i<num;i++)
  {
   long size,off;


   off=OFF3(offsets,index+i);
   if (off==0xffffff)   // SPARSE TILE?
     {
      *(XMSlookup+i)=-1;
      continue;
     }

   size=tile_len[i];

   //
   // GET COMPRESSED TILE DATA FROM CACHE OR DISK
   //
   if (cacheon==1)
   {
    //
    // HUFFMAN DECOMPRESS
    //
    HuffExpand((unsigned char huge *)CacheBlock+off,(unsigned char huge *)unpack+unpackoff,
	  expsize,(huffnode *)hufftable);
   }
   else
   if (cacheon==2)
   {
    XMSmove(cacheon_h,off,0,(long)packed,size);
    //
    // HUFFMAN DECOMPRESS
    //
    HuffExpand((unsigned char huge *)packed,(unsigned char huge *)unpack+unpackoff,
	  expsize,(huffnode *)hufftable);
   }
   else
   {
    LoadFile(gname1,(char huge *)packed,off,size);
    //
    // HUFFMAN DECOMPRESS
    //
    HuffExpand((unsigned char huge *)packed,(unsigned char huge *)unpack+unpackoff,
	  expsize,(huffnode *)hufftable);
   }

   unpacksize+=expsize;
   unpackoff+=expsize;

   *(XMSlookup+i)=xmsoff;
   xmsoff+=expsize;

   if (unpacksize>unpackmax-expsize)
   {
    XMSmove(0,(long)MK_FP(unpack,0),xmshandle,unpackxms,unpacksize);
    unpacksize=unpackoff=0;
    unpackxms=xmsoff;

    sx=ox;
    sy=oy;
    printint(num+numm-i);
    print(" ");
   }

   //
   // ESC will exit!
   //
   if (keydown[1])
     if (rtn)
     {
      switch(tempvid)
      {
       case CGA:
	 XMSFreeMem(CgaXMS);
	 MMFreePtr((memptr *)&CgaXMSlookup);
	 CgaXMS=0;
	 break;
       case EGA1:
       case EGA2:
	 XMSFreeMem(EgaXMS);
	 MMFreePtr((memptr *)&EgaXMSlookup);
	 EgaXMS=0;
	 break;
       case VGA:
	 XMSFreeMem(VgaXMS);
	 MMFreePtr((memptr *)&VgaXMSlookup);
	 VgaXMS=0;
      }

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

      MMFreePtr((memptr *)&GraphHeader);
      MMFreePtr((memptr *)&packed);
      MMFreePtr((memptr *)&unpack);

      if (cacheon)
	MMFreePtr((memptr *)&CacheBlock);

      return 0;
     }
     else
       Quit("XMS LOADING ABORTED!");
  }

  //
  // FLUSH THE FAST(?)-CACHE
  //
  if (unpacksize)
  {
   XMSmove(0,(long)MK_FP(unpack,0),xmshandle,unpackxms,unpacksize);
   unpacksize=unpackoff=0;
   unpackxms=xmsoff;
  }

  //
  // MOVE MASKED TILES INTO XMS MEMORY!
  //      ------
  for (i=0;i<numm;i++)
  {
   long size,off;

   off=OFF3(offsets,indexm+i);
   if (off==0xffffff)   // SPARSE TILE?
	 {
	  *(XMSlookup+i+num)=-1;
	  continue;
     }

   size=tilem_len[i];

   //
   // GET COMPRESSED TILE DATA FROM CACHE OR DISK
   //
   if (cacheon==1)
   {
    //
    // HUFFMAN DECOMPRESS
    //
    HuffExpand((unsigned char huge *)CacheBlock+off,(unsigned char huge *)unpack+unpackoff,
	  expmsize,(huffnode *)hufftable);
   }
   else
   if (cacheon==2)
   {
    XMSmove(cacheon_h,off,0,(long)packed,size);
    //
    // HUFFMAN DECOMPRESS
    //
    HuffExpand((unsigned char huge *)packed,(unsigned char huge *)unpack+unpackoff,
	  expmsize,(huffnode *)hufftable);
   }
   else
   {
    LoadFile(gname1,(char huge *)packed,off,size);
    //
    // HUFFMAN DECOMPRESS
    //
    HuffExpand((unsigned char huge *)packed,(unsigned char huge *)unpack+unpackoff,
	  expmsize,(huffnode *)hufftable);
   }

   unpacksize+=expmsize;
   unpackoff+=expmsize;

   *(XMSlookup+i+num)=xmsoff;
   xmsoff+=expmsize;

   if (unpacksize>unpackmax-expmsize)
   {
    XMSmove(0,(long)MK_FP(unpack,0),xmshandle,unpackxms,unpacksize);
	unpacksize=unpackoff=0;
    unpackxms=xmsoff;

    sx=ox;
    sy=oy;
    printint(numm-i);
    print(" ");
   }


   //
   // ESC will exit!
   //
   if (keydown[1])
     if (rtn)
     {
      switch(tempvid)
      {
       case CGA:
	 XMSFreeMem(CgaXMS);
	 MMFreePtr((memptr *)&CgaXMSlookup);
	 CgaXMS=0;
	 break;
       case EGA1:
       case EGA2:
	 XMSFreeMem(EgaXMS);
	 MMFreePtr((memptr *)&EgaXMSlookup);
	 EgaXMS=0;
	 break;
       case VGA:
	 XMSFreeMem(VgaXMS);
	 MMFreePtr((memptr *)&VgaXMSlookup);
	 VgaXMS=0;
      }

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

      MMFreePtr((memptr *)&GraphHeader);
      MMFreePtr((memptr *)&packed);
      MMFreePtr((memptr *)&unpack);

      if (cacheon)
	MMFreePtr((memptr *)&CacheBlock);

      return 0;
     }
     else
       Quit("XMS LOADING ABORTED!");
  }
  //
  // FLUSH THE FAST-CACHE
  //
  if (unpacksize)
  {
   XMSmove(0,(long)MK_FP(unpack,0),xmshandle,unpackxms,unpacksize);
   unpacksize=unpackoff=0;
   unpackxms=xmsoff;
  }

  if (cacheon==1)
    MMFreePtr((memptr *)&CacheBlock);
  else
  if (cacheon==2)
    XMSFreeMem(cacheon_h);
 }

 //
 // GET RID OF TILE-FILE CACHE MEMORY (OR WE'RE TOASTY)
 //
 MMFreePtr((memptr *)&GraphHeader);
 MMFreePtr((memptr *)&packed);
 MMFreePtr((memptr *)&unpack);

 MMFreePtr((memptr *)&tile_len);
 MMFreePtr((memptr *)&tilem_len);

 whicht=0;
 whichi=tilenum;
 whichtm=tilenum;

 switch(tsize)
 {
  case 1: lasticon=tilenum+36*maxiconrows;
	  break;
  case 2: lasticon=tilenum+18*maxiconrows;
	  break;
  case 3: lasticon=tilenum+7*maxiconrows;
 }
 firsticon=tilenum;

 RestoreBackground();
 return 1;
}


////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
// Load TEDINFO.ext file
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////
void LoadInfoFile(void)
{
 char pname[14]="TEDINFO.",gfxname[14]="GFXINFO";


 //
 // Load the TEDINFO.ext file!
 //
 strcat(pname,ext);
 strcpy(infoname,pname);

 if (access(pname,0))
   {
    MMAllocate((memptr *)&TEDInfo,sizeof(InfoStruct));
    _fmemset(TEDInfo,0,sizeof(InfoStruct));
				//  BFI  BFI
    TEDInfo->pflags=0x27;	// 0010 0111
   }
 else
   LoadIn(pname,(memptr *)&TEDInfo);

 tsize=TEDInfo->tsize;
 if (launchname[0])
   _fmemcpy((char far *)TEDInfo->launchname,(char far *)launchname,14);

 //
 // LOAD THE "GFXINFO?.EXT" FILE
 //
 strcat(gfxname,format);
 strcat(gfxname,".");
 strcat(gfxname,ext);
 LoadIn(gfxname,(memptr *)&GFXInfo);

 switch(tsize)
 {
  case 1:
    tilenum=GFXInfo->num8;
    tilemnum=GFXInfo->num8m;
    break;
  case 2:
    tilenum=GFXInfo->num16;
    tilemnum=GFXInfo->num16m;
    break;
  case 3:
    tilenum=GFXInfo->num32;
    tilemnum=GFXInfo->num32m;
 }

 _fstrcpy((char far *)launchname,(char far *)TEDInfo->launchname);

 if (launched)
   TEDInfo->lastvid=LaunchInfo.lastmode;

 //
 // SET PLANE FLAGS BACK TO NORMAL
 //
 planeton=(TEDInfo->pflags>>6)&1;
 planemon=(TEDInfo->pflags>>5)&1;
 planeion=(TEDInfo->pflags>>4)&1;
 viewton=(TEDInfo->pflags>>2)&1;
 viewmon=(TEDInfo->pflags>>1)&1;
 viewion=(TEDInfo->pflags)&1;

 //
 // SET BACKGROUND COLOR
 //
 BkgndColor=TEDInfo->BackgndColor;
 if (BkgndColor>16)
   TEDInfo->BackgndColor=BkgndColor=O_FGNDBACK;
}


////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
// Find ?GAGRAPH.ext file
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////
void FindGraphFile(void)
{
 struct ffblk ffblk;
 char pname[15]="?GAGRAPH.*",*tempstr,tiname[13]="TEDINFO.TMP";
 int i,which;


 //
 // RETURNING FROM LAUNCH...GET INFO BACK
 //
 if (launched)
   {
	LoadFile(tiname,(char huge *)&LaunchInfo,0,0);
	unlink(tiname);
	videomode=LaunchInfo.lastmode;
	switch(videomode)
	{
	 case CGA:
	   format[0]='C';
	   break;
	 case EGA1:
	 case EGA2:
	   format[0]='E';
	   break;
	 case VGA:
	   format[0]='V';
	}

	strcpy(ext,LaunchInfo.ext);
	projname[0]=format[0];
	strcat(projname,"GAGRAPH.");
	strcat(projname,ext);
	return;
   }

 //
 // Find ?GAGRAPH.ext
 //
 if (ext[0])
   {
	strcpy(pname,"?GAGRAPH.");
	strcat(pname,ext);
   }

 if (findfirst(pname,&ffblk,FA_ARCH))
   {
	ErrDialog("I can't find a graphics\nfile! (ex:?GAGRAPH.ext)"," Alright ");
	Quit("Can't work without graphics ya know!");
   }


 if (GfxToUse)
   format[0] = GfxToUse;
 else
 {
  // setup the dialog

  strcpy(bstrings[0],ffblk.ff_name);
  ProjButns[0].xoff=9;
  ProjButns[0].yoff=2;
  ProjButns[0].border=0;
  ProjButns[0].text=bstrings[0];
  for (i=1;i<10;i++)
	{
	 if (findnext(&ffblk))
	   break;
	 strcpy(bstrings[i],ffblk.ff_name);
	 ProjButns[i].xoff=9;
	 ProjButns[i].yoff=2+i;
	 ProjButns[i].border=0;
	 ProjButns[i].text=bstrings[i];
	}
  ProjSelect.numbuttons=i;
  ProjSelect.height=i+3;

  which=1;
  if (i>1)
	do
	{
	 which=DoDialog(&ProjSelect);
	} while(!which);
  which--;

  tempstr=strpbrk(bstrings[which],".")+1;
  strcpy(ext,tempstr);
  format[0]=bstrings[which][0];
 }

 strcpy(projname,bstrings[which]);
}

//
// draw border for project window
//
void DrawProjBord(int x,int y)
{
 DrawBorder(x+8,y+1,13,ProjSelect.height-2,1);
}


////////////////////////////////////////////////////
//
// Input amount of icons to reserve
// Returns maximum # of icon rows to reserve
//
////////////////////////////////////////////////////
btype ICb={"    ",8,3,1};
DialogDef ICd={"Enter maximum amount\nof icons to reserve:",20,5,1,&ICb,NULL};

int InputIconAmount(void)
{
 char tempstr[4];
 int value;


 if (!(MapFileHeader->maptype&IPLANE))
   return 4;

 MouseHide();
 DrawDialog(&ICd,1);
 while(1)
 {
  GetDialogXY(&ICd,&sx,&sy);
  GetButtonXY(&ICd,0,&sx,&sy);
  if (input(tempstr,3))
  {
   value=atoi(tempstr);
   if (value>0)
   {
    MouseShow();
    RestoreBackground();
    return (value+17)/18;
   }
  }
 }
}


////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
// Unhook everything
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////
void Unhook(void)
{
 ShutdownKBD();
 if (CgaXMS)
   XMSFreeMem(CgaXMS);
 if (EgaXMS)
   XMSFreeMem(EgaXMS);
 if (VgaXMS)
   XMSFreeMem(VgaXMS);
 if (XMSundoB)
   XMSFreeMem(XMSundoB);
 if (XMSundoF)
   XMSFreeMem(XMSundoF);
 if (XMSundoI)
   XMSFreeMem(XMSundoI);
 if (XMSmaps)
   XMSFreeMem(XMSmaps);
}

void PatchPointers(void)
{
}


////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
// Menu Definitions
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////
MenuDef AboutMenu[]=
  {{"About...",Item_About,0,0},
   {"Video Mode Switch",Item_ModeSwitch,0,0x43},
   {"Last Video Mode",Item_LastVideo,ALT,0x2c},
   {"Memory Available",Item_PrintMem,0,0x44},
   {"Launch Project",Item_Launch,ALT,0x26},
   {"--------------------",NULL,0,0},
//   {"Display Unused Tiles",Item_CountTiles,0,0},
   {"Project Re-Select",Item_ProjectReSelect,0,0},
   {"Visit DOS",Item_VisitDOS,0,0}
  };

MenuDef FileMenu[]=
  {{"Edit New Map",Item_EditMap,ALT,0x18},
   {"Save Map",Item_SaveMap,ALT,0x1f},
   {"Create Map",Item_CreateMap,ALT,0x2e},
   {"Delete Map",Item_DeleteMap,ALT,0x20},
   {"Switch Map",Item_SwitchMap,ALT,0x11},
   {"Amputate Maps",Item_Amputate},
   {"---------------",NULL,0,0},
   {"Import Maps",Item_ImportMaps,0,0},
   {"Change ICON Rows",Item_ChangeIconRows,0,0},
   {"Carmacize Maps",Item_Huffman,0,0},
   {"Quit TED5",Item_Quit,ALT,0x2d}};

MenuDef EditMenu[]=
  {{"Switch to Last Map",Item_LastMap,ALT,0x32},
   {"Edit TILEINFO/M Values",Item_EditTinfoValues,ALT,0x14},
   {"Change LAUNCH name",Item_LAUNCHname,0,0},
   {"Change PARM string",Item_PARMstring,0,0},
   {"Edit TILEINFO/M Names",Item_EditTinfoNames,0,0},
   {"Add/Del TILEINFO/M Planes",Item_AddDelTinfo,0,0},
   {"Edit MAP Names",Item_EditMapNames,0,0},
   {"Change MAP Edges",Item_EditMapEdges,0,0}};

MenuDef ModeMenu[]=
{
 {"Copy Mode",Item_Copy,0,0x2e},
 {"Paste Mode",Item_Paste,0,0x19},
 {"Block Fill",Item_BlockFill,0,0x30},
 {"Flood Fill",Item_FloodFill,0,0x21},
 {"Undo last action",Item_Undo,0,0x16},
 {"Tile Search",Item_TileSearch,0,0x14},
 {"GridMode toggle",Item_GridMode,0,0x22},
 {"Snap-Paste toggle",Item_SnapTog,0,0x1f},
 {"Paste Overlay toggle",Item_POtog,0,0x3d}
};


MenuDef MiscMenu[]=
{
 {"Select Tile",Item_SelectTile,0,0x39},
 {"Map Stats",Item_MapStats,0,0x17},
 {"Toggle INFOBAR",Item_ToggleInfo,0,0x42},
 {"New INFOPLANE value",Item_InputInfoplane,0,0x1c},
 {"View Map & Goto",Item_ViewMap,ALT,0x2f},
 {"ReView Map & Goto",Item_ReviewMap,0,0xe},
 {"Change LAUNCH icon",Item_ChangeLaunchIcon,0,0},
 {"Change bkgnd color",Item_ChangeBkgndColor,0,0},
 {"TILEINFOM Copy",Item_TINFOCopy,0,0},
 {"Graphic Map Dump",Item_GraphicDump,0,0}
};


MBarDef TED5MenuBar[]=
  {{9,AboutMenu," ? "},
   {11,FileMenu,"File"},
   {8,EditMenu,"Edit"},
   {9,ModeMenu,"Modes"},
   {10,MiscMenu,"Misc"},
   {0,0,""}};
