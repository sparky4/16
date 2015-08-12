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

#include <stdio.h>
#include <i86.h>
#include <string.h>

#include "timer.h"
#include "vdriver.h"

// ================================= Data ====================================

extern char mousepic[]; // blah

static union REGS regs;

int mx=0,my=0,mb=0;
int rb=0,wb=0;
int lmx=0,lmy=0,lmb=0;

// ================================= Code ====================================

void PutMouse(int x, int y)
{
  lmx=mx; lmy=my; lmb=mb;
  memset(&regs, 0, sizeof regs);
  regs.w.ax=4;
  regs.w.cx=x*2;
  regs.w.dx=y*2;
  int386(0x33,&regs,&regs);

  mx=x; my=y;
}

void InitMouse()
{
  memset(&regs, 0, sizeof regs);
  int386(0x33,&regs,&regs);

  regs.w.ax=7;
  regs.w.cx=0;
  regs.w.dx=640; /*320;*/ /* -- ric:13/Jun/98 - fix horizontal jumps -- */
  int386(0x33,&regs,&regs);

  regs.w.ax=8;
  regs.w.cx=0;
  regs.w.dx=sy; // aen
  int386(0x33,&regs,&regs);

  PutMouse(sx/2,(sy/2) /2); // aen
}

void ReadMouse()
{
  lmx=mx; lmy=my; lmb=mb;
  memset(&regs, 0, sizeof regs);
  regs.w.ax=3;
  int386(0x33,&regs,&regs);

  mx=((regs.w.cx)>>1)+16;  /* -- ric:13/Jun/98 - fix horizontal jumps -- */
  my=(regs.w.dx)+16;
  rb=regs.w.bx;

  if (rb && wb) { mb=0; return; }
  if (wb && !rb) wb=0;
  mb=rb;

  if ((lmx!=mx) || (lmy!=my) || (lmb!=mb)) idlect=0;
}

void DrawMouse()
{
  ReadMouse();
  TCopySprite(mx,my,4,5,mousepic);
}

void WaitRelease()
{
  wb=1;
  mb=0;
}
