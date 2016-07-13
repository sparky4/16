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

#ifndef MODEX_INC
#define MODEX_INC

extern int  ModeXShutdown();
extern int  ModeXShowPage();
extern int  ModeXCopySprite(int x, int y, int width, int height, char *src);
extern int  ModeXCCopySprite(int x,int y,int width,int height,char *src);
extern int  ModeXTCCopySprite(int x,int y,int width,int height,char *src);
extern int  ModeXTCopySprite(int x, int y, int width, int height, char *src);
extern int  ModeXCopyTile(int x, int y, char *src);
extern int  ModeXVLine(int x, int y, int length, char color);
extern int  ModeXHLine(int x, int y, int width, char color);
extern int  ModeXFilledBox(int x, int y, int width, int height, char c);
extern int  ModeXColorGrid(int x, int y, char c);
extern int  ModeXClearScreen();
extern int  ModeXCopySpriteLucentClip(int x, int y, int width, int height, unsigned char *src);
extern void InitModeX();

#endif // MODEX_INC
