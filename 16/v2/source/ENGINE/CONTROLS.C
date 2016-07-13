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

// ================================= Data ====================================

// -- Hardware Dependent -- //

int kb1, kb2, kb3, kb4;             // keyboard controls
int jb1, jb2, jb3, jb4;             // joystick controls

// -- Unified Interface -- //

char b1, b2, b3, b4;
char up, down, left, right;

// -- UnPress support variables -- //

char wb1, wb2, wb3, wb4;
char wup, wdown, wleft, wright;

char rb1, rb2, rb3, rb4;
char rup, rdown, rleft, rright;

// ================================= Code ====================================

void UpdateControls()
{
  rb1=0; rb2=0; rb3=0; rb4=0;
  rup=0; rdown=0; rleft=0; rright=0;

  if (key[SCAN_UP]) rup=1;
  if (key[SCAN_DOWN]) rdown=1;
  if (key[SCAN_RIGHT]) rright=1;
  if (key[SCAN_LEFT]) rleft=1;

  if (key[kb1]) rb1=1;
  if (key[kb2]) rb2=1;
  if (key[kb3]) rb3=1;
  if (key[kb4]) rb4=1;

       if (wb1 && rb1) rb1=0;
  else if (wb1 && !rb1) wb1=0;
  b1=rb1;

       if (wb2 && rb2) rb2=0;
  else if (wb2 && !rb2) wb2=0;
  b2=rb2;

       if (wb3 && rb3) rb3=0;
  else if (wb3 && !rb3) wb3=0;
  b3=rb3;

       if (wb4 && rb4) rb4=0;
  else if (wb4 && !rb4) wb4=0;
  b4=rb4;

       if (wup && rup) rup=0;
  else if (wup && !rup) wup=0;
  up=rup;

       if (wdown && rdown) rdown=0;
  else if (wdown && !rdown) wdown=0;
  down=rdown;

       if (wleft && rleft) rleft=0;
  else if (wleft && !rleft) wleft=0;
  left=rleft;

       if (wright && rright) rright=0;
  else if (wright && !rright) wright=0;
  right=rright;
}

void UnPress(int control)
{
  switch (control)
  {
    case 0: wb1=1; wb2=1; wb3=1; wb4=1;
            b1=0; b2=0; b3=0; b4=0;
            key[kb1]=0; key[kb2]=0; key[kb3]=0; key[kb4]=0; break;
    case 1: wb1=1; b1=0; key[kb1]=0; break;
    case 2: wb2=1; b2=0; key[kb2]=0; break;
    case 3: wb3=1; b3=0; key[kb3]=0; break;
    case 4: wb4=1; b4=0; key[kb4]=0; break;
    case 5: wup=1; up=0; key[SCAN_UP]=0; break;
    case 6: wdown=1; down=0; key[SCAN_DOWN]=0; break;
    case 7: wleft=1; left=0; key[SCAN_LEFT]=0; break;
    case 8: wright=1; right=0; key[SCAN_RIGHT]=0; break;
  }
}
