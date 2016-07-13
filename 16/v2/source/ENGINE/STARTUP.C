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

#include <stdarg.h> // va_*()
#include <time.h>

#define MAIN_H
#include "verge.h"
extern void vmain(int argc, char *argv[]);
extern void Log(char *message, ...);

int mx, my, mb;

// ================================= Code ====================================

void err(char *message, ...)
{
  char tempbuf[256];
  va_list lst;

  ShutdownVideo(1);
  ShutdownTimer();
  ShutdownKeyboard();
//  CD_Deinit();
  MD_PlayStop();
  MD_Exit();

  va_start(lst, message);
  vsprintf(tempbuf, message, lst);
  va_end(lst);

  printf(tempbuf);
  fflush(stdout);

  Log("Sys: Exiting with message: %s", tempbuf);

  exit(-1);
}

int sgn(int x)
{
  if (x>=0) return 1;
  return 0;
}

void ver()
{ char temp[256];

  Con_NextLine();
  Con_Printf("{|||||||||||||||||}");
  sprintf(temp,"VERGE v.%s Copyright (C)1998 vecna",VERSION);
  Con_Printf(temp);
  Con_Printf("All rights reserved. DJGPP/DOS build.");
  sprintf(strbuf,"Timestamp %s at %s.",__DATE__, __TIME__);
  Con_Printf(strbuf);
  Con_Printf("Options: -Wall -m486 -O1 -ffast-math");
  Con_Printf("{|||||||||||||||||}");
}

void CheckMessages()
{
  // Win95 can bite me.
}

void InitMouse(int x, int y)
{ REGISTERS r;

  SET_AX(r,7);
  SET_CX(r,0);
  SET_DX(r,x);
  INTERRUPT(0x33, r);

  SET_AX(r,8);
  SET_CX(r,0);
  SET_DX(r,y);
  INTERRUPT(0x33, r);
}

void ReadMouse()
{ REGISTERS r;

  SET_AX(r,3);
  INTERRUPT(0x33, r);

  mx=r.x.cx;
  my=r.x.dx;
  mb=r.x.bx;
}

void SetMouse(int x, int y)
{ REGISTERS r;

  SET_AX(r, 4);
  SET_CX(r, x);
  SET_DX(r, y);
  INTERRUPT(0x33, r);
}

int main(int argc, char *argv[])
{
  if (argc==1)
  {
    printf("VERGE v.%s build %s at %s. \n",VERSION,__DATE__,__TIME__);
    printf("Copyright (C)1998 vecna \n");
    delay(500);
  }
  srand(time(NULL));
  vmain(argc, argv);
  return 0;
}
