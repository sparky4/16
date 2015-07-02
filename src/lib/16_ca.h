/* Catacomb Apocalypse Source Code
 * Copyright (C) 1993-2014 Flat Rock Software
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

// ID_CA.H
#ifndef __16_CA__
#define __16_CA__

#include <stdio.h>
#include <stdlib.h>

#ifndef __16_MM__
#include "src/lib/16_head.h"
#include "src/lib/16_mm.h"
#endif

//===========================================================================

//#define NOMAPS
//#define NOGRAPHICS
//#define NOAUDIO

//#define MAPHEADERLINKED
//#define GRHEADERLINKED
//#define AUDIOHEADERLINKED

//#define NUMMAPS		39
//#define MAPPLANES		3

//===========================================================================

/*typedef	struct
{
	long		planestart[3];
	unsigned	planelength[3];
	unsigned	width,height;
	char		name[16];
} maptype;*/

//===========================================================================

/*extern	byte 		_seg	*tinf;
extern	int			mapon;

extern	unsigned	_seg	*mapsegs[3];
extern	maptype		_seg	*mapheaderseg[NUMMAPS];
extern	byte		_seg	*audiosegs[NUMSNDCHUNKS];
extern	void		_seg	*grsegs[NUMCHUNKS];

extern	byte		far	grneeded[NUMCHUNKS];
extern	byte		ca_levelbit,ca_levelnum;

extern	char		*titleptr[8];

extern	int			profilehandle,debughandle;*/

//
// hooks for custom cache dialogs
//
extern	void	(*drawcachebox)		(char *title, unsigned numcache);
extern	void	(*updatecachebox)	(void);
extern	void	(*finishcachebox)	(void);

//===========================================================================

// just for the score box reshifting

//void CAL_ShiftSprite (unsigned segment,unsigned source,unsigned dest,unsigned width, unsigned height, unsigned pixshift, boolean domask);

//===========================================================================

void CA_OpenDebug (void);
void CA_CloseDebug (void);
boolean CA_FarRead (int handle, byte far *dest, dword length);
boolean CA_FarWrite (int handle, byte far *source, dword length);
boolean CA_ReadFile (char *filename, memptr *ptr);
boolean CA_LoadFile (char *filename, memptr *ptr);

//long CA_RLEWCompress (unsigned huge *source, long length, unsigned huge *dest,unsigned rlewtag);

//void CA_RLEWexpand (unsigned huge *source, unsigned huge *dest,long length,unsigned rlewtag);

void CA_Startup (void);
void CA_Shutdown (void);

//void CA_CacheAudioChunk (int chunk);
//void CA_LoadAllSounds (void);

/*void CA_UpLevel (void);
void CA_DownLevel (void);

void CA_SetAllPurge (void);

void CA_ClearMarks (void);
void CA_ClearAllMarks (void);

#define CA_MarkGrChunk(chunk)	grneeded[chunk]|=ca_levelbit

void CA_CacheGrChunk (int chunk);
void CA_CacheMap (int mapnum);

void CA_CacheMarks (char *title);*/
#endif
