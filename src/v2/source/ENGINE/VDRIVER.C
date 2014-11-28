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

#define VDRIVER_H

#include <malloc.h>
#include <mem.h>
#include <conio.h>
#include <math.h>
#include "verge.h"

#define SWAP(a,b) { a-=b; b+=a; a=b-a; }

// ================================= Data ====================================

char *DriverDesc;

byte *screen,*video,*vscreen;
byte pal[768], pal2[768];
byte *translucency_table;

int sx, sy;
int tx, ty;
int tsx, tsy;
int cx1=0,cy1=0,cx2=319,cy2=199;

// Driver function-pointers.
int (*ShutdownVideo) (int i);
int (*ShowPage) (void);

// ================================= Code ====================================

void SetPalette(byte *pall)
{ quad i;

  outp(0x03c8, 0);
  for (i=0; i<768; i++)
    outp(0x03c9, pall[i]);
}

void GetPalette()
{ quad i;

  outp(0x03c7, 0);
  for (i=0; i<768; i++)
    pal[i]=inp(0x03c9);
}

void set_intensity(quad n)
{ quad i;

  for (i=0; i<768; i++)
    pal2[i] = (pal[i] * n) >> 6;

  SetPalette(pal2);
}

int LFB_ShowPage(void)
{
  if (key[SCAN_ALT] && key[SCAN_X]) err("Exiting: ALT-X pressed.");
  RenderGUI();
  cpubyte=PFLIP;
  memcpy(video,screen,sx*sy);
  cpubyte=ETC;
  return 0;
}

void CopySprite(int x, int y, int width, int height, byte *src)
{
  byte *d;

  cpubyte=RENDER;
  d=screen+(y*sx)+x;
  for (; height; height--)
  {
    memcpy(d,src,width);
    src+=width;
    d+=sx;
  }
  cpubyte=ETC;
}

void TCopySprite(int x, int y, int width, int height, byte *src)
{
  byte *d, c;

  cpubyte=RENDER;
  d=screen+(y*sx)+x;
  for (; height; height--)
  {
    for (x=0; x<width; x++,src++)
    {
      c=*src;
      if (c)
        d[x]=c;
    }
    d+=sx;
  }
  cpubyte=ETC;
}

void CopySpriteLucent(int x, int y, int width, int height, byte *src)
{
  byte *d;

  cpubyte=RENDER;
  d=screen+(y*sx)+x;
  for (; height; height--)
  {
    for (x=0; x<width; x++,src++)
      d[x]=translucency_table[d[x]|(*src<<8)];
    d+=sx;
  }
  cpubyte=ETC;
}

void TCopySpriteLucent(int x, int y, int width, int height, byte *src)
{
  byte *d, c;

  cpubyte=RENDER;
  d=screen+(y*sx)+x;
  for (; height; height--)
  {
    for (x=0; x<width; x++,src++)
    {
      c=*src;
      if (c)
        d[x]=translucency_table[d[x]|(c<<8)];
    }
    d+=sx;
  }
  cpubyte=ETC;
}

void CopySpriteZoom(int x, int y, int sw, int sh, int dw, int dh, byte *src)
{
  int i,j;
  int xerr,yerr;
  int xadj,yadj;
  byte *d;

  cpubyte=RENDER;
  if (dw<1 || dh<1) return;
  xadj = (sw<<16)/dw;
  yadj = (sh<<16)/dh;
  d = screen+(y*sx)+x;
  yerr=0;

  for (i=0; i<dh; i++)
  {
    xerr=0;
    for (j=0; j<dw; j++)
    {
      d[j]=src[(xerr>>16)];
      xerr+=xadj;
    }
    d+=sx;
    yerr+=yadj;
    src+=(yerr>>16)*sw;
    yerr &= 0xffff;
  }
  cpubyte=ETC;
}

void TCopySpriteZoom(int x, int y, int sw, int sh, int dw, int dh, byte *src)
{
  int i,j;
  int xerr,yerr;
  int xadj,yadj;
  byte *d;
  byte c;

  cpubyte=RENDER;
  if (dw<1 || dh<1) return;
  xadj = (sw<<16)/dw;
  yadj = (sh<<16)/dh;
  d = screen+(y*sx)+x;
  yerr=0;
  for (i=0; i<dh; i++)
  {
    xerr=0;
    for (j=0; j<dw; j++)
    {
      c=src[(xerr>>16)];
      if (c)
        d[j]=c;
      xerr+=xadj;
    }
    d+=sx;
    yerr+=yadj;
    src+=(yerr>>16)*sw;
    yerr &= 0xffff;
  }
  cpubyte=ETC;
}

void CopySpriteZoomLucent(int x, int y, int sw, int sh, int dw, int dh, byte *src)
{
  int i,j;
  int xerr,yerr;
  int xadj,yadj;
  byte *d;

  cpubyte=RENDER;
  if (dw<1 || dh<1) return;
  xadj = (sw<<16)/dw;
  yadj = (sh<<16)/dh;
  d = screen+(y*sx)+x;
  yerr=0;

  for (i=0; i<dh; i++)
  {
    xerr=0;
    for (j=0; j<dw; j++)
    {
      d[j]=translucency_table[d[j]|(src[(xerr>>16)]<<8)];
      xerr+=xadj;
    }
    d+=sx;
    yerr+=yadj;
    src+=(yerr>>16)*sw;
    yerr &= 0xffff;
  }
  cpubyte=ETC;
}

void TCopySpriteZoomLucent(int x, int y, int sw, int sh, int dw, int dh, byte *src)
{
  int i,j;
  int xerr,yerr;
  int xadj,yadj;
  byte *d;
  byte c;

  cpubyte=RENDER;
  if (dw<1 || dh<1) return;
  xadj = (sw<<16)/dw;
  yadj = (sh<<16)/dh;
  d = screen+(y*sx)+x;
  yerr=0;

  for (i=0; i<dh; i++)
  {
    xerr=0;
    for (j=0; j<dw; j++)
    {
      c=src[(xerr>>16)];
      if (c)
        d[j]=c;
      xerr+=xadj;
    }
    d+=sx;
    yerr+=yadj;
    src+=(yerr>>16)*sw;
    yerr &= 0xffff;
  }
  cpubyte=ETC;
}

void Silhouette(int x, int y, int width, int height, int color, byte *src)
{
  byte *d=0;

  cpubyte=RENDER;
  if (height<1 || width<1) return;
  d=screen+(y*sx)+x;
  do {
    for (x=0; x<width; x++)
    {
      if (src[x])
        d[x]=color;
    }
    src+=width;
    d+=sx;
  } while (--height);
  cpubyte=ETC;
}

void SilhouetteZoom(int x, int y, int sw, int sh, int dw, int dh, int color, byte *src)
{
  int xerr=0,yerr=0;
  int xadj=0,yadj=0;
  byte *d=0;

  cpubyte=RENDER;
  if (dw<1 || dh<1) return;
  xadj=(sw<<16)/dw;
  yadj=(sh<<16)/dh;
  d=screen+(y*sx)+x;
  yerr=0;
  do {
    xerr=0;
    for (x=0; x<dw; x++)
    {
      if (src[(xerr>>16)])
        d[x]=color;
      xerr+=xadj;
    }
    d+=sx;
    yerr+=yadj;
    src+=(yerr>>16)*sw;
    yerr &= 0xffff;
  } while (--dh);
  cpubyte=ETC;
}

void SilhouetteLucent(int x, int y, int width, int height, int color, byte *src)
{
  byte *d=0;
  byte *range=0;

  cpubyte=RENDER;
  if (width<1 || height<1) return;
  d=screen+(y*sx)+x;
  range=translucency_table+(color<<8);
  do {
    for (x=0; x<width; x++)
    {
      if (src[x])
        d[x]=range[d[x]];
    }
    src+=width;
    d+=sx;
  } while (--height);
  cpubyte=ETC;
}

void SilhouetteZoomLucent(int x, int y, int sw, int sh, int dw, int dh, int color, byte *src)
{
  int xerr=0,yerr=0;
  int xadj=0,yadj=0;
  byte *d=0;
  byte *range=0;

  cpubyte=RENDER;
  if (dw<1 || dh<1) return;
  xadj=(sw<<16)/dw;
  yadj=(sh<<16)/dh;
  d=screen+(y*sx)+x;
  range=translucency_table+(color<<8);
  yerr=0;
  do {
    xerr=0;
    for (x=0; x<dw; x++)
    {
      if (src[(xerr>>16)])
        d[x]=range[d[x]];
      xerr+=xadj;
    }
    d+=sx;
    yerr+=yadj;
    src+=(yerr>>16)*sw;
    yerr &= 0xffff;
  } while (--dh);
  cpubyte=ETC;
}

// aen
void Tint(int x, int y, int width, int height, int color, byte *src)
{
  byte *d=0;
  byte *range=0;
  byte c=0;

  cpubyte=RENDER;
  if (width<1 || height<1) return;
  d=screen+(y*sx)+x;
  range=translucency_table+(color<<8);
  do {
    for (x=0; x<width; x++)
    {
      c=src[x];
      if (c)
        d[x]=range[c];
    }
    src+=width;
    d+=sx;
  } while (--height);
  cpubyte=ETC;
}

// aen
void TintZoom(int x, int y, int sw, int sh, int dw, int dh, int color, byte *src)
{
  int xerr=0,yerr=0;
  int xadj=0,yadj=0;
  byte *d=0;
  byte *range=0;
  byte c=0;

  cpubyte=RENDER;
  if (dw<1 || dh<1) return;
  xadj=(sw<<16)/dw;
  yadj=(sh<<16)/dh;
  d=screen+(y*sx)+x;
  range=translucency_table+(color<<8);
  yerr=0;
  do {
    xerr=0;
    for (x=0; x<dw; x++)
    {
      c=src[(xerr>>16)];
      if (c)
        d[x]=range[c];
      xerr+=xadj;
    }
    d+=sx;
    yerr+=yadj;
    src+=(yerr>>16)*sw;
    yerr &= 0xffff;
  } while (--dh);
  cpubyte=ETC;
}

void CopySpriteClip(int x, int y, int width, int height, byte *src)
{
  byte *s,*d;
  int xl,yl,xs,ys;

  cpubyte=RENDER;
  xl=width;
  yl=height;
  xs=ys=0;
  if (x>cx2 || y>cy2 || x+xl<cx1 || y+yl<cy1)
    return;

  if (x+xl > cx2) xl=cx2-x+1;
  if (y+yl > cy2) yl=cy2-y+1;
  if (x<cx1) { xs=cx1-x; xl-=xs; x=cx1; }
  if (y<cy1) { ys=cy1-y; yl-=ys; y=cy1; }

  s=src+(ys*width)+xs;
  d=screen+(y*sx)+x;

  for (; yl; yl--)
  {
    memcpy(d,s,xl);
    s+=width;
    d+=sx;
  }
  cpubyte=ETC;
}

void TCopySpriteClip(int x, int y, int width, int height, byte *src)
{
  byte *s,*d,c;
  int xl,yl,xs,ys;

  cpubyte=RENDER;
  xl=width;
  yl=height;
  xs=ys=0;
  if (x>cx2 || y>cy2 || x+xl<cx1 || y+yl<cy1)
    return;
  if (x+xl > cx2) xl=cx2-x+1;
  if (y+yl > cy2) yl=cy2-y+1;
  if (x<cx1) { xs=cx1-x; xl-=xs; x=cx1; }
  if (y<cy1) { ys=cy1-y; yl-=ys; y=cy1; }
  s=src+(ys*width)+xs;
  d=screen+(y*sx)+x;

  for (; yl; yl--)
  {
    for (x=0; x<xl; x++)
    {
      c=s[x];
      if (c)
        d[x]=c;
    }
    s+=width;
    d+=sx;
  }
  cpubyte=ETC;
}

void CopySpriteLucentClip(int x, int y, int width, int height, byte *src)
{
  byte *s,*d;
  int xl,yl,xs,ys;

  cpubyte=RENDER;
  xl=width;
  yl=height;
  xs=ys=0;
  if (x>cx2 || y>cy2 || x+xl<cx1 || y+yl<cy1)
    return;

  if (x+xl > cx2) xl=cx2-x+1;
  if (y+yl > cy2) yl=cy2-y+1;

  if (x<cx1) { xs=cx1-x; xl-=xs; x=cx1; }
  if (y<cy1) { ys=cy1-y; yl-=ys; y=cy1; }

  s=src+(ys*width)+xs;
  d=screen+(y*sx)+x;

  for (; yl; yl--)
  {
    for (x=0; x<xl; x++)
      d[x]=translucency_table[d[x]|(s[x]<<8)];

    s+=width;
    d+=sx;
  }
  cpubyte=ETC;
}

void TCopySpriteLucentClip(int x, int y, int width, int height, byte *src)
{
  byte *s,*d,c;
  int xl,yl,xs,ys;

  cpubyte=RENDER;
  xl=width;
  yl=height;
  xs=ys=0;
  if (x>cx2 || y>cy2 || x+xl<cx1 || y+yl<cy1)
    return;
  if (x+xl > cx2) xl=cx2-x+1;
  if (y+yl > cy2) yl=cy2-y+1;
  if (x<cx1) { xs=cx1-x; xl-=xs; x=cx1; }
  if (y<cy1) { ys=cy1-y; yl-=ys; y=cy1; }

  s=src+(ys*width)+xs;
  d=screen+(y*sx)+x;

  for (; yl; yl--)
  {
    for (x=0; x<xl; x++)
    {
      c=s[x];
      if (c)
        d[x]=translucency_table[d[x]|(c<<8)];
    }
    s+=width;
    d+=sx;
  }
  cpubyte=ETC;
}

void CopySpriteZoomClip(int x, int y, int sw, int sh, int dw, int dh, byte *src)
{
  int i,j;
  int xerr,yerr;
  int xerr_start, yerr_start;
  int xadj,yadj;
  byte *d;
  int xl,yl,xs,ys;

  cpubyte=RENDER;

  if (dw<1 || dh<1) return;
  xl=dw;
  yl=dh;
  xs=ys=0;
  if (x>cx2 || y>cy2 || x+xl<cx1 || y+yl<cy1)
    return;
  if (x+xl > cx2) xl=cx2-x+1;
  if (y+yl > cy2) yl=cy2-y+1;
  if (x<cx1) { xs=cx1-x; xl-=xs; x=cx1; }
  if (y<cy1) { ys=cy1-y; yl-=ys; y=cy1; }

  xadj = (sw<<16)/dw;
  yadj = (sh<<16)/dh;
  xerr_start = xadj * xs;
  yerr_start = yadj * ys;

  if (ys) src+=((yerr_start>>16)*sw);
  d = screen+(y*sx)+x;
  yerr = yerr_start & 0xffff;
  for (i=0; i<yl; i++)
  {
    xerr = xerr_start;
    for (j=0; j<xl; j++)
    {
      d[j]=src[(xerr>>16)];
      xerr+=xadj;
    }
    d+=sx;
    yerr += yadj;
    src += (yerr>>16)*sw;
    yerr &= 0xffff;
  }
  cpubyte=ETC;
}

void TCopySpriteZoomClip(int x, int y, int sw, int sh, int dw, int dh, byte *src)
{
  int i,j;
  int xerr,yerr;
  int xerr_start, yerr_start;
  int xadj,yadj;
  byte *d;
  byte c;
  int xl,yl,xs,ys;

  cpubyte=RENDER;
  if (dw<1 || dh<1) return;

  xl=dw;
  yl=dh;
  xs=ys=0;
  if (x>cx2 || y>cy2 || x+xl<cx1 || y+yl<cy1)
    return;

  if (x+xl > cx2) xl=cx2-x+1;
  if (y+yl > cy2) yl=cy2-y+1;
  if (x<cx1) { xs=cx1-x; xl-=xs; x=cx1; }
  if (y<cy1) { ys=cy1-y; yl-=ys; y=cy1; }
  xadj = (sw<<16)/dw;
  yadj = (sh<<16)/dh;
  xerr_start = xadj * xs;
  yerr_start = yadj * ys;

  if (ys) src+=((yerr_start>>16)*sw);
  d = screen+(y*sx)+x;
  yerr = yerr_start & 0xffff;
  for (i=0; i<yl; i++)
  {
    xerr = xerr_start;
    for (j=0; j<xl; j++)
    {
      c=src[(xerr>>16)];
      if (c)
        d[j]=c;
      xerr+=xadj;
    }
    d+=sx;
    yerr += yadj;
    src += (yerr>>16)*sw;
    yerr &= 0xffff;
  }
  cpubyte=ETC;
}

void CopySpriteZoomLucentClip(int x, int y, int sw, int sh, int dw, int dh, byte *src)
{
  int i,j;
  int xerr,yerr;
  int xerr_start, yerr_start;
  int xadj,yadj;
  byte *d;
  int xl,yl,xs,ys;

  cpubyte=RENDER;
  if (dw<1 || dh<1) return;
  xl=dw;
  yl=dh;
  xs=ys=0;

  if (x>cx2 || y>cy2 || x+xl<cx1 || y+yl<cy1)
    return;
  if (x+xl > cx2) xl=cx2-x+1;
  if (y+yl > cy2) yl=cy2-y+1;
  if (x<cx1) { xs=cx1-x; xl-=xs; x=cx1; }
  if (y<cy1) { ys=cy1-y; yl-=ys; y=cy1; }
  xadj = (sw<<16)/dw;
  yadj = (sh<<16)/dh;
  xerr_start = xadj * xs;
  yerr_start = yadj * ys;

  if (ys) src+=((yerr_start>>16)*sw);
  d = screen+(y*sx)+x;
  yerr = yerr_start & 0xffff;
  for (i=0; i<yl; i++)
  {
    xerr = xerr_start;
    for (j=0; j<xl; j++)
    {
      d[j]=translucency_table[d[j]|(src[(xerr>>16)]<<8)];
      xerr+=xadj;
    }
    d+=sx;
    yerr += yadj;
    src += (yerr>>16)*sw;
    yerr &= 0xffff;
  }
  cpubyte=ETC;
}

void TCopySpriteZoomLucentClip(int x, int y, int sw, int sh, int dw, int dh, byte *src)
{
  int i,j;
  int xerr,yerr;
  int xerr_start, yerr_start;
  int xadj,yadj;
  byte *d;
  int c;
  int xl,yl,xs,ys;

  cpubyte=RENDER;
  if (dw<1 || dh<1) return;
  xl=dw;
  yl=dh;
  xs=ys=0;
  if (x>cx2 || y>cy2 || x+xl<cx1 || y+yl<cy1)
    return;
  if (x+xl > cx2) xl=cx2-x+1;
  if (y+yl > cy2) yl=cy2-y+1;
  if (x<cx1) { xs=cx1-x; xl-=xs; x=cx1; }
  if (y<cy1) { ys=cy1-y; yl-=ys; y=cy1; }

  xadj = (sw<<16)/dw;
  yadj = (sh<<16)/dh;
  xerr_start = xadj * xs;
  yerr_start = yadj * ys;
  if (ys) src+=((yerr_start>>16)*sw);
    d = screen+(y*sx)+x;
  yerr = yerr_start & 0xffff;
  for (i=0; i<yl; i++)
  {
    xerr = xerr_start;
    for (j=0; j<xl; j++)
    {
      c=src[(xerr>>16)];
      if (c)
        d[j]=translucency_table[d[j]|(c<<8)];
      xerr+=xadj;
    }
    d+=sx;
    yerr += yadj;
    src += (yerr>>16)*sw;
    yerr &= 0xffff;
  }
  cpubyte=ETC;
}

void SilhouetteClip(int x, int y, int width, int height, int color, byte *src)
{
  byte *s=0,*d=0;
  int xl=0,yl=0,xs=0,ys=0;

  cpubyte=RENDER;
  xl=width;
  yl=height;
  xs=ys=0;
  if (x>cx2 || y>cy2 || x+xl<cx1 || y+yl<cy1)
    return;
  if (x+xl > cx2) xl=cx2-x+1;
  if (y+yl > cy2) yl=cy2-y+1;
  if (x<cx1) { xs=cx1-x; xl-=xs; x=cx1; }
  if (y<cy1) { ys=cy1-y; yl-=ys; y=cy1; }
  s=src+(ys*width)+xs;
  d=screen+(y*sx)+x;

  for (; yl; yl--)
  {
    for (x=0; x<xl; x++)
    {
      if (s[x])
        d[x]=color;
    }
    s+=width;
    d+=sx;
  }
  cpubyte=ETC;
}

void SilhouetteZoomClip(int x, int y, int sw, int sh, int dw, int dh, int color, byte *src)
{
  int xerr=0,yerr=0;
  int xerr_start=0, yerr_start=0;
  int xadj=0,yadj=0;
  byte *d=0;
  int xl=0,yl=0,xs=0,ys=0;

  cpubyte=RENDER;
  if (dw<1 || dh<1) return;

  xl=dw;
  yl=dh;
  xs=ys=0;
  if (x>cx2 || y>cy2 || x+xl<cx1 || y+yl<cy1)
    return;

  if (x+xl > cx2) xl=cx2-x+1;
  if (y+yl > cy2) yl=cy2-y+1;
  if (x<cx1) { xs=cx1-x; xl-=xs; x=cx1; }
  if (y<cy1) { ys=cy1-y; yl-=ys; y=cy1; }
  xadj = (sw<<16)/dw;
  yadj = (sh<<16)/dh;
  xerr_start = xadj * xs;
  yerr_start = yadj * ys;

  if (ys) src+=((yerr_start>>16)*sw);
  d = screen+(y*sx)+x;
  yerr = yerr_start & 0xffff;
  for (y=0; y<yl; y++)
  {
    xerr = xerr_start;
    for (x=0; x<xl; x++)
    {
      if (src[(xerr>>16)])
        d[x]=color;
      xerr+=xadj;
    }
    d+=sx;
    yerr += yadj;
    src += (yerr>>16)*sw;
    yerr &= 0xffff;
  }
  cpubyte=ETC;
}

void SilhouetteLucentClip(int x, int y, int width, int height, int color, byte *src)
{
  byte *s=0,*d=0;
  int xl=0,yl=0,xs=0,ys=0;
  byte *range=0;

  cpubyte=RENDER;
  xl=width;
  yl=height;
  xs=ys=0;
  if (x>cx2 || y>cy2 || x+xl<cx1 || y+yl<cy1)
    return;
  if (x+xl > cx2) xl=cx2-x+1;
  if (y+yl > cy2) yl=cy2-y+1;
  if (x<cx1) { xs=cx1-x; xl-=xs; x=cx1; }
  if (y<cy1) { ys=cy1-y; yl-=ys; y=cy1; }

  s=src+(ys*width)+xs;
  d=screen+(y*sx)+x;
  range=translucency_table+(color<<8);
  for (; yl; yl--)
  {
    for (x=0; x<xl; x++)
    {
      if (s[x])
        d[x]=range[d[x]];
    }
    s+=width;
    d+=sx;
  }
  cpubyte=ETC;
}

void SilhouetteZoomLucentClip(int x, int y, int sw, int sh, int dw, int dh, int color, byte *src)
{
  int xerr=0,yerr=0;
  int xerr_start=0, yerr_start=0;
  int xadj=0,yadj=0;
  byte *d=0;
  int xl=0,yl=0,xs=0,ys=0;
  byte *range=0;

  cpubyte=RENDER;
  if (dw<1 || dh<1) return;
  xl=dw;
  yl=dh;
  xs=ys=0;
  if (x>cx2 || y>cy2 || x+xl<cx1 || y+yl<cy1)
    return;
  if (x+xl > cx2) xl=cx2-x+1;
  if (y+yl > cy2) yl=cy2-y+1;
  if (x<cx1) { xs=cx1-x; xl-=xs; x=cx1; }
  if (y<cy1) { ys=cy1-y; yl-=ys; y=cy1; }

  xadj = (sw<<16)/dw;
  yadj = (sh<<16)/dh;
  xerr_start = xadj * xs;
  yerr_start = yadj * ys;
  if (ys) src+=((yerr_start>>16)*sw);
  d = screen+(y*sx)+x;
  range=translucency_table+(color<<8);
  yerr = yerr_start & 0xffff;
  for (y=0; y<yl; y++)
  {
    xerr = xerr_start;
    for (x=0; x<xl; x++)
    {
      if (src[(xerr>>16)])
        d[x]=range[d[x]];
      xerr+=xadj;
    }
    d+=sx;
    yerr += yadj;
    src += (yerr>>16)*sw;
    yerr &= 0xffff;
  }
  cpubyte=ETC;
}

// aen
void TintClip(int x, int y, int width, int height, int color, byte *src)
{
  byte *s=0,*d=0;
  int xl=0,yl=0,xs=0,ys=0;
  byte *range=0;
  byte c=0;

  cpubyte=RENDER;
  xl=width;
  yl=height;
  xs=ys=0;
  if (x>cx2 || y>cy2 || x+xl<cx1 || y+yl<cy1)
    return;
  if (x+xl > cx2) xl=cx2-x+1;
  if (y+yl > cy2) yl=cy2-y+1;
  if (x<cx1) { xs=cx1-x; xl-=xs; x=cx1; }
  if (y<cy1) { ys=cy1-y; yl-=ys; y=cy1; }

  s=src+(ys*width)+xs;
  d=screen+(y*sx)+x;
  range=translucency_table+(color<<8);
  for (; yl; yl--)
  {
    for (x=0; x<xl; x++)
    {
      c=s[x];
      if (c)
        d[x]=range[c];
    }
    s+=width;
    d+=sx;
  }
  cpubyte=ETC;
}

// aen
void TintZoomClip(int x, int y, int sw, int sh, int dw, int dh, int color, byte *src)
{
  int xerr=0,yerr=0;
  int xerr_start=0, yerr_start=0;
  int xadj=0,yadj=0;
  byte *d=0;
  int xl=0,yl=0,xs=0,ys=0;
  byte *range=0;
  byte c=0;

  cpubyte=RENDER;
  if (dw<1 || dh<1) return;
  xl=dw;
  yl=dh;
  xs=ys=0;
  if (x>cx2 || y>cy2 || x+xl<cx1 || y+yl<cy1)
    return;
  if (x+xl > cx2) xl=cx2-x+1;
  if (y+yl > cy2) yl=cy2-y+1;
  if (x<cx1) { xs=cx1-x; xl-=xs; x=cx1; }
  if (y<cy1) { ys=cy1-y; yl-=ys; y=cy1; }

  xadj = (sw<<16)/dw;
  yadj = (sh<<16)/dh;
  xerr_start = xadj * xs;
  yerr_start = yadj * ys;
  if (ys) src+=((yerr_start>>16)*sw);
  d = screen+(y*sx)+x;
  range=translucency_table+(color<<8);
  yerr = yerr_start & 0xffff;
  for (y=0; y<yl; y++)
  {
    xerr = xerr_start;
    for (x=0; x<xl; x++)
    {
      c=src[(xerr>>16)];
      if (c)
        d[x]=range[c];
      xerr+=xadj;
    }
    d+=sx;
    yerr += yadj;
    src += (yerr>>16)*sw;
    yerr &= 0xffff;
  }
  cpubyte=ETC;
}

void CopyTile(int x, int y, byte *src)
{
  CopySpriteClip(x,y,16,16,src);
}

void TCopyTile(int x, int y, byte *src, byte *matte)
{
  matte=matte;
  TCopySpriteClip(x,y,16,16,src);
}

void CopyTileLucent(int x, int y, byte *src)
{
  CopySpriteLucentClip(x,y,16,16,src);
}

void TCopyTileLucent(int x, int y, byte *src)
{
  TCopySpriteLucentClip(x,y,16,16,src);
}

void CopyTileClip(int x, int y, byte *src)
{
  CopySpriteClip(x,y,16,16,src);
}

void TCopyTileClip(int x, int y, byte *src, byte *matte)
{
  matte=matte;
  TCopySpriteClip(x,y,16,16,src);
}

void CopyTileLucentClip(int x, int y, byte *src)
{
  CopySpriteLucentClip(x,y,16,16,src);
}

void TCopyTileLucentClip(int x, int y, byte *src)
{
  TCopySpriteLucentClip(x,y,16,16,src);
}

// aen
void MapLine(int x, int y, int yofs, word *map)
{
  byte *d;
  int tilesx,startx;
  int counter;
  int span, s;

  cpubyte=RENDER;

  // y clipping
  if (y<0 || y>=tsy) return;
  // x clipping
  startx=0;
  if (x<0) { startx=-x; x=0; }

  d=screen+(y*sx)+x;
  tilesx=(tsx/16);

  // do clipped left tile
  counter=0;
  s=tileidx[*map];
  memcpy(d, vsp+((s<numtiles ? s : 0)*256)+(yofs*16)+startx, 16-startx);
  d+=16-startx;
  counter+=16-startx;
  map++;
  // do midsection of line
  for (x=1; x<tilesx; x++,counter+=16)
  {
    s=tileidx[*map];
    memcpy(d, vsp+((s<numtiles ? s : 0)*256)+(yofs*16), 16);

    d+=16;
    map++;
  }
  // do clipped right tile
  while (counter<tsx)
  {
    span=(tsx-counter);
    if (span>16) span=16;
    s=tileidx[*map];
    memcpy(d, vsp+((s<numtiles ? s : 0)*256)+(yofs*16), span);
    d+=span;
    counter+=span;
    map++;
  }

  cpubyte=ETC;
}

// aen
void TMapLine(int x, int y, int yofs, word *map)
{
  byte *s,*d;
  byte c;
  int tilesx,startx;
  int counter;
  int span;

  cpubyte=RENDER;

  // y clipping
  if (y<0 || y>=tsy) return;
  // x clipping
  startx=0;
  if (x<0) { startx=-x; x=0; }

  d=screen+(y*sx)+x;
  tilesx=(tsx/16);

  // do clipped left tile
  counter=0;
  s=vsp+(256*tileidx[*map])+(16*yofs)+startx;
  for (x=0; x<16-startx; x++)
  {
    c=s[x];
    if (c)
      d[x]=c;
  }
  d+=16-startx;
  counter+=16-startx;
  map++;
  // do midsection of line
  for (y=1; y<tilesx; y++,counter+=16)
  {
    s=vsp+(256*tileidx[*map])+(16*yofs);
    for (x=0; x<16; x++)
    {
      c=s[x];
      if (c)
        d[x]=c;
    }
    d+=16;
    map++;
  }
  // do clipped right tile
  while (counter<tsx)
  {
    span=(tsx-counter);
    if (span>16) span=16;
    s=vsp+(256*tileidx[*map])+(16*yofs);
    for (x=0; x<span; x++)
    {
      c=s[x];
      if (c)
        d[x]=c;
    }
    d+=span;
    counter+=span;
    map++;
  }

  cpubyte=ETC;
}

void ClearScreen()
{
  cpubyte=RENDER;
  memset(screen, 0, sx*sy);
  cpubyte=ETC;
}

void SetPixel(int x, int y, int color)
{
  screen[(y*sx)+x]=color;
}

void SetPixelLucent(int x, int y, int color)
{
  byte *d;

  cpubyte=RENDER;
  d=screen+(y*sx)+x;
  *d=translucency_table[*d|(color<<8)];
  cpubyte=ETC;
}

void SetPixelClip(int x, int y, int color)
{
  if (x<cx1 || y<cy1 || x>cx2 || y>cy2) return;
  screen[(y*sx)+x]=color;
}

void SetPixelLucentClip(int x, int y, int color)
{
  byte *d;

  if (x<cx1 || y<cy1 || x>cx2 || y>cy2) return;
  d=screen+(y*sx)+x;
  *d=translucency_table[*d|(color<<8)];
}

int GetPixel(int x, int y)
{
  return screen[(y*sx)+x];
}

int GetPixelClip(int x, int y)
{
  if (x<cx1 || y<cy1 || x>cx2 || y>cy2) return 0;
  return screen[(y*sx)+x];
}

void ColorField(int x, int y, char c)
{
  int i;

  cpubyte=RENDER;
  i=0;
  do
  {
    SetPixelClip(x+0, y+i, 0);
    SetPixelClip(x+2, y+i, 0);
    SetPixelClip(x+4, y+i, 0);
    SetPixelClip(x+6, y+i, 0);
    SetPixelClip(x+8, y+i, 0);
    SetPixelClip(x+10, y+i, 0);
    SetPixelClip(x+12, y+i, 0);
    SetPixelClip(x+14, y+i, 0);
    i++;

    SetPixelClip(x+0 +1, y+i, 0);
    SetPixelClip(x+2 +1, y+i, 0);
    SetPixelClip(x+4 +1, y+i, 0);
    SetPixelClip(x+6 +1, y+i, 0);
    SetPixelClip(x+8 +1, y+i, 0);
    SetPixelClip(x+10 +1, y+i, 0);
    SetPixelClip(x+12 +1, y+i, 0);
    SetPixelClip(x+14 +1, y+i, 0);
    i++;
  } while (i<16);
  cpubyte=ETC;
}

void HLine(int x, int y, int x2, int color)
{
  cpubyte=RENDER;
  if (x2<x) SWAP(x,x2);
  memset(screen+(y*sx)+x,color,x2-x+1);
  cpubyte=ETC;
}

void HLineClip(int x, int y, int x2, int color)
{
  int width;

  cpubyte=RENDER;
  if (x2<x) SWAP(x,x2);
  width=x2-x+1;
  if (x>cx2 || y>cy2 || x+width<cx1 || y<cy1)
    return;
  if (x+width > cx2) width=cx2-x+1;
  if (x<cx1) { width-=(cx1-x); x=cx1; }
  memset(screen+(y*sx)+x,color,width);
  cpubyte=ETC;
}

void HLineLucent(int x, int y, int x2, int color)
{
  byte *d;
  int c;

  cpubyte=RENDER;
  if (x2<x) SWAP(x,x2);
  c=color<<8;
  d=screen+(y*sx)+x;
  for (; x<=x2; x++,d++)
    *d=translucency_table[c|*d];
  cpubyte=ETC;
}

void HLineLucentClip(int x, int y, int x2, int color)
{
  byte *d;
  int c,width;

  cpubyte=RENDER;
  if (x2<x) SWAP(x,x2);
  width=x2-x+1;
  if (x>cx2 || y>cy2 || x+width<cx1 || y<cy1)
    return;

  if (x+width > cx2) width=cx2-x+1;
  if (x<cx1) { width-=(cx1-x); x=cx1; }
  c=color<<8;
  d=screen+(y*sx)+x;
  for (x=0; x<width; x++,d++)
    *d=translucency_table[c|*d];
  cpubyte=ETC;
}

void VLine(int x, int y, int y2, int color)
{
  byte *d;

  cpubyte=RENDER;
  if (y2<y) SWAP(y,y2);
  d=screen+(y*sx)+x;
  for (; y<=y2; y++)
  {
    *d=color;
    d+=sx;
  }
  cpubyte=ETC;
}

void VLineClip(int x, int y, int y2, int color)
{
  byte *d;
  int height;

  cpubyte=RENDER;
  if (y2<y) SWAP(y,y2);
  height=y2-y+1;
  if (x>cx2 || y>cy2 || x<cx1 || y+height<cy1)
  {
    cpubyte=ETC;
    return;
  }

  if (y+height > cy2) height=cy2-y+1;
  if (y<cy1) { height-=(cy1-y); y=cy1; }
  d=screen+(y*sx)+x;
  for (; height; height--)
  {
    *d=color;
    d+=sx;
  }
  cpubyte=ETC;
}

void VLineLucent(int x, int y, int y2, int color)
{
  byte *d;
  int c;

  cpubyte=RENDER;
  if (y2<y) SWAP(y,y2);

  c=color<<8;
  d=screen+(y*sx)+x;
  for (; y<=y2; y++)
  {
    *d=translucency_table[c|*d];
    d+=sx;
  }
  cpubyte=ETC;
}

void VLineLucentClip(int x, int y, int y2, int color)
{
  byte *d;
  int c,height;

  cpubyte=RENDER;
  if (y2<y) SWAP(y,y2);
  height=y2-y+1;
  if (x>cx2 || y>cy2 || x<cx1 || y+height<cy1)
  {
    cpubyte=ETC;
    return;
  }
  if (y+height > cy2) height=cy2-y+1;
  if (y<cy1) { height-=(cy1-y); y=cy1; }
  c=color<<8;
  d=screen+(y*sx)+x;
  for (; height; height--)
  {
    *d=translucency_table[c|*d];
    d+=sx;
  }
  cpubyte=ETC;
}

// all the Line* routine were grabbed/modded from the Abuse source

void Line(int x1, int y1, int x2, int y2, int color)
{
  short i,xc,yc,er,n,m,xi,yi,xcxi,ycyi,xcyi;
  unsigned dcy,dcx;

  cpubyte=RENDER;
  if (x1>x2)
  {
    SWAP(x1,x2);
    SWAP(y1,y2);
  }

  if (y1>y2)
  {
    SWAP(x1,x2);
    SWAP(y1,y2);
  }
  // assume y1<=y2 from above swap operation
  yi=y2; yc=y1;

  dcx=x1; dcy=y1;
  xc=(x2-x1); yc=(y2-y1);
  if (xc<0) xi=-1; else xi=1;
  if (yc<0) yi=-1; else yi=1;
  n=abs(xc); m=abs(yc);
  ycyi=abs(2*yc*xi);
  er=0;

  if (n>m)
  {
    xcxi=abs(2*xc*xi);
    for (i=0;i<=n;i++)
    {
      screen[(dcy*sx)+dcx]=color;
      if (er>0)
      { dcy+=yi;
        er-=xcxi;
      }
      er+=ycyi;
      dcx+=xi;
    }
  }
  else
  {
    xcyi=abs(2*xc*yi);
    for (i=0;i<=m;i++)
    {
      screen[(dcy*sx)+dcx]=color;
      if (er>0)
      { dcx+=xi;
        er-=ycyi;
      }
      er+=xcyi;
      dcy+=yi;
    }
  }
  cpubyte=ETC;
}

// grabbed/modded from Abuse source
void LineClip(int x1, int y1, int x2, int y2, int color)
{
  short i,xc,yc,er,n,m,xi,yi,xcxi,ycyi,xcyi;
  unsigned dcy,dcx;

  cpubyte=RENDER;
  // check to see if the line is completly clipped off
  if ((x1<cx1 && x2<cx1) || (x1>cx2 && x2>cx2)
  || (y1<cy1 && y2<cy1) || (y1>cy2 && y2>cy2))
  {
    cpubyte=ETC;
    return;
  }

  if (x1>x2)
  {
    SWAP(x1,x2);
    SWAP(y1,y2);
  }

  // clip the left side
  if (x1<cx1)
  { int myy=(y2-y1);
    int mxx=(x2-x1),b;
    if (!mxx)
    {
      cpubyte=ETC;
      return;
    }
    if (myy)
    {
      b=y1-(y2-y1)*x1/mxx;
      y1=myy*cx1/mxx+b;
      x1=cx1;
    }
    else x1=cx1;
  }

  // clip the right side
  if (x2>cx2)
  { int myy=(y2-y1);
    int mxx=(x2-x1),b;
    if (!mxx)
    {
      cpubyte=ETC;
      return;
    }
    if (myy)
    {
      b=y1-(y2-y1)*x1/mxx;
      y2=myy*cx2/mxx+b;
      x2=cx2;
    }
    else x2=cx2;
  }

  if (y1>y2)
  {
    SWAP(x1,x2);
    SWAP(y1,y2);
  }

  // clip the bottom
  if (y2>cy2)
  { int mxx=(x2-x1);
    int myy=(y2-y1),b;
    if (!myy)
    {
      cpubyte=ETC;
      return;
    }
    if (mxx)
    {
      b=y1-(y2-y1)*x1/mxx;
      x2=(cy2-b)*mxx/myy;
      y2=cy2;
    }
    else y2=cy2;
  }

  // clip the top
  if (y1<cy1)
  { int mxx=(x2-x1);
    int myy=(y2-y1),b;
    if (!myy)
    {
      cpubyte=ETC;
      return;
    }
    if (mxx)
    {
      b=y1-(y2-y1)*x1/mxx;
      x1=(cy1-b)*mxx/myy;
      y1=cy1;
    }
    else y1=cy1;
  }

  // ???
  // see if it got cliped into the box, out out
  if (x1<cx1 || x2<cx1 || x1>cx2 || x2>cx2 || y1<cy1 || y2 <cy1 || y1>cy2 || y2>cy2)
  {
    cpubyte=ETC;
    return;
  }

  if (x1>x2)
  { xc=x2; xi=x1; }
  else { xi=x2; xc=x1; }

  // assume y1<=y2 from above swap operation
  yi=y2; yc=y1;

  dcx=x1; dcy=y1;
  xc=(x2-x1); yc=(y2-y1);
  if (xc<0) xi=-1; else xi=1;
  if (yc<0) yi=-1; else yi=1;
  n=abs(xc); m=abs(yc);
  ycyi=abs(2*yc*xi);
  er=0;

  if (n>m)
  {
    xcxi=abs(2*xc*xi);
    for (i=0;i<=n;i++)
    {
      screen[(dcy*sx)+dcx]=color;
      if (er>0)
      { dcy+=yi;
        er-=xcxi;
      }
      er+=ycyi;
      dcx+=xi;
    }
  }
  else
  {
    xcyi=abs(2*xc*yi);
    for (i=0;i<=m;i++)
    {
      screen[(dcy*sx)+dcx]=color;
      if (er>0)
      { dcx+=xi;
        er-=ycyi;
      }
      er+=xcyi;
      dcy+=yi;
    }
  }
  cpubyte=ETC;
  return;
}

void LineLucent(int x1, int y1, int x2, int y2, int color)
{
  short i,xc,yc,er,n,m,xi,yi,xcxi,ycyi,xcyi;
  unsigned dcy,dcx;
  byte *d;
  int c;

  cpubyte=RENDER;
  if (x1>x2)
  {
    SWAP(x1,x2);
    SWAP(y1,y2);
  }
  if (y1>y2)
  {
    SWAP(x1,x2);
    SWAP(y1,y2);
  }

  // assume y1<=y2 from above swap operation
  yi=y2; yc=y1;
  dcx=x1; dcy=y1;
  xc=(x2-x1); yc=(y2-y1);
  if (xc<0) xi=-1; else xi=1;
  if (yc<0) yi=-1; else yi=1;
  n=abs(xc); m=abs(yc);
  ycyi=abs(2*yc*xi);
  er=0;

  c=color<<8; // for translucency
  if (n>m)
  {
    xcxi=abs(2*xc*xi);
    for (i=0;i<=n;i++)
    {
      d=screen+(dcy*sx)+dcx;
      *d=translucency_table[c|*d];
      //screen[(dcy*sx)+dcx]=color;
      if (er>0)
      { dcy+=yi;
        er-=xcxi;
      }
      er+=ycyi;
      dcx+=xi;
    }
  }
  else
  {
    xcyi=abs(2*xc*yi);
    for (i=0;i<=m;i++)
    {
      d=screen+(dcy*sx)+dcx;
      *d=translucency_table[c|*d];
      //screen[(dcy*sx)+dcx]=color;
      if (er>0)
      { dcx+=xi;
        er-=ycyi;
      }
      er+=xcyi;
      dcy+=yi;
    }
  }
  cpubyte=ETC;
}

void LineLucentClip(int x1, int y1, int x2, int y2, int color)
{
  short i,xc,yc,er,n,m,xi,yi,xcxi,ycyi,xcyi;
  unsigned dcy,dcx;
  byte *d;
  int c;

  cpubyte=RENDER;
  // check to see if the line is completly clipped off
  if ((x1<cx1 && x2<cx1) || (x1>cx2 && x2>cx2)
  || (y1<cy1 && y2<cy1) || (y1>cy2 && y2>cy2))
  {
    cpubyte=ETC;
    return;
  }

  if (x1>x2)
  {
    SWAP(x1,x2);
    SWAP(y1,y2);
  }

  // clip the left side
  if (x1<cx1)
  { int myy=(y2-y1);
    int mxx=(x2-x1),b;
    if (!mxx)
    {
      cpubyte=ETC;
      return;
    }
    if (myy)
    {
      b=y1-(y2-y1)*x1/mxx;
      y1=myy*cx1/mxx+b;
      x1=cx1;
    }
    else x1=cx1;
  }

  // clip the right side
  if (x2>cx2)
  { int myy=(y2-y1);
    int mxx=(x2-x1),b;
    if (!mxx)
    {
      cpubyte=ETC;
      return;
    }
    if (myy)
    {
      b=y1-(y2-y1)*x1/mxx;
      y2=myy*cx2/mxx+b;
      x2=cx2;
    }
    else x2=cx2;
  }

  if (y1>y2)
  {
    SWAP(x1,x2);
    SWAP(y1,y2);
  }

  // clip the bottom
  if (y2>cy2)
  { int mxx=(x2-x1);
    int myy=(y2-y1),b;
    if (!myy)
    {
      cpubyte=ETC;
      return;
    }
    if (mxx)
    {
      b=y1-(y2-y1)*x1/mxx;
      x2=(cy2-b)*mxx/myy;
      y2=cy2;
    }
    else y2=cy2;
  }

  // clip the top
  if (y1<cy1)
  { int mxx=(x2-x1);
    int myy=(y2-y1),b;
    if (!myy)
    {
      cpubyte=ETC;
      return;
    }
    if (mxx)
    {
      b=y1-(y2-y1)*x1/mxx;
      x1=(cy1-b)*mxx/myy;
      y1=cy1;
    }
    else y1=cy1;
  }

  // ???
  // see if it got cliped into the box, out out
  if (x1<cx1 || x2<cx1 || x1>cx2 || x2>cx2 || y1<cy1 || y2 <cy1 || y1>cy2 || y2>cy2)
  {
    cpubyte=ETC;
    return;
  }

  if (x1>x2)
  { xc=x2; xi=x1; }
  else { xi=x2; xc=x1; }

  // assume y1<=y2 from above swap operation
  yi=y2; yc=y1;

  dcx=x1; dcy=y1;
  xc=(x2-x1); yc=(y2-y1);
  if (xc<0) xi=-1; else xi=1;
  if (yc<0) yi=-1; else yi=1;
  n=abs(xc); m=abs(yc);
  ycyi=abs(2*yc*xi);
  er=0;

  c=color<<8; // for translucency
  if (n>m)
  {
    xcxi=abs(2*xc*xi);
    for (i=0;i<=n;i++)
    {
      d=screen+(dcy*sx)+dcx;
      *d=translucency_table[c|*d];
      //screen[(dcy*sx)+dcx]=color;
      if (er>0)
      { dcy+=yi;
        er-=xcxi;
      }
      er+=ycyi;
      dcx+=xi;
    }
  }
  else
  {
    xcyi=abs(2*xc*yi);
    for (i=0;i<=m;i++)
    {
      d=screen+(dcy*sx)+dcx;
      *d=translucency_table[c|*d];
      //screen[(dcy*sx)+dcx]=color;
      if (er>0)
      { dcx+=xi;
        er-=ycyi;
      }
      er+=xcyi;
      dcy+=yi;
    }
  }
  cpubyte=ETC;
}

// All the Circle* routines were grabbed/modded from Allegro source

void Circle(int x, int y, int radius, int color)
{
  int cx=0;
  int cy=radius;
  int df=1-radius;
  int d_e=3;
  int d_se=-2*radius+5;

  cpubyte=RENDER;
  do
  {
    SetPixel(x+cx,y+cy,color);
    if (cx) SetPixel(x-cx,y+cy,color);
    if (cy) SetPixel(x+cx,y-cy,color);
    if ((cx) && (cy)) SetPixel(x-cx,y-cy,color);

    if (cx != cy)
    {
      SetPixel(x+cy,y+cx,color);
      if (cx) SetPixel(x+cy,y-cx,color);
      if (cy) SetPixel(x-cy,y+cx,color);
      if (cx && cy) SetPixel(x-cy,y-cx,color);
    }

    if (df<0)
    {
      df+=d_e;
      d_e+=2;
      d_se+=2;
    }
    else
    {
      df+=d_se;
      d_e+=2;
      d_se+=4;
      cy--;
    }
    cx++;
  } while (cx <= cy);
  cpubyte=ETC;
}

void CircleClip(int x, int y, int radius, int color)
{
  int cx=0;
  int cy=radius;
  int df=1-radius;
  int d_e=3;
  int d_se=-2*radius+5;

  cpubyte=RENDER;
  do {
    SetPixelClip(x+cx,y+cy,color);
    if (cx) SetPixelClip(x-cx,y+cy,color);
    if (cy) SetPixelClip(x+cx,y-cy,color);
    if ((cx) && (cy)) SetPixelClip(x-cx,y-cy,color);

    if (cx != cy)
    {
      SetPixelClip(x+cy,y+cx,color);
      if (cx) SetPixelClip(x+cy,y-cx,color);
      if (cy) SetPixelClip(x-cy,y+cx,color);
      if (cx && cy) SetPixelClip(x-cy,y-cx,color);
    }

    if (df<0)
    {
      df+=d_e;
      d_e+=2;
      d_se+=2;
    }
    else
    {
      df+=d_se;
      d_e+=2;
      d_se+=4;
      cy--;
    }
    cx++;
  } while (cx <= cy);
  cpubyte=ETC;
}

void CircleLucent(int x, int y, int radius, int color)
{
  int cx=0;
  int cy=radius;
  int df=1-radius;
  int d_e=3;
  int d_se=-2*radius+5;

  cpubyte=RENDER;

  do
  {
    SetPixelLucent(x+cx,y+cy,color);
    if (cx) SetPixelLucent(x-cx,y+cy,color);
    if (cy) SetPixelLucent(x+cx,y-cy,color);
    if ((cx) && (cy)) SetPixelLucent(x-cx,y-cy,color);

    if (cx != cy)
    {
      SetPixelLucent(x+cy,y+cx,color);
      if (cx) SetPixelLucent(x+cy,y-cx,color);
      if (cy) SetPixelLucent(x-cy,y+cx,color);
      if (cx && cy) SetPixelLucent(x-cy,y-cx,color);
    }

    if (df<0)
    {
      df+=d_e;
      d_e+=2;
      d_se+=2;
    }
    else
    {
      df+=d_se;
      d_e+=2;
      d_se+=4;
      cy--;
    }
    cx++;
  } while (cx <= cy);
  cpubyte=ETC;
}

void CircleLucentClip(int x, int y, int radius, int color)
{
  int cx=0;
  int cy=radius;
  int df=1-radius;
  int d_e=3;
  int d_se=-2*radius+5;

  cpubyte=RENDER;
  do
  {
    SetPixelLucentClip(x+cx,y+cy,color);
    if (cx) SetPixelLucentClip(x-cx,y+cy,color);
    if (cy) SetPixelLucentClip(x+cx,y-cy,color);
    if ((cx) && (cy)) SetPixelLucentClip(x-cx,y-cy,color);

    if (cx != cy)
    {
      SetPixelLucentClip(x+cy,y+cx,color);
      if (cx) SetPixelLucentClip(x+cy,y-cx,color);
      if (cy) SetPixelLucentClip(x-cy,y+cx,color);
      if (cx && cy) SetPixelLucentClip(x-cy,y-cx,color);
    }

    if (df<0)
    {
      df+=d_e;
      d_e+=2;
      d_se+=2;
    }
    else
    {
      df+=d_se;
      d_e+=2;
      d_se+=4;
      cy--;
    }
    cx++;
  } while (cx <= cy);
  cpubyte=ETC;
}

void CircleFill(int x, int y, int radius, int color)
{
  int cx=0;
  int cy=radius;
  int df=1-radius;
  int d_e=3;
  int d_se=-2*radius+5;

  cpubyte=RENDER;

  do
  {
    HLine(x-cy,y-cx,x+cy,color);
    if (cx) HLine(x-cy,y+cx,x+cy,color);

    if (df<0)
    {
      df+=d_e;
      d_e+=2;
      d_se+=2;
    }
    else
    {
      if (cx != cy)
      {
        HLine(x-cx,y-cy,x+cx,color);
        if (cy) HLine(x-cx,y+cy,x+cx,color);
      }
      df+=d_se;
      d_e+=2;
      d_se+=4;
      cy--;
    }
    cx++;
  } while (cx <= cy);
  cpubyte=ETC;
}

void CircleFillClip(int x, int y, int radius, int color)
{
  int cx=0;
  int cy=radius;
  int df=1-radius;
  int d_e=3;
  int d_se=-2*radius+5;

  cpubyte=RENDER;
  do
  {
    HLineClip(x-cy,y-cx,x+cy,color);
    if (cx) HLineClip(x-cy,y+cx,x+cy,color);

    if (df<0)
    {
      df+=d_e;
      d_e+=2;
      d_se+=2;
    }
    else
    {
      if (cx != cy)
      {
        HLineClip(x-cx,y-cy,x+cx,color);
        if (cy) HLineClip(x-cx,y+cy,x+cx,color);
      }
      df+=d_se;
      d_e+=2;
      d_se+=4;
      cy--;
    }
    cx++;
  } while (cx <= cy);
  cpubyte=ETC;
}

void CircleFillLucent(int x, int y, int radius, int color)
{
  int cx=0;
  int cy=radius;
  int df=1-radius;
  int d_e=3;
  int d_se=-2*radius+5;

  cpubyte=RENDER;
  do
  {
    HLineLucent(x-cy,y-cx,x+cy,color);
    if (cx) HLineLucent(x-cy,y+cx,x+cy,color);

    if (df<0)
    {
      df+=d_e;
      d_e+=2;
      d_se+=2;
    }
    else
    {
      if (cx != cy)
      {
        HLineLucent(x-cx,y-cy,x+cx,color);
        if (cy) HLineLucent(x-cx,y+cy,x+cx,color);
      }
      df+=d_se;
      d_e+=2;
      d_se+=4;
      cy--;
    }
    cx++;
  } while (cx <= cy);
  cpubyte=ETC;
}

void CircleFillLucentClip(int x, int y, int radius, int color)
{
  int cx=0;
  int cy=radius;
  int df=1-radius;
  int d_e=3;
  int d_se=-2*radius+5;

  cpubyte=RENDER;
  do
  {
    HLineLucentClip(x-cy,y-cx,x+cy,color);
    if (cx) HLineLucentClip(x-cy,y+cx,x+cy,color);

    if (df<0)
    {
      df+=d_e;
      d_e+=2;
      d_se+=2;
    }
    else
    {
      if (cx != cy)
      {
        HLineLucentClip(x-cx,y-cy,x+cx,color);
        if (cy) HLineLucentClip(x-cx,y+cy,x+cx,color);
      }
      df+=d_se;
      d_e+=2;
      d_se+=4;
      cy--;
    }
    cx++;
  } while (cx <= cy);
  cpubyte=ETC;
}

void Rect(int x, int y, int x2, int y2, int color)
{
  HLine(x,y,x2,color);
  HLine(x,y2,x2,color);
  VLine(x,y+1,y2-1,color);
  VLine(x2,y+1,y2-1,color);
}

void RectClip(int x, int y, int x2, int y2, int color)
{
  HLineClip(x,y,x2,color);
  HLineClip(x,y2,x2,color);
  VLineClip(x,y+1,y2-1,color);
  VLineClip(x2,y+1,y2-1,color);
}

void RectLucent(int x, int y, int x2, int y2, int color)
{
  HLineLucent(x,y,x2,color);
  HLineLucent(x,y2,x2,color);
  VLineLucent(x,y+1,y2-1,color);
  VLineLucent(x2,y+1,y2-1,color);
}

void RectLucentClip(int x, int y, int x2, int y2, int color)
{
  HLineLucentClip(x,y,x2,color);
  HLineLucentClip(x,y2,x2,color);
  VLineLucentClip(x,y+1,y2-1,color);
  VLineLucentClip(x2,y+1,y2-1,color);
}

void RectFill(int x, int y, int x2, int y2, int color)
{
  cpubyte=RENDER;
  if (y2<y) SWAP(y,y2);
  for (; y<=y2; y++)
    HLine(x,y,x2,color);
  cpubyte=ETC;
}

void RectFillClip(int x, int y, int x2, int y2, int color)
{
  cpubyte=RENDER;
  if (y2<y) SWAP(y,y2);
  for (; y<=y2; y++)
    HLineClip(x,y,x2,color);
  cpubyte=ETC;
}

void RectFillLucent(int x, int y, int x2, int y2, int color)
{
  cpubyte=RENDER;
  if (y2<y) SWAP(y,y2);
  for (; y<=y2; y++)
    HLineLucent(x,y,x2,color);
  cpubyte=ETC;
}

void RectFillLucentClip(int x, int y, int x2, int y2, int color)
{
  cpubyte=RENDER;
  if (y2<y) SWAP(y,y2);
  for (; y<=y2; y++)
    HLineLucentClip(x,y,x2,color);
  cpubyte=ETC;
}

// ============================================================================
// =                          aen's wrap blitters                             =
// ============================================================================

void WrapBlit(quad x, quad y, int wide, int high, byte *src)
{
  int i;
  int cliph,clipw;
  int curx,cury;
  int spanx,spany;
  byte *source,*dest;

  cpubyte=RENDER;
  cliph=cy2-cy1+1;
  clipw=cx2-cx1+1;
  y%=high;
  curx=0;

  do
  {
    x%=curx?1:wide;
    spanx=wide-x;
    if (curx+spanx>=clipw)
      spanx=clipw-curx;
    source=src+(y*wide)+x;
    dest=screen+(cy1*sx)+cx1+curx;
    cury=0;

    do
    {
      spany=high-(cury?0:y);
      if (cury+spany>=cliph)
        spany=cliph-cury;

      for (i=0; i<spany; i++,source+=wide,dest+=sx)
        memcpy(dest, source, spanx);

      source=src+x;
      cury+=spany;
    } while (cury<cliph);
    curx+=spanx;
  } while (curx<clipw);
  cpubyte=ETC;
}

void WrapBlitMasked(quad x, quad y, int wide, int high, byte *src)
{
  int i,j,c;
  int cliph,clipw;
  int curx,cury;
  int spanx,spany;
  byte *source,*dest;

  cpubyte=RENDER;
  cliph=cy2-cy1+1;
  clipw=cx2-cx1+1;
  y%=high;
  curx=0;

  do
  {
    x%=curx?1:wide;
    spanx=wide-x;
    if (curx+spanx>=clipw)
      spanx=clipw-curx;
    source=src+(y*wide)+x;
    dest=screen+(cy1*sx)+cx1+curx;
    cury=0;

    do
    {
      spany=high-(cury?0:y);
      if (cury+spany>=cliph)
        spany=cliph-cury;
      for (i=0; i<spany; i++,source+=wide,dest+=sx)
        for (j=0; j<spanx; j++)
        {
          c=source[j];
          if (c)
            dest[j]=c;
        }
      source=src+x;
      cury+=spany;
    } while (cury<cliph);
    curx+=spanx;
  } while (curx<clipw);
  cpubyte=ETC;
}

void WrapBlitLucent(quad x, quad y, int wide, int high, byte *src)
{
  int i,j;
  int cliph,clipw;
  int curx,cury;
  int spanx,spany;
  byte *source,*dest;

  cpubyte=RENDER;
  cliph=cy2-cy1+1;
  clipw=cx2-cx1+1;
  y%=high;
  curx=0;

  do
  {
    x%=curx?1:wide;
    spanx=wide-x;
    if (curx+spanx>=clipw)
      spanx=clipw-curx;
    source=src+(y*wide)+x;
    dest=screen+(cy1*sx)+cx1+curx;
    cury=0;

    do
    {
      spany=high-(cury?0:y);
      if (cury+spany>=cliph)
        spany=cliph-cury;
      for (i=0; i<spany; i++,source+=wide,dest+=sx)
        for (j=0; j<spanx; j++)
          dest[j]=translucency_table[(source[j]<<8)|dest[j]];
      source=src+x;
      cury+=spany;
    } while (cury<cliph);
    curx+=spanx;
  } while (curx<clipw);
  cpubyte=ETC;
}

void WrapBlitLucentMasked(quad x, quad y, int wide, int high, byte *src)
{
  int i,j,c;
  int cliph,clipw;
  int curx,cury;
  int spanx,spany;
  byte *source,*dest;

  cpubyte=RENDER;
  cliph=cy2-cy1+1;
  clipw=cx2-cx1+1;
  y%=high;
  curx=0;

  do
  {
    x%=curx?1:wide;
    spanx=wide-x;
    if (curx+spanx>=clipw)
      spanx=clipw-curx;
    source=src+(y*wide)+x;
    dest=screen+(cy1*sx)+cx1+curx;
    cury=0;

    do
    {
      spany=high-(cury?0:y);
      if (cury+spany>=cliph)
        spany=cliph-cury;
      for (i=0; i<spany; i++,source+=wide,dest+=sx)
        for (j=0; j<spanx; j++)
        {
          c=source[j];
          if (c)
            dest[j]=translucency_table[(c<<8)|dest[j]];
        }
      source=src+x;
      cury+=spany;
    } while (cury<cliph);
    curx+=spanx;
  } while (curx<clipw);
  cpubyte=ETC;
}

// ============================================================================
// =           alias.zip / zero / aen rotational scaler routines              =
// ============================================================================

void RotScale(int posx, int posy, quad width, quad height, float angle, float scale, byte *src)
{
  // new! shamelessly ripped off from alias.zip
  // except the atan2 stuff which i had to make up myself AEN so there :p

  int xs,ys,xl,yl;
  int sinas,cosas,xc,yc,srcx,srcy,x,y,tempx,tempy,T_WIDTH_CENTER,T_HEIGHT_CENTER,W_WIDTH_CENTER,W_HEIGHT_CENTER,W_HEIGHT,W_WIDTH;
  byte *dest,pt;
  float ft;

  ft=atan2((float)width,(float)height);

  T_WIDTH_CENTER=width>>1;
  T_HEIGHT_CENTER=height>>1;
  W_WIDTH=((float)width/scale*sin(ft) + (float)height/scale*cos(ft));
  W_HEIGHT=W_WIDTH;
  W_HEIGHT_CENTER=W_HEIGHT>>1;
  W_WIDTH_CENTER=W_HEIGHT_CENTER; //W_WIDTH/2;

  sinas=sin(-angle)*65536*scale;
  cosas=cos(-angle)*65536*scale;

  xc=T_WIDTH_CENTER*65536 - (W_HEIGHT_CENTER*(cosas+sinas));
  yc=T_HEIGHT_CENTER*65536 - (W_WIDTH_CENTER*(cosas-sinas));
  posx-=W_WIDTH_CENTER;
  posy-=W_HEIGHT_CENTER;

  // clipping
  if (W_WIDTH<2 || W_HEIGHT<2) return;
  xl=W_WIDTH;
  yl=W_HEIGHT;
  xs=ys=0;
  if (posx>cx2 || posy>cy2 || posx+xl<cx1 || posy+yl<cy1)
    return;
  if (posx+xl > cx2) xl=cx2-posx+1;
  if (posy+yl > cy2) yl=cy2-posy+1;
  if (posx<cx1)
  {
    xs=cx1-posx;
    xl-=xs;
    posx=cx1;

    xc+=cosas*xs; // woo!
    yc-=sinas*xs;
  }
  if (posy<cy1)
  {
    ys=cy1-posy;
    yl-=ys;
    posy=cy1;

    xc+=sinas*ys; // woo!
    yc+=cosas*ys;
  }

  dest=screen+posx+posy*sx;
  for (y=0; y<yl; y++)
  {
    srcx=xc;
    srcy=yc;

    for (x=0; x<xl; x++)
    {
      tempx=(srcx>>16);
      tempy=(srcy>>16);

      if (tempx>=0 && tempx<width && tempy>=0 && tempy<height)
      {
        if ((pt=src[tempx+tempy*width]))
          dest[x]=pt;
      }

      srcx+=cosas;
      srcy-=sinas;
    }

    dest+=sx;

    xc+=sinas;
    yc+=cosas;
  }
}

// ============================================================================
// =                        zero's Mosiac effects                             =
// ============================================================================

//mosaic routines
//TODO-need to asm optimize these badly! they are rather slow!

byte FindPalMatchForMosaic(quad r, quad g, quad b)
{
  quad index;
  quad diffrecord,diff,record;
  diffrecord=100000;
  record=100000;

  for(index=0; index<256; index++)
  {
    diff=abs(r-pal[index*3])+abs(g-pal[index*3+1])+abs(b-pal[index*3+2])+1;
    if(diff<diffrecord)
    {
      diffrecord=diff;
      if(diff==1) return index;
      record=index;
    }
  }
  return record;
}

byte *InitMosaicTable(void)
{
  quad r,g,b;
  byte *tmp;
  tmp=(byte *)malloc(262144);

  for(r=0; r<64; r++)
    for(g=0; g<64; g++)
      for(b=0; b<64; b++)
        tmp[r*4096+g*64+b]=FindPalMatchForMosaic(r,g,b);
  return tmp;
}

void Mosaic(quad xlevel, quad ylevel, byte *tbl, quad xmin, quad ymin, quad xmax, quad ymax)
{
  quad x,y,rtot,btot,gtot,xloop,yloop,xfier,txlevel;
  byte *src;
  byte pixel;

  for(y=ymin; y<ymax; y+=ylevel)
  {
    for(x=xmin; x<xmax; x+=xlevel)
    {
      rtot=0; gtot=0; btot=0;
      src=y*sx+x+screen;
      if(ymax-y<ylevel) ylevel=ymax-y;
      if(xmax-x<xlevel) txlevel=xmax-x; else txlevel=xlevel;
      xfier=ylevel*txlevel;
      for(yloop=0; yloop<ylevel; yloop++)
      {
        for(xloop=0; xloop<txlevel; xloop++)
        {
          rtot+=pal[*src*3];
          gtot+=pal[*src*3+1];
          btot+=pal[*src*3+2];
          src++;
        }
        src+=sx-txlevel;
      }
      rtot/=xfier;
      gtot/=xfier;
      btot/=xfier;
      pixel=tbl[rtot*4096+gtot*64+btot];
      src=y*sx+x+screen;
      for(yloop=0; yloop<ylevel; yloop++)
      {
        for(xloop=0; xloop<txlevel; xloop++)
        {
          *src=pixel;
          src++;
        }
        src+=sx-txlevel;
      }
    }
  }
}

// ============================================================================
// =                           Mord's trifillers                              =
// ============================================================================

void FlatPoly(int x1, int y1, int x2, int y2, int x3, int y3, char color)
{
  int xstep,xstep2;
  int xval,xval2;
  int yon;
  int swaptemp;

  if (y1 > y3)
  {
    swaptemp=x1; x1=x3; x3=swaptemp;
    swaptemp=y1; y1=y3; y3=swaptemp;
  }
  if (y2 > y3)
  {
    swaptemp=x2; x2=x3; x3=swaptemp;
    swaptemp=y2; y2=y3; y3=swaptemp;
  }
  if (y1 > y2)
  {
    swaptemp=x1; x1=x2; x2=swaptemp;
    swaptemp=y1; y1=y2; y2=swaptemp;
  }

  xstep2=((x3-x1) << 16) / (y3-y1);
  xval2=x1 << 16;

  if (y1 != y2)
  {
    xstep = ((x2-x1) << 16) / (y2-y1);
    xval = x1 << 16;
    for (yon=y1;yon < y2; yon++)
    {
      if ((yon > -1) && (yon < 200))
      {
        HLineClip(xval>>16,yon,xval2>>16,color);
      }
      xval+=xstep;
      xval2+=xstep2;
    }
  }
  if (y2 != y3)
  {
    xstep = ((x3-x2) << 16) / (y3-y2);
    xval = x2 << 16;
    for (yon=y2;yon < y3; yon++)
    {
      if ((yon > -1) && (yon < 200))
      {
        HLineClip(xval>>16,yon,xval2>>16,color);
      }
      xval+=xstep;
      xval2+=xstep2;
    }
  }
}

char *image;
int texw,texh;  // those damn bastards want EVERYTHING!

void tmaphline(int x1, int x2, int y, int tx1, int tx2, int ty1, int ty2)
{
  int i;
  int txstep,txval;
  int tystep,tyval;

  if (x1 != x2)
  {
    if (x2 < x1)
    {
      i=x1; x1=x2; x2=i;
      i=tx1; tx1=tx2; tx2=i;
      i=ty1; ty1=ty2; ty2=i;
    }
    if ((x1 > sx) || (x2 < 0)) return;
    txstep=((tx2-tx1)<<16)/(x2-x1);
    tystep=((ty2-ty1)<<16)/(x2-x1);
    txval=tx1<<16;
    tyval=ty1<<16;

    for (i=x1;i<x2;i++)
    {
      screen[y*sx+i] = image[(tyval>>16)*texw+(txval>>16)];
      txval+=txstep;
      tyval+=tystep;
    }
  }
}

void TMapPoly(int x1, int y1, int x2, int y2, int x3, int y3,
              int tx1, int ty1, int tx2, int ty2, int tx3, int ty3,
              int tw, int th, char *img)
{
  int xstep,xstep2;
  int xval,xval2;
  int txstep,txstep2;
  int tystep,tystep2;
  int txval,txval2;
  int tyval,tyval2;
  int yon;
  int swaptemp;

  image=img; texw=tw; texh=th;
  if (y1 > y3)
  {
    swaptemp=x1; x1=x3; x3=swaptemp;
    swaptemp=y1; y1=y3; y3=swaptemp;
    swaptemp=tx1; tx1=tx3; tx3=swaptemp;
    swaptemp=ty1; ty1=ty3; ty3=swaptemp;
  }
  if (y2 > y3)
  {
    swaptemp=x2; x2=x3; x3=swaptemp;
    swaptemp=y2; y2=y3; y3=swaptemp;
    swaptemp=tx2; tx2=tx3; tx3=swaptemp;
    swaptemp=ty2; ty2=ty3; ty3=swaptemp;
  }
  if (y1 > y2)
  {
    swaptemp=x1; x1=x2; x2=swaptemp;
    swaptemp=y1; y1=y2; y2=swaptemp;
    swaptemp=tx1; tx1=tx2; tx2=swaptemp;
    swaptemp=ty1; ty1=ty2; ty2=swaptemp;
  }
  xstep2=((x3-x1) << 16) / (y3-y1);
  xval2=x1 << 16;
  txstep2=((tx3-tx1) << 16) / (y3-y1);
  txval2=tx1 << 16;
  tystep2=((ty3-ty1) << 16) / (y3-y1);
  tyval2=ty1 << 16;

  if (y1 != y2)
  {
    xstep = ((x2-x1) << 16) / (y2-y1);
    xval = x1 << 16;
    txstep = ((tx2-tx1) << 16) / (y2-y1);
    txval = tx1 << 16;
    tystep = ((ty2-ty1) << 16) / (y2-y1);
    tyval = ty1 << 16;

    for (yon=y1;yon < y2; yon++)
    {
      if ((yon > -1) && (yon < 200))
      {
        tmaphline(xval>>16,xval2>>16,yon,txval>>16,txval2>>16,
                  tyval>>16,tyval2>>16);
      }
      xval+=xstep;
      xval2+=xstep2;
      txval+=txstep;
      txval2+=txstep2;
      tyval+=tystep;
      tyval2+=tystep2;
    }
  }
  if (y2 != y3)
  {
    xstep = ((x3-x2) << 16) / (y3-y2);
    xval = x2 << 16;
    txstep = ((tx3-tx2) << 16) / (y3-y2);
    txval = tx2 << 16;
    tystep = ((ty3-ty2) << 16) / (y3-y2);
    tyval = ty2 << 16;

    for (yon=y2;yon < y3; yon++)
    {
      if ((yon > -1) && (yon < 200))
      {
        tmaphline(xval>>16,xval2>>16,yon,txval>>16,txval2>>16,
                  tyval>>16,tyval2>>16);
      }
      xval+=xstep;
      xval2+=xstep2;
      txval+=txstep;
      txval2+=txstep2;
      tyval+=tystep;
      tyval2+=tystep2;
    }
  }
}

/*
// ============================================================================
// =                          aen's ripple-blitter                            =
// ============================================================================

int thresh;
int dispx[64],dispy[64];

void InitDisplaceStuff(float xmag, float ymag)
{
  int x, y, *d;

  xmag=4.0,d=dispx;
  for (y=0; y<32; y++)
    *d++=sin(y * (360.0/32) * (3.14/180))*xmag;

  ymag=3.9,d=dispy;
  for (x=0; x<32; x++)
    *d++=cos(x * (360.0/32) * (3.14/180))*ymag;

  thresh=14;
}

void CopyTileDisplace(int x, int y, unsigned char *spr)
{
  int i, j;
  int xx, yy, xt, yt;
  int xin, yin;
  byte *dest;

  xin = ((x+xwin)&31)<16 ? 0 : 16;
  yin = ((y+ywin)&31)<16 ? 0 : 16;
  dest=screen+(y*sx)+x;

  // in the yy and xx calcs, +y and +x keep the start position correct
  // within the 32 element lookup tables (ie. so they flow continuously
  // across the 16x16 images).

  for (j=0; j<16; j++)
  {
    yy = (j+yin+thresh) & 31;
    for (i=0; i<16; i++)
    {
      xx = (i+xin+thresh) & 31;
      xt = (i+dispx[yy]) & 15;
      yt = (j+dispy[xx]) & 15;

      dest[i]=spr[(yt*16)+xt];
    }
    dest+=sx;
  }
}

void CopyTileDisplaceClip(int x, int y, unsigned char *spr)
{
  byte *s,*d;
  int xl,yl,xs,ys;
  int i, j;
  int xx, yy, xt, yt;
  int xin, yin;

  cpubyte=RENDER;
  xl=16;
  yl=16;
  xs=ys=0;
  if (x>cx2 || y>cy2 || x+xl<cx1 || y+yl<cy1)
    return;

  if (x+xl > cx2) xl=cx2-x+1;
  if (y+yl > cy2) yl=cy2-y+1;
  if (x<cx1) { xs=cx1-x; xl-=xs; x=cx1; }
  if (y<cy1) { ys=cy1-y; yl-=ys; y=cy1; }

  s=spr;
  //if (ys) s+=(ys*width); // only perform mul if necessary ;)
  //if (xs) s+=xs;
  d=screen+(y*sx)+x;

  xin = ((x+xwin)&31)<16 ? 0 : 16; xin += xs;
  yin = ((y+ywin)&31)<16 ? 0 : 16; yin += ys;

  //dest=screen+(y*sx)+x;

  // in the yy and xx calcs, +y and +x keep the start position correct
  // within the 32 element lookup tables (ie. so they flow continuously
  // across the 16x16 images).

  for (j=0; j<yl; j++)
  {
    yy = (j+yin+thresh) & 31;
    for (i=0; i<xl; i++)
    {
      xx = (i+xin+thresh) & 31;
      xt = (i+xs+dispx[yy]) & 15;
      yt = (j+ys+dispy[xx]) & 15;

      d[i]=s[(yt*16)+xt];
    }
    d+=sx;
  }
  cpubyte=ETC;
} */
