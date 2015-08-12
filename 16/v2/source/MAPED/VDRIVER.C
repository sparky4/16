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

#include "mode13h.h"
#include "modex.h"

// ================================= Data ====================================

// Driver function-pointers.
int (*ShutdownVideo) (void);
int (*ShowPage) (void);
int (*CopySprite) (int x, int y, int width, int height, char *src);
int (*CCopySprite) (int x, int y, int width, int height, char *src);
int (*TCCopySprite) (int x, int y, int width, int height, char *src);
int (*TCopySprite) (int x, int y, int width, int height, char *src);
int (*CopyTile) (int x, int y, char *src);
int (*FilledBox) (int x, int y, int width, int height, char c);
int (*VLine) (int x, int y, int height, char color);
int (*HLine) (int x, int y, int width, char color);
int (*ColorGrid) (int x, int y, char c);
int (*ClearScreen) (void);
int (*CopySpriteLucentClip) (int x, int y, int width, int height, unsigned char *src);

unsigned char pal[768], pal2[768];
unsigned char *screen=0;

int sx=0, sy=0;
int tx=0, ty=0;
int tsx=0, tsy=0;
int map_scroll_x=1;
int map_scroll_y=1;

// ================================= Code ====================================

void InitVideo(int mode)
{
  switch (mode)
  {
    case 0: InitMode13h(); break;
    case 1: InitModeX();             // 320x240
            break;
  }
}

void SetPalette(unsigned char *pall)
{ unsigned int i;

  outp(0x03c8,0);
  for (i=0; i<768; i++)
    outp(0x03c9,pall[i]);
}

void GetPalette(void)
{ unsigned int i;

  outp(0x03c7,0);
  for (i=0; i<768; i++)
    pal[i]=inp(0x03c9);
}

void set_intensity(unsigned int n)
{ unsigned int i;

  for (i=0; i<768; i++)
    pal2[i]=(pal[i]*n)>>6;

  SetPalette(pal2);
}

#define byte unsigned char
int cx1=16, cy1=16, cx2=336, cy2=256;

char stipple[256]={
  1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
  0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
  1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
  0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
  1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
  0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
  1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
  0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
  1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
  0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
  1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
  0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
  1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
  0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
  1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
  0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
};
