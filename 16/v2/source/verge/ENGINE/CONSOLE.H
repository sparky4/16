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

#ifndef CONSOLE_H
#define CONSOLE_H

#include "main.h"

// -- externs --

extern byte *consolebg;                   // Console background image
extern byte *consoletext;                 // Console text buffer
extern byte *cmd, *cmd2, cmdlen;          // command line buffer
extern char *lastcmds, numcmds, cmdpos;   // last-command memory
extern byte *args[10], numargs;           // command argument pointers
extern char cursor, more;                 // flag on/off cursor visible
extern int cswtime;                       // cursor switch time.
extern int conlines;                      // Number of visible lines
extern int lines;                         // number of lines entered since last draw
extern char startln;                      // Start display ofs (for scrollback)
extern char allowconsole;
extern char consoleoverride;

extern byte key_ascii_tbl[128];
extern byte key_shift_tbl[128];

extern char *concmds[];

// -- prototypes --

extern void InitConsole(void);
extern void Con_Printf(char *str);
extern void Con_NextLine(void);

extern int ShowConsole(void);
extern int HideConsole(void);

extern void DrawConsole(void);
extern int Args(int num);
extern void ListCmds(void);
extern void LastCmds(void);
extern void ExecuteCommand(int i);
extern void ParseCommand(void);
extern void ProcessCommand(void);
extern void CommandInput(void);
extern void ActivateConsole(void);

#endif // CONSOLE_H
