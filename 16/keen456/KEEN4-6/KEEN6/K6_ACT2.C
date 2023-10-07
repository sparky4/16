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
K6_ACT2.C
=========

Contains the following actor types (in this order):

- Nospike
- Gik
- Turrets
- Orbatrix
- Bip & Bipship
- Flect

*/

#include "CK_DEF.H"

/*
=============================================================================

						  NOSPIKE

temp1 = step counter for running on thin air
temp2 = low byte: running flag; high byte: flash countdown
temp3 = sprite pointer for the question mark
temp4 = health

=============================================================================
*/

statetype s_nospikestand     = {NOSPIKESTANDSPR,   NOSPIKESTANDSPR,   step,  false, true,  90,   0, 0, NULL, C_Nospike, R_Walk, &s_nospikewalk1};
statetype s_nospikewalk1     = {NOSPIKEWALKL1SPR,  NOSPIKEWALKR1SPR,  step,  false, true,  10, 128, 0, T_NospikeWalk, C_Nospike, R_Walk, &s_nospikewalk2};
statetype s_nospikewalk2     = {NOSPIKEWALKL2SPR,  NOSPIKEWALKR2SPR,  step,  false, true,  10, 128, 0, T_NospikeWalk, C_Nospike, R_Walk, &s_nospikewalk3};
statetype s_nospikewalk3     = {NOSPIKEWALKL3SPR,  NOSPIKEWALKR3SPR,  step,  false, true,  10, 128, 0, T_NospikeWalk, C_Nospike, R_Walk, &s_nospikewalk4};
statetype s_nospikewalk4     = {NOSPIKEWALKL4SPR,  NOSPIKEWALKR4SPR,  step,  false, true,  10, 128, 0, T_NospikeWalk, C_Nospike, R_Walk, &s_nospikewalk1};
statetype s_nospikerun1      = {NOSPIKERUNL1SPR,   NOSPIKERUNR1SPR,   step,  false, true,   4, 128, 0, T_NospikeRun, C_Nospike, R_NospikeRun, &s_nospikerun2};
statetype s_nospikerun2      = {NOSPIKERUNL2SPR,   NOSPIKERUNR2SPR,   step,  false, true,   4, 128, 0, T_NospikeRun, C_Nospike, R_NospikeRun, &s_nospikerun3};
statetype s_nospikerun3      = {NOSPIKERUNL3SPR,   NOSPIKERUNR3SPR,   step,  false, true,   4, 128, 0, T_NospikeRun, C_Nospike, R_NospikeRun, &s_nospikerun4};
statetype s_nospikerun4      = {NOSPIKERUNL4SPR,   NOSPIKERUNR4SPR,   step,  false, true,   4, 128, 0, T_NospikeRun, C_Nospike, R_NospikeRun, &s_nospikerun1};
statetype s_nospikeconfused1 = {NOSPIKESTANDSPR,   NOSPIKESTANDSPR,   step,  false, false, 20,   0, 1, NULL, NULL, R_Draw, &s_nospikeconfused2};
statetype s_nospikeconfused2 = {NOSPIKESTANDSPR,   NOSPIKESTANDSPR,   step,  false, false, 90,   0, 0, T_NospikeConfused, NULL, R_NospikeConfused, &s_nospikeconfused3};
statetype s_nospikeconfused3 = {NOSPIKESTANDSPR,   NOSPIKESTANDSPR,   step,  false, false, 20,   0, 0, NULL, NULL, R_Draw, &s_nospikefall};
statetype s_nospikefall      = {NOSPIKESTANDSPR,   NOSPIKESTANDSPR,   think, false, false,  0,   0, 0, T_Projectile, NULL, R_NospikeFall, NULL};
statetype s_nospikestun      = {NOSPIKESTUNSPR,    NOSPIKESTUNSPR,    think, false, false,  0,   0, 0, T_Projectile, NULL, R_Stunned, &s_nospikestun};

/*
===========================
=
= SpawnNospike
=
===========================
*/

void SpawnNospike(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->obclass = nospikeobj;
	new->active = ac_yes;
	new->priority = 0;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY) + -24*PIXGLOBAL;
	if (US_RndT() < 0x80)
	{
		new->xdir = 1;
	}
	else
	{
		new->xdir = -1;
	}
	new->ydir = 1;
	NewState(new, &s_nospikestand);
	new->temp4 = 4;	// health
}

/*
===========================
=
= T_NospikeWalk
=
===========================
*/

void T_NospikeWalk(objtype *ob)
{
	if (US_RndT() < 0x10)
	{
		ob->state = &s_nospikestand;
	}
	else if (ob->bottom == player->bottom && US_RndT() <= 0x20)
	{
		//
		// start running towards player
		//
		if (player->x > ob->x)
		{
			ob->xdir = 1;
		}
		else
		{
			ob->xdir = -1;
		}
		ob->temp1 = 0;	// nospike is still on solid ground (should already be 0 anyway)
		ob->temp2 = 1;	// nospike is running
		if (ob->state == &s_nospikewalk1)
		{
			ob->state = &s_nospikerun2;
		}
		else if (ob->state == &s_nospikewalk2)
		{
			ob->state = &s_nospikerun3;
		}
		else if (ob->state == &s_nospikewalk3)
		{
			ob->state = &s_nospikerun4;
		}
		else if (ob->state == &s_nospikewalk4)
		{
			ob->state = &s_nospikerun1;
		}
	}
}

/*
===========================
=
= T_NospikeRun
=
===========================
*/

void T_NospikeRun(objtype *ob)
{
	if (ob->temp1)
		return;	// nospike is running on thin air, so we'd better not stop

	if ( ( ( player->bottom != ob->bottom	// not on same ground level as Keen?
			 || (ob->xdir == -1 && ob->x < player->x)
			 || (ob->xdir == 1 && ob->x > player->x) ) // already ran past Keen?
		  && US_RndT() < 8 )
		|| !OnScreen(ob) )	// always stop running when off-screen
	{
		//
		// stop running
		//
		ob->temp2 = 0;
		if (ob->state == &s_nospikerun1)
		{
			ob->state = &s_nospikewalk2;
		}
		else if (ob->state == &s_nospikerun2)
		{
			ob->state = &s_nospikewalk3;
		}
		else if (ob->state == &s_nospikerun3)
		{
			ob->state = &s_nospikewalk4;
		}
		else if (ob->state == &s_nospikerun4)
		{
			ob->state = &s_nospikewalk1;
		}
	}
}

/*
===========================
=
= C_Nospike
=
===========================
*/

void C_Nospike(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		KillKeen();
	}
	else if (hit->obclass == stunshotobj)
	{
		if (--ob->temp4 == 0)	// handle health
		{
			StunObj(ob, hit, &s_nospikestun);
			ob->yspeed = -24;
		}
		else
		{
			if (player->x > ob->x)
			{
				ob->xdir = 1;
			}
			else
			{
				ob->xdir = -1;
			}
			ob->temp2 |= 0x400;	// draw white 4 times
			ob->needtoreact = true;
			if (ob->state == &s_nospikestand || ob->state == &s_nospikewalk1)
			{
				ChangeState(ob, &s_nospikerun2);
			}
			else if (ob->state == &s_nospikewalk2)
			{
				ChangeState(ob, &s_nospikerun3);
			}
			else if (ob->state == &s_nospikewalk3)
			{
				ChangeState(ob, &s_nospikerun4);
			}
			else if (ob->state == &s_nospikewalk4)
			{
				ChangeState(ob, &s_nospikerun1);
			}
			ExplodeShot(hit);
		}
	}
	else if (hit->obclass == nospikeobj
		&& (hit->temp2 & 0xFF) && (ob->temp2 & 0xFF)	// both nospikes are running?
		&& hit->xdir != ob->xdir)	// running in opposite directions?
	{
		//
		// stun both nospikes
		//
		ob->temp1=ob->temp2=ob->temp3=hit->temp1=hit->temp2=hit->temp3 = 0;
		ob->temp4 = hit->temp4 = ob->obclass;
		ChangeState(ob, &s_nospikestun);
		ChangeState(hit, &s_nospikestun);
		SD_PlaySound(SND_SMASH);
		ob->obclass = hit->obclass = stunnedobj;
		ob->yspeed = hit->yspeed = -24;
	}
}

/*
===========================
=
= T_NospikeConfused
=
===========================
*/

void T_NospikeConfused(objtype* ob)
{
	RF_RemoveSprite((void**)&ob->temp3);
}

/*
===========================
=
= R_NospikeConfused
=
===========================
*/

void R_NospikeConfused(objtype *ob)
{
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
	RF_PlaceSprite((void**)&ob->temp3, ob->x+TILEGLOBAL, ob->y-8*PIXGLOBAL, QUESTIONMARKSPR, spritedraw, 3);
}

/*
===========================
=
= R_NospikeFall
=
===========================
*/

void R_NospikeFall(objtype *ob)
{
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
	if (ob->hitnorth)
	{
		ob->temp1=ob->temp2=ob->temp3 = 0;
		ob->temp4 = ob->obclass;
		ChangeState(ob, &s_nospikestun);
		SD_PlaySound(SND_SMASH);
		ob->obclass = stunnedobj;
		ob->yspeed = -24;
	}
}

/*
===========================
=
= R_NospikeRun
=
===========================
*/

void R_NospikeRun(objtype *ob)
{
	if (ob->hitnorth)
	{
		ob->temp1 = 0;	// on solid ground
		if (ob->hiteast || ob->hitwest)
		{
			ob->x -= ob->xdir << 7;
			NewState(ob, &s_nospikestand);
			RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
			ob->temp2 = 0;	// no longer running or flashing white
			return;
		}
	}
	else
	{
		if (++ob->temp1 == 6)	// not on solid ground for 6 steps?
		{
			ChangeState(ob, &s_nospikeconfused1);
#if 0
			// bugfix:
			ob->nothink = 0;	// to make sure T_NospikeConfused can remove the question mark sprite
#endif
		}
	}
	if (ob->temp2 & 0xFF00)
	{
		ob->temp2 -= 0x100;
		RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, maskdraw, ob->priority);
	}
	else
	{
		RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
	}
}

/*
=============================================================================

						  GIK

=============================================================================
*/

statetype s_gikwalk1      = {GIKWALKL1SPR,  GIKWALKR1SPR,  step,      false, true,  10, 128, 0, T_GikWalk, C_ClipTop, R_Walk, &s_gikwalk2};
statetype s_gikwalk2      = {GIKWALKL2SPR,  GIKWALKR2SPR,  step,      false, true,  10, 128, 0, T_GikWalk, C_ClipTop, R_Walk, &s_gikwalk3};
statetype s_gikwalk3      = {GIKWALKL3SPR,  GIKWALKR3SPR,  step,      false, true,  10, 128, 0, T_GikWalk, C_ClipTop, R_Walk, &s_gikwalk1};
statetype s_gikjump       = {GIKJUMPLSPR,   GIKJUMPRSPR,   think,     false, false,  0,   0, 0, T_Projectile, C_ClipSide, R_GikJump, &s_gikslide1};
statetype s_gikslide1     = {GIKSLIDEL1SPR, GIKSLIDER1SPR, stepthink, false, false,  6,   0, 0, T_GikSlide, C_Lethal, R_GikSlide, &s_gikslide2};
statetype s_gikslide2     = {GIKSLIDEL2SPR, GIKSLIDER2SPR, stepthink, false, false,  6,   0, 0, T_GikSlide, C_Lethal, R_GikSlide, &s_gikslide1};
statetype s_gikstand      = {GIKSLIDEL1SPR, GIKSLIDER1SPR, step,      false, true,  20,   0, 0, NULL, C_Lethal, R_Walk, &s_gikwalk1};

/*
===========================
=
= SpawnGik
=
===========================
*/

void SpawnGik(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->obclass = gikobj;
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
	NewState(new, &s_gikwalk1);
}

/*
===========================
=
= T_GikWalk
=
===========================
*/

void T_GikWalk(objtype *ob)
{
	Sint16 xdist;

	if (ob->hitnorth != 9)	// if NOT on flat ground that kills Keen
	{
		xdist = player->x - ob->x;
		if (player->bottom <= ob->bottom && ob->bottom - player->bottom <= 4*TILEGLOBAL)
		{
			if (xdist < 0)
			{
				ob->xdir = -1;
			}
			else
			{
				ob->xdir = 1;
			}
			if (xdist >= -7*TILEGLOBAL && xdist <= 7*TILEGLOBAL
				&& (xdist <= -TILEGLOBAL || xdist >= TILEGLOBAL) )
			{
				if (xdist < 0)
				{
					ob->xspeed = -40;
				}
				else
				{
					ob->xspeed = 40;
				}
				ob->yspeed = -28;
				ob->state = &s_gikjump;
				SD_PlaySound(SND_GIKJUMP);
			}
		}
	}
}

/*
===========================
=
= T_GikSlide
=
===========================
*/

void T_GikSlide(objtype *ob)
{
	// tic masks for friction, based on slope type and direction
	// 0 - no friction
	// 7 - lowest friction (speed decreases every 8 tics)
	// 3 - medium friction (speed decreases every 4 tics)
	// 1 - highest friction (speed decreases every 2 tics)
	static Sint16 rticmask[8] = {0, 7, 0, 0, 0, 3, 3, 1};
	static Sint16 lticmask[8] = {0, 7, 3, 3, 1, 0, 0, 0};

	Sint16 ticmask;
	Sint16 slope;
	Sint32 i;

	DoGravity(ob);

	slope = ob->hitnorth & 7;
	if (ob->xdir == 1)
	{
		ticmask = rticmask[slope];
	}
	else
	{
		ticmask = lticmask[slope];
	}

	if (ob->xspeed == 0 && ob->hitnorth)
	{
		ob->state = &s_gikstand;
	}
	else
	{
		for (i = lasttimecount-tics; i < lasttimecount; i++)
		{
			if (ticmask && !(i & ticmask))
			{
				if ((ob->xspeed < 0 && ++ob->xspeed == 0)
					|| (ob-> xspeed > 0 && --ob->xspeed == 0))
				{
					ob->state = &s_gikstand;
					return;
				}
			}
			xtry += ob->xspeed;
		}
	}
}

/*
===========================
=
= R_GikJump
=
===========================
*/

void R_GikJump(objtype *ob)
{
	if (ob->hiteast || ob->hitwest)
		ob->xspeed = 0;

	if (ob->hitsouth)
		ob->yspeed = 0;

	if (ob->hitnorth)
	{
		ob->yspeed = 0;
		SD_PlaySound(SND_GIKLAND);
		ChangeState(ob, ob->state->nextstate);
	}
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}

/*
===========================
=
= R_GikSlide
=
===========================
*/

void R_GikSlide(objtype *ob)
{
	if ((ob->hiteast && ob->xspeed < 0) || (ob->hitwest && ob->xspeed > 0))
		ob->xspeed = 0;

	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}

/*
=============================================================================

						  CANNON

temp1 = direction

=============================================================================
*/

statetype s_cannon     = {0,            0,            step,      false, false, 120, 0, 0, NULL, NULL, R_Draw, &s_cannonfire};
statetype s_cannonfire = {0,            0,            step,      true,  false,   1, 0, 0, T_Cannon, NULL, R_Draw, &s_cannon};
statetype s_cshot1     = {LASER1SPR,    LASER1SPR,    stepthink, false, false,   8, 0, 0, T_Velocity, C_CShot, R_CShot, &s_cshot2};
statetype s_cshot2     = {LASER2SPR,    LASER2SPR,    stepthink, false, false,   8, 0, 0, T_Velocity, C_CShot, R_CShot, &s_cshot3};
statetype s_cshot3     = {LASER3SPR,    LASER3SPR,    stepthink, false, false,   8, 0, 0, T_Velocity, C_CShot, R_CShot, &s_cshot4};
statetype s_cshot4     = {LASER4SPR,    LASER4SPR,    stepthink, false, false,   8, 0, 0, T_Velocity, C_CShot, R_CShot, &s_cshot1};
statetype s_cshothit1  = {LASERHIT1SPR, LASERHIT1SPR, step,      false, false,  10, 0, 0, NULL, NULL, R_Draw, &s_cshothit2};
statetype s_cshothit2  = {LASERHIT2SPR, LASERHIT2SPR, step,      false, false,  10, 0, 0, NULL, NULL, R_Draw, NULL};

/*
===========================
=
= SpawnCannon
=
===========================
*/

void SpawnCannon(Uint16 tileX, Uint16 tileY, Sint16 dir)
{
	GetNewObj(false);
	new->obclass = cannonobj;
	new->active = ac_yes;
	new->tileright = new->tileleft = tileX;
	new->tiletop = new->tilebottom = tileY;
	new->x = new->left = new->right = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = new->top = new->bottom = CONVERT_TILE_TO_GLOBAL(tileY);
	new->temp1 = dir;
	NewState(new, &s_cannon);
}

/*
===========================
=
= T_Cannon
=
===========================
*/

void T_Cannon(objtype *ob)
{
	GetNewObj(true);
	new->obclass = mshotobj;
	new->active = ac_yes;	// BUG? NOT removable in Keen 6 (checked v1.0, v1.4 and v1.5)
	new->x = ob->x;
	new->y = ob->y;
	switch (ob->temp1)
	{
	case 0:
		new->yspeed = -64;
		break;
	case 1:
		new->xspeed = 64;
		break;
	case 2:
		new->yspeed = 64;
		break;
	case 3:
		new->xspeed = -64;
	}
	NewState(new, &s_cshot1);
	SD_PlaySound(SND_ENEMYSHOT);
}

/*
===========================
=
= C_CShot
=
===========================
*/

void C_CShot(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		KillKeen();
		ChangeState(ob, &s_cshothit1);
	}
}

/*
===========================
=
= R_CShot
=
===========================
*/

void R_CShot(objtype *ob)
{
	if (ob->hitnorth || ob->hiteast || ob->hitsouth || ob->hitwest)
	{
		SD_PlaySound(SND_ENEMYSHOTEXPLODE);
		ChangeState(ob, &s_cshothit1);
	}
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}

/*
=============================================================================

						  ORBATRIX

temp1 = bounce counter
temp2 = amount to move up during uncurl animation
temp3 = float offset
temp4 = float direction (up or down)

=============================================================================
*/

statetype s_orbatrix1       = {ORBATRIXL1SPR,    ORBATRIXR1SPR,    slide,     false, true,  12, 16, 0, T_OrbatrixFly, C_Orbatrix, R_Orbatrix, &s_orbatrix2};
statetype s_orbatrix2       = {ORBATRIXL2SPR,    ORBATRIXR2SPR,    slide,     false, true,  12, 16, 0, T_OrbatrixFly, C_Orbatrix, R_Orbatrix, &s_orbatrix1};
statetype s_orbatrixcurl1   = {ORBATRIX1SPR,     ORBATRIX1SPR,     stepthink, false, true,  12,  0, 0, NULL, C_Orbatrix, R_Orbatrix, &s_orbatrixcurl2};
statetype s_orbatrixcurl2   = {ORBATRIXCURLSPR,  ORBATRIXCURLSPR,  stepthink, false, true,  12,  0, 0, NULL, C_Orbatrix, R_Orbatrix, &s_orbatrixcurl3};
statetype s_orbatrixcurl3   = {ORBATRIXCURLSPR,  ORBATRIXCURLSPR,  think,     false, true,  12,  0, 0, T_OrbatrixCurl, C_Orbatrix, R_Orbatrix, &s_orbatrixbounce1};
statetype s_orbatrixuncurl1 = {ORBATRIXSPIN1SPR, ORBATRIXSPIN1SPR, think,     false, false, 12,  0, 0, T_OrbatrixUncurl, C_OrbatrixBounce, R_Draw, &s_orbatrixuncurl2};
statetype s_orbatrixuncurl2 = {ORBATRIXCURLSPR,  ORBATRIXCURLSPR,  step,      false, false, 12,  0, 0, NULL, C_OrbatrixBounce, R_Draw, &s_orbatrixidle1};
statetype s_orbatrixidle1   = {ORBATRIX1SPR,     ORBATRIX1SPR,     stepthink, false, true,  12,  0, 0, NULL, C_Orbatrix, R_Orbatrix, &s_orbatrixidle2};
statetype s_orbatrixidle2   = {ORBATRIX2SPR,     ORBATRIX2SPR,     stepthink, false, true,  12,  0, 0, NULL, C_Orbatrix, R_Orbatrix, &s_orbatrixidle3};
statetype s_orbatrixidle3   = {ORBATRIX3SPR,     ORBATRIX3SPR,     stepthink, false, true,  12,  0, 0, NULL, C_Orbatrix, R_Orbatrix, &s_orbatrixidle4};
statetype s_orbatrixidle4   = {ORBATRIX4SPR,     ORBATRIX4SPR,     stepthink, false, true,  12,  0, 0, NULL, C_Orbatrix, R_Orbatrix, &s_orbatrix1};
statetype s_orbatrixbounce1 = {ORBATRIXSPIN4SPR, ORBATRIXSPIN1SPR, stepthink, false, false,  6,  0, 0, T_Projectile, C_OrbatrixBounce, R_OrbatrixBounce, &s_orbatrixbounce2};
statetype s_orbatrixbounce2 = {ORBATRIXSPIN3SPR, ORBATRIXSPIN2SPR, stepthink, false, false,  6,  0, 0, T_Projectile, C_OrbatrixBounce, R_OrbatrixBounce, &s_orbatrixbounce3};
statetype s_orbatrixbounce3 = {ORBATRIXSPIN2SPR, ORBATRIXSPIN3SPR, stepthink, false, false,  6,  0, 0, T_Projectile, C_OrbatrixBounce, R_OrbatrixBounce, &s_orbatrixbounce4};
statetype s_orbatrixbounce4 = {ORBATRIXSPIN1SPR, ORBATRIXSPIN4SPR, stepthink, false, false,  6,  0, 0, T_Projectile, C_OrbatrixBounce, R_OrbatrixBounce, &s_orbatrixbounce1};

/*
===========================
=
= SpawnOrbatrix
=
===========================
*/

void SpawnOrbatrix(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->obclass = orbatrixobj;
	new->active = ac_yes;
	new->priority = 0;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY) + -24*PIXGLOBAL;
	if (US_RndT() < 0x80)
	{
		new->xdir = 1;
	}
	else
	{
		new->xdir = -1;
	}
	new->ydir = 1;
	new->temp4 = 1;
	NewState(new, &s_orbatrix1);
}

/*
===========================
=
= T_OrbatrixFly
=
===========================
*/

void T_OrbatrixFly(objtype *ob)
{
	Sint16 dist;

	if (US_RndT() < 0x20)
	{
		ob->state = &s_orbatrixidle1;
		return;
	}

	if (ob->bottom != player->bottom)
	{
		return;
	}

	dist = player->x - ob->x;
	ob->xdir = (dist < 0)? -1 : 1;
	if (dist > -5*TILEGLOBAL && dist < 5*TILEGLOBAL)
	{
		ob->state = &s_orbatrixcurl1;
	}
}

/*
===========================
=
= C_Orbatrix
=
===========================
*/

void C_Orbatrix(objtype *ob, objtype *hit)
{
	if (hit->obclass == stunshotobj)
	{
		ExplodeShot(hit);
		ChangeState(ob, &s_orbatrixidle1);
	}
}

/*
===========================
=
= R_Orbatrix
=
===========================
*/

void R_Orbatrix(objtype *ob)
{
	//
	// ugly hack: apply float offset before drawing the sprite
	// (it's ugly because the sprite moves up/down, but the hitbox doesn't)
	//
	ob->y -= ob->temp3;
	R_Walk(ob);
	ob->y += ob->temp3;

	//
	// update the float offset
	//
	ob->temp3 = ob->temp3 + ob->temp4 * tics * 4;
	if (ob->temp3 > 8*PIXGLOBAL)
	{
		ob->temp3 = 8*PIXGLOBAL;
		ob->temp4 = -1;
	}
	else if (ob->temp3 < -8*PIXGLOBAL)
	{
		ob->temp3 = -8*PIXGLOBAL;
		ob->temp4 = 1;
	}
}

/*
===========================
=
= R_OrbatrixBounce
=
===========================
*/

void R_OrbatrixBounce(objtype *ob)
{
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);

	if (ob->hitnorth)
	{
		ob->yspeed = -ob->yspeed;
	}
	if (ob->hitnorth || ob->hitwest || ob->hiteast)
	{
		ob->xspeed = -ob->xspeed;
		SD_PlaySound(SND_ORBATRIXBOUNCE);
		if (ob->hitnorth && --ob->temp1 == 0)
		{
			ChangeState(ob, &s_orbatrixuncurl1);
			ob->temp2 = 24*PIXGLOBAL;
		}
	}
}

/*
===========================
=
= T_OrbatrixCurl
=
===========================
*/

void T_OrbatrixCurl(objtype *ob)
{
	if (ob->temp3 >= 16)
	{
		ob->xspeed = ob->xdir * 60;
		ob->yspeed = -32;
		ob->y -= ob->temp3;
		ob->temp1 = 5;	// bounce 5 times
		ob->state = ob->state->nextstate;
	}
	ob->needtoreact = true;
}

/*
===========================
=
= T_OrbatrixUncurl
=
===========================
*/

void T_OrbatrixUncurl(objtype *ob)
{
	ob->temp2 += (ytry = tics * -8);
	if (ob->temp2 <= 0)
	{
		ytry -= ob->temp2;
		ob->state = ob->state->nextstate;
	}
}

/*
===========================
=
= C_OrbatrixBounce
=
===========================
*/

void C_OrbatrixBounce(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		KillKeen();
	}
	else if (hit->obclass == stunshotobj)
	{
		ExplodeShot(hit);
		ob->xspeed = 0;
	}
}

/*
=============================================================================

						  BIP

=============================================================================
*/

statetype s_bipstand      = {BIPSTANDSPR,    BIPSTANDSPR,    step,  false, true, 30,  0, 0, NULL, C_Bip, R_Walk, &s_bipwalk1};
statetype s_bipwalk1      = {BIPWALKL1SPR,   BIPWALKR1SPR,   step,  true,  true,  4, 32, 0, T_BipWalk, C_Bip, R_Walk, &s_bipwalk2};
statetype s_bipwalk2      = {BIPWALKL2SPR,   BIPWALKR2SPR,   step,  true,  true,  4, 32, 0, T_BipWalk, C_Bip, R_Walk, &s_bipwalk3};
statetype s_bipwalk3      = {BIPWALKL3SPR,   BIPWALKR3SPR,   step,  true,  true,  4, 32, 0, T_BipWalk, C_Bip, R_Walk, &s_bipwalk4};
statetype s_bipwalk4      = {BIPWALKL4SPR,   BIPWALKR4SPR,   step,  true,  true,  4, 32, 0, T_BipWalk, C_Bip, R_Walk, &s_bipwalk1};
statetype s_bipsquished   = {BIPSQUISHEDSPR, BIPSQUISHEDSPR, think, false, true,  0,  0, 0, T_Projectile, NULL, R_Draw, NULL};

/*
===========================
=
= T_BipWalk
=
===========================
*/

void T_BipWalk(objtype *ob)
{
	if (ob->bottom == player->bottom)
	{
		if (ob->right < player->left - 4*PIXGLOBAL)
			ob->xdir = 1;

		if (ob->left > player->right + 4*PIXGLOBAL)
			ob->xdir = -1;
	}
	else if (US_RndT() < 0x10)
	{
		ob->xdir = -ob->xdir;
		ob->state = &s_bipstand;
	}
}

/*
===========================
=
= C_Bip
=
===========================
*/

void C_Bip(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj && hit->ymove > 0)
	{
		SD_PlaySound(SND_BIPSQUISH);
		ob->obclass = inertobj;
		ChangeState(ob, &s_bipsquished);
	}
}

/*
=============================================================================

						  BIPSHIP

=============================================================================
*/

statetype s_bipship         = {BIPSHIPLSPR,        BIPSHIPRSPR,        think,     false, true,      0, 0, 0, T_BipshipFly, C_Bipship, R_Draw, &s_bipship};
statetype s_bipshipshot     = {BIPSHIPSHOTSPR,     BIPSHIPSHOTSPR,     think,     false, false,     0, 0, 0, T_Velocity, C_Lethal, R_BipShot, NULL};
statetype s_bipshipturn1    = {BIPSHIPRTURN1SPR,   BIPSHIPLTURN1SPR,   stepthink, false, true,     10, 0, 0, T_BipshipTurn, C_Bipship, R_Draw, &s_bipshipturn2};
statetype s_bipshipturn2    = {BIPSHIPRTURN2SPR,   BIPSHIPLTURN2SPR,   stepthink, false, true,     10, 0, 0, T_BipshipTurn, C_Bipship, R_Draw, &s_bipshipturn3};
statetype s_bipshipturn3    = {BIPSHIPRTURN3SPR,   BIPSHIPLTURN3SPR,   stepthink, false, true,     10, 0, 0, T_BipshipTurn, C_Bipship, R_Draw, &s_bipshipturn4};
statetype s_bipshipturn4    = {BIPSHIPRTURN4SPR,   BIPSHIPLTURN4SPR,   stepthink, false, true,     10, 0, 0, T_BipshipTurn, C_Bipship, R_Draw, &s_bipship};
statetype s_bipshipexplode1 = {BIPSHIPEXPLODE2SPR, BIPSHIPEXPLODE1SPR, think,     false, false,     0, 0, 0, T_Projectile, NULL, R_Land, &s_bipshipexplode2};
statetype s_bipshipexplode2 = {BIPSHIPEXPLODE2SPR, BIPSHIPEXPLODE1SPR, step,      true,  false,     1, 0, 0, T_BipshipExplode, NULL, R_Land, &s_bipshipexplode3};
statetype s_bipshipexplode3 = {BIPSHIPEXPLODE5SPR, BIPSHIPEXPLODE5SPR, step,      true,  false, 30000, 0, 0, NULL, NULL, R_Land, &s_bipshipexplode3};
statetype s_bipshipsmoke1   = {BIPSHIPEXPLODE3SPR, BIPSHIPEXPLODE3SPR, step,      true,  false,    10, 0, 0, NULL, NULL, R_Draw, &s_bipshipsmoke2};
statetype s_bipshipsmoke2   = {BIPSHIPEXPLODE4SPR, BIPSHIPEXPLODE4SPR, step,      true,  false,    10, 0, 0, NULL, NULL, R_Draw, NULL};

/*
===========================
=
= SpawnBipship
=
===========================
*/

void SpawnBipship(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->obclass = bipshipobj;
	new->active = ac_yes;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY)+ -24*PIXGLOBAL;
	if (US_RndT() < 0x80)
	{
		new->xdir = 1;
	}
	else
	{
		new->xdir = -1;
	}
	new->xspeed = new->xdir * 20;
	NewState(new, &s_bipship);
}

/*
===========================
=
= R_BipShot
=
===========================
*/

void R_BipShot(objtype *ob)
{
	if (ob->hitnorth || ob->hitsouth || ob->hiteast || ob->hitwest)
	{
		RemoveObj(ob);
	}
	else
	{
		RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
	}
}

/*
===========================
=
= T_BipshipTurn
=
===========================
*/

void T_BipshipTurn(objtype *ob)
{
	AccelerateX(ob, ob->xdir, 20);
}

/*
===========================
=
= T_BipshipFly
=
===========================
*/

void T_BipshipFly(objtype *ob)
{
	Uint16 far *map;
	Sint16 dir;
	Uint16 tile, tx, ty;

	AccelerateX(ob, ob->xdir, 20);
	dir = ob->xdir;
	if (player->bottom + TILEGLOBAL - ob->bottom <= 2*TILEGLOBAL)
	{
		if (player->x < ob->x)
		{
			dir = -1;
		}
		else
		{
			dir = 1;
		}
		if (ob->xdir == dir && US_RndT() < tics*4)
		{
			SD_PlaySound(SND_KEENFIRE);
			GetNewObj(true);
			new->obclass = mshotobj;
			new->active = ac_removable;
			new->priority = 1;
			if (ob->xdir == 1)
			{
				new->x = ob->x + TILEGLOBAL;
				new->xspeed = 64;
			}
			else
			{
				new->x = ob->x;
				new->xspeed = -64;
			}
			new->y = ob->y + 10*PIXGLOBAL;
			new->yspeed = 16;
			NewState(new, &s_bipshipshot);
		}
	}

	tx = ob->tilemidx + dir*4;
	map = mapsegs[1] + mapbwidthtable[ob->tiletop]/2 + tx;

	for (ty = ob->tiletop; ty <= ob->tilebottom; ty++, map += mapwidth)
	{
		tile = *map;
		if (tinf[tile+EASTWALL] || tinf[tile+WESTWALL])
		{
			dir = -dir;
			goto check_turn;
		}
	}
	tile = *map;
	if (!tinf[tile+NORTHWALL])
	{
		dir = -dir;
	}
check_turn:
	if (dir != ob->xdir)
	{
		ob->xdir = dir;
		ChangeState(ob, &s_bipshipturn1);
	}
}

/*
===========================
=
= T_BipshipExplode
=
===========================
*/

void T_BipshipExplode(objtype *ob)
{
	SD_PlaySound(SND_BIPSHIPEXPLODE);

	GetNewObj(true);
	new->obclass = inertobj;
	new->active = ac_yes;
	new->priority = 2;
	new->x = ob->x;
	new->y = ob->y - 24*PIXGLOBAL;
	NewState(new, &s_bipshipsmoke1);

	GetNewObj(true);
	new->obclass = bipobj;
	new->active = ac_yes;
	new->priority = 0;
	new->x = ob->x;
	new->y = ob->y - 8*PIXGLOBAL;
	if (US_RndT() < 0x80)
	{
		new->xdir = 1;
	}
	else
	{
		new->xdir = -1;
	}
	NewState(new, &s_bipstand);
}

/*
===========================
=
= C_Bipship
=
===========================
*/

void C_Bipship(objtype *ob, objtype *hit)
{
	if (hit->obclass == stunshotobj)
	{
		ExplodeShot(hit);
		ChangeState(ob, &s_bipshipexplode1);
	}
}

/*
=============================================================================

						  FLECT

=============================================================================
*/

statetype s_flectstand = {FLECTSTANDLSPR, FLECTSTANDRSPR, think, false, true, 60,   0, 0, T_FlectStand, C_Flect, R_Flect, &s_flectwalk1};
statetype s_flectturn  = {FLECTSTANDSPR,  FLECTSTANDSPR,  step,  false, true,  8,   0, 0, NULL, C_Flect, R_Flect, &s_flectwalk1};
statetype s_flectwalk1 = {FLECTWALKL1SPR, FLECTWALKR1SPR, step,  false, true, 10, 128, 0, T_FlectWalk, C_Flect, R_Flect, &s_flectwalk2};
statetype s_flectwalk2 = {FLECTWALKL2SPR, FLECTWALKR2SPR, step,  false, true, 10, 128, 0, T_FlectWalk, C_Flect, R_Flect, &s_flectwalk3};
statetype s_flectwalk3 = {FLECTWALKL3SPR, FLECTWALKR3SPR, step,  false, true, 10, 128, 0, T_FlectWalk, C_Flect, R_Flect, &s_flectwalk4};
statetype s_flectwalk4 = {FLECTWALKL4SPR, FLECTWALKR4SPR, step,  false, true, 10, 128, 0, T_FlectWalk, C_Flect, R_Flect, &s_flectwalk1};
statetype s_flectstun  = {FLECTSTUNSPR,   FLECTSTUNSPR,   think, false, false, 0,   0, 0, T_Projectile, NULL, R_Stunned, &s_flectstun};

/*
===========================
=
= SpawnFlect
=
===========================
*/

void SpawnFlect(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->obclass = flectobj;
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
	NewState(new, &s_flectwalk1);
}

/*
===========================
=
= T_FlectStand
=
===========================
*/

void T_FlectStand(objtype *ob)
{
	if (player->x < ob->x)
	{
		if (ob->xdir != -1)
		{
			ob->state = &s_flectturn;
			ob->xdir = -1;
		}
		else
		{
			ob->state = &s_flectwalk1;
		}
	}
	else
	{
		if (ob->xdir != 1)
		{
			ob->state = &s_flectturn;
			ob->xdir = 1;
		}
		else
		{
			ob->state = &s_flectwalk1;
		}
	}
}

/*
===========================
=
= T_FlectWalk
=
===========================
*/

void T_FlectWalk(objtype *ob)
{
	if (player->x < ob->x && ob->xdir == 1)
	{
		if (ob->xdir != -1)	// always true here!
		{
			ob->state = &s_flectturn;
		}
		ob->xdir = -1;
	}

	if (player->x > ob->x && ob->xdir == -1)
	{
		if (ob->xdir != 1)	// always true here!
		{
			ob->state = &s_flectturn;
		}
		ob->xdir = 1;
	}

	if (US_RndT() < 0x20)
	{
		ob->state = &s_flectstand;
	}
}

/*
===========================
=
= C_Flect
=
===========================
*/

void C_Flect(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		ClipToSpriteSide(hit, ob);
	}
	else if (hit->obclass == stunshotobj)
	{
		if (hit->xdir == 0)
		{
			StunObj(ob, hit, &s_flectstun);
		}
		else if (hit->xdir != ob->xdir)
		{
			// reflect shot:
			hit->xdir = ob->xdir;
			hit->temp4 = true;	// shot can now stun Keen
			SD_PlaySound(SND_SHOTBOUNCE);
		}
	}
}

/*
===========================
=
= R_Flect
=
===========================
*/

void R_Flect(objtype *ob)
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
		ChangeState(ob, ob->state);
	}
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}
