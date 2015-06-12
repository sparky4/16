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


#include <dpmi.h>
#include <sys\nearptr.h>
#include "verge.h"

// ================================= Data ====================================

#define SEQU_ADDR               0x3c4
#define CRTC_ADDR               0x3d4
#define MISC_OUTPUT             0x3c2

byte *screenbase;                   // ptr to A000:0000
int endcol,nextl;                   // end column number thingy | next line
int winofs;

// ================================= Code ====================================

void ModeXSetMode(int mode)
{
  REGISTERS r;
  SET_AX(r, mode);
  INTERRUPT(0x10, r);
}

void plane(byte p)
{
  int hi=1<<p;
  outpw(0x03c4, (hi<<8)|0x02);
}

int ModeXShutdown()
{
  ModeXSetMode(0x3);
  vfree(screen);
  return 0;
}

void clearScreen()
{
   // set write to ALL PLANES simultaneously for extra-quick clears.
   asm("movw $0x3c4, %%dx              \n\t"
       "movw $0x0f02, %%ax             \n\t"
       "outw %%ax, %%dx                \n\t"
       "movl _screenbase, %%edi        \n\t"
       "movl $0, %%eax                 \n\t"
       "movl $11264, %%ecx             \n\t"
       "rep                            \n\t"
       "stosl                          \n\t"
       :
       :
       : "eax","edx","edi","ecx","cc" );
}

void SetView(word offset)
{
  while (inportb(0x3da) & 1);

  _disable();

  outportb(0x3d4, (offset >> 8) | 0x0c);
  outportb(0x3d4, (offset & 0xff) | 0x0d);

  _enable();
}

int ModeXShowPage()
{
  byte *s,*d;
  int x,y,k;
  int sx2;
  int b;

  RenderGUI();
  cpubyte=PFLIP;

  //s=screen+(16*tsx)+16;
  b=(16*tsx)+16;
  d=screenbase;
  sx2=sx>>2;

  //while (!(inp(986) & 8));

  for (y=0; y<sy; ++y,d+=sx2)
  {
    s=screen+(y*tsx)+b;
    for (k=0; k<4; k++,s-=sx)
    {
      plane(k);
      for (x=0; x<sx2; x++,s+=4)
        d[x]=s[k];
    }
  }

  cpubyte=ETC;
  return 0;
}

int ModeXCopySprite(int x, int y, int width, int height, byte *src)
{
  cpubyte=RENDER;

  asm("movl %3, %%edx                   \n\t"
      "movl %4, %%esi                   \n\t"
"csl0:                                  \n\t"
      "movl %1, %%eax                   \n\t"
      "imul _tsx, %%eax                 \n\t"
      "addl %0, %%eax                   \n\t"
      "addl _screen, %%eax              \n\t"
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

int ModeXTCopySprite(int x, int y, int width, int height, byte *src)
{
  cpubyte=RENDER;

  asm("movl %3, %%ecx                   \n\t"
      "movl %4, %%esi                   \n\t"
"tcsl0:                                 \n\t"
      "movl %1, %%eax                   \n\t"
      "imul _tsx, %%eax                 \n\t"
      "addl %0, %%eax                   \n\t"
      "addl _screen, %%eax              \n\t"
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

int ModeXCopyTile(int x, int y, byte *spr)
{
  cpubyte=RENDER;

  asm("movl $16, %%ecx                  \n\t"
      "movl %2, %%esi                   \n\t"
      "movl %1, %%edi                   \n\t"
      "imul _tsx, %%edi                 \n\t"
      "addl %0, %%edi                   \n\t"
      "addl _screen, %%edi              \n\t"
" ctl0:                                 \n\t"
      "movl (%%edi), %%eax              \n\t"
      "andl $0, %%eax                   \n\t"
      "orl  (%%esi), %%eax              \n\t"
      "movl %%eax, (%%edi)              \n\t"
      "movl 4(%%edi), %%eax             \n\t"
      "andl $0, %%eax                   \n\t"
      "orl  4(%%esi), %%eax             \n\t"
      "movl %%eax, 4(%%edi)             \n\t"
      "movl 8(%%edi), %%eax             \n\t"
      "andl $0, %%eax                   \n\t"
      "orl  8(%%esi), %%eax             \n\t"
      "movl %%eax, 8(%%edi)             \n\t"
      "movl 12(%%edi), %%eax            \n\t"
      "andl $0, %%eax                   \n\t"
      "orl  12(%%esi), %%eax            \n\t"
      "movl %%eax, 12(%%edi)            \n\t"
      "addl $16, %%esi                  \n\t"
      "addl _tsx, %%edi                 \n\t"
      "decl %%ecx                       \n\t"
      "jnz ctl0                         \n\t"
      :
      : "m" (x), "m" (y), "m" (spr)
      : "eax","ecx","esi","edi","cc" );
  cpubyte=ETC;
  return 0;
}

int ModeXTCopyTile(int x, int y, byte *spr, byte *matte)
{
  cpubyte=RENDER;
  asm("movl $16, %%ecx                  \n\t"
      "movl %2, %%esi                   \n\t"
      "movl %1, %%edi                   \n\t"
      "imul _tsx, %%edi                 \n\t"
      "addl %0, %%edi                   \n\t"
      "addl _screen, %%edi              \n\t"
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
      "addl _tsx, %%edi                 \n\t"
      "addl $16, %%edx                  \n\t"
      "decl %%ecx                       \n\t"
      "jnz tctl0                        \n\t"
      :
      : "m" (x), "m" (y), "m" (spr), "m" (matte)
      : "eax","ecx","edx","esi","edi","cc" );

  cpubyte=ETC;
  return 0;
}

int ModeXCCopySprite(int x,int y,int width,int height,byte *src)
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
  d=screen+(y*tsx)+x;

  for (; yl; yl--)
  {
    memcpy(d,s,xl);
    s+=width;
    d+=tsx;
  }

  cpubyte=ETC;
  return 0;
}

int ModeXTCCopySprite(int x,int y,int width,int height,byte *src)
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

  cpubyte=ETC;
  return 0;
}

int ModeXScaleSprite(int x, int y, int w, int h, int tw, int th, byte *s)
{ int i,j,xm,ym,xd,yd,sx,sy=0,xs,ys,dys=0;
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

  d = screen+(y*tsx)+x;
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

int ModeXRenderMAPLine(int x, int y, int yofs, word *map)
{
  cpubyte=RENDER;
  asm("movl %1, %%edi                  \n\t"
      "imul _tsx, %%edi                \n\t"
      "addl %0, %%edi                  \n\t"
      "addl _screen, %%edi             \n\t"
      "movl _tx, %%ebx                 \n\t"
      "incl %%ebx                      \n\t"
      "movl %3, %%ecx                  \n\t"
"tileloop:                             \n\t"
      "movw (%%ecx), %%ax              \n\t"
      "movzwl %%ax, %%edx              \n\t"
      "shll $1, %%edx                  \n\t"
      "addl _tileidx, %%edx            \n\t"
      "movw (%%edx), %%ax              \n\t"
      "movzwl %%ax, %%esi              \n\t"
      "shll $8, %%esi                  \n\t"
      "addl _vsp, %%esi                \n\t"
      "movl %2, %%eax                  \n\t"
      "shll $4, %%eax                  \n\t"
      "addl %%eax, %%esi               \n\t"
      "movl (%%esi), %%eax             \n\t"
      "movl %%eax, (%%edi)             \n\t"
      "movl 4(%%esi), %%eax            \n\t"
      "movl %%eax, 4(%%edi)            \n\t"
      "movl 8(%%esi), %%eax            \n\t"
      "movl %%eax, 8(%%edi)            \n\t"
      "movl 12(%%esi), %%eax           \n\t"
      "movl %%eax, 12(%%edi)           \n\t"
      "addl $16, %%edi                 \n\t"
      "addl $2, %%ecx                  \n\t"
      "decl %%ebx                      \n\t"
      "jnz tileloop                    \n\t"
      :
      : "m" (x), "m" (y), "m" (yofs), "m" (map)
      : "eax","ebx","ecx","edx","esi","edi","cc" );

  cpubyte=ETC;
  return 0;
}

int ModeXTRenderMAPLine(int x, int y, int yofs, word *map)
{
  cpubyte=RENDER;
  asm("movl %1, %%edi                  \n\t"
      "imul _tsx, %%edi                \n\t"
      "addl %0, %%edi                  \n\t"
      "addl _screen, %%edi             \n\t"
      "movl _tx, %%ebx                 \n\t"
      "incl %%ebx                      \n\t"
      "movl %3, %%ecx                  \n\t"
"tileloop1:                            \n\t"
      "movw (%%ecx), %%ax              \n\t"
      "movzwl %%ax, %%edx              \n\t"
      "shll $1, %%edx                  \n\t"
      "addl _tileidx, %%edx            \n\t"
      "movw (%%edx), %%ax              \n\t"
      "orw %%ax, %%ax                  \n\t"
      "jz next1                        \n\t"
      "movzwl %%ax, %%esi              \n\t"
      "shll $8, %%esi                  \n\t"
      "movl %%esi, %%edx               \n\t"
      "addl _vspmask, %%edx            \n\t"
      "addl _vsp, %%esi                \n\t"
      "movl %2, %%eax                  \n\t"
      "shll $4, %%eax                  \n\t"
      "addl %%eax, %%esi               \n\t"
      "addl %%eax, %%edx               \n\t"
      "movl (%%edi), %%eax             \n\t"
      "andl (%%edx), %%eax             \n\t"
      "orl  (%%esi), %%eax             \n\t"
      "movl %%eax, (%%edi)             \n\t"
      "movl 4(%%edi), %%eax            \n\t"
      "andl 4(%%edx), %%eax            \n\t"
      "orl  4(%%esi), %%eax            \n\t"
      "movl %%eax, 4(%%edi)            \n\t"
      "movl 8(%%edi), %%eax            \n\t"
      "andl 8(%%edx), %%eax            \n\t"
      "orl  8(%%esi), %%eax            \n\t"
      "movl %%eax, 8(%%edi)            \n\t"
      "movl 12(%%edi), %%eax           \n\t"
      "andl 12(%%edx), %%eax           \n\t"
      "orl  12(%%esi), %%eax           \n\t"
      "movl %%eax, 12(%%edi)           \n\t"
"next1:                                \n\t"
      "addl $16, %%edi                 \n\t"
      "addl $2, %%ecx                  \n\t"
      "decl %%ebx                      \n\t"
      "jnz tileloop1                   \n\t"
      :
      : "m" (x), "m" (y), "m" (yofs), "m" (map)
      : "eax","ebx","ecx","edx","esi","edi","cc" );

  cpubyte=ETC;
  return 0;
}

int ModeXColorField(int x, int y, byte c)
{
  cpubyte=RENDER;
  asm("movl %1, %%eax                   \n\t"
      "imul _tsx, %%eax                 \n\t"
      "addl %0, %%eax                   \n\t"
      "addl _screen, %%eax              \n\t"
      "movl %%eax, %%edi                \n\t"
      "movl $8, %%ecx                   \n\t"
      "movb %2, %%al                    \n\t"
"lineloop1:                             \n\t"
      "stosb                            \n\t"
      "incl %%edi                       \n\t"
      "stosb                            \n\t"
      "incl %%edi                       \n\t"
      "stosb                            \n\t"
      "incl %%edi                       \n\t"
      "stosb                            \n\t"
      "incl %%edi                       \n\t"
      "stosb                            \n\t"
      "incl %%edi                       \n\t"
      "stosb                            \n\t"
      "incl %%edi                       \n\t"
      "stosb                            \n\t"
      "incl %%edi                       \n\t"
      "stosb                            \n\t"
      "incl %%edi                       \n\t"
      "addl _sx, %%edi                  \n\t"
      "addl $16, %%edi                  \n\t"
      "incl %%edi                       \n\t"
      "stosb                            \n\t"
      "incl %%edi                       \n\t"
      "stosb                            \n\t"
      "incl %%edi                       \n\t"
      "stosb                            \n\t"
      "incl %%edi                       \n\t"
      "stosb                            \n\t"
      "incl %%edi                       \n\t"
      "stosb                            \n\t"
      "incl %%edi                       \n\t"
      "stosb                            \n\t"
      "incl %%edi                       \n\t"
      "stosb                            \n\t"
      "incl %%edi                       \n\t"
      "stosb                            \n\t"
      "addl _sx, %%edi                  \n\t"
      "addl $16, %%edi                  \n\t"
      "decl %%ecx                       \n\t"
      "jnz lineloop1                    \n\t"
      :
      : "m" (x), "m" (y), "m" (c)
      : "eax","edi","ecx","cc" );
  cpubyte=ETC;
  return 0;
}

int ModeXClearScreen()
{
  cpubyte=RENDER;
  memset(screen+(tsx*16)+16,0,(tsx*sy));
  cpubyte=ETC;
  return 0;
}

int ModeXPutPixel(int x, int y, int color)
{
  cpubyte=RENDER;

  if (x<cx1 || y<cy1 || x>cx2 || y>cy2)
  {
    cpubyte=ETC;
    return 0;
  }

  x+=16;
  y+=16;

  screen[(y*tsx)+x]=color;

  cpubyte=ETC;
  return 0;
}

int ModeXGetPixel(int x, int y)
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
  return screen[(y*tsx)+x];
}

int ModeXHLine(int x, int y, int x2, int color)
{ byte *d;
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

  d=screen+(y*tsx)+x;
  memset(d,color,width);

  cpubyte=ETC;
  return 0;
}

int ModeXVLine(int x, int y, int y2, int color)
{ byte *d;
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

  d=screen+(y*tsx)+x;
  for (; height; height--)
  {
    *d=color;
    d+=tsx;
  }

  cpubyte=ETC;
  return 0;
}

int ModeXLine(int x1, int y1, int x2, int y2, int color)
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
      screen[(dcy*tsx)+dcx]=color;
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
      screen[(dcy*tsx)+dcx]=color;
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

int ModeXCircle(int x, int y, int radius, int color)
{ int cx=0;
  int cy=radius;
  int df=1-radius;
  int d_e=3;
  int d_se=-2*radius+5;

  cpubyte=RENDER;

  do {
    ModeXPutPixel(x+cx,y+cy,color);
    if (cx) ModeXPutPixel(x-cx,y+cy,color);
    if (cy) ModeXPutPixel(x+cx,y-cy,color);
    if ((cx) && (cy)) ModeXPutPixel(x-cx,y-cy,color);

    if (cx != cy)
    {
      ModeXPutPixel(x+cy,y+cx,color);
      if (cx) ModeXPutPixel(x+cy,y-cx,color);
      if (cy) ModeXPutPixel(x-cy,y+cx,color);
      if (cx && cy) ModeXPutPixel(x-cy,y-cx,color);
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

int ModeXCircleFill(int x, int y, int radius, int color)
{ int cx=0;
  int cy=radius;
  int df=1-radius;
  int d_e=3;
  int d_se=-2*radius+5;

  cpubyte=RENDER;

  do {
    ModeXHLine(x-cy,y-cx,x+cy,color);
    if (cx) ModeXHLine(x-cy,y+cx,x+cy,color);

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
        ModeXHLine(x-cx,y-cy,x+cx,color);
        if (cy) ModeXHLine(x-cx,y+cy,x+cx,color);
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

int ModeXRect(int x, int y, int x2, int y2, int color)
{
  ModeXHLine(x,y,x2,color);
  ModeXHLine(x,y2,x2,color);
  ModeXVLine(x,y+1,y2-1,color);
  ModeXVLine(x2,y+1,y2-1,color);
  return 0;
}

int ModeXRectFill(int x, int y, int x2, int y2, int color)
{
  cpubyte=RENDER;

  if (y2<y) { int t=y2; y=y2; y2=t; }

  for (; y<=y2; y++)
    ModeXHLine(x,y,x2,color);

  cpubyte=ETC;
  return 0;
}

void Set256x256()
// -- ric: 15/Jun/98 --
// My first attempt at graphics code :)
{
  char in_byte;

  outportw(SEQU_ADDR, 0x0100);
  outportb(CRTC_ADDR, 0x11);
  in_byte = inportb((CRTC_ADDR+1));
  in_byte = (in_byte && 0x7f);
  outportb((CRTC_ADDR+1), in_byte);
  outportw(SEQU_ADDR, 0x0604);
  outportb(MISC_OUTPUT, 0xe3);

  outportb(CRTC_ADDR, 0x0); outportb(CRTC_ADDR+1, 0x5f);
  outportb(CRTC_ADDR, 0x1); outportb(CRTC_ADDR+1, 0x3f);
  outportb(CRTC_ADDR, 0x2); outportb(CRTC_ADDR+1, 0x40);
  outportb(CRTC_ADDR, 0x3); outportb(CRTC_ADDR+1, 0x82);
  outportb(CRTC_ADDR, 0x4); outportb(CRTC_ADDR+1, 0x4a);
  outportb(CRTC_ADDR, 0x5); outportb(CRTC_ADDR+1, 0x9a);
  outportb(CRTC_ADDR, 0x6); outportb(CRTC_ADDR+1, 0x23);
  outportb(CRTC_ADDR, 0x7); outportb(CRTC_ADDR+1, 0xb2);
  outportb(CRTC_ADDR, 0x8); outportb(CRTC_ADDR+1, 0x0);
  outportb(CRTC_ADDR, 0x9); outportb(CRTC_ADDR+1, 0x61);
  outportb(CRTC_ADDR, 0x10); outportb(CRTC_ADDR+1, 0xa);
  outportb(CRTC_ADDR, 0x11); outportb(CRTC_ADDR+1, 0xac);
  outportb(CRTC_ADDR, 0x12); outportb(CRTC_ADDR+1, 0xff);
  outportb(CRTC_ADDR, 0x14); outportb(CRTC_ADDR+1, 0x0);
  outportb(CRTC_ADDR, 0x15); outportb(CRTC_ADDR+1, 0x7);
  outportb(CRTC_ADDR, 0x16); outportb(CRTC_ADDR+1, 0x1a);
  outportb(CRTC_ADDR, 0x17); outportb(CRTC_ADDR+1, 0xe3);
  outportb(SEQU_ADDR, 0x1);  outportw(SEQU_ADDR+1, 0x1);
  outportb(0x3ce, 0x5); outportb(0x3cf, 0x40);
  outportb(0x3ce, 0x6); outportb(0x3cf, 0x5);
  inportb(0x3DA);
  outportb(0x3C0, 0x1 | 0x20);
  outportb(0x3C0, 0x41);
  outportb(CRTC_ADDR, 0x13);
  outportb(CRTC_ADDR+1, 0x20);

  outportw(SEQU_ADDR, 0x0300);

  sx=256;  sy=256;
  tsx=288; tsy=288;
  tx=17;   ty=16;
  cx1=0;   cy1=0;
  cx2=sx-1;cy2=sy-1;
  endcol=272; nextl=64;
  winofs=4608;
  DriverDesc = "256x256 (ModeX, planar)";
}

void Set320x240()
{ char in_byte;

  outportw(SEQU_ADDR, 0x0604);
  outportw(SEQU_ADDR, 0x0110);
  outportb(MISC_OUTPUT, 0xe3);
  outportw(SEQU_ADDR, 0x0300);
  outportb(CRTC_ADDR, 0x11);
  in_byte = inportb((CRTC_ADDR+1));
  in_byte = (in_byte && 0x7f);
  outportb((CRTC_ADDR+1), in_byte);
  outportw(CRTC_ADDR, 0x0d06);
  outportw(CRTC_ADDR, 0x3e07);
  outportw(CRTC_ADDR, 0x4109);
  outportw(CRTC_ADDR, 0xea10);
  outportw(CRTC_ADDR, 0xac11);
  outportw(CRTC_ADDR, 0xdf12);
  outportw(CRTC_ADDR, 0x0014);
  outportw(CRTC_ADDR, 0xe715);
  outportw(CRTC_ADDR, 0x0616);
  outportw(CRTC_ADDR, 0xe317);

  sx=320;  sy=240;
  tsx=352; tsy=272;
  tx=20;   ty=15;
  cx1=0;   cy1=0;
  cx2=sx-1;cy2=sy-1;
  endcol=336; nextl=80;
  winofs=5632;
  DriverDesc = "320x240 (ModeX, planar)";
}

void Set360x240()
{ char in_byte;

  outportw(SEQU_ADDR, 0x0604);
  outportw(SEQU_ADDR, 0x100);
  outportb(MISC_OUTPUT, 0xe7);
  outportw(SEQU_ADDR, 0x300);
  outportb(CRTC_ADDR, 0x11);
  in_byte = inportb((CRTC_ADDR+1));
  in_byte = (in_byte && 0x7f);
  outportb((CRTC_ADDR+1), in_byte);
  outportw(CRTC_ADDR, 0x6b00);
  outportw(CRTC_ADDR, 0x5901);
  outportw(CRTC_ADDR, 0x5a02);
  outportw(CRTC_ADDR, 0x8e03);
  outportw(CRTC_ADDR, 0x5e04);
  outportw(CRTC_ADDR, 0x8a05);
  outportw(CRTC_ADDR, 0x0d06);
  outportw(CRTC_ADDR, 0x3e07);
  outportw(CRTC_ADDR, 0x4109);
  outportw(CRTC_ADDR, 0xea10);
  outportw(CRTC_ADDR, 0xac11);
  outportw(CRTC_ADDR, 0xdf12);
  outportw(CRTC_ADDR, 0x2d13);
  outportw(CRTC_ADDR, 0x0014);
  outportw(CRTC_ADDR, 0xe715);
  outportw(CRTC_ADDR, 0x0616);
  outportw(CRTC_ADDR, 0xe317);

  sx=360;  sy=240;
  tsx=392; tsy=272;
  tx=23;   ty=15;
  cx1=0;   cy1=0;
  cx2=sx-1;cy2=sy-1;
  endcol=376; nextl=90;
  winofs=6272;
  DriverDesc = "360x240 (ModeX, planar)";
}

void InitModeX(int res)
{
  ModeXSetMode(0x13);

  switch (res)
  {
    case 0: Set320x240(); break;
    case 1: Set360x240(); break;
    case 2: Set256x256(); break; /* -- ric: 15/Jun/98 -- */
    default: err("Internal error: unknown ModeX resolution code");
  }

  screenbase=(char *) 0xA0000+__djgpp_conventional_base;
  screen=(char *) malloc(107648);
  memset(screen,0,107648);
  clearScreen();

  // Mode successfuly set, now lets set up the driver.

  ShutdownVideo = ModeXShutdown;
  ShowPage = ModeXShowPage;
  CopySprite = ModeXCopySprite;
  TCopySprite = ModeXTCopySprite;
  CCopySprite = ModeXCCopySprite;
  TCCopySprite = ModeXTCCopySprite;
  CopyTile = ModeXCopyTile;
  TCopyTile = ModeXTCopyTile;
  ScaleSprite = ModeXScaleSprite;
  RenderMAPLine = ModeXRenderMAPLine;
  TRenderMAPLine = ModeXTRenderMAPLine;
  ColorField = ModeXColorField;
  ClearScreen = ModeXClearScreen;
  PutPixel = ModeXPutPixel;
  GetPixel = ModeXGetPixel;
  HLine = ModeXHLine;
  VLine = ModeXVLine;
  Line = ModeXLine;
  Circle = ModeXCircle;
  CircleFill = ModeXCircleFill;
  Rect = ModeXRect;
  RectFill = ModeXRectFill;
}
