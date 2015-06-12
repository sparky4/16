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

#define FONT_H
#include "verge.h"

// ================================= Data ====================================

struct fontstruct
{
  byte width, height;                  // Font character dimensions
  byte *data;                          // Font bitmap data
};

struct fontstruct font[10];            // Font stuff;
int numfonts=0;                        // number of loaded fonts.
int curx, cury;                        // x/y font location
char tc=0;                             // text color

// ================================= Code ====================================

int LoadFont(char *fname)
{ VFILE *f=0;
  struct fontstruct *cfont=0;
  int z=0;

  if (!(f=vopen(fname)))
     err("Could not open font %s. \n",fname);

  // alias current font struct
  cfont=font+numfonts;

  if (vgetc(f) != 1)
    err("Font file incorrect version number. \n");

  // font dims
  cfont->width=vgetw(f);
  cfont->height=vgetw(f);

  // get num of sub-set; calc total bytes as well
  z=vgetw(f) * 96 * cfont->width*cfont->height;

  // nab room for font and read it in
  cfont->data=(byte *)valloc(z, "LoadFont:cfont->data", OID_IMAGE);
  vread(cfont->data, z, f);

  vclose(f);

  return numfonts++;
}

void TextColor(int c)
{
  tc=c;
}

void GotoXY(int x, int y)
{
  curx=x;
  cury=y;
}

static void pchar(int fnt, int x, int y, unsigned char c)
{
  int w;
  if (c>31 && c<127)
  {
    w=font[fnt].width,h=font[fnt].height;
    TCopySpriteClip(x,y, w,h, font[fnt].data+((c-32)*w*h)+((tc&3)*96*w*h));
  }
}

void printstring(int fnt, char *str)
{
  if (fnt<0 || fnt >= numfonts)
    return;
  for (; *str; str++)
  {
    unsigned char c=*str;
    switch (c)
    {
      case 126: tc=0; str++; continue;
      case 128: tc=1; str++; continue;
      case 129: tc=2; str++; continue;
      case 130: tc=3; str++; continue;
    }
    pchar(fnt,curx,cury,c);
    curx+=font[fnt].width;
  }
}