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

#include "src/lib/16_t.h"

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
	byte far **data;
	word ntiles;   /* the number of tiles */
	word twidth;   /* width of the tiles */
	word theight;  /* height of the tiles */
	byte *palette; /* palette for the tile set */
} tileset_t;

typedef struct {
	byte far *plane[4];	// 4 planes of image data
	word width;			// width of the image (spread across 4 planes)
	word height;		// height of the image (spread across 4 planes)
	word pwidth;		// the number of bytes in each plane
	byte *palette;
} planar_buf_t;

//TODO: 16_mm and 16_ca must handle this
typedef struct {
	bitmap_t far *btdata;		//old
	planar_buf_t far *data;	//old
	word tileHeight, tileWidth;
	unsigned int rows, cols;
// 	#ifdef __DEBUG__
// 	boolean debug_text;	//show the value of the tile! wwww
// 	byte *debug_data;
// 	#endif
} tiles_t;

//TODO: 16_mm and 16_ca must handle this
//TODO: add variables from 16_ca
typedef struct {
	//long		planestart[3];
	//unsigned	planelength[3];
	byte *data;			//TODO: 16_mm and 16_ca must handle this
	byte * far *layerdata;	//TODO: 16_mm and 16_ca must handle this
	tiles_t *tiles;		//TODO: 16_mm and 16_ca must handle this
	tiles_t * far *layertile;	//TODO: 16_mm and 16_ca must handle this
	int width, height;		//this has to be signed!
	char		name[16];
} map_t;

// tile info
typedef struct{
	word tw;		/* screen width in tiles */
	word th;		/* screen height in tiles */
	word tilesw;		/* virtual screen width in tiles */
	word tilesh;		/* virtual screen height in tiles */
	sword tilemidposscreenx;	/* middle tile x position */	/* needed for scroll system to work accordingly */
	sword tilemidposscreeny;	/* middle tile y position */	/* needed for scroll system to work accordingly */
	sword tileplayerposscreenx;	/* player position on screen */	/* needed for scroll and map system to work accordingly */
	sword tileplayerposscreeny;	/* player position on screen */	/* needed for scroll and map system to work accordingly */
} ti_t;

typedef struct {
	nibble/*word*/ id;	/* the Identification number of the page~ For layering~ */
	byte far* data;	/* the data for the page */
	word dx;		/* col we are viewing on virtual screen (on page[0]) */	/* off screen buffer on the left size */
	word dy;		/* row we are viewing on virtual screen (on page[0]) */	/* off screen buffer on the top size */
	word sw;		/* screen width */	/* resolution */
	word sh;		/* screen heigth */	/* resolution */
	word tw;		/* screen width in tiles */
	word th;		/* screen height in tiles */
	word width;		/* virtual width of the page */
	word height;	/* virtual height of the page */
	word tilesw;		/* virtual screen width in tiles */
	word tilesh;		/* virtual screen height in tiles */
	sword tilemidposscreenx;	/* middle tile x position */	/* needed for scroll system to work accordingly */
	sword tilemidposscreeny;	/* middle tile y position */	/* needed for scroll system to work accordingly */
	sword tileplayerposscreenx;	/* player position on screen */	/* needed for scroll and map system to work accordingly */
	sword tileplayerposscreeny;	/* player position on screen */	/* needed for scroll and map system to work accordingly */
	word stridew;			/* width/4 */	/* VGA */
	word pagesize;			/* page size */
	word pi;				/* increment page by this much to preserve location */
//newer vars
//TODO: find where they are used
	sword delta;			// How much should we shift the page for smooth scrolling
} page_t;

//TODO: MAKE THIS WWWW
typedef struct
{
	//sprite ....
	boolean wwww;
} spri_t;

//TODO: MAKE THIS TO WWWW
typedef struct
{
	//vrs with sprite ....
	spri_t *spri;
} vrs_t;

typedef struct
{
	int profilehandle,debughandle,showmemhandle;
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
#define NUMCHUNKS	416	//keen

typedef struct
{
	char old_mode;		//old video mode before game!
	page_t page[MAXPAGE];	//can be used as a pointer to root page[0]
	word vmem_remain;	//remaining video memory
	byte num_of_pages;	//number of actual pages
	boolean __near p;			//render page number
	boolean __near r;			//page flip if true
	word pr[MAXPAGE][4];	//render sections of pages (this is supposed to be set up to draw sections of the screen if updated)
	//0000word startclk; float clk, tickclk;	//timer
//newer vars
//TODO: find out how they are used
	byte grneeded[NUMCHUNKS];
} video_t;

//from 16_mm
//==========================================================================

#define MAXBLOCKS		1024
#define MAXUMBS		12

typedef struct mmblockstruct
{
	word	start,length;
	//word	start;	dword length;
	word	blob;	//for data larger than 64k
	unsigned	attributes;
	memptr		*useptr;	// pointer to the segment start
	struct mmblockstruct far *next;
} mmblocktype;

typedef struct
{
	dword	nearheap,farheap,EMSmem,XMSmem,mainmem;
//	boolean		PMStarted, MainPresent, EMSPresent, XMSPresent;
} mminfotype;

typedef struct
{
	memptr bufferseg;
	boolean		mmstarted, bombonerror, mmerror;
	void far	*farheap;
#ifdef __BORLANDC__
	void	*nearheap;
#endif
#ifdef __WATCOMC__
	void __near	*nearheap;
#endif
	unsigned int		EMSVer;
	word numUMBs,UMBbase[MAXUMBS];
	word			totalEMSpages, freeEMSpages, EMSpagesmapped, EMSHandle, EMSPageFrame;
	//dword	numUMBs,UMBbase[MAXUMBS];
	mmblocktype	far mmblocks[MAXBLOCKS],far *mmhead,far *mmfree,far *mmrover,far *mmnew;
} mminfo_t;

//==========================================================================


//from 16_pm
//==========================================================================

//	NOTE! PMPageSize must be an even divisor of EMSPageSize, and >= 1024
#define	EMSPageSize		16384
#define	EMSPageSizeSeg	(EMSPageSize >> 4)
#define	EMSPageSizeKB	(EMSPageSize >> 10)
#define	EMSFrameCount	4
#define	PMPageSize		4096
#define	PMPageSizeSeg	(PMPageSize >> 4)
#define	PMPageSizeKB	(PMPageSize >> 10)
#define	PMEMSSubPage	(EMSPageSize / PMPageSize)

#define	PMMinMainMem	10			// Min acceptable # of pages from main
#define	PMMaxMainMem	100			// Max number of pages in main memory

#define	PMThrashThreshold	1	// Number of page thrashes before panic mode
#define	PMUnThrashThreshold	5	// Number of non-thrashing frames before leaving panic mode

typedef	enum
		{
			pml_Unlocked,
			pml_Locked
		} PMLockType;

typedef	enum
		{
			pmba_Unused = 0,
			pmba_Used = 1,
			pmba_Allocated = 2
		} PMBlockAttr;

typedef	struct
		{
			dword	offset;		// Offset of chunk into file
			word		length;		// Length of the chunk

			int			xmsPage;	// If in XMS, (xmsPage * PMPageSize) gives offset into XMS handle

			PMLockType	locked;		// If set, this page can't be purged
			int			emsPage;	// If in EMS, logical page/offset into page
			int			mainPage;	// If in Main, index into handle array

			dword	lastHit;	// Last frame number of hit
		} PageListStruct;

typedef	struct
		{
			int			baseEMSPage;	// Base EMS page for this phys frame
			dword	lastHit;		// Last frame number of hit
		} EMSListStruct;

//	Main Mem specific variables
typedef struct
{
	boolean			MainPresent;
	memptr			MainMemPages[PMMaxMainMem];
	PMBlockAttr		MainMemUsed[PMMaxMainMem];
	int				MainPagesAvail;
} pm_mmi_t;

//	EMS specific variables
typedef struct
{
	boolean			EMSPresent;
	unsigned int			EMSVer;
	word			EMSAvail,EMSPagesAvail,EMSHandle,
					EMSPageFrame,EMSPhysicalPage;
	word			totalEMSpages, freeEMSpages, EMSpagesmapped;
	EMSListStruct	EMSList[EMSFrameCount];
} pm_emmi_t;

//	XMS specific variables
typedef struct
{
	boolean			XMSPresent;
	word			XMSAvail,XMSPagesAvail,XMSHandle;//,XMSVer;
	dword			XMSDriver;
	int				XMSProtectPage;// = -1;
} pm_xmmi_t;

//	File specific variables
typedef struct
{
	char			PageFileName[13];// = {"VSWAP."};
	int				PageFile;// = -1;
	word			ChunksInFile;
	word			PMSpriteStart,PMSoundStart;
} pm_fi_t;

//	General usage variables
typedef struct
{
	boolean			PMStarted,
					PMPanicMode,
					PMThrashing;
	word			XMSPagesUsed,
					EMSPagesUsed,
					MainPagesUsed,
					PMNumBlocks;
	long			PMFrameCount;
	PageListStruct	far *PMPages;
	__SEGA *PMSegPages;
	pm_mmi_t	mm;
	pm_emmi_t	emm;
	pm_xmmi_t	xmm;
	pm_fi_t	fi;
} pm_t;

//==========================================================================

//from 16_ca
//==========================================================================

#define NUMMAPS		4//39
#define MAPPLANES		3

typedef struct
{
  word bit0,bit1;	// 0-255 is a character, > is a pointer to a node
} huffnode;

typedef struct
{
	int		mapon, mapnum;
	__SEGA	*mapsegs[4];
	__SEGA	*mapheaderseg[NUMMAPS];
	__SEGA	*tinf;
} ca_mapinfo_t;

typedef struct
{
	int			maphandle[4];		// handle to MAPTEMP / GAMEMAPS
} ca_handle_t;

typedef struct
{
	byte		ca_levelbit,ca_levelnum;
	ca_handle_t	file;		//files to open
	ca_mapinfo_t	camap;
	__SEGA	*grsegs[NUMCHUNKS];
	byte		far	grneeded[NUMCHUNKS];
	huffnode huffnode;
} ca_t;

//==========================================================================

//actual global game varables!
typedef enum {
	ENGI_EXIT,
	ENGI_QUIT,
	ENGI_RUN,
	ENGI_INPUT,
	ENGI_PAUSE
} engi_stat_t;

typedef struct
{
	video_t	video;	// video settings variable
	ca_t		ca;	// ca stuff
	pm_t		pm;	// pm stuff
	loghandle_t handle;	//handles for file logging
	kurokku_t kurokku;	//clock struct
	mminfo_t mm; mminfotype mmi;
} global_game_variables_t;

extern char global_temp_status_text[512];
#ifdef __WATCOMC__
#define EINVFMT EMFILE
#endif
#endif /* _TYPEDEFSTRUCT_H_ */
