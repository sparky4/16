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
// MUSE Header for .CK5
//
/////////////////////////////////////////////////

#define NUMSOUNDS     LASTSOUND
#define NUMSNDCHUNKS  ((3*LASTSOUND)+LASTMUSIC)

//
// Sound names & indexes
//
typedef enum {
	SND_WORLDWALK1,        // 0
	SND_WORLDWALK2,        // 1
	SND_JUMP,              // 2
	SND_LAND,              // 3
	SND_KEENFIRE,          // 4
	SND_MINEEXPLODE,       // 5
	SND_SLICESTARBOUNCE,   // 6
	SND_POGOBOUNCE,        // 7
	SND_GETPOINTS,         // 8
	SND_GETAMMO,           // 9
	SND_GETWATER,          // 10
	SND_11,                // 11
	SND_ENTERLEVEL,        // 12
	SND_LEVELDONE,         // 13
	SND_NOWAY,             // 14
	SND_HELMETHIT,         // 15
	SND_BOUNCE,            // 16
	SND_EXTRAKEEN,         // 17
	SND_OPENCARDDOOR,      // 18
	SND_GETKEY,            // 19
	SND_PLUMMET,           // 20
	SND_USESWITCH,         // 21
	SND_22,                // 22
	SND_KEENDEAD,          // 23
	SND_24,                // 24
	SND_SHOTEXPLODE,       // 25
	SND_26,                // 26
	SND_SPIROGRAB,         // 27
	SND_SPINDREDBOUNCE,    // 28
	SND_ENEMYSHOT,         // 29
	SND_ENEMYSHOTEXPLODE,  // 30
	SND_AMPTONWALK1,       // 31
	SND_AMPTONWALK2,       // 32
	SND_AMPTONDIE,         // 33
	SND_SHOWSTATUS,        // 34
	SND_HIDESTATUS,        // 35
	SND_SHELLEYEXPLODE,    // 36
	SND_SPINDREDFLIP,      // 37
	SND_MASTERATTACK,      // 38
	SND_MASTERBLAST,       // 39
	SND_SHIKADIATTACK,     // 40
	SND_TELEPORT,          // 41
	SND_SHOCKSHUNDBARK,    // 42
	SND_FLAGSPIN,          // 43
	SND_FLAGLAND,          // 44
	SND_SHOCKBALLEXPLODE,  // 45
	KEENPADDLESND,         // 46
	BALLBOUNCESND,         // 47
	COMPPADDLESND,         // 48
	COMPSCOREDSND,         // 49
	KEENSCOREDSND,         // 50
	SND_51,                // 51
	SND_BIGSPARK,          // 52
	SND_GAMEOVER1,         // 53
	SND_GAMEOVER2,         // 54
	SND_GETKEYCARD,        // 55
	SND_56,                // 56
	SND_LANDONFUSE,        // 57
	SND_SPARKYCHARGE,      // 58
	SND_SPHEREFULBOUNCE,   // 59
	SND_OPENDOOR,          // 60
	SND_SPIROFLY,          // 61
	SND_62,                // 62
	SND_ELEVATORDOOR,      // 63
	LASTSOUND
} soundnames;

#if LASTSOUND != 64
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
	CAMEIN_MUS,
	HAVING_T_MUS,
	SKATING_MUS,
	SNOOPING_MUS,
	BAGPIPES_MUS,
	WEDNESDY_MUS,
	ROCK_ME_MUS,
	BREATHE_MUS,
	SHIKAIRE_MUS,
	SPHEREFUL_MUS,
	TIGHTER_MUS,
	ROBOROCK_MUS,
	FANFARE_MUS,
	FEARSOME_MUS,
	LASTMUSIC
} musicnames;

/////////////////////////////////////////////////
//
// Thanks for playing with MUSE!
//
/////////////////////////////////////////////////

#endif