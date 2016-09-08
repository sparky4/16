/* Project 16 Source Code~
 * Copyright (C) 2012-2016 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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

#define MAXPAGE 4

/*
 * typedefs of the game variables!
 */
typedef struct {
	byte *data;
	word width;
	word height;
	byte *palette;
	word offset;
} bitmap_t;

typedef struct {
	byte huge **data;
	word ntiles;   /* the number of tiles */
	word twidth;   /* width of the tiles */
	word theight;  /* height of the tiles */
	byte *palette; /* palette for the tile set */
} tileset_t;

typedef struct {
	byte huge *plane[4];     /* 4 planes of image data */
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
	word tw;		/* screen width in tiles */
	word th;		/* screen height in tiles */
	word width;		/* virtual width of the page */
	word height;	/* virtual height of the page */
	word tilesw;		/* virtual screen width in tiles */
	word tilesh;		/* virtual screen height in tiles */
	sword tilemidposscreenx;	/* middle tile position */
	sword tilemidposscreeny;	/* middle tile position */
	sword tileplayerposscreenx;	/* player position on screen */
	sword tileplayerposscreeny;	/* player position on screen */
	word stridew;			/*width/4*/
	word pagesize;			/* page size */
	word pi;				/* incremention page by this much to preserve location */
} page_t;

typedef struct
{
	//sprite ....
	boolean wwww;
} spri_t;

typedef struct
{
	//vrs with sprite ....
	spri_t *spri;
} vrs_t;

typedef struct
{
	int			profilehandle,debughandle,showmemhandle;
	int heaphandle;
} loghandle_t;

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
	word	pn;
} pan_t;

//video
#define NUMCHUNKS	3016	//keen

typedef struct
{
	char old_mode;		//old video mode before game!
	byte grneeded[NUMCHUNKS];
	page_t page[MAXPAGE];		//pointer to root page[0]
	word vmem_remain;	//remaining video memory
	byte num_of_pages;	//number of actual pages
	boolean __near p;			//render page number
	boolean __near r;			//page flip if true
	word pr[MAXPAGE][4];	//render sections of pages
	//0000word startclk; float clk, tickclk;	//timer
} video_t;

typedef struct mmblockstruct
{
	word	start,length;
	//word	start;	dword length;
	word	blob;	//for data larger than 64k
	unsigned	attributes;
	memptr		*useptr;	// pointer to the segment start
	//huge struct mmblockstruct huge *next;
	struct mmblockstruct far *next;
} mmblocktype;

//from 16_mm
//==========================================================================

#define MAXBLOCKS		1024
#define MAXUMBS		12

typedef struct
{
	dword	nearheap,farheap,EMSmem,XMSmem,mainmem;
} mminfotype;

typedef struct
{
	memptr bufferseg;
	boolean		mmstarted, bombonerror, mmerror;
	//huge void huge	*farheap;
	void far	*farheap;
#ifdef __BORLANDC__
	void	*nearheap;
#endif
#ifdef __WATCOMC__
	void __near	*nearheap;
#endif
	//byte		EMS_status;
	unsigned	totalEMSpages,freeEMSpages,EMSpageframe,EMSpagesmapped,EMShandle;
	unsigned int EMSVer;
	word numUMBs,UMBbase[MAXUMBS];
	//dword	numUMBs,UMBbase[MAXUMBS];
	//huge mmblocktype	huge mmblocks[MAXBLOCKS],huge *mmhead,huge *mmfree,huge *mmrover,huge *mmnew;
	mmblocktype	far mmblocks[MAXBLOCKS],far *mmhead,far *mmfree,far *mmrover,far *mmnew;
} mminfo_t;

//==========================================================================

//from 16_ca
//==========================================================================

#define NUMMAPS		4//39
#define MAPPLANES		3

typedef struct
{
	__segment	*mapsegs[4];
	__segment	*mapheaderseg[NUMMAPS];
} mapinfo_t;

typedef struct
{
	int			maphandle[4];		// handle to MAPTEMP / GAMEMAPS
} handle_t;

typedef struct
{
	byte		ca_levelbit,ca_levelnum;
	int		mapon, mapnum;
	handle_t	file;		//files to open
	mapinfo_t	map;
	//_seg	*grsegs[NUMCHUNKS];
	//byte		far	grneeded[NUMCHUNKS];
	//huffnode huffnode;
} ca_t;

//==========================================================================

//actual global game varables!
typedef struct
{
	video_t	video;	// video settings variable
	ca_t		ca;	// ca stuff
	byte *pee;		// message for fps
	loghandle_t handle;	//handles for file logging
	kurokku_t kurokku;	//clock struct
	mminfo_t mm; mminfotype mmi;
} global_game_variables_t;

#endif /* _TYPEDEFSTRUCT_H_ */
