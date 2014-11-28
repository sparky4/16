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

#ifndef FONT_H
#define FONT_H

struct fontstruct
{
  byte width, height;                  // Font character dimensions
  byte *data;                          // Font bitmap data
};

extern struct fontstruct font[10];      // Font stuff;
extern int numfonts;                    // number of loaded fonts.
extern int curx, cury;

extern int LoadFont(char *fname);
extern void TextColor(int c);
extern void NumColor(int c);
extern void GotoXY(int x, int y);
extern void printstring(int fnt, char *str);
extern void dec_to_asciiz(int num, char *buf);

#endif // FONT_H
