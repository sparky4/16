/* Keen Dreams Source Code
 * Copyright (C) 2014 Javier M. Chavez
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

// 16_RF.C

/*
=============================================================================

notes
-----

scrolling more than one tile / refresh forces a total redraw

two overlapping sprites of equal priority can change drawing order when
updated

=============================================================================
*/

#include "src/lib/16_rf.h"
#pragma hdrstop

struct glob_game_vars *gvar;
static word far* clockw= (word far*) 0x046C; /* 18.2hz clock */

/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/

#define	SCREENTILESWIDE	20
#define	SCREENTILESHIGH	13

#define	SCREENSPACE		(SCREENWIDTH*240)
#define FREEEGAMEM		(0x10000l-3l*SCREENSPACE)

//
// the update array must have enough space for two screens that can float
// up two two tiles each way
//
// (PORTTILESWIDE+1)*PORTTILESHIGH must be even so the arrays can be cleared
// by word width instructions

#define	UPDATESCREENSIZE	(UPDATEWIDE*PORTTILESHIGH+2)
#define	UPDATESPARESIZE		(UPDATEWIDE*2+4)
#define UPDATESIZE			(UPDATESCREENSIZE+2*UPDATESPARESIZE)

#define G_EGASX_SHIFT	7	// global >> ?? = screen x
#define G_CGASX_SHIFT	6	// global >> ?? = screen x
#define G_SY_SHIFT		4	// global >> ?? = screen y

unsigned	SX_T_SHIFT;		// screen x >> ?? = tile EGA = 1, CGA = 2;
#define	SY_T_SHIFT		4	// screen y >> ?? = tile


#define	EGAPORTSCREENWIDE	42
#define	CGAPORTSCREENWIDE	84
#define	PORTSCREENHIGH  224

#define	UPDATESCREENSIZE	(UPDATEWIDE*PORTTILESHIGH+2)
#define	UPDATESPARESIZE		(UPDATEWIDE*2+4)
#define UPDATESIZE			(UPDATESCREENSIZE+2*UPDATESPARESIZE)

#define MAXSCROLLEDGES	6

/*
=============================================================================

						   LOCAL TYPES

=============================================================================
*/

typedef	struct spriteliststruct
{
	int			screenx,screeny;
	int			width,height;

	unsigned	grseg,sourceofs,planesize;
	drawtype	draw;
	unsigned	tilex,tiley,tilewide,tilehigh;
	int			priority,updatecount;
	struct spriteliststruct **prevptr,*nextsprite;
} spritelisttype;


typedef struct
{
	int			screenx,screeny;
	int			width,height;
} eraseblocktype;


typedef struct
{
	unsigned	current;		// foreground tiles have high bit set
	int			count;
} tiletype;


typedef struct animtilestruct
{
	unsigned	x,y,tile;
	tiletype	*chain;
	unsigned	far *mapplane;
	struct animtilestruct **prevptr,*nexttile;
} animtiletype;

/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

unsigned	tics;
long		lasttimecount;

boolean		compatability;			// crippled refresh for wierdo SVGAs

unsigned	mapwidth,mapheight,mapbyteswide,mapwordswide
			,mapbytesextra,mapwordsextra;
unsigned	mapbwidthtable[MAXMAPHEIGHT];

//
// Global : Actor coordinates are in this, at 1/16 th of a pixel, to allow
// for fractional movement and acceleration.
//
// Tiles  : Tile offsets from the upper left corner of the current map.
//
// Screen : Graphics level offsets from map origin, x in bytes, y in pixels.
// originxscreen is the same spot as originxtile, just with extra precision
// so graphics don't need to be done in tile boundaries.
//

unsigned	originxglobal,originyglobal;
unsigned	originxtile,originytile;
unsigned	originxscreen,originyscreen;
unsigned	originmap;
unsigned	originxmin,originxmax,originymin,originymax;
unsigned	originxtile,originytile;

unsigned	masterofs;

//
// Table of the offsets from bufferofs of each tile spot in the
// view port.  The extra wide tile should never be drawn, but the space
// is needed to account for the extra 0 in the update arrays.  Built by
// RF_Startup
//

unsigned	blockstarts[UPDATEWIDE*UPDATEHIGH];
unsigned	updatemapofs[UPDATEWIDE*UPDATEHIGH];

unsigned	uwidthtable[PORTTILESHIGH];		// lookup instead of multiply

byte		update[2][UPDATESIZE];
byte		*updateptr,*baseupdateptr,						// current start of update window
			*updatestart[2],
			*baseupdatestart[2];

//from others
cardtype	videocard;		// set by VW_Startup
grtype		grmode;			// CGAgr, EGAgr, VGAgr

unsigned	bufferofs;		// hidden area to draw to before displaying
unsigned	displayofs;		// origin of the visable screen
//

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/
#ifdef PROFILE
static		char	scratch[20],str[20];
#endif

tiletype	allanims[MAXANIMTYPES];
unsigned	numanimchains;

void 		(*refreshvector) (void);

unsigned	screenstart[3] =
	{0,SCREENSPACE,SCREENSPACE*2};

unsigned	xpanmask;			// prevent panning to odd pixels

unsigned	screenpage;			// screen currently being displayed
unsigned	otherpage;

#if GRMODE == EGAGR
unsigned	tilecache[NUMTILE16];
#endif

spritelisttype	spritearray[MAXSPRITES],*prioritystart[PRIORITIES],
				*spritefreeptr;

animtiletype	animarray[MAXANIMTILES],*animhead,*animfreeptr;

int				animfreespot;

eraseblocktype	eraselist[2][MAXSPRITES],*eraselistptr[2];

/*
=============================================================================

						 LOCAL PROTOTYPES

=============================================================================
*/

void RFL_NewTile (unsigned updateoffset);
void RFL_MaskForegroundTiles (void);
void RFL_UpdateTiles (void);

void RFL_BoundScroll (int x, int y);//++++??
void RFL_CalcOriginStuff (long x, long y);
void RFL_ClearScrollBlocks (void);//++++??
void RFL_InitSpriteList (void);
void RFL_InitAnimList (void);
void RFL_CheckForAnimTile (unsigned x, unsigned y);
void RFL_AnimateTiles (void);
void RFL_RemoveAnimsOnX (unsigned x);
void RFL_RemoveAnimsOnY (unsigned y);
void RFL_EraseBlocks (void);
void RFL_UpdateSprites (void);


/*
=============================================================================

					 GRMODE INDEPENDANT ROUTINES

=============================================================================
*/


/*
=====================
=
= RF_Startup
=
=====================
*/

static	char *ParmStrings[] = {"comp",""};

void RF_Startup (void)
{
	int i,x,y;
	unsigned	*blockstart;

	if (grmode == EGAGR)
		for (i = 1;i < _argc;i++)
			if (US_CheckParm(_argv[i],ParmStrings) == 0)
			{
				compatability = true;
				break;
			}

	for (i=0;i<PORTTILESHIGH;i++)
		uwidthtable[i] = UPDATEWIDE*i;

	originxmin = originymin = MAPBORDER*TILEGLOBAL;

	eraselistptr[0] = &eraselist[0][0];
	eraselistptr[1] = &eraselist[1][0];



	if (grmode == EGAGR)
	{
		SX_T_SHIFT = 1;

		baseupdatestart[0] = &update[0][UPDATESPARESIZE];
		baseupdatestart[1] = &update[1][UPDATESPARESIZE];

		screenpage = 0;
		otherpage = 1;
		displayofs = screenstart[screenpage];
		bufferofs = screenstart[otherpage];
		masterofs = screenstart[2];

		updateptr = baseupdatestart[otherpage];

		blockstart = &blockstarts[0];
		for (y=0;y<UPDATEHIGH;y++)
			for (x=0;x<UPDATEWIDE;x++)
				*blockstart++ = SCREENWIDTH*16*y+x*TILEWIDTH;

		xpanmask = 6;	// dont pan to odd pixels
	}

	else if (grmode == CGAGR)
	{
		SX_T_SHIFT = 2;

		updateptr = baseupdateptr = &update[0][UPDATESPARESIZE];

		bufferofs = 0;
		masterofs = 0x8000;

		blockstart = &blockstarts[0];
		for (y=0;y<UPDATEHIGH;y++)
			for (x=0;x<UPDATEWIDE;x++)
				*blockstart++ = SCREENWIDTH*16*y+x*TILEWIDTH;
	}
}




/*
=====================
=
= RF_Shutdown
=
=====================
*/

void RF_Shutdown (void)
{

}

//===========================================================================


/*
=====================
=
= RF_FixOfs
=
= Sets bufferofs,displayofs, and masterofs to regular values, for the
= occasions when you have moved them around manually
=
=====================
*/

void RF_FixOfs (global_game_variables_t *gvar)
{
//	if (grmode == EGAGR)
//	{
		screenpage = 0;
		otherpage = 1;
		gvar->video.ofs.pan.panx = gvar->video.ofs.pan.pany = gvar->video.ofs.pan.pansx = gvar->video.ofs.pan.pansy = gvar->video.ofs.pan.panadjust = 0;
		displayofs = screenstart[screenpage];
		bufferofs = screenstart[otherpage];
		masterofs = screenstart[2];
/*++++		VL_SetScreen (displayofs,0);
	}
	else
	{
		bufferofs = 0;
		masterofs = 0x8000;
	}*/
}


//===========================================================================

/*
=====================
=
= RF_NewMap
=
= Makes some convienient calculations based on maphead->
=
=====================
*/
/*++++
void RF_NewMap (void)
{
	int i,x,y;
	unsigned spot,*table;

	mapwidth = mapheaderseg[mapon]->width;
	mapbyteswide = 2*mapwidth;
	mapheight = mapheaderseg[mapon]->height;
	mapwordsextra = mapwidth-PORTTILESWIDE;
	mapbytesextra = 2*mapwordsextra;

//
// make a lookup table for the maps left edge
//
	if (mapheight > MAXMAPHEIGHT)
	Quit (gvar, "RF_NewMap: Map too tall!");
	spot = 0;
	for (i=0;i<mapheight;i++)
	{
	  mapbwidthtable[i] = spot;
	  spot += mapbyteswide;
	}

//
// fill in updatemapofs with the new width info
//
	table = &updatemapofs[0];
	for (y=0;y<PORTTILESHIGH;y++)
		for (x=0;x<UPDATEWIDE;x++)
			*table++ = mapbwidthtable[y]+x*2;

//
// the y max value clips off the bottom half of a tile so a map that is
// 13 + MAPBORDER*2 tile high will not scroll at all vertically
//
	originxmax = (mapwidth-MAPBORDER-SCREENTILESWIDE)*TILEGLOBAL;
	originymax = (mapheight-MAPBORDER-SCREENTILESHIGH)*TILEGLOBAL;
	if (originxmax<originxmin)		// for very small maps
		originxmax=originxmin;
	if (originymax<originymin)
		originymax=originymin;

//
// clear out the lists
//
	RFL_InitSpriteList ();
	RFL_InitAnimList ();
	RFL_ClearScrollBlocks ();
	RF_SetScrollBlock (0,MAPBORDER-1,true);
	RF_SetScrollBlock (0,mapheight-MAPBORDER,true);
	RF_SetScrollBlock (MAPBORDER-1,0,false);
	RF_SetScrollBlock (mapwidth-MAPBORDER,0,false);


	lasttimecount = TimeCount;		// setup for adaptive timing
	tics = 1;
}
*/
//===========================================================================

/*
==========================
=
= RF_MarkTileGraphics
=
= Goes through mapplane[0/1] and marks all background/foreground tiles
= needed, then follows all animation sequences to make sure animated
= tiles get all the stages.  Every unique animating tile is given an
= entry in allanims[], so every instance of that tile will animate at the
= same rate.  The info plane for each animating tile will hold a pointer
= into allanims[], therefore you can't have both an animating foreground
= and background tile in the same spot!
=
==========================
*/
/*++++
void RF_MarkTileGraphics (void)
{
	unsigned	size;
	int			tile,next,anims;
	unsigned	far	*start,far *end,far *info;
	unsigned	i,tilehigh;

	memset (allanims,0,sizeof(allanims));
	numanimchains = 0;

	size = mapwidth*mapheight;

//
// background plane
//
	start = mapsegs[0];
	info = mapsegs[2];
	end = start+size;
	do
	{
		tile = *start++;
		if (tile>=0)			// <0 is a tile that is never drawn
		{
			CA_MarkGrChunk(STARTTILE16+tile);
			if (tinf[ANIM+tile])
			{
				// this tile will animated

				for (i=0;i<numanimchains;i++)
					if (allanims[i].current == tile)
					{
						*info = (unsigned)&allanims[i];
						goto nextback;
					}

				// new chain of animating tiles

				if (i>=MAXANIMTYPES)
					Quit (gvar, "RF_MarkTileGraphics: Too many unique animated tiles!");
				allanims[i].current = tile;
				allanims[i].count = tinf[SPEED+tile];

				*info = (unsigned)&allanims[i];
				numanimchains++;

				anims = 0;
				next = tile+(signed char)(tinf[ANIM+tile]);
				while (next != tile)
				{
					CA_MarkGrChunk(STARTTILE16+next);
					next += (signed char)(tinf[ANIM+next]);
					if (++anims > 20)
						Quit (gvar, "MarkTileGraphics: Unending animation!");
				}

			}
		}
nextback:
		info++;
	} while (start<end);

//
// foreground plane
//
	start = mapsegs[1];
	info = mapsegs[2];
	end = start+size;
	do
	{
		tile = *start++;
		if (tile>=0)			// <0 is a tile that is never drawn
		{
			CA_MarkGrChunk(STARTTILE16M+tile);
			if (tinf[MANIM+tile])
			{
				// this tile will animated

				tilehigh = tile | 0x8000;	// foreground tiles have high bit
				for (i=0;i<numanimchains;i++)
					if (allanims[i].current == tilehigh)
					{
						*info = (unsigned)&allanims[i];
						goto nextfront;
					}

				// new chain of animating tiles

				if (i>=MAXANIMTYPES)
					Quit (gvar, "RF_MarkTileGraphics: Too many unique animated tiles!");
				allanims[i].current = tilehigh;
				allanims[i].count = tinf[MSPEED+tile];

				*info = (unsigned)&allanims[i];
				numanimchains++;

				anims = 0;
				next = tile+(signed char)(tinf[MANIM+tile]);
				while (next != tile)
				{
					CA_MarkGrChunk(STARTTILE16M+next);
					next += (signed char)(tinf[MANIM+next]);
					if (++anims > 20)
						Quit (gvar, "MarkTileGraphics: Unending animation!");
				}

			}
		}
nextfront:
		info++;
	} while (start<end);
}
*/

//===========================================================================


/*
=========================
=
= RFL_InitAnimList
=
= Call to clear out the entire animating tile list and return all of them to
= the free list.
=
=========================
*/

void RFL_InitAnimList (void)
{
	int	i;

	animfreeptr = &animarray[0];

	for (i=0;i<MAXANIMTILES-1;i++)
		animarray[i].nexttile = &animarray[i+1];

	animarray[i].nexttile = NULL;

	animhead = NULL;			// nothing in list
}


/*
====================
=
= RFL_CheckForAnimTile
=
====================
*/
/*++++
void RFL_CheckForAnimTile (unsigned x, unsigned y)
{
	unsigned 	tile,offset,speed,lasttime,thistime,timemissed;
	unsigned	far *map;
	animtiletype	*anim,*next;

// the info plane of each animating tile has a near pointer into allanims[]
// which gives the current state of all concurrently animating tiles

	offset = mapbwidthtable[y]/2+x;

//
// background
//
	map = mapsegs[0]+offset;
	tile = *map;
	if (tinf[ANIM+tile] && tinf[SPEED+tile])
	{
		if (!animfreeptr)
			Quit (gvar, "RF_CheckForAnimTile: No free spots in tilearray!");
		anim = animfreeptr;
		animfreeptr = animfreeptr->nexttile;
		next = animhead;				// stick it at the start of the list
		animhead = anim;
		if (next)
			next->prevptr = &anim->nexttile;
		anim->nexttile = next;
		anim->prevptr = &animhead;

		anim->x = x;
		anim->y = y;
		anim->tile = tile;
		anim->mapplane = map;
		anim->chain = (tiletype *)*(mapsegs[2]+offset);
	}

//
// foreground
//
	map = mapsegs[1]+offset;
	tile = *map;
	if (tinf[MANIM+tile] && tinf[MSPEED+tile])
	{
		if (!animfreeptr)
			Quit (gvar, "RF_CheckForAnimTile: No free spots in tilearray!");
		anim = animfreeptr;
		animfreeptr = animfreeptr->nexttile;
		next = animhead;				// stick it at the start of the list
		animhead = anim;
		if (next)
			next->prevptr = &anim->nexttile;
		anim->nexttile = next;
		anim->prevptr = &animhead;

		anim->x = x;
		anim->y = y;
		anim->tile = tile;
		anim->mapplane = map;
		anim->chain = (tiletype *)*(mapsegs[2]+offset);
	}

}
*/

/*
====================
=
= RFL_RemoveAnimsOnX
=
====================
*/

void RFL_RemoveAnimsOnX (unsigned x)
{
	animtiletype *current,*next;

	current = animhead;
	while (current)
	{
		if (current->x == x)
		{
			*(void **)current->prevptr = current->nexttile;
			if (current->nexttile)
				current->nexttile->prevptr = current->prevptr;
			next = current->nexttile;
			current->nexttile = animfreeptr;
			animfreeptr = current;
			current = next;
		}
		else
			current = current->nexttile;
	}
}


/*
====================
=
= RFL_RemoveAnimsOnY
=
====================
*/

void RFL_RemoveAnimsOnY (unsigned y)
{
	animtiletype *current,*next;

	current = animhead;
	while (current)
	{
		if (current->y == y)
		{
			*(void **)current->prevptr = current->nexttile;
			if (current->nexttile)
				current->nexttile->prevptr = current->prevptr;
			next = current->nexttile;
			current->nexttile = animfreeptr;
			animfreeptr = current;
			current = next;
		}
		else
			current = current->nexttile;
	}
}


/*
====================
=
= RFL_RemoveAnimsInBlock
=
====================
*/

void RFL_RemoveAnimsInBlock (unsigned x, unsigned y, unsigned width, unsigned height)
{
	animtiletype *current,*next;

	current = animhead;
	while (current)
	{
		if (current->x - x < width && current->y - y < height)
		{
			*(void **)current->prevptr = current->nexttile;
			if (current->nexttile)
				current->nexttile->prevptr = current->prevptr;
			next = current->nexttile;
			current->nexttile = animfreeptr;
			animfreeptr = current;
			current = next;
		}
		else
			current = current->nexttile;
	}
}


/*
====================
=
= RFL_AnimateTiles
=
====================
*/
/*++++
void RFL_AnimateTiles (void)
{
	animtiletype *current;
	unsigned	updateofs,tile,x,y;
	tiletype	*anim;

//
// animate the lists of tiles
//
	anim = &allanims[0];
	while (anim->current)
	{
		anim->count-=tics;
		while ( anim->count < 1)
		{
			if (anim->current & 0x8000)
			{
				tile = anim->current & 0x7fff;
				tile += (signed char)tinf[MANIM+tile];
				anim->count += tinf[MSPEED+tile];
				tile |= 0x8000;
			}
			else
			{
				tile = anim->current;
				tile += (signed char)tinf[ANIM+tile];
				anim->count += tinf[SPEED+tile];
			}
			anim->current = tile;
		}
		anim++;
	}


//
// traverse the list of animating tiles
//
	current = animhead;
	while (current)
	{
		tile =current->chain->current;
		if ( tile != current->tile)
		{
		// tile has animated
		//
		// remove tile from master screen cache,
		// change a tile to its next state, set the structure up for
		// next animation, and post an update region to both update pages
		//
			current->tile = tile;

			*(current->mapplane) = tile & 0x7fff; 		// change in map

#if GRMODE == EGAGR
			if (tile<0x8000)		// background
				tilecache[tile] = 0;
#endif

			x = current->x-originxtile;
			y = current->y-originytile;

			if (x>=PORTTILESWIDE || y>=PORTTILESHIGH)
				Quit (gvar, "RFL_AnimateTiles: Out of bounds!");

			updateofs = uwidthtable[y] + x;
			RFL_NewTile(updateofs);				// puts "1"s in both pages
		}
		current = current->nexttile;
	}
}
*/

//===========================================================================

/*
=========================
=
= RFL_InitSpriteList
=
= Call to clear out the entire sprite list and return all of them to
= the free list.
=
=========================
*/

void RFL_InitSpriteList (void)
{
	int	i;

	spritefreeptr = &spritearray[0];
	for (i=0;i<MAXSPRITES-1;i++)
		spritearray[i].nextsprite = &spritearray[i+1];

	spritearray[i].nextsprite = NULL;

// NULL in all priority levels

	memset (prioritystart,0,sizeof(prioritystart));
}

//===========================================================================

/*
=================
=
= RFL_CalcOriginStuff
=
= Calculate all the global variables for a new position
= Long parms so position can be clipped to a maximum near 64k
=
=================
*/

void RFL_CalcOriginStuff (long x, long y)
{
	if (x<originxmin)
	  x=originxmin;
	else if (x>originxmax)
	  x=originxmax;

	if (y<originymin)
	  y=originymin;
	else if (y>originymax)
	  y=originymax;

	originxglobal = x;
	originyglobal = y;
	originxtile = originxglobal>>G_T_SHIFT;
	originytile = originyglobal>>G_T_SHIFT;
	originxscreen = originxtile<<SX_T_SHIFT;
	originyscreen = originytile<<SY_T_SHIFT;
	originmap = mapbwidthtable[originytile] + originxtile*2;

//#if GRMODE == EGAGR
	gvar->video.ofs.pan.panx = (originxglobal>>G_P_SHIFT) & 15;
	gvar->video.ofs.pan.pansx = gvar->video.ofs.pan.panx & 8;
	gvar->video.ofs.pan.pany = gvar->video.ofs.pan.pansy = (originyglobal>>G_P_SHIFT) & 15;
	gvar->video.ofs.pan.panadjust = gvar->video.ofs.pan.panx/8 + gvar->video.ofs.ylookup[gvar->video.ofs.pan.pany];
/*#endif

#if GRMODE == CGAGR
	gvar->video.ofs.pan.panx = (originxglobal>>G_P_SHIFT) & 15;
	gvar->video.ofs.pan.pansx = gvar->video.ofs.pan.panx & 12;
	gvar->video.ofs.pan.pany = gvar->video.ofs.pan.pansy = (originyglobal>>G_P_SHIFT) & 15;
	gvar->video.ofs.pan.panadjust = gvar->video.ofs.pan.pansx/4 + gvar->video.ofs.ylookup[gvar->video.ofs.pan.pansy];
#endif
	*/

}


/*
=================
=
= RFL_ClearScrollBlocks
=
=================
*/

void RFL_ClearScrollBlocks (void)
{
	gvar->video.ofs.pan.hscrollblocks = gvar->video.ofs.pan.vscrollblocks = 0;
}


/*
=================
=
= RF_SetScrollBlock
=
= Sets a horizontal or vertical scroll block
= a horizontal block is ----, meaning it blocks up/down movement
=
=================
*/

void RF_SetScrollBlock (int x, int y, boolean horizontal)
{
	if (horizontal)
	{
		gvar->video.ofs.pan.hscrolledge[gvar->video.ofs.pan.hscrollblocks] = y;
		if (gvar->video.ofs.pan.hscrollblocks++ == MAXSCROLLEDGES)
			Quit (gvar, "RF_SetScrollBlock: Too many horizontal scroll blocks");
	}
	else
	{
		gvar->video.ofs.pan.vscrolledge[gvar->video.ofs.pan.vscrollblocks] = x;
		if (gvar->video.ofs.pan.vscrollblocks++ == MAXSCROLLEDGES)
			Quit (gvar, "RF_SetScrollBlock: Too many vertical scroll blocks");
	}
}


/*
=================
=
= RFL_BoundScroll
=
= Bound a given x/y movement to scroll blocks
=
=================
*/

void RFL_BoundScroll (int x, int y)
{
	int	check,newxtile,newytile;

	originxglobal += x;
	originyglobal += y;

	newxtile= originxglobal >> G_T_SHIFT;
	newytile = originyglobal >> G_T_SHIFT;

	if (x>0)
	{
		newxtile+=SCREENTILESWIDE;
		for (check=0;check<gvar->video.ofs.pan.vscrollblocks;check++)
			if (gvar->video.ofs.pan.vscrolledge[check] == newxtile)
			{
				originxglobal = originxglobal&0xff00;
				break;
			}
	}
	else if (x<0)
	{
		for (check=0;check<gvar->video.ofs.pan.vscrollblocks;check++)
			if (gvar->video.ofs.pan.vscrolledge[check] == newxtile)
			{
				originxglobal = (originxglobal&0xff00)+0x100;
				break;
			}
	}


	if (y>0)
	{
		newytile+=SCREENTILESHIGH;
		for (check=0;check<gvar->video.ofs.pan.hscrollblocks;check++)
			if (gvar->video.ofs.pan.hscrolledge[check] == newytile)
			{
				originyglobal = originyglobal&0xff00;
				break;
			}
	}
	else if (y<0)
	{
		for (check=0;check<gvar->video.ofs.pan.hscrollblocks;check++)
			if (gvar->video.ofs.pan.hscrolledge[check] == newytile)
			{
				originyglobal = (originyglobal&0xff00)+0x100;
				break;
			}
	}


	RFL_CalcOriginStuff (originxglobal, originyglobal);
}


//===========================================================================

/*
=====================
=
= RF_SetRefreshHook
=
=====================
*/

void RF_SetRefreshHook (void (*func) (void) )
{
	refreshvector = func;
}


//===========================================================================

/*
=================
=
= RFL_NewRow
=
= Bring a new row of tiles onto the port, spawning animating tiles
=
=================
*/

void	RFL_NewRow (int dir)
{
	unsigned count,updatespot,updatestep;
	int		x,y,xstep,ystep;

	switch (dir)
	{
	case 0:		// top row
		updatespot = 0;
		updatestep = 1;
		x = originxtile;
		y = originytile;
		xstep = 1;
		ystep = 0;
		count = PORTTILESWIDE;
		break;

	case 1:		// right row
		updatespot = PORTTILESWIDE-1;
		updatestep = UPDATEWIDE;
		x = originxtile + PORTTILESWIDE-1;
		y = originytile;
		xstep = 0;
		ystep = 1;
		count = PORTTILESHIGH;
		break;

	case 2:		// bottom row
		updatespot = UPDATEWIDE*(PORTTILESHIGH-1);
		updatestep = 1;
		x = originxtile;
		y = originytile + PORTTILESHIGH-1;
		xstep = 1;
		ystep = 0;
		count = PORTTILESWIDE;
		break;

	case 3:		// left row
		updatespot = 0;
		updatestep = UPDATEWIDE;
		x = originxtile;
		y = originytile;
		xstep = 0;
		ystep = 1;
		count = PORTTILESHIGH;
		break;
	default:
		Quit (gvar, "RFL_NewRow: Bad dir!");
	}

	while (count--)
	{
		RFL_NewTile(updatespot);
		RFL_CheckForAnimTile (x,y);
		updatespot+=updatestep;
		x+=xstep;
		y+=ystep;
	}
}

//===========================================================================

/*
=====================
=
= RF_ForceRefresh
=
=====================
*/

void RF_ForceRefresh (void)
{
	RF_NewPosition (originxglobal,originyglobal);
	RF_Refresh ();
	RF_Refresh ();
}

//===========================================================================

/*
=====================
=
= RF_MapToMap
=
= Copies a block of tiles (all three planes) from one point
= in the map to another, accounting for animating tiles
=
=====================
*/
/*++++
void RF_MapToMap (unsigned srcx, unsigned srcy,
				  unsigned destx, unsigned desty,
				  unsigned width, unsigned height)
{
	int			x,y;
	unsigned	source,destofs,xspot,yspot;
	unsigned	linedelta,p0,p1,p2,updatespot;
	unsigned	far *source0, far *source1, far *source2;
	unsigned	far *dest0, far *dest1, far *dest2;
	boolean		changed;

	RFL_RemoveAnimsInBlock (destx,desty,width,height);

	source = mapbwidthtable[srcy]/2 + srcx;

	source0 = mapsegs[0]+source;
	source1 = mapsegs[1]+source;
	source2 = mapsegs[2]+source;

	destofs = mapbwidthtable[desty]/2 + destx;
	destofs -= source;

	linedelta = mapwidth - width;

	for (y=0;y<height;y++,source0+=linedelta,source1+=linedelta,source2+=linedelta)
		for (x=0;x<width;x++,source0++,source1++,source2++)
		{
			p0 = *source0;
			p1 = *source1;
			p2 = *source2;

			dest0 = source0 + destofs;
			dest1 = source1 + destofs;
			dest2 = source2 + destofs;

//
// only make a new tile if it is different
//
			if (p0 != *dest0 || p1 != *dest1 || p2 != *dest2)
			{
				*dest0 = p0;
				*dest1 = p1;
				*dest2 = p2;
				changed = true;
			}
			else
				changed = false;

//
// if tile is on the view port
//
			xspot = destx+x-originxtile;
			yspot = desty+y-originytile;
			if (yspot < PORTTILESHIGH && xspot < PORTTILESWIDE)
			{
				if (changed)
				{
					updatespot = uwidthtable[yspot]+xspot;
					RFL_NewTile(updatespot);
				}
				RFL_CheckForAnimTile (destx+x,desty+y);
			}
		}
}
*/
//===========================================================================


/*
=====================
=
= RF_MemToMap
=
= Copies a string of tiles from main memory to the map,
= accounting for animating tiles
=
=====================
*/
/*++++
void RF_MemToMap (unsigned far *source, unsigned plane,
				  unsigned destx, unsigned desty,
				  unsigned width, unsigned height)
{
	int			x,y;
	unsigned	xspot,yspot;
	unsigned	linedelta,updatespot;
	unsigned	far *dest,old,new;
	boolean		changed;

	RFL_RemoveAnimsInBlock (destx,desty,width,height);

	dest = mapsegs[plane] + mapbwidthtable[desty]/2 + destx;

	linedelta = mapwidth - width;

	for (y=0;y<height;y++,dest+=linedelta)
		for (x=0;x<width;x++)
		{
			old = *dest;
			new = *source++;
			if (old != new)
			{
				*dest = new;
				changed = true;
			}
			else
				changed = false;

			dest++;
			xspot = destx+x-originxtile;
			yspot = desty+y-originytile;
			if (yspot < PORTTILESHIGH && xspot < PORTTILESWIDE)
			{
				if (changed)
				{
					updatespot = uwidthtable[yspot]+xspot;
					RFL_NewTile(updatespot);
				}
				RFL_CheckForAnimTile (destx+x,desty+y);
			}
		}
}*/

//===========================================================================


/*
=====================
=
= RFL_BoundNewOrigin
=
= Copies a string of tiles from main memory to the map,
= accounting for animating tiles
=
=====================
*/

void RFL_BoundNewOrigin (unsigned orgx,unsigned orgy)
{
	int	check,edge;

//
// calculate new origin related globals
//
	if (orgx<originxmin)
	  orgx=originxmin;
	else if (orgx>originxmax)
	  orgx=originxmax;

	if (orgy<originymin)
	  orgy=originymin;
	else if (orgy>originymax)
	  orgy=originymax;

	originxtile = orgx>>G_T_SHIFT;
	originytile = orgy>>G_T_SHIFT;

	for (check=0;check<gvar->video.ofs.pan.vscrollblocks;check++)
	{
		edge = gvar->video.ofs.pan.vscrolledge[check];
		if (edge>=originxtile && edge <=originxtile+10)
		{
			orgx = (edge+1)*TILEGLOBAL;
			break;
		}
		if (edge>=originxtile+11 && edge <=originxtile+20)
		{
			orgx = (edge-20)*TILEGLOBAL;
			break;
		}
	}

	for (check=0;check<gvar->video.ofs.pan.hscrollblocks;check++)
	{
		edge = gvar->video.ofs.pan.hscrolledge[check];
		if (edge>=originytile && edge <=originytile+6)
		{
			orgy = (edge+1)*TILEGLOBAL;
			break;
		}
		if (edge>=originytile+7 && edge <=originytile+13)
		{
			orgy = (edge-13)*TILEGLOBAL;
			break;
		}
	}


	RFL_CalcOriginStuff (orgx,orgy);
}


//===========================================================================

/*
=====================
=
= RF_ClearBlock
=
= Posts erase blocks to clear a certain area of the screen to the master
= screen, to erase text or something draw directly to the screen
=
= Parameters in pixels, but erasure is byte bounded
=
=====================
*/

void RF_ClearBlock (int	x, int y, int width, int height)
{
	eraseblocktype block;

#if GRMODE == EGAGR
	block.screenx = x/8+originxscreen;
	block.screeny = y+originyscreen;
	block.width = (width+(x&7)+7)/8;
	block.height = height;
	memcpy (eraselistptr[0]++,&block,sizeof(block));
	memcpy (eraselistptr[1]++,&block,sizeof(block));
#endif

#if GRMODE == CGAGR
	block.screenx = x/4+originxscreen;
	block.screeny = y+originyscreen;
	block.width = (width+(x&3)+3)/4;
	block.height = height;
	memcpy (eraselistptr[0]++,&block,sizeof(block));
#endif

}

//===========================================================================

/*
=====================
=
= RF_RedrawBlock
=
= Causes a number of tiles to be redrawn to the master screen and updated
=
= Parameters in pixels, but erasure is tile bounded
=
=====================
*/

void RF_RedrawBlock (int x, int y, int width, int height)
{
	int	xx,yy,xl,xh,yl,yh;

	xl=(x+gvar->video.ofs.pan.panx)/16;
	xh=(x+gvar->video.ofs.pan.panx+width+15)/16;
	yl=(y+gvar->video.ofs.pan.pany)/16;
	yh=(y+gvar->video.ofs.pan.pany+height+15)/16;
	for (yy=yl;yy<=yh;yy++)
		for (xx=xl;xx<=xh;xx++)
			RFL_NewTile (yy*UPDATEWIDE+xx);
}


//===========================================================================

/*
=====================
=
= RF_CalcTics
=
=====================
*/

void RF_CalcTics (void)
{
	long	newtime,oldtimecount;
	word TimeCount = *clockw;

//
// calculate tics since last refresh for adaptive timing
//
	if (lasttimecount > TimeCount)
		TimeCount = lasttimecount;		// if the game was paused a LONG time

/*++++	if (DemoMode)					// demo recording and playback needs
	{								// to be constant
//
// take DEMOTICS or more tics, and modify Timecount to reflect time taken
//
		oldtimecount = lasttimecount;
		while (TimeCount<oldtimecount+DEMOTICS*2)
		;
		lasttimecount = oldtimecount + DEMOTICS;
		TimeCount = lasttimecount + DEMOTICS;
		tics = DEMOTICS;
	}
	else
	{*/
//
// non demo, so report actual time
//
		do
		{
			newtime = TimeCount;
			tics = newtime-lasttimecount;
		} while (tics<MINTICS);
		lasttimecount = newtime;

#ifdef PROFILE
			strcpy (scratch,"\tTics:");
			itoa (tics,str,10);
			strcat (scratch,str);
			strcat (scratch,"\n");
			write (profilehandle,scratch,strlen(scratch));
#endif

		if (tics>MAXTICS)
		{
			TimeCount -= (tics-MAXTICS);
			tics = MAXTICS;
		}
//	}
}

/*
=============================================================================

					EGA specific routines

=============================================================================
*/

#if GRMODE == EGAGR

/*
=====================
=
= RF_FindFreeBuffer
=
= Finds the start of unused, non visable buffer space
=
=====================
*/

unsigned RF_FindFreeBuffer (void)
{
	unsigned	spot,i,j;
	boolean		ok;

	for (i=0;i<3;i++)
	{
		spot = screenstart[i]+SCREENSPACE;
		ok = true;
		for (j=0;j<3;j++)
			if (spot == screenstart[j])
			{
				ok = false;
				break;
			}
		if (ok)
			return spot;
	}

	return 0;	// never get here...
}

//===========================================================================

/*
=====================
=
= RF_NewPosition EGA
=
=====================
*/

void RF_NewPosition (unsigned x, unsigned y)
{
	int mx,my;
	byte	*page0ptr,*page1ptr;
	unsigned 	updatenum;

	RFL_BoundNewOrigin (x,y);

// calculate new origin related globals
//
	RFL_CalcOriginStuff (x,y);

//
// clear out all animating tiles
//
	RFL_InitAnimList ();

//
// set up the new update arrays at base position
//
	memset (tilecache,0,sizeof(tilecache));		// old cache is invalid

	updatestart[0] = baseupdatestart[0];
	updatestart[1] = baseupdatestart[1];
	updateptr = updatestart[otherpage];

	page0ptr = updatestart[0]+PORTTILESWIDE;	// used to stick "0"s after rows
	page1ptr = updatestart[1]+PORTTILESWIDE;

	updatenum = 0;				// start at first visable tile

	for (my=0;my<PORTTILESHIGH;my++)
	{
		for (mx=0;mx<PORTTILESWIDE;mx++)
		{
			RFL_NewTile(updatenum);			// puts "1"s in both pages
			RFL_CheckForAnimTile(mx+originxtile,my+originytile);
			updatenum++;
		}
		updatenum++;
		*page0ptr = *page1ptr = 0; // set a 0 at end of a line of tiles
		page0ptr+=(PORTTILESWIDE+1);
		page1ptr+=(PORTTILESWIDE+1);
	}
	*(word *)(page0ptr-PORTTILESWIDE)
		= *(word *)(page1ptr-PORTTILESWIDE) = UPDATETERMINATE;
}

//===========================================================================

/*
=================
=
= RFL_OldRow EGA
=
= Uncache the trailing row of tiles
=
=================
*/

void	RFL_OldRow (unsigned updatespot,unsigned count,unsigned step)
{

asm	mov	si,[updatespot]			// pointer inside each map plane
asm	mov	cx,[count]				// number of tiles to clear
asm	mov	dx,[step]				// move to next tile
asm	mov	es,[WORD PTR mapsegs]			// background plane
asm	mov	ds,[WORD PTR mapsegs+2]			// foreground plane

clearcache:
asm	mov	bx,[si]
asm	or	bx,bx
asm	jnz	blockok					// if a foreground tile, block wasn't cached
asm	mov	bx,[es:si]
asm	shl	bx,1
asm	mov	[WORD PTR ss:tilecache+bx],0  //tile is no longer in master screen cache
blockok:
asm	add	si,dx
asm	loop	clearcache

asm	mov	ax,ss
asm	mov	ds,ax

}


/*
=====================
=
= RF_Scroll  EGA
=
= Move the origin x/y global coordinates, readjust the screen panning, and
= scroll if needed.  If the scroll distance is greater than one tile, the
= entire screen will be redrawn (this could be generalized, but scrolling
= more than one tile per refresh is a bad idea!).
=
=====================
*/

void RF_Scroll (int x, int y)
{
	long		neworgx,neworgy;
	int			i,deltax,deltay,absdx,absdy;
	int			oldxt,oldyt,move,yy;
	unsigned	updatespot;
	byte		*update0,*update1;
	unsigned	oldgvar->video.ofs.pan.panx,oldgvar->video.ofs.pan.panadjust,oldoriginmap,oldscreen,newscreen,screencopy;
	int			screenmove;

	oldxt = originxtile;
	oldyt = originytile;
	oldoriginmap = originmap;
	oldgvar->video.ofs.pan.panadjust = gvar->video.ofs.pan.panadjust;
	oldgvar->video.ofs.pan.panx = gvar->video.ofs.pan.panx;

	RFL_CalcOriginStuff ((long)originxglobal + x,(long)originyglobal + y);

	deltax = originxtile - oldxt;
	absdx = abs(deltax);
	deltay = originytile - oldyt;
	absdy = abs(deltay);

	if (absdx>1 || absdy>1)
	{
	//
	// scrolled more than one tile, so start from scratch
	//
		RF_NewPosition(originxglobal,originyglobal);
		return;
	}

	if (!absdx && !absdy)
		return;					// the screen has not scrolled an entire tile


//
// adjust screens and handle SVGA crippled compatability mode
//
	screenmove = deltay*16*SCREENWIDTH + deltax*TILEWIDTH;
	for (i=0;i<3;i++)
	{
		screenstart[i]+= screenmove;
		if (compatability && screenstart[i] > (0x10000l-SCREENSPACE) )
		{
			//
			// move the screen to the opposite end of the buffer
			//
			screencopy = screenmove>0 ? FREEEGAMEM : -FREEEGAMEM;
			oldscreen = screenstart[i] - screenmove;
			newscreen = oldscreen + screencopy;
			screenstart[i] = newscreen + screenmove;
//++++			VW_ScreenToScreen (oldscreen,newscreen,
				PORTTILESWIDE*2,PORTTILESHIGH*16);

//++++			if (i==screenpage)
//++++				VL_SetScreen(newscreen+oldgvar->video.ofs.pan.panadjust,oldgvar->video.ofs.pan.panx & xpanmask);
		}
	}
	bufferofs = screenstart[otherpage];
	displayofs = screenstart[screenpage];
	masterofs = screenstart[2];


//
// float the update regions
//
	move = deltax;
	if (deltay==1)
	  move += UPDATEWIDE;
	else if (deltay==-1)
	  move -= UPDATEWIDE;

	updatestart[0]+=move;
	updatestart[1]+=move;

//
// draw the new tiles just scrolled on to the master screen, and
// mark them as needing to be copied to each screen next refreshes
// Make sure a zero is at the end of each row in update
//

	if (deltax)
	{
		if (deltax==1)
		{
			RFL_NewRow (1);			// new right row
			RFL_OldRow (oldoriginmap,PORTTILESHIGH,mapbyteswide);
			RFL_RemoveAnimsOnX (originxtile-1);
		}
		else
		{
			RFL_NewRow (3);			// new left row
			RFL_OldRow (oldoriginmap+(PORTTILESWIDE-1)*2,PORTTILESHIGH
			,mapbyteswide);
			RFL_RemoveAnimsOnX (originxtile+PORTTILESWIDE);
		}

		update0 = updatestart[0]+PORTTILESWIDE;
		update1 = updatestart[1]+PORTTILESWIDE;
		for	(yy=0;yy<PORTTILESHIGH;yy++)
		{
			*update0 = *update1 = 0;	// drop a 0 at end of each row
			update0+=UPDATEWIDE;
			update1+=UPDATEWIDE;
		}
	}

//----------------

	if (deltay)
	{
		if (deltay==1)
		{
			RFL_NewRow (2);			// new bottom row
			RFL_OldRow (oldoriginmap,PORTTILESWIDE,2);
			updatespot = UPDATEWIDE*(PORTTILESHIGH-1);
			RFL_RemoveAnimsOnY (originytile-1);
		}
		else
		{
			RFL_NewRow (0);			// new top row
			RFL_OldRow (oldoriginmap+mapbwidthtable[PORTTILESHIGH-1]
			,PORTTILESWIDE,2);
			updatespot = 0;
			RFL_RemoveAnimsOnY (originytile+PORTTILESHIGH);
		}

		*(updatestart[0]+updatespot+PORTTILESWIDE) =
			*(updatestart[1]+updatespot+PORTTILESWIDE) = 0;
	}

//----------------

	//
	// place a new terminator
	//
	update0 = updatestart[0]+UPDATEWIDE*PORTTILESHIGH-1;
	update1 = updatestart[1]+UPDATEWIDE*PORTTILESHIGH-1;
	*update0++ = *update1++ = 0;
	*(unsigned *)update0 = *(unsigned *)update1 = UPDATETERMINATE;
}

//===========================================================================

/*
=====================
=
= RF_PlaceSprite   EGA
=
=====================
*/

void RF_PlaceSprite (void **user,unsigned globalx,unsigned globaly,
	unsigned spritenumber, drawtype draw, int priority)
{
	spritelisttype	register *sprite,*next;
	spritetabletype far *spr;
	spritetype _seg	*block;
	unsigned	shift,pixx;
	char		str[80],str2[10];

	if (!spritenumber || spritenumber == (unsigned)-1)
	{
		RF_RemoveSprite (user);
		return;
	}

	sprite = (spritelisttype *)*user;

	if	(sprite)
	{
	// sprite allready exists in the list, so we can use it's block

	//
	// post an erase block to both pages by copying screenx,screeny,width,height
	// both pages may not need to be erased if the sprite just changed last frame
	//
		if (sprite->updatecount<2)
		{
			if (!sprite->updatecount)
				memcpy (eraselistptr[otherpage]++,sprite,sizeof(eraseblocktype));
			memcpy (eraselistptr[screenpage]++,sprite,sizeof(eraseblocktype));
		}

		if (priority != sprite->priority)
		{
		// sprite mvoed to another priority, so unlink the old one and
		// relink it in the new priority

			next = sprite->nextsprite;			// cut old links
			if (next)
				next->prevptr = sprite->prevptr;
			*sprite->prevptr = next;
			goto linknewspot;
		}
	}
	else
	{
	// this is a brand new sprite, so allocate a block from the array

		if (!spritefreeptr)
			Quit (gvar, "RF_PlaceSprite: No free spots in spritearray!");

		sprite = spritefreeptr;
		spritefreeptr = spritefreeptr->nextsprite;

linknewspot:
		next = prioritystart[priority];		// stick it in new spot
		if (next)
			next->prevptr = &sprite->nextsprite;
		sprite->nextsprite = next;
		prioritystart[priority] = sprite;
		sprite->prevptr = &prioritystart[priority];
	}

//
// write the new info to the sprite
//
	spr = &spritetable[spritenumber-STARTSPRITES];
	block = (spritetype _seg *)grsegs[spritenumber];

	if (!block)
	{
		strcpy (str,"RF_PlaceSprite: Placed an uncached sprite:");
		itoa (spritenumber,str2,10);
		strcat (str,str2);
		Quit (str);
	}

	globaly+=spr->orgy;
	globalx+=spr->orgx;

	pixx = globalx >> G_SY_SHIFT;
	shift = (pixx&7)/2;

	sprite->screenx = pixx >> (G_EGASX_SHIFT-G_SY_SHIFT);
	sprite->screeny = globaly >> G_SY_SHIFT;
	sprite->width = block->width[shift];
	sprite->height = spr->height;
	sprite->grseg = spritenumber;
	sprite->sourceofs = block->sourceoffset[shift];
	sprite->planesize = block->planesize[shift];
	sprite->draw = draw;
	sprite->priority = priority;
	sprite->tilex = sprite->screenx >> SX_T_SHIFT;
	sprite->tiley = sprite->screeny >> SY_T_SHIFT;
	sprite->tilewide = ( (sprite->screenx + sprite->width -1) >> SX_T_SHIFT )
		- sprite->tilex + 1;
	sprite->tilehigh = ( (sprite->screeny + sprite->height -1) >> SY_T_SHIFT )
		- sprite->tiley + 1;

	sprite->updatecount = 2;		// draw on next two refreshes

// save the sprite pointer off in the user's pointer so it can be moved
// again later

	*user = sprite;
}

//===========================================================================

/*
=====================
=
= RF_RemoveSprite  EGA
=
=====================
*/

void RF_RemoveSprite (void **user)
{
	spritelisttype	*sprite,*next;

	sprite = (spritelisttype *)*user;
	if (!sprite)
		return;

//
// post an erase block to both pages by copying screenx,screeny,width,height
// both pages may not need to be erased if the sprite just changed last frame
//
	if (sprite->updatecount<2)
	{
		if (!sprite->updatecount)
			memcpy (eraselistptr[otherpage]++,sprite,sizeof(eraseblocktype));
		memcpy (eraselistptr[screenpage]++,sprite,sizeof(eraseblocktype));
	}

//
// unlink the sprite node
//
	next = sprite->nextsprite;
	if (next)						// if (!next), sprite is last in chain
		next->prevptr = sprite->prevptr;
	*sprite->prevptr = next;

//
// add it back to the free list
//
	sprite->nextsprite = spritefreeptr;
	spritefreeptr = sprite;

//
// null the users pointer, so next time that actor gets placed, it will
// allocate a new block
//

	*user = 0;
}


//===========================================================================


/*
====================
=
= RFL_EraseBlocks  EGA
=
= Write mode 1 should be set
=
====================
*/

void RFL_EraseBlocks (void)
{
	eraseblocktype	*block,*done;
	int			screenxh,screenyh;
	unsigned	pos,xtl,ytl,xth,yth,x,y;
	byte		*updatespot;
	unsigned	updatedelta;
	unsigned	erasecount;

#ifdef PROFILE
	erasecount = 0;
#endif

	block = otherpage ? &eraselist[1][0] : &eraselist[0][0];

	done = eraselistptr[otherpage];

	while (block != done)
	{

	//
	// clip the block to the current screen view
	//
		block->screenx -= originxscreen;
		block->screeny -= originyscreen;

		if (block->screenx < 0)
		{
			block->width += block->screenx;
			if (block->width<1)
				goto next;
			block->screenx = 0;
		}

		if (block->screeny < 0)
		{
			block->height += block->screeny;
			if (block->height<1)
				goto next;
			block->screeny = 0;
		}

		screenxh = block->screenx + block->width;
		screenyh = block->screeny + block->height;

		if (screenxh > EGAPORTSCREENWIDE)
		{
			block->width = EGAPORTSCREENWIDE-block->screenx;
			screenxh = block->screenx + block->width;
		}

		if (screenyh > PORTSCREENHIGH)
		{
			block->height = PORTSCREENHIGH-block->screeny;
			screenyh = block->screeny + block->height;
		}

		if (block->width<1 || block->height<1)
			goto next;

	//
	// erase the block by copying from the master screen
	//
		pos = gvar->video.ofs.ylookup[block->screeny]+block->screenx;
//++++		VW_ScreenToScreen (masterofs+pos,bufferofs+pos,
			block->width,block->height);

	//
	// put 2s in update where the block was, to force sprites to update
	//
		xtl = block->screenx >> SX_T_SHIFT;
		xth = (block->screenx+block->width-1) >> SX_T_SHIFT;
		ytl = block->screeny >> SY_T_SHIFT;
		yth = (block->screeny+block->height-1) >> SY_T_SHIFT;

		updatespot = updateptr + uwidthtable[ytl] + xtl;
		updatedelta = UPDATEWIDE - (xth-xtl+1);

		for (y=ytl;y<=yth;y++)
		{
			for (x=xtl;x<=xth;x++)
				*updatespot++ = 2;
			updatespot += updatedelta;		// down to next line
		}
#ifdef PROFILE
		erasecount++;
#endif

next:
		block++;
	}
	eraselistptr[otherpage] = otherpage ? &eraselist[1][0] : &eraselist[0][0];
#ifdef PROFILE
	strcpy (scratch,"\tErase:");
	itoa (erasecount,str,10);
	strcat (scratch,str);
	write (profilehandle,scratch,strlen(scratch));
#endif

}


/*
====================
=
= RFL_UpdateSprites EGA
=
= NOTE: Implement vertical clipping!
=
====================
*/

void RFL_UpdateSprites (void)
{
	spritelisttype	*sprite;
	int	portx,porty,x,y,xtl,xth,ytl,yth;
	int	priority;
	unsigned dest;
	byte		*updatespot,*baseupdatespot;
	unsigned	updatedelta;
	unsigned	updatecount;
	unsigned	height,sourceofs;

#ifdef PROFILE
	updatecount = 0;
#endif

	for (priority=0;priority<PRIORITIES;priority++)
	{
		if (priority==MASKEDTILEPRIORITY)
			RFL_MaskForegroundTiles ();

		for (sprite = prioritystart[priority]; sprite ;
			sprite = (spritelisttype *)sprite->nextsprite)
		{
		//
		// see if the sprite has any visable area in the port
		//

			portx = sprite->screenx - originxscreen;
			porty = sprite->screeny - originyscreen;
			xtl = portx >> SX_T_SHIFT;
			xth = (portx + sprite->width-1) >> SX_T_SHIFT;
			ytl = porty >> SY_T_SHIFT;
			yth = (porty + sprite->height-1) >> SY_T_SHIFT;

			if (xtl<0)
			  xtl = 0;
			if (xth>=PORTTILESWIDE)
			  xth = PORTTILESWIDE-1;
			if (ytl<0)
			  ytl = 0;
			if (yth>=PORTTILESHIGH)
			  yth = PORTTILESHIGH-1;

			if (xtl>xth || ytl>yth)
				continue;

		//
		// see if it's visable area covers any non 0 update tiles
		//
			updatespot = baseupdatespot = updateptr + uwidthtable[ytl] + xtl;
			updatedelta = UPDATEWIDE - (xth-xtl+1);

			if (sprite->updatecount)
			{
				sprite->updatecount--;			// the sprite was just placed,
				goto redraw;					// so draw it for sure
			}

			for (y=ytl;y<=yth;y++)
			{
				for (x=xtl;x<=xth;x++)
					if (*updatespot++)
						goto redraw;
				updatespot += updatedelta;		// down to next line
			}
			continue;							// no need to update

redraw:
		//
		// set the tiles it covers to 3, because those tiles are being
		// updated
		//
			updatespot = baseupdatespot;
			for (y=ytl;y<=yth;y++)
			{
				for (x=xtl;x<=xth;x++)
					*updatespot++ = 3;
				updatespot += updatedelta;		// down to next line
			}
		//
		// draw it!
		//
			height = sprite->height;
			sourceofs = sprite->sourceofs;
			if (porty<0)
			{
				height += porty;					// clip top off
				sourceofs -= porty*sprite->width;
				porty = 0;
			}
			else if (porty+height>PORTSCREENHIGH)
			{
				height = PORTSCREENHIGH - porty;    // clip bottom off
			}

			dest = bufferofs + gvar->video.ofs.ylookup[porty] + portx;

			switch (sprite->draw)
			{
			case spritedraw:
//++++				VW_MaskBlock(grsegs[sprite->grseg], sourceofs,
					dest,sprite->width,height,sprite->planesize);
				break;

			case maskdraw:
				break;

			}
#ifdef PROFILE
			updatecount++;
#endif


		}
	}
#ifdef PROFILE
	strcpy (scratch,"\tSprites:");
	itoa (updatecount,str,10);
	strcat (scratch,str);
	write (profilehandle,scratch,strlen(scratch));
#endif

}


/*
=====================
=
= RF_Refresh   EGA
=
= All routines will draw at the port at bufferofs, possibly copying from
= the port at masterofs.  The EGA version then page flips, while the
= CGA version updates the screen from the buffer port.
=
= Screenpage is the currently displayed page, not the one being drawn
= Otherpage is the page to be worked with now
=
=====================
*/

void RF_Refresh (void)
{
	byte	*newupdate;

	updateptr = updatestart[otherpage];

	RFL_AnimateTiles ();		// DEBUG

//
// update newly scrolled on tiles and animated tiles from the master screen
//
	EGAWRITEMODE(1);
	EGAMAPMASK(15);
	RFL_UpdateTiles ();
	RFL_EraseBlocks ();

//
// Update is all 0 except where sprites have changed or new area has
// been scrolled on.  Go through all sprites and update the ones that cover
// a non 0 update tile
//
	EGAWRITEMODE(0);
	RFL_UpdateSprites ();

//
// if the main program has a refresh hook set, call their function before
// displaying the new page
//
	if (refreshvector)
		refreshvector();

//
// display the changed screen
//
	VL_SetScreen(bufferofs+gvar->video.ofs.pan.panadjust,gvar->video.ofs.pan.panx & xpanmask);

//
// prepare for next refresh
//
// Set the update array to the middle position and clear it out to all "0"s
// with an UPDATETERMINATE at the end
//
	updatestart[otherpage] = newupdate = baseupdatestart[otherpage];
asm	mov	ax,ds
asm	mov	es,ax
asm	xor	ax,ax
asm	mov	cx,(UPDATESCREENSIZE-2)/2
asm	mov	di,[newupdate]
asm	rep	stosw
asm	mov	[WORD PTR es:di],UPDATETERMINATE

	screenpage ^= 1;
	otherpage ^= 1;
	bufferofs = screenstart[otherpage];
	displayofs = screenstart[screenpage];

//
// calculate tics since last refresh for adaptive timing
//
	RF_CalcTics ();
}

#endif		// GRMODE == EGAGR

/*
=============================================================================

					CGA specific routines

=============================================================================
*/

#if GRMODE == CGAGR


/*
=====================
=
= RF_NewPosition   CGA
=
=====================
*/

void RF_NewPosition (unsigned x, unsigned y)
{
	int mx,my;
	byte	*spotptr;
	unsigned 	updatenum;

	RFL_BoundNewOrigin (x,y);

// calculate new origin related globals
//
	RFL_CalcOriginStuff (x,y);

//
// clear out all animating tiles
//
	RFL_InitAnimList ();

//
// set up the new update arrays at base position
//
	updateptr = baseupdateptr;

	spotptr = updateptr + PORTTILESWIDE;	// used to stick "0"s after rows

	updatenum = 0;				// start at first visable tile

	for (my=0;my<PORTTILESHIGH;my++)
	{
		for (mx=0;mx<PORTTILESWIDE;mx++)
		{
			RFL_NewTile(updatenum);			// puts "1"s in both pages
			RFL_CheckForAnimTile(mx+originxtile,my+originytile);
			updatenum++;
		}
		updatenum++;
		*spotptr = 0; // set a 0 at end of a line of tiles
		spotptr +=(PORTTILESWIDE+1);
	}
	*(word *)(spotptr-PORTTILESWIDE) = UPDATETERMINATE;
}


/*
=====================
=
= RF_Scroll       CGA
=
= Move the origin x/y global coordinates, readjust the screen panning, and
= scroll if needed.  If the scroll distance is greater than one tile, the
= entire screen will be redrawn (this could be generalized, but scrolling
= more than one tile per refresh is a bad idea!).
=
=====================
*/

void RF_Scroll (int x, int y)
{
	long		neworgx,neworgy;
	int			i,deltax,deltay,absdx,absdy;
	int			oldxt,oldyt,move,yy;
	unsigned	updatespot;
	byte		*spotptr;
	unsigned	oldoriginmap,oldscreen,newscreen,screencopy;
	int			screenmove;

	oldxt = originxtile;
	oldyt = originytile;

	RFL_CalcOriginStuff ((long)originxglobal + x,(long)originyglobal + y);

	deltax = originxtile - oldxt;
	absdx = abs(deltax);
	deltay = originytile - oldyt;
	absdy = abs(deltay);

	if (absdx>1 || absdy>1)
	{
	//
	// scrolled more than one tile, so start from scratch
	//
		RF_NewPosition(originxglobal,originyglobal);
		return;
	}

	if (!absdx && !absdy)
		return;					// the screen has not scrolled an entire tile


//
// float screens
//
	screenmove = deltay*16*SCREENWIDTH + deltax*TILEWIDTH;
	bufferofs += screenmove;
	masterofs += screenmove;


//
// float the update regions
//
	move = deltax;
	if (deltay==1)
	  move += UPDATEWIDE;
	else if (deltay==-1)
	  move -= UPDATEWIDE;

	updateptr+=move;

//
// draw the new tiles just scrolled on to the master screen, and
// mark them as needing to be copied to each screen next refreshes
// Make sure a zero is at the end of each row in update
//

	if (deltax)
	{
		if (deltax==1)
		{
			RFL_NewRow (1);			// new right row
			RFL_RemoveAnimsOnX (originxtile-1);
		}
		else
		{
			RFL_NewRow (3);			// new left row
			RFL_RemoveAnimsOnX (originxtile+PORTTILESWIDE);
		}

		spotptr = updateptr+PORTTILESWIDE;
		for	(yy=0;yy<PORTTILESHIGH;yy++)
		{
			*spotptr = 0;		// drop a 0 at end of each row
			spotptr+=UPDATEWIDE;
		}
	}

//----------------

	if (deltay)
	{
		if (deltay==1)
		{
			RFL_NewRow (2);			// new bottom row
			*(updateptr+UPDATEWIDE*(PORTTILESHIGH-1)+PORTTILESWIDE) = 0;
			RFL_RemoveAnimsOnY (originytile-1);
		}
		else
		{
			RFL_NewRow (0);			// new top row
			*(updateptr+PORTTILESWIDE) = 0;
			RFL_RemoveAnimsOnY (originytile+PORTTILESHIGH);
		}
	}

//----------------

	//
	// place a new terminator
	//
	spotptr = updateptr+UPDATEWIDE*PORTTILESHIGH-1;
	*spotptr++ = 0;
	*(unsigned *)spotptr = UPDATETERMINATE;
}

/*
=====================
=
= RF_PlaceSprite  CGA
=
=====================
*/

void RF_PlaceSprite (void **user,unsigned globalx,unsigned globaly,
	unsigned spritenumber, drawtype draw, int priority)
{
	spritelisttype	register *sprite,*next;
	spritetabletype far *spr;
	spritetype _seg	*block;
	unsigned	shift,pixx;
	char		str[80],str2[10];

	if (!spritenumber || spritenumber == (unsigned)-1)
	{
		RF_RemoveSprite (user);
		return;
	}

	sprite = (spritelisttype *)*user;

	if	(sprite)
	{
	// sprite allready exists in the list, so we can use it's block

	//
	// post an erase block to erase the old position by copying
	// screenx,screeny,width,height
	//
		if (!sprite->updatecount)		// may not have been drawn at all yet
			memcpy (eraselistptr[0]++,sprite,sizeof(eraseblocktype));

		if (priority != sprite->priority)
		{
		// sprite moved to another priority, so unlink the old one and
		// relink it in the new priority

			next = sprite->nextsprite;			// cut old links
			if (next)
				next->prevptr = sprite->prevptr;
			*sprite->prevptr = next;
			goto linknewspot;
		}
	}
	else
	{
	// this is a brand new sprite, so allocate a block from the array

		if (!spritefreeptr)
			Quit (gvar, "RF_PlaceSprite: No free spots in spritearray!");

		sprite = spritefreeptr;
		spritefreeptr = spritefreeptr->nextsprite;

linknewspot:
		next = prioritystart[priority];		// stick it in new spot
		if (next)
			next->prevptr = &sprite->nextsprite;
		sprite->nextsprite = next;
		prioritystart[priority] = sprite;
		sprite->prevptr = &prioritystart[priority];
	}

//
// write the new info to the sprite
//
	spr = &spritetable[spritenumber-STARTSPRITES];
	block = (spritetype _seg *)grsegs[spritenumber];

	if (!block)
	{
		strcpy (str,"RF_PlaceSprite: Placed an uncached sprite!");
		itoa (spritenumber,str2,10);
		strcat (str,str2);
		Quit (str);
	}


	globaly+=spr->orgy;
	globalx+=spr->orgx;

	sprite->screenx = globalx >> G_CGASX_SHIFT;
	sprite->screeny = globaly >> G_SY_SHIFT;
	sprite->width = block->width[0];
	sprite->height = spr->height;
	sprite->grseg = spritenumber;
	sprite->sourceofs = block->sourceoffset[0];
	sprite->planesize = block->planesize[0];
	sprite->draw = draw;
	sprite->priority = priority;
	sprite->tilex = sprite->screenx >> SX_T_SHIFT;
	sprite->tiley = sprite->screeny >> SY_T_SHIFT;
	sprite->tilewide = ( (sprite->screenx + sprite->width -1) >> SX_T_SHIFT )
		- sprite->tilex + 1;
	sprite->tilehigh = ( (sprite->screeny + sprite->height -1) >> SY_T_SHIFT )
		- sprite->tiley + 1;

	sprite->updatecount = 1;		// draw on next refresh

// save the sprite pointer off in the user's pointer so it can be moved
// again later

	*user = sprite;
}

//===========================================================================

/*
=====================
=
= RF_RemoveSprite CGA
=
=====================
*/

void RF_RemoveSprite (void **user)
{
	spritelisttype	*sprite,*next;

	sprite = (spritelisttype *)*user;
	if (!sprite)
		return;

//
// post an erase block to erase the old position by copying
// screenx,screeny,width,height
//
	if (!sprite->updatecount)
	{
		memcpy (eraselistptr[0]++,sprite,sizeof(eraseblocktype));
	}

//
// unlink the sprite node
//
	next = sprite->nextsprite;
	if (next)						// if (!next), sprite is last in chain
		next->prevptr = sprite->prevptr;
	*sprite->prevptr = next;

//
// add it back to the free list
//
	sprite->nextsprite = spritefreeptr;
	spritefreeptr = sprite;

//
// null the users pointer, so next time that actor gets placed, it will
// allocate a new block
//

	*user = 0;
}


/*
====================
=
= RFL_EraseBlocks CGA
=
= Write mode 1 should be set
=
====================
*/

void RFL_EraseBlocks (void)
{
	eraseblocktype	*block,*done;
	int			screenxh,screenyh;
	unsigned	pos,xtl,ytl,xth,yth,x,y;
	byte		*updatespot;
	unsigned	updatedelta;

	block = &eraselist[0][0];

	done = eraselistptr[0];

	while (block != done)
	{

	//
	// clip the block to the current screen view
	//
		block->screenx -= originxscreen;
		block->screeny -= originyscreen;

		if (block->screenx < 0)
		{
			block->width += block->screenx;
			if (block->width<1)
				goto next;
			block->screenx = 0;
		}

		if (block->screeny < 0)
		{
			block->height += block->screeny;
			if (block->height<1)
				goto next;
			block->screeny = 0;
		}

		screenxh = block->screenx + block->width;
		screenyh = block->screeny + block->height;

		if (screenxh > CGAPORTSCREENWIDE)
		{
			block->width = CGAPORTSCREENWIDE-block->screenx;
			screenxh = block->screenx + block->width;
		}

		if (screenyh > PORTSCREENHIGH)
		{
			block->height = PORTSCREENHIGH-block->screeny;
			screenyh = block->screeny + block->height;
		}

		if (block->width<1 || block->height<1)
			goto next;

	//
	// erase the block by copying from the master screen
	//
		pos = gvar->video.ofs.ylookup[block->screeny]+block->screenx;
		block->width = (block->width + (pos&1) + 1)& ~1;
		pos &= ~1;				// make sure a word copy gets used
//++++		VW_ScreenToScreen (masterofs+pos,bufferofs+pos,
			block->width,block->height);

	//
	// put 2s in update where the block was, to force sprites to update
	//
		xtl = block->screenx >> SX_T_SHIFT;
		xth = (block->screenx+block->width-1) >> SX_T_SHIFT;
		ytl = block->screeny >> SY_T_SHIFT;
		yth = (block->screeny+block->height-1) >> SY_T_SHIFT;

		updatespot = updateptr + uwidthtable[ytl] + xtl;
		updatedelta = UPDATEWIDE - (xth-xtl+1);

		for (y=ytl;y<=yth;y++)
		{
			for (x=xtl;x<=xth;x++)
				*updatespot++ = 2;
			updatespot += updatedelta;		// down to next line
		}

next:
		block++;
	}
	eraselistptr[0] = &eraselist[0][0];
}


/*
====================
=
= RFL_UpdateSprites      CGA
=
= NOTE: Implement vertical clipping!
=
====================
*/

void RFL_UpdateSprites (void)
{
	spritelisttype	*sprite;
	int	portx,porty,x,y,xtl,xth,ytl,yth;
	int	priority;
	unsigned dest;
	byte		*updatespot,*baseupdatespot;
	unsigned	updatedelta;

	unsigned	updatecount;
	unsigned	height,sourceofs;

#ifdef PROFILE
	updatecount = 0;
#endif


	for (priority=0;priority<PRIORITIES;priority++)
	{
		if (priority==MASKEDTILEPRIORITY)
			RFL_MaskForegroundTiles ();

		for (sprite = prioritystart[priority]; sprite ;
			sprite = (spritelisttype *)sprite->nextsprite)
		{
		//
		// see if the sprite has any visable area in the port
		//

			portx = sprite->screenx - originxscreen;
			porty = sprite->screeny - originyscreen;
			xtl = portx >> SX_T_SHIFT;
			xth = (portx + sprite->width-1) >> SX_T_SHIFT;
			ytl = porty >> SY_T_SHIFT;
			yth = (porty + sprite->height-1) >> SY_T_SHIFT;

			if (xtl<0)
			  xtl = 0;
			if (xth>=PORTTILESWIDE)
			  xth = PORTTILESWIDE-1;
			if (ytl<0)
			  ytl = 0;
			if (yth>=PORTTILESHIGH)
			  yth = PORTTILESHIGH-1;

			if (xtl>xth || ytl>yth)
				continue;

		//
		// see if it's visable area covers any non 0 update tiles
		//
			updatespot = baseupdatespot = updateptr + uwidthtable[ytl] + xtl;
			updatedelta = UPDATEWIDE - (xth-xtl+1);

			if (sprite->updatecount)
			{
				sprite->updatecount--;			// the sprite was just placed,
				goto redraw;					// so draw it for sure
			}

			for (y=ytl;y<=yth;y++)
			{
				for (x=xtl;x<=xth;x++)
					if (*updatespot++)
						goto redraw;
				updatespot += updatedelta;		// down to next line
			}
			continue;							// no need to update

redraw:
		//
		// set the tiles it covers to 3, because those tiles are being
		// updated
		//
			updatespot = baseupdatespot;
			for (y=ytl;y<=yth;y++)
			{
				for (x=xtl;x<=xth;x++)
					*updatespot++ = 3;
				updatespot += updatedelta;		// down to next line
			}
		//
		// draw it!
		//
			height = sprite->height;
			sourceofs = sprite->sourceofs;
			if (porty<0)
			{
				height += porty;					// clip top off
				sourceofs -= porty*sprite->width;
				porty = 0;
			}
			else if (porty+height>PORTSCREENHIGH)
			{
				height = PORTSCREENHIGH - porty;    // clip bottom off
			}

			dest = bufferofs + gvar->video.ofs.ylookup[porty] + portx;

			switch (sprite->draw)
			{
			case spritedraw:
				VW_MaskBlock(grsegs[sprite->grseg], sourceofs,
					dest,sprite->width,height,sprite->planesize);
				break;

			case maskdraw:
				break;

			}
#ifdef PROFILE
			updatecount++;
#endif


		}
	}
}


/*
=====================
=
= RF_Refresh        CGA
=
= All routines will draw at the port at bufferofs, possibly copying from
= the port at masterofs.  The EGA version then page flips, while the
= CGA version updates the screen from the buffer port.
=
= Screenpage is the currently displayed page, not the one being drawn
= Otherpage is the page to be worked with now
=
=====================
*/

void RF_Refresh (void)
{
	long newtime;

	RFL_AnimateTiles ();

//
// update newly scrolled on tiles and animated tiles from the master screen
//
	RFL_UpdateTiles ();
	RFL_EraseBlocks ();

//
// Update is all 0 except where sprites have changed or new area has
// been scrolled on.  Go through all sprites and update the ones that cover
// a non 0 update tile
//
	RFL_UpdateSprites ();

//
// if the main program has a refresh hook set, call their function before
// displaying the new page
//
	if (refreshvector)
		refreshvector();

//
// update everything to the screen
//
	VW_CGAFullUpdate ();

//
// calculate tics since last refresh for adaptive timing
//
	RF_CalcTics ();
}

#endif		// GRMODE == CGAGR
//===============================
/*
; Keen Dreams Source Code
; Copyright (C) 2014 Javier M. Chavez
;
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License along
; with this program; if not, write to the Free Software Foundation, Inc.,
; 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

; ID_RF_A.ASM

IDEAL
MODEL	MEDIUM,C

INCLUDE	"ID_ASM.EQU"

CACHETILES	= 1		;enable master screen tile caching

;============================================================================

TILESWIDE	=	21
TILESHIGH	=	14

UPDATESIZE	=	(TILESWIDE+1)*TILESHIGH+1

DATASEG

EXTRN	screenseg:WORD
EXTRN	updateptr:WORD
EXTRN	updatestart:WORD
EXTRN	masterofs:WORD		;start of master tile port
EXTRN	bufferofs:WORD		;start of current buffer port
EXTRN	screenstart:WORD	;starts of three screens (0/1/master) in EGA mem
EXTRN	grsegs:WORD
EXTRN	mapsegs:WORD
EXTRN	originmap:WORD
EXTRN	updatemapofs:WORD
EXTRN	tilecache:WORD
EXTRN	tinf:WORD			;seg pointer to map header and tile info
EXTRN	blockstarts:WORD	;offsets from bufferofs for each update block

planemask	db	?
planenum	db	?

CODESEG

screenstartcs	dw	?		;in code segment for accesability




IFE GRMODE-CGAGR
;============================================================================
;
; CGA refresh routines
;
;============================================================================

TILEWIDTH	=	4

;=================
;
; RFL_NewTile
;
; Draws a composit two plane tile to the master screen and sets the update
; spot to 1 in both update pages, forcing the tile to be copied to the
; view pages the next two refreshes
;
; Called to draw newlly scrolled on strips and animating tiles
;
;=================

PROC	RFL_NewTile	updateoffset:WORD
PUBLIC	RFL_NewTile
USES	SI,DI

;
; mark both update lists at this spot
;
	mov	di,[updateoffset]

	mov	bx,[updateptr]			;start of update matrix
	mov	[BYTE bx+di],1

	mov	dx,SCREENWIDTH-TILEWIDTH		;add to get to start of next line

;
; set di to the location in screenseg to draw the tile
;
	shl	di,1
	mov	si,[updatemapofs+di]	;offset in map from origin
	add	si,[originmap]
	mov	di,[blockstarts+di]		;screen location for tile
	add	di,[masterofs]

;
; set BX to the foreground tile number and SI to the background number
; If either BX or SI = 0xFFFF, the tile does not need to be masked together
; as one of the planes totally eclipses the other
;
	mov	es,[mapsegs+2]			;foreground plane
	mov	bx,[es:si]
	mov	es,[mapsegs]			;background plane
	mov	si,[es:si]

	mov	es,[screenseg]

	or	bx,bx
	jz	@@singletile
	jmp	@@maskeddraw			;draw both together

;=============
;
; Draw single background tile from main memory
;
;=============

@@singletile:
	shl	si,1
	mov	ds,[grsegs+STARTTILE16*2+si]

	xor	si,si					;block is segment aligned

REPT	15
	movsw
	movsw
	add	di,dx
ENDM
	movsw
	movsw

	mov	ax,ss
	mov	ds,ax					;restore turbo's data segment
	ret


;=========
;
; Draw a masked tile combo
; Interupts are disabled and the stack segment is reassigned
;
;=========
@@maskeddraw:
	cli							; don't allow ints when SS is set
	shl	bx,1
	mov	ss,[grsegs+STARTTILE16M*2+bx]
	shl	si,1
	mov	ds,[grsegs+STARTTILE16*2+si]

	xor	si,si					;first word of tile data

REPT	16
	mov	ax,[si]					;background tile
	and	ax,[ss:si]				;mask
	or	ax,[ss:si+64]			;masked data
	stosw
	mov	ax,[si+2]				;background tile
	and	ax,[ss:si+2]			;mask
	or	ax,[ss:si+66]			;masked data
	stosw
	add	si,4
	add	di,dx
ENDM

	mov	ax,@DATA
	mov	ss,ax
	sti
	mov	ds,ax
	ret
ENDP

ENDIF



IFE GRMODE-EGAGR
;===========================================================================
;
; EGA refresh routines
;
;===========================================================================

TILEWIDTH	=	2

;=================
;
; RFL_NewTile
;
; Draws a composit two plane tile to the master screen and sets the update
; spot to 1 in both update pages, forcing the tile to be copied to the
; view pages the next two refreshes
;
; Called to draw newlly scrolled on strips and animating tiles
;
; Assumes write mode 0
;
;=================

PROC	RFL_NewTile	updateoffset:WORD
PUBLIC	RFL_NewTile
USES	SI,DI

;
; mark both update lists at this spot
;
	mov	di,[updateoffset]

	mov	bx,[updatestart]		;page 0 pointer
	mov	[BYTE bx+di],1
	mov	bx,[updatestart+2]		;page 1 pointer
	mov	[BYTE bx+di],1

;
; set screenstartcs to the location in screenseg to draw the tile
;
	shl	di,1
	mov	si,[updatemapofs+di]	;offset in map from origin
	add	si,[originmap]
	mov	di,[blockstarts+di]		;screen location for tile
	add	di,[masterofs]
	mov	[cs:screenstartcs],di

;
; set BX to the foreground tile number and SI to the background number
; If either BX or SI = 0xFFFF, the tile does not need to be masked together
; as one of the planes totally eclipses the other
;
	mov	es,[mapsegs+2]			;foreground plane
	mov	bx,[es:si]
	mov	es,[mapsegs]			;background plane
	mov	si,[es:si]

	mov	es,[screenseg]
	mov	dx,SC_INDEX				;for stepping through map mask planes

	or	bx,bx
	jz	@@singletile
	jmp	@@maskeddraw			;draw both together

;=========
;
; No foreground tile, so draw a single background tile.
; Use the master screen cache if possible
;
;=========
@@singletile:

	mov	bx,SCREENWIDTH-2		;add to get to start of next line
	shl	si,1

IFE CACHETILES
	jmp	@@singlemain
ENDIF

	mov	ax,[tilecache+si]
	or	ax,ax
	jz	@@singlemain
;=============
;
; Draw single tile from cache
;
;=============

	mov	si,ax

	mov	ax,SC_MAPMASK + 15*256	;all planes
	WORDOUT

	mov	dx,GC_INDEX
	mov	ax,GC_MODE + 1*256		;write mode 1
	WORDOUT

	mov	di,[cs:screenstartcs]
	mov	ds,[screenseg]

REPT	15
	movsb
	movsb
	add	si,bx
	add	di,bx
ENDM
	movsb
	movsb

	xor	ah,ah					;write mode 0
	WORDOUT

	mov	ax,ss
	mov	ds,ax					;restore turbo's data segment
	ret

;=============
;
; Draw single tile from main memory
;
;=============

@@singlemain:
	mov	ax,[cs:screenstartcs]
	mov	[tilecache+si],ax		;next time it can be drawn from here with latch
	mov	ds,[grsegs+STARTTILE16*2+si]

	xor	si,si					;block is segment aligned

	mov	ax,SC_MAPMASK+0001b*256	;map mask for plane 0

	mov	cx,4					;draw four planes
@@planeloop:
	mov	dx,SC_INDEX
	WORDOUT

	mov	di,[cs:screenstartcs]	;start at same place in all planes

REPT	15
	movsw
	add	di,bx
ENDM
	movsw

	shl	ah,1					;shift plane mask over for next plane
	loop	@@planeloop

	mov	ax,ss
	mov	ds,ax					;restore turbo's data segment
	ret


;=========
;
; Draw a masked tile combo
; Interupts are disabled and the stack segment is reassigned
;
;=========
@@maskeddraw:
	cli							; don't allow ints when SS is set
	shl	bx,1
	mov	ss,[grsegs+STARTTILE16M*2+bx]
	shl	si,1
	mov	ds,[grsegs+STARTTILE16*2+si]

	xor	si,si					;first word of tile data

	mov	ax,SC_MAPMASK+0001b*256	;map mask for plane 0

	mov	di,[cs:screenstartcs]
@@planeloopm:
	WORDOUT
tileofs		=	0
lineoffset	=	0
REPT	16
	mov	bx,[si+tileofs]			;background tile
	and	bx,[ss:tileofs]			;mask
	or	bx,[ss:si+tileofs+32]	;masked data
	mov	[es:di+lineoffset],bx
tileofs		=	tileofs + 2
lineoffset	=	lineoffset + SCREENWIDTH
ENDM
	add	si,32
	shl	ah,1					;shift plane mask over for next plane
	cmp	ah,10000b
	je	@@done					;drawn all four planes
	jmp	@@planeloopm

@@done:
	mov	ax,@DATA
	mov	ss,ax
	sti
	mov	ds,ax
	ret
ENDP

ENDIF

IFE GRMODE-VGAGR
;============================================================================
;
; VGA refresh routines
;
;============================================================================


ENDIF


;============================================================================
;
; reasonably common refresh routines
;
;============================================================================


;=================
;
; RFL_UpdateTiles
;
; Scans through the update matrix pointed to by updateptr, looking for 1s.
; A 1 represents a tile that needs to be copied from the master screen to the
; current screen (a new row or an animated tiled).  If more than one adjacent
; tile in a horizontal row needs to be copied, they will be copied as a group.
;
; Assumes write mode 1
;
;=================


; AX	0/1 for scasb, temp for segment register transfers
; BX    width for block copies
; CX	REP counter
; DX	line width deltas
; SI	source for copies
; DI	scas dest / movsb dest
; BP	pointer to UPDATETERMINATE
;
; DS
; ES
; SS

PROC	RFL_UpdateTiles
PUBLIC	RFL_UpdateTiles
USES	SI,DI,BP

	jmp	SHORT @@realstart
@@done:
;
; all tiles have been scanned
;
	ret

@@realstart:
	mov	di,[updateptr]
	mov	bp,(TILESWIDE+1)*TILESHIGH+1
	add	bp,di					; when di = bx, all tiles have been scanned
	push	di
	mov	cx,-1					; definately scan the entire thing

;
; scan for a 1 in the update list, meaning a tile needs to be copied
; from the master screen to the current screen
;
@@findtile:
	pop	di						; place to continue scaning from
	mov	ax,ss
	mov	es,ax					; search in the data segment
	mov	ds,ax
	mov al,1
	repne	scasb
	cmp	di,bp
	je	@@done

	cmp	[BYTE di],al
	jne	@@singletile
	jmp	@@tileblock

;============
;
; copy a single tile
;
;============
EVEN
@@singletile:
	inc	di						; we know the next tile is nothing
	push	di					; save off the spot being scanned
	sub	di,[updateptr]
	shl	di,1
	mov	di,[blockstarts-4+di]	; start of tile location on screen
	mov	si,di
	add	di,[bufferofs]			; dest in current screen
	add	si,[masterofs]			; source in master screen

	mov	dx,SCREENWIDTH-TILEWIDTH
	mov	ax,[screenseg]
	mov	ds,ax
	mov	es,ax

;--------------------------

IFE GRMODE-CGAGR

REPT	15
	movsw
	movsw
	add	si,dx
	add	di,dx
ENDM
	movsw
	movsw

ENDIF

;--------------------------

IFE GRMODE-EGAGR

REPT	15
	movsb
	movsb
	add	si,dx
	add	di,dx
ENDM
	movsb
	movsb

ENDIF

;--------------------------

	jmp	@@findtile

;============
;
; more than one tile in a row needs to be updated, so do it as a group
;
;============
EVEN
@@tileblock:
	mov	dx,di					; hold starting position + 1 in dx
	inc	di						; we know the next tile also gets updated
	repe	scasb				; see how many more in a row
	push	di					; save off the spot being scanned

	mov	bx,di
	sub	bx,dx					; number of tiles in a row
	shl	bx,1					; number of bytes / row

	mov	di,dx					; lookup position of start tile
	sub	di,[updateptr]
	shl	di,1
	mov	di,[blockstarts-2+di]	; start of tile location
	mov	si,di
	add	di,[bufferofs]			; dest in current screen
	add	si,[masterofs]			; source in master screen

	mov	dx,SCREENWIDTH
	sub	dx,bx					; offset to next line on screen
IFE GRMODE-CGAGR
	sub	dx,bx					; bx is words wide in CGA tiles
ENDIF

	mov	ax,[screenseg]
	mov	ds,ax
	mov	es,ax

REPT	15
	mov	cx,bx
IFE GRMODE-CGAGR
	rep	movsw
ENDIF
IFE GRMODE-EGAGR
	rep	movsb
ENDIF
	add	si,dx
	add	di,dx
ENDM
	mov	cx,bx
IFE GRMODE-CGAGR
	rep	movsw
ENDIF
IFE GRMODE-EGAGR
	rep	movsb
ENDIF

	dec	cx						; was 0 from last rep movsb, now $ffff for scasb
	jmp	@@findtile

ENDP


;============================================================================


;=================
;
; RFL_MaskForegroundTiles
;
; Scan through update looking for 3's.  If the foreground tile there is a
; masked foreground tile, draw it to the screen
;
;=================

PROC	RFL_MaskForegroundTiles
PUBLIC	RFL_MaskForegroundTiles
USES	SI,DI,BP
	jmp	SHORT @@realstart
@@done:
;
; all tiles have been scanned
;
	ret

@@realstart:
	mov	di,[updateptr]
	mov	bp,(TILESWIDE+1)*TILESHIGH+2
	add	bp,di					; when di = bx, all tiles have been scanned
	push	di
	mov	cx,-1					; definately scan the entire thing
;
; scan for a 3 in the update list
;
@@findtile:
	mov	ax,ss
	mov	es,ax					; scan in the data segment
	mov	al,3
	pop	di						; place to continue scaning from
	repne	scasb
	cmp	di,bp
	je	@@done

;============
;
; found a tile, see if it needs to be masked on
;
;============

	push	di

	sub	di,[updateptr]
	shl	di,1
	mov	si,[updatemapofs-2+di]	; offset from originmap
	add	si,[originmap]

	mov	es,[mapsegs+2]			; foreground map plane segment
	mov	si,[es:si]				; foreground tile number

	or	si,si
	jz	@@findtile				; 0 = no foreground tile

	mov	bx,si
	add	bx,INTILE				;INTILE tile info table
	mov	es,[tinf]
	test	[BYTE PTR es:bx],80h		;high bit = masked tile
	jz	@@findtile

;-------------------

IFE GRMODE-CGAGR
;=================
;
; mask the tile CGA
;
;=================

	mov	di,[blockstarts-2+di]
	add	di,[bufferofs]
	mov	es,[screenseg]
	shl	si,1
	mov	ds,[grsegs+STARTTILE16M*2+si]

	mov	bx,64					;data starts 64 bytes after mask

	xor	si,si

lineoffset	=	0
REPT	16
	mov	ax,[es:di+lineoffset]	;background
	and	ax,[si]					;mask
	or	ax,[si+bx]				;masked data
	mov	[es:di+lineoffset],ax	;background
	inc	si
	inc	si
	mov	ax,[es:di+lineoffset+2]	;background
	and	ax,[si]					;mask
	or	ax,[si+bx]				;masked data
	mov	[es:di+lineoffset+2],ax	;background
	inc	si
	inc	si
lineoffset	=	lineoffset + SCREENWIDTH
ENDM
ENDIF

;-------------------

IFE GRMODE-EGAGR
;=================
;
; mask the tile
;
;=================

	mov	[BYTE planemask],1
	mov	[BYTE planenum],0

	mov	di,[blockstarts-2+di]
	add	di,[bufferofs]
	mov	[cs:screenstartcs],di
	mov	es,[screenseg]
	shl	si,1
	mov	ds,[grsegs+STARTTILE16M*2+si]

	mov	bx,32					;data starts 32 bytes after mask

@@planeloopm:
	mov	dx,SC_INDEX
	mov	al,SC_MAPMASK
	mov	ah,[ss:planemask]
	WORDOUT
	mov	dx,GC_INDEX
	mov	al,GC_READMAP
	mov	ah,[ss:planenum]
	WORDOUT

	xor	si,si
	mov	di,[cs:screenstartcs]
lineoffset	=	0
REPT	16
	mov	cx,[es:di+lineoffset]	;background
	and	cx,[si]					;mask
	or	cx,[si+bx]				;masked data
	inc	si
	inc	si
	mov	[es:di+lineoffset],cx
lineoffset	=	lineoffset + SCREENWIDTH
ENDM
	add	bx,32					;the mask is now further away
	inc	[ss:planenum]
	shl	[ss:planemask],1		;shift plane mask over for next plane
	cmp	[ss:planemask],10000b	;done all four planes?
	je	@@drawn					;drawn all four planes
	jmp	@@planeloopm

@@drawn:
ENDIF

;-------------------

	mov	ax,ss
	mov	ds,ax
	mov	cx,-1					;definately scan the entire thing

	jmp	@@findtile

ENDP


END

*/
