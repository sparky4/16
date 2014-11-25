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

#ifndef VC_H
#define VC_H

typedef struct
{
  char fname[40];
  char argtype[12];
  int numargs;
  int returntype;
  int syscodeofs;
} funcdecl;

extern funcdecl funcs[100];
extern int numfuncs, mapevents;
extern int hookretrace, hooktimer;

typedef struct
{
  char vname[40];
  int varstartofs;
  int arraylen;
} vardecl;

extern vardecl vars[100];
extern int numvars, invc;
extern char *mapvc, kill;
extern char *vcsp, *vcstack;

void LoadSystemVC();
void LoadMapVC(VFILE *f);
void ReadVCVar();
void WriteVCVar();
void ExecuteEvent(int);
void ExecuteUserFunc(int);

void HookRetrace();
void HookTimer();
void HookKey(int script);

#endif
