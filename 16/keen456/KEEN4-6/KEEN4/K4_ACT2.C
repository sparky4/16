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
K4_ACT2.C
=========

Contains the following actor types (in this order):

- Wormouth
- Thundercloud & Lightning
- Berkeloid & Fireball
- Inchworm & Foot (in-level)
- Bounder
- Lick
- Platform
- Dropping Platform

*/

#include "CK_DEF.H"

/*
=============================================================================

						  WORMOUTH

=============================================================================
*/

statetype s_worm      = {WORMOUTHSPR,       WORMOUTHSPR,       slide, true,  true,  4, 16, 0, T_Worm,          NULL,                  R_Walk,    &s_worm};
statetype s_wormpeek1 = {WORMOUTHPEEKL1SPR, WORMOUTHPEEKL1SPR, step,  false, false, 20,  0, 0, NULL,                   C_Worm,   R_Draw,    &s_wormpeek2};
statetype s_wormpeek2 = {WORMOUTHPEEKL2SPR, WORMOUTHPEEKL2SPR, step,  false, false,  8,  0, 0, NULL,                   C_Worm,   R_Draw,    &s_wormpeek3};
statetype s_wormpeek3 = {WORMOUTHPEEKL1SPR, WORMOUTHPEEKL1SPR, step,  false, false, 20,  0, 0, T_WormLookLeft,  C_Worm,   R_Draw,    &s_wormpeek4};
statetype s_wormpeek4 = {WORMOUTHSPR,       WORMOUTHSPR,       step,  false, false,  8,  0, 0, NULL,                   C_Worm,   R_Draw,    &s_wormpeek5};
statetype s_wormpeek5 = {WORMOUTHPEEKR1SPR, WORMOUTHPEEKR1SPR, step,  false, false, 20,  0, 0, NULL,                   C_Worm,   R_Draw,    &s_wormpeek6};
statetype s_wormpeek6 = {WORMOUTHPEEKR2SPR, WORMOUTHPEEKR2SPR, step,  false, false,  8,  0, 0, NULL,                   C_Worm,   R_Draw,    &s_wormpeek7};
statetype s_wormpeek7 = {WORMOUTHPEEKR1SPR, WORMOUTHPEEKR1SPR, step,  false, false, 20,  0, 0, T_WormLookRight, C_Worm,   R_Draw,    &s_wormpeek8};
statetype s_wormpeek8 = {WORMOUTHSPR,       WORMOUTHSPR,       step,  false, false,  8,  0, 0, T_WormLook,      NULL,                  R_Draw,    &s_worm};
statetype s_wormbite1 = {WORMOUTHBITEL1SPR, WORMOUTHBITER1SPR, step,  false, false,  8,  0, 0, NULL,                   C_WormKill, R_Draw,    &s_wormbite2};
statetype s_wormbite2 = {WORMOUTHBITEL2SPR, WORMOUTHBITER2SPR, step,  false, false,  8,  0, 0, NULL,                   C_WormKill, R_Draw,    &s_wormbite3};
statetype s_wormbite3 = {WORMOUTHBITEL3SPR, WORMOUTHBITER3SPR, step,  false, false, 16,  0, 0, NULL,                   C_WormKill, R_Draw,    &s_wormbite4};
statetype s_wormbite4 = {WORMOUTHBITEL2SPR, WORMOUTHBITER2SPR, step,  false, false,  8,  0, 0, NULL,                   C_WormKill, R_Draw,    &s_wormbite5};
statetype s_wormbite5 = {WORMOUTHBITEL1SPR, WORMOUTHBITER1SPR, step,  false, false,  8,  0, 0, NULL,                   C_WormKill, R_Draw,    &s_worm};
statetype s_wormstun  = {WORMOUTHSTUNSPR,   WORMOUTHSTUNSPR,   think, false, false, 10,  0, 0, T_Projectile,       NULL,                  R_Stunned, NULL};

/*
===========================
=
= SpawnWormMouth
=
===========================
*/

void SpawnWormMouth(Sint16 x, Sint16 y)
{
	GetNewObj(false);
	new->obclass = wormouthobj;
	new->active = ac_yes;
	new->priority = 0;
	new->x = CONVERT_TILE_TO_GLOBAL(x);
	new->y = CONVERT_TILE_TO_GLOBAL(y) + 0x8F;
	if (US_RndT() < 0x80)
	{
		new->xdir = 1;
	}
	else
	{
		new->xdir = -1;
	}
	new->ydir = 1;
	NewState(new, &s_worm);
}

/*
===========================
=
= T_WormLookRight
=
===========================
*/

void T_WormLookRight(objtype *ob)
{
	if (player->x > ob->x)
	{
		ob->xdir = 1;
		ob->state = &s_worm;
	}
}

/*
===========================
=
= T_WormLook
=
===========================
*/

void T_WormLook(objtype *ob)
{
	if (player->x > ob->x)
	{
		ob->xdir = 1;
	}
	else
	{
		ob->xdir = -1;
	}
}

/*
===========================
=
= T_WormLookLeft
=
===========================
*/

void T_WormLookLeft(objtype *ob)
{
	if (player->x < ob->x)
	{
		ob->xdir = -1;
		ob->state = &s_worm;
	}
}

/*
===========================
=
= T_Worm
=
===========================
*/

void T_Worm(objtype *ob)
{
	Sint16 xdist, ydist;

	xdist = player->x - ob->x;
	ydist = player->bottom - ob->bottom;
	if ((xdist < -3*TILEGLOBAL || xdist > 3*TILEGLOBAL) && US_RndT() < 6)
	{
		ob->state = &s_wormpeek1;
	}
	else if (ydist >= -TILEGLOBAL && ydist <= TILEGLOBAL)
	{
		if (ob->xdir == 1 && xdist > 8*PIXGLOBAL && xdist < 24*PIXGLOBAL
			|| ob->xdir == -1 && xdist < -8*PIXGLOBAL && xdist > -32*PIXGLOBAL)
		{
			SD_PlaySound(SND_WORMOUTHATTACK);
			ob->state = &s_wormbite1;
		}
	}
}

/*
===========================
=
= C_Worm
=
===========================
*/

void C_Worm(objtype *ob, objtype *hit)
{
	if (hit->obclass == stunshotobj)
	{
		StunObj(ob, hit, &s_wormstun);
	}
}

/*
===========================
=
= C_WormKill
=
===========================
*/

void C_WormKill(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		if (!(ob->xdir == 1 && ob->x > hit->x || ob->xdir == -1 && ob->x < hit->x))
		{
			KillKeen();
		}
		return;
	}
	else
	{
		C_Worm(ob, hit);
	}
}

/*
=============================================================================

						  THUNDERCLOUD

=============================================================================
*/

statetype s_cloudsleep   = {CLOUDSPR,       CLOUDSPR,       think,     false, false,  20, 0, 0, NULL,         C_CloudSleep, R_Draw,  NULL};
statetype s_cloudwake    = {CLOUDACTIVESPR, CLOUDACTIVESPR, step,      false, false, 100, 0, 0, NULL,         NULL,         R_Cloud, &s_cloud};
statetype s_cloud        = {CLOUDACTIVESPR, CLOUDACTIVESPR, think,     false, false,  20, 0, 0, T_Cloud,      NULL,         R_Cloud, NULL};
statetype s_cloudalign   = {CLOUDACTIVESPR, CLOUDACTIVESPR, think,     false, false,  20, 0, 0, T_CloudAlign, NULL,         R_Cloud, NULL};
statetype s_cloudcharge  = {CLOUDACTIVESPR, CLOUDACTIVESPR, stepthink, false, false,  60, 0, 0, T_Velocity,   NULL,         R_Cloud, &s_cloud};
statetype s_cloudattack1 = {CLOUDCHARGESPR, CLOUDCHARGESPR, step,      false, false,  10, 0, 0, NULL,         NULL,         R_Draw,  &s_cloudattack2};
statetype s_cloudattack2 = {CLOUDACTIVESPR, CLOUDACTIVESPR, step,      false, false,  10, 0, 0, NULL,         NULL,         R_Draw,  &s_cloudattack3};
statetype s_cloudattack3 = {CLOUDCHARGESPR, CLOUDCHARGESPR, step,      false, false,  10, 0, 0, NULL,         NULL,         R_Draw,  &s_cloudattack4};
statetype s_cloudattack4 = {CLOUDACTIVESPR, CLOUDACTIVESPR, step,      false, false,  10, 0, 0, T_CloudShoot, NULL,         R_Draw,  &s_cloudattack5};
statetype s_cloudattack5 = {CLOUDCHARGESPR, CLOUDCHARGESPR, step,      false, false,  10, 0, 0, NULL,         NULL,         R_Draw,  &s_cloudattack6};
statetype s_cloudattack6 = {CLOUDACTIVESPR, CLOUDACTIVESPR, step,      false, false,  10, 0, 0, NULL,         NULL,         R_Draw,  &s_cloudattack7};
statetype s_cloudattack7 = {CLOUDCHARGESPR, CLOUDCHARGESPR, step,      false, false,  10, 0, 0, NULL,         NULL,         R_Draw,  &s_cloudattack8};
statetype s_cloudattack8 = {CLOUDACTIVESPR, CLOUDACTIVESPR, step,      false, false,  10, 0, 0, NULL,         NULL,         R_Draw,  &s_cloudattack9};
statetype s_cloudattack9 = {CLOUDCHARGESPR, CLOUDCHARGESPR, step,      false, false,  48, 0, 0, NULL,         NULL,         R_Draw,  &s_cloudcharge};

statetype s_bolt1 = {BOLT1SPR, BOLT1SPR, step, false, false, 8, 0, 0, NULL, C_Lethal, R_Draw, &s_bolt2};
statetype s_bolt2 = {BOLT2SPR, BOLT2SPR, step, false, false, 8, 0, 0, NULL, C_Lethal, R_Draw, &s_bolt3};
statetype s_bolt3 = {BOLT1SPR, BOLT1SPR, step, false, false, 8, 0, 0, NULL, C_Lethal, R_Draw, &s_bolt4};
statetype s_bolt4 = {BOLT2SPR, BOLT2SPR, step, false, false, 8, 0, 0, NULL, C_Lethal, R_Draw, &s_bolt5};
statetype s_bolt5 = {BOLT1SPR, BOLT1SPR, step, false, false, 8, 0, 0, NULL, C_Lethal, R_Draw, &s_bolt6};
statetype s_bolt6 = {BOLT2SPR, BOLT2SPR, step, false, false, 8, 0, 0, NULL, C_Lethal, R_Draw, NULL};

/*
===========================
=
= SpawnCloudster
=
===========================
*/

void SpawnCloudster(Sint16 x, Sint16 y)
{
	GetNewObj(false);
	new->obclass = thundercloudobj;
	new->active = ac_yes;
	new->priority = 2;
	new->x = CONVERT_TILE_TO_GLOBAL(x);
	new->y = CONVERT_TILE_TO_GLOBAL(y);
	new->ydir = new->xdir = 1;
	NewState(new, &s_cloudsleep);
}

/*
===========================
=
= T_Cloud
=
===========================
*/

void T_Cloud(objtype *ob)
{
	if (US_RndT() < tics)
		ob->xdir = ob->x > player->x? -1 : 1;

	AccelerateX(ob, ob->xdir, 10);
	if (player->bottom < ob->bottom || (Sint16)(player->top - ob->bottom) > 64*PIXGLOBAL)
		return;
	if (ob->left < player->right && ob->right > player->left)
	{
		ob->state = &s_cloudalign;
	}
}

/*
===========================
=
= T_CloudAlign
=
===========================
*/

void T_CloudAlign(objtype *ob)
{
	AccelerateX(ob, ob->xdir, 10);
	if (xtry < 0 && (Sint16)((ob->x & 0x7F) + xtry) <= 0)
	{
		xtry = -(ob->x & 0x7F);
		ob->state = &s_cloudattack1;
	}
	if (xtry > 0 && (ob->x & 0x7F) + xtry >= 8*PIXGLOBAL)
	{
		xtry = 8*PIXGLOBAL - (ob->x & 0x7F);
		ob->state = &s_cloudattack1;
	}
}

/*
===========================
=
= R_Cloud
=
===========================
*/

void R_Cloud(objtype *ob)
{
	if (ob->hitwest)
	{
		ob->xspeed = 0;
		ob->xdir = -1;
	}
	else if (ob->hiteast)
	{
		ob->xspeed = 0;
		ob->xdir = 1;
	}
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}

/*
===========================
=
= T_CloudShoot
=
===========================
*/

void T_CloudShoot(objtype *ob)
{
	GetNewObj(true);
	new->obclass = lightningobj;
	new->active = ac_removable;
	new->needtoclip = cl_noclip;
	new->x = ob->x + TILEGLOBAL;
	new->y = ob->y + TILEGLOBAL;
	NewState(new, &s_bolt1);
	SD_PlaySound(SND_THUNDER);
}

/*
===========================
=
= C_CloudSleep
=
===========================
*/

void C_CloudSleep(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		ChangeState(ob, &s_cloudwake);
	}
}

/*
=============================================================================

						  BERKELOID

temp1 = float offset, in global units (added to ob->y when drawing the sprite)
temp2 = float speed, in global units per tic (added to temp1 for every tic)

=============================================================================
*/

statetype s_berkefloat1  = {BERKEWALKL1SPR,  BERKEWALKR1SPR,  slide, false, true,  6, 8, 0, BerkeThink, C_Berke, BerkeWalkReact, &s_berkefloat2};
statetype s_berkefloat2  = {BERKEWALKL2SPR,  BERKEWALKR2SPR,  slide, false, true,  6, 8, 0, BerkeThink, C_Berke, BerkeWalkReact, &s_berkefloat3};
statetype s_berkefloat3  = {BERKEWALKL3SPR,  BERKEWALKR3SPR,  slide, false, true,  6, 8, 0, BerkeThink, C_Berke, BerkeWalkReact, &s_berkefloat4};
statetype s_berkefloat4  = {BERKEWALKL4SPR,  BERKEWALKR4SPR,  slide, false, true,  6, 8, 0, BerkeThink, C_Berke, BerkeWalkReact, &s_berkefloat1};
statetype s_berkethrow1  = {BERKETHROWL1SPR, BERKETHROWR1SPR, step,  false, false, 6, 0, 0, SetReactThink, C_Berke, BerkeDrawReact, &s_berkethrow2};
statetype s_berkethrow2  = {BERKETHROWL1SPR, BERKETHROWR1SPR, step,  false, false, 6, 0, 0, SetReactThink, C_Berke, BerkeDrawReact, &s_berkethrow3};
statetype s_berkethrow3  = {BERKETHROWL1SPR, BERKETHROWR1SPR, step,  false, false, 6, 0, 0, SetReactThink, C_Berke, BerkeDrawReact, &s_berkethrow4};
statetype s_berkethrow4  = {BERKETHROWL1SPR, BERKETHROWR1SPR, step,  false, false, 6, 0, 0, SetReactThink, C_Berke, BerkeDrawReact, &s_berkethrow5};
statetype s_berkethrow5  = {BERKETHROWL1SPR, BERKETHROWR1SPR, step,  false, false, 6, 0, 0, SetReactThink, C_Berke, BerkeDrawReact, &s_berkethrow6};
statetype s_berkethrow6  = {BERKETHROWL1SPR, BERKETHROWR1SPR, step,  false, false, 6, 0, 0, BerkeThrowThink, C_Berke, BerkeDrawReact, &s_berkethrow7};
statetype s_berkethrow7  = {BERKETHROWL2SPR, BERKETHROWR2SPR, step,  false, false, 6, 0, 0, SetReactThink, C_Berke, BerkeDrawReact, &s_berkethrow8};
statetype s_berkethrow8  = {BERKETHROWL2SPR, BERKETHROWR2SPR, step,  false, false, 6, 0, 0, SetReactThink, C_Berke, BerkeDrawReact, &s_berkethrow9};
statetype s_berkethrow9  = {BERKETHROWL2SPR, BERKETHROWR2SPR, step,  false, false, 6, 0, 0, SetReactThink, C_Berke, BerkeDrawReact, &s_berkethrow10};
statetype s_berkethrow10 = {BERKETHROWL2SPR, BERKETHROWR2SPR, step,  false, false, 6, 0, 0, SetReactThink, C_Berke, BerkeDrawReact, &s_berkethrow11};
statetype s_berkethrow11 = {BERKETHROWL2SPR, BERKETHROWR2SPR, step,  false, false, 6, 0, 0, SetReactThink, C_Berke, BerkeDrawReact, &s_berkethrow12};
statetype s_berkethrow12 = {BERKETHROWL2SPR, BERKETHROWR2SPR, step,  false, false, 6, 0, 0, BerkeThrowDone, C_Berke, BerkeDrawReact, &s_berkefloat1};

statetype s_fire1     = {FIREBALL1SPR, FIREBALL1SPR, stepthink, false, false, 6, 0, 0, T_WeakProjectile, C_Lethal, FireReact, &s_fire2};
statetype s_fire2     = {FIREBALL2SPR, FIREBALL2SPR, stepthink, false, false, 6, 0, 0, T_WeakProjectile, C_Lethal, FireReact, &s_fire1};
statetype s_fireland1 = {FIREBALL1SPR, FIREBALL1SPR, step, false, false,  6, 0, 0, NULL, C_Berke, R_Draw, &s_fireland2};
statetype s_fireland2 = {FIREBALL3SPR, FIREBALL3SPR, step, false, false, 12, 0, 0, NULL, C_Berke, R_Draw, &s_fireland3};
statetype s_fireland3 = {FIREBALL4SPR, FIREBALL4SPR, step, false, false, 12, 0, 0, NULL, C_Berke, R_Draw, &s_fireland4};
statetype s_fireland4 = {FIREBALL3SPR, FIREBALL3SPR, step, false, false, 12, 0, 0, NULL, C_Berke, R_Draw, &s_fireland5};
statetype s_fireland5 = {FIREBALL4SPR, FIREBALL4SPR, step, false, false, 12, 0, 0, NULL, C_Berke, R_Draw, &s_fireland6};
statetype s_fireland6 = {FIREBALL1SPR, FIREBALL1SPR, step, false, false, 12, 0, 0, NULL, C_Berke, R_Draw, &s_fireland7};
statetype s_fireland7 = {FIREBALL2SPR, FIREBALL2SPR, step, false, false, 12, 0, 0, NULL, C_Berke, R_Draw, &s_fireland8};
statetype s_fireland8 = {FIREBALL1SPR, FIREBALL1SPR, step, false, false, 12, 0, 0, NULL, C_Berke, R_Draw, &s_fireland9};
statetype s_fireland9 = {FIREBALL2SPR, FIREBALL2SPR, step, false, false, 12, 0, 0, NULL, C_Berke, R_Draw, NULL};

/*
===========================
=
= SpawnBerkeloid
=
===========================
*/

void SpawnBerkeloid(Sint16 x, Sint16 y)
{
	GetNewObj(false);
	new->obclass = berkeloidobj;
	new->active = ac_yes;
	new->priority = 2;
	new->x = CONVERT_TILE_TO_GLOBAL(x);
	new->y = CONVERT_TILE_TO_GLOBAL(y) + -2*TILEGLOBAL;
	if (US_RndT() < 0x80)
	{
		new->xdir = 1;
	}
	else
	{
		new->xdir = -1;
	}
	new->ydir = 1;
	new->temp2 = 8;
	NewState(new, &s_berkefloat1);
}


/*
===========================
=
= BerkeThink
=
===========================
*/

void BerkeThink(objtype *ob)
{
	Sint16 xdist, ydist;

	if (US_RndT() < 0x20)
		ob->xdir = player->x < ob->x? -1 : 1;

	if (US_RndT() < 8)
	{
		// BUG: might be about to move off a ledge, causing it to get stuck
		// after throwing (the throw states don't use BerkeWalkReact)!
		// To avoid that, set xtry to 0 here.

		ob->state = &s_berkethrow1;
		// BUG? this doesn't play the attack sound
	}
	else if (US_RndT() <= 0x40)
	{
		xdist = player->x - ob->x;
		ydist = player->y - ob->y;
		if (ydist >= -TILEGLOBAL && ydist <= TILEGLOBAL
			&& (ob->xdir == 1 && xdist > 0 || ob->xdir == -1 && xdist < 0))
		{
			// BUG: might be about to move off a ledge, causing it to get stuck
			// after throwing (the throw states don't use BerkeWalkReact)!
			// To avoid that, set xtry to 0 here.

			ob->state = &s_berkethrow1;
			SD_PlaySound(SND_BERKELOIDATTACK);
		}
	}
}

/*
===========================
=
= BerkeThrowThink
=
===========================
*/

void BerkeThrowThink(objtype *ob)
{
	GetNewObj(true);
	new->active = ac_removable;
	new->obclass = berkeloidobj;
	new->y = ob->y + 8*PIXGLOBAL;
	new->yspeed = -8;
	if (ob->xdir == 1)
	{
		new->xspeed = 48;
		new->x = ob->x + 32*PIXGLOBAL;
		new->xdir = 1;
	}
	else
	{
		new->xspeed = -48;
		new->x = ob->x - 16*PIXGLOBAL;
		new->xdir = -1;
	}
	NewState(new, &s_fire1);
	ob->needtoreact = true;
}

/*
===========================
=
= BerkeThrowDone
=
===========================
*/

void BerkeThrowDone(objtype *ob)
{
	ob->nothink = 4;
	ob->needtoreact = true;
}

/*
===========================
=
= C_Berke
=
===========================
*/

void C_Berke(objtype *ob, objtype *hit)
{
	ob++;			// shut up compiler
	if (hit->obclass == stunshotobj)
	{
		ExplodeShot(hit);
	}
	else if (hit->obclass == keenobj)
	{
		KillKeen();
	}
}

/*
===========================
=
= FireReact
=
===========================
*/

void FireReact(objtype *ob)
{
	if (ob->hiteast || ob->hitwest)
		ob->xspeed = 0;

	if (ob->hitnorth)
	{
		SD_PlaySound(SND_FIREBALLLAND);
		ChangeState(ob, &s_fireland1);
	}
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}

/*
===========================
=
= BerkeDrawReact
=
===========================
*/

void BerkeDrawReact(objtype *ob)
{
	//float up & down:
	ob->temp1 = ob->temp1 + ob->temp2 * tics;
	if (ob->temp1 > 0)
	{
		ob->temp1 = 0;
		ob->temp2 = -8;
	}
	else if (ob->temp1 < -TILEGLOBAL)
	{
		ob->temp1 = -TILEGLOBAL;
		ob->temp2 = 8;
	}

	RF_PlaceSprite(&ob->sprite, ob->x, ob->y+ob->temp1, ob->shapenum, spritedraw, 0);
}

/*
===========================
=
= BerkeWalkReact
=
===========================
*/

void BerkeWalkReact(objtype *ob)
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
		ob->x -= ob->xmove*2;
		ob->xdir = -ob->xdir;
		ob->nothink = US_RndT() >> 5;
		ChangeState(ob, ob->state);
	}
	BerkeDrawReact(ob);
}

/*
=============================================================================

						  INCHWORM & FOOT

temp1 = last lasttimecount (for resetting the touch counter after each frame)
temp2 = touch counter

=============================================================================
*/

statetype s_footsmoke1 = {SMOKE1SPR, SMOKE1SPR, step, false, false, 12, 0, 0, NULL, NULL, R_Draw, &s_footsmoke2};
statetype s_footsmoke2 = {SMOKE2SPR, SMOKE2SPR, step, false, false, 12, 0, 0, NULL, NULL, R_Draw, &s_footsmoke3};
statetype s_footsmoke3 = {SMOKE3SPR, SMOKE3SPR, step, false, false, 12, 0, 0, NULL, NULL, R_Draw, &s_footsmoke4};
statetype s_footsmoke4 = {SMOKE4SPR, SMOKE4SPR, step, false, false, 12, 0, 0, NULL, NULL, R_Draw, NULL};
statetype s_inch1      = {INCHWORML1SPR, INCHWORMR1SPR, step, false, true, 30, 128, 0, InchThink, InchContact, R_Walk, &s_inch2};
statetype s_inch2      = {INCHWORML2SPR, INCHWORMR2SPR, step, false, true, 30, 128, 0, InchThink, InchContact, R_Walk, &s_inch1};
statetype s_footchange = { -1,  -1, step, false, false, 48, 0, 0, NULL, NULL, R_Draw, &s_footwait};	//never used!
statetype s_footwait   = {FOOTSPR, FOOTSPR, think, false, false, 30000, 0, 0, NULL, FootContact, R_Draw, NULL};

/*
===========================
=
= SpawnInchworm
=
===========================
*/

void SpawnInchworm(Sint16 x, Sint16 y)
{
	GetNewObj(false);
	new->obclass = inchwormobj;
	new->active = ac_yes;
	new->priority = 2;
	new->x = CONVERT_TILE_TO_GLOBAL(x);
	new->y = CONVERT_TILE_TO_GLOBAL(y);
	if (US_RndT() < 0x80)
	{
		new->xdir = 1;
	}
	else
	{
		new->xdir = -1;
	}
	new->ydir = 1;
	NewState(new, &s_inch1);
	new->ticcount = US_RndT() / 32;
}

/*
===========================
=
= SpawnFoot
=
===========================
*/

void SpawnFoot(Sint16 x, Sint16 y)
{
	GetNewObj(false);
	new->obclass = footobj;
	new->active = ac_yes;
	new->priority = 0;
	new->x = CONVERT_TILE_TO_GLOBAL(x);
	new->y = CONVERT_TILE_TO_GLOBAL(y-3);
	NewState(new, &s_footwait);
}

/*
===========================
=
= InchThink
=
===========================
*/

void InchThink(objtype *ob)
{
	if (ob->x > player->x)
	{
		ob->xdir = -1;
	}
	else
	{
		ob->xdir = 1;
	}
}

/*
===========================
=
= InchContact
=
===========================
*/

void InchContact(objtype *ob, objtype *hit)
{
	objtype *ob2;

	if (hit->obclass != inchwormobj)
		return;

	if (ob->temp1 != (Sint16)lasttimecount)
	{
		ob->temp1 = (Sint16)lasttimecount;
		ob->temp2 = 0;
	}

	if (++ob->temp2 != 11)	//11 instead of 12 since the object can't contact itself
		return;

	//change current inchworm into a foot:
	SD_PlaySound(SND_MAKEFOOT);
	ob->y -= 5*TILEGLOBAL;
	ob->obclass = footobj;
	ChangeState(ob, &s_footwait);

	//Note: It would make more sense to remove the remaining inchworm BEFORE
	//spawning the smoke, just in case there are not enough free spots in the
	//objarray to spawn the smoke. The game won't crash either way, though.

	//spawn smoke:
	GetNewObj(true);
	new->x = ob->x -  8*PIXGLOBAL;
	new->y = ob->y + 16*PIXGLOBAL;
	new->priority = 3;
	NewState(new, &s_footsmoke1);

	GetNewObj(true);
	new->x = ob->x + 16*PIXGLOBAL;
	new->y = ob->y + 24*PIXGLOBAL;
	new->priority = 3;
	NewState(new, &s_footsmoke1);

	GetNewObj(true);
	new->x = ob->x + 40*PIXGLOBAL;
	new->y = ob->y + 16*PIXGLOBAL;
	new->priority = 3;
	NewState(new, &s_footsmoke1);

	GetNewObj(true);
	new->x = ob->x;
	new->y = ob->y - 8*PIXGLOBAL;
	new->priority = 3;
	NewState(new, &s_footsmoke1);

	//remove ALL inchworm from the level:
	for (ob2 = player->next; ob2; ob2=ob2->next)
	{
		if (ob2->obclass == inchwormobj)
			RemoveObj(ob2);
	}
}

/*
===========================
=
= FootContact
=
===========================
*/

void FootContact(objtype *ob, objtype *hit)	//completely useless
{
	ob++;			// shut up compiler
	hit++;			// shut up compiler
}

/*
=============================================================================

						  BOUNDER

temp1 = if non-zero, pick a new (random) direction when hitting the ground
        Makes the Bounder jump straight up at least once after having jumped
		  left/right (unless Keen is riding the Bounder)
temp2 = jump counter to make the Bounder jump straight up at least twice
        after Keen has fallen/jumped off

=============================================================================
*/

statetype s_bounderup1   = {BOUNDERC1SPR,   BOUNDERC1SPR,   stepthink, false, false, 20, 0, 0, T_Projectile, C_Bounder, R_Bounder, &s_bounderup2};
statetype s_bounderup2   = {BOUNDERC2SPR,   BOUNDERC2SPR,   stepthink, false, false, 20, 0, 0, T_Projectile, C_Bounder, R_Bounder, &s_bounderup1};
statetype s_bounderside1 = {BOUNDERL1SPR,   BOUNDERR1SPR,   stepthink, false, false, 20, 0, 0, T_Projectile, C_Bounder, R_Bounder, &s_bounderside2};
statetype s_bounderside2 = {BOUNDERL2SPR,   BOUNDERR2SPR,   stepthink, false, false, 20, 0, 0, T_Projectile, C_Bounder, R_Bounder, &s_bounderside1};
statetype s_bounderstun  = {BOUNDERC1SPR,   BOUNDERC1SPR,   think,     false, false,  0, 0, 0, T_Projectile, NULL, R_Stunned, &s_bounderstun2};
statetype s_bounderstun2 = {BOUNDERSTUNSPR, BOUNDERSTUNSPR, think,     false, false,  0, 0, 0, T_Projectile, NULL, R_Stunned, NULL};

/*
===========================
=
= SpawnBounder
=
===========================
*/

void SpawnBounder(Sint16 x, Sint16 y)
{
	GetNewObj(false);
	new->obclass = bounderobj;
	new->active = ac_yes;
	new->priority = 0;
	new->x = CONVERT_TILE_TO_GLOBAL(x);
	new->y = CONVERT_TILE_TO_GLOBAL(y) + -8*PIXGLOBAL;
	new->ydir = 1;
	new->xdir = 0;
	NewState(new, &s_bounderup1);
}

/*
===========================
=
= C_Bounder
=
===========================
*/

void C_Bounder(objtype *ob, objtype *hit)
{
	if (hit->obclass == stunshotobj)
	{
		//basically StunObj(), but in different order:
		ob->temp1 = 0;
		ob->temp2 = 0;
		ob->temp3 = 0;
		ob->temp4 = ob->obclass;
		ExplodeShot(hit);
		ChangeState(ob, &s_bounderstun);
		ob->obclass = stunnedobj;

		ob->yspeed -= 32;
	}
}

/*
===========================
=
= R_Bounder
=
===========================
*/

void R_Bounder(objtype *ob)
{
	Sint16 randnum;

	if (ob->hitsouth)
		ob->yspeed = 0;

	if (ob->hitnorth)
	{
		ob->temp2++;
		if (OnScreen(ob))
			SD_PlaySound(SND_BOUNCE2);

		ob->yspeed = -50;
		if (gamestate.riding == ob)
		{
			ob->temp2 = 0;
			if (player->left < ob->left-4*PIXGLOBAL)
			{
				ob->xdir = -1;
			}
			else if (player->right > ob->right+4*PIXGLOBAL)
			{
				ob->xdir = 1;
			}
			else
			{
				ob->xdir = 0;
			}
			ob->xspeed = ob->xdir * 24;
		}
		else if (ob->temp2 <= 2 || ob->temp1 == 0)
		{
			ob->temp1 = 1;
			ob->xdir = ob->xspeed = 0;
			ChangeState(ob, &s_bounderup1);
		}
		else
		{
			ob->temp1 = 0;
			randnum = US_RndT();
			if (randnum < 100)
			{
				ob->xdir = -1;
			}
			else if (randnum < 200)
			{
				ob->xdir = 1;
			}
			else
			{
				ob->xdir = 0;
			}
			ob->xspeed = ob->xdir * 24;
		}

		if (ob->xdir)
		{
			ChangeState(ob, &s_bounderside1);
		}
		else
		{
			ChangeState(ob, &s_bounderup1);
		}
	}

	if (ob->hiteast || ob->hitwest)
	{
		ob->xdir = -ob->xdir;
		ob->xspeed = -ob->xspeed;
	}

	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}

/*
=============================================================================

						  LICK

=============================================================================
*/

statetype s_lick1     = {LICKMOVEL1SPR,   LICKMOVER1SPR,   step,  false, false, 10, 0, 0, LickJumpThink, LickContact, R_Draw, &s_lick2};
statetype s_lick2     = {LICKMOVEL2SPR,   LICKMOVER2SPR,   think, false, false,  0, 0, 0, T_Projectile, LickContact, LickAirReact, &s_lick3};
statetype s_lick3     = {LICKMOVEL3SPR,   LICKMOVER3SPR,   think, false, false,  0, 0, 0, T_Projectile, LickContact, LickAirReact, NULL};
statetype s_lick4     = {LICKMOVEL4SPR,   LICKMOVER4SPR,   step,  false, false, 10, 0, 0, NULL, LickContact, R_Draw, &s_lick1};
statetype s_licklick1 = {LICKATTACKL1SPR, LICKATTACKR1SPR, step,  true,  false,  4, 0, 0, NULL, LickKillContact, R_Draw, &s_licklick2};
statetype s_licklick2 = {LICKATTACKL2SPR, LICKATTACKR2SPR, step,  true,  false,  4, 0, 0, NULL, LickKillContact, R_Draw, &s_licklick3};
statetype s_licklick3 = {LICKATTACKL3SPR, LICKATTACKR3SPR, step,  true,  false,  4, 0, 0, NULL, LickKillContact, R_Draw, &s_licklick4};
statetype s_licklick4 = {LICKATTACKL2SPR, LICKATTACKR2SPR, step,  true,  false,  4, 0, 0, NULL, LickKillContact, R_Draw, &s_licklick5};
statetype s_licklick5 = {LICKATTACKL1SPR, LICKATTACKR1SPR, step,  true,  false,  4, 0, 0, NULL, LickKillContact, R_Draw, &s_licklick6};
statetype s_licklick6 = {LICKATTACKL2SPR, LICKATTACKR2SPR, step,  true,  false,  4, 0, 0, NULL, LickKillContact, R_Draw, &s_licklick7};
statetype s_licklick7 = {LICKATTACKL3SPR, LICKATTACKR3SPR, step,  true,  false,  4, 0, 0, NULL, LickKillContact, R_Draw, &s_licklick8};
statetype s_licklick8 = {LICKATTACKL2SPR, LICKATTACKR2SPR, step,  true,  false,  4, 0, 0, NULL, LickKillContact, R_Draw, &s_lick3};
statetype s_lickstun  = {LICKSTUNSPR,     LICKSTUNSPR,     think, false, false,  0, 0, 0, T_Projectile, NULL, R_Stunned, &s_lickstun2};
statetype s_lickstun2 = {LICKSTUNSPR,     LICKSTUNSPR,     think, false, false,  0, 0, 0, T_Projectile, NULL, R_Stunned, NULL};

/*
===========================
=
= SpawnLick
=
===========================
*/

void SpawnLick(Sint16 x, Sint16 y)
{
	GetNewObj(false);
	new->obclass = lickobj;
	new->active = ac_yes;
	new->priority = 2;
	new->x = CONVERT_TILE_TO_GLOBAL(x);
	new->y = CONVERT_TILE_TO_GLOBAL(y);
	if (US_RndT() < 0x80)
	{
		new->xdir = 1;
	}
	else
	{
		new->xdir = -1;
	}
	new->ydir = 1;
	new->nothink = US_RndT() / 64;
	NewState(new, &s_lick3);
}

/*
===========================
=
= LickJumpThink
=
===========================
*/

void LickJumpThink(objtype *ob)
{
	Sint16 xdist, ydist;

	if (ob->x > player->x)
	{
		ob->xdir = -1;
	}
	else
	{
		ob->xdir = 1;
	}

	xdist = player->x - ob->x;
	ydist = player->y - ob->y;

	if (ydist >= -TILEGLOBAL && ydist <= TILEGLOBAL	&& 
		( ob->xdir == 1 && xdist > -2*PIXGLOBAL && xdist < 24*PIXGLOBAL
		 || ob->xdir == -1 && xdist < 2*PIXGLOBAL && xdist > -32*PIXGLOBAL ) )
	{
		SD_PlaySound(SND_LICKATTACK);
		ob->state = &s_licklick1;
	}
	else if (abs(xdist) > 3*TILEGLOBAL)
	{
		ob->xspeed = ob->xdir * 32;
		ob->yspeed = -32;
	}
	else
	{
		ob->xspeed = (ob->xdir * 32)/2;
		ob->yspeed = -16;
	}
}

/*
===========================
=
= LickContact
=
===========================
*/

void LickContact(objtype *ob, objtype *hit)
{
	if (hit->obclass == stunshotobj)
	{
		StunObj(ob, hit, &s_lickstun);
		ob->yspeed -= 16;
	}
}

/*
===========================
=
= LickKillContact
=
===========================
*/

void LickKillContact(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		if (ob->xdir == 1 && player->x > ob->x 
			|| ob->xdir == -1 && player->x < ob->x)
		{
			KillKeen();
		}
	}
	else
	{
		LickContact(ob, hit);
	}
}

/*
===========================
=
= LickAirReact
=
===========================
*/

void LickAirReact(objtype *ob)
{
	if (ob->hitnorth)
		ChangeState(ob, &s_lick4);

	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}

/*
=============================================================================

						  PLATFORM

temp2 = additional sprite pointer for thruster sprites
temp3 = additional sprite pointer for thruster sprites

=============================================================================
*/

statetype s_platform = {PLATFORMSPR, PLATFORMSPR, think, false, false, 0, 0, 0, T_Platform, NULL, R_Platform, NULL};

/*
===========================
=
= SpawnPlatform
=
===========================
*/

void SpawnPlatform(Sint16 x, Sint16 y, Sint16 dir)
{
	GetNewObj(false);
	new->obclass = platformobj;
	new->active = ac_allways;
	new->priority = 0;
	new->x = CONVERT_TILE_TO_GLOBAL(x);
	new->y = CONVERT_TILE_TO_GLOBAL(y);
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
		break;
	}
	NewState(new, &s_platform);
}

/*
===========================
=
= T_Platform
=
===========================
*/

void T_Platform(objtype *ob)
{
	Uint16 newpos, newtile;

	xtry = ob->xdir * 12 * tics;
	ytry = ob->ydir * 12 * tics;

	if (ob->xdir == 1)
	{
		newpos = ob->right + xtry;
		newtile = CONVERT_GLOBAL_TO_TILE(newpos);
		if (ob->tileright != newtile)
		{
			if (*(mapsegs[2]+mapbwidthtable[ob->tiletop]/2+newtile) == 31)
			{
				ob->xdir = -1;
				xtry = xtry - (newpos & 0xFF);
			}
		}
	}
	else if (ob->xdir == -1)
	{
		newpos = ob->left + xtry;
		newtile = CONVERT_GLOBAL_TO_TILE(newpos);
		if (ob->tileleft != newtile)
		{
			if (*(mapsegs[2]+mapbwidthtable[ob->tiletop]/2+newtile) == 31)
			{
				ob->xdir = 1;
				xtry = xtry + (0x100 - (newpos & 0xFF));
			}
		}
	}
	else if (ob->ydir == 1)
	{
		newpos = ob->bottom + ytry;
		newtile = CONVERT_GLOBAL_TO_TILE(newpos);
		if (ob->tilebottom != newtile)
		{
			if (*(mapsegs[2]+mapbwidthtable[newtile]/2+ob->tileleft) == 31)
			{
				if (*(mapsegs[2]+mapbwidthtable[newtile-2]/2+ob->tileleft) == 31)
				{
					ytry = 0;
					ob->needtoreact = true;
				}
				else
				{
					ob->ydir = -1;
					ytry = ytry - (newpos & 0xFF);
				}
			}
		}
	}
	else if (ob->ydir == -1)
	{
		newpos = ob->top + ytry;
		newtile = CONVERT_GLOBAL_TO_TILE(newpos);
		if (ob->tiletop != newtile)
		{
			if (*(mapsegs[2]+mapbwidthtable[newtile]/2+ob->tileleft) == 31)
			{
				if (*(mapsegs[2]+mapbwidthtable[newtile+2]/2+ob->tileleft) == 31)
				{
					ytry = 0;
					ob->needtoreact = true;
				}
				else
				{
					ob->ydir = 1;
					ytry = ytry + (0x100 - (newpos & 0xFF));
				}
			}
		}
	}
}

/*
===========================
=
= R_Platform
=
===========================
*/

void R_Platform(objtype *ob)
{
	Uint16 frame;

	//place platform sprite:
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
	
	//place (or remove) thruster sprites:
	frame = (lasttimecount >> 2) & 1;
	if (ob->xdir == 1)
	{
		RF_PlaceSprite((void**)&ob->temp2, ob->x-1*PIXGLOBAL, ob->y+3*PIXGLOBAL, frame+PLATSIDETHRUST1SPR, spritedraw, 0);
		if (ob->temp3)
			RF_RemoveSprite((void**)&ob->temp3);
	}
	else if (ob->xdir == -1)
	{
		if (ob->temp2)
			RF_RemoveSprite((void**)&ob->temp2);
		RF_PlaceSprite((void**)&ob->temp3, ob->x+48*PIXGLOBAL, ob->y+5*PIXGLOBAL, frame+PLATSIDETHRUST1SPR, spritedraw, 1);
	}
	else if (ob->ydir == -1)
	{
		RF_PlaceSprite((void**)&ob->temp2, ob->x+2*PIXGLOBAL, ob->y+9*PIXGLOBAL, frame+PLATLTHRUST1SPR, spritedraw, 0);
		RF_PlaceSprite((void**)&ob->temp3, ob->x+46*PIXGLOBAL, ob->y+8*PIXGLOBAL, frame+PLATRTHRUST1SPR, spritedraw, 0);
	}
	else if (ob->ydir == 1)
	{
		if (frame)
		{
			RF_PlaceSprite((void**)&ob->temp2, ob->x+2*PIXGLOBAL, ob->y+9*PIXGLOBAL, frame+PLATLTHRUST1SPR, spritedraw, 0);
			RF_PlaceSprite((void**)&ob->temp3, ob->x+46*PIXGLOBAL, ob->y+8*PIXGLOBAL, frame+PLATRTHRUST1SPR, spritedraw, 0);
		}
		else
		{
			if (ob->temp2)
				RF_RemoveSprite((void**)&ob->temp2);
			if (ob->temp3)
				RF_RemoveSprite((void**)&ob->temp3);
		}
	}
}

/*
=============================================================================

						  DROPPING PLATFORM

temp1 = initial y position

=============================================================================
*/

statetype s_dropplatsit  = {PLATFORMSPR, PLATFORMSPR, think,      false, false, 0, 0,   0, T_DropPlatSit, NULL, R_Draw, NULL};
statetype s_dropplatfall = {PLATFORMSPR, PLATFORMSPR, think,      false, false, 0, 0,   0, T_DropPlatFall, NULL, R_Draw, NULL};
statetype s_dropplatrise = {PLATFORMSPR, PLATFORMSPR, slidethink, false, false, 0, 0, -32, T_DropPlatRise, NULL, R_Draw, NULL};

/*
===========================
=
= SpawnDropPlat
=
===========================
*/

void SpawnDropPlat(Sint16 x, Sint16 y)
{
	GetNewObj(false);
	new->obclass = platformobj;
	new->active = ac_allways;
	new->priority = 0;
	new->x = CONVERT_TILE_TO_GLOBAL(x);
	new->y = new->temp1 = CONVERT_TILE_TO_GLOBAL(y);
	new->xdir = 0;
	new->ydir = 1;
	new->needtoclip = cl_noclip;
	NewState(new, &s_dropplatsit);
}

/*
===========================
=
= T_DropPlatSit
=
===========================
*/

void T_DropPlatSit(objtype *ob)
{
	if (gamestate.riding == ob)
	{
		ytry = tics * 16;
		ob->yspeed = 0;
		if (ob->y + ytry - ob->temp1 >= 8*PIXGLOBAL)
			ob->state = &s_dropplatfall;
	}
}

/*
===========================
=
= T_DropPlatFall
=
===========================
*/

void T_DropPlatFall(objtype *ob)
{
	Uint16 newy, ty;

	DoGravity(ob);

	if (ytry >= 15*PIXGLOBAL)
		ytry = 15*PIXGLOBAL;

	newy = ob->bottom + ytry;
	ty = CONVERT_GLOBAL_TO_TILE(newy);
	if (ob->tilebottom != ty)
	{
		if (*(mapsegs[2]+mapbwidthtable[ty]/2+ob->tileleft) == 31)
		{
			ytry = 0xFF - (ob->bottom & 0xFF);
			if (gamestate.riding != ob)
				ob->state = &s_dropplatrise;
		}
	}
}

/*
===========================
=
= T_DropPlatRise
=
===========================
*/

void T_DropPlatRise(objtype *ob)
{
	if (gamestate.riding == ob)
	{
		ob->yspeed = 0;
		ob->state = &s_dropplatfall;
	}
	else if (ob->y <= ob->temp1)
	{
		ytry = ob->temp1 - ob->y;
		ob->state = &s_dropplatsit;
	}
}
