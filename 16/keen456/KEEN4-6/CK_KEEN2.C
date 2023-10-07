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

/*
CK_KEEN2.C
==========

Contains the following actor types (in this order):

- Score Box & Demo sprites
- Keen (world map)
- Flags (world map)
- Neural Stunner Shots
- Gem Door Opener
- Card Door Opener (Keen 5 only)

*/

#include "CK_DEF.H"

Direction opposite[8] = {dir_South, dir_SouthWest, dir_West, dir_NorthWest, dir_North, dir_NorthEast, dir_East, dir_SouthEast};

/*
=============================================================================

						 SCORE BOX ROUTINES

=============================================================================
*/

statetype s_score         = {  0,   0, think, false, false, 0, 0, 0, NULL, NULL, NULL, NULL};
statetype s_demo          = {DEMOPLAQUESPR, DEMOPLAQUESPR, think, false, false, 0, 0, 0, NULL, NULL, NULL, NULL};

/*
======================
=
= SpawnScore
=
======================
*/

void SpawnScore(void)
{
	scoreobj->obclass = inertobj;
	scoreobj->priority = 3;
	scoreobj->active = ac_allways;
	scoreobj->needtoclip = cl_noclip;
	scoreobj->temp2 = -1;
	scoreobj->temp1 = -1;
	scoreobj->temp3 = -1;
	scoreobj->temp4 = -1;
	if (scorescreenkludge)
	{
		scoreobj->state = &sc_deadstate;
	}
	else if (!DemoMode)
	{
		NewState(scoreobj, &s_score);
	}
	else
	{
		NewState(scoreobj, &s_demo);
		CA_MarkGrChunk(DEMOPLAQUESPR);
	}
}


// Taken from Keen Dreams: MemDrawChar and ShiftScore

/*
======================
=
= MemDrawChar
=
======================
*/

#if GRMODE == EGAGR

void MemDrawChar(Sint16 char8, Uint8 far *dest, Uint16 width, Uint16 planesize)
{
	Uint16 source = (Uint16)grsegs[STARTTILE8];	// Note: this differs from Keen Dreams source

asm	mov	si,[char8]
asm	shl	si,1
asm	shl	si,1
asm	shl	si,1
asm	shl	si,1
asm	shl	si,1		// index into char 8 segment

asm	mov	ds,[WORD PTR source]	// Note: this differs from Keen Dreams source
asm	mov	es,[WORD PTR dest+2]

asm	mov	cx,4		// draw four planes
asm	mov	bx,[width]
asm	dec	bx

planeloop:

asm	mov	di,[WORD PTR dest]

asm	movsb
asm	add	di,bx
asm	movsb
asm	add	di,bx
asm	movsb
asm	add	di,bx
asm	movsb
asm	add	di,bx
asm	movsb
asm	add	di,bx
asm	movsb
asm	add	di,bx
asm	movsb
asm	add	di,bx
asm	movsb

asm	mov	ax,[planesize]
asm	add	[WORD PTR dest],ax

asm	loop	planeloop

asm	mov	ax,ss
asm	mov	ds,ax
}

#elif GRMODE == CGAGR

void MemDrawChar (int char8,byte far *dest,unsigned width,unsigned planesize)
{
asm	mov	si,[char8]
asm	shl	si,1
asm	shl	si,1
asm	shl	si,1
asm	shl	si,1		// index into char 8 segment

asm	mov	ds,[WORD PTR grsegs+STARTTILE8*2]
asm	mov	es,[WORD PTR dest+2]

asm	mov	bx,[width]
asm	sub	bx,2

asm	mov	di,[WORD PTR dest]

asm	movsw
asm	add	di,bx
asm	movsw
asm	add	di,bx
asm	movsw
asm	add	di,bx
asm	movsw
asm	add	di,bx
asm	movsw
asm	add	di,bx
asm	movsw
asm	add	di,bx
asm	movsw
asm	add	di,bx
asm	movsw

asm	mov	ax,ss
asm	mov	ds,ax

	planesize++;		// shut the compiler up
}
#endif

/*
====================
=
= ShiftScore
=
====================
*/
#if GRMODE == EGAGR
void ShiftScore (void)
{
	spritetabletype far *spr;
	spritetype _seg *dest;

	spr = &spritetable[SCOREBOXSPR-STARTSPRITES];
	dest = (spritetype _seg *)grsegs[SCOREBOXSPR];

	CAL_ShiftSprite (FP_SEG(dest),dest->sourceoffset[0],
		dest->sourceoffset[1],spr->width,spr->height,2);

	CAL_ShiftSprite (FP_SEG(dest),dest->sourceoffset[0],
		dest->sourceoffset[2],spr->width,spr->height,4);

	CAL_ShiftSprite (FP_SEG(dest),dest->sourceoffset[0],
		dest->sourceoffset[3],spr->width,spr->height,6);
}
#endif

/*
===============
=
= UpdateScore
=
===============
*/

void UpdateScore(objtype *ob)
{
	char		str[10],*ch;
	spritetype	_seg	*block;
	Uint8		far *dest;
	Uint16	i, length, width, planesize, number;
	boolean changed;

	if (scorescreenkludge)
		return;

	if (DemoMode)
	{
		DrawDemoPlaque(ob);
		return;
	}

	if (!showscorebox)
		return;

	changed = false;

//code below is a combination of ScoreThink and ScoreReact from Keen Dreams with minor changes

//
// score changed
//
	if ((gamestate.score>>16) != ob->temp1
		|| (Uint16)gamestate.score != ob->temp2 )
	{
		block = (spritetype _seg *)grsegs[SCOREBOXSPR];
		width = block->width[0];
		planesize = block->planesize[0];
		dest = (Uint8 far *)grsegs[SCOREBOXSPR]+block->sourceoffset[0]
			+ planesize + width*4;

		ltoa (gamestate.score,str,10);

		// erase leading spaces
		length = strlen(str);
		for (i=9;i>length;i--)
			MemDrawChar (41,dest+=CHARWIDTH,width,planesize);

		// draw digits
		ch = str;
		while (*ch)
			MemDrawChar (*ch++ - '0'+42,dest+=CHARWIDTH,width,planesize);

#if GRMODE == EGAGR
		ShiftScore ();
#endif
		ob->needtoreact = true;
		ob->temp1 = gamestate.score>>16;
		ob->temp2 = gamestate.score;

		changed = true;
	}

//
// ammo changed
//
	number = gamestate.ammo;
	if (number != ob->temp3)
	{
		block = (spritetype _seg *)grsegs[SCOREBOXSPR];
		width = block->width[0];
		planesize = block->planesize[0];
		dest = (byte far *)grsegs[SCOREBOXSPR]+block->sourceoffset[0]
			+ planesize + width*20 + 7*CHARWIDTH;

		if (number > 99)
			strcpy (str,"99");
		else
			ltoa (number,str,10);

		// erase leading spaces
		length = strlen(str);
		for (i=2;i>length;i--)
			MemDrawChar (41,dest+=CHARWIDTH,width,planesize);

		// draw digits
		ch = str;
		while (*ch)
			MemDrawChar (*ch++ - '0'+42,dest+=CHARWIDTH,width,planesize);

#if GRMODE == EGAGR
		ShiftScore ();
#endif
		ob->needtoreact = true;
		ob->temp3 = number;

		changed = true;
	}

//
// lives changed
//
	if (gamestate.lives != ob->temp4)
	{
		block = (spritetype _seg *)grsegs[SCOREBOXSPR];
		width = block->width[0];
		planesize = block->planesize[0];
		dest = (byte far *)grsegs[SCOREBOXSPR]+block->sourceoffset[0]
			+ planesize + width*20 + 2*CHARWIDTH;

		if (gamestate.lives > 99)
			strcpy (str,"99");
		else
			ltoa (gamestate.lives,str,10);

		// erase leading spaces
		length = strlen(str);
		for (i=2;i>length;i--)
			MemDrawChar (41,dest+=CHARWIDTH,width,planesize);

		// draw digits
		ch = str;
		while (*ch)
			MemDrawChar (*ch++ - '0'+42,dest+=CHARWIDTH,width,planesize);

#if GRMODE == EGAGR
		ShiftScore ();
#endif
		ob->needtoreact = true;
		ob->temp4 = gamestate.lives;

		changed = true;
	}

/*
Note:
-----

It would be more efficient to use

	if (changed)
		ShiftScore();

here instead of the individual ShiftScore() calls above. Because if the player
gains a life by collecting points items, both the score and lives numbers need
to be updated, which means the sprite would be shifted twice. And if the player
fires a shot during the same frame, the ammo number also needs to be updated,
leading to up to three shifts in one frame.
*/

	if (ob->x != originxglobal || ob->y != originyglobal)
	{
		ob->x = originxglobal;
		ob->y = originyglobal;
		changed = true;
	}

	if (changed)
#if GRMODE == EGAGR
		RF_PlaceSprite(&ob->sprite, ob->x+4*PIXGLOBAL, ob->y+4*PIXGLOBAL, SCOREBOXSPR, spritedraw, 3);
#elif GRMODE == CGAGR
		RF_PlaceSprite(&ob->sprite, ob->x+8*PIXGLOBAL, ob->y+8*PIXGLOBAL, SCOREBOXSPR, spritedraw, 3);
#endif
}

/*
===============
=
= DrawDemoPlaque
=
===============
*/

void DrawDemoPlaque(objtype *ob)
{
	if (ob->x != originxglobal || ob->y != originyglobal)
	{
		ob->x = originxglobal;
		ob->y = originyglobal;
		RF_PlaceSprite(&ob->sprite, ob->x + 160*PIXGLOBAL - 32*PIXGLOBAL, ob->y + 8*PIXGLOBAL, DEMOPLAQUESPR, spritedraw, 3);
	}
}


/*
=============================================================================

							   MINI KEEN

player->temp1 = dir
player->temp2 = animation stage

=============================================================================
*/

#ifdef KEEN4
statetype s_keenonfoot1 = {WOLRDKEENRIDE1SPR, WOLRDKEENRIDE1SPR, stepthink, false, false, 30, 0, 0, T_FootFly, NULL, R_Draw, &s_keenonfoot2};
statetype s_keenonfoot2 = {WOLRDKEENRIDE2SPR, WOLRDKEENRIDE2SPR, stepthink, false, false, 30, 0, 0, T_FootFly, NULL, R_Draw, &s_keenonfoot1};
statetype s_worldswim = {0, 0, slide, true, false, 6, 16, 16, T_KeenWorldSwim, NULL, R_Draw, &s_worldswim};
#endif

#ifdef KEEN5
statetype s_worldelevate = {-1, -1, think, true, false, 6, 16, 16, T_Elevate, NULL, R_Draw, NULL};
#endif

statetype s_worldkeen     = {0, 0, stepthink, false, false, 360, 0, 0, T_KeenWorld, NULL, R_Draw, &s_worldkeenwave1};

statetype s_worldkeenwave1 = {WORLDKEENWAVE1SPR, WORLDKEENWAVE1SPR, stepthink, false, false, 20, 0, 0, T_KeenWorld, NULL, R_Draw, &s_worldkeenwave2};
statetype s_worldkeenwave2 = {WORLDKEENWAVE2SPR, WORLDKEENWAVE2SPR, stepthink, false, false, 20, 0, 0, T_KeenWorld, NULL, R_Draw, &s_worldkeenwave3};
statetype s_worldkeenwave3 = {WORLDKEENWAVE1SPR, WORLDKEENWAVE1SPR, stepthink, false, false, 20, 0, 0, T_KeenWorld, NULL, R_Draw, &s_worldkeenwave4};
statetype s_worldkeenwave4 = {WORLDKEENWAVE2SPR, WORLDKEENWAVE2SPR, stepthink, false, false, 20, 0, 0, T_KeenWorld, NULL, R_Draw, &s_worldkeenwave5};
statetype s_worldkeenwave5 = {WORLDKEENWAVE1SPR, WORLDKEENWAVE1SPR, stepthink, false, false, 20, 0, 0, T_KeenWorldWalk, NULL, R_Draw, &s_worldkeen};

statetype s_worldkeenwalk = {0, 0, slide, true, false, 4, 24, 24, T_KeenWorldWalk, NULL, R_Draw, &s_worldkeenwalk};

Sint16 worldshapes[8] = {WORLDKEENU1SPR-1, WORLDKEENUR1SPR-1, WORLDKEENR1SPR-1, WORLDKEENDR1SPR-1, WORLDKEEND1SPR-1, WORLDKEENDL1SPR-1, WORLDKEENL1SPR-1, WORLDKEENUL1SPR-1};	//-1 to everything because worldanims values are 1-based
Sint16 worldanims[4] = {2, 3, 1, 3};
#ifdef KEEN4
Sint16 swimshapes[8] = {WORLDKEENSWIMU1SPR, WORLDKEENSWIMUR1SPR, WORLDKEENSWIMR1SPR, WORLDKEENSWIMDR1SPR, WORLDKEENSWIMD1SPR, WORLDKEENSWIMDL1SPR, WORLDKEENSWIML1SPR, WORLDKEENSWIMUL1SPR};
#endif
#ifndef KEEN6
Sint16 tiledir[4] = {dir_South, dir_West, dir_North, dir_East};
#endif

/*
======================
=
= SpawnWorldKeen
=
======================
*/

void SpawnWorldKeen(Sint16 x, Sint16 y)
{
#ifdef KEEN4
	if (playstate == ex_foot)
	{
		player->needtoclip = cl_noclip;
		player->obclass = keenobj;
		player->x = gamestate.worldx;
		player->y = gamestate.worldy;
		player->active = ac_allways;
		player->priority = 3;
		player->xdir = 0;
		player->ydir = 0;
		if (gamestate.worldx < 20*TILEGLOBAL)
		{
			player->temp1 = 280;
			player->xspeed = (30*TILEGLOBAL - player->x)/280 + 1;
			player->yspeed = (55*TILEGLOBAL - player->y)/280 + 1;
		}
		else
		{
			player->temp1 = 140;
			player->xspeed = (Sint16)(16*TILEGLOBAL - player->x)/140 + 1;
			player->yspeed = (Sint16)(47*TILEGLOBAL - player->y)/140 + 1;
		}
		NewState(player, &s_keenonfoot1);
		return;
	}
#endif

	player->obclass = keenobj;
	if (gamestate.worldx == 0)
	{
		player->x = CONVERT_TILE_TO_GLOBAL(x);
		player->y = CONVERT_TILE_TO_GLOBAL(y);
	}
	else
	{
		player->x = gamestate.worldx;
		player->y = gamestate.worldy;
	}
	player->active = ac_allways;
	player->priority = 1;
	player->xdir = 0;
	player->ydir = 0;
	player->temp1 = dir_West;
	player->temp2 = 3;
	player->temp3 = 0;
	player->shapenum = WORLDKEENL3SPR;
	NewState(player, &s_worldkeen);
}

#ifdef KEEN5
/*
======================
=
= SpawnWorldKeenPort
=
======================
*/

void SpawnWorldKeenPort(Uint16 tileX, Uint16 tileY)
{
	player->obclass = keenobj;
	player->x = CONVERT_TILE_TO_GLOBAL(tileX);
	player->y = CONVERT_TILE_TO_GLOBAL(tileY);
	player->active = ac_allways;
	player->priority = 1;
	player->xdir = 0;
	player->ydir = 0;
	player->temp1 = dir_West;
	player->temp2 = 3;
	player->temp3 = 0;
	player->shapenum = WORLDKEENL3SPR;
	NewState(player, &s_worldkeen);
}
#endif


/*
======================
=
= CheckEnterLevel
=
======================
*/

void CheckEnterLevel(objtype *ob)
{
	Uint16 x, y, info;

	for (y = ob->tiletop; y <= ob->tilebottom; y++)
	{
		for (x = ob->tileleft; x <= ob->tileright; x++)
		{
			info = *(mapsegs[2]+mapbwidthtable[y]/2 + x);
			if (info > 0xC000 && info <= (0xC000 + 18))
			{
				gamestate.worldx = ob->x;
				gamestate.worldy = ob->y;
				gamestate.mapon = info - 0xC000;
				playstate = ex_completed;
				SD_PlaySound(SND_ENTERLEVEL);
			}
		}
	}
}

/*
======================
=
= T_KeenWorld
=
======================
*/

void T_KeenWorld(objtype *ob)
{
	if (c.dir != dir_None)
	{
		ob->state = &s_worldkeenwalk;
		ob->temp2 = 0;
		T_KeenWorldWalk(ob);
	}
	if (jumpbutton || pogobutton || firebutton)
	{
		CheckEnterLevel(ob);
	}
}

/*
======================
=
= T_KeenWorldWalk
=
======================
*/

void T_KeenWorldWalk(objtype *ob)
{
	if (ob->temp3)
	{
		ob->temp3 -= 4;
		if (ob->temp3 < 0)
			ob->temp3 = 0;
	}
	else
	{
		ob->xdir = c.xaxis;
		ob->ydir = c.yaxis;
		if (pogobutton || firebutton || jumpbutton)
		{
			CheckEnterLevel(ob);
		}
		if (c.dir == dir_None)
		{
			ob->state = &s_worldkeen;
			ob->shapenum = worldshapes[ob->temp1] + 3;
			return;
		}
		ob->temp1 = c.dir;
	}
	if (++ob->temp2 == 4)
		ob->temp2 = 0;
	ob->shapenum = worldshapes[ob->temp1] + worldanims[ob->temp2];

	if (ob->temp2 == 1)
	{
		SD_PlaySound(SND_WORLDWALK1);
	}
	else if (ob->temp2 == 3)
	{
		SD_PlaySound(SND_WORLDWALK2);
	}
}

#ifdef KEEN4
/*
======================
=
= T_FootFly
=
======================
*/

void T_FootFly(objtype *ob)
{
	ob->temp1 = ob->temp1 - tics;
	xtry = ob->xspeed * tics;
	ytry = ob->yspeed * tics;
	if (ob->temp1 <= 0)
	{
		xtry -= ob->xspeed * -ob->temp1;
		ytry -= ob->yspeed * -ob->temp1;
		ob->priority = 1;
		ob->temp1 = dir_West;
		ob->temp2 = 3;
		ob->temp3 = 0;
		player->xdir = 0;
		player->ydir = 0;
		ob->state = &s_worldkeen;
		ob->shapenum = WORLDKEENL3SPR;
		ob->needtoclip = cl_midclip;
	}
}

/*
======================
=
= T_KeenWorldSwim
=
======================
*/

void T_KeenWorldSwim(objtype *ob)
{
	if (ob->temp3)
	{
		ob->temp3 -= 6;
		if (ob->temp3 < 0)
			ob->temp3 = 0;
	}
	else
	{
		ob->xdir = c.xaxis;
		ob->ydir = c.yaxis;
		if (c.xaxis || c.yaxis)
			ob->temp1 = c.dir;
	}
	ob->shapenum = swimshapes[ob->temp1] + ob->temp2;
	if (++ob->temp2 == 2)
		ob->temp2 = 0;

	if (ob->temp2 == 0)
	{
		SD_PlaySound(SND_SWIM1);
	}
	else
	{
		SD_PlaySound(SND_SWIM2);
	}
}

#else	// NOT Keen 4 (i.e. Keen 5 & 6):

/*
======================
=
= Teleport
=
======================
*/

void Teleport(Uint16 tileX, Uint16 tileY)
{
	Uint16 tile, globalx, globaly, duration, move;
	objtype *o;
	objtype *ob = player;

	//
	// enter the teleporter
	//
	SD_PlaySound(SND_TELEPORT);
	globalx = CONVERT_TILE_TO_GLOBAL(tileX);
	globaly = CONVERT_TILE_TO_GLOBAL(tileY);

#ifdef KEEN6Ev15
	// We need to make the compiler "forget" that duration starts at 0
	// to make sure the while-loop check is performed when entering the
	// loop. Can't change compiler settings since we do need that loop
	// optimization for the for-loop at the end of this routine.
	if (true)
		duration = 0;
#else
	duration = 0;
#endif

	while (duration < 130)
	{
		RF_Refresh();
		move = tics*2;
		duration += tics;

		if (ob->x == globalx && ob->y == globaly)
			break;

		if (ob->y < globaly)
		{
			ob->y += move;
			if (ob->y > globaly)
				ob->y = globaly;
		}
		else if (ob->y > globaly)
		{
			ob->y -= move;
			if (ob->y < globaly)
				ob->y = globaly;
		}

		if (ob->x < globalx)
		{
			ob->x += move;
			if (ob->x > globalx)
				ob->x = globalx;
		}
		else if (ob->x > globalx)
		{
			ob->x -= move;
			if (ob->x < globalx)
				ob->x = globalx;
		}

		ob->shapenum = ((TimeCount >> 3) % 3) + WORLDKEENU1SPR;
		RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);

		tile = ((TimeCount >> 2) & TELEPORERTILEMASK) + TELEPORTERTILE1;
		RF_MemToMap(&tile, 1, tileX, tileY, 1, 1);
	}

	tile = TELEPORTERTILE2;
	RF_MemToMap(&tile, 1, tileX, tileY, 1, 1);

	//
	// teleport to new location
	//
	tile = *(mapsegs[2]+mapbwidthtable[tileY]/2 + tileX);
	tileX = tile >> 8;
	tileY = tile & 0x7F;	// BUG? y coordinate is limited to 1..127
	ob->x = CONVERT_TILE_TO_GLOBAL(tileX);
	ob->y = CONVERT_TILE_TO_GLOBAL(tileY);
	ob->xdir = 0;
	ob->ydir = 1;
	ob->temp1 = dir_South;
	NewState(ob, ob->state);
	CenterActor(ob);

	//
	// draw flags/signs for new location
	//
	for (o=player->next; o; o=o->next)
	{
		if (!o->active && o->obclass == flagobj
			&& o->tileright >= originxtile-1 && o->tileleft <= originxtilemax+1
			&& o->tiletop <= originytilemax+1 && o->tilebottom >= originytile-1)
		{
			o->needtoreact = true;
			o->active = ac_yes;
			RF_PlaceSprite(&o->sprite, o->x, o->y, o->shapenum, spritedraw, o->priority);
		}
	}
	UpdateScore(scoreobj);
	RF_Refresh();
	RF_Refresh();

	//
	// leave teleporter
	//
	SD_PlaySound(SND_TELEPORT);

	for (duration = 0; duration < 90; )
	{
		RF_Refresh();
		duration += tics;
		ob->y += tics*2 + tics;

		ob->shapenum = ((TimeCount >> 3) % 3) + WORLDKEEND1SPR;
		RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);

		tile = ((TimeCount >> 2) & TELEPORERTILEMASK) + TELEPORTERTILE3;
		RF_MemToMap(&tile, 1, tileX, tileY, 1, 1);
	}

	tile = TELEPORTERTILE4;
	RF_MemToMap(&tile, 1, tileX, tileY, 1, 1);
	xtry = ytry = 0;
	ClipToWalls(ob);
}

#ifdef KEEN5

/*
======================
=
= T_Elevate
=
======================
*/

void T_Elevate(objtype *ob)
{
	Sint16 i, x, y, tx, ty;
	Uint16 tiles[2][2];

	ytry = ob->ydir * 64 * tics;
	if (ob->x != ob->temp2)
	{
		xtry = ob->xdir * 12 * tics;
		if ( (ob->xdir == 1 && ob->x + xtry > ob->temp2)
			|| (ob->xdir == -1 && ob->x + xtry < ob->temp2) )
		{
			xtry = ob->temp2 - ob->x;
		}
	}

	//
	// Keen has no sprite in this state, so we need to update the hitbox manually
	// to avoid issues (the screen scrolling routines use left/right/top/bottom)
	//
	ob->left = ob->x + xtry;
	ob->right = ob->left + (TILEGLOBAL-1);
	ob->top = ob->y + ytry;
	ob->bottom = ob->top + (TILEGLOBAL-1);

	if (ob->ydir == 1)
	{
		if (ob->y + ytry < ob->temp1)
			return;
	}
	else
	{
		if (ob->y + ytry > ob->temp1)
			return;
	}

	//
	// the invisible Keen has arrived at its destination
	//
	ytry = 0;
	xtry = 0;
	ob->x = ob->temp2;
	ob->y = ob->temp1;
	ob->priority = 1;
	ob->temp1 = 4;
	ob->temp2 = 3;
	ob->temp3 = 0;
	player->xdir = 0;
	player->ydir = 0;
	ob->state = &s_worldkeen;
	ob->shapenum = WORLDKEEND3SPR;
	ob->needtoclip = cl_midclip;
	tx = CONVERT_GLOBAL_TO_TILE(ob->x);
	ty = CONVERT_GLOBAL_TO_TILE(ob->y);
	WorldScrollScreen(ob);
	UpdateScore(scoreobj);
	RF_Refresh();
	RF_Refresh();

	ob->y -= TILEGLOBAL;
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);

	//
	// open the elevator door
	//
	SD_PlaySound(SND_ELEVATORDOOR);
	for (i=0; i<=5; i++)
	{
		for (y=0; y<2; y++)
		{
			for (x=0; x<2; x++)
			{
				tiles[y][x] = *(mapsegs[1]+mapbwidthtable[y]/2 + i*2 + x);
			}
		}
		RF_MemToMap(&tiles[0][0], 1, tx, ty-2, 2, 2);
		RF_Refresh();
		VW_WaitVBL(8);
	}

	//
	// make Keen walk out of the elevator
	//
	for (y=0; y<32; y++)
	{
		ob->y += 8;	// move half a pixel every frame for 32 frames -> move down 16 pixels total
		ob->shapenum = (y / 4) % 3 + WORLDKEEND1SPR;
		RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
		RF_Refresh();
	}
	ob->needtoclip = cl_midclip;	// redundant, but doesn't do any harm
}

/*
======================
=
= Elevator
=
======================
*/

void Elevator(Uint16 tileX, Uint16 tileY, Sint16 dir)
{
	Uint16 info, globalx, globaly, duration, move;
	Sint16 x, y, i;
	Uint16 tiles[2][2];
	objtype *ob = player;

	globalx = CONVERT_TILE_TO_GLOBAL(tileX);
	globaly = CONVERT_TILE_TO_GLOBAL(tileY);

	//
	// make Keen walk into the elevator
	//
	for (duration = 0; duration < 130; )
	{
		CalcBounds(ob);
		WorldScrollScreen(ob);
		UpdateScore(scoreobj);
		RF_Refresh();

		move = tics * 2;
		duration += tics;

		if (ob->x == globalx && ob->y == globaly)
			break;

		if (ob->y < globaly)
		{
			ob->y += move;
			if (ob->y > globaly)
				ob->y = globaly;
		}
		else if (ob->y > globaly)
		{
			ob->y -= move;
			if (ob->y < globaly)
				ob->y = globaly;
		}

		if (ob->x < globalx)
		{
			ob->x += move;
			if (ob->x > globalx)
				ob->x = globalx;
		}
		else if (ob->x > globalx)
		{
			ob->x -= move;
			if (ob->x < globalx)
				ob->x = globalx;
		}

		ob->shapenum = ((duration / 8) % 3) + WORLDKEENU1SPR;
		RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
	}

	//
	// close the elevator door
	//
	SD_PlaySound(SND_ELEVATORDOOR);
	for (i=5; i >= 0; i--)
	{
		for (y=0; y<2; y++)
		{
			for (x=0; x<2; x++)
			{
				tiles[y][x] = *(mapsegs[1]+mapbwidthtable[y]/2 + i*2 + x);
			}
		}
		RF_MemToMap(&tiles[0][0], 1, tileX+dir, tileY-1, 2, 2);
		RF_Refresh();
		VW_WaitVBL(8);
	}

	//
	// make Keen invisible (and not clipping) and send him to the destination
	//
	RF_RemoveSprite(&ob->sprite);
	info = *(mapsegs[2] + mapbwidthtable[tileY]/2 + tileX);
	ob->temp2 = CONVERT_TILE_TO_GLOBAL(info >> 8);
	ob->temp1 = CONVERT_TILE_TO_GLOBAL((info & 0x7F) + 1);	// BUG? y coordinate is limited to 1..127
	if (ob->temp1 < ob->y)
	{
		ob->ydir = -1;
	}
	else
	{
		ob->ydir = 1;
	}
	if (ob->temp2 < ob->x)
	{
		ob->xdir = -1;
	}
	else
	{
		ob->xdir = 1;
	}
	ob->needtoclip = cl_noclip;
	ob->state = &s_worldelevate;
}

#endif	//ifdef KEEN5

#endif	//ifdef KEEN4 ... else ...

/*
======================
=
= CheckWorldInTiles
=
======================
*/

void CheckWorldInTiles(objtype *ob)
{
	Uint16 tx, ty, intile;

	if (ob->temp3)
		return;

	tx = ob->tilemidx;
	ty = CONVERT_GLOBAL_TO_TILE(ob->top + (ob->bottom-ob->top)/2);
	intile = tinf[INTILE + *(mapsegs[1]+mapbwidthtable[ty]/2+tx)];
#if defined KEEN4
	if (intile == INTILE_SHORESOUTH || intile == INTILE_SHORENORTH
		|| intile == INTILE_SHOREEAST || intile == INTILE_SHOREWEST)
	{
		if (!gamestate.wetsuit)
		{
			SD_PlaySound(SND_NOWAY);
			CantSwim();
			RF_ForceRefresh();
			xtry = -ob->xmove;
			ytry = -ob->ymove;
			ob->xdir = ob->ydir = 0;
			ClipToWalls(ob);
		}
		else
		{
			ob->temp1 = tiledir[intile-INTILE_SHORESOUTH];
			if (ob->state == &s_worldswim)
			{
				ob->temp1 = opposite[ob->temp1];
			}
			switch (ob->temp1)
			{
			case dir_North:
				ob->xdir = 0;
				ob->ydir = -1;
				break;
			case dir_East:
				ob->xdir = 1;
				ob->ydir = 0;
				break;
			case dir_South:
				ob->xdir = 0;
				ob->ydir = 1;
				break;
			case dir_West:
				ob->xdir = -1;
				ob->ydir = 0;
				break;
			}
			ob->temp2 = 0;
			ob->temp3 = 18;
			if (ob->state == &s_worldswim)
			{
				ChangeState(ob, &s_worldkeenwalk);
			}
			else
			{
				ChangeState(ob, &s_worldswim);
			}
		}
	}
#elif defined KEEN5
	switch (intile)
	{
	case INTILE_TELEPORT:
		Teleport(tx, ty);
		break;
	case INTILE_ELEVATORLEFT:
		Elevator(tx, ty, 0);
		break;
	case INTILE_ELEVATORRIGHT:
		Elevator(tx, ty, -1);
		break;
	}
#elif defined KEEN6
	switch (intile)
	{
	case INTILE_TELEPORT:
		Teleport(tx, ty);
		break;
	}
#endif
}

/*
=============================================================================

							   FLAGS

temp1 = x destination for the thrown flag
temp2 = y destination for the thrown flag
temp3 = amount of time passed since flag was thrown (in tics)

=============================================================================
*/

statetype s_flagwave1     = {FLAGFLAP1SPR, FLAGFLAP1SPR, step, false, false, 10, 0, 0, NULL, NULL, R_Draw, &s_flagwave2};
statetype s_flagwave2     = {FLAGFLAP2SPR, FLAGFLAP2SPR, step, false, false, 10, 0, 0, NULL, NULL, R_Draw, &s_flagwave3};
statetype s_flagwave3     = {FLAGFLAP3SPR, FLAGFLAP3SPR, step, false, false, 10, 0, 0, NULL, NULL, R_Draw, &s_flagwave4};
statetype s_flagwave4     = {FLAGFLAP4SPR, FLAGFLAP4SPR, step, false, false, 10, 0, 0, NULL, NULL, R_Draw, &s_flagwave1};

#ifndef KEEN5
statetype s_throwflag0    = {FLAGFLIP1SPR, FLAGFLIP1SPR, think, false, false, 6, 0, 0, TossThink, NULL, R_Draw, &s_throwflag1};
statetype s_throwflag1    = {FLAGFLIP1SPR, FLAGFLIP1SPR, stepthink, false, false, 12, 0, 0, PathThink, NULL, R_Draw, &s_throwflag2};
statetype s_throwflag2    = {FLAGFLIP2SPR, FLAGFLIP2SPR, stepthink, false, false, 12, 0, 0, PathThink, NULL, R_Draw, &s_throwflag3};
statetype s_throwflag3    = {FLAGFLIP3SPR, FLAGFLIP3SPR, stepthink, false, false, 12, 0, 0, PathThink, NULL, R_Draw, &s_throwflag4};
statetype s_throwflag4    = {FLAGFLIP4SPR, FLAGFLIP4SPR, stepthink, false, false, 12, 0, 0, PathThink, NULL, R_Draw, &s_throwflag5};
statetype s_throwflag5    = {FLAGFALL1SPR, FLAGFALL1SPR, stepthink, false, false, 12, 0, 0, PathThink, NULL, R_Draw, &s_throwflag6};
statetype s_throwflag6    = {FLAGFALL1SPR, FLAGFALL1SPR, stepthink, true, false, 1, 0, 0, FlagAlign, NULL, R_Draw, &s_flagwave1};
#endif

Sint16 flagx, flagy;
Point flagpath[30];

/*
======================
=
= SpawnFlag
=
======================
*/

void SpawnFlag(Sint16 x, Sint16 y)
{
	GetNewObj(false);
	new->needtoclip = cl_noclip;
	new->priority = 3;
	new->obclass = flagobj;
	new->active = ac_yes;
#if defined KEEN4
	new->x = CONVERT_TILE_TO_GLOBAL(x) + 6*PIXGLOBAL;
	new->y = CONVERT_TILE_TO_GLOBAL(y) + -30*PIXGLOBAL;
#elif defined KEEN5
	new->x = CONVERT_TILE_TO_GLOBAL(x) + -5*PIXGLOBAL;
	new->y = CONVERT_TILE_TO_GLOBAL(y) + -30*PIXGLOBAL;
#elif defined KEEN6
	new->x = CONVERT_TILE_TO_GLOBAL(x) + 6*PIXGLOBAL;		// useless!
	new->y = CONVERT_TILE_TO_GLOBAL(y) + -30*PIXGLOBAL;	// useless!
#if GRMODE == CGAGR
	new->x = CONVERT_TILE_TO_GLOBAL(x) + 10*PIXGLOBAL;
#else
	new->x = CONVERT_TILE_TO_GLOBAL(x) + 14*PIXGLOBAL;
#endif
	new->y = CONVERT_TILE_TO_GLOBAL(y) + -26*PIXGLOBAL;
	{
		Uint16 tile = *(mapsegs[1]+mapbwidthtable[y]/2 + x) + 1;
		RF_MemToMap(&tile, 1, x, y, 1, 1);
	}
#endif
	new->ticcount = US_RndT() / 16;
	NewState(new, &s_flagwave1);
}

#ifndef KEEN5
/*
======================
=
= SpawnThrowFlag
=
======================
*/

void SpawnThrowFlag(Sint16 x, Sint16 y)
{
	Sint16 i;
	Sint32 xdist, ydist;

	GetNewObj(false);
	new->needtoclip = cl_noclip;
	new->priority = 3;
	new->obclass = flagobj;
	new->active = ac_allways;
	new->x = gamestate.worldx - 16*PIXGLOBAL;
	new->y = gamestate.worldy - 16*PIXGLOBAL;
#if defined KEEN4
	new->temp1 = CONVERT_TILE_TO_GLOBAL(x) + 6*PIXGLOBAL;
	new->temp2 = CONVERT_TILE_TO_GLOBAL(y) + -38*PIXGLOBAL;
#elif defined KEEN6
	flagx = x;
	flagy = y;
#if GRMODE == CGAGR
	new->temp1 = CONVERT_TILE_TO_GLOBAL(x) + 10*PIXGLOBAL;
#else
	new->temp1 = CONVERT_TILE_TO_GLOBAL(x) + 14*PIXGLOBAL;
#endif
	new->temp2 = CONVERT_TILE_TO_GLOBAL(y) + -34*PIXGLOBAL;
#endif
	xdist = (Sint32)new->temp1 - (Sint32)new->x;
	ydist = (Sint32)new->temp2 - (Sint32)new->y;
	for (i = 0; i < 30; i++)
	{
		flagpath[i].x = new->x + (xdist * min(i, 24))/24;
		flagpath[i].y = new->y + (ydist * i)/30;
		if (i < 10)
		{
			flagpath[i].y -= i*3*PIXGLOBAL;
		}
		else if (i < 15)
		{
			flagpath[i].y -= i*PIXGLOBAL + 20*PIXGLOBAL;
		}
		else if (i < 20)
		{
			flagpath[i].y -= (20-i)*PIXGLOBAL + 30*PIXGLOBAL;
		}
		else
		{
			flagpath[i].y -= (29-i)*3*PIXGLOBAL;
		}
	}
	NewState(new, &s_throwflag0);
}

/*
======================
=
= TossThink
=
======================
*/

void TossThink(objtype *ob)
{
	if (screenfaded)
		return;

	SD_StopSound();
	SD_PlaySound(SND_FLAGSPIN);
	ob->state = ob->state->nextstate;
}

/*
======================
=
= PathThink
=
======================
*/

void PathThink(objtype *ob)
{
	ob->temp3 = ob->temp3 + tics;
	if (ob->temp3 > 58)
		ob->temp3 = 58;

	ob->x = flagpath[ob->temp3/2].x;
	ob->y = flagpath[ob->temp3/2].y;
	ob->needtoreact = true;
	if (ob->temp1 == 0)
	{
		SD_PlaySound(SND_FLAGSPIN);
	}
}

/*
======================
=
= FlagAlign
=
======================
*/

void FlagAlign(objtype *ob)
{
	ob->x = ob->temp1;
	ob->y = ob->temp2 + 8*PIXGLOBAL;
	SD_PlaySound(SND_FLAGLAND);
#ifdef KEEN6
	{
		Uint16 tile = *(mapsegs[1]+mapbwidthtable[flagy]/2 + flagx) + 1;
		RF_MemToMap(&tile, 1, flagx, flagy, 1, 1);
	}
#endif
}
#endif

/*
=============================================================================

						 NEURAL STUNNER

=============================================================================
*/
statetype s_stunray1 = {STUN1SPR, STUN1SPR, slide, false, false, 6, 64, 64, T_Shot, NULL, R_Shot, &s_stunray2};
statetype s_stunray2 = {STUN2SPR, STUN2SPR, slide, false, false, 6, 64, 64, T_Shot, NULL, R_Shot, &s_stunray3};
statetype s_stunray3 = {STUN3SPR, STUN3SPR, slide, false, false, 6, 64, 64, T_Shot, NULL, R_Shot, &s_stunray4};
statetype s_stunray4 = {STUN4SPR, STUN4SPR, slide, false, false, 6, 64, 64, T_Shot, NULL, R_Shot, &s_stunray1};

statetype s_stunhit  = {STUNHIT1SPR, STUNHIT1SPR, step, false, false, 12, 0, 0, NULL, NULL, R_Draw, &s_stunhit2};
statetype s_stunhit2 = {STUNHIT2SPR, STUNHIT2SPR, step, false, false, 12, 0, 0, NULL, NULL, R_Draw, NULL};

/*
======================
=
= SpawnShot
=
======================
*/

void SpawnShot(Uint16 x, Uint16 y, Direction dir)
{
	if (!gamestate.ammo)
	{
		SD_PlaySound(SND_USESWITCH);
		return;
	}

	gamestate.ammo--;
	GetNewObj(true);
	new->x = x;
	new->y = y;
	new->priority = 0;
	new->obclass = stunshotobj;
	new->active = ac_allways;
	SD_PlaySound(SND_KEENFIRE);
	switch (dir)
	{
	case dir_North:
		new->xdir = 0;
		new->ydir = -1;
		break;
	case dir_East:
		new->xdir = 1;
		new->ydir = 0;
		break;
	case dir_South:
		new->xdir = 0;
		new->ydir = 1;
		break;
	case dir_West:
		new->xdir = -1;
		new->ydir = 0;
		break;
	default:
		Quit("SpawnShot: Bad dir!");
		break;
	}
	NewState(new, &s_stunray1);

#ifdef KEEN6
	{
		objtype *ob;

		for (ob=player->next; ob; ob=ob->next)
		{
			if (ob->active
				&& new->right > ob->left && new->left < ob->right
				&& new->top < ob->bottom && new->bottom > ob->top
				&& ob->state->contact)
			{
				ob->state->contact(ob, new);
				return;
			}
		}
	}
#endif
}

/*
======================
=
= ExplodeShot
=
======================
*/

void ExplodeShot(objtype *ob)
{
	ob->obclass = inertobj;
	ChangeState(ob, &s_stunhit);
	SD_PlaySound(SND_SHOTEXPLODE);
}

/*
======================
=
= T_Shot
=
======================
*/

void T_Shot(objtype *ob)
{
	objtype *ob2;

	if (ob->tileright >= originxtile && ob->tilebottom >= originytile
		&& ob->tileleft <= originxtilemax && ob->tiletop <= originytilemax)
	{
		//object is visible, so do nothing
		return;
	}

	if (ob->tileright+10 < originxtile
		|| ob->tileleft-10 > originxtilemax
		|| ob->tilebottom+6 < originytile
		|| ob->tiletop-6 > originytilemax)
	{
		//shot is off-screen by more than half a screen, so remove it
		RemoveObj(ob);
		return;
	}

	//check for collisions with INACTIVE objects
	for (ob2 = player->next; ob2; ob2 = ob2->next)
	{
		if (!ob2->active && ob->right > ob2->left && ob->left < ob2->right
			&& ob->top < ob2->bottom && ob->bottom > ob2->top)
		{
			if (ob2->state->contact)
			{
				ob2->state->contact(ob2, ob);
				ob2->needtoreact = true;
				ob2->active = ac_yes;
			}

			if (ob->obclass == nothing)	//BUG: obclass is 'inertobj' for the exploded shot
				break;
		}
	}
}

/*
======================
=
= R_Shot
=
======================
*/

void R_Shot(objtype *ob)
{
	Uint16 tile;

	if (ob->hitnorth == 1 && ob->tileleft != ob->tileright)
	{
		tile = *(mapsegs[1]+mapbwidthtable[ob->tiletop-1]/2+ob->tileright);
		if (tinf[NORTHWALL+tile] == 17)
		{
			ob->hitnorth = 17;
			ob->x += 0x100 - (ob->x & 0xFF);
		}
	}
	else if (ob->hitnorth == 17 && ob->tileleft != ob->tileright)
	{
		ob->x &= 0xFF00;
	}
	if (ob->hitsouth == 1 && ob->tileleft != ob->tileright)
	{
		tile = *(mapsegs[1]+mapbwidthtable[ob->tilebottom+1]/2+ob->tileright);
		if (tinf[SOUTHWALL+tile] == 17)
		{
			ob->hitsouth = 17;
			ob->x += 0x100 - (ob->x & 0xFF);
		}
	}
	else if (ob->hitsouth == 17 && ob->tileleft != ob->tileright)
	{
		ob->x &= 0xFF00;
	}
	if (ob->hitnorth == 17 || ob->hitsouth == 17)
	{
		ytry = ob->state->ymove * tics * ob->ydir;
		ob->y += ytry;
		ob->top += ytry;
		ob->bottom += ytry;
		ob->tiletop = CONVERT_GLOBAL_TO_TILE(ob->top);
		ob->tilebottom = CONVERT_GLOBAL_TO_TILE(ob->bottom);
	}
	else if (ob->hitnorth || ob->hitsouth || ob->hiteast || ob->hitwest)
	{
		ExplodeShot(ob);
	}
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}

/*
=============================================================================

						 DOOR

temp1 = height of the door's main section (identical tiles!), in tiles
        DoorOpen changes two more tiles at the bottom end of the door
		  (total door height in tiles is temp1 + 2)

=============================================================================
*/

statetype s_door1    = {0, 0, step, false, false, 10, 0, 0, DoorOpen, NULL, NULL, &s_door2};
statetype s_door2    = {0, 0, step, false, false, 10, 0, 0, DoorOpen, NULL, NULL, &s_door3};
statetype s_door3    = {0, 0, step, false, false, 10, 0, 0, DoorOpen, NULL, NULL, NULL};

/*
======================
=
= DoorOpen
=
======================
*/

void DoorOpen(objtype *ob)
{
	Sint16 i;
	Uint16 far *map;
	Uint16 tiles[50];

	map = mapsegs[1] + mapbwidthtable[ob->y]/2 + ob->x;
	for (i=0; i < ob->temp1+2; i++, map+=mapwidth)
	{
		tiles[i] = *map + 1;
	}
	RF_MemToMap(tiles, 1, ob->x, ob->y, 1, ob->temp1+2);
}

#ifdef KEEN5
/*
=============================================================================

						 CARD DOOR

temp1 = frame counter

=============================================================================
*/
statetype s_carddoor    = {0, 0, step, false, false, 15, 0, 0, CardDoorOpen, NULL, NULL, &s_carddoor};

/*
======================
=
= CardDoorOpen
=
======================
*/

void CardDoorOpen(objtype *ob)
{
	Sint16 x, y;
	Uint16 far *map;
	Uint16 tiles[16], *tileptr;

	tileptr = tiles;
	map = mapsegs[1] + mapbwidthtable[ob->y]/2 + ob->x;
	for (y=0; y<4; y++, map+=mapwidth)
	{
		for (x=0; x<4; x++)
		{
			*tileptr++ = map[x]-4;
		}
	}
	RF_MemToMap(tiles, 1, ob->x, ob->y, 4, 4);

	if (++ob->temp1 == 3)
		ob->state = NULL;
}

#endif