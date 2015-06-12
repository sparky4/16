/*
Copyright (C) 1998 BJ Eirich (aka vecna)
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public Lic
See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <string.h>
#include <malloc.h>

#include "mode13h.h"
#include "vdriver.h"

// ================================= Data ====================================

char *screenr;                      // realscr
char startline=16;                  // virtual scroll around 240 vert. pixels
extern unsigned char* translucency_table; //zero

extern void *valloc(int amount, char*, int);
extern int  vfree(void *);
extern void CheckTimerStuff(); // in maped.c

// ================================= Code ====================================


void SetMode(int mode)
{
  REGISTERS r;
  SET_AX(r, mode);
  INTERRUPT(0x10, r);
}

int Mode13hShutdown()
{
  SetMode(0x3);
  vfree(screen);
  screen=0;
  return 0;
}

int Mode13hShowPage()
{ int rows;
  char *s,*d;

  CheckTimerStuff();

  s=screen+(16*tsx)+16;
  d=screenr;
  rows=sy;

  for (; rows; rows--)
  {
    memcpy(d,s,sx);
    s+=tsx;
    d+=sx;
  }

  return 0;
}

int Mode13hCopySprite(int x, int y, int width, int height, char *src)
{ char *d;

  d=screen+(y*tsx)+x;
  for (; height; height--)
  {
    memcpy(d,src,width);
    src+=width;
    d+=tsx;
  }

  return 0;
}

int Mode13hCCopySprite(int x,int y,int width,int height,char *src)
{ int cx1,cy1,cx2,cy2;
  char *s,*d;
  int xl,yl,xs,ys;

  xl=width;
  yl=height;
  xs=ys=0;

  cx1=0;
  cy1=0;
  cx2=tsx-1;
  cy2=tsy-1;

  if (x>cx2 || y>cy2 || x+xl<cx1 || y+yl<cy1)
    return 0;

  if (x+xl > cx2) xl=cx2-x+1;
  if (y+yl > cy2) yl=cy2-y+1;

  if (x<cx1) { xs=cx1-x; xl-=xs; x=cx1; }
  if (y<cy1) { ys=cy1-y; yl-=ys; y=cy1; }

  s=src;
  if (xs+ys) s+=(ys*width)+xs;
  d=screen+(y*tsx)+x;

  for (; yl; yl--)
  {
    memcpy(d,s,xl);
    s+=width;
    d+=tsx;
  }

  return 0;
}

int Mode13hTCCopySprite(int x,int y,int width,int height,char *src)
{ int cx1,cy1,cx2,cy2;
  char *s,*d;
  int xl,yl,xs,ys;
  char c;

  xl=width;
  yl=height;
  xs=ys=0;

  cx1=0;
  cy1=0;
  cx2=tsx-1;
  cy2=tsy-1;

  if (x>cx2 || y>cy2 || x+xl<cx1 || y+yl<cy1)
    return 0;

  if (x+xl > cx2) xl=cx2-x+1;
  if (y+yl > cy2) yl=cy2-y+1;

  if (x<cx1) { xs=cx1-x; xl-=xs; x=cx1; }
  if (y<cy1) { ys=cy1-y; yl-=ys; y=cy1; }

  s=src+(ys*width)+xs;
  d=screen+(y*tsx)+x;

  for (; yl; yl--)
  {
    for (x=0; x<xl; x++)
    {
      c=s[x];
      if (c)
        d[x]=c;
    }
    s+=width;
    d+=tsx;
  }

  return 0;
}

int Mode13hTCopySprite(int x, int y, int width, int height, char *src)
{ char *d;
  char c;

  d=screen+(y*tsx)+x;
  for (; height; height--)
  {
    for (x=0; x<width; x++)
    {
      c=src[x];
      if (c)
        d[x]=c;
    }
    src+=width;
    d+=tsx;
  }

  return 0;
}

int Mode13hCopyTile(int x, int y, char *src)
{ int h;
  char *d;

  h=16;
  d=screen+(y*tsx)+x;
  for (; h; h--)
  {
    memcpy(d, src, 16);
    src+=16;
    d+=tsx;
  }

  return 0;
}

int Mode13hVLine(int x, int y, int length, char color)
{ int cx1,cy1,cx2,cy2;
  char *d;

  cx1=0;
  cy1=0;
  cx2=tsx-1;
  cy2=tsy-1;

  if (x>cx2 || y>cy2 || x<cx1 || y+length<cy1)
    return 0;

  if (y+length > cy2) length=cy2-y+1;
  if (y<cy1) { length-=(cy1-y); y=cy1; }

  d=screen+(y*tsx)+x;
  for (; length; length--)
  {
    *d=color;
    d+=tsx;
  }

  return 0;
}

int Mode13hHLine(int x, int y, int width, char color)
{ int cx1,cy1,cx2,cy2;
  char *d;

  cx1=0;
  cy1=0;
  cx2=tsx-1;
  cy2=tsy-1;

  if (x>cx2 || y>cy2 || x+width<cx1 || y<cy1)
    return 0;

  if (x+width > cx2) width=cx2-x+1;
  if (x<cx1) { width-=(cx1-x); x=cx1; }

  d=screen+(y*tsx)+x;
  memset(d,color,width);

  return 0;
}

int Mode13hFilledBox(int x, int y, int width, int height, char c)
{
  for (; height; height--,y++)
    Mode13hHLine(x, y, width, c);

  return 0;
}

int Mode13hColorGrid(int x, int y, char c)
{ char *d;

  if (x<0 || x>336 || y<0 || y>256) return 0;
  d=screen+(y*tsx)+x;
  for (y=0; y<8; y++)
  {
    d[0]=c; d[2]=c; d[4]=c; d[6]=c;
    d[8]=c; d[10]=c; d[12]=c; d[14]=c;

    d+=(tsx+1);

    d[0]=c; d[2]=c; d[4]=c; d[6]=c;
    d[8]=c; d[10]=c; d[12]=c; d[14]=c;

    d+=(tsx-1);
  }

  return 0;
}

int Mode13hClearScreen()
{
  memset(screen,0,76032);
  return 0;
}

int Mode13hCopySpriteLucentClip(int x, int y, int width, int height, unsigned char *src)
{
 int cx1,cy1,cx2,cy2;
  unsigned char *s,*d,c;
  int xl,yl,xs,ys;

  cx1=0;
  cy1=0;
  cx2=tsx-1;
  cy2=tsy-1;

  xl=width;
  yl=height;
  xs=ys=0;
  if (x>cx2 || y>cy2 || x+xl<cx1 || y+yl<cy1)
    return 1;
  if (x+xl > cx2) xl=cx2-x+1;
  if (y+yl > cy2) yl=cy2-y+1;
  if (x<cx1) { xs=cx1-x; xl-=xs; x=cx1; }
  if (y<cy1) { ys=cy1-y; yl-=ys; y=cy1; }

  s=src;
  if (ys) s+=(ys*width); // only perform mul if necessary ;)
  if (xs) s+=xs;
  d=screen+(y*tsx)+x;

  for (; yl; yl--)
  {
    for (x=0; x<xl; x++)
    {
      c=s[x];
      if (c)
        d[x]=translucency_table[d[x]|(c<<8)];
    }
    s+=width;
    d+=tsx;
  }
  return 1;
}

void InitMode13h()
{
  SetMode(0x13);

  screenr=(char *) 0xA0000;
  screen=(char *) valloc(352*(216+16),"Screen",0);
  //memset(screen,0,95744);

  sx=320;  sy=200;
  tsx=352; tsy=216;
  tx=20;   ty=13;

  // Mode successfuly set, now lets set up the driver.
  ShutdownVideo=&Mode13hShutdown;
  ShowPage=&Mode13hShowPage;
  CopySprite=&Mode13hCopySprite;
  CCopySprite=&Mode13hCCopySprite;
  TCCopySprite=&Mode13hTCCopySprite;
  TCopySprite=&Mode13hTCopySprite;
  CopyTile=&Mode13hCopyTile;
  FilledBox=&Mode13hFilledBox;
  VLine=&Mode13hVLine;
  HLine=&Mode13hHLine;
  ColorGrid=&Mode13hColorGrid;
  ClearScreen=&Mode13hClearScreen;
  CopySpriteLucentClip=&Mode13hCopySpriteLucentClip;

  map_scroll_x =
  map_scroll_y = 2;
}
