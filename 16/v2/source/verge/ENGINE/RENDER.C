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

#include "verge.h"
#include <math.h>

// ================================= Data ====================================

int oxw, oyw;
int xofs, yofs;
int xtc, ytc;

byte curlayer, animate=0;
byte cameratracking=1, tracker=0;
byte showobs=0, showzone=0;

// ================================= Code ====================================

void BlitBackLayer(byte l)
{
  int i,j,c;
  byte *img;

  if (!layertoggle[l]) return;

  // Adjust view

  oxw=xwin*layer[l].pmultx/layer[l].pdivx;
  oyw=ywin*layer[l].pmulty/layer[l].pdivy;
  xofs=-(oxw&15); //(16-(oxw&15));
  yofs=-(oyw&15); //(16-(oyw&15));
  xtc=oxw>>4;
  ytc=oyw>>4;

  // Draw it, bitch!

  for (i=0; i<ty+1; i++)
    for (j=0; j<tx+1; j++)
    {
      c=layers[l][(((ytc+i)*layer[l].sizex)+xtc+j)];
      c=tileidx[c];
      c=c<numtiles?c:0;
      img=vsp+(256*c);
      CopyTileClip((j*16)+xofs,(i*16)+yofs,img);
    }
  curlayer++;
}

void TransBlitLayer(byte l)
{
  int i,j,offset,c;
  byte *img,*mask;

  if (!layertoggle[l]) return;

  // Adjust view

  oxw=xwin*layer[l].pmultx/layer[l].pdivx;
  oyw=ywin*layer[l].pmulty/layer[l].pdivy;
  xofs=-(oxw&15);
  yofs=-(oyw&15);
  xtc=oxw>>4;
  ytc=oyw>>4;

  // Draw it, bitch!

  for (i=0; i<ty+1; i++)
    for (j=0; j<tx+1; j++)
    {
      c=tileidx[layers[l][(((ytc+i)*layer[l].sizex)+xtc+j)]];
      c=c<numtiles?c:0;
      offset=c*256;
      img=vsp+offset; mask=vspmask+offset;
      if (img!=vsp)
        TCopyTileClip((j*16)+xofs,(i*16)+yofs,img,mask);
    }
  curlayer++;
}

void BlitLayerLucent(byte l)
{
  int i,j,offset,c;
  byte *img,*mask;

  if (!layertoggle[l]) return;

  // Adjust view

  oxw=xwin*layer[l].pmultx/layer[l].pdivx;
  oyw=ywin*layer[l].pmulty/layer[l].pdivy;
  xofs=-(oxw&15);
  yofs=-(oyw&15);
  xtc=oxw>>4;
  ytc=oyw>>4;

  // Draw it, bitch!

  for (i=0; i<ty+1; i++)
    for (j=0; j<tx+1; j++)
    {
      c=tileidx[layers[l][(((ytc+i)*layer[l].sizex)+xtc+j)]];
      c=c<numtiles?c:0;
      offset=c*256;
      img=vsp+offset; mask=vspmask+offset;
      if (img!=vsp)
        TCopyTileLucentClip((j*16)+xofs,(i*16)+yofs,img);
    }
  curlayer++;
}

void BlitLayer(byte c)
{
  if ( curlayer && !layer[c].hline && !layer[c].trans) { TransBlitLayer(c); return; }
  if (!curlayer && !layer[c].hline && !layer[c].trans) { BlitBackLayer(c); return; }
  if (layer[c].trans) { BlitLayerLucent(c); return; }
  if (layer[c].hline) { ExecuteEvent(layer[c].hline); return; }
}

void DrawObstructions()
{
  int i,j;

  oxw=xwin*layer[0].pmultx/layer[0].pdivx;
  oyw=ywin*layer[0].pmulty/layer[0].pdivy;
  xofs=-(oxw&15);
  yofs=-(oyw&15);
  xtc=oxw/16;
  ytc=oyw/16;

  for (i=0; i<ty+1; i++)
    for (j=0; j<tx+1; j++)
    {
      if (obstruct[((ytc+i)*layer[0].sizex)+(xtc+j)])
        ColorField((j*16)+xofs,(i*16)+yofs,7);
    }
}

void DrawZones()
{
  int i,j;
  char z;

  oxw=xwin*layer[0].pmultx/layer[0].pdivx;
  oyw=ywin*layer[0].pmulty/layer[0].pdivy;
  xofs=-(oxw&15);
  yofs=-(oyw&15);
  xtc=oxw>>4;
  ytc=oyw>>4;

  for (i=0; i<ty+1; i++)
    for (j=0; j<tx+1; j++)
    {
      z=zone[((ytc+i)*layer[0].sizex)+(xtc+j)];
      if (z)
      {
        ColorField((j*16)+xofs,(i*16)+yofs,z);
      }
      if (player)
         {
           if ( ((player->x+7)/16)==(xtc+j) && ((player->y+7)/16)==(ytc+i))
           {
              ColorField((j*16)+xofs,(i*16)+yofs,31);
           }
         }
    }
}

void HookScriptThing(char *src)
{
  char mystr[10];

  src++;
  strcpy(mystr, src);
  src=mystr;
  while (*src != 'X') src++;
  *src=0;
  ExecuteEvent(atoi(mystr));
}

void RenderMAP()
{ char *src;

  curlayer=0;
  src=rstring;
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
      case 'E': RenderEntities(); break;
      case 'S': HookScriptThing(src); break;
      case 'R': HookRetrace();
                curlayer++; break;
    }
    src++;
  }
   if (!curlayer) ClearScreen();
   if (showobs) DrawObstructions();
   if (showzone) DrawZones();
}

void Render()
{
  if (cameratracking==1 && player)
  {
    if (player->x+8>(sx/2)) xwin=(player->x+8-(sx/2)); else xwin=0;
    if (player->y+8>(sy/2)) ywin=(player->y+8-(sy/2)); else ywin=0;
    if (xwin>((layer[0].sizex*16)-sx)) xwin=((layer[0].sizex*16)-sx);
    if (ywin>((layer[0].sizey*16)-sy)) ywin=((layer[0].sizey*16)-sy);
  }
  if (cameratracking==2)
  {
    if (entity[tracker].x+8>(sx/2)) xwin=(entity[tracker].x+8-(sx/2)); else xwin=0;
    if (entity[tracker].y+8>(sy/2)) ywin=(entity[tracker].y+8-(sy/2)); else ywin=0;
    if (xwin>((layer[0].sizex*16)-sx)) xwin=((layer[0].sizex*16)-sx);
    if (ywin>((layer[0].sizey*16)-sy)) ywin=((layer[0].sizey*16)-sy);
  }
  SiftEntities();
  RenderMAP();
}

int rnd(int lo, int hi)
{
  int range=hi-lo+1;
  int i=rand() % range;
  return i+lo;
}

void AnimateTile(byte i, int l)
{
  switch (vspanim[i].mode)
  {
    case 0: if (tileidx[l]<vspanim[i].finish) tileidx[l]++;
            else tileidx[l]=vspanim[i].start;
            break;
    case 1: if (tileidx[l]>vspanim[i].start) tileidx[l]--;
            else tileidx[l]=vspanim[i].finish;
            break;
    case 2: tileidx[l]=rnd(vspanim[i].start,vspanim[i].finish);
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

void Animate(byte i)
{ static int l;

  vadelay[i]=0;
  for (l=vspanim[i].start; l<=vspanim[i].finish; l++)
    AnimateTile(i,l);
}

void CheckTileAnimation()
{ static byte i;

  if (!animate) return;
  if (!vsp) return;
  for (i=0; i<100; i++)
  {
    if ((vspanim[i].delay) && (vspanim[i].delay<vadelay[i]))
       Animate(i);
    vadelay[i]++;
  }
}
