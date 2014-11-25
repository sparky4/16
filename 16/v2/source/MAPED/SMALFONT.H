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

#ifndef SMALFONT_INC
#define SMALFONT_INC

extern int fontx;
extern int fonty;

extern void GotoXY(int x1, int y1);
extern void print_char(char c);
extern void printstring(char *str);
extern int  pixels(char *str);
extern void dec_to_asciiz(int num, char *buf);
extern void TextColor(unsigned char newc);

#endif // SMALFONT_INC
