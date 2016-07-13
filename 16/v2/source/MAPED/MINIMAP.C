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
#include <string.h>
#include <malloc.h>

#include "config.h"
#include "keyboard.h"
#include "maped.h"
#include "mouse.h"
#include "vdriver.h"

// ============================ Data ============================

unsigned char mvsp[2048];         // mini-VSP dominant color
int xoff=0, yoff=0;               // x-offset, y-offset

// ============================ Code ============================
/*
FindDominantColor(int i)
{ unsigned char *src;
  int j,tally=0;

  // This is a cheap-ass method. I have a more proper method in mind, but
  // I'm going to do this for now b/c I'm lazy. Actually, the real reason is
  // speed.

  src=vsp+(i*256);

  for (j=0; j<256; j++)
  {
    tally+=*src;
    src++;
  }
  tally=tally/256;
  mvsp[i]=(unsigned char) tally;
}
*/

void FindDominantColor(int i)
{ unsigned char *src, tally[256], tab;
  int j;

  src=vsp+(i*256);
  memset(&tally, 0, 256);

  for (j=0; j<256; j++)
  {
    tally[*src]++;
    src++;
  }
  tab=0;
  for (j=0; j<256; j++)
  {
    if (tally[j] > tab) tab=j;
  }
  mvsp[i]=(unsigned char) tab;
}

void GenerateMiniVSP()
{ int i;

  for (i=0; i<numtiles; i++)
      FindDominantColor(i);
}

void MiniMAP()
{ char *ptr,c;
  int _x,_y,ct;
  int i;

  GenerateMiniVSP();
  ptr=(char *) valloc(layer[0].sizex*layer[0].sizey,"minimap",0);
  //memset(ptr,0,layer[0].sizex*layer[0].sizey);

  // Now we "draw" the tiles into the buffer.

  for (i=0; i<4; i++)
  {
    if (!layertoggle[i]) continue;
    ct=0;
    for (_y=0; _y<layer[i].sizey; _y++)
      for (_x=0; _x<layer[i].sizex; _x++)
      {
         c=(unsigned char) mvsp[layers[i][(_y*layer[i].sizex)+_x]];
         if (layers[i][(_y*layer[i].sizex)+_x]) ptr[ct]=c;
         ct++;
      }
  }

  while (!key[SCAN_ESC] && !mb)
  {
    ClearScreen();
    CCopySprite(16-xoff, 16-yoff, layer[0].sizex, layer[0].sizey, ptr);
    ReadMouse();
    HLine(mx, my, 20, white);
    VLine(mx, my, ty, white);
    VLine(mx+19, my, ty, white);
    HLine(mx, my+ty, 20, white);

    ShowPage();

    if (key[SCAN_LEFT] && xoff)
    {
      if (xoff > 0) xoff-=16;
      key[SCAN_LEFT]=0;
    }
    if (key[SCAN_UP] && yoff)
    {
      if (yoff > 0) yoff-=16;
      key[SCAN_UP]=0;
    }
    if (key[SCAN_DOWN])
    {
      if (yoff < layer[0].sizey)
        yoff+=16;
      key[SCAN_DOWN]=0;
    }
    if (key[SCAN_RIGHT])
    {
      if (xoff < layer[0].sizex)
        xoff+=16;
      key[SCAN_RIGHT]=0;
    }
    if (mb)
    {
      xwin=(xoff+mx-16)*16;
      ywin=(yoff+my-16)*16;
      if (xwin>=(layer[0].sizex*16)-320) xwin=(layer[0].sizex*16)-320;
      if (ywin>=(layer[0].sizey*16)-tsy-15) ywin=(layer[0].sizey*16)-tsy-15;
      WaitRelease();
      break;
    }
  }
  key[SCAN_ESC]=0;
  vfree(ptr);
}
