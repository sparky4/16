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

#ifndef RENDER_INC
#define RENDER_INC

#define MAXTILES 10240

extern int oxw, oyw;
extern int xofs, yofs;
extern int xtc, ytc;

extern char curlayer;
extern short tileidx[MAXTILES];
extern char flipped[MAXTILES];

extern short tileidx[MAXTILES];

typedef struct
{
  char *frame;
  int fx, fy;
  int hx, hy;
} chri;

extern chri chrs[100];

extern void DoCHR(int i);
extern void DoCHRdealy();
extern void BlitBackLayer(char l);
extern void LucentBlitLayer(char l);
extern void TransBlitLayer(char l);
extern void TransBlitLayer_(char l);
extern void BlitLayer(char c);
extern void DrawObstructions();
extern void DrawZones();
extern void DrawEntities();
extern void DrawSelection();
extern void RenderMap();
extern void AnimateTile(char i, int l);
extern void Animate(char i);
extern void CheckTileAnimation();

#endif // RENDER_INC
