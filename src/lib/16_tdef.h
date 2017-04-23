/* Project 16 Source Code~
 * Copyright (C) 2012-2017 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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

#define BDOFSCONV (unsigned __near)
#define BOFS page[1].data
#define DOFS page[0].data

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
	word ntiles;		// the number of tiles
	word twidth;	// width of the tiles
	word theight;	// height of the tiles
	byte *palette;	// palette for the tile set
} tileset_t;

typedef struct {
	byte far *plane[4];	// 4 planes of image data
	word width;			// width of the image (spread across 4 planes)
	word height;		// height of the image (spread across 4 planes)
	word pwidth;		// the number of bytes in each plane
	byte *palette;
} planar_buf_t;

//from 16_sprit.h
#ifdef	__WATCOMC__
#include <hw/vga/vrl.h>

typedef struct vrs_container{
	// Size of a .vrs blob in memory
	// minus header
	dword data_size;
	union{
		byte far *buffer;
		struct vrs_header far *vrs_hdr;
	};
	// Array of corresponding vrl line offsets
	vrl1_vgax_offset_t **vrl_line_offsets;
} vrs_container_t;

typedef struct vrl_container{
	// Size of a .vrl blob in memory
	// minus header
	dword data_size;
	union{
		byte far *buffer;
		struct vrl1_vgax_header far *vrl_header;
	};
	// Pointer to a corresponding vrl line offsets struct
	vrl1_vgax_offset_t *line_offsets;
} vrl_container_t;

typedef struct sprite
{
	// VRS container from which we will extract animation and image data
	vrs_container_t	spritesheet;
	// Container for a vrl sprite
	vrl_container_t	sprite_vrl_cont;
	// Current sprite id
	int curr_spri_id;
	// Index of a current sprite in an animation sequence
	int curr_anim_spri;
	// Current animation sequence
	struct vrs_animation_list_entry_t *curr_anim_list;
	// Index of current animation in relevant VRS offsets table
	int curr_anim;
	// Delay in time units untill we should change sprite
	int delay;
	// Position of sprite on screen
	int x, y;
} sprite_t;
#endif

//newer structs
typedef	struct
{
	int x; //entity exact position on the viewable map
	int y; //entity exact position on the viewable map
	int tx; //entity tile position on the viewable map
	int ty; //entity tile position on the viewable map
	int triggerx; //entity's trigger box tile position on the viewable map
	int triggery; //entity's trigger box tile position on the viewable map
// 	int sheetsetx; //NOT USED YET! entity sprite sheet set on the image x
// 	int sheetsety; //NOT USED YET! entity sprite sheet set on the image y
	nibble d;		//direction to render sprite!! wwww
	nibble pred;	//prev. direction for animation changing
	word dire;		//sprite in use
	nibble q;		//loop variable for anumation and locking the playing to compleate the animation cycle to prevent issues with misalignment www
	word speed;		//entity speed!
	word spt;		//speed per tile
#ifdef	__WATCOMC__
	sprite_t spri; // sprite used by entity
#endif
	sword hp; //hitpoints of the entity
	nibble overdraww, overdrawh;	// how many pixels to "overdraw" so that moving sprites with edge pixels don't leave streaks.
						// if the sprite's edge pixels are clear anyway, you can set this to 0.
	nibble /*int*/ persist_aniframe;    // gonna be increased to 1 before being used, so 0 is ok for default
} entity_t;

typedef	struct
{
	int x; //entity exact position on the viewable map
	int y; //entity exact position on the viewable map
	int tx; //entity tile position on the viewable map
	int ty; //entity tile position on the viewable map

#ifdef	__WATCOMC__
	sprite_t spri; // sprite used by entity
#endif
	sword hp; //hitpoints of the entity
} static_map_entity_t;

//===========================================================================//

//TODO: 16_mm and 16_ca must handle this
typedef struct {
#ifdef	__WATCOMC__
	sprite_t *spri;			// I will probibaly use this --sparky4
#endif
	word tileHeight, tileWidth;	//defined by mapfile
	unsigned int rows, cols;
	byte	imgname[8];		//image file of tileset (set to 8 because DOS ^^;)
} tiles_t;	//seems to be the tileset properties

//TODO: 16_mm and 16_ca must handle this
//TODO: add variables from 16_ca
typedef struct {
	byte	layername[8];
	byte	*data;			//TODO: 16_mm and 16_ca must handle this
} mapl_t;	//map layer array type def

#define MAPPLANES		3
typedef struct {
	//long		planestart[3];
	//unsigned	planelength[3];
	mapl_t layerdata[MAPPLANES];	// mapdata for multilayer (map index values for rendering which image on the tile)
	tiles_t *tiles;		//TODO: 16_mm and 16_ca must handle this	// tilesets for layers (currently ony 4 can be loaded wwww)
	int width, height;		//this has to be signed!
	byte name[16];
} map_t;

//===================================//

typedef struct{
	word tw;				/* screen width in tiles */
	word th;				/* screen height in tiles */
	word tilesw;				/* virtual screen width in tiles */
	word tilesh;				/* virtual screen height in tiles */
	sword tilemidposscreenx;	/* middle tile x position */	/* needed for scroll system to work accordingly */
	sword tilemidposscreeny;	/* middle tile y position */	/* needed for scroll system to work accordingly */
	sword tileplayerposscreenx;	/* player position on screen */	/* needed for scroll and map system to work accordingly */
	sword tileplayerposscreeny;	/* player position on screen */	/* needed for scroll and map system to work accordingly */
} pagetileinfo_t;

typedef struct {
	nibble/*word*/ id;	/* the Identification number of the page~ For layering~ */
	byte far* data;	/* the data for the page */
	pagetileinfo_t ti;	// the tile information of the page
	word dx;		/* col we are viewing on virtual screen (on page[0]) */	/* off screen buffer on the left size */
	word dy;		/* row we are viewing on virtual screen (on page[0]) */	/* off screen buffer on the top size */
	word sw;		/* screen width */	/* resolution */
	word sh;		/* screen heigth */	/* resolution */
	word width;		/* virtual width of the page */
	word height;	/* virtual height of the page */
	word stridew;	/* width/4 */	/* VGA */
	word pagesize;	/* page size */
	word pi;		/* increment page by this much to preserve location */
	int tlx,tly;
//newer vars
//TODO: find where they are used
	sword delta;			// How much should we shift the page for smooth scrolling
} page_t;

//from 16_in
//==========================================================================
#define	KeyInt		9	// The keyboard ISR number

//
// mouse constants
//
#define	MReset		0
#define	MButtons	3
#define	MDelta		11

#define	MouseInt	0x33
//#define	Mouse(x)	_AX = x,geninterrupt(MouseInt)

//
// joystick constants
//
#define	JoyScaleMax		32768
#define	JoyScaleShift	8
#define	MaxJoyValue		5000

#define	MaxPlayers	4
#define	MaxKbds		2
#define	MaxJoys		2
#define	NumCodes	128

typedef	byte		ScanCode;

typedef	enum		{
						//ctrl_None,				// MDM (GAMERS EDGE) - added
						ctrl_Keyboard,
							ctrl_Keyboard1 = ctrl_Keyboard,ctrl_Keyboard2,
						ctrl_Joystick,
							ctrl_Joystick1 = ctrl_Joystick,ctrl_Joystick2,
						ctrl_Mouse,
					} ControlType;
typedef	enum		{
						motion_Left = -1,motion_Up = -1,
						motion_None = 0,
						motion_Right = 1,motion_Down = 1
					} Motion;
typedef	enum		{		// Quick lookup for total direction
						/*dir_NorthWest,	*/dir_North,/*	dir_NorthEast,*/
						dir_West,		dir_None,	dir_East,
						/*dir_SouthWest,	*/dir_South/*,	dir_SouthEast*/
					} Direction;
typedef	struct		{
						boolean	near	button0,button1,button2,button3;
						int	near		x,y;
						Motion	near	xaxis,yaxis;
						Direction near	dir;
					} CursorInfo;

typedef	struct		{
						ScanCode near	button0,button1,
									//upleft,
									up,
									down,
									left,
									right
									//upright,
									//downleft,
									//,downright
									;
					} KeyboardDef;
typedef	struct		{
						word	near	joyMinX,joyMinY,
									threshMinX,threshMinY,
									threshMaxX,threshMaxY,
									joyMaxX,joyMaxY,
									joyMultXL,joyMultYL,
									joyMultXH,joyMultYH;
					} JoystickDef;

typedef struct instat {
	boolean		CapsLock;
	ScanCode	CurCode,LastCode;

	boolean		Keyboard[NumCodes];
	boolean		Paused;
	char		LastASCII;
	ScanCode	LastScan;
} inst_t;

typedef struct// inconfig
{
	boolean		IN_Started;
//
// configuration variables
//
	boolean		JoysPresent[MaxJoys],
					MousePresent,
					JoyPadPresent;

// 	Global variables
	KeyboardDef	KbdDefs[MaxKbds];
	JoystickDef	JoyDefs[MaxJoys];
	inst_t	*inst;
} in_info_t;

//==========================================================================

typedef	struct
{
	entity_t near	enti;
#ifdef	__WATCOMC__
	//struct sprite	*spri;	//supposively the sprite sheet data
	//memptr		gr;
#endif
//	bitmap_t	*data;		//supposively the sprite sheet data//old format
//	bitmap_t	bmp;

	//input
	byte near		pdir;	//previous direction~ used in IN_16 in IN_ReadControl()
	CursorInfo		info;
	ControlType	Controls;
	word walktype;
//newer vars
	int dx, dy, delta;	//TODO: what is this? ^^
} player_t;

//===========================================//

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
	nibble wcpu;	//stored value of cpu type
} kurokku_t;

//===================================//
#define PALSIZE	768
#define NUMCHUNKS	416	//keen
#define MAXSCANLINES	240			// size of ylookup table

typedef struct
{
	int width,height;
} pictabletype;

//video
typedef struct
{
	word	tileHeight, tileWidth;	//defined by mapfile
	word	quadwh;			//preproccessed quad size of tilewidth and tileheight
} tile_dimention_t;

#define MAXSCROLLEDGES 6
typedef struct
{
	unsigned	panx,pany;		// panning adjustments inside port in pixels
	unsigned	pansx,pansy;
	unsigned	panadjust;		// panx/pany adjusted by screen resolution
	int		hscrollblocks,vscrollblocks;
	int		hscrolledge[MAXSCROLLEDGES],vscrolledge[MAXSCROLLEDGES];
} pan_t;

typedef struct
{
	//ylookup[MAXSCANLINES],
	unsigned int offscreen_ofs;
	unsigned int pattern_ofs;
//	unsigned	bufferofs,linewidth,displayofs;
	pan_t		pan;

} ofs_t;	//unfinished

typedef struct
{
//doslib origi vars
	byte far * omemptr;
	byte vga_draw_stride;
	byte vga_draw_stride_limit;		// further X clipping
//end of doslib origi vars
	boolean __near rss;		//render sprite switch
	boolean __near bgps;		//bg preservation render switch between old and new
} vga_state_t;

typedef struct
{
	boolean	VL_Started,VL_Initiated;
	char old_mode;		//old video mode before game!
	sword curr_mode;		//modex curent mode
	byte palette[PALSIZE], dpal[PALSIZE];	//palette array
	page_t page[MAXPAGE];	//can be used as a pointer to root page[0]
	word vmem_remain;	//remaining video memory
	byte num_of_pages;	//number of actual pages
	vga_state_t	vga_state;
	sword __near sprifilei;		//player file's i
	nibble __near p;			//render page number
	nibble __near sp;			//show page number(for showpage)
	boolean __near dorender;	//page flip, showpage, or render	if true
	word pr[MAXPAGE][4];	//render sections of pages (this is supposed to be set up to draw sections of the screen if updated)

	nibble sfip;		//shinku_fps_indicator_page; // we're on page 1 now, shinku(). follow along please or it will not be visible.
	ofs_t	ofs;		//offset vars used for doslib
	word	vh;		//video combined height
	tile_dimention_t	td;
	//0000word startclk; float clk, tickclk;	//timer
	//wolf3d vars
	boolean	screenfaded;
	word		bordercolor;
	boolean	fastpalette;
	byte		far	palette1[256][3],far palette2[256][3];
	pictabletype	_seg *pictable;
} video_t;

//from scroll16
//==========================================================================
typedef struct
{
	map_t *map;
	page_t *page;
	int tx,ty; //appears to be the top left tile position on the viewable screen map
	word dxThresh,dyThresh; //Threshold for physical tile switch
	video_t *video;	//pointer to game variables of the video
	kurokku_t *kurokku;	//pointer to game variables of the kurokku
	nibble __near *p;	// pointer to video's render page num
	nibble __near *sp;	// pointer to video's show page num
	int dx, dy;	// draw row and col var
//newer vars!
	int delta, d;
} map_view_t;
/* Map is presumed to:
 * 1. Have all the required layers and tilesets within itself
 * 2. Have a 'fence' around accessible blocks to simplify boundary logic
 * 3. Have a persistent map and tile size among the layers
 * Map view is presumed to:
 * 1. Calculate, store and update a panning info, which includes, but not limited to:
 * 	combined layer information, actual map representation (reflecting real state of the game),
 * 	pixel shift for smooth tile scrolling.
 * 2. Provide ways to draw a visible part of map. For simplicity with smooth scrolling,
 * 	additional row/column is always drawn at the each side of the map. This implies that 'fence'
 * 	should have a sprite too. Map is drawn left-to-right, top-to-bottom.
 */

//==========================================================================

//from 16_mm
//==========================================================================

#define MAXBLOCKS		1024//kd=1300 wolf3d=700 cata=600
//----#define MAXUMBS		12

typedef struct mmblockstruct
{
	word	start,length;
	//word	start;	dword length;
//++++	word	blob;	//for data larger than 64k
	unsigned	attributes;
	memptr		*useptr;	// pointer to the segment start
	struct mmblockstruct far *next;
} mmblocktype;

typedef struct mmshowmemoryinfo
{
	word x,y;
	mmblocktype far *scan;
} mmshowmemoryinfo_t;

typedef struct
{
	dword	nearheap,farheap,EMSmem,XMSmem,mainmem;
//	boolean		PMStarted, MainPresent, EMSPresent, XMSPresent;
} mminfotype;

typedef struct
{
	boolean		mmstarted, bombonerror, mmerror;
	void far	*farheap;
#ifdef __BORLANDC__
	void		*nearheap;
#endif
#ifdef __WATCOMC__
	void __near	*nearheap;
#endif
//----	unsigned int		EMSVer;
//----	word numUMBs,UMBbase[MAXUMBS];
//----	word			totalEMSpages, freeEMSpages, EMSpagesmapped, EMSHandle, EMSPageFrame;
//----	dword	numUMBs,UMBbase[MAXUMBS];
	mmblocktype	far mmblocks[MAXBLOCKS],far *mmhead,far *mmfree,far *mmrover,far *mmnew;
	memptr	bufferseg;	//Allocates bufferseg misc buffer
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
	PageListStruct	far *PMPages,
					_seg *PMSegPages;
	pm_mmi_t	mm;
	pm_emmi_t	emm;
	pm_xmmi_t	xmm;
	pm_fi_t	fi;
} pm_t;

//==========================================================================

//for 16_sd
//==========================================================================
#pragma pack(push,1)
typedef struct imf_entry
{
	byte		reg,data;
	word		delay;
} imf_entry_t;
#pragma pack(pop)

typedef struct
{
	volatile unsigned long irq0_ticks;//=0;
	volatile unsigned int irq0_cnt,irq0_add,irq0_max;//=0;
	imf_entry_t*	imf_music;//=NULL;
	imf_entry_t*	imf_play_ptr;//=NULL;
	imf_entry_t*	imf_music_end;//=NULL;
	word		imf_delay_countdown;//=0;
} sd_t;

//==========================================================================

//from 16_ca
//==========================================================================

#define NOMAPS
//#define NOGRAPHICS
#define NOAUDIO

#define MAPHEADERLINKED
//#define GRHEADERLINKED
#define AUDIOHEADERLINKED

#define NUMMAPS		4//39
#define NUMSNDCHUNKS		4//3
//#define NUMPICS		4//132//wolf3d wl6

#define STRUCTPIC	0
#define CASVT		dword	//*start var type


#define DATADIR		"data/"
#define GDICTNAME	DATADIR"vgadict."
#define GHEADNAME	DATADIR"vgahead."
#define GFILENAME	DATADIR"vgagraph."
#define EXTENSION	"hb1"


typedef struct
{
  word bit0,bit1;	// 0-255 is a character, > is a pointer to a node
} huffnode;

typedef	struct
{
	long		planestart[3];
	unsigned	planelength[3];
	unsigned	width,height;
	char		name[16];
} maptype;

typedef struct
{
	int		mapon, mapnum;
	maptype		_seg	*mapheaderseg[NUMMAPS];
} ca_mapinfo_t;

typedef struct
{
	int			maphandle;		// handle to MAPTEMP / GAMEMAPS
	int			grhandle;		// handle to EGAGRAPH
	int			audiohandle;	// handle to AUDIOT / AUDIO
} ca_handle_t;
/*
16/wf3d8086/id_ca.c:	grstarts = (long _seg *)FP_SEG(&EGAhead);
16/wf3d8086/id_ca.c:	tinf = (byte _seg *)FP_SEG(&maphead);
16/wf3d8086/id_ca.c:		pos = ((mapfiletype	_seg *)tinf)->headeroffsets[i];
16/wf3d8086/id_ca.c:	audiostarts = (long _seg *)FP_SEG(&audiohead);
16/wf3d8086/id_ca.c:		((mapfiletype _seg *)tinf)->RLEWtag);
16/wf3d8086/id_ca.c:		((mapfiletype _seg *)tinf)->RLEWtag);
16/wf3d8086/id_ca.c:					source = (byte _seg *)bufferseg+(pos-bufferstart);
*/
typedef struct	//TODO: USE THIS!!!!
{
	byte	ca_levelbit,ca_levelnum;
	ca_handle_t		file;		//files to open
	ca_mapinfo_t	camap;

	byte 		_seg	*tinf;//?? where in the id engine is this used and what is it? --sparky4
	unsigned	_seg	*mapsegs[MAPPLANES];
	void		_seg	*grsegs[NUMCHUNKS];
	byte		far		grneeded[NUMCHUNKS];
	word		_seg *audiosegs[NUMSNDCHUNKS];//long

	CASVT		_seg	*grstarts;	// array of offsets in egagraph, -1 for sparse//long
	CASVT		_seg	*audiostarts;	// array of offsets in audio / audiot//long

#ifdef GRHEADERLINKED
	huffnode	*grhuffman;
#else
	huffnode	grhuffman[255];
#endif

#ifdef AUDIOHEADERLINKED
	huffnode	*audiohuffman;
#else
	huffnode	audiohuffman[255];
#endif

	CASVT		chunkcomplen,chunkexplen;//long

	sd_t		sd;
	//TODO: extend! and learn from keen/wolf/catacomb's code wwww
	memptr	spribuff;
} ca_t;

//==========================================================================

//actual global game varables!
/*typedef struct {
	boolean clearmem;
} vidsw_t;

typedef struct {
	vidsw_t vsw;
} sw_t;*/



typedef struct glob_game_vars
{
	video_t	video;	// video settings variable
	ca_t		ca;	// ca stuff
	pm_t		pm;	// pm stuff
	loghandle_t	handle;	//handles for file logging
	kurokku_t	kurokku;	//clock struct
	mminfo_t	mm; mminfotype	mmi;	// mm stuff
	in_info_t	in;		// 16_in info
	player_t	player[MaxPlayers];	// player vars
	map_view_t	mv[4];
//	sw_t		sw;
	boolean	DLStarted;
} global_game_variables_t;

#ifdef __WATCOMC__
extern char global_temp_status_text[512];
extern char global_temp_status_text2[512];
#define EINVFMT EMFILE
#endif
#endif /* _TYPEDEFSTRUCT_H_ */
