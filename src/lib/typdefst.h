/* Project 16 Source Code~
 * Copyright (C) 2012-2015 sparky4 & pngwen & andrius4669
 *
 * This file is part of Project 16.
 *
 * Project 16 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Project 16 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 * write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef _TYPEDEFSTRUCT_H_
#define _TYPEDEFSTRUCT_H_

#include "src/lib/types.h"


#define AARED		"\x1b[41;31m"
#define AABLUE		"\x1b[44;34m"
#define AAGREEN	"\x1b[42;32m"
#define AAYELLOW	"\x1b[43;33m"
#define AAGREY		"\x1b[47;37m"
#define AABLACK	"\x1b[40;30m"
#define AAWHITE		"\x1b[47;37m"
#define AAMAGENTA	"\x1b[45;35m"
#define AARESET	"\x1b[0m"

/*
 * typedefs of the game variables!
 */
typedef struct {
	byte huge *data;
	word width;
	word height;
	byte *palette;
	word offset;
} bitmap_t;

typedef struct {
	byte **data;
	word ntiles;   /* the number of tiles */
	word twidth;   /* width of the tiles */
	word theight;  /* height of the tiles */
	byte *palette; /* palette for the tile set */
} tileset_t;

typedef struct {
	byte far *plane[4];     /* 4 planes of image data */
	word width;         /* width of the image (spread across 4 planes) */
	word height;        /* height of the image (spread across 4 planes) */
	word pwidth;        /* the number of bytes in each plane */
	byte *palette;
} planar_buf_t;

typedef struct {
	word id;	/* the Identification number of the page~ For layering~ */
	byte far* data;	/* the data for the page */
	word dx;		/* col we are viewing on the virtual screen */
	word dy;		/* row we are viewing on the virtual screen */
	word sw;		/* screen width */
	word sh;		/* screen heigth */
	word tilesw;		/* screen width in tiles */
	word tilesh;		/* screen height in tiles */
	word width;		/* virtual width of the page */
	word height;	/* virtual height of the page */
	word tw;
	word th;
	sword tilemidposscreenx;	/* middle tile position */
	sword tilemidposscreeny;	/* middle tile position */
	sword tileplayerposscreenx;	/* player position on screen */
	sword tileplayerposscreeny;	/* player position on screen */
} page_t;

typedef struct
{
// 	int showmemhandle;
	int			profilehandle,debughandle;
	int heaphandle;
} handle_t;

typedef struct
{
	word frames_per_second;
	clock_t t;
	dword tiku;		//frames passed
	word clock_start;	//timer start
	word *clock;	//current time on clock
	boolean fpscap;	//cap the fps var
} kurokku_t;

typedef struct
{
	long old_mode;	//old video mode before game!
	page_t page[4];	//pointer to root page[0]
} video_t;

typedef struct
{
	video_t video;	// video settings variable
	byte *pee;		// message for fps
	handle_t handle;	//handles for file logging
	kurokku_t kurokku;	//clock struct
} global_game_variables_t;

#endif /* _TYPEDEFSTRUCT_H_ */
