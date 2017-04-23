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

#ifndef __16_MM__
#include "src/lib/16_head.h"
#include "src/lib/16_mm.h"
#endif

//===========================================================================

/*moved to src/lib/16_tdef.h
#define NOMAPS
#define NOGRAPHICS
#define NOAUDIO

#define MAPHEADERLINKED
//#define GRHEADERLINKED
#define AUDIOHEADERLINKED

#define NUMMAPS		39
#define MAPPLANES		3*/
//++++#define PROFILE//++++

//===========================================================================

/*typedef struct
{
	unsigned	RLEWtag;
	long		headeroffsets[100];
	byte		tileinfo[];
} mapfiletype;

typedef	struct
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

extern	char		*titleptr[8];*/

//extern

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

void CA_OpenDebug (global_game_variables_t *gvar);
void CA_CloseDebug (global_game_variables_t *gvar);
boolean CA_FarRead (int handle, byte far *dest, dword length, global_game_variables_t *gvar);
boolean CA_FarWrite (int handle, byte far *source, dword length, global_game_variables_t *gvar);

boolean CA_ReadFile (char *filename, memptr *ptr, global_game_variables_t *gvar);
boolean CA_WriteFile (char *filename, void far *ptr, long length, global_game_variables_t *gvar);
boolean CA_LoadFile (char *filename, memptr *ptr, global_game_variables_t *gvar);

long CA_RLEWCompress (unsigned far *source, long length, unsigned far *dest,unsigned rlewtag);

void CA_RLEWexpand (unsigned far *source, unsigned far *dest,long length,unsigned rlewtag);

void CA_Startup (global_game_variables_t *gvar);
void CA_Shutdown (global_game_variables_t *gvar);

//void CA_CacheAudioChunk (int chunk);
//void CA_LoadAllSounds (void);

/*void CA_UpLevel (void);
void CA_DownLevel (void);*/

void CA_SetGrPurge (global_game_variables_t *gvar);
void CA_SetAllPurge (global_game_variables_t *gvar);

void CA_ClearMarks (global_game_variables_t *gvar);
void CA_ClearAllMarks (global_game_variables_t *gvar);

/*#define CA_MarkGrChunk(chunk)	grneeded[chunk]|=ca_levelbit

void CA_CacheGrChunk (int chunk);
void CA_CacheMap (int mapnum);

void CA_CacheMarks (char *title);*/
#endif
