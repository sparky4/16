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

#include <pc.h>
#include <dpmi.h>
#include <go32.h>

#include <dpmi.h>
#include <sys\nearptr.h>
#include "verge.h"

// ================================= Data ====================================

byte *vesascreen, *vesascreenx;             // virscr | realscr

// ================================= Code ====================================

void VESASetMode(int mode)
{
  REGISTERS r;
  SET_AX(r, mode);
  INTERRUPT(0x10, r);
}

int VESAShutdown(void)
{
  VESASetMode(0x3);
  free(vesascreen);
  return 0;
}

void VESASetBank(int bank)
{
  REGISTERS r;
  SET_AX(r, 0x4f05);
  SET_BX(r, 0);
  SET_DX(r, bank);
  INTERRUPT(0x10, r);
}

int VESAShowPage(void)
{ byte *s,*d;
  int y,bytes,bank;
  int span;

  RenderGUI();
  cpubyte=PFLIP;

  s=vesascreen+(16*tsx)+16;
  d=vesascreenx;

  bank=0;
  bytes=65536;
  VESASetBank(bank++);

  for (y=sy; y; y--)
  {
    if (bytes >= sx)
    { memcpy(d,s,sx);
      bytes-=sx;
      s+=sx;
      d+=sx;
    }
    else
    { memcpy(d,s,bytes);
      s+=bytes;
      span=sx-bytes;

      VESASetBank(bank++);
      d=vesascreenx;
      bytes=65536;

      memcpy(d,s,span);
      bytes-=span;
      s+=span;
      d+=span;
    }
    s+=32;
  }

  cpubyte=ETC;
  return 0;
}

int VESACopySprite(int x, int y, int width, int height, byte *src)
{
  cpubyte=RENDER;

  asm("movl %3, %%edx                   \n\t"
      "movl %4, %%esi                   \n\t"
"csl0:                                  \n\t"
      "movl %1, %%eax                   \n\t"
      "imul $672, %%eax                 \n\t"
      "addl %0, %%eax                   \n\t"
      "addl _vesascreen, %%eax          \n\t"
      "movl %%eax, %%edi                \n\t"
      "movl %2, %%ecx                   \n\t"
      "shrl $2, %%ecx                   \n\t"
      "repz                             \n\t"
      "movsl                            \n\t"
      "incl %1                          \n\t"
      "decl %%edx                       \n\t"
      "jnz csl0                         \n\t"
      :
      : "m" (x), "m" (y), "m" (width), "m" (height), "m" (src)
      : "eax","edx","esi","edi","ecx","cc" );
  cpubyte=ETC;
  return 0;
}

int VESACCopySprite(int x,int y,int width,int height,byte *src)
{
  byte *s,*d;
  int xl,yl,xs,ys;
  int cx1=0, cy1=0, cx2=tsx-1, cy2=tsy-1;

  cpubyte=RENDER;

  xl=width;
  yl=height;
  xs=ys=0;

  if (x>cx2 || y>cy2 || x+xl<cx1 || y+yl<cy1)
    return 0;

  if (x+xl > cx2) xl=cx2-x+1;
  if (y+yl > cy2) yl=cy2-y+1;

  if (x<cx1) { xs=cx1-x; xl-=xs; x=cx1; }
  if (y<cy1) { ys=cy1-y; yl-=ys; y=cy1; }

  s=src;
  if (xs+ys) s+=(ys*width)+xs;
  d=vesascreen+(y*tsx)+x;

  for (; yl; yl--)
  {
    memcpy(d,s,xl);
    s+=width;
    d+=tsx;
  }

  cpubyte=ETC;
  return 0;
}

int VESATCCopySprite(int x,int y,int width,int height,byte *src)
{
  byte *s,*d,c;
  int xl,yl,xs,ys;
  int cx1=0, cy1=0, cx2=tsx-1, cy2=tsy-1;

  cpubyte=RENDER;

  xl=width;
  yl=height;
  xs=ys=0;

  if (x>cx2 || y>cy2 || x+xl<cx1 || y+yl<cy1)
    return 0;

  if (x+xl > cx2) xl=cx2-x+1;
  if (y+yl > cy2) yl=cy2-y+1;

  if (x<cx1) { xs=cx1-x; xl-=xs; x=cx1; }
  if (y<cy1) { ys=cy1-y; yl-=ys; y=cy1; }

  s=src;
  if (xs+ys) s+=(ys*width)+xs;
  d=vesascreen+(y*tsx)+x;

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

  cpubyte=ETC;
  return 0;
}

int VESACopyTile(int x, int y, byte *spr)
{
  int h;
  byte *d;

  cpubyte=RENDER;

  h=16;

  d=vesascreen+(y*tsx)+x;
  for (; h; h--)
  {
    memcpy(d, spr, 16);
    spr+=16;
    d+=tsx;
  }

  cpubyte=ETC;
  return 0;
}

int VESATCopySprite(int x, int y, int width, int height, byte *src)
{
  cpubyte=RENDER;

  asm("movl %3, %%ecx                   \n\t"
      "movl %4, %%esi                   \n\t"
"tcsl0:                                 \n\t"
      "movl %1, %%eax                   \n\t"
      "imul $672, %%eax                 \n\t"
      "addl %0, %%eax                   \n\t"
      "addl _vesascreen, %%eax          \n\t"
      "movl %%eax, %%edi                \n\t"
      "movl %2, %%edx                   \n\t"
"drawloop:                              \n\t"
      "lodsb                            \n\t"
      "orb %%al, %%al                   \n\t"
      "jz nodraw                        \n\t"
      "stosb                            \n\t"
      "decl %%edx                       \n\t"
      "jz endline                       \n\t"
      "jmp drawloop                     \n\t"
"nodraw:                                \n\t"
      "incl %%edi                       \n\t"
      "decl %%edx                       \n\t"
      "jnz drawloop                     \n\t"
"endline:                               \n\t"
      "incl %1                          \n\t"
      "decl %%ecx                       \n\t"
      "jnz tcsl0                        \n\t"
      :
      : "m" (x), "m" (y), "m" (width), "m" (height), "m" (src)
      : "eax","edx","esi","edi","ecx","cc" );

  cpubyte=ETC;
  return 0;
}

int VESATCopyTile(int x, int y, byte *spr, byte *matte)
{
  cpubyte=RENDER;
  asm("movl $16, %%ecx                  \n\t"
      "movl %2, %%esi                   \n\t"
      "movl %1, %%edi                   \n\t"
      "imul $672, %%edi                 \n\t"
      "addl %0, %%edi                   \n\t"
      "addl _vesascreen, %%edi          \n\t"
      "movl %3, %%edx                   \n\t"
"tctl0:                                 \n\t"
      "movl (%%edi), %%eax              \n\t"
      "andl (%%edx), %%eax              \n\t"
      "orl  (%%esi), %%eax              \n\t"
      "movl %%eax, (%%edi)              \n\t"
      "movl 4(%%edi), %%eax             \n\t"
      "andl 4(%%edx), %%eax             \n\t"
      "orl  4(%%esi), %%eax             \n\t"
      "movl %%eax, 4(%%edi)             \n\t"
      "movl 8(%%edi), %%eax             \n\t"
      "andl 8(%%edx), %%eax             \n\t"
      "orl  8(%%esi), %%eax             \n\t"
      "movl %%eax, 8(%%edi)             \n\t"
      "movl 12(%%edi), %%eax            \n\t"
      "andl 12(%%edx), %%eax            \n\t"
      "orl  12(%%esi), %%eax            \n\t"
      "movl %%eax, 12(%%edi)            \n\t"
      "addl $16, %%esi                  \n\t"
      "addl $672, %%edi                 \n\t"
      "addl $16, %%edx                  \n\t"
      "decl %%ecx                       \n\t"
      "jnz tctl0                        \n\t"
      :
      : "m" (x), "m" (y), "m" (spr), "m" (matte)
      : "eax","ecx","edx","esi","edi","cc" );

  cpubyte=ETC;
  return 0;
}

int VESAScaleSprite(int x, int y, int w, int h, int tw, int th, byte *s)
{
  int i,j,xm,ym,xd,yd,sx,sy=0,xs,ys,dys=0;
  unsigned char *d;

  cpubyte=RENDER;

  if (!tw || !th) return 0;
  tw += sgn(tw); th += sgn(th);
  xm = abs(tw); ym = abs(th);
  xs = (w<<16)/xm; ys = (h<<16)/ym;
  xd = sgn(tw); yd = sgn(th);

  if (tw>0 && th>0) dys=tsx-xm;
  else if (tw>0 && th<0) dys=(0-tsx)-xm;
  else if (tw<0 && th>0) dys=tsx+xm;
  else if (tw<0 && th<0) dys=(0-tsx)+xm;

  d = vesascreen+(y*tsx)+x;
  for (i=0;i<ym;++i) {
    sx=0;
    for (j=0;j<xm;++j) {
      *d=s[(sx>>16)];
      d+=xd;
      sx+=xs;
      }
    d+=dys;
    sy+=ys;
    s+=(sy>>16)*w;
    sy&=0xffff;
    }
  cpubyte=ETC;
  return 0;
}

int VESARenderMAPLine(int x, int y, int yofs, word *map)
{
  cpubyte=RENDER;
  // ...
  cpubyte=ETC;
  return 0;
}

int VESATRenderMAPLine(int x, int y, int yofs, word *map)
{
  cpubyte=RENDER;
  // ...
  cpubyte=ETC;
  return 0;
}

int VESAColorField(int x, int y, byte c)
{
  cpubyte=RENDER;
  // ...
  cpubyte=ETC;
  return 0;
}

int VESAClearScreen()
{
  cpubyte=RENDER;
  memset(vesascreen+(672*16)+16,0,(672*480));
  cpubyte=ETC;
  return 0;
}

int VESAPutPixel(int x, int y, int color)
{
  cpubyte=RENDER;

  if (x<cx1 || y<cy1 || x>cx2 || y>cy2)
  {
    cpubyte=ETC;
    return 0;
  }

  x+=16;
  y+=16;

  vesascreen[(y*tsx)+x]=color;

  cpubyte=ETC;
  return 0;
}

int VESAGetPixel(int x, int y)
{
  cpubyte=RENDER;

  if (x<cx1 || y<cy1 || x>cx2 || y>cy2)
  {
    cpubyte=ETC;
    return 0;
  }

  x+=16;
  y+=16;

  cpubyte=ETC;
  return vesascreen[(y*tsx)+x];
  return 0;
}

int VESAHLine(int x, int y, int x2, int color)
{
  byte *d;
  int width;

  cpubyte=RENDER;

  // swap?
  if (x2<x) { int t=x2; x=x2; x2=t; }

  width=x2-x+1;
  if (x>cx2 || y>cy2 || x+width<cx1 || y<cy1)
    return 0;

  if (x+width > cx2) width=cx2-x+1;
  if (x<cx1) { width-=(cx1-x); x=cx1; }

  x+=16;
  y+=16;
  x2+=16;

  d=vesascreen+(y*tsx)+x;
  memset(d,color,width);

  cpubyte=ETC;
  return 0;
}

int VESAVLine(int x, int y, int y2, int color)
{
  byte *d;
  int height;

  cpubyte=RENDER;

  // swap?
  if (y2<y) { int t=y2; x=y2; y2=t; }

  height=y2-y+1;
  if (x>cx2 || y>cy2 || x<cx1 || y+height<cy1)
  {
    cpubyte=ETC;
    return 0;
  }

  if (y+height > cy2) height=cy2-y+1;
  if (y<cy1) { height-=(cy1-y); y=cy1; }

  x+=16;
  y+=16;
  y2+=16;

  d=vesascreen+(y*tsx)+x;
  for (; height; height--)
  {
    *d=color;
    d+=tsx;
  }

  cpubyte=ETC;
  return 0;
}

int VESALine(int x1, int y1, int x2, int y2, int color)
{
  short i,xc,yc,er,n,m,xi,yi,xcxi,ycyi,xcyi;
  unsigned dcy,dcx;

  cpubyte=RENDER;

  // check to see if the line is completly clipped off
  if ((x1<cx1 && x2<cx1) || (x1>cx2 && x2>cx2)
  || (y1<cy1 && y2<cy1) || (y1>cy2 && y2>cy2))
  {
    cpubyte=ETC;
    return 0;
  }

  if (x1>x2)
  {
    i=x1; x1=x2; x2=i;
    i=y1; y1=y2; y2=i;
  }

  // clip the left side
  if (x1<cx1)
  { int myy=(y2-y1);
    int mxx=(x2-x1),b;
    if (!mxx)
    {
      cpubyte=ETC;
      return 0;
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
      return 0;
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
    i=x1; x1=x2; x2=i;
    i=y1; y1=y2; y2=i;
  }

  // clip the bottom
  if (y2>cy2)
  { int mxx=(x2-x1);
    int myy=(y2-y1),b;
    if (!myy)
    {
      cpubyte=ETC;
      return 0;
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
      return 0;
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
    return 0;
  }

  if (x1>x2)
  { xc=x2; xi=x1; }
  else { xi=x2; xc=x1; }

  x1+=16;y1+=16; // aen; adjust these here??
  x2+=16;y2+=16;

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
      vesascreen[(dcy*tsx)+dcx]=color;
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
      vesascreen[(dcy*tsx)+dcx]=color;
      if (er>0)
      { dcx+=xi;
        er-=ycyi;
      }
      er+=xcyi;
      dcy+=yi;
    }
  }

  cpubyte=ETC;
  return 0;
}

int VESACircle(int x, int y, int radius, int color)
{
  int cx=0;
  int cy=radius;
  int df=1-radius;
  int d_e=3;
  int d_se=-2*radius+5;

  cpubyte=RENDER;

  do {
    VESAPutPixel(x+cx,y+cy,color);
    if (cx) VESAPutPixel(x-cx,y+cy,color);
    if (cy) VESAPutPixel(x+cx,y-cy,color);
    if ((cx) && (cy)) VESAPutPixel(x-cx,y-cy,color);

    if (cx != cy)
    {
      VESAPutPixel(x+cy,y+cx,color);
      if (cx) VESAPutPixel(x+cy,y-cx,color);
      if (cy) VESAPutPixel(x-cy,y+cx,color);
      if (cx && cy) VESAPutPixel(x-cy,y-cx,color);
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
  return 0;
}

int VESACircleFill(int x, int y, int radius, int color)
{
  int cx=0;
  int cy=radius;
  int df=1-radius;
  int d_e=3;
  int d_se=-2*radius+5;

  cpubyte=RENDER;

  do {
    VESAHLine(x-cy,y-cx,x+cy,color);
    if (cx) VESAHLine(x-cy,y+cx,x+cy,color);

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
        VESAHLine(x-cx,y-cy,x+cx,color);
        if (cy) VESAHLine(x-cx,y+cy,x+cx,color);
      }

      df+=d_se;
      d_e+=2;
      d_se+=4;
      cy--;
    }

    cx++;

  } while (cx <= cy);

  cpubyte=ETC;
  return 0;
}

int VESARect(int x, int y, int x2, int y2, int color)
{
  VESAHLine(x,y,x2,color);
  VESAHLine(x,y2,x2,color);
  VESAVLine(x,y+1,y2-1,color);
  VESAVLine(x2,y+1,y2-1,color);
  return 0;
}

int VESARectFill(int x, int y, int x2, int y2, int color)
{
  cpubyte=RENDER;

  if (y2<y) { int t=y2; y=y2; y2=t; }

  for (; y<=y2; y++)
    VESAHLine(x,y,x2,color);

  cpubyte=ETC;
  return 0;
}

void InitVESA(void)
{
  REGISTERS r;

  SET_BX(r, 0x0101);
  SET_AX(r, 0x4f02);
  INTERRUPT(0x10, r);

  vesascreenx = (byte *) 0xA0000 + __djgpp_conventional_base;
  vesascreen = (byte *) valloc(344064, "vesascreen", 0);
  memset(vesascreen, 0, 344064);

  sx=640;  sy=480;
  tsx=672; tsy=512;
  tx=40;   ty=30;
  cx1=0;   cy1=0;
  cx2=sx-1;cy2=sy-1;

  // Mode successfuly set, now lets set up the driver.
  ShutdownVideo = VESAShutdown;
  ShowPage = VESAShowPage;
  CopySprite = VESACopySprite;
  TCopySprite = VESATCopySprite;
  CCopySprite = VESACCopySprite;
  TCCopySprite = VESATCCopySprite;
  CopyTile = VESACopyTile;
  TCopyTile = VESATCopyTile;
  ScaleSprite = VESAScaleSprite;
  RenderMAPLine = VESARenderMAPLine;
  TRenderMAPLine = VESATRenderMAPLine;
  ColorField = VESAColorField;
  ClearScreen = VESAClearScreen;
  PutPixel = VESAPutPixel;
  GetPixel = VESAGetPixel;
  HLine = VESAHLine;
  VLine = VESAVLine;
  Line = VESALine;
  Circle = VESACircle;
  CircleFill = VESACircleFill;
  Rect = VESARect;
  RectFill = VESARectFill;
  DriverDesc = "640x480 (VESA, Banked)";
}
