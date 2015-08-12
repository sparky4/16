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

#include <conio.h>
#include <string.h>
#include <malloc.h>

#include "mode13h.h" // SetMode()
#include "modex.h"
#include "vdriver.h"

// ================================= Data ====================================

#define SEQU_ADDR               0x3c4
#define CRTC_ADDR               0x3d4
#define MISC_OUTPUT             0x3c2

char *screen1, *screen2;            // page banks;
char *screenx, curscr;              // ptr to active page | page flip ctr
char *screenbase;                   // ptr to A000:0000

extern unsigned char* translucency_table; //zero

extern void CheckTimerStuff();
extern void *valloc(int amount,char*,int);
extern int  vfree(void *);

// ================================= Code ====================================

static void plane(char p)
{
  int hi=1<<p;
  outpw(0x03c4, (hi<<8)|0x02);
}

extern void SetModeX(void);

int ModeXShutdown()
{
  SetMode(0x3);
  vfree(screen);
  return 0;
}

int ModeXShowPage()
{ char *s,*se,*d,*de;
  int k;

  CheckTimerStuff();

  s=screen+(16*tsx)+16;
  se=s+(sy*tsx);
  d=screenx;

  while (s<se)
  {
    for (k=0; k<4; k++)
    {
      plane(k);
      de=d+(sx>>2);

      while (d<de)
      {
        d[0]=s[k];
        d[1]=s[k+4];
        s+=8;
        d+=2;
      }

      s-=sx;
      d-=(sx>>2);
    }

    s+=tsx;
    d+=(sx>>2);
  }
  return 0;
}

int ModeXCopySprite(int x, int y, int width, int height, char *src)
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

int ModeXCCopySprite(int x,int y,int width,int height,char *src)
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

int ModeXTCCopySprite(int x,int y,int width,int height,char *src)
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

  s=src;
  if (xs+ys) s+=(ys*width)+xs;
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

int ModeXTCopySprite(int x, int y, int width, int height, char *src)
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

int ModeXCopyTile(int x, int y, char *src)
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

int ModeXVLine(int x, int y, int length, char color)
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

int ModeXHLine(int x, int y, int width, char color)
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

int ModeXFilledBox(int x, int y, int width, int height, char c)
{
  for (; height; height--,y++)
    ModeXHLine(x, y, width, c);

  return 0;
}

int ModeXColorGrid(int x, int y, char c)
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

int ModeXClearScreen()
{
  memset(screen,0,90117);
  return 0;
}

int ModeXCopySpriteLucentClip(int x, int y, int width, int height, unsigned char *src)
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


void InitModeX()
{ char in_byte;

  SetMode(0x13);

  outpw(SEQU_ADDR, 0x0604);
  memset((unsigned char *)0xA0000, 0, 0x10000);
  outpw(SEQU_ADDR, 0x0110);
  outp(MISC_OUTPUT, 0xe3);
  outpw(SEQU_ADDR, 0x0300);
  outp(CRTC_ADDR, 0x11);
  in_byte = inp((CRTC_ADDR+1));
  in_byte = (in_byte && 0x7f);
  outp((CRTC_ADDR+1), in_byte);
  outpw(CRTC_ADDR, 0x0d06);
  outpw(CRTC_ADDR, 0x3e07);
  outpw(CRTC_ADDR, 0x4109);
  outpw(CRTC_ADDR, 0xea10);
  outpw(CRTC_ADDR, 0xac11);
  outpw(CRTC_ADDR, 0xdf12);
  outpw(CRTC_ADDR, 0x0014);
  outpw(CRTC_ADDR, 0xe715);
  outpw(CRTC_ADDR, 0x0616);
  outpw(CRTC_ADDR, 0xe317);

  //sx=320;  sy=240;
  //tsx=352; tsy=272;
  //tx=20;   ty=15;
  //endcol=336; nextl=80;
  //winofs=5632;

  screenx=(char *) 0xA0000;
  screen=(char *) valloc(352*(256+16),"screen",0);
  //memset(screen,0,95744);
  curscr=0;

  sx=320;  sy=240;
  tsx=352; tsy=256;
  tx=20;   ty=15;

  // Mode successfuly set, now lets set up the driver.
  ShutdownVideo=&ModeXShutdown;
  ShowPage=&ModeXShowPage;
  CopySprite=&ModeXCopySprite;
  CCopySprite=&ModeXCCopySprite;
  TCCopySprite=&ModeXTCCopySprite;
  TCopySprite=&ModeXTCopySprite;
  CopyTile=&ModeXCopyTile;
  FilledBox=&ModeXFilledBox;
  HLine=&ModeXHLine;
  VLine=&ModeXVLine;
  ColorGrid=&ModeXColorGrid;
  ClearScreen=&ModeXClearScreen;
  CopySpriteLucentClip=&ModeXCopySpriteLucentClip;

  map_scroll_x =
  map_scroll_y = 2;
}


