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

#include <math.h>
#include <malloc.h>
#include <string.h>

#include "maped.h"
#include "timer.h"
#include "vdriver.h"

#include "mouse.h"

// ================================= Data ====================================

static int *dmapx = NULL;
static int *dmapy = NULL;
static unsigned char *dscr = NULL;
static int recalc_distort = 1;
static long build = 0;

// ================================= Code ====================================

// aen
static void init_distort(void)
  {
  int x = 0;
  int y = 0;
  int *dx = NULL;
  int *dy = NULL;

  if (dmapx) vfree(dmapx); dmapx=NULL;
  if (dmapy) vfree(dmapy); dmapy=NULL;
  if (dscr) vfree(dscr); dscr=NULL;

  // allocate distort lookups
  dmapx = (int *) valloc(tsx*tsy*4,"ssaver X",0);
  dmapy = (int *) valloc(tsx*tsy*4,"ssaver Y",0);

  // allocate distort image buffer
  dscr = (unsigned char *) valloc(tsx*tsy,"ssaver scrn",0);

  // copy viewport into distort buffer
  memcpy(dscr, screen, tsx*tsy);

  dx = dmapx;
  dy = dmapy;

  // generate distort lookups
  for (y=0; y<tsy; ++y) {
    for (x=0; x<tsx; ++x) {
      *dx++ = sin(y*5*(360.0/tsy)*(3.14/180))*10;
      *dy++ = sin(x*5*(360.0/tsx)*(3.14/180))*10;
      }
    }
  }

// aen
static void do_distort(void)
  {
  unsigned char *ds=NULL;
  int x=0;
  int y=0;
  static int xt=0;
  static int yt=0;
  int xx=0;
  int yy=0;
  static int thresh=0;

  if (recalc_distort) {
    init_distort();
    recalc_distort=0;
    }

  ds=screen; //+(tsx*16)+16;

  do {
    yy = (y+thresh);
    if (yy >= tsy) yy -= tsy;
    xt += (dmapx[(yy*tsx)+x] * build) >> 16;

    xx = (x+((thresh*tsx)/tsy));
    if (xx >= tsx) xx -= tsx;
    yt += (dmapy[(yy*tsx)+xx] * build) >> 16;

    if (xt < 0) xt += tsx;
    if (yt < 0) yt += tsy;
    if (xt >= tsx) xt -= tsx;
    if (yt >= tsy) yt -= tsy;

    *ds++ = dscr[(yt*tsx)+xt];

    ++x;
    if (x >= tsx) {
      x = 0;
      ++y;
      //ds+=32;
      }

    xt = x;
    yt = y;

    } while (y < tsy);

  thresh += 2;
  if (thresh >= tsy)
    thresh -= tsy;

  if (build < 85196) //1.5
    build += 3276;
  }

void ScreenSaver(void)
  {
  while (idlect > 750)
    {
    ReadMouse();
    do_distort();
    ShowPage();
    }
  recalc_distort=1;
  build = 0;
  }
