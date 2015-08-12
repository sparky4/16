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

#ifndef TILEED_INC
#define TILEED_INC

// TEtype defs
#define TEUNKNOWN 0
#define TETILE    1

// external functions
extern void TileEdit(void);

// external variables
extern unsigned int TEtype;    // type of graphic being edited
extern unsigned int TEtile; // tile to edit - may possibly expand this to an array for simultaneous editing
extern unsigned int TExsize;   // width of graphic being edited
extern unsigned int TEysize;   // height of graphic being edited
extern unsigned char *TEsource; // pointer to original image being edited

#endif
