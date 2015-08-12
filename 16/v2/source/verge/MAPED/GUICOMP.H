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

#ifndef GUICOMP_INC
#define GUICOMP_INC

extern void stdwindow(int x1, int y1, int x2, int y2);
extern void Window(int x1, int y1, int x2, int y2, char *title);
extern void CheckBox(int x, int y, char checked);
extern void Button(int x, int y, char *str);
extern void BigButton(int x, int y, char *str);
extern void ButtonPressed(int x, int y, char *str);
extern void BigButtonPressed(int x, int y, char *str);
extern void TextField(int x, int y, int width, char *str, char blink);

#endif // GUICOMP_INC
