////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
// TED5-3
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////
#include "ted5.h"
#pragma hdrstop

int xms1;
extern int tics;

////////////////////////////////////////////////////
//
// MAP SCROLLING
//
////////////////////////////////////////////////////
void CheckMapScroll(void)
{
 //
 // LEFT
 //
 if (keydown[0x4b])	// left arrow
   {
    int i,j,imax,jmax,tilesmoved;

    tics=biostime(0,0);

    if (xbase)
    {
     EraseFloatPaste();

     if (keydown[0x1d])	// CTRL-KEY
     {
      tilesmoved=screenw;
      if (keydown[0x38]) // ALT-KEY
      {
       xbase=0;
       DrawMap();
       return;
      }
     }
     else
       switch(videomode)
       {
	case CGA:
	case EGA1:
	case VGA:
	  tilesmoved=1;
	  break;
	case EGA2:
	  tilesmoved=2;
       }

     if (xbase<tilesmoved)
       tilesmoved=xbase;
     xbase-=tilesmoved;

     MouseHide();
     if (tilesmoved<screenw)
       CopyScreen(0,8,infomaxw-(tilesmoved<<(tsize-1)),screenh<<(tsize+2),tilesmoved<<(tsize-1),8);

     jmax=screenh;
     if (jmax>mapheight)
       jmax=mapheight;

     for (i=0;i<tilesmoved;i++)
       for (j=0;j<jmax;j++)
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

     px+=tilesmoved;
     DrawFloatPaste();
     MouseShow();
     if (keydown[0x36] || keydown[0x1d])
       while(keydown[0x4b]);
    }
   }
 else
 //
 // RIGHT
 //
 if (keydown[0x4d])	// right arrow
   {
    int i,j,imax,jmax,tilesmoved;

    tics=biostime(0,0);

    if (xbase+screenw<mapwidth)
    {
     EraseFloatPaste();

     if (keydown[0x1d])	// CTRL-KEY
     {
      tilesmoved=screenw;
      if (keydown[0x38]) // ALT-KEY
      {
       xbase=mapwidth-screenw;
       DrawMap();
       return;
      }
     }
     else
       switch(videomode)
       {
	case CGA:
	case EGA1:
	case VGA:
	  tilesmoved=1;
	  break;
	case EGA2:
	  tilesmoved=2;
       }


     if (xbase+screenw+tilesmoved>=mapwidth)
       tilesmoved=mapwidth-screenw-xbase;
     xbase+=tilesmoved;

     MouseHide();
     if (tilesmoved<screenw)
       CopyScreen(tilesmoved<<(tsize-1),8,infomaxw-(tilesmoved<<(tsize-1)),screenh<<(tsize+2),0,8);

     jmax=screenh;
     if (jmax>mapheight)
       jmax=mapheight;

     for (i=0;i<tilesmoved;i++)
       for (j=0;j<jmax;j++)
       {
	unsigned tilet,tilem,tilei,loc;

	loc=(ybase+j)*mapwidth+xbase+screenw-tilesmoved+i;

	tilet=*(MapBkgnd+loc);
	tilem=*(MapFrgnd+loc)+tilenum;
	tilei=*(MapInfoPl+loc)+tilenum;

	CombineTiles(tilet*viewton-BkgndColor*(!viewton),tilem*viewmon,tilei*viewion,tsize);
	if (GridMode)
	  Overlay(tsize);
	DrawTile(infomaxw-((tilesmoved-i)<<(tsize-1)),j*(4<<tsize)+8,tsize);
	CheckInfoValues(screenw-tilesmoved+i,j,tilei);
	CheckSelectEdges(screenw-tilesmoved+i+xbase,j+ybase,i+screenw-tilesmoved,j);
       }

     px+=tilesmoved;
     DrawFloatPaste();
     MouseShow();
     if (keydown[0x36] || keydown[0x1d])
       while(keydown[0x4d]);
    }
   }

 //
 // UP
 //
 if (keydown[0x48])	// up arrow
   {
    int i,j,imax,jmax,tilesmoved;

    tics=biostime(0,0);

    if (ybase)
    {
     EraseFloatPaste();

     if (keydown[0x1d])	// CTRL-KEY
     {
      tilesmoved=screenh;
      if (keydown[0x38]) // ALT-KEY
      {
       ybase=0;
       DrawMap();
       return;
      }
     }
     else
       switch(videomode)
       {
	case CGA:
	case EGA1:
	case VGA:
	  tilesmoved=1;
	  break;
	case EGA2:
	  tilesmoved=2;
       }

     if (ybase<tilesmoved)
       tilesmoved=ybase;
     ybase-=tilesmoved;

     MouseHide();
     if (tilesmoved<screenh)
       CopyScreen(0,8,infomaxw,(screenh-tilesmoved)<<(tsize+2),0,8+(tilesmoved<<(tsize+2)));

     imax=screenw;
     if (imax>mapwidth)
       imax=mapwidth;

     for (j=0;j<tilesmoved;j++)
       for (i=0;i<imax;i++)
       {
	unsigned tilet,tilem,tilei,loc;

	loc=(ybase+j)*mapwidth+xbase+i;

	tilet=*(MapBkgnd+loc);
	tilem=*(MapFrgnd+loc)+tilenum;
	tilei=*(MapInfoPl+loc)+tilenum;

	CombineTiles(tilet*viewton-BkgndColor*(!viewton),tilem*viewmon,tilei*viewion,tsize);
	if (GridMode)
	  Overlay(tsize);
	DrawTile(i<<(tsize-1),8+(j<<(tsize+2)),tsize);
	CheckInfoValues(i,j,tilei);
	CheckSelectEdges(i+xbase,j+ybase,i,j);
       }

     py+=tilesmoved;
     DrawFloatPaste();
     MouseShow();
     if (keydown[0x36] || keydown[0x1d])
       while(keydown[0x48]);
    }
   }
 else
 //
 // DOWN
 //
 if (keydown[0x50])	// down arrow
   {
    int i,j,imax,jmax,tilesmoved;

    tics=biostime(0,0);

    if (ybase+screenh<mapheight)
    {
     EraseFloatPaste();

     if (keydown[0x1d])	// CTRL-KEY
     {
      tilesmoved=screenh;
      if (keydown[0x38]) // ALT-KEY
      {
       ybase=mapheight-screenh;
       DrawMap();
       return;
      }
     }
     else
       switch(videomode)
       {
	case CGA:
	case EGA1:
	case VGA:
	  tilesmoved=1;
	  break;
	case EGA2:
	  tilesmoved=2;
       }

     if (ybase+screenh+tilesmoved>=mapheight)
       tilesmoved=mapheight-screenh-ybase;
     ybase+=tilesmoved;

     MouseHide();
     if (tilesmoved<screenh)
       CopyScreen(0,8+(tilesmoved<<(tsize+2)),infomaxw,(screenh-tilesmoved)<<(tsize+2),0,8);

     imax=screenw;
     if (imax>mapwidth)
       imax=mapwidth;

     for (j=0;j<tilesmoved;j++)
       for (i=0;i<imax;i++)
       {
	unsigned tilet,tilem,tilei,loc;

	loc=(ybase+j+screenh-tilesmoved)*mapwidth+xbase+i;

	tilet=*(MapBkgnd+loc);
	tilem=*(MapFrgnd+loc)+tilenum;
	tilei=*(MapInfoPl+loc)+tilenum;

	CombineTiles(tilet*viewton-BkgndColor*(!viewton),tilem*viewmon,tilei*viewion,tsize);
	if (GridMode)
	  Overlay(tsize);
	DrawTile(i<<(tsize-1),8+((screenh-tilesmoved+j)<<(tsize+2)),tsize);
	CheckInfoValues(i,screenh-tilesmoved+j,tilei);
	CheckSelectEdges(i+xbase,screenh-tilesmoved+j+ybase,i,j+screenh-tilesmoved);
       }


     py+=tilesmoved;
     DrawFloatPaste();
     MouseShow();
     if (keydown[0x36] || keydown[0x1d])
       while(keydown[0x50]);
    }
   }
}


////////////////////////////////////////////////////
//
// Copy a screen section to another, mode independent
//
////////////////////////////////////////////////////
void CopyScreen(int srcx,int srcy,int width,int height,int destx,int desty)
{
 switch(videomode)
 {
  case CGA: CopyCGA(srcx,srcy,width,height,destx,desty); break;
  case EGA1:
  case EGA2:outport(SCindex,0x0f00 | SCmapmask);CopyEGA(srcx,srcy,width,height,destx,desty); break;
  case VGA: CopyVGA(srcx,srcy,width,height,destx,desty);
 }
}


////////////////////////////////////////////////////
//
// See if either of the Select edges are visible on the map
// Draw them if so
//
////////////////////////////////////////////////////
void CheckSelectEdges(int x,int y,int i,int j)
{
 int temp;

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

 if (y==SelY2 && x==SelX2)
   {
    switch(tsize)
    {
     case 1: drawchar(i,j+1,8);
	     break;
     case 2: drawchar(i*2+1,j*2+2,8);
	     drawchar(i*2+1,j*2+1,5);
	     drawchar(i*2,j*2+2,7);
	     break;
     case 3: drawchar(i*4+3,j*4+4,8);

	     drawchar(i*4+3,j*4+3,5);
	     drawchar(i*4+3,j*4+2,5);
	     drawchar(i*4+3,j*4+1,5);

	     drawchar(i*4+2,j*4+4,7);
	     drawchar(i*4+1,j*4+4,7);
	     drawchar(i*4,j*4+4,7);
	     break;
    }
   }

 if (y==SelY1 && x==SelX1)
   {
    switch(tsize)
    {
     case 1: drawchar(i,j+1,1);
	     break;
     case 2: drawchar(i*2,j*2+1,1);
	     drawchar(i*2+1,j*2+1,2);
	     drawchar(i*2,j*2+2,4);
	     break;
     case 3: drawchar(i*4,j*4+1,1);

	     drawchar(i*4+1,j*4+1,2);
	     drawchar(i*4+2,j*4+1,2);
	     drawchar(i*4+3,j*4+1,2);

	     drawchar(i*4,j*4+2,4);
	     drawchar(i*4,j*4+3,4);
	     drawchar(i*4,j*4+4,4);
	     break;
    }
   }

 if (y==SelY2 && x==SelX1)
   {
    switch(tsize)
    {
     case 1: drawchar(i,j+1,6);
	     break;
     case 2: drawchar(i*2  ,j*2+2,6);
	     drawchar(i*2+1,j*2+2,7);
	     drawchar(i*2  ,j*2+1,4);
	     break;
     case 3: drawchar(i*4,j*4+4,6);

	     drawchar(i*4+1,j*4+4,7);
	     drawchar(i*4+2,j*4+4,7);
	     drawchar(i*4+3,j*4+4,7);

	     drawchar(i*4,j*4+1,4);
	     drawchar(i*4,j*4+2,4);
	     drawchar(i*4,j*4+3,4);
	     break;
    }
   }

 if (y==SelY1 && x==SelX2)
   {
    switch(tsize)
    {
     case 1: drawchar(i,j+1,3);
	     break;
     case 2: drawchar(i*2+1,j*2+1,3);
	     drawchar(i*2+1,j*2+2,5);
	     drawchar(i*2  ,j*2+1,2);
	     break;
     case 3: drawchar(i*4+3,j*4+1,3);

	     drawchar(i*4+3,j*4+4,5);
	     drawchar(i*4+3,j*4+3,5);
	     drawchar(i*4+3,j*4+2,5);

	     drawchar(i*4+2,j*4+4,7);
	     drawchar(i*4+1,j*4+4,7);
	     drawchar(i*4,j*4+4,7);
	     break;
    }
   }

 if (y==SelY1 && x>SelX1 && x<SelX2)
   {
    switch(tsize)
    {
     case 3: drawchar(i*4+2,j*4+1,2);
	     drawchar(i*4+3,j*4+1,2);
	     drawchar(i*4+1,j*4+1,2);
	     drawchar(i*4  ,j*4+1,2);
	     break;
     case 2: drawchar(i*2+1,j*2+1,2);
	     drawchar(i*2  ,j*2+1,2);
	     break;
     case 1: drawchar(i,j+1,2);
	     break;
    }
   }

 if (y==SelY2 && x>SelX1 && x<SelX2)
   {
    switch(tsize)
    {
     case 1: drawchar(i,j+1,7);
	     break;
     case 2: drawchar(i*2,j*2+2,7);
	     drawchar(i*2+1,j*2+2,7);
	     break;
     case 3: drawchar(i*4  ,j*4+4,7);
	     drawchar(i*4+1,j*4+4,7);
	     drawchar(i*4+2,j*4+4,7);
	     drawchar(i*4+3,j*4+4,7);
	     break;
    }
   }

 if (x==SelX1 && y>SelY1 && y<SelY2)
   {
    switch(tsize)
    {
     case 3: drawchar(i*4,j*4+3,4);
	     drawchar(i*4,j*4+4,4);
	     drawchar(i*4,j*4+2,4);
	     drawchar(i*4,j*4+1,4);
	     break;
     case 2: drawchar(i*2,j*2+2,4);
	     drawchar(i*2,j*2+1,4);
	     break;
     case 1: drawchar(i,j+1,4);
	     break;
    }
   }

 if (x==SelX2 && y>SelY1 && y<SelY2)
   {
    switch(tsize)
    {
     case 1: drawchar(i,j+1,5);
	     break;
     case 2: drawchar(i*2+1,j*2+1,5);
	     drawchar(i*2+1,j*2+2,5);
	     break;
     case 3: drawchar(i*4+3,j*4+1,5);
	     drawchar(i*4+2,j*4+1,5);
	     drawchar(i*4+1,j*4+1,5);
	     drawchar(i*4  ,j*4+1,5);
    }
   }


}

////////////////////////////////////////////////////
//
// FLOATING PASTE REGION!
//
////////////////////////////////////////////////////
void CheckFloatPaste(int mx,int my)
{
 int i,j,maxh,maxw;


 if (SnapMode && PasteMode && (pixely>=8 || pixely<infoy*8))
   {
    mx=(mx/snapxsize)*snapxsize+snapx;
    my=(my/snapysize)*snapysize+snapy;

    if (mx<xbase)
      mx+=snapxsize;
    if (my<ybase)
      my+=snapysize;
    if (mx>=xbase+screenw)
      mx-=snapxsize;
    if (my>=ybase+screenh)
      my-=snapysize;
   }

 if (!PasteMode || (px==mx && py==my) || (pixely<8 || pixely>infoy*8))
   return;

 if (mx>mapwidth || my>mapheight)
   return;

 MouseHide();
 EraseFloatPaste();

 //
 // FLOAT IT...
 //
 px=mx;
 py=my;

 DrawFloatPaste();
 MouseShow();
}


////////////////////////////////////////////////////
//
// DRAW FLOATING PASTE REGION
//
void DrawFloatPaste(void)
{
 int i,j,maxw,maxh;


 if (px==-1 || py==-1 || !PasteMode)
   return;
 //
 // NOW, DRAW IT IN A NEW LOCATION!
 //
 MouseHide();


 maxh=TileCopy.h;
 if (py+maxh>mapheight)
   maxh=mapheight-py;
 if (py+maxh-ybase>screenh)
   maxh=screenh-(py-ybase);

 maxw=TileCopy.w;
 if (px+maxw>mapwidth)
   maxw=mapwidth-px;
 if (px+maxw-xbase>screenw)
   maxw=screenw-(px-xbase);

 switch(TileCopy.MapOrTileSelect)
 {
  case 0: // MAP PASTE DRAW
    for (j=0;j<maxh;j++)
      for (i=0;i<maxw;i++)
	{
	 unsigned tilet,tilem,tilei,loc,loc1,oldt,oldm,oldi;
	 int theT,theM,theI;

	 loc=(TileCopy.y+j)*mapwidth+TileCopy.x+i;
	 loc1=(py+j)*mapwidth+px+i;

	 tilet=*(CutBkgnd+loc);
	 oldt=MapBkgnd[loc1];
	 tilem=*(CutFrgnd+loc)+tilenum;
	 if (tilem==tilenum)
	   tilem=0;
	 oldm=MapFrgnd[loc1]+tilenum;
	 tilei=*(CutInfoPl+loc)+tilenum;
	 if (tilei==tilenum)
	   tilei=0;
	 oldi=MapInfoPl[loc1]+tilenum;

	 theT=(TileCopy.PlanesCopied&BPLANE)?tilet:-BkgndColor;
	 if (theT==-BkgndColor && viewton)
	   theT=oldt;
	 theM=(TileCopy.PlanesCopied&FPLANE)?tilem:0;
	 if (!theM && viewmon && !(TileCopy.PlanesCopied&FPLANE && F3_flag))
	   theM=oldm;
	 theI=(TileCopy.PlanesCopied&IPLANE)?tilei:0;
	 if (!theI && viewion && !(TileCopy.PlanesCopied&IPLANE && F3_flag))
	   theI=oldi;

	 CombineTiles(theT,theM,theI,tsize);
	 if (GridMode)
	   Overlay(tsize);
	 DrawTile((px+i-xbase)<<(tsize-1),(py+j-ybase)*(4<<tsize)+8,tsize);
	}
    break;

  case 1: // TILE PASTE DRAW
    for (j=0;j<maxh;j++)
      for (i=0;i<maxw;i++)
	{
	 int val=(j+TileCopy.y)*selectcols+TileCopy.x+i;
	 unsigned tilem,tilei,loc;

	 loc=(py+j)*mapwidth+px+i;

	 tilem=MapFrgnd[loc]+tilenum;
	 tilei=MapInfoPl[loc]+tilenum;

	 if (XMSlookup[val]!=-1)
	   {
	    CombineTiles(viewton?val:-BkgndColor,
			 viewmon?tilem:0,
			 viewion?tilei:0,
			 tsize);
	    if (GridMode)
	      Overlay(tsize);
	    DrawTile((px+i-xbase)<<(tsize-1),(py+j-ybase)*(4<<tsize)+8,tsize);
	   }
	}
    break;

  case 2: // MASKED PASTE DRAW
    for (j=0;j<maxh;j++)
      for (i=0;i<maxw;i++)
	{
	 int val=(j+TileCopy.y)*selectcols+TileCopy.x+i+tilenum+maxiconrows*selectcols;
	 unsigned tilet,tilei,loc;

	 loc=(py+j)*mapwidth+px+i;

	 tilet=MapBkgnd[loc];
	 tilei=MapInfoPl[loc]+tilenum;

	 if (XMSlookup[val]!=-1)
	   {
	    CombineTiles(viewton?tilet:-BkgndColor,
			 viewmon?val:0,
			 viewion?tilei:0,
			 tsize);
	    if (GridMode)
	      Overlay(tsize);
	    DrawTile((px+i-xbase)<<(tsize-1),(py+j-ybase)*(4<<tsize)+8,tsize);
	   }
	}
 }

 MouseShow();
}

////////////////////////////////////////////////////
//
// ERASE FLOATING PASTE REGION
//
void EraseFloatPaste(void)
{
 int maxh,maxw,i,j;


 if (px==-1 || py==-1 || !PasteMode)
   return;

 //
 // ERASE THE FLOATING REGION
 // (PRECLIPPING REGION)
 //
 MouseHide();

 maxh=TileCopy.h;
 if (py+maxh>mapheight)
   maxh=mapheight-py;
 if (py+maxh-ybase>screenh)
   maxh=screenh-(py-ybase);

 maxw=TileCopy.w;
 if (px+maxw>mapwidth)
   maxw=mapwidth-px;
 if (px+maxw-xbase>screenw)
   maxw=screenw-(px-xbase);

 for (j=0;j<maxh;j++)
   for (i=0;i<maxw;i++)
     {
      unsigned tile_t,tile_m,tile_i,loc;

      loc=(py+j)*mapwidth+px+i;

      tile_t=*(MapBkgnd+loc);
      tile_m=*(MapFrgnd+loc)+tilenum;
      tile_i=*(MapInfoPl+loc)+tilenum;

      CombineTiles(viewton?tile_t:-BkgndColor,viewmon?tile_m:0,viewion?tile_i:0,tsize);
      if (GridMode)
	Overlay(tsize);
      DrawTile((px+i-xbase)<<(tsize-1),(py+j-ybase)*(4<<tsize)+8,tsize);
     }

 MouseShow();
}


////////////////////////////////////////////////////
//
// SAVE CUT BUFFERS TO XMS BEFORE MAP LOAD/CREATE
//
////////////////////////////////////////////////////
void SaveCutBuffers(void)
{
 long size,off;
 int j;

 //
 // IS SOMETHING IN THE COPY BUFFER?
 // IF SO, PRESERVE AS MUCH AS POSSIBLE
 //
 if (!TileCopy.MapOrTileSelect)
   {
    size=(2L*TileCopy.w*TileCopy.h)*(((TileCopy.PlanesCopied&BPLANE)>0)+
      ((TileCopy.PlanesCopied&FPLANE)>0)+((TileCopy.PlanesCopied&IPLANE)>0));

    xms1=XMSAllocate(size);

    off=0;
    if (TileCopy.PlanesCopied&BPLANE)
      for (j=0;j<TileCopy.h;j++)
	{
	 XMSmove(0,(long)MK_FP(CutBkgnd,2L*((TileCopy.y+j)*mapwidth)+2L*TileCopy.x),
	   xms1,off,2L*TileCopy.w);
	 off+=2L*TileCopy.w;
	}

    if (TileCopy.PlanesCopied&FPLANE)
      for (j=0;j<TileCopy.h;j++)
	{
	 XMSmove(0,(long)MK_FP(CutFrgnd,2L*((TileCopy.y+j)*mapwidth)+2L*TileCopy.x),
	   xms1,off,2L*TileCopy.w);
	 off+=2L*TileCopy.w;
	}

    if (TileCopy.PlanesCopied&IPLANE)
      for (j=0;j<TileCopy.h;j++)
	{
	 XMSmove(0,(long)MK_FP(CutInfoPl,2L*((TileCopy.y+j)*mapwidth)+2L*TileCopy.x),
	   xms1,off,2L*TileCopy.w);
	 off+=2L*TileCopy.w;
	}
   }
}


////////////////////////////////////////////////////
//
// RESTORE CUT BUFFERS FROM XMS AFTER MAP LOAD/CREATE
//
////////////////////////////////////////////////////
void RestoreCutBuffers(void)
{
 //
 // COPY BACK STUFF IN XMS
 //
 if (!TileCopy.MapOrTileSelect)
   {
    long ToOff,off;
    int newwidth,newheight,j;

    newwidth=(TileCopy.w>mapwidth)?mapwidth:TileCopy.w;
    newheight=(TileCopy.h>mapheight)?mapheight:TileCopy.h;

    ToOff=off=0;

    if (TileCopy.PlanesCopied&BPLANE)
      for (j=0;j<newheight;j++)
	{
	 XMSmove(xms1,off,0,(long)MK_FP(CutBkgnd,2L*(j*mapwidth)),2L*newwidth);
	 ToOff+=2L*newwidth;
	 off+=2L*TileCopy.w;
	}

    if (TileCopy.PlanesCopied&FPLANE)
      for (j=0;j<newheight;j++)
	{
	 XMSmove(xms1,off,0,(long)MK_FP(CutFrgnd,2L*(j*mapwidth)),2L*newwidth);
	 ToOff+=2L*newwidth;
	 off+=2L*TileCopy.w;
	}

    if (TileCopy.PlanesCopied&IPLANE)
      for (j=0;j<newheight;j++)
	{
	 XMSmove(xms1,off,0,(long)MK_FP(CutInfoPl,2L*(j*mapwidth)),2L*newwidth);
	 ToOff+=2L*newwidth;
	 off+=2L*TileCopy.w;
	}

    TileCopy.x=TileCopy.y=0;
    TileCopy.w=newwidth;
    TileCopy.h=newheight;

    XMSFreeMem(xms1);	// RELEASE THE MEMORY
   }
}

////////////////////////////////////////////////////
//
// SEE IF THERE'S AN INFO VALUES AT THE CURRENT POSITION
// THAT NEEDS TO BE PRINTED
//
////////////////////////////////////////////////////
void CheckInfoValues(int i,int j,int tilei)
{
 sx=i<<(tsize-1);
 sy=(j<<(tsize-1))+1;

 if (TsearchMode)
   {
    unsigned loc,temp,p=0;

    loc=(j+ybase)*mapwidth+i+xbase;

    if (planeton)
      {
       if (MapBkgnd[loc]==whicht)
	 {
	  temp=whicht;
	  p=1;
	 }
      }
    else
    if (planemon)
      {
       if (MapFrgnd[loc]==whichtm-tilenum)
	 {
	  temp=whichtm-tilenum;
	  p=1;
	 }
      }
    else
    if (planeion)
      if ((unsigned)MapInfoPl[loc]==whichi-tilenum)
	{
	 temp=whichi-tilenum;
	 p=1;
	}

    if (p)
      switch(tsize)
      {
       case 1:
	 print("#");
	 break;
       case 2:
	 printhexb(temp>>8);
	 sx-=2;
	 sy++;
	 printhexb(temp&0xff);
	 break;
       case 3: printhex(temp);
      }
   }
 else
 if (viewion && tilei>lasticon)
   {
    switch(tsize)
    {
     case 1:
       print("#");
       break;
     case 2:
       printhexb(tilei-tilenum>>8);
       sx-=2;
       sy++;
       printhexb(tilei-tilenum&0xff);
       break;
     case 3: printhex(tilei-tilenum);
    }
   }
}

////////////////////////////////////////////////////
//
// Item - Edit Map Edges
//
////////////////////////////////////////////////////
btype MapEdgeB[]={{"\xb",7,3,1},
		  {"\xc",7,7,1},
		  {"\xe",4,5,1},
		  {"\x1f",10,5,1},
		  {" Exit ",5,16,2}};
DialogDef MapEdgeD={"  PICK MAP EDGE\n"
		    "    TO CHANGE",
		    16,18,5,&MapEdgeB[0],NULL};

void Item_EditMapEdges(void)
{
 int which,val,newwidth,newheight,b,f,i,
	_seg *tempB,_seg *tempF,_seg *tempI;
 unsigned dx,dy,obx,oby,k,j,modified=0;
 long size;

 b=MapFileHeader->maptype&BPLANE;
 f=MapFileHeader->maptype&FPLANE;
 i=MapFileHeader->maptype&IPLANE;

 DrawDialog(&MapEdgeD,1);
 GetDialogXY(&MapEdgeD,&dx,&dy);
 do
 {
  MouseHide();
  sx=dx+2;
  sy=dy+14;
  print("W:");
  printint(MapHeader.width);
  print(", H:");
  printint(MapHeader.height);
  print("   ");
  MouseShow();


  which=CheckButtons(&MapEdgeD);
  GetButtonXY(&MapEdgeD,which-1,&obx,&oby);
  MouseHide();
  if (which>=1 && which<=4)
    {
     // DRAW INPUT BAR
     sx=dx;
     sy=dy+9;
     print("+ or - value");
     DrawBorder(dx,dy+10,15,2,1);

     // INPUT VALUE
     sx=dx+1;
     sy=dy+11;
     val=inputint(9);

     // ERASE THE ARROW AND INPUT BAR
     sx=obx;
     sy=oby;
     print(MapEdgeB[which-1].text);
     bar(dx,dy+9,dx+15,dy+12,' ');

     // CHECK FOR ESC
     if (val==(int)ESCOUT)
       which=6;
    }

  MouseShow();
  switch(which)
  {
   //
   // ADD OR DELETE FROM TOP
   //
   case 1:
     newheight=mapheight+val;
     size=2L*newheight*mapwidth;
     if (size<=0 || size>0x10000L)
       {
	RestoreBackground();
	ErrDialog("Invalid Map height!"," OK ");
	return;
       }

     // FREE UP SOME MEMORY!
     SaveCutBuffers();
     RemoveUndoBuffers();

     if (b)
       {
	MMFreePtr((memptr *)&CutBkgnd);
	MMAllocate((memptr *)&tempB,size);
       }
     if (f)
       {
	MMFreePtr((memptr *)&CutFrgnd);
	MMAllocate((memptr *)&tempF,size);
       }
     if (i)
       {
	MMFreePtr((memptr *)&CutInfoPl);
	MMAllocate((memptr *)&tempI,size);
       }

     if (val<0)
       {
	// COPY MAP PLANES INTO TEMP MEMORY (CLIPPED, HERE)
	for (j=abs(val);j<mapheight;j++)
	  for (k=0;k<mapwidth;k++)
	    {
	     if (b)
	       tempB[(j+val)*mapwidth+k]=MapBkgnd[j*mapwidth+k];
	     if (f)
	       tempF[(j+val)*mapwidth+k]=MapFrgnd[j*mapwidth+k];
	     if (i)
	       tempI[(j+val)*mapwidth+k]=MapInfoPl[j*mapwidth+k];
	    }
       }
     else
       {
	for (j=0;j<mapheight+val;j++)
	  for (k=0;k<mapwidth;k++)
	    {
	     if (j<abs(val))
	       {
		if (b)
		  tempB[j*mapwidth+k]=whicht;
		if (f)
		  tempF[j*mapwidth+k]=whichtm-tilenum;
		if (i)
		  tempI[j*mapwidth+k]=whichi-tilenum;
	       }
	     else
	       {
		if (b)
		  tempB[j*mapwidth+k]=MapBkgnd[(j-val)*mapwidth+k];
		if (f)
		  tempF[j*mapwidth+k]=MapFrgnd[(j-val)*mapwidth+k];
		if (i)
		  tempI[j*mapwidth+k]=MapInfoPl[(j-val)*mapwidth+k];
	       }
	    }
       }

     // DEALLOCATE & REALLOCATE THE MAP BUFFERS, RESIZED
     if (b)
       {
	MMFreePtr((memptr *)&MapBkgnd);
	MMAllocate((memptr *)&MapBkgnd,size);
       }
     if (f)
       {
	MMFreePtr((memptr *)&MapFrgnd);
	MMAllocate((memptr *)&MapFrgnd,size);
       }
     if (i)
       {
	MMFreePtr((memptr *)&MapInfoPl);
	MMAllocate((memptr *)&MapInfoPl,size);
       }

     // COPY THE REGION BACK IN...
     for (j=0;j<newheight;j++)
       for (k=0;k<mapwidth;k++)
	 {
	  if (b)
	    MapBkgnd[j*mapwidth+k]=tempB[j*mapwidth+k];
	  if (f)
	    MapFrgnd[j*mapwidth+k]=tempF[j*mapwidth+k];
	  if (i)
	    MapInfoPl[j*mapwidth+k]=tempI[j*mapwidth+k];
	 }

     // TOAST THE 'TEMP' BUFFERS & REALLOCATE THE 'CUT' BUFFERS
     if (b)
       {
	MMFreePtr((memptr *)&tempB);
	MMAllocate((memptr *)&CutBkgnd,size);
       }
     if (f)
       {
	MMFreePtr((memptr *)&tempF);
	MMAllocate((memptr *)&CutFrgnd,size);
       }
     if (i)
       {
	MMFreePtr((memptr *)&tempI);
	MMAllocate((memptr *)&CutInfoPl,size);
       }

     mapheight=newheight;
     MapHeader.height=newheight;
     modified=DirtyFlag=1;
     AllocateUndoBuffers();
     UndoRegion.x=-1;
     SaveUndo(0,0,mapwidth,mapheight);
     RestoreCutBuffers();

     break;

   //
   // ADD OR DELETE FROM BOTTOM
   //
   case 2:
     newheight=mapheight+val;
     size=2L*newheight*mapwidth;
     if (size<=0 || size>0x10000L)
       {
	RestoreBackground();
	ErrDialog("Invalid Map height!"," OK ");
	return;
       }

     // FREE UP SOME MEMORY!
     SaveCutBuffers();
     RemoveUndoBuffers();
     if (b)
       {
	MMFreePtr((memptr *)&CutBkgnd);
	MMAllocate((memptr *)&tempB,size);
       }
     if (f)
       {
	MMFreePtr((memptr *)&CutFrgnd);
	MMAllocate((memptr *)&tempF,size);
       }
     if (i)
       {
	MMFreePtr((memptr *)&CutInfoPl);
	MMAllocate((memptr *)&tempI,size);
       }

     if (val<0)
       {
	// COPY MAP PLANES INTO TEMP MEMORY (CLIPPED, HERE)
	for (j=0;j<mapheight+val;j++)
	  for (k=0;k<mapwidth;k++)
	    {
	     if (b)
	       tempB[j*mapwidth+k]=MapBkgnd[j*mapwidth+k];
	     if (f)
	       tempF[j*mapwidth+k]=MapFrgnd[j*mapwidth+k];
	     if (i)
	       tempI[j*mapwidth+k]=MapInfoPl[j*mapwidth+k];
	    }
       }
     else
       {
	for (j=0;j<mapheight+val;j++)
	  for (k=0;k<mapwidth;k++)
	    {
	     if (j>=mapheight)
	       {
		if (b)
		  tempB[j*mapwidth+k]=whicht;
		if (f)
		  tempF[j*mapwidth+k]=whichtm-tilenum;
		if (i)
		  tempI[j*mapwidth+k]=whichi-tilenum;
	       }
	     else
	       {
		if (b)
		  tempB[j*mapwidth+k]=MapBkgnd[j*mapwidth+k];
		if (f)
		  tempF[j*mapwidth+k]=MapFrgnd[j*mapwidth+k];
		if (i)
		  tempI[j*mapwidth+k]=MapInfoPl[j*mapwidth+k];
	       }
	    }
       }

     // DEALLOCATE & REALLOCATE THE MAP BUFFERS, RESIZED
     if (b)
       {
	MMFreePtr((memptr *)&MapBkgnd);
	MMAllocate((memptr *)&MapBkgnd,size);
       }
     if (f)
       {
	MMFreePtr((memptr *)&MapFrgnd);
	MMAllocate((memptr *)&MapFrgnd,size);
       }
     if (i)
       {
	MMFreePtr((memptr *)&MapInfoPl);
	MMAllocate((memptr *)&MapInfoPl,size);
       }

     // COPY THE REGION BACK IN...
     for (j=0;j<newheight;j++)
       for (k=0;k<mapwidth;k++)
	 {
	  if (b)
	    MapBkgnd[j*mapwidth+k]=tempB[j*mapwidth+k];
	  if (f)
	    MapFrgnd[j*mapwidth+k]=tempF[j*mapwidth+k];
	  if (i)
	    MapInfoPl[j*mapwidth+k]=tempI[j*mapwidth+k];
	 }

     // TOAST THE 'TEMP' BUFFERS & REALLOCATE THE 'CUT' BUFFERS
     if (b)
       {
	MMFreePtr((memptr *)&tempB);
	MMAllocate((memptr *)&CutBkgnd,size);
       }
     if (f)
       {
	MMFreePtr((memptr *)&tempF);
	MMAllocate((memptr *)&CutFrgnd,size);
       }
     if (i)
       {
	MMFreePtr((memptr *)&tempI);
	MMAllocate((memptr *)&CutInfoPl,size);
       }

     mapheight=newheight;
     MapHeader.height=newheight;
     modified=DirtyFlag=1;
     AllocateUndoBuffers();
     UndoRegion.x=-1;
     SaveUndo(0,0,mapwidth,mapheight);
     RestoreCutBuffers();

     break;

   //
   // ADD OR DELETE FROM LEFTEDGE
   //
   case 3:
     newwidth=mapwidth+val;
     size=2L*newwidth*mapheight;
     if (size<=0 || size>0x10000L)
       {
	RestoreBackground();
	ErrDialog("Invalid Map width!"," OK ");
	return;
       }

     // FREE UP SOME MEMORY!
     SaveCutBuffers();
     RemoveUndoBuffers();
     if (b)
       {
	MMFreePtr((memptr *)&CutBkgnd);
	MMAllocate((memptr *)&tempB,size);
       }
     if (f)
       {
	MMFreePtr((memptr *)&CutFrgnd);
	MMAllocate((memptr *)&tempF,size);
       }
     if (i)
       {
	MMFreePtr((memptr *)&CutInfoPl);
	MMAllocate((memptr *)&tempI,size);
       }

     if (val<0)
       {
	// COPY MAP PLANES INTO TEMP MEMORY (CLIPPED, HERE)
	for (j=0;j<mapheight;j++)
	  for (k=abs(val);k<mapwidth;k++)
	    {
	     if (b)
	       tempB[j*newwidth+k+val]=MapBkgnd[j*mapwidth+k];
	     if (f)
	       tempF[j*newwidth+k+val]=MapFrgnd[j*mapwidth+k];
	     if (i)
	       tempI[j*newwidth+k+val]=MapInfoPl[j*mapwidth+k];
	    }
       }
     else
       {
	for (j=0;j<mapheight;j++)
	  for (k=0;k<mapwidth+val;k++)
	    {
	     if (k<abs(val))
	       {
		if (b)
		  tempB[j*newwidth+k]=whicht;
		if (f)
		  tempF[j*newwidth+k]=whichtm-tilenum;
		if (i)
		  tempI[j*newwidth+k]=whichi-tilenum;
	       }
	     else
	       {
		if (b)
		  tempB[j*newwidth+k]=MapBkgnd[j*mapwidth+k-val];
		if (f)
		  tempF[j*newwidth+k]=MapFrgnd[j*mapwidth+k-val];
		if (i)
		  tempI[j*newwidth+k]=MapInfoPl[j*mapwidth+k-val];
	       }
	    }
       }

     // DEALLOCATE & REALLOCATE THE MAP BUFFERS, RESIZED
     if (b)
       {
	MMFreePtr((memptr *)&MapBkgnd);
	MMAllocate((memptr *)&MapBkgnd,size);
       }
     if (f)
       {
	MMFreePtr((memptr *)&MapFrgnd);
	MMAllocate((memptr *)&MapFrgnd,size);
       }
     if (i)
       {
	MMFreePtr((memptr *)&MapInfoPl);
	MMAllocate((memptr *)&MapInfoPl,size);
       }

     // COPY THE REGION BACK IN...
     for (j=0;j<mapheight;j++)
       for (k=0;k<newwidth;k++)
	 {
	  if (b)
	    MapBkgnd[j*newwidth+k]=tempB[j*newwidth+k];
	  if (f)
	    MapFrgnd[j*newwidth+k]=tempF[j*newwidth+k];
	  if (i)
	    MapInfoPl[j*newwidth+k]=tempI[j*newwidth+k];
	 }

     // TOAST THE 'TEMP' BUFFERS & REALLOCATE THE 'CUT' BUFFERS
     if (b)
       {
	MMFreePtr((memptr *)&tempB);
	MMAllocate((memptr *)&CutBkgnd,size);
       }
     if (f)
       {
	MMFreePtr((memptr *)&tempF);
	MMAllocate((memptr *)&CutFrgnd,size);
       }
     if (i)
       {
	MMFreePtr((memptr *)&tempI);
	MMAllocate((memptr *)&CutInfoPl,size);
       }

     mapwidth=newwidth;
     MapHeader.width=newwidth;
     modified=DirtyFlag=1;
     AllocateUndoBuffers();
     UndoRegion.x=-1;
     SaveUndo(0,0,mapwidth,mapheight);
     RestoreCutBuffers();

     break;

   //
   // ADD OR DELETE FROM RIGHTEDGE
   //
   case 4:
     newwidth=mapwidth+val;
     size=2L*newwidth*mapheight;
     if (size<=0 || size>0x10000L)
       {
	RestoreBackground();
	ErrDialog("Invalid Map width!"," OK ");
	return;
       }

     // FREE UP SOME MEMORY!
     SaveCutBuffers();
     RemoveUndoBuffers();
     if (b)
       {
	MMFreePtr((memptr *)&CutBkgnd);
	MMAllocate((memptr *)&tempB,size);
       }
     if (f)
       {
	MMFreePtr((memptr *)&CutFrgnd);
	MMAllocate((memptr *)&tempF,size);
       }
     if (i)
       {
	MMFreePtr((memptr *)&CutInfoPl);
	MMAllocate((memptr *)&tempI,size);
       }

     if (val<0)
       {
	// COPY MAP PLANES INTO TEMP MEMORY (CLIPPED, HERE)
	for (j=0;j<mapheight;j++)
	  for (k=0;k<mapwidth+val;k++)
	    {
	     if (b)
	       tempB[j*newwidth+k]=MapBkgnd[j*mapwidth+k];
	     if (f)
	       tempF[j*newwidth+k]=MapFrgnd[j*mapwidth+k];
	     if (i)
	       tempI[j*newwidth+k]=MapInfoPl[j*mapwidth+k];
	    }
       }
     else
       {
	for (j=0;j<mapheight;j++)
	  for (k=0;k<mapwidth+val;k++)
	    {
	     if (k>=mapwidth)
	       {
		if (b)
		  tempB[j*newwidth+k]=whicht;
		if (f)
		  tempF[j*newwidth+k]=whichtm-tilenum;
		if (i)
		  tempI[j*newwidth+k]=whichi-tilenum;
	       }
	     else
	       {
		if (b)
		  tempB[j*newwidth+k]=MapBkgnd[j*mapwidth+k];
		if (f)
		  tempF[j*newwidth+k]=MapFrgnd[j*mapwidth+k];
		if (i)
		  tempI[j*newwidth+k]=MapInfoPl[j*mapwidth+k];
	       }
	    }
       }

     // DEALLOCATE & REALLOCATE THE MAP BUFFERS, RESIZED
     if (b)
       {
	MMFreePtr((memptr *)&MapBkgnd);
	MMAllocate((memptr *)&MapBkgnd,size);
       }
     if (f)
       {
	MMFreePtr((memptr *)&MapFrgnd);
	MMAllocate((memptr *)&MapFrgnd,size);
       }
     if (i)
       {
	MMFreePtr((memptr *)&MapInfoPl);
	MMAllocate((memptr *)&MapInfoPl,size);
       }

     // COPY THE REGION BACK IN...
     for (j=0;j<mapheight;j++)
       for (k=0;k<newwidth;k++)
	 {
	  if (b)
	    MapBkgnd[j*newwidth+k]=tempB[j*newwidth+k];
	  if (f)
	    MapFrgnd[j*newwidth+k]=tempF[j*newwidth+k];
	  if (i)
	    MapInfoPl[j*newwidth+k]=tempI[j*newwidth+k];
	 }

     // TOAST THE 'TEMP' BUFFERS & REALLOCATE THE 'CUT' BUFFERS
     if (b)
       {
	MMFreePtr((memptr *)&tempB);
	MMAllocate((memptr *)&CutBkgnd,size);
       }
     if (f)
       {
	MMFreePtr((memptr *)&tempF);
	MMAllocate((memptr *)&CutFrgnd,size);
       }
     if (i)
       {
	MMFreePtr((memptr *)&tempI);
	MMAllocate((memptr *)&CutInfoPl,size);
       }

     mapwidth=newwidth;
     MapHeader.width=newwidth;
     modified=DirtyFlag=1;
     AllocateUndoBuffers();
     UndoRegion.x=-1;
     SaveUndo(0,0,mapwidth,mapheight);
     RestoreCutBuffers();
     break;
  }
 } while(which && which!=5);
 RestoreBackground();
 if (modified)
   {
    xbase=ybase=0;
    MouseHide();
    InitDesktop(TED5MenuBar,0);
    DrawInfoBar();
    DrawMap();
    MouseShow();
   }
}


////////////////////////////////////////////////////
//
// Display amount of memory available
//
////////////////////////////////////////////////////
btype     ShowMemb={" OK ",6,11,2};
DialogDef ShowMemd={"Extended Memory \n"
		    "Available:\n\n"
		    "Main Memory\n"
		    "Available:\n\n"
		    "In Extended Mem\n"
		    "----------------\n"
		    "CGA:     EGA:\n"
		    "VGA:    Maps:"
		    ,16,13,1,&ShowMemb,PrintMem};

void Item_PrintMem(void)
{
 DoDialog(&ShowMemd);
}

void PrintMem(int x,int y)
{
 long total;
 char memstring[10];

 sx=x+10;
 sy=y+1;
 itoa(XMSTotalFree(),memstring,10);
 strcat(memstring,"K");
 print(memstring);

 sx=x+10;
 sy=y+4;
 total=(MMTotalFree()*16L)/1024;
 ltoa(total,memstring,10);
 strcat(memstring,"K");
 print(memstring);

 sx=x+4;
 sy=y+8;
 if (!CgaXMS)
   print("No");
 else
   print("Yes");

 sx=x+13;
 sy=y+8;
 if (!EgaXMS)
   print("No");
 else
   print("Yes");

 sx=x+4;
 sy=y+9;
 if (!VgaXMS)
   print("No");
 else
   print("Yes");


 sx=x+13;
 sy=y+9;
 if (!XMSmaps)
   print("No");
 else
   print("Yes");
}


////////////////////////////////////////////////////
//
// CARMACIZE THE MAP FILE!
//
////////////////////////////////////////////////////
void Item_Huffman(void)
{
 OutputHeadStr NewFileHeader;
 char huffname[14]="GAMEMAPS.",huffheadname[14]="MAPSHEAD.",tempstr[200],
	mapidstr[8]=IDSTRING,objname[14],dictname[14],tempname[14];
 int dx,dy,i,oxb,oyb,maplengths[100];
 MapHeaderStr TempHeader;
 memptr block,block1,block2;
 long size,fsize,nsize;


 Item_SaveMap();

 strcat(huffname,ext);
 strcat(huffheadname,ext);
 strcpy(tempstr,"This will take a while.\n");
 strcat(tempstr,huffname);
 strcat(tempstr,". [               ]");
 ErrDialog(tempstr,"");
 dy=sy;
 dx=sx-16;

 oxb=xbase;
 oyb=ybase;
 //
 // Free up memory
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
 // Now, time to Carmacize all the maps
 //
 sx=dx;
 sy=dy;
 print("Carmacizing");
 dx=sx+1;

 fsize=0;
 SaveFile(huffname,(char huge *)&mapidstr,fsize,strlen(mapidstr));
 fsize+=strlen(mapidstr);

 memset(&maplengths,0,sizeof(long)*100);
 memset(&NewFileHeader,0,sizeof(OutputHeadStr));
 MMAllocate(&block1,sizeof(MapHeaderStr));
 for (i=0;i<100;i++)
   if (MapFileHeader->dataoffsets[i]!=-1)
     {
      //
      // LOAD MAP HEADER
      //
      sx=dx;
      printint(i);
      LoadFile(mapname,(char huge *)&TempHeader,MapFileHeader->dataoffsets[i],sizeof(MapHeaderStr));

      //
      // COMPRESS EACH MAP PLANE
      //
      #pragma warn -sus
      if (MapFileHeader->maptype&BPLANE)
	{
	 size=TempHeader.mapbkgndlen;
	 MMAllocate(&block,size);
	 MMAllocate(&block2,size);
	 LoadFile(mapname,block,TempHeader.mapbkgndpl,size);
	 *(int _seg *)block2=TempHeader.mapbkgndlen;
	 nsize=CarmackCompress((unsigned  char huge *)block,size,(unsigned char huge *)block2+2)+2;
	 maplengths[i]+=nsize;
	 if (nsize==2)
	 {
	  RestoreBackground();
	  MMFreePtr(&block2);
	  MMFreePtr(&block);
	  MMFreePtr(&block1);
	  LoadMap(whichmap);
	  xbase=oxb;
	  ybase=oyb;
	  ErrDialog("ESC out of this infernal thing!"," YES! ");
	  return;
	 }

	 SaveFile(huffname,block2,fsize,nsize);
	 TempHeader.mapbkgndpl=fsize;
	 TempHeader.mapbkgndlen=nsize;
	 MMFreePtr(&block2);
	 MMFreePtr(&block);
	 fsize+=nsize;
	}
      if (MapFileHeader->maptype&FPLANE)
	{
	 size=TempHeader.mapfrgndlen;
	 MMAllocate(&block,size);
	 MMAllocate(&block2,size);
	 LoadFile(mapname,block,TempHeader.mapfrgndpl,size);
	 *(int _seg *)block2=TempHeader.mapfrgndlen;
	 nsize=CarmackCompress((unsigned  char huge *)block,size,(unsigned  char huge *)block2+2)+2;
	 maplengths[i]+=nsize;
	 if (nsize==2)
	 {
	  RestoreBackground();
	  MMFreePtr(&block2);
	  MMFreePtr(&block);
	  MMFreePtr(&block1);
	  LoadMap(whichmap);
	  xbase=oxb;
	  ybase=oyb;
	  ErrDialog("ESC out of this infernal thing!"," YES! ");
	  return;
	 }

	 SaveFile(huffname,block2,fsize,nsize);
	 TempHeader.mapfrgndpl=fsize;
	 TempHeader.mapfrgndlen=nsize;
	 MMFreePtr(&block2);
	 MMFreePtr(&block);
	 fsize+=nsize;
	}
      if (MapFileHeader->maptype&IPLANE)
	{
	 size=TempHeader.mapinfolen;
	 MMAllocate(&block,size);
	 MMAllocate(&block2,size);
	 LoadFile(mapname,block,TempHeader.mapinfopl,size);
	 *(int _seg *)block2=TempHeader.mapinfolen;
	 nsize=CarmackCompress((unsigned  char huge *)block,size,(unsigned  char huge *)block2+2)+2;
	 maplengths[i]+=nsize;
	 if (nsize==2)
	 {
	  RestoreBackground();
	  MMFreePtr(&block2);
	  MMFreePtr(&block);
	  MMFreePtr(&block1);
	  LoadMap(whichmap);
	  xbase=oxb;
	  ybase=oyb;
	  ErrDialog("ESC out of this infernal thing!"," YES! ");
	  return;
	 }

	 SaveFile(huffname,block2,fsize,nsize);
	 TempHeader.mapinfopl=fsize;
	 TempHeader.mapinfolen=nsize;
	 MMFreePtr(&block2);
	 MMFreePtr(&block);
	 fsize+=nsize;
	}
      #pragma warn +sus

      //
      // SAVE MAP HEADER
      //
      nsize=sizeof(TempHeader);
      maplengths[i]+=nsize;
      SaveFile(huffname,(char huge *)&TempHeader,fsize,nsize);
      NewFileHeader.dataoffsets[i]=fsize;
      fsize+=nsize;

      SaveFile(huffname,"!ID!",fsize,4);
      fsize+=4;
     }
 MMFreePtr(&block1);


 //
 // COPY PERTINENT MAPFILEHEADER DATA TO NEWFILEHEADER
 //
 {
  char outname[14],tempname[14]="MTEMP.TMP",doutname[14];


  strcpy(outname,ext);
  strcat(outname,"MHEAD.OBJ");

  NewFileHeader.RLEWtag=MapFileHeader->RLEWtag;
  fsize=sizeof(OutputHeadStr);
  SaveFile(tempname,(char huge *)&NewFileHeader,0,fsize);

  for (i=0;i<numtplanes;i++)
	{
	 SaveFile(tempname,MK_FP(Tinfo[i],0),fsize,tilenum);
	 fsize+=tilenum;
	}
  for (i=0;i<numtmplanes;i++)
	{
	 SaveFile(tempname,MK_FP(TMinfo[i],0),fsize,tilemnum);
	 fsize+=tilemnum;
	}

  MakeOBJ(tempname,outname,"_maphead",FARDATA,"MapHeader");
//  unlink(tempname);
 }

 RestoreBackground();
 SignalSound();

 //
 // FINSIHED. PRINT REPORT?
 //
 if (Message("Finished with compression.\n"
	     "Print report on lengths?")==2)
 {
  fprintf(stdprn,"TED5 Carmacized Map Lengths for .%s\n\n",ext);
  fprintf(stdprn,"#\tMap Name\t\tLength\n");
  fprintf(stdprn,"_\t--------\t\t------\n");
  for (i=0;i<100;i++)
    if (MapFileHeader->dataoffsets[i]!=-1)
    {
     char tstr[16];
     strcpy(tstr,MapNames[i]);
     while(strlen(tstr)<16)
       strcat(tstr," ");
     fprintf(stdprn,"%d\t%s\t%d\n",i,tstr,maplengths[i]);
    }
  fprintf(stdprn,"%c",12);
 }

 LoadMap(whichmap);
 xbase=oxb;
 ybase=oyb;
}


////////////////////////////////////////////////////
//
// Item - Change the LAUNCH name
//
////////////////////////////////////////////////////
btype CLNb={"                                    ",1,5,1};
DialogDef CLNd={"Current LAUNCH name:\n\n\nNew LAUNCH name:",38,7,1,&CLNb,NULL};

void Item_LAUNCHname(void)
{
 char tempstr[40];


 MouseHide();
 DrawDialog(&CLNd,1);
 GetDialogXY(&CLNd,&sx,&sy);
 sy++;
 sx++;
 print(launchname);
 GetButtonXY(&CLNd,0,&sx,&sy);
 if (input(tempstr,36))
 {
  strcpy(launchname,tempstr);
  SaveTEDInfo();
 }
 MouseShow();
 RestoreBackground();
}


////////////////////////////////////////////////////
//
// Item - Change the PARM string
//
////////////////////////////////////////////////////
btype CPSb={"                                    ",1,5,1};
DialogDef CPSd={"Current PARM string:\n\n\nNew PARM string:",38,7,1,&CPSb,NULL};

void Item_PARMstring(void)
{
 char tempstr[40];


 MouseHide();
 DrawDialog(&CPSd,1);
 GetDialogXY(&CPSd,&sx,&sy);
 sy++;
 sx++;
 print(parmstring);
 GetButtonXY(&CPSd,0,&sx,&sy);
 if (input(tempstr,36))
 {
  strcpy(parmstring,tempstr);
  _fstrcpy(TEDInfo->parmstring,(char far *)parmstring);
  SaveTEDInfo();
 }
 MouseShow();
 RestoreBackground();
}


////////////////////////////////////////////////////
//
// Item - Change Icon Rows
//
////////////////////////////////////////////////////
btype CIRb={"    ",8,4,1};
DialogDef CIRd={"Enter amount of icons\n"
		"to add/delete.\n"
		"Use + or -.",22,6,1,&CIRb,NULL};

void Item_ChangeIconRows(void)
{
 unsigned i,j,dx,dy,max,oxbase,oybase,base;
 int value,owm;
 memptr block;


 if (!(MapFileHeader->maptype&IPLANE))
 {
  ErrDialog("You don't have an icon plane!"," OK ");
  return;
 }

 CheckForMapSave();

 oxbase=xbase;
 oybase=ybase;

 DrawDialog(&CIRd,1);
 GetButtonXY(&CIRd,0,&dx,&dy);
 sx=dx;
 sy=dy;
 MouseHide();
 value=inputint(3);
 MouseShow();
 if (!value || value==(int)ESCOUT)
 {
  RestoreBackground();
  return;
 }

 value=SGN(value)*(18*((abs(value)+17)/18));

 base=18*maxiconrows;
 maxiconrows+=value/18;
 MapFileHeader->NumIconRows+=value/18;

 owm=whichmap;
 RestoreBackground();


 //
 // MAKE SURE WE ADJUST TILEINFOM!
 //
 MMAllocate(&block,tilemnum+value);
 for (i=0;i<numtmplanes;i++)
 {
  _fmemset(block,0,tilemnum+value);
  movedata((unsigned)TMinfo[i],0,(unsigned)block,0,base);
  movedata((unsigned)TMinfo[i],base,
	   (unsigned)block,base+value,tilemnum-base);
  MMFreePtr((memptr *)&TMinfo[i]);
  MMAllocate((memptr *)&TMinfo[i],tilemnum+value);
  movedata((unsigned)block,0,(unsigned)TMinfo[i],0,tilemnum+value);
 }
 MMFreePtr(&block);
 MapFileHeader->oldtilemnum=tilemnum+value;


 //
 // RUN THROUGH EACH MAP AND ADJUST TILEM VALUES
 //
 for (i=0;i<100;i++)
   if (MapFileHeader->dataoffsets[i]>=0)
     {
      whichmap=i;
      LoadMap(i);
      max=mapwidth*mapheight;
      for (j=0;j<max;j++)
	if (MapFrgnd[j]>0)
	  MapFrgnd[j]+=value;
      DirtyFlag=1;
      SaveMap(0);
     }

 DirtyFlag=0;
 whichmap=owm;
 LoadMap(whichmap);
 xbase=oxbase;
 ybase=oybase;
 DrawMap();
 ErrDialog("If your map looks messed up,\n"
	   "you need to change the amount\n"
	   "of icons in your IGRAB script\n"
	   "and re-grab your tiles!"," OK ");
}


////////////////////////////////////////////////////
//
// Item - Change LAUNCH Icon
//
////////////////////////////////////////////////////
void Item_ChangeLaunchIcon(void)
{
 sound(1700);
 TEDInfo->permicon=whichi-tilenum;
 delay(30);
 nosound();
}


////////////////////////////////////////////////////
//
// Item - Change Background color
//
////////////////////////////////////////////////////
btype CBCb[]={{"\xb",5,2,1},
	      {"\xc",16,2,1},
	      {"Exit",9,2,2}};
DialogDef CBCd={"Change Backgrnd Color!",22,4,3,&CBCb[0],0};
void Item_ChangeBkgndColor(void)
{
 int which;

 do
 {
  which=DoDialog(&CBCd);
  switch(which)
  {
   case 1:
     sound(1700);
     if (--BkgndColor<0)
       BkgndColor=15;
     TEDInfo->BackgndColor=BkgndColor;
     DrawMap();
     DrawInfoBar();
     break;
   case 2:
     sound(1700);
     if (++BkgndColor>15)
       BkgndColor=0;
     TEDInfo->BackgndColor=BkgndColor;
     DrawMap();
     DrawInfoBar();
  }
  nosound();
 } while(which>0 && which<3);
}
