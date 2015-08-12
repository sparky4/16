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

#ifndef VDRIVER_INC
#define VDRIVER_INC

#include "inter.h"

extern int (*ShutdownVideo) (void);
extern int (*ShowPage) (void);
extern int (*CopySprite) (int x, int y, int width, int height, char *src);
extern int (*CCopySprite) (int x, int y, int width, int height, char *src);
extern int (*TCCopySprite) (int x, int y, int width, int height, char *src);
extern int (*TCopySprite) (int x, int y, int width, int height, char *src);
extern int (*CopySpriteLucentClip) (int x, int y, int width, int height, unsigned char *src);
extern int (*CopyTile) (int x, int y, char *src);
extern int (*FilledBox) (int x, int y, int width, int height, char c);
extern int (*VLine) (int x, int y, int height, char color);
extern int (*HLine) (int x, int y, int width, char color);
extern int (*ColorGrid) (int x, int y, char c);
extern int (*ClearScreen) (void);

extern unsigned char pal[768], pal2[768];
extern unsigned char *screen;

extern int sx, sy, tx, ty, tsx, tsy;
extern int map_scroll_x, map_scroll_y;

extern void InitVideo(int mode);
extern void SetPalette(unsigned char *pall);
extern void GetPalette(void);
extern void set_intensity(unsigned int n);

extern int cx1, cy1, cx2, cy2;

#endif // VDRIVER_INC
