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
#include "maped.h"
#include "vdriver.h"

#include "render.h" // MAXTILES

// ================================= Data ====================================

extern char stipple[256];

int oxw=0, oyw=0;
int xofs=0, yofs=0;
int xtc=0, ytc=0;

char curlayer=0;
short tileidx[MAXTILES];
char flipped[MAXTILES];

// ------

chri chrs[100];

// ================================= Code ====================================

void DoCHR(int i) // hee hee!
{
  FILE *f;
  char b;
  int q;
  short numframes;

//  free(chrs[i].frame);
//  chrs[i].frame=0;
//  return;

  if (!strlen(chrlist[i].t))
  {
    if (chrs[i].frame) vfree(chrs[i].frame);
    chrs[i].frame=0;
    chrs[i].fx=chrs[i].fy=0;
    chrs[i].hx=chrs[i].hy=0;
    return;
  }
  if (!(f=fopen(chrlist[i].t,"rb")))
  {
    if (chrs[i].frame) vfree(chrs[i].frame);
    chrs[i].frame=0;
    chrs[i].fx=chrs[i].fy=0;
    chrs[i].hx=chrs[i].hy=0;
    return;
  }
  if (chrs[i].frame) vfree(chrs[i].frame);
  fread(&b, 1, 1, f);
  if (b!=2)
  {
    // This should really not work with V1 CHRs, but..
    chrs[i].frame=(char *) malloc(512);
    fseek(f, 0, 0);
    fread(chrs[i].frame, 1, 512, f);
    fclose(f);
    chrs[i].fx=16; chrs[i].fy=32;
    chrs[i].hx=0;  chrs[i].hy=16;
    return;
  }
  chrs[i].fx=0; fread(&chrs[i].fx, 1, 2, f);
  chrs[i].fy=0; fread(&chrs[i].fy, 1, 2, f);
  chrs[i].hx=0; fread(&chrs[i].hx, 1, 2, f);
  chrs[i].hy=0; fread(&chrs[i].hy, 1, 2, f);
  fread(strbuf, 2, 2, f);
  fread(&numframes, 1, 2, f);
  chrs[i].frame=(char *) valloc(chrs[i].fx*chrs[i].fy*numframes,"chr preview",i);
  fread(&q, 1, 4, f);

  ReadCompressedLayer1(chrs[i].frame, chrs[i].fx*chrs[i].fy*numframes,f);

  fclose(f);
}

void DoCHRdealy()
{
  int i;

  for (i=0; i<100; i++)
    DoCHR(i);
}

void BlitBackLayer(char l)
{
  int i,j;
  //int a;
  char *img;
  //char *d;
  unsigned short *lay;
  //int v;
  int mapx,mapy;
  int x,y;
  unsigned int tile;

  if (!layertoggle[l]) return;

  mapx = layer[l].sizex;
  mapy = layer[l].sizey;
  lay = layers[l];

  // Adjust view
  oxw  = xwin * layer[l].pmultx / layer[l].pdivx;
  oyw  = ywin * layer[l].pmulty / layer[l].pdivy;
  xofs = 16 - (oxw & 15);
  yofs = 16 - (oyw & 15);
  xtc  = oxw / 16;
  ytc  = oyw / 16;

  // Draw it, bitch!
  for (i=0; i <= ty; i++)
  {
    for (j=0; j <= tx; j++)
    {
      x = xtc+j;
      y = ytc+i;

      // write better clipping sometime
      if (y >= 0 || y < mapy || x >=0 || x < mapx)
      {
        // what the HELL @_@
        tile = lay[(y * mapx) + x];
        if (tile >= numtiles) tile = numtiles-1;
        tile = tileidx[tile];
        if (tile >= numtiles) tile = numtiles-1;

        img = &vsp[tile * 256];

        x = (j * 16) + xofs;
        y = (i * 16) + yofs;

        //CopyTile(x, y, img);
        CCopySprite(x, y, 16, 16, img);
      }
    }
  }

  curlayer++;
}

//zero
extern char* translucency_table;
void LucentBlitLayer(char l)
{
  int i,j;
  //int a,b;
  char *img;
  //char *d;
  unsigned short *lay;
  //int v;
  int mapx,mapy;
  int x,y;
  unsigned int tile;

  if (!layertoggle[l]) return;
  if(!translucency_table)
  {
    TransBlitlayer(l);
    return;
  }

  mapx = layer[l].sizex;
  mapy = layer[l].sizey;
  lay = layers[l];

  // Adjust view
  oxw  = xwin * layer[l].pmultx / layer[l].pdivx;
  oyw  = ywin * layer[l].pmulty / layer[l].pdivy;
  xofs = 16 - (oxw & 15);
  yofs = 16 - (oyw & 15);
  xtc  = oxw / 16;
  ytc  = oyw / 16;

  // Draw it, bitch!
  for (i=0; i <= ty; i++)
  {
    for (j=0; j <= tx; j++)
    {
      x = xtc+j;
      y = ytc+i;

      // write better clipping sometime
      if (y >= 0 || y < mapy || x >=0 || x < mapx)
      {
        // what the HELL @_@
        tile = lay[(y * mapx) + x];
        if (tile >= numtiles) tile = numtiles-1;
        tile = tileidx[tile];
        if (tile >= numtiles) tile = numtiles-1;

        img = &vsp[tile * 256];
        if (img != vsp)
        {
          x = (j * 16) + xofs;
          y = (i * 16) + yofs;
          CopySpriteLucentClip(x, y, 16, 16, img);
        }
      }
    }
  }
  curlayer++;
}


void TransBlitlayer(char l)
{
  int i,j;
  //int a,b;
  char *img;
  //char *d;
  unsigned short *lay;
  //int v;
  int mapx,mapy;
  int x,y;
  unsigned int tile;

  if (!layertoggle[l]) return;

  lay = layers[l];

  mapx = layer[l].sizex;
  mapy = layer[l].sizey;

  // Adjust view
  oxw  = xwin * layer[l].pmultx / layer[l].pdivx;
  oyw  = ywin * layer[l].pmulty / layer[l].pdivy;
  xofs = 16 - (oxw & 15);
  yofs = 16 - (oyw & 15);
  xtc  = oxw / 16;
  ytc  = oyw / 16;

  // Draw it, bitch!
  for (i=0; i <= ty; i++)
  {
    for (j=0; j <= tx; j++)
    {
      x = xtc+j;
      y = ytc+i;

      // write better clipping sometime
      if (y >= 0 || y < mapy || x >=0 || x < mapx)
      {
        // what the HELL @_@
        tile = lay[(y * mapx) + x];
        if (tile >= numtiles) tile = numtiles-1;
        tile = tileidx[tile];
        if (tile >= numtiles) tile = numtiles-1;

        img = &vsp[tile * 256];
        if (img != vsp)
        {
          x = (j * 16) + xofs;
          y = (i * 16) + yofs;
          //TCopySprite(x, y, 16, 16, img);
          TCCopySprite(x, y, 16, 16, img);
        }
      }
    }
  }

  curlayer++;
}

void BlitLayer(char c)
{
  if(layer[c].trans) { LucentBlitLayer(c); return; } //zero
  if (curlayer) TransBlitLayer(c);
  else BlitBackLayer(c);
}

void DrawObstructions()
{ int i,j;
  int mapx;

  mapx = layer[0].sizex;

  oxw=xwin*layer[0].pmultx/layer[0].pdivx;
  oyw=ywin*layer[0].pmulty/layer[0].pdivy;
  xofs=16-(oxw&15);
  yofs=16-(oyw&15);
  xtc=oxw>>4;
  ytc=oyw>>4;

  for (i=0; i<ty+1; i++)
    for (j=0; j<tx+1; j++)
    {
      if (obstruct[((ytc+i)*mapx)+(xtc+j)])
      {
         TCCopySprite((j*16)+xofs,(i*16)+yofs,16,16,stipple);
         //ColorGrid((j*16)+xofs,(i*16)+yofs,7);
      }
    }
}

void DrawZones()
{ int i,j;
  char z;
  int mapx;

  mapx = layer[0].sizex;
  oxw=xwin*layer[0].pmultx/layer[0].pdivx;
  oyw=ywin*layer[0].pmulty/layer[0].pdivy;
  xofs=16-(oxw&15);
  yofs=16-(oyw&15);
  xtc=oxw>>4;
  ytc=oyw>>4;

  for (i=0; i<ty+1; i++)
    for (j=0; j<tx+1; j++)
    {
      z=zone[((ytc+i)*mapx)+(xtc+j)];
      if (z)
      {
        TCCopySprite((j*16)+xofs,(i*16)+yofs,16,16,stipple);
        //ColorGrid((j*16)+xofs,(i*16)+yofs,z);
      }
    }
}

void DrawEntities()
{ int m;

  for (m=0; m<entities; m++)
    if ((entity[m].x>=(xwin/16)) && (entity[m].x<(xwin/16)+22) &&
        (entity[m].y>=(ywin/16)) && (entity[m].y<(ywin/16)+ty+2))
    {
      if (chrs[entity[m].chrindex].frame)
         TCCopySprite((entity[m].x*16)-xwin+16-chrs[entity[m].chrindex].hx,
                      (entity[m].y*16)-ywin+16-chrs[entity[m].chrindex].hy,
                       chrs[entity[m].chrindex].fx,
                       chrs[entity[m].chrindex].fy,
                       chrs[entity[m].chrindex].frame);
      else
      {
         TCCopySprite(
           (entity[m].x*16)-xwin+16,(entity[m].y*16)-ywin+16,16,16,stipple);
        //ColorGrid((entity[m].x*16)-xwin+16,
        //  (entity[m].y*16)-ywin+16, 0);
      }
    }
}

// aen
void DrawSelection()
{ int i,j;
  int x1,y1,x2,y2;

  oxw=xwin*layer[el].pmultx/layer[el].pdivx;
  oyw=ywin*layer[el].pmulty/layer[el].pdivy;
  xofs=16-(oxw&15);
  yofs=16-(oyw&15);
  xtc=oxw>>4;
  ytc=oyw>>4;

  x1=selx1;
  y1=sely1;
  x2=selx2;
  y2=sely2;

  if (x2<x1) x2^=x1,x1^=x2,x2^=x1;
  if (y2<y1) y2^=y1,y1^=y2,y2^=y1;

  for (i=0; i<ty+1; i++)
    for (j=0; j<tx+1; j++)
    {
      if (xtc+j >= x1 && xtc+j <= x2
      && ytc+i >= y1 && ytc+i <= y2)
      {
        TCCopySprite((j*16)+xofs, (i*16)+yofs, 16,16, stipple);
        //ColorGrid((j*16)+xofs,(i*16)+yofs,7);
      }
    }
}

void RenderMap()
{ char *src;
  //char rs[2];

  curlayer=0;
  src=rstring;

  /*
  rs[1]=0; // aen
  for (; *src; ++src)
  {
    rs[0]=*src;
    BlitLayer(atoi(rs) -1);
  }
  */
  while (*src)
  {
    switch (*src)
    {
      case '1': BlitLayer(0); break;
      case '2': BlitLayer(1); break;
      case '3': BlitLayer(2); break;
      case '4': BlitLayer(3); break;
      case '5': BlitLayer(4); break;
      case '6': BlitLayer(5); break;
    }
    src++;
  }

  if (!curlayer) memset(screen+5648,0,84480);
  if (layertoggle[6]) DrawObstructions();
  if (layertoggle[7]) DrawZones();
  if (layertoggle[8]) DrawEntities();
  if (shifted) DrawSelection();
}

void AnimateTile(char i, int l)
{
  switch (vspanim[i].mode)
  {
    case 0: if (tileidx[l]<vspanim[i].finish) tileidx[l]++;
            else tileidx[l]=vspanim[i].start;
            break;
    case 1: if (tileidx[l]>vspanim[i].start) tileidx[l]--;
            else tileidx[l]=vspanim[i].finish;
            break;
    case 2: tileidx[l]=random(vspanim[i].start,vspanim[i].finish);
            break;
    case 3: if (flipped[l])
            {
              if (tileidx[l]!=vspanim[i].start) tileidx[l]--;
              else { tileidx[l]++; flipped[l]=0; }
            }
            else
            {
              if (tileidx[l]!=vspanim[i].finish) tileidx[l]++;
              else { tileidx[l]--; flipped[l]=1; }
            }
  }
}

void Animate(char i)
{ static int l;

  vadelay[i]=0;
  for (l=vspanim[i].start; l<=vspanim[i].finish; l++)
    AnimateTile(i,l);
}

void CheckTileAnimation()
{ static char i;

  for (i=0; i<100; i++)
      { if ((vspanim[i].delay) && (vspanim[i].delay<vadelay[i]))
           Animate(i);
        vadelay[i]++; }
}

