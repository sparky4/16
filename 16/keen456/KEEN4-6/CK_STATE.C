/* Reconstructed Commander Keen 4-6 Source Code
 * Copyright (C) 2021 K1n9_Duk3
 *
 * This file is loosely based on:
 * Keen Dreams Source Code
 * Copyright (C) 2014 Javier M. Chavez
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

#include "CK_DEF.H"

/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

Sint16 wallclip[8][16] = {			// the height of a given point in a tile
{ 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256},
{   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
{   0,0x08,0x10,0x18,0x20,0x28,0x30,0x38,0x40,0x48,0x50,0x58,0x60,0x68,0x70,0x78},
{0x80,0x88,0x90,0x98,0xa0,0xa8,0xb0,0xb8,0xc0,0xc8,0xd0,0xd8,0xe0,0xe8,0xf0,0xf8},
{   0,0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,0x90,0xa0,0xb0,0xc0,0xd0,0xe0,0xf0},
{0x78,0x70,0x68,0x60,0x58,0x50,0x48,0x40,0x38,0x30,0x28,0x20,0x18,0x10,0x08,   0},
{0xf8,0xf0,0xe8,0xe0,0xd8,0xd0,0xc8,0xc0,0xb8,0xb0,0xa8,0xa0,0x98,0x90,0x88,0x80},
{0xf0,0xe0,0xd0,0xc0,0xb0,0xa0,0x90,0x80,0x70,0x60,0x50,0x40,0x30,0x20,0x10,   0}
};

Sint16 xtry, ytry;
boolean playerkludgeclipcancel;

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

Uint16 oldtileleft, oldtiletop, oldtileright, oldtilebottom, oldtilemidx;
Uint16 oldleft, oldtop, oldright, oldbottom, oldmidx;
Sint16 leftmoved, topmoved, rightmoved, bottommoved, midxmoved;

//==========================================================================

/*
====================
=
= MoveObjVert
=
====================
*/

void MoveObjVert(objtype *ob, Sint16 ymove)
{
	ob->y += ymove;
	ob->top += ymove;
	ob->bottom += ymove;
	ob->tiletop = CONVERT_GLOBAL_TO_TILE(ob->top);
	ob->tilebottom = CONVERT_GLOBAL_TO_TILE(ob->bottom);
}

/*
====================
=
= MoveObjHoriz
=
====================
*/

void MoveObjHoriz(objtype *ob, Sint16 xmove)
{
	//BUG? ob->midx is not adjusted in Keen 4 & 5
	ob->x += xmove;
	ob->left += xmove;
	ob->right += xmove;
#ifdef KEEN6
	ob->midx += xmove;	//BUG? ob->tilemidx is not updated
#endif
	ob->tileleft = CONVERT_GLOBAL_TO_TILE(ob->left);
	ob->tileright = CONVERT_GLOBAL_TO_TILE(ob->right);
}

//==========================================================================

/*
====================
=
= PlayerBottomKludge
=
====================
*/

void PlayerBottomKludge(objtype *ob)
{
	Uint16 far *map;
	Uint16 wall, clip, xpix;
	Sint16 xmove, ymove;

	map = (Uint16 far *)mapsegs[1] + mapbwidthtable[ob->tilebottom-1]/2;
	if (ob->xdir == 1)
	{
		xpix = 0;
		map += ob->tileright;
		xmove = ob->right - ob->midx;
		if (tinf[*(map-mapwidth)+WESTWALL] || tinf[*map+WESTWALL])
		{
			return;
		}
	}
	else
	{
		xpix = 15;
		map += ob->tileleft;
		xmove = ob->left - ob->midx;
		if (tinf[*(map-mapwidth)+EASTWALL] || tinf[*map+EASTWALL])
		{
			return;
		}
	}
	if ((_AX = tinf[*map+NORTHWALL]) != 0)	// the _AX = ... part is just to recreate the original code's quirks, feel free to delete this
	{
		return;
	}
	map += mapwidth;
	if ((wall = tinf[*map+NORTHWALL]) != 1)
	{
		return;
	}
	clip = wallclip[wall&7][xpix];
	ymove = CONVERT_TILE_TO_GLOBAL(ob->tilebottom) + clip - 1 -ob->bottom;
	if (ymove <= 0 && ymove >= -bottommoved)
	{
		ob->hitnorth = wall;
		MoveObjVert(ob, ymove);
		MoveObjHoriz(ob, xmove);
	}
}

/*
====================
=
= PlayerTopKludge
=
====================
*/

void PlayerTopKludge(objtype *ob)
{
	Uint16 far *map;
	Uint16 xpix, wall, clip;
	Sint16 move;

	map = (Uint16 far *)mapsegs[1] + mapbwidthtable[ob->tiletop+1]/2;
	if (ob->xdir == 1)
	{
		xpix = 0;
		map += ob->tileright;
		if (tinf[*(map+mapwidth)+WESTWALL] || tinf[*(map+2*mapwidth)+WESTWALL])
		{
			return;
		}
	}
	else
	{
		xpix = 15;
		map += ob->tileleft;
		if (tinf[*(map+mapwidth)+EASTWALL] || tinf[*(map+2*mapwidth)+EASTWALL])
		{
			return;
		}
	}
	if ((_AX = tinf[*map+SOUTHWALL]) != 0)	// the _AX = ... part is just to recreate the original code's quirks, feel free to delete this
	{
		return;
	}
	map -= mapwidth;
	if ((wall = tinf[*map+SOUTHWALL]) != 0)
	{
		clip = wallclip[wall&7][xpix];
		move = CONVERT_TILE_TO_GLOBAL(ob->tiletop+1) - clip - ob->top;
		if (move >= 0 && move <= -topmoved)
		{
			ob->hitsouth = wall;
			MoveObjVert(ob, move);
		}
	}
}

/*
===========================
=
= ClipToEnds
=
===========================
*/

void ClipToEnds(objtype *ob)
{
	Uint16 far *map;
	Uint16 wall, y, clip;
	Sint16 totalmove, maxmove, move;
	Uint16 midxpix;
	
	midxpix = CONVERT_GLOBAL_TO_PIXEL(ob->midx & 0xF0);
	maxmove = -abs(midxmoved)-bottommoved-16;
	map = (Uint16 far *)mapsegs[1] + (mapbwidthtable-1)[oldtilebottom]/2 + ob->tilemidx;
	for (y=oldtilebottom-1; y <= ob->tilebottom; y++,map+=mapwidth)
	{
		if ((wall = tinf[*map + NORTHWALL]) != 0)
		{
			clip = wallclip[wall&7][midxpix];
			move = (CONVERT_TILE_TO_GLOBAL(y) + clip)-1-ob->bottom;
			if (move < 0 && move >= maxmove)
			{
				ob->hitnorth = wall;
				MoveObjVert(ob, move);
				return;
			}
		}
	}
	maxmove = abs(midxmoved)-topmoved+16;
	map = (Uint16 far *)mapsegs[1] + (mapbwidthtable+1)[oldtiletop]/2 + ob->tilemidx;
	for (y=oldtiletop+1; y >= ob->tiletop; y--,map-=mapwidth)	// BUG: unsigned comparison - loop never ends if ob->tiletop is 0
	{
		if ((wall = tinf[*map + SOUTHWALL]) != 0)
		{
			clip = wallclip[wall&7][midxpix];
			move = CONVERT_TILE_TO_GLOBAL(y+1) - clip - ob->top;
			if (move > 0 && move <= maxmove)
			{
				totalmove = ytry+move;
				if (totalmove < 0x100 && totalmove > -0x100)
				{
					ob->hitsouth = wall;
					MoveObjVert(ob, move);
					//BUG? no return here
				}
			}
		}
	}
}

/*
===========================
=
= ClipToSides
=
===========================
*/

void ClipToSides(objtype *ob)
{
	Sint16 move, y, top, bottom;
	Uint16 far *map;
	
	top = ob->tiletop;
	if (ob->hitsouth > 1)
	{
		top++;
	}
	bottom = ob->tilebottom;
	if (ob->hitnorth > 1)
	{
		bottom--;
	}
	for (y=top; y<=bottom; y++)
	{
		map = (Uint16 far *)mapsegs[1] + mapbwidthtable[y]/2 + ob->tileleft;
		if ((ob->hiteast = tinf[*map+EASTWALL]) != 0)
		{
			move = CONVERT_TILE_TO_GLOBAL(ob->tileleft+1) - ob->left;
			MoveObjHoriz(ob, move);
			return;
		}
	}
	for (y=top; y<=bottom; y++)
	{
		map = (Uint16 far *)mapsegs[1] + mapbwidthtable[y]/2 + ob->tileright;
		if ((ob->hitwest = tinf[*map+WESTWALL]) != 0)
		{
			move = (CONVERT_TILE_TO_GLOBAL(ob->tileright)-1)-ob->right;
			MoveObjHoriz(ob, move);
			return;
		}
	}
}

/*
===========================
=
= CheckPosition
=
===========================
*/

boolean CheckPosition(objtype *ob)
{
#ifdef KEEN6Ev15
	// This version is pretty much a compiler-optimized version of the other
	// version below, but I simply could not get the compiler to optimize it
	// in exactly the same way.

	Uint16 tile, x, tileright;
	Uint16 far *map;
	Uint16 rowdiff;
	Uint16 tileleft, y, tilebottom;
	
	map = (Uint16 far *)mapsegs[1] + mapbwidthtable[ob->tiletop]/2 + ob->tileleft;
	rowdiff = mapwidth-(ob->tileright-ob->tileleft+1);

	y = ob->tiletop;
	tileleft = ob->tileleft;
	tileright = _AX = ob->tileright;
	tilebottom = ob->tilebottom;

	for (; tilebottom>=y; y++,map+=rowdiff)
	{
		for (x=tileleft; tileright>=x; x++)
		{
			tile = *(map++);
			if (tinf[tile+NORTHWALL] || tinf[tile+EASTWALL] || tinf[tile+SOUTHWALL] || tinf[tile+WESTWALL])
			{
				return false;
			}
		}
	}
	return true;
#else
	Uint16 tile, x, y;
	Uint16 far *map;
	Uint16 rowdiff;
	
	map = (Uint16 far *)mapsegs[1] + mapbwidthtable[ob->tiletop]/2 + ob->tileleft;
	rowdiff = mapwidth-(ob->tileright-ob->tileleft+1);
	for (y=ob->tiletop; y<=ob->tilebottom; y++,map+=rowdiff)
	{
		for (x=ob->tileleft; x<=ob->tileright; x++)
		{
			tile = *(map++);
			if (tinf[tile+NORTHWALL] || tinf[tile+EASTWALL] || tinf[tile+SOUTHWALL] || tinf[tile+WESTWALL])
			{
				return false;
			}
		}
	}
	return true;
#endif
}

/*
===========================
=
= StatePositionOk
=
===========================
*/

boolean StatePositionOk(objtype *ob, statetype *state)
{
	spritetabletype far *shape;

	if (ob->xdir > 0)
	{
		ob->shapenum = state->rightshapenum;
	}
	else
	{
		ob->shapenum = state->leftshapenum;
	}
	shape = &spritetable[ob->shapenum-STARTSPRITES];
	ob->left = ob->x + shape->xl;
	ob->right = ob->x + shape->xh;
	ob->top = ob->y + shape->yl;
	ob->bottom = ob->y + shape->yh;
	ob->midx = ob->left + (ob->right-ob->left)/2;
	ob->tileleft = CONVERT_GLOBAL_TO_TILE(ob->left);
	ob->tileright = CONVERT_GLOBAL_TO_TILE(ob->right);
	ob->tiletop = CONVERT_GLOBAL_TO_TILE(ob->top);
	ob->tilebottom = CONVERT_GLOBAL_TO_TILE(ob->bottom);
	ob->tilemidx = CONVERT_GLOBAL_TO_TILE(ob->midx);
	return CheckPosition(ob);
}

#ifdef KEEN5
/*
===========================
=
= CalcBounds
=
===========================
*/

void CalcBounds(objtype *ob)	//not present in Keen 4 & 6
{
	spritetabletype far *shape;

	shape = &spritetable[ob->shapenum-STARTSPRITES];
	ob->left = ob->x + shape->xl;
	ob->right = ob->x + shape->xh;
	ob->top = ob->y + shape->yl;
	ob->bottom = ob->y + shape->yh;
	ob->midx = ob->left + (ob->right-ob->left)/2;
}
#endif

//==========================================================================

/*
================
=
= ClipToWalls
=
= Moves the current object xtry/ytry units, clipping to walls
=
================
*/

void ClipToWalls(objtype *ob)
{
	Uint16 oldx, oldy;
#ifdef KEEN6
	Uint16 y;
#endif
	spritetabletype far *shape;
	boolean pushed;

	oldx = ob->x;
	oldy = ob->y;
	pushed = false;

//
// make sure it stays in contact with a 45 degree slope
//
	if (ob->state->pushtofloor)
	{
		if (ob->hitnorth == 25)
		{
			ytry = 145;
		}
		else
		{
			if (xtry > 0)
			{
				ytry = xtry+16;
			}
			else
			{
				ytry = -xtry+16;
			}
			pushed = true;
		}
	}

//
// move the shape
//
	if (xtry > 239)
	{
		xtry = 239;
	}
	else if (xtry < -239)
	{
		xtry = -239;
	}
	if (ytry > 255)			// +16 for push to floor
	{
		ytry = 255;
	}
	else if (ytry < -239)
	{
		ytry = -239;
	}

	ob->x += xtry;
	ob->y += ytry;

	ob->needtoreact = true;

	if (!ob->shapenum)				// can't get a hit rect with no shape!
	{
		return;
	}

	shape = &spritetable[ob->shapenum-STARTSPRITES];

	oldtileright = ob->tileright;
	oldtiletop = ob->tiletop;
	oldtileleft = ob->tileleft;
	oldtilebottom = ob->tilebottom;
	oldtilemidx = ob->tilemidx;

	oldright = ob->right;
	oldtop = ob->top;
	oldleft = ob->left;
	oldbottom = ob->bottom;
	oldmidx = ob->midx;

	ob->left = ob->x + shape->xl;
	ob->right = ob->x + shape->xh;
	ob->top = ob->y + shape->yl;
	ob->bottom = ob->y + shape->yh;
	ob->midx = ob->left + (ob->right-ob->left)/2;

	ob->tileleft = CONVERT_GLOBAL_TO_TILE(ob->left);
	ob->tileright = CONVERT_GLOBAL_TO_TILE(ob->right);
	ob->tiletop = CONVERT_GLOBAL_TO_TILE(ob->top);
	ob->tilebottom = CONVERT_GLOBAL_TO_TILE(ob->bottom);
	ob->tilemidx = CONVERT_GLOBAL_TO_TILE(ob->midx);

	ob->hitnorth=ob->hiteast=ob->hitsouth=ob->hitwest=0;

	if (ob->needtoclip)
	{
		leftmoved = ob->left - oldleft;
		rightmoved = ob->right - oldright;
		topmoved = ob->top - oldtop;
		bottommoved = ob->bottom - oldbottom;
		midxmoved = ob->midx - oldmidx;

	//
	// clip it
	//
		ClipToEnds(ob);

		if (ob == player && !playerkludgeclipcancel)	// zero tolerance near the edge when player gets pushed!
		{
			if (!ob->hitnorth && bottommoved > 0)
			{
				PlayerBottomKludge(ob);
			}
			if (!ob->hitsouth && topmoved < 0)
			{
				PlayerTopKludge(ob);
			}
		}
		ClipToSides(ob);

#ifdef KEEN6
		//
		// special hack to prevent player from getting stuck on slopes?
		//
		if (ob == player && (ob->hitnorth & 7) > 1 && (ob->hiteast || ob->hitwest))
		{
			Uint16 far *map;
			Uint16 pixx, clip, move;
			Uint16 wall;

			pixx = CONVERT_GLOBAL_TO_PIXEL(ob->midx & (15*PIXGLOBAL));
			map = (Uint16 far *)mapsegs[1] + mapbwidthtable[oldtilebottom]/2 + ob->tilemidx;

			for (y=oldtilebottom; ob->tilebottom+1 >= y; y++, map+=mapwidth)
			{
				if ((wall = tinf[*map + NORTHWALL]) != 0)
				{
					clip = wallclip[wall & 7][pixx];
					move = CONVERT_TILE_TO_GLOBAL(y) + clip - 1 - ob->bottom;
					ob->hitnorth = wall;
					MoveObjVert(ob, move);
					return;
				}
			}
		}
#endif
	}
	if (pushed && !ob->hitnorth)
	{
		ob->y = oldy;
		ob->x = oldx + xtry;

		ob->left = ob->x + shape->xl;
		ob->right = ob->x + shape->xh;
		ob->top = ob->y + shape->yl;
		ob->bottom = ob->y + shape->yh;
		ob->midx = ob->left + (ob->right-ob->left)/2;

		ob->tileleft = CONVERT_GLOBAL_TO_TILE(ob->left);
		ob->tileright = CONVERT_GLOBAL_TO_TILE(ob->right);
		ob->tiletop = CONVERT_GLOBAL_TO_TILE(ob->top);
		ob->tilebottom = CONVERT_GLOBAL_TO_TILE(ob->bottom);
		ob->tilemidx = CONVERT_GLOBAL_TO_TILE(ob->midx);
	}

	ob->xmove = ob->xmove + (ob->x - oldx);
	ob->ymove = ob->ymove + (ob->y - oldy);
}

/*
================
=
= FullClipToWalls
=
= Moves the current object xtry/ytry units, clipping to walls
=
================
*/

void FullClipToWalls(objtype *ob)
{
	Uint16 oldx, oldy, w, h;
	spritetabletype far *shape;

	oldx = ob->x;
	oldy = ob->y;

//
// move the shape
//
	if (xtry > 239)
	{
		xtry = 239;
	}
	else if (xtry < -239)
	{
		xtry = -239;
	}
	if (ytry > 239)
	{
		ytry = 239;
	}
	else if (ytry < -239)
	{
		ytry = -239;
	}

	ob->x += xtry;
	ob->y += ytry;

	ob->needtoreact = true;

	shape = &spritetable[ob->shapenum-STARTSPRITES];

	switch (ob->obclass)
	{
#if defined KEEN4
	case keenobj:
		w = 40*PIXGLOBAL;
		h = 24*PIXGLOBAL;
		break;
	case eggbirdobj:
		w = 64*PIXGLOBAL;
		h = 32*PIXGLOBAL;
		break;
	case dopefishobj:
		w = 88*PIXGLOBAL;
		h = 64*PIXGLOBAL;
		break;
	case schoolfishobj:
		w = 16*PIXGLOBAL;
		h = 8*PIXGLOBAL;
		break;
#elif defined KEEN5
	case slicestarobj:
	case spherefulobj:
		w = h = 32*PIXGLOBAL;
		break;
#elif defined KEEN6
	case blorbobj:
		w = h = 32*PIXGLOBAL;
		break;
#endif

	default:
		Quit("FullClipToWalls: Bad obclass");
		break;
	}

	ob->right = ob->x + w;
	ob->left = ob->x;
	ob->top = ob->y;
	ob->bottom = ob->y + h;

	ob->tileleft = CONVERT_GLOBAL_TO_TILE(ob->left);
	ob->tileright = CONVERT_GLOBAL_TO_TILE(ob->right);
	ob->tiletop = CONVERT_GLOBAL_TO_TILE(ob->top);
	ob->tilebottom = CONVERT_GLOBAL_TO_TILE(ob->bottom);

	ob->hitnorth=ob->hiteast=ob->hitsouth=ob->hitwest=0;

//
// clip it
//
	if (!CheckPosition(ob))
	{
		MoveObjHoriz(ob, -xtry);	//undo x movement
		if (CheckPosition(ob))
		{
			if (xtry > 0)
			{
				ob->hitwest = 1;
			}
			else
			{
				ob->hiteast = 1;
			}
		}
		else
		{
			if (ytry > 0)
			{
				ob->hitnorth = 1;
			}
			else
			{
				ob->hitsouth = 1;
			}
			MoveObjHoriz(ob, xtry);	//redo x movement
			MoveObjVert(ob, -ytry);	//undo y movement
			if (!CheckPosition(ob))
			{
				MoveObjHoriz(ob, -xtry);	//undo x movement
				if (xtry > 0)
				{
					ob->hitwest = 1;
				}
				else
				{
					ob->hiteast = 1;
				}
			}
		}
	}

	ob->xmove = ob->xmove + (ob->x - oldx);
	ob->ymove = ob->ymove + (ob->y - oldy);

	ob->left = ob->x + shape->xl;
	ob->right = ob->x + shape->xh;
	ob->top = ob->y + shape->yl;
	ob->bottom = ob->y + shape->yh;
	ob->midx = ob->left + (ob->right-ob->left)/2;
}

/*
================
=
= PushObj
=
= Moves the current object xtry/ytry units, clipping to walls
=
================
*/

void PushObj(objtype *ob)
{
	Uint16 oldx, oldy;
	spritetabletype far *shape;
	
	oldx = ob->x;
	oldy = ob->y;

//
// move the shape
//
	ob->x += xtry;
	ob->y += ytry;

	ob->needtoreact = true;

	if (!ob->shapenum)				// can't get a hit rect with no shape!
	{
		return;
	}

	shape = &spritetable[ob->shapenum-STARTSPRITES];

	oldtileright = ob->tileright;
	oldtiletop = ob->tiletop;
	oldtileleft = ob->tileleft;
	oldtilebottom = ob->tilebottom;
	oldtilemidx = ob->tilemidx;

	oldright = ob->right;
	oldtop = ob->top;
	oldleft = ob->left;
	oldbottom = ob->bottom;
	oldmidx = ob->midx;

	ob->left = ob->x + shape->xl;
	ob->right = ob->x + shape->xh;
	ob->top = ob->y + shape->yl;
	ob->bottom = ob->y + shape->yh;
	ob->midx = ob->left + (ob->right-ob->left)/2;

	ob->tileleft = CONVERT_GLOBAL_TO_TILE(ob->left);
	ob->tileright = CONVERT_GLOBAL_TO_TILE(ob->right);
	ob->tiletop = CONVERT_GLOBAL_TO_TILE(ob->top);
	ob->tilebottom = CONVERT_GLOBAL_TO_TILE(ob->bottom);
	ob->tilemidx = CONVERT_GLOBAL_TO_TILE(ob->midx);

	if (ob->needtoclip)
	{
		leftmoved = ob->left - oldleft;
		rightmoved = ob->right - oldright;
		topmoved = ob->top - oldtop;
		bottommoved = ob->bottom - oldbottom;
		midxmoved = ob->midx - oldmidx;

		ClipToEnds(ob);
		ClipToSides(ob);
	}

	ob->xmove = ob->xmove + (ob->x - oldx);
	ob->ymove = ob->ymove + (ob->y - oldy);
}

//==========================================================================


/*
==================
=
= ClipToSpriteSide
=
= Clips push to solid
=
==================
*/

void ClipToSpriteSide(objtype *push, objtype *solid)
{
	Sint16 xmove, leftinto, rightinto;

	//
	// amount the push shape can be pushed
	//
	xmove = solid->xmove - push->xmove;

	//
	// amount it is inside
	//
	leftinto = solid->right - push->left;
	rightinto = push->right - solid->left;

	if (leftinto > 0 && leftinto <= xmove)
	{
		xtry = leftinto;
		if (push->state->pushtofloor)
		{
			ytry = leftinto+16;
		}
		ClipToWalls(push);
		push->hiteast = 1;
	}
	else if (rightinto > 0 && rightinto <= -xmove)
	{
		xtry = -rightinto;
		if (push->state->pushtofloor)
		{
			ytry = rightinto+16;
		}
		ClipToWalls(push);
		push->hitwest = 1;
	}
}

//==========================================================================


/*
==================
=
= ClipToSpriteTop
=
= Clips push to solid
=
==================
*/

void ClipToSpriteTop(objtype *push, objtype *solid)
{
	Sint16 temp, ymove, bottominto;

	//
	// amount the push shape can be pushed
	//
	ymove = push->ymove - solid->ymove;

	//
	// amount it is inside
	//
	bottominto = push->bottom - solid->top;

	if (bottominto >= 0 && bottominto <= ymove)
	{
		if (push == player)
		{
			gamestate.riding = solid;
		}
		ytry = -bottominto;
		temp = push->state->pushtofloor;
		push->state->pushtofloor = false;
		ClipToWalls(push);
		push->state->pushtofloor = temp;
		if (!push->hitsouth)
		{
			push->hitnorth = 25;
		}
	}
}

//==========================================================================


/*
==================
=
= ClipToSprite
=
= Clips push to solid
=
==================
*/

void ClipToSprite(objtype *push, objtype *solid, boolean squish)
{
	Sint16 xmove, ymove, leftinto, rightinto, topinto, bottominto;
	
	xmove = solid->xmove - push->xmove;
	xtry = ytry = 0;

	//
	// left / right
	//
	leftinto = solid->right - push->left;
	rightinto = push->right - solid->left;

	if (leftinto > 0 && xmove+1 >= leftinto)
	{
		xtry = leftinto;
		push->xspeed = 0;
		PushObj(push);
		if (squish && push->hitwest)
		{
			KillKeen();
		}
		push->hiteast = 1;
		return;
	}
	else if (rightinto > 0 && -xmove+1 >= rightinto)
	{
		xtry = -rightinto;
		push->xspeed = 0;
		PushObj(push);
		if (squish && push->hiteast)
		{
			KillKeen();
		}
		push->hitwest = 1;
		return;
	}

	//
	// top / bottom
	//
	ymove = push->ymove - solid->ymove;
	topinto = solid->bottom - push->top;
	bottominto = push->bottom - solid->top;
	if (bottominto >= 0 && bottominto <= ymove)
	{
		if (push == player)
		{
			gamestate.riding = solid;
		}
		ytry = -bottominto;
		PushObj(push);
		if (squish && push->hitsouth)
		{
			KillKeen();
		}
		if (!push->hitsouth)
		{
			push->hitnorth = 25;
		}
		return;
	}
	else if (topinto >= 0 && topinto <= ymove)	// BUG: should be 'topinto <= -ymove'
	{
		ytry = topinto;
		ClipToWalls(push);
		if (squish && push->hitnorth)
		{
			KillKeen();
		}
		push->hitsouth = 25;
	}
}

//==========================================================================


/*
==================
=
= DoActor
=
= Moves an actor in its current state by a given number of tics.
= If that time takes it into the next state, it changes the state
= and returns the number of excess tics after the state change
=
==================
*/

Sint16 DoActor(objtype *ob, Sint16 numtics)
{
	Sint16 ticcount, usedtics, excesstics;
	statetype *state;
	
	state = ob->state;

	if (state->progress == think)
	{
		if (state->think)
		{
			if (ob->nothink)
			{
				ob->nothink--;
			}
			else
			{
				state->think(ob);
			}
		}
		return 0;
	}

	ticcount = ob->ticcount + numtics;

	if (state->tictime > ticcount || state->tictime == 0)
	{
		ob->ticcount = ticcount;
		if (state->progress == slide || state->progress == slidethink)
		{
			if (ob->xdir)
			{
				xtry += ob->xdir == 1? numtics*state->xmove : -numtics*state->xmove;
			}
			if (ob->ydir)
			{
				ytry += ob->ydir == 1? numtics*state->ymove : -numtics*state->ymove;
			}
		}
		if ((state->progress == slidethink || state->progress == stepthink) && state->think)
		{
			if (ob->nothink)
			{
				ob->nothink--;
			}
			else
			{
				state->think(ob);
			}
		}
		return 0;
	}
	else
	{
		usedtics = state->tictime - ob->ticcount;
		excesstics = ticcount - state->tictime;
		ob->ticcount = 0;
		if (state->progress == slide || state->progress == slidethink)
		{
			if (ob->xdir)
			{
				xtry += ob->xdir == 1? usedtics*state->xmove : -usedtics*state->xmove;
			}
			if (ob->ydir)
			{
				ytry += ob->ydir == 1? usedtics*state->ymove : -usedtics*state->ymove;
			}
		}
		else
		{
			if (ob->xdir)
			{
				xtry += ob->xdir == 1? state->xmove : -state->xmove;
			}
			if (ob->ydir)
			{
				ytry += ob->ydir == 1? state->ymove : -state->ymove;
			}
		}

		if (state->think)
		{
			if (ob->nothink)
			{
				ob->nothink--;
			}
			else
			{
				state->think(ob);
			}
		}

		if (state == ob->state)
		{
			ob->state = state->nextstate;	// go to next state
		}
		else if (!ob->state)
		{
			return 0;			// object removed itself
		}
		return excesstics;
	}
}

//==========================================================================


/*
====================
=
= StateMachine
=
= Change state and give directions
=
====================
*/

void StateMachine(objtype *ob)
{
	Sint16 excesstics, oldshapenum;
	statetype *state;
	
	ob->xmove=ob->ymove=xtry=ytry=0;
	oldshapenum = ob->shapenum;

	state = ob->state;

	excesstics = DoActor(ob, tics);
	if (ob->state != state)
	{
		ob->ticcount = 0;		// start the new state at 0, then use excess
		state = ob->state;
	}

	while (excesstics)
	{
	//
	// passed through to next state
	//
		if (!state->skippable && state->tictime <= excesstics)
		{
			excesstics = DoActor(ob, state->tictime-1);
		}
		else
		{
			excesstics = DoActor(ob, excesstics);
		}
		if (ob->state != state)
		{
			ob->ticcount = 0;		// start the new state at 0, then use excess
			state = ob->state;
		}
	}

	if (!state)			// object removed itself
	{
		RemoveObj(ob);
		return;
	}

	//
	// if state->rightshapenum == NULL, the state does not have a standard
	// shape (the think routine should have set it)
	//
	if (state->rightshapenum)
	{
		if (ob->xdir > 0)
		{
			ob->shapenum = state->rightshapenum;
		}
		else
		{
			ob->shapenum = state->leftshapenum;
		}
	}
	if ((Sint16)ob->shapenum == -1)
	{
		ob->shapenum = 0;		// make it invisable this time
	}

	if (xtry || ytry || ob->shapenum != oldshapenum || ob->hitnorth == 25)
	{
	//
	// actor moved or changed shape
	// make sure the movement is within limits (one tile)
	//
		if (ob->needtoclip == cl_fullclip)
		{
			FullClipToWalls(ob);
		}
		else
		{
			ClipToWalls(ob);
		}
	}
}

//==========================================================================


/*
====================
=
= NewState
=
====================
*/

void NewState(objtype *ob, statetype *state)
{
	Sint16 temp;
	
	ob->state = state;

	if (state->rightshapenum)
	{
		if (ob->xdir > 0)
		{
			ob->shapenum = state->rightshapenum;
		}
		else
		{
			ob->shapenum = state->leftshapenum;
		}
	}

	if ((Sint16)ob->shapenum == -1)
	{
		ob->shapenum = 0;
	}

	temp = ob->needtoclip;
	ob->needtoclip = cl_noclip;

	xtry = ytry = 0;					// no movement
	ClipToWalls(ob);					// just calculate values

	ob->needtoclip = temp;

	if (ob->needtoclip == cl_fullclip)
	{
		FullClipToWalls(ob);
	}
	else if (ob->needtoclip == cl_midclip)
	{
		ClipToWalls(ob);
	}
}

//==========================================================================


/*
====================
=
= ChangeState
=
====================
*/

void ChangeState(objtype *ob, statetype *state)
{
	ob->state = state;
	ob->ticcount = 0;
	if (state->rightshapenum)
	{
		if (ob->xdir > 0)
		{
			ob->shapenum = state->rightshapenum;
		}
		else
		{
			ob->shapenum = state->leftshapenum;
		}
	}

	if ((Sint16)ob->shapenum == -1)
	{
		ob->shapenum = 0;
	}

	ob->needtoreact = true;			// it will need to be redrawn this frame

	xtry = ytry = 0;					// no movement

	if (ob->hitnorth != 25)
	{
		ClipToWalls(ob);
	}
}

//==========================================================================


/*
====================
=
= OnScreen
=
====================
*/

boolean OnScreen(objtype *ob)
{
	if (ob->tileright < originxtile || ob->tilebottom < originytile
		|| ob->tileleft > originxtilemax || ob->tiletop > originytilemax)
	{
		return false;
	}
	return true;
}

//==========================================================================


/*
====================
=
= DoGravity
=
====================
*/

void DoGravity(objtype *ob)
{
	Sint32 i;
//
// only accelerate on odd tics, because of limited precision
//
	for (i = lasttimecount-tics; i<lasttimecount; i++)
	{
		if (i&1)
		{
			if (ob->yspeed < 0 && ob->yspeed >= -4)
			{
				ytry += ob->yspeed;
				ob->yspeed = 0;
				return;
			}
			ob->yspeed += 4;
			if (ob->yspeed > 70)
			{
				ob->yspeed = 70;
			}
		}
		ytry += ob->yspeed;
	}
}


/*
====================
=
= DoWeakGravity
=
====================
*/

void DoWeakGravity(objtype *ob)
{
	Sint32 i;
//
// only accelerate on odd tics, because of limited precision
//
	for (i = lasttimecount-tics; i<lasttimecount; i++)
	{
		if (i&1)
		{
			if (ob->yspeed < 0 && ob->yspeed >= -3)
			{
				ytry += ob->yspeed;
				ob->yspeed = 0;
				return;
			}
			ob->yspeed += 3;
			if (ob->yspeed > 70)
			{
				ob->yspeed = 70;
			}
		}
		ytry += ob->yspeed;
	}
}


/*
====================
=
= DoTinyGravity
=
====================
*/

void DoTinyGravity(objtype *ob)
{
	Sint32 i;
//
// only accelerate every 4 tics, because of limited precision
//
	for (i = lasttimecount-tics; i<lasttimecount; i++)
	{
		if (!i & 3)	//BUG: this is equal to ((!i) & 3), not (!(i & 3))
		{
			ob->yspeed++;
			if (ob->yspeed > 70)
			{
				ob->yspeed = 70;
			}
		}
		ytry += ob->yspeed;
	}
}


/*
===============
=
= AccelerateX
=
===============
*/

void AccelerateX(objtype *ob, Sint16 dir, Sint16 maxspeed)
{
	Sint32 i;
	Uint16 oldsign;
	
	oldsign = ob->xspeed & 0x8000;
//
// only accelerate on odd tics, because of limited precision
//
	for (i=lasttimecount-tics; i<lasttimecount; i++)
	{
		if (i & 1)
		{
			ob->xspeed += dir;
			if ((ob->xspeed & 0x8000) != oldsign)
			{
				oldsign = ob->xspeed & 0x8000;
				ob->xdir = oldsign? -1 : 1;
			}
			if (ob->xspeed > maxspeed)
			{
				ob->xspeed = maxspeed;
			}
			else if (ob->xspeed < -maxspeed)
			{
				ob->xspeed = -maxspeed;
			}
		}
		xtry += ob->xspeed;
	}
}


/*
===============
=
= AccelerateXv
=
= Doesn't change object's xdir
=
===============
*/

void AccelerateXv(objtype *ob, Sint16 dir, Sint16 maxspeed)
{
	Sint32 i;

//
// only accelerate on odd tics, because of limited precision
//
	for (i=lasttimecount-tics; i<lasttimecount; i++)
	{
		if (i & 1)
		{
			ob->xspeed += dir;
			if (ob->xspeed > maxspeed)
			{
				ob->xspeed = maxspeed;
			}
			else if (ob->xspeed < -maxspeed)
			{
				ob->xspeed = -maxspeed;
			}
		}
		xtry += ob->xspeed;
	}
}


/*
===============
=
= AccelerateY
=
===============
*/

void AccelerateY(objtype *ob, Sint16 dir, Sint16 maxspeed)
{
	Sint32 i;

//
// only accelerate on odd tics, because of limited precision
//
	for (i=lasttimecount-tics; i<lasttimecount; i++)
	{
		if (i & 1)
		{
			ob->yspeed += dir;
			if (ob->yspeed > maxspeed)
			{
				ob->yspeed = maxspeed;
			}
			else if (ob->yspeed < -maxspeed)
			{
				ob->yspeed = -maxspeed;
			}
		}
		ytry += ob->yspeed;
	}
}


/*
===============
=
= FrictionX
=
===============
*/

void FrictionX(objtype *ob)
{
	Sint16 friction, oldsign;
	Sint32 i;

	oldsign = ob->xspeed & 0x8000;
	if (ob->xspeed > 0)
	{
		friction = -1;
	}
	else if (ob->xspeed < 0)
	{
		friction = 1;
	}
	else
	{
		friction = 0;
	}
//
// only accelerate on odd tics, because of limited precision
//

	for (i=lasttimecount-tics; i<lasttimecount; i++)
	{
		if (i & 1)
		{
			ob->xspeed += friction;
			if ((ob->xspeed & 0x8000) != oldsign)
			{
				ob->xspeed = 0;
			}
		}
		xtry += ob->xspeed;
	}
}


/*
===============
=
= FrictionY
=
===============
*/

void FrictionY(objtype *ob)
{
	Sint16 friction, oldsign;
	Sint32 i;

	if (ob->yspeed > 0)
	{
		friction = -1;
	}
	else if (ob->yspeed < 0)
	{
		friction = 1;
	}
	else
	{
		friction = 0;
	}
//
// only accelerate on odd tics, because of limited precision
//
	for (i=lasttimecount-tics; i<lasttimecount; i++)
	{
		if (i & 1)
		{
			ob->yspeed += friction;
			if ((ob->yspeed & 0x8000) != oldsign)	//BUG: oldsign is not initialized!
			{
				ob->yspeed = 0;
			}
		}
		ytry += ob->yspeed;
	}
}

//==========================================================================

/*
===============
=
= StunObj
=
===============
*/

void StunObj(objtype *ob, objtype *shot, statetype *stunstate)
{
	ExplodeShot(shot);
	ob->temp1 = ob->temp2 = ob->temp3 = 0;	// Note: ob->nothink should also be set to 0
	ob->temp4 = ob->obclass;
	ChangeState(ob, stunstate);
	ob->obclass = stunnedobj;
#ifndef KEEN4
	ob->yspeed -= 24;
	if (ob->yspeed < -48)
		ob->yspeed = -48;
#endif
}

//==========================================================================

/*
===============
=
= T_Projectile
=
===============
*/

void T_Projectile(objtype *ob)
{
	DoGravity(ob);
	xtry = ob->xspeed*tics;
}


/*
===============
=
= T_WeakProjectile
=
===============
*/

void T_WeakProjectile(objtype *ob)
{
	DoWeakGravity(ob);
	xtry = ob->xspeed*tics;
}


/*
===============
=
= T_Velocity
=
===============
*/

void T_Velocity(objtype *ob)
{
	xtry = ob->xspeed*tics;
	ytry = ob->yspeed*tics;
}


/*
===============
=
= SetReactThink
=
===============
*/

void SetReactThink(objtype *ob)
{
	ob->needtoreact = true;
}


/*
===============
=
= T_Stunned
=
===============
*/

void T_Stunned(objtype *ob)
{
	ob->temp1 = 0;
	ob->needtoreact = true;
	if (++ob->temp2 == 3)
		ob->temp2 = 0;
}


/*
===============
=
= C_Lethal
=
===============
*/

void C_Lethal(objtype *ob, objtype *hit)
{
	ob++;			// shut up compiler
	if (hit->obclass == keenobj)
	{
		KillKeen();
	}
}


/*
===============
=
= R_Draw
=
===============
*/

void R_Draw(objtype *ob)
{
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}


/*
===============
=
= R_Walk
=
===============
*/

void R_Walk(objtype *ob)
{
	if (ob->xdir == 1 && ob->hitwest)
	{
		ob->x -= ob->xmove;
		ob->xdir = -1;
		ob->nothink = US_RndT() >> 5;
		ChangeState(ob, ob->state);
	}
	else if (ob->xdir == -1 && ob->hiteast)
	{
		ob->x -= ob->xmove;
		ob->xdir = 1;
		ob->nothink = US_RndT() >> 5;
		ChangeState(ob, ob->state);
	}
	else if (!ob->hitnorth)
	{
		ob->x -= ob->xmove;
		ob->xdir = -ob->xdir;
		ob->nothink = US_RndT() >> 5;
		ChangeState(ob, ob->state);
	}
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}


/*
===============
=
= R_WalkNormal
=
= Actor will not walk onto tiles with special (e.g. deadly) north walls
=
===============
*/

void R_WalkNormal(objtype *ob)
{
	if (ob->xdir == 1 && ob->hitwest)
	{
		ob->x -= ob->xmove;
		ob->xdir = -1;
		ob->nothink = US_RndT() >> 5;
		ChangeState(ob, ob->state);
	}
	else if (ob->xdir == -1 && ob->hiteast)
	{
		ob->x -= ob->xmove;
		ob->xdir = 1;
		ob->nothink = US_RndT() >> 5;
		ChangeState(ob, ob->state);
	}
	else if (!ob->hitnorth || (ob->hitnorth & ~7))
	{
		ob->x -= ob->xmove*2;
		ob->xdir = -ob->xdir;
		ob->nothink = US_RndT() >> 5;
		ChangeState(ob, ob->state);
	}
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}


/*
===============
=
= BadState
=
===============
*/

void BadState(void)
{
	Quit("Object with bad state!");
}


/*
===============
=
= R_Stunned
=
===============
*/

void R_Stunned(objtype *ob)
{
	Sint16 starx, stary;

	if (ob->hitwest || ob->hiteast)
		ob->xspeed = 0;

	if (ob->hitsouth)
		ob->yspeed = 0;

	if (ob->hitnorth)
	{
		ob->xspeed = ob->yspeed = 0;
		if (ob->state->nextstate)
			ChangeState(ob, ob->state->nextstate);
	}

	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);

	starx = stary = 0;
	switch (ob->temp4)
	{
#if defined KEEN4
	case mimrockobj:
		stary = -4*PIXGLOBAL;
		break;
	case eggobj:
		starx = 8*PIXGLOBAL;
		stary = -8*PIXGLOBAL;
		break;
	case treasureeaterobj:
		starx = 8*PIXGLOBAL;
		break;
	case bounderobj:
		starx = 4*PIXGLOBAL;
		stary = -8*PIXGLOBAL;
		break;
	case wormouthobj:
		starx = 4*PIXGLOBAL;
		stary = -350;	// -21.875 pixels (this one is a bit strange)
		break;
	case lickobj:
		stary = -8*PIXGLOBAL;
		break;
	case inchwormobj:
		starx = -4*PIXGLOBAL;
		stary = -8*PIXGLOBAL;
		break;
	case slugobj:
		stary = -8*PIXGLOBAL;
		break;
#elif defined KEEN5
	case sparkyobj:
		starx += 4*PIXGLOBAL;
		break;
	case amptonobj:
		stary -= 8*PIXGLOBAL;
		asm jmp done;		// just to recreate the original code's quirks, feel free to delete this
		break;
	case scottieobj:
		stary -= 8*PIXGLOBAL;
		break;
#elif defined KEEN6
	case blooguardobj:
		starx = 16*PIXGLOBAL;
		stary = -4*PIXGLOBAL;
		break;
	case flectobj:
		starx = 4*PIXGLOBAL;
		stary = -4*PIXGLOBAL;
		break;
	case bloogobj:
	case nospikeobj:
		starx = 8*PIXGLOBAL;
		stary = -4*PIXGLOBAL;
		break;
	case bloogletobj:
	case babobbaobj:
		stary = -8*PIXGLOBAL;
		break;
	case fleexobj:
		starx = 16*PIXGLOBAL;
		stary = 8*PIXGLOBAL;
		break;
	case ceilickobj:
		stary = 12*PIXGLOBAL;
		break;
	default:
		Quit("No star spec for object!");
#endif
	}
done:

	ob->temp1 = ob->temp1 + tics;
	if (ob->temp1 > 10)
	{
		ob->temp1 -= 10;
		if (++ob->temp2 == 3)
			ob->temp2 = 0;
	}

	RF_PlaceSprite((void **)&ob->temp3, ob->x+starx, ob->y+stary, ob->temp2+STUNSTARS1SPR, spritedraw, 3);
}

//==========================================================================

statetype sc_deadstate = {0, 0, think, false, false, 0, 0, 0, NULL, NULL, NULL, NULL};
#pragma warn -sus	//BadState is not a valid think/contact/react function. Nobody cares.
statetype sc_badstate  = {0, 0, think, false, false, 0, 0, 0, &BadState, &BadState, &BadState, NULL};
#pragma warn +sus