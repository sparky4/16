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

#ifndef VDRIVER_H
#define VDRIVER_H

extern byte *screen,*video,*vscreen;
extern byte pal[768], pal2[768];
extern byte *translucency_table;

extern int sx, sy;
extern int tx, ty;
extern int tsx, tsy;
extern int cx1,cy1,cx2,cy2;

extern char *DriverDesc;

extern int InitVideo(int xres, int yres);
extern void VidInfo(void);
extern void SetPalette(byte *pall);
extern void GetPalette(void);
extern void set_intensity(unsigned int n);

// driver interface

extern int (*ShutdownVideo)(int i);
extern int (*ShowPage)(void);

// LFB routines for all driver functions

extern int  LFB_ShowPage(void);
extern void ColorField(int x, int y, byte c);
extern void ClearScreen(void);
extern void MapLine(int x, int y, int yofs, word *map);
extern void TMapLine(int x, int y, int yofs, word *map);

extern void CopySprite(int x, int y, int width, int height, byte *src);
extern void TCopySprite(int x, int y, int width, int height, byte *src);
extern void CopySpriteZoom(int x, int y, int width, int height, int dw, int dh, byte *src);
extern void TCopySpriteZoom(int x, int y, int width, int height, int dw, int dh, byte *src);
extern void CopySpriteLucent(int x, int y, int width, int height, byte *src);
extern void TCopySpriteLucent(int x, int y, int width, int height, byte *src);
extern void CopySpriteZoomLucent(int x, int y, int width, int height, int dw, int dh, byte *src);
extern void TCopySpriteZoomLucent(int x, int y, int width, int height, int dw, int dh, byte *src);
extern void Silhouette(int x, int y, int width, int height, int color, byte *src);
extern void SilhouetteZoom(int x, int y, int sw, int sh, int dw, int dh, int color, byte *src);
extern void SilhouetteLucent(int x, int y, int width, int height, int color, byte *src);
extern void SilhouetteZoomLucent(int x, int y, int sw, int sh, int dw, int dh, int color, byte *src);
extern void Tint(int x, int y, int width, int height, int color, byte *src);
extern void TintZoom(int x, int y, int sw, int sh, int dw, int dh, int color, byte *src);

extern void CopySpriteClip(int x, int y, int width, int height, byte *src);
extern void TCopySpriteClip(int x, int y, int width, int height, byte *src);
extern void CopySpriteZoomClip(int x, int y, int width, int height, int dw, int dh, byte *src);
extern void TCopySpriteZoomClip(int x, int y, int width, int height, int dw, int dh, byte *src);
extern void CopySpriteLucentClip(int x, int y, int width, int height, byte *src);
extern void TCopySpriteLucentClip(int x, int y, int width, int height, byte *src);
extern void CopySpriteZoomLucentClip(int x, int y, int width, int height, int dw, int dh, byte *src);
extern void TCopySpriteZoomLucentClip(int x, int y, int width, int height, int dw, int dh, byte *src);
extern void SilhouetteClip(int x, int y, int width, int height, int color, byte *src);
extern void SilhouetteZoomClip(int x, int y, int sw, int sh, int dw, int dh, int color, byte *src);
extern void SilhouetteLucentClip(int x, int y, int width, int height, int color, byte *src);
extern void SilhouetteZoomLucentClip(int x, int y, int sw, int sh, int dw, int dh, int color, byte *src);
extern void TintClip(int x, int y, int width, int height, int color, byte *src);
extern void TintZoomClip(int x, int y, int sw, int sh, int dw, int dh, int color, byte *src);

extern void CopyTile(int x, int y, byte *src);
extern void TCopyTile(int x, int y, byte *spr, byte *matte);
extern void CopyTileLucent(int x, int y, byte *src);
extern void TCopyTileLucent(int x, int y, byte *spr);

extern void CopyTileClip(int x, int y, byte *src);
extern void TCopyTileClip(int x, int y, byte *spr, byte *matte);
extern void CopyTileLucentClip(int x, int y, byte *src);
extern void TCopyTileLucentClip(int x, int y, byte *spr);

extern int  GetPixel(int x, int y);
extern void SetPixel(int x, int y, int color);
extern void SetPixelLucent(int x, int y, int color);
extern void HLine(int x, int y, int x2, int color);
extern void HLineLucent(int x, int y, int x2, int color);
extern void RectFill(int x, int y, int x2, int y2, int color);
extern void RectFillLucent(int x, int y, int x2, int y2, int color);
extern void VLine(int x, int y, int y2, int color);
extern void VLineLucent(int x, int y, int y2, int color);
extern void Rect(int x, int y, int x2, int y2, int color);
extern void RectLucent(int x, int y, int x2, int y2, int color);
extern void Line(int x, int y, int x2, int y2, int color);
extern void LineLucent(int x, int y, int x2, int y2, int color);
extern void CircleFill(int x, int y, int radius, int color);
extern void CircleFillLucent(int x, int y, int radius, int color);
extern void Circle(int x, int y, int radius, int color);
extern void CircleLucent(int x, int y, int radius, int color);

extern int  GetPixelClip(int x, int y);
extern void SetPixelClip(int x, int y, int color);
extern void SetPixelLucentClip(int x, int y, int color);
extern void HLineClip(int x, int y, int x2, int color);
extern void HLineLucentClip(int x, int y, int x2, int color);
extern void RectFillClip(int x, int y, int x2, int y2, int color);
extern void RectFillLucentClip(int x, int y, int x2, int y2, int color);
extern void VLineClip(int x, int y, int y2, int color);
extern void VLineLucentClip(int x, int y, int y2, int color);
extern void RectClip(int x, int y, int x2, int y2, int color);
extern void RectLucentClip(int x, int y, int x2, int y2, int color);
extern void LineClip(int x, int y, int x2, int y2, int color);
extern void LineLucentClip(int x, int y, int x2, int y2, int color);
extern void CircleFillClip(int x, int y, int radius, int color);
extern void CircleFillLucentClip(int x, int y, int radius, int color);
extern void CircleClip(int x, int y, int radius, int color);
extern void CircleLucentClip(int x, int y, int radius, int color);

extern void WrapBlit(quad x, quad y, int wide, int high, byte *src);
extern void WrapBlitMasked(quad x, quad y, int wide, int high, byte *src);
extern void WrapBlitLucent(quad x, quad y, int wide, int high, byte *src);
extern void WrapBlitLucentMasked(quad x, quad y, int wide, int high, byte *src);

void RotScale(int posx, int posy, quad width, quad height, float angle, float scale, byte *src);
byte *InitMosaicTable(void);
void Mosaic(quad xlevel, quad ylevel, byte *tbl, quad xmin, quad ymin, quad xmax, quad ymax);

void TMapPoly(int x1, int y1, int x2, int y2, int x3, int y3,
              int tx1, int ty1, int tx2, int ty2, int tx3, int ty3,
              int tw, int th, char *img);
void FlatPoly(int x1, int y1, int x2, int y2, int x3, int y3, char color);

#endif // VDRIVER_H
