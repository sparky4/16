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
K6_ACT3.C
=========

Contains the following actor types (in this order):

- Fleex
- Bobba
- Babobba
- Blorb
- Ceilick

*/

#include "CK_DEF.H"

/*
=============================================================================

						  FLEEX

temp1 = flash countdown
temp2 = health

=============================================================================
*/

statetype s_fleexwalk1 = {FLEEXWALKL1SPR, FLEEXWALKR1SPR, step,  false, true,  7, 128, 0, T_FleexWalk, C_Fleex, R_Blooguard, &s_fleexwalk2};
statetype s_fleexwalk2 = {FLEEXWALKL2SPR, FLEEXWALKR2SPR, step,  false, true,  7, 128, 0, T_FleexWalk, C_Fleex, R_Blooguard, &s_fleexwalk1};
statetype s_fleexrun1  = {FLEEXWALKL1SPR, FLEEXWALKR1SPR, step,  false, true,  7, 128, 0, NULL, C_Fleex, R_Blooguard, &s_fleexrun2};
statetype s_fleexrun2  = {FLEEXWALKL2SPR, FLEEXWALKR2SPR, step,  false, true,  7, 128, 0, NULL, C_Fleex, R_Blooguard, &s_fleexrun3};
statetype s_fleexrun3  = {FLEEXWALKL1SPR, FLEEXWALKR1SPR, step,  false, true,  7, 128, 0, NULL, C_Fleex, R_Blooguard, &s_fleexrun4};
statetype s_fleexrun4  = {FLEEXWALKL2SPR, FLEEXWALKR2SPR, step,  false, true,  7, 128, 0, NULL, C_Fleex, R_Blooguard, &s_fleexwalk1};
statetype s_fleexlook1 = {FLEEXLOOK1SPR,  FLEEXLOOK1SPR,  step,  false, true, 60,   0, 0, NULL, C_Fleex, R_Blooguard, &s_fleexlook2};
statetype s_fleexlook2 = {FLEEXLOOK2SPR,  FLEEXLOOK2SPR,  step,  false, true, 60,   0, 0, T_FleexLook, C_Fleex, R_Blooguard, &s_fleexrun1};
statetype s_fleexstun  = {FLEEXSTUNSPR,   FLEEXSTUNSPR,   think, false, false, 0,   0, 0, T_Projectile, NULL, R_Stunned, NULL};

/*
===========================
=
= SpawnFleex
=
===========================
*/

void SpawnFleex(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->obclass = fleexobj;
	new->active = ac_yes;
	new->priority = 0;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY) + -40*PIXGLOBAL;
	if (US_RndT() < 0x80)
	{
		new->xdir = 1;
	}
	else
	{
		new->xdir = -1;
	}
	new->ydir = 1;
	NewState(new, &s_fleexwalk1);
	new->temp2 = 4;	// health
}

/*
===========================
=
= T_FleexWalk
=
===========================
*/

void T_FleexWalk(objtype *ob)
{
	if (!player->xmove && !ob->temp1)
	{
		ob->state = &s_fleexlook1;
	}
	else
	{
		ob->xdir = (ob->x < player->x)? 1 : -1;
	}
}

/*
===========================
=
= T_FleexLook
=
===========================
*/

void T_FleexLook(objtype *ob)
{
	ob->xdir = (ob->x < player->x)? 1 : -1;
}

/*
===========================
=
= C_Fleex
=
===========================
*/

void C_Fleex(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		KillKeen();
	}
	if (hit->obclass == stunshotobj)	// this is not 'else if' in the original code
	{
		if (--ob->temp2 == 0)
		{
			StunObj(ob, hit, &s_fleexstun);
			ob->yspeed = -20;
		}
		else
		{
			ob->temp1 = 2;	// draw white twice
			ob->needtoreact = true;
			ExplodeShot(hit);
			if (ob->state == &s_fleexlook1 || ob->state == &s_fleexlook2)
			{
				ob->xdir = (ob->x < player->x)? 1 : -1;
				ChangeState(ob, &s_fleexwalk1);
			}
		}
	}
}

/*
=============================================================================

						  BOBBA

temp1 = jump counter

=============================================================================
*/

statetype s_bobbajump1       = {BOBBAL2SPR,    BOBBAR2SPR,    stepthink, false, false,  8,  0, 0, T_Projectile, C_Bobba, R_Bobba, &s_bobbajump2};
statetype s_bobbajump2       = {BOBBAL3SPR,    BOBBAR3SPR,    think,     false, false,  8,  0, 0, T_Projectile, C_Bobba, R_Bobba, &s_bobbajump2};
statetype s_bobbastand       = {BOBBAL1SPR,    BOBBAR1SPR,    step,      false, false, 20,  0, 0, T_BobbaStand, C_Bobba, R_Draw, &s_bobbajump1};
statetype s_bobbaattack      = {BOBBAL1SPR,    BOBBAR1SPR,    step,      false, false, 40,  0, 0, NULL, C_Bobba, R_Draw, &s_bobbajump1};
statetype s_bobbashot1       = {BOBBASHOT1SPR, BOBBASHOT1SPR, step,      false, false,  8,  0, 0, NULL, NULL, R_Draw, &s_bobbashot2};
statetype s_bobbashot2       = {BOBBASHOT2SPR, BOBBASHOT2SPR, step,      false, false,  8,  0, 0, NULL, NULL, R_Draw, &s_bobbashot3};
statetype s_bobbashot3       = {BOBBASHOT1SPR, BOBBASHOT1SPR, step,      false, false,  8,  0, 0, NULL, NULL, R_Draw, &s_bobbashot4};
statetype s_bobbashot4       = {BOBBASHOT2SPR, BOBBASHOT2SPR, step,      false, false,  8,  0, 0, T_BobbaShot, NULL, R_Draw, &s_bobbashot5};
statetype s_bobbashot5       = {BOBBASHOT3SPR, BOBBASHOT3SPR, slide,     false, false,  8, 48, 0, NULL, C_Lethal, R_BobbaShot, &s_bobbashot6};
statetype s_bobbashot6       = {BOBBASHOT4SPR, BOBBASHOT4SPR, slide,     false, false,  8, 48, 0, NULL, C_Lethal, R_BobbaShot, &s_bobbashot7};
statetype s_bobbashot7       = {BOBBASHOT5SPR, BOBBASHOT5SPR, slide,     false, false,  8, 48, 0, NULL, C_Lethal, R_BobbaShot, &s_bobbashot8};
statetype s_bobbashot8       = {BOBBASHOT6SPR, BOBBASHOT6SPR, slide,     false, false,  8, 48, 0, NULL, C_Lethal, R_BobbaShot, &s_bobbashot5};
statetype s_bobbashotvanish1 = {BOBBASHOT6SPR, BOBBASHOT6SPR, step,      false, false,  8,  0, 0, NULL, NULL, R_Draw, &s_bobbashotvanish2};
statetype s_bobbashotvanish2 = {BOBBASHOT6SPR, BOBBASHOT6SPR, step,      false, false,  8,  0, 0, NULL, NULL, R_Draw, &s_bobbashotvanish3};
statetype s_bobbashotvanish3 = {BOBBASHOT6SPR, BOBBASHOT6SPR, step,      false, false,  8,  0, 0, NULL, NULL, R_Draw, NULL};

/*
===========================
=
= SpawnBobba
=
===========================
*/

void SpawnBobba(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->obclass = bobbaobj;
	new->active = ac_yes;
	new->priority = 0;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY) + -2*TILEGLOBAL;
	if (US_RndT() < 0x80)
	{
		new->xdir = 1;
	}
	else
	{
		new->xdir = -1;
	}
	new->ydir = 1;
	NewState(new, &s_bobbajump1);
}

/*
===========================
=
= T_BobbaShot
=
===========================
*/

#pragma argsused
void T_BobbaShot(objtype *ob)
{
	SD_PlaySound(SND_BOBBASHOT);
}

/*
===========================
=
= T_BobbaStand
=
===========================
*/

void T_BobbaStand(objtype *ob)
{
	Sint16 i;
	Uint16 far *map;

	if (++ob->temp1 == 3)
	{
		ob->temp1 = 0;
		GetNewObj(true);
		new->active = ac_removable;
		new->obclass = mshotobj;
		new->y = ob->y + 11*PIXGLOBAL;
		new->xdir = ob->xdir;
		if (ob->xdir == 1)
		{
			new->x = ob->x + 16*PIXGLOBAL;
		}
		else
		{
			new->x = ob->x + 11*PIXGLOBAL;
		}
		NewState(new, &s_bobbashot1);
		new->priority = 2;
		ob->state = &s_bobbaattack;
	}
	else
	{
		map = mapsegs[1] + mapbwidthtable[ob->tilebottom+1]/2 + ob->tilemidx;
		for (i=0; i<4; map += ob->xdir, i++)
		{
			if ( !tinf[*map+NORTHWALL]
				&& !tinf[*(map-mapwidth)+NORTHWALL]
				&& !tinf[*(map+mapwidth)+NORTHWALL] )
			{
				ob->xdir = -ob->xdir;
				break;
			}
		}
		ob->xspeed = ob->xdir << 5;
		ob->yspeed = -32;
		SD_PlaySound(SND_BOBBAJUMP);
	}
}

/*
===========================
=
= C_Bobba
=
===========================
*/

void C_Bobba(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		KillKeen();
	}
	else if (hit->obclass == stunshotobj)
	{
		ExplodeShot(hit);
		if (hit->xdir != 0)
		{
			ob->xdir = -hit->xdir;
		}
	}
}

/*
===========================
=
= R_Bobba
=
===========================
*/

void R_Bobba(objtype *ob)
{
	if (ob->hiteast)
	{
		ob->xdir = 1;
		ob->xspeed = -ob->xspeed;
	}
	else if (ob->hitwest)
	{
		ob->xdir = -1;
		ob->xspeed = -ob->xspeed;
	}

	if (ob->hitsouth)
		ob->yspeed = 0;

	if (ob->hitnorth)
	{
		SD_PlaySound(SND_BOBBALAND);
		ChangeState(ob, &s_bobbastand);
	}
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}

/*
===========================
=
= R_BobbaShot
=
===========================
*/

void R_BobbaShot(objtype *ob)
{
	if (ob->hitnorth || ob->hitsouth || ob->hiteast || ob->hitwest)
	{
		ChangeState(ob, &s_bobbashotvanish1);
	}
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}

/*
=============================================================================

						  BABOBBA

temp1 = jump counter (Babobba) / animation counter (Shot)

=============================================================================
*/

statetype s_babobbajump1       = {BABOBBAL2SPR,     BABOBBAR2SPR,     stepthink, false, false,   8, 0, 0, T_Projectile, C_Babobba, R_Babobba, &s_babobbajump2};
statetype s_babobbajump2       = {BABOBBAL3SPR,     BABOBBAR3SPR,     think,     false, false,   8, 0, 0, T_Projectile, C_Babobba, R_Babobba, &s_babobbajump2};
statetype s_babobbastand       = {BABOBBAL1SPR,     BABOBBAR1SPR,     step,      false, false,  20, 0, 0, T_BabobbaStand, C_Babobba, R_Draw, &s_babobbajump1};
statetype s_babobbaattack      = {BABOBBAL1SPR,     BABOBBAR1SPR,     step,      false, false,  70, 0, 0, NULL, C_Babobba, R_Draw, &s_babobbastand};
statetype s_babobbastun1       = {BABOBBAL2SPR,     BABOBBAR2SPR,     think,     false, false,   0, 0, 0, T_Projectile, NULL, R_Stunned, &s_babobbastun2};
statetype s_babobbastun2       = {BABOBBASTUNSPR,   BABOBBASTUNSPR,   think,     false, false,   0, 0, 0, T_Projectile, NULL, R_Stunned, NULL};
statetype s_babobbasleep1      = {BABOBBASLEEP1SPR, BABOBBASLEEP1SPR, step,      false, false,  15, 0, 0, NULL, C_BabobbaSleep, R_Draw, &s_babobbasleep2};
statetype s_babobbasleep2      = {BABOBBASLEEP2SPR, BABOBBASLEEP2SPR, step,      false, false,  15, 0, 0, NULL, C_BabobbaSleep, R_Draw, &s_babobbasleep3};
statetype s_babobbasleep3      = {BABOBBASLEEP3SPR, BABOBBASLEEP3SPR, step,      false, false,  15, 0, 0, NULL, C_BabobbaSleep, R_Draw, &s_babobbasleep4};
statetype s_babobbasleep4      = {BABOBBASLEEP4SPR, BABOBBASLEEP4SPR, step,      false, false, 500, 0, 0, NULL, C_BabobbaSleep, R_Draw, &s_babobbasleep5};
statetype s_babobbasleep5      = {BABOBBASLEEP3SPR, BABOBBASLEEP3SPR, step,      false, false,  15, 0, 0, NULL, C_BabobbaSleep, R_Draw, &s_babobbasleep6};
statetype s_babobbasleep6      = {BABOBBASLEEP2SPR, BABOBBASLEEP2SPR, step,      false, false,  15, 0, 0, NULL, C_BabobbaSleep, R_Draw, &s_babobbasleep7};
statetype s_babobbasleep7      = {BABOBBASLEEP1SPR, BABOBBASLEEP1SPR, step,      false, false,  15, 0, 0, NULL, C_BabobbaSleep, R_Draw, &s_babobbastand};
statetype s_babobbashot1       = {BABOBBASHOT1SPR,  BABOBBASHOT1SPR,  think,     false, false,   0, 0, 0, T_Projectile, C_Lethal, R_Bounce, &s_babobbashot2};
statetype s_babobbashot2       = {BABOBBASHOT2SPR,  BABOBBASHOT2SPR,  step,      false, false,   8, 0, 0, T_BabobbaShot, C_Lethal, R_Draw, &s_babobbashot3};
statetype s_babobbashot3       = {BABOBBASHOT1SPR,  BABOBBASHOT1SPR,  step,      false, false,   8, 0, 0, NULL, C_Lethal, R_Draw, &s_babobbashot2};
statetype s_babobbashotvanish1 = {BABOBBASHOT2SPR,  BABOBBASHOT2SPR,  step,      false, false,   8, 0, 0, T_BabobbaShotVanish, C_Lethal, R_Draw, &s_babobbashotvanish2};
statetype s_babobbashotvanish2 = {-1,               -1,               step,      false, false,   8, 0, 0, NULL, NULL, R_Draw, &s_babobbashotvanish1};

/*
===========================
=
= SpawnBabobba
=
===========================
*/

void SpawnBabobba(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->obclass = babobbaobj;
	new->active = ac_yes;
	new->priority = 0;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY) + -2*TILEGLOBAL;
	if (US_RndT() < 0x80)
	{
		new->xdir = 1;
	}
	else
	{
		new->xdir = -1;
	}
	new->ydir = 1;
	NewState(new, &s_babobbajump1);
}

/*
===========================
=
= T_BabobbaStand
=
===========================
*/

void T_BabobbaStand(objtype *ob)
{
	Sint16 i;
	Uint16 far *map;

	if (US_RndT() < 4)
	{
		ob->temp1 = 0;
		ob->state = &s_babobbasleep1;
	}
	else if (++ob->temp1 == 3)
	{
		ob->temp1 = 0;

		GetNewObj(true);
		new->active = ac_removable;
		new->obclass = mshotobj;
		new->y = ob->y + 4*PIXGLOBAL;
		new->xdir = ob->xdir;
		if (ob->xdir == 1)
		{
			new->x = ob->x + 16*PIXGLOBAL;
		}
		else
		{
			new->x = ob->x + 11*PIXGLOBAL;
		}
		new->xspeed = new->xdir << 5;
		NewState(new, &s_babobbashot1);
		new->priority = 2;
		ob->state = &s_babobbaattack;
	}
	else
	{
		map = mapsegs[1] + mapbwidthtable[ob->tilebottom+1]/2 + ob->tilemidx;
		for (i=0; i<4; map += ob->xdir, i++)
		{
			if ( !tinf[*map+NORTHWALL]
				&& !tinf[*(map-mapwidth)+NORTHWALL]
				&& !tinf[*(map+mapwidth)+NORTHWALL] )
			{
				ob->xdir = -ob->xdir;
				break;
			}
		}
		ob->xspeed = ob->xdir *24;
		ob->yspeed = -32;
	}
}

/*
===========================
=
= C_Babobba
=
===========================
*/

void C_Babobba(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		KillKeen();
	}
	else if (hit->obclass == stunshotobj)
	{
		StunObj(ob, hit, &s_babobbastun1);
	}
}

/*
===========================
=
= C_BabobbaSleep
=
===========================
*/

void C_BabobbaSleep(objtype *ob, objtype *hit)
{
	if (hit->obclass == stunshotobj)
	{
		StunObj(ob, hit, &s_babobbastun1);
	}
}

/*
===========================
=
= R_Babobba
=
===========================
*/

void R_Babobba(objtype *ob)
{
	if (ob->hiteast)
	{
		ob->xdir = 1;
		ob->xspeed = -ob->xspeed;
	}
	else if (ob->hitwest)
	{
		ob->xdir = -1;
		ob->xspeed = -ob->xspeed;
	}

	if (ob->hitsouth)
		ob->yspeed = 0;

	if (ob->hitnorth)
	{
		ChangeState(ob, &s_babobbastand);
	}
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}

/*
===========================
=
= T_BabobbaShot
=
===========================
*/

void T_BabobbaShot(objtype *ob)
{
	if (++ob->temp1 == 10)
	{
		ob->temp1 = 0;
		ob->state = &s_babobbashotvanish1;
	}
}

/*
===========================
=
= T_BabobbaShotVanish
=
===========================
*/

void T_BabobbaShotVanish(objtype *ob)
{
	if (++ob->temp1 == 5)
	{
		RemoveObj(ob);
	}
}

/*
=============================================================================

						  BLORB

=============================================================================
*/

statetype s_blorb1        = {BLORB1SPR, BLORB1SPR, slide, false, false, 20, 8, 8, 0, C_Lethal, R_Blorb, &s_blorb2};
statetype s_blorb2        = {BLORB2SPR, BLORB2SPR, slide, false, false, 20, 8, 8, 0, C_Lethal, R_Blorb, &s_blorb3};
statetype s_blorb3        = {BLORB3SPR, BLORB3SPR, slide, false, false, 20, 8, 8, 0, C_Lethal, R_Blorb, &s_blorb1};

/*
===========================
=
= SpawnBlorb
=
===========================
*/

void SpawnBlorb(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->obclass = blorbobj;
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
	if (US_RndT() < 0x80)
	{
		new->ydir = 1;
	}
	else
	{
		new->ydir = -1;
	}
	new->needtoclip = cl_fullclip;
	NewState(new, &s_blorb1);
}

/*
===========================
=
= R_Blorb
=
===========================
*/

void R_Blorb(objtype *ob)
{
	if (ob->hitnorth)
	{
		ob->ydir = -1;
		SD_PlaySound(SND_BLORBBOUNCE);
	}
	else if (ob->hitsouth)
	{
		ob->ydir = 1;
		SD_PlaySound(SND_BLORBBOUNCE);
	}
	if (ob->hitwest)
	{
		ob->xdir = -1;
		SD_PlaySound(SND_BLORBBOUNCE);
	}
	else if (ob->hiteast)
	{
		ob->xdir = 1;
		SD_PlaySound(SND_BLORBBOUNCE);
	}
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}

/*
=============================================================================

						  CEILICK

temp1 = initial y position

=============================================================================
*/

statetype s_ceilickhidden   = {TONGUE1SPR,     TONGUE1SPR,     think, false, false, 20, 0,   0, T_CeilickHidden, NULL, R_Draw, NULL};
statetype s_ceilickattack1  = {TONGUE1SPR,     TONGUE1SPR,     step,  false, false,  6, 0,   0, NULL, NULL, R_Draw, &s_ceilickattack2};
statetype s_ceilickattack2  = {TONGUE2SPR,     TONGUE2SPR,     step,  false, false,  6, 0,   0, NULL, C_Lethal, R_Draw, &s_ceilickattack3};
statetype s_ceilickattack3  = {TONGUE3SPR,     TONGUE3SPR,     step,  false, false,  6, 0,   0, NULL, C_Lethal, R_Draw, &s_ceilickattack4};
statetype s_ceilickattack4  = {TONGUE4SPR,     TONGUE4SPR,     step,  false, false,  6, 0,   0, NULL, C_Lethal, R_Draw, &s_ceilickattack5};
statetype s_ceilickattack5  = {TONGUE5SPR,     TONGUE5SPR,     step,  false, false,  6, 0,   0, NULL, C_Lethal, R_Draw, &s_ceilickattack6};
statetype s_ceilickattack6  = {TONGUE4SPR,     TONGUE4SPR,     step,  false, false,  6, 0,   0, NULL, C_Lethal, R_Draw, &s_ceilickattack7};
statetype s_ceilickattack7  = {TONGUE3SPR,     TONGUE3SPR,     step,  false, false,  6, 0,   0, NULL, C_Lethal, R_Draw, &s_ceilickattack8};
statetype s_ceilickattack8  = {TONGUE4SPR,     TONGUE4SPR,     step,  false, false,  6, 0,   0, NULL, C_Lethal, R_Draw, &s_ceilickattack9};
statetype s_ceilickattack9  = {TONGUE5SPR,     TONGUE5SPR,     step,  false, false,  6, 0,   0, NULL, C_Lethal, R_Draw, &s_ceilickattack10};
statetype s_ceilickattack10 = {TONGUE2SPR,     TONGUE2SPR,     step,  false, false, 10, 0,   0, NULL, C_Lethal, R_Draw, &s_ceilickattack11};
statetype s_ceilickattack11 = {TONGUE1SPR,     TONGUE1SPR,     step,  false, false, 10, 0,   0, NULL, NULL, R_Draw, &s_ceilicklaugh1};
statetype s_ceilicklaugh1   = {CEILICK1SPR,    CEILICK1SPR,    slide, true,  false, 16, 0,  16, T_CeilickLaugh, C_Ceilick, R_Draw, &s_ceilicklaugh2};
statetype s_ceilicklaugh2   = {CEILICK2SPR,    CEILICK2SPR,    step,  true,  false, 10, 0,   0, NULL, C_Ceilick, R_Draw, &s_ceilicklaugh3};
statetype s_ceilicklaugh3   = {CEILICK1SPR,    CEILICK1SPR,    step,  true,  false, 10, 0,   0, NULL, C_Ceilick, R_Draw, &s_ceilicklaugh4};
statetype s_ceilicklaugh4   = {CEILICK2SPR,    CEILICK2SPR,    step,  true,  false, 10, 0,   0, NULL, C_Ceilick, R_Draw, &s_ceilicklaugh5};
statetype s_ceilicklaugh5   = {CEILICK1SPR,    CEILICK1SPR,    step,  true,  false, 10, 0,   0, NULL, C_Ceilick, R_Draw, &s_ceilicklaugh6};
statetype s_ceilicklaugh6   = {CEILICK2SPR,    CEILICK2SPR,    step,  true,  false, 10, 0,   0, NULL, C_Ceilick, R_Draw, &s_ceilicklaugh7};
statetype s_ceilicklaugh7   = {CEILICK1SPR,    CEILICK1SPR,    step,  true,  false, 10, 0,   0, NULL, C_Ceilick, R_Draw, &s_ceilicklaugh8};
statetype s_ceilicklaugh8   = {CEILICK1SPR,    CEILICK1SPR,    slide, true,  false, 16, 0, -16, NULL, C_Ceilick, R_Draw, &s_ceilicklaugh9};
statetype s_ceilicklaugh9   = {-1,             -1,             step,  true,  false, 60, 0,   0, NULL, C_Ceilick, R_Draw, &s_ceilickhidden};
statetype s_ceilickstun     = {CEILICKSTUNSPR, CEILICKSTUNSPR, think, true,  false,  0, 0,   0, T_CeilickStunned, NULL, R_Stunned, NULL};

/*
===========================
=
= SpawnCeilick
=
===========================
*/

void SpawnCeilick(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->obclass = ceilickobj;
	new->active = ac_yes;
	new->priority = 0;
	new->needtoclip = cl_noclip;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = new->temp1 = CONVERT_TILE_TO_GLOBAL(tileY);
	new->ydir = 1;
	NewState(new, &s_ceilickhidden);
}

/*
===========================
=
= T_CeilickHidden
=
===========================
*/

void T_CeilickHidden(objtype *ob)
{
	if ( player->y - ob->y <= 40*PIXGLOBAL
		&& player->left < ob->right+PIXGLOBAL
		&& player->right > ob->left-PIXGLOBAL )
	{
		SD_PlaySound(SND_CEILICKATTACK);
		ob->state = &s_ceilickattack1;
	}
}

/*
===========================
=
= T_CeilickLaugh
=
===========================
*/

#pragma argsused
void T_CeilickLaugh(objtype *ob)
{
	SD_PlaySound(SND_CEILICKLAUGH);
}

/*
===========================
=
= T_CeilickStunned
=
===========================
*/

void T_CeilickStunned(objtype *ob)
{
	ob->needtoreact = true;	// to make sure the stunned stars animate
}

/*
===========================
=
= C_Ceilick
=
===========================
*/

void C_Ceilick(objtype *ob, objtype *hit)
{
	if (hit->obclass == stunshotobj)
	{
		ob->y = ob->temp1;
		ExplodeShot(hit);
		ob->temp1 = ob->temp2 = ob->temp3 = 0;
		ob->temp4 = ob->obclass;
		ChangeState(ob, &s_ceilickstun);
		ob->obclass = stunnedobj;
	}
}