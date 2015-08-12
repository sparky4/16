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

byte *screen, *screenx;             // virscr | realscr

// ================================= Code ====================================

void SetMode(int mode)
{
  REGISTERS r;
  SET_AX(r, mode);
  INTERRUPT(0x10, r);
}

int Mode13hShutdown(void)
{
  SetMode(0x3);
  free(screen);
  return 0;
}

void xShowPage(int startofs)
{
  asm("movl _screen, %%esi              \n\t"
      "addl %0, %%esi                   \n\t"
      "movl _screenx, %%edi             \n\t"
      "movl $200, %%eax                 \n\t"
"lineloop:                              \n\t"
      "movl $80, %%ecx                  \n\t"
      "rep                              \n\t"
      "movsl                            \n\t"
      "addl $32, %%esi                  \n\t"
      "decl %%eax                       \n\t"
      "jnz lineloop                     \n\t"
      :
      : "m" (startofs)
      : "esi", "edi", "cc", "eax", "ecx");
}

int Mode13hShowPage(void)
{
  RenderGUI();
  cpubyte=PFLIP;
  xShowPage((16*352)+16);
  cpubyte=ETC;
  return 0;
}

int Mode13hCopySprite(int x, int y, int width, int height, byte *src)
{
  cpubyte=RENDER;

  asm("movl %3, %%edx                   \n\t"
      "movl %4, %%esi                   \n\t"
"csl0:                                  \n\t"
      "movl %1, %%eax                   \n\t"
      "imul $352, %%eax                 \n\t"
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

int Mode13hCopyTile(int x, int y, byte *spr)
{
  cpubyte=RENDER;

  asm("movl $16, %%ecx                  \n\t"
      "movl %2, %%esi                   \n\t"
      "movl %1, %%edi                   \n\t"
      "imul $352, %%edi                 \n\t"
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
      "addl $352, %%edi                 \n\t"
      "decl %%ecx                       \n\t"
      "jnz ctl0                         \n\t"
      :
      : "m" (x), "m" (y), "m" (spr)
      : "eax","ecx","esi","edi","cc" );
  cpubyte=ETC;
  return 0;
}

int Mode13hTCopySprite(int x, int y, int width, int height, byte *src)
{
  cpubyte=RENDER;

  asm("movl %3, %%ecx                   \n\t"
      "movl %4, %%esi                   \n\t"
"tcsl0:                                 \n\t"
      "movl %1, %%eax                   \n\t"
      "imul $352, %%eax                 \n\t"
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

int Mode13hTCopyTile(int x, int y, byte *spr, byte *matte)
{
  cpubyte=RENDER;
  asm("movl $16, %%ecx                  \n\t"
      "movl %2, %%esi                   \n\t"
      "movl %1, %%edi                   \n\t"
      "imul $352, %%edi                 \n\t"
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
      "addl $352, %%edi                 \n\t"
      "addl $16, %%edx                  \n\t"
      "decl %%ecx                       \n\t"
      "jnz tctl0                        \n\t"
      :
      : "m" (x), "m" (y), "m" (spr), "m" (matte)
      : "eax","ecx","edx","esi","edi","cc" );

  cpubyte=ETC;
  return 0;
}

int Mode13hCCopySprite(int x,int y,int width,int height,byte *src)
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
  if (ys) s+=(ys*width); // only perform mul if necessary ;)
  if (xs) s+=xs;
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

int Mode13hTCCopySprite(int x,int y,int width,int height,byte *src)
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
  if (ys) s+=(ys*width); // only perform mul if necessary ;)
  if (xs) s+=xs;
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

int Mode13hScaleSprite(int x, int y, int w, int h, int tw, int th, byte *s)
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

int Mode13hRenderMAPLine(int x, int y, int yofs, word *map)
{
  cpubyte=RENDER;
  asm("movl %1, %%edi                  \n\t"
      "imul $352, %%edi                \n\t"
      "addl %0, %%edi                  \n\t"
      "addl _screen, %%edi             \n\t"
      "movl $22, %%ebx                 \n\t"
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

int Mode13hTRenderMAPLine(int x, int y, int yofs, word *map)
{
  cpubyte=RENDER;
  asm("movl %1, %%edi                  \n\t"
      "imul $352, %%edi                \n\t"
      "addl %0, %%edi                  \n\t"
      "addl _screen, %%edi             \n\t"
      "movl $22, %%ebx                 \n\t"
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

int Mode13hColorField(int x, int y, byte c)
{
  cpubyte=RENDER;
  asm("movl %1, %%eax                   \n\t"
      "imul $352, %%eax                 \n\t"
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
      "addl $336, %%edi                 \n\t"
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
      "addl $336, %%edi                 \n\t"
      "decl %%ecx                       \n\t"
      "jnz lineloop1                    \n\t"
      :
      : "m" (x), "m" (y), "m" (c)
      : "eax","edi","ecx","cc" );
  cpubyte=ETC;
  return 0;
}

int Mode13hClearScreen()
{
  cpubyte=RENDER;
  memset(screen+(352*16)+16,0,(352*200));
  cpubyte=ETC;
  return 0;
}

int Mode13hPutPixel(int x, int y, int color)
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

int Mode13hGetPixel(int x, int y)
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

int Mode13hHLine(int x, int y, int x2, int color)
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

int Mode13hVLine(int x, int y, int y2, int color)
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

int Mode13hLine(int x1, int y1, int x2, int y2, int color)
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

int Mode13hCircle(int x, int y, int radius, int color)
{ int cx=0;
  int cy=radius;
  int df=1-radius;
  int d_e=3;
  int d_se=-2*radius+5;

  cpubyte=RENDER;

  do {
    Mode13hPutPixel(x+cx,y+cy,color);
    if (cx) Mode13hPutPixel(x-cx,y+cy,color);
    if (cy) Mode13hPutPixel(x+cx,y-cy,color);
    if ((cx) && (cy)) Mode13hPutPixel(x-cx,y-cy,color);

    if (cx != cy)
    {
      Mode13hPutPixel(x+cy,y+cx,color);
      if (cx) Mode13hPutPixel(x+cy,y-cx,color);
      if (cy) Mode13hPutPixel(x-cy,y+cx,color);
      if (cx && cy) Mode13hPutPixel(x-cy,y-cx,color);
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

int Mode13hCircleFill(int x, int y, int radius, int color)
{ int cx=0;
  int cy=radius;
  int df=1-radius;
  int d_e=3;
  int d_se=-2*radius+5;

  cpubyte=RENDER;

  do {
    Mode13hHLine(x-cy,y-cx,x+cy,color);
    if (cx) Mode13hHLine(x-cy,y+cx,x+cy,color);

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
        Mode13hHLine(x-cx,y-cy,x+cx,color);
        if (cy) Mode13hHLine(x-cx,y+cy,x+cx,color);
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

int Mode13hRect(int x, int y, int x2, int y2, int color)
{
  Mode13hHLine(x,y,x2,color);
  Mode13hHLine(x,y2,x2,color);
  Mode13hVLine(x,y+1,y2-1,color);
  Mode13hVLine(x2,y+1,y2-1,color);
  return 0;
}

int Mode13hRectFill(int x, int y, int x2, int y2, int color)
{
  cpubyte=RENDER;

  if (y2<y) { int t=y2; y=y2; y2=t; }

  for (; y<=y2; y++)
    Mode13hHLine(x,y,x2,color);

  cpubyte=ETC;
  return 0;
}

void InitMode13h(void)
{
  SetMode(0x13);

  screenx=(byte *) 0xA0000 + __djgpp_conventional_base;
  screen=(byte *) malloc(95744);
  memset(screen, 0, 95744);

  sx=320;  sy=200;
  tsx=352; tsy=232;
  tx=20;   ty=13;
  cx1=0;   cy1=0;
  cx2=sx-1;cy2=sy-1;

  // Mode successfuly set, now lets set up the driver.
  ShutdownVideo = Mode13hShutdown;
  ShowPage = Mode13hShowPage;
  CopySprite = Mode13hCopySprite;
  TCopySprite = Mode13hTCopySprite;
  CCopySprite = Mode13hCCopySprite;
  TCCopySprite = Mode13hTCCopySprite;
  CopyTile = Mode13hCopyTile;
  TCopyTile = Mode13hTCopyTile;
  ScaleSprite = Mode13hScaleSprite;
  RenderMAPLine = Mode13hRenderMAPLine;
  TRenderMAPLine = Mode13hTRenderMAPLine;
  ColorField = Mode13hColorField;
  ClearScreen = Mode13hClearScreen;
  PutPixel = Mode13hPutPixel;
  GetPixel = Mode13hGetPixel;
  HLine = Mode13hHLine;
  VLine = Mode13hVLine;
  Line = Mode13hLine;
  Circle = Mode13hCircle;
  CircleFill = Mode13hCircleFill;
  Rect = Mode13hRect;
  RectFill = Mode13hRectFill;
  DriverDesc = "320x200 (Mode 13h, linear)";
}
