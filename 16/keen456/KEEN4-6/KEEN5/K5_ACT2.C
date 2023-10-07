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
K5_ACT2.C
=========

Contains the following actor types (in this order):

- Sparky
- Little Ampton
- Slicestar
- Shelley

*/

#include "CK_DEF.H"

/*
=============================================================================

						  SPARKY

temp1 = charge countdown

=============================================================================
*/

statetype s_sparkywalk1   = {SPARKYWALKL1SPR, SPARKYWALKR1SPR, step,  false, true,  8, 128, 0, T_Sparky, C_Sparky, R_Sparky, &s_sparkywalk2};
statetype s_sparkywalk2   = {SPARKYWALKL2SPR, SPARKYWALKR2SPR, step,  false, true,  8, 128, 0, NULL, C_Sparky, R_Sparky, &s_sparkywalk3};
statetype s_sparkywalk3   = {SPARKYWALKL3SPR, SPARKYWALKR3SPR, step,  false, true,  8, 128, 0, NULL, C_Sparky, R_Sparky, &s_sparkywalk4};
statetype s_sparkywalk4   = {SPARKYWALKL4SPR, SPARKYWALKR4SPR, step,  false, true,  8, 128, 0, NULL, C_Sparky, R_Sparky, &s_sparkywalk1};
statetype s_sparkylook1   = {SPARKYTURN1SPR,  SPARKYTURN1SPR,  step,  false, true,  6,   0, 0, NULL, C_Sparky, R_Draw, &s_sparkylook2};
statetype s_sparkylook2   = {SPARKYTURN2SPR,  SPARKYTURN2SPR,  step,  false, true,  6,   0, 0, NULL, C_Sparky, R_Draw, &s_sparkylook3};
statetype s_sparkylook3   = {SPARKYTURN3SPR,  SPARKYTURN3SPR,  step,  false, true,  6,   0, 0, NULL, C_Sparky, R_Draw, &s_sparkylook4};
statetype s_sparkylook4   = {SPARKYWALKR1SPR, SPARKYWALKR1SPR, step,  false, true,  6,   0, 0, T_SparkyLookL, C_Sparky, R_Sparky, &s_sparkylook5};
statetype s_sparkylook5   = {SPARKYTURN3SPR,  SPARKYTURN3SPR,  step,  false, true,  6,   0, 0, NULL, C_Sparky, R_Draw, &s_sparkylook6};
statetype s_sparkylook6   = {SPARKYTURN2SPR,  SPARKYTURN3SPR,  step,  false, true,  6,   0, 0, NULL, C_Sparky, R_Draw, &s_sparkylook7};
statetype s_sparkylook7   = {SPARKYTURN1SPR,  SPARKYTURN3SPR,  step,  false, true,  6,   0, 0, NULL, C_Sparky, R_Draw, &s_sparkylook8};
statetype s_sparkylook8   = {SPARKYWALKL1SPR, SPARKYWALKR1SPR, step,  false, true,  6,   0, 0, T_SparkyLookR, C_Sparky, R_Sparky, &s_sparkywalk2};
statetype s_sparkyspeed1  = {SPARKYWALKL1SPR, SPARKYWALKR1SPR, step,  true,  true,  4,   0, 0, NULL, C_Sparky, R_Sparky, &s_sparkyspeed2};
statetype s_sparkyspeed2  = {SPARKYWALKL2SPR, SPARKYWALKR2SPR, step,  true,  true,  4,   0, 0, NULL, C_Sparky, R_Sparky, &s_sparkyspeed3};
statetype s_sparkyspeed3  = {SPARKYWALKL3SPR, SPARKYWALKR3SPR, step,  true,  true,  4,   0, 0, NULL, C_Sparky, R_Sparky, &s_sparkyspeed4};
statetype s_sparkyspeed4  = {SPARKYWALKL4SPR, SPARKYWALKR4SPR, step,  true,  true,  4,   0, 0, T_ChargeCount, C_Sparky, R_Sparky, &s_sparkyspeed1};
statetype s_sparkycharge1 = {SPARKYWALKL1SPR, SPARKYWALKR1SPR, step,  true,  true,  4, 128, 0, NULL, C_Sparky, R_Sparky, &s_sparkycharge2};
statetype s_sparkycharge2 = {SPARKYWALKL2SPR, SPARKYWALKR2SPR, step,  true,  true,  4, 128, 0, T_RunSnd1, C_Sparky, R_Sparky, &s_sparkycharge3};
statetype s_sparkycharge3 = {SPARKYWALKL3SPR, SPARKYWALKR3SPR, step,  true,  true,  4, 128, 0, NULL, C_Sparky, R_Sparky, &s_sparkycharge4};
statetype s_sparkycharge4 = {SPARKYWALKL4SPR, SPARKYWALKR4SPR, step,  true,  true,  4, 128, 0, T_RunSnd2, C_Sparky, R_Sparky, &s_sparkycharge1};
statetype s_sparkyturn1   = {SPARKYTURN3SPR,  SPARKYTURN1SPR,  step,  false, true,  8,   0, 0, NULL, C_Sparky, R_Draw, &s_sparkyturn2};
statetype s_sparkyturn2   = {SPARKYTURN2SPR,  SPARKYTURN2SPR,  step,  false, true,  8,   0, 0, NULL, C_Sparky, R_Draw, &s_sparkyturn3};
statetype s_sparkyturn3   = {SPARKYTURN1SPR,  SPARKYTURN3SPR,  step,  false, true,  8,   0, 0, NULL, C_Sparky, R_Draw, &s_sparkywalk1};
statetype s_sparkystun    = {SPARKYSTUNSPR,   SPARKYSTUNSPR,   think, false, false, 0,   0, 0, T_Projectile, NULL, R_Stunned, NULL};

/*
===========================
=
= SpawnSparky
=
===========================
*/

void SpawnSparky(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->obclass = sparkyobj;
	new->active = ac_yes;
	new->priority = 0;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY) + -1*TILEGLOBAL;
	if (US_RndT() < 0x80)
	{
		new->xdir = 1;
	}
	else
	{
		new->xdir = -1;
	}
	new->ydir = 1;
	NewState(new, &s_sparkywalk1);
}

/*
===========================
=
= T_Sparky
=
===========================
*/

void T_Sparky(objtype *ob)
{
	if (US_RndT() < 0x40)
	{
		ob->state = &s_sparkylook1;
		xtry = 0;
	}
}

/*
===========================
=
= T_ChargeCount
=
===========================
*/

void T_ChargeCount(objtype *ob)
{
	if (--ob->temp1 == 0)
		ob->state = &s_sparkycharge1;
}

/*
===========================
=
= T_SparkyLookL
=
===========================
*/

void T_SparkyLookL(objtype *ob)
{
	Uint16 dist = player->bottom + TILEGLOBAL - ob->bottom;
	if (dist > 2*TILEGLOBAL)
		return;

	if (player->x < ob->x)
	{	
		ob->xdir = -1;
		SD_PlaySound(SND_SPARKYCHARGE);
		ob->state = &s_sparkyspeed1;
		ob->temp1 = 3;
	}
}

/*
===========================
=
= T_SparkyLookR
=
===========================
*/

void T_SparkyLookR(objtype *ob)
{
	Uint16 dist = player->bottom + TILEGLOBAL - ob->bottom;
	if (dist > 2*TILEGLOBAL)
		return;

	if (player->x > ob->x)
	{
		ob->xdir = 1;
		SD_PlaySound(SND_SPARKYCHARGE);
		ob->state = &s_sparkyspeed1;
		ob->temp1 = 3;
	}
}

/*
===========================
=
= T_RunSnd1
=
===========================
*/

#pragma argsused
void T_RunSnd1(objtype *ob)
{
	SD_PlaySound(SND_WORLDWALK1);
}

/*
===========================
=
= T_RunSnd2
=
===========================
*/

#pragma argsused
void T_RunSnd2(objtype *ob)
{
	SD_PlaySound(SND_WORLDWALK1);
}

/*
===========================
=
= C_Sparky
=
===========================
*/

void C_Sparky(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		KillKeen();
	}
	else if (hit->obclass == stunshotobj)
	{
		StunObj(ob, hit, &s_sparkystun);
	}
}

/*
===========================
=
= R_Sparky
=
===========================
*/

void R_Sparky(objtype *ob)
{
	if (ob->xdir == 1 && ob->hitwest)
	{
		ob->x -= ob->xmove;
		ob->xdir = -1;
		ob->nothink = US_RndT() >> 5;
		ChangeState(ob, &s_sparkyturn1);
	}
	else if (ob->xdir == -1 && ob->hiteast)
	{
		ob->x -= ob->xmove;
		ob->xdir = 1;
		ob->nothink = US_RndT() >> 5;
		ChangeState(ob, &s_sparkyturn1);
	}
	else if (!ob->hitnorth)
	{
		ob->x -= ob->xmove;
		ob->xdir = -ob->xdir;
		ob->nothink = US_RndT() >> 5;
		ChangeState(ob, &s_sparkyturn1);
	}
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}

/*
=============================================================================

						  LITTLE AMPTON

=============================================================================
*/

statetype s_amptonwalk1    = {AMTONWALKL1SPR, AMPTONWALKR1SPR, step,       false, true,  8, 128,  0, T_Ampton, C_Ampton, R_Ampton, &s_amptonwalk2};
statetype s_amptonwalk2    = {AMTONWALKL2SPR, AMPTONWALKR2SPR, step,       false, true,  8, 128,  0, T_Ampton, C_Ampton, R_Ampton, &s_amptonwalk3};
statetype s_amptonwalk3    = {AMTONWALKL3SPR, AMPTONWALKR3SPR, step,       false, true,  8, 128,  0, T_Ampton, C_Ampton, R_Ampton, &s_amptonwalk4};
statetype s_amptonwalk4    = {AMTONWALKL4SPR, AMPTONWALKR4SPR, step,       false, true,  8, 128,  0, T_Ampton, C_Ampton, R_Ampton, &s_amptonwalk1};
statetype s_amptonturn     = {AMPTONFACESPR,  AMPTONFACESPR,   step,       false, true,  8,   0,  0, NULL, C_Ampton, R_Draw, &s_amptonwalk1};
statetype s_amptongrab1    = {AMPTONGRAB1SPR, AMPTONGRAB1SPR,  step,       false, true,  8,   0,  0, NULL, C_Ampton, R_Draw, &s_amptongrab2};
statetype s_amptongrab2    = {AMPTONGRAB2SPR, AMPTONGRAB2SPR,  step,       false, true,  8,   0,  0, NULL, C_Ampton, R_Draw, &s_amptonclimb};
statetype s_amptonclimb    = {AMPTONGRAB2SPR, AMPTONGRAB2SPR,  slidethink, false, false, 0,   0, 32, T_AmptonClimb, C_Ampton, R_Draw, NULL};
statetype s_amptonrelease1 = {AMPTONGRAB2SPR, AMPTONGRAB2SPR,  step,       false, false, 8,   0,  0, NULL, C_Ampton, R_Draw, &s_amptonrelease2};
statetype s_amptonrelease2 = {AMPTONGRAB1SPR, AMPTONGRAB1SPR,  step,       false, false, 8,   0,  0, T_SetNoThink, C_Ampton, R_Draw, &s_amptonwalk1};
statetype s_amptonfiddle1  = {AMPTONGRAB1SPR, AMPTONGRAB1SPR,  step,       false, true, 12,   0,  0, NULL, C_Ampton, R_Draw, &s_amptonfiddle2};
statetype s_amptonfiddle2  = {AMPTONGRAB2SPR, AMPTONGRAB2SPR,  step,       false, true, 12,   0,  0, NULL, C_Ampton, R_Draw, &s_amptonfiddle3};
statetype s_amptonfiddle3  = {AMPTONGRAB1SPR, AMPTONGRAB1SPR,  step,       false, true, 12,   0,  0, NULL, C_Ampton, R_Draw, &s_amptonfiddle4};
statetype s_amptonfiddle4  = {AMPTONGRAB2SPR, AMPTONGRAB2SPR,  step,       false, true, 12,   0,  0, NULL, C_Ampton, R_Draw, &s_amptonfiddle5};
statetype s_amptonfiddle5  = {AMPTONGRAB1SPR, AMPTONGRAB1SPR,  step,       false, true, 12,   0,  0, T_SetNoThink, C_Ampton, R_Draw, &s_amptonwalk1};
statetype s_amptonstun     = {AMPTONSTUNSPR,  AMPTONSTUNSPR,   think,      false, false, 0,   0,  0, T_Projectile, NULL, R_Stunned, NULL};

/*
===========================
=
= SpawnAmpton
=
===========================
*/

void SpawnAmpton(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->obclass = amptonobj;
	new->active = ac_yes;
	new->priority = 0;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY) + -8*PIXGLOBAL;
	if (US_RndT() < 0x80)
	{
		new->xdir = 1;
	}
	else
	{
		new->xdir = -1;
	}
	new->ydir = 1;
	NewState(new, &s_amptonwalk1);
}

/*
===========================
=
= T_Ampton
=
===========================
*/

void T_Ampton(objtype *ob)
{
	Uint16 far *map;
	Uint16 intile, var8;
	boolean poleup, poledown;

	if (ob->state == &s_amptonwalk1)
	{
		SD_PlaySound(SND_AMPTONWALK1);
	}
	else if (ob->state == &s_amptonwalk3)
	{
		SD_PlaySound(SND_AMPTONWALK2);
	}
	if (ob->x & 0xFF)
	{
		map = mapsegs[1] + mapbwidthtable[ob->tiletop]/2 + ob->tileleft + 1;
		intile = tinf[*map + INTILE] & INTILE_TYPEMASK;
		if (intile == INTILE_AMPTONCOMPUTER)
		{
			ob->state = &s_amptonfiddle1;
		}
		else if (intile == INTILE_POLE && US_RndT() < 196)
		{
			if ((tinf[*(map + mapwidth*2) + INTILE] & INTILE_TYPEMASK) == INTILE_POLE)
			{
				poledown = true;
			}
			else
			{
				poledown = false;
			}
			if ((tinf[*(map - mapwidth*2) + INTILE] & INTILE_TYPEMASK) == INTILE_POLE)
			{
				poleup = true;
			}
			else
			{
				poleup = false;
			}
			if (poleup && poledown)
			{
				if (US_RndT() < 0x80)
					poleup = false;
				else
					poledown = false;
			}

			if (poleup)
			{
				ob->ydir = -1;
				ob->state = &s_amptongrab1;
				ob->needtoclip = cl_noclip;
				ob->nothink = 6;
				xtry = 0;
			}
			else if (poledown)
			{
				ob->ydir = 1;
				ob->state = &s_amptongrab1;
				ob->needtoclip = cl_noclip;
				ob->nothink = 6;
				xtry = 0;
			}
		}
	}
}

/*
===========================
=
= T_AmptonClimb
=
===========================
*/

void T_AmptonClimb(objtype *ob)
{
	Uint16 newtile;
	Uint16 far *map;
	Uint16 move;

	newtile = CONVERT_GLOBAL_TO_TILE(ob->bottom + ytry);
	if (ob->tilebottom != newtile)
	{
		if (ob->ydir == -1)
		{
			map = mapsegs[1] + mapbwidthtable[newtile]/2 + ob->tileleft + 1;
			if (!tinf[map[0] + NORTHWALL] && tinf[map[mapwidth]+NORTHWALL])
			{
				if ((tinf[*(map-4*mapwidth)+INTILE] & INTILE_TYPEMASK) == INTILE_POLE && US_RndT() < 0x80)
					return;

				move = (ob->bottom & 0xFF) + 1;
				ob->y -= move;
				ob->bottom -= move;
				ob->needtoclip = cl_midclip;
				ob->state = &s_amptonrelease1;
				ytry = PIXGLOBAL;
				ob->ydir = 1;
				ClipToWalls(ob);
				ob->nothink = 4;
				return;
			}
			if ((tinf[*(map-mapwidth)+INTILE] & INTILE_TYPEMASK) != INTILE_POLE)
			{
				ytry = 0;
				ob->ydir = 1;
			}
		}
		else
		{
			map = mapsegs[1] + mapbwidthtable[newtile]/2 + ob->tileleft + 1;
			if (tinf[map[0] + NORTHWALL] && !tinf[*(map-mapwidth)+NORTHWALL])
			{
				if ((tinf[map[2*mapwidth] + INTILE] & INTILE_TYPEMASK) == INTILE_POLE && US_RndT() < 0x80)
					return;

				move = 0xFF - (ob->bottom & 0xFF);
				ob->y += move;
				ob->bottom += move;
				ob->needtoclip = cl_midclip;
				ob->state = &s_amptonrelease1;
				ytry = PIXGLOBAL;
				ClipToWalls(ob);
				ob->nothink = 4;
				return;
			}
			if ((tinf[map[0] + INTILE] & INTILE_TYPEMASK) != INTILE_POLE)
			{
				ytry = 0;
				ob->ydir = -1;
			}
		}
	}
}

/*
===========================
=
= T_SetNoThink
=
===========================
*/

void T_SetNoThink(objtype *ob)
{
	ob->nothink = 4;
}

/*
===========================
=
= C_Ampton
=
===========================
*/

void C_Ampton(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		if (ob->state == &s_amptonclimb)
		{
			KillKeen();
		}
		else
		{
			ClipToSpriteSide(hit, ob);
		}
	}
	else if (hit->obclass == stunshotobj)
	{
		ob->needtoclip = cl_midclip;
		ob->ydir = 1;
		ob->yspeed = 0;
		SD_PlaySound(SND_AMPTONDIE);
		StunObj(ob, hit, &s_amptonstun);
	}
}

/*
===========================
=
= R_Ampton
=
===========================
*/

void R_Ampton(objtype *ob)
{
	if (ob->xdir == 1 && ob->hitwest)
	{
		ob->x -= ob->xmove;
		ob->xdir = -1;
		ChangeState(ob, &s_amptonturn);
	}
	else if (ob->xdir == -1 && ob->hiteast)
	{
		ob->x -= ob->xmove;
		ob->xdir = 1;
		ChangeState(ob, &s_amptonturn);
	}
	else if (!ob->hitnorth)
	{
		ob->x -= ob->xmove;
		ob->xdir = -ob->xdir;
		ChangeState(ob, &s_amptonturn);
	}
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}

/*
=============================================================================

						  SLICESTAR

temp4 = health

=============================================================================
*/

statetype s_slicestarslide  = {SLICESTARSPR,     SLICESTARSPR,     think, false, false,  0,  0,  0, T_Platform, C_Slicestar, R_Draw, NULL};
statetype s_slicestarbounce = {SLICESTARSPR,     SLICESTARSPR,     slide, false, false,  0, 24, 24, NULL, C_Slicestar, R_Slicestar, &s_slicestarbounce};
statetype s_slicestarboom   = {SLICESTARBOOMSPR, SLICESTARBOOMSPR, step,  false, false, 20,  0,  0, NULL, NULL, R_Draw, NULL};

/*
===========================
=
= SpawnSlicestarSlide
=
===========================
*/

void SpawnSlicestarSlide(Uint16 tileX, Uint16 tileY, Sint16 dir)
{
	GetNewObj(false);
	new->obclass = slicestarobj;
	new->active = ac_yes;
	new->priority = 2;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY);
	new->temp4 = 20;	// health!
	switch (dir)
	{
	case 0:
		new->xdir = 0;
		new->ydir = -1;
		break;
	case 1:
		new->xdir = 1;
		new->ydir = 0;
		break;
	case 2:
		new->xdir = 0;
		new->ydir = 1;
		break;
	case 3:
		new->xdir = -1;
		new->ydir = 0;
	}
	NewState(new, &s_slicestarslide);
}

/*
===========================
=
= SpawnSlicestarBounce
=
===========================
*/

void SpawnSlicestarBounce(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->obclass = slicestarobj;
	new->active = ac_yes;
	new->priority = 2;
	new->needtoclip = cl_fullclip;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY);
	new->temp4 = 50;	// health!
	switch (US_RndT() / 0x40)
	{
	case 0:
		new->xdir = -1;
		new->ydir = -1;
		break;
	case 1:
		new->xdir = 1;
		new->ydir = 1;
		break;
	case 2:
		new->xdir = -1;
		new->ydir = 1;
		break;
	case 3:
		new->xdir = 1;
		new->ydir = -1;
	}
	NewState(new, &s_slicestarbounce);
}

/*
===========================
=
= C_Slicestar
=
===========================
*/

void C_Slicestar(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		KillKeen();
	}
	else if (hit->obclass == stunshotobj)
	{
		ExplodeShot(hit);
		if (--ob->temp4 == 0)
		{
			ChangeState(ob, &s_slicestarboom);
		}
	}
}

/*
===========================
=
= R_Slicestar
=
===========================
*/

void R_Slicestar(objtype *ob)
{
	if (ob->hitnorth)
	{
		ob->ydir = -1;
		SD_PlaySound(SND_SLICESTARBOUNCE);
	}
	else if (ob->hitsouth)
	{
		ob->ydir = 1;
		SD_PlaySound(SND_SLICESTARBOUNCE);
	}
	if (ob->hitwest)
	{
		ob->xdir = -1;
		SD_PlaySound(SND_SLICESTARBOUNCE);
	}
	else if (ob->hiteast)
	{
		ob->xdir = 1;
		SD_PlaySound(SND_SLICESTARBOUNCE);
	}
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}

/*
=============================================================================

						  SHELLEY

=============================================================================
*/

statetype s_shellywalk1  = {SHELLEYL1SPR,     SHELLEYR1SPR,     step,      false, true,    8, 128, 0, NULL, C_Shelly, R_Shelly, &s_shellywalk2};
statetype s_shellywalk2  = {SHELLEYL2SPR,     SHELLEYR2SPR,     step,      false, true,    8, 128, 0, NULL, C_Shelly, R_Shelly, &s_shellywalk3};
statetype s_shellywalk3  = {SHELLEYL3SPR,     SHELLEYR3SPR,     step,      false, true,    8, 128, 0, NULL, C_Shelly, R_Shelly, &s_shellywalk4};
statetype s_shellywalk4  = {SHELLEYL4SPR,     SHELLEYR4SPR,     step,      false, true,    8, 128, 0, NULL, C_Shelly, R_Shelly, &s_shellywalk1};
statetype s_shellylook   = {SHELLEYL2SPR,     SHELLEYR2SPR,     stepthink, false, true,  100,   0, 0, T_ShellyLook, C_Shelly, R_Draw, &s_shellylook2};
statetype s_shellylook2  = {SHELLEYL2SPR,     SHELLEYR2SPR,     step,      true,  true,    1,   0, 0, T_Turn, C_Shelly, R_Draw, &s_shellywalk1};
statetype s_shellyjump1  = {SHELLEYJUMPLSPR,  SHELLEYJUMPRSPR,  stepthink, false, false,   8,   0, 0, T_Projectile, C_Shelly, R_Shell, &s_shellyjump2};
statetype s_shellyjump2  = {SHELLEYFALLLSPR,  SHELLEYFALLRSPR,  think,     false, false,   8,   0, 0, T_Projectile, C_Shelly, R_Shell, NULL};
statetype s_shellydie    = {SHELLEYFALLLSPR,  SHELLEYFALLRSPR,  step,      false, false,   8,   0, 0, T_ShellyFrag, NULL, R_Shell, NULL};
statetype s_shellydieup  = {SHELLEYL2SPR,     SHELLEYR2SPR,     step,      false, false,   8,   0, 0, T_ShellyFrag, NULL, R_Shell, NULL};
statetype s_shellyboom1  = {SHELLEYBOOM1SPR,  SHELLEYBOOM1SPR,  step,      false, false,  20,   0, 0, NULL, C_Lethal, R_Draw, &s_shellyboom2};
statetype s_shellyboom2  = {SHELLEYBOOM2SPR,  SHELLEYBOOM2SPR,  step,      false, false,  20,   0, 0, NULL, C_Lethal, R_Draw, &s_shellyboom3};
statetype s_shellyboom3  = {SHELLEYBOOM3SPR,  SHELLEYBOOM3SPR,  step,      false, false,  20,   0, 0, NULL, C_Lethal, R_Draw, &s_shellyboom4};
statetype s_shellyboom4  = {SHELLEYBOOM4SPR,  SHELLEYBOOM4SPR,  step,      false, false,  20,   0, 0, NULL, C_Lethal, R_Draw, NULL};
statetype s_shellypiece1 = {SHELLEYPIECE1SPR, SHELLEYPIECE1SPR, think,     false, false,   8,   0, 0, T_Projectile, C_Lethal, R_Bounce, NULL};
statetype s_shellypiece2 = {SHELLEYPIECE2SPR, SHELLEYPIECE2SPR, think,     false, false,   8,   0, 0, T_Projectile, C_Lethal, R_Bounce, NULL};

/*
===========================
=
= SpawnShelly
=
===========================
*/

void SpawnShelly(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->obclass = sparkyobj;	// BUG: should use shelleyobj
	new->active = ac_yes;
	new->priority = 0;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY);
	if (US_RndT() < 0x80)
	{
		new->xdir = 1;
	}
	else
	{
		new->xdir = -1;
	}
	new->ydir = 1;
	NewState(new, &s_shellywalk1);
}

/*
===========================
=
= T_ShellyLook
=
===========================
*/

void T_ShellyLook(objtype *ob)
{
	Sint16 xdist;

	if (player->top < ob->bottom)
		return;

	xdist = player->midx - ob->midx;
	if (ob->xdir == 1)
	{
		if (xdist > 1*TILEGLOBAL && xdist < 3*TILEGLOBAL)
		{
			ob->xspeed = 16;
			ob->yspeed = -24;
			ob->state = &s_shellyjump1;
			xtry = ytry = 0;
		}
	}
	else
	{
		if (xdist < -1*TILEGLOBAL && xdist > -3*TILEGLOBAL)
		{
			ob->xspeed = -16;
			ob->yspeed = -24;
			ob->state = &s_shellyjump1;
			xtry = ytry = 0;
		}
	}
}

/*
===========================
=
= T_Turn
=
===========================
*/

void T_Turn(objtype *ob)
{
	ob->xdir = -ob->xdir;
}

/*
===========================
=
= T_ShellyFrag
=
===========================
*/

void T_ShellyFrag(objtype *ob)
{
	GetNewObj(true);
	new->x = ob->x;
	new->y = ob->y;
	new->xspeed = 32;
	new->yspeed = -24;
	NewState(new, &s_shellypiece1);

	GetNewObj(true);
	new->x = ob->x;
	new->y = ob->y;
	new->xspeed = -32;
	new->yspeed = -24;
	NewState(new, &s_shellypiece2);
}

/*
===========================
=
= C_Shelly
=
===========================
*/

void C_Shelly(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		ClipToSpriteSide(hit, ob);
		if (player->midx < ob->left || player->midx > ob->right)
			return;
	}
	else if (hit->obclass == stunshotobj)
	{
		ExplodeShot(hit);
	}
	else if (hit->obclass == mshotobj)
	{
		RemoveObj(hit);
	}
	else
		return;

explode:
	SD_PlaySound(SND_SHELLEYEXPLODE);
	if (ob->hitnorth)
	{
		ChangeState(ob, &s_shellydieup);
	}
	else
	{
		ChangeState(ob, &s_shellydie);
	}
	GetNewObj(true);
	new->x = ob->x;
	new->y = ob->y;
	NewState(new, &s_shellyboom1);
}

/*
===========================
=
= R_Shelly
=
===========================
*/

void R_Shelly(objtype *ob)
{
	if (ob->xdir == 1 && ob->hitwest)
	{
		ob->x -= ob->xmove;
		ob->xdir = -1;
	}
	else if (ob->xdir == -1 && ob->hiteast)
	{
		ob->x -= ob->xmove;
		ob->xdir = 1;
	}
	else if (!ob->hitnorth)
	{
		ob->x -= ob->xmove;
		ChangeState(ob, &s_shellylook);
	}
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}

/*
===========================
=
= R_Shell
=
===========================
*/

void R_Shell(objtype *ob)
{
	if (ob->hiteast || ob->hitwest)
	{
		ob->xspeed = 0;
	}
	if (ob->hitnorth)
	{
		SD_PlaySound(SND_SHELLEYEXPLODE);
		ChangeState(ob, &s_shellydie);

		GetNewObj(true);
		new->x = ob->x;
		new->y = ob->y;
		NewState(new, &s_shellyboom1);
	}
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}