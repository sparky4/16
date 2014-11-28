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

// Additional code by Richard Lau (aka Ric)

#include "config.h"    // Colour definitions
#include "vdriver.h"   // Video driver functions
#include "smalfont.h"

// ================================= Code ====================================

void stdwindow(int x1, int y1, int x2, int y2)
{
  FilledBox(x1, y1, x2-x1, y2-y1, winbg);
  HLine(x1, y1, x2-x1, black);     /* -- ric: 13/Jun/98 --     */
  VLine(x1, y1, y2-y1, black);     /* Outline window in black */
  HLine(x1+1, y2-1, x2-x1-1, black);
  VLine(x2-1, y1+1, y2-y1-1, black);

  HLine(x1+1, y1+1, x2-x1-2, brightw);
  VLine(x1+1, y1+1, y2-y1-2, brightw);
  HLine(x1+2, y2-2, x2-x1-3, darkw);
  VLine(x2-2, y1+2, y2-y1-3, darkw);
}

void Window(int x1, int y1, int x2, int y2, char *title)
{
  FilledBox(x1, y1, x2-x1, y2-y1, winbg);
  FilledBox(x1, y1, x2-x1, 8, titlebg);
  HLine(x1, y1, x2-x1, brightw);
  VLine(x1, y1, y2-y1, brightw);
  HLine(x1+1, y2-1, x2-x1-1, darkw);
  VLine(x2-1, y1+1, y2-y1-1, darkw);
  FilledBox(x2-8, y1+2, 5, 5, winbg);
  HLine(x2-8, y1+2, 6, brightw);
  VLine(x2-8, y1+2, 5, brightw);
  HLine(x2-7, y1+6, 4, darkw);
  VLine(x2-3, y1+3, 4, darkw);

  GotoXY(x1+2, y1+2);
  printstring(title);
}

void CheckBox(int x, int y, char checked)
{
  FilledBox(x, y, 7, 6, 0);
  HLine(x, y, 8, darkw);
  VLine(x, y, 7, darkw);
  HLine(x+1, y+6, 7, brightw);
  VLine(x+7, y+1, 5, brightw);
  if (checked)
  {
    GotoXY(x+2, y+2);
    printstring("*");
  }
}

void Button(int x, int y, char *str)
{
  stdwindow(x,y,x+30,y+10);
//  HLine(x, y, 30, black);      /* -- ric: 13/Jun/98 --     */
//  VLine(x, y, 10, black);      /* Outline buttons in black */
/*  HLine(x+1, y+9, 29, black);
  VLine(x+29, y+1, 9, black);

  HLine(x+1, y+1, 28, white);
  VLine(x+1, y+1, 8, white);
  HLine(x+2, y+8, 27, darkw);
  VLine(x+28, y+2, 7, darkw); */
  GotoXY(x+(15-(pixels(str)/2)), y+2);
  printstring(str);
}

void BigButton(int x, int y, char *str)
{
  stdwindow(x,y,x+50,y+10);
//  HLine(x, y, 50, black);      /* -- ric: 13/Jun/98 --     */
//  VLine(x, y, 10, black);      /* Outline buttons in black */
/*  HLine(x+1, y+9, 49, black);
  VLine(x+49, y+1, 9, black);

  HLine(x+1, y+1, 48, white);
  VLine(x+1, y+1, 8, white);
  HLine(x+2, y+8, 47, darkw);
  VLine(x+48, y+2, 7, darkw); */
  GotoXY(x+(25-(pixels(str)/2)), y+2);
  printstring(str);
}

void ButtonPressed(int x, int y, char *str)
/* -- ric: 13/Jun/98 --     */
/* Draws a depressed button */
{
  stdwindow(x,y,x+30,y+10);
  HLine(x+1, y+1, 28, darkw);
  VLine(x+1, y+1, 8, darkw);
  HLine(x+2, y+8, 27, winbg);
  VLine(x+28, y+2, 7, winbg);
  GotoXY(x+1+(15-(pixels(str)/2)), y+3);
  printstring(str);
}

void BigButtonPressed(int x, int y, char *str)
{
  stdwindow(x,y,x+50,y+10);
  HLine(x+1, y+1, 48, darkw);
  VLine(x+1, y+1, 8, darkw);
  HLine(x+2, y+8, 47, winbg);
  VLine(x+48, y+2, 7, winbg);
  GotoXY(x+1+(25-(pixels(str)/2)), y+3);
  printstring(str);
}

void TextField(int x, int y, int width, char *str, char blink)
{
  FilledBox(x, y, width, 9, 0);
  HLine(x, y, width+1, darkw);
  VLine(x, y, 9, darkw);
  HLine(x+1, y+8, width-1, brightw);
  VLine(x+width, y+1, 8, brightw);
  GotoXY(x+2, y+2);
  printstring(str);
  if (blink) printstring("_");
}
