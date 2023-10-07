/* Reconstructed Commander Keen 4-6 Source Code
 * Copyright (C) 2021 K1n9_Duk3
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

#ifndef __AUDIO_H__
#define __AUDIO_H__

//#include "VERSION.H"

/////////////////////////////////////////////////
//
// MUSE Header for .CK4
//
/////////////////////////////////////////////////

#define NUMSOUNDS     LASTSOUND
#define NUMSNDCHUNKS  ((3*LASTSOUND)+LASTMUSIC)

//
// Sound names & indexes
//
typedef enum {
	SND_WORLDWALK1,
	SND_WORLDWALK2,
	SND_JUMP,
	SND_LAND,
	SND_KEENFIRE,
	SND_WORMOUTHATTACK,
	SND_6,
	SND_POGOBOUNCE,
	SND_GETPOINTS,
	SND_GETAMMO,
	SND_GETWATER,
	SND_11,
	SND_ENTERLEVEL,
	SND_LEVELDONE,
	SND_NOWAY,
	SND_HELMETHIT,
	SND_BOUNCE2,
	SND_EXTRAKEEN,
	SND_OPENDOOR,
	SND_GETKEY,
	SND_PLUMMET,
	SND_USESWITCH,
	SND_SQUISH,
	SND_KEENDEAD,
	SND_24,
	SND_SHOTEXPLODE,
	SND_SWIM1,
	SND_SWIM2,
	SND_BOUNCE1,
	SND_EATBONUS,
	SND_TREASUREEATERVANISH,
	SND_LINDSEY,
	SND_LICKATTACK,
	SND_BERKELOIDATTACK,
	SND_SHOWSTATUS,
	SND_HIDESTATUS,
	SND_BLUB,
	SND_MINEEXPLODE,
	SND_SPRITEFIRE,
	SND_THUNDER,
	SND_FIREBALLLAND,
	SND_SHOOTDART,
	SND_BURP,
	SND_FLAGSPIN,
	SND_FLAGLAND,
	SND_MAKEFOOT,
	SND_SLUGPOO,
	KEENPADDLESND,
	BALLBOUNCESND,
	COMPPADDLESND,
	COMPSCOREDSND,
	KEENSCOREDSND,
	LASTSOUND
} soundnames;

#if LASTSOUND != 52
#error bad sound enum!
#endif

#define NOWAYSND SND_NOWAY

//
// Base offsets
//
#define STARTPCSOUNDS     0
#define STARTADLIBSOUNDS  (STARTPCSOUNDS+NUMSOUNDS)
#define STARTDIGISOUNDS   (STARTADLIBSOUNDS+NUMSOUNDS)
#define STARTMUSIC        (STARTDIGISOUNDS+NUMSOUNDS)

//
// Music names & indexes
//
typedef enum {
	SHADOWS_MUS,
	VEGGIES_MUS,
	TOOHOT_MUS,
	OASIS_MUS,
	KICKPANT_MUS,
	WONDER_MUS,
	LASTMUSIC
} musicnames;

/////////////////////////////////////////////////
//
// Thanks for playing with MUSE!
//
/////////////////////////////////////////////////

#endif