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
K4_ACT3.C
=========

Contains the following actor types (in this order):

- Treasure Eater
- Mimrock
- Dopefish
- Schoolfish
- Sprite
- Mine
- Lindsey
- Dart Shooter & Dart
- Wetsuit

*/

#include "CK_DEF.H"

/*
=============================================================================

						  TREASURE EATER

temp1 = turn counter

=============================================================================
*/

statetype s_eaterstand1 = {EATERSTAND1SPR, EATERSTAND1SPR, step, false, false, 20, 0, 0, NULL, C_Eater, R_Draw, &s_eaterstand2};
statetype s_eaterstand2 = {EATERSTAND2SPR, EATERSTAND2SPR, step, false, false, 20, 0, 0, T_EaterJump, C_Eater, R_Draw, NULL};
statetype s_eatertport1 = {SMOKE1SPR, SMOKE1SPR, step, false, false, 20, 0, 0, NULL, C_Eater, R_Draw, &s_eatertport2};
statetype s_eatertport2 = {SMOKE2SPR, SMOKE2SPR, step, false, false, 20, 0, 0, NULL, C_Eater, R_Draw, &s_eatertport3};
statetype s_eatertport3 = {SMOKE3SPR, SMOKE3SPR, step, false, false, 20, 0, 0, NULL, C_Eater, R_Draw, &s_eatertport4};
statetype s_eatertport4 = {SMOKE4SPR, SMOKE4SPR, step, false, false, 20, 0, 0, T_EaterTeleport, C_Eater, R_Draw, &s_eatertport5};
statetype s_eatertport5 = {SMOKE4SPR, SMOKE4SPR, step, false, false, 20, 0, 0, NULL, C_Eater, R_Draw, &s_eatertport6};
statetype s_eatertport6 = {SMOKE3SPR, SMOKE3SPR, step, false, false, 20, 0, 0, NULL, C_Eater, R_Draw, &s_eatertport7};
statetype s_eatertport7 = {SMOKE2SPR, SMOKE2SPR, step, false, false, 20, 0, 0, NULL, C_Eater, R_Draw, &s_eatertport8};
statetype s_eatertport8 = {SMOKE1SPR, SMOKE1SPR, step, false, false, 20, 0, 0, NULL, C_Eater, R_Draw, &s_eaterjump1};
statetype s_eaterjump1  = {EATERJUMPL1SPR, EATERJUMPR1SPR, stepthink, false, false, 6, 0, 0, T_WeakProjectile, C_Eater, R_EaterAir, &s_eaterjump2};
statetype s_eaterjump2  = {EATERJUMPL2SPR, EATERJUMPR2SPR, stepthink, false, false, 6, 0, 0, T_WeakProjectile, C_Eater, R_EaterAir, &s_eaterjump3};
statetype s_eaterjump3  = {EATERJUMPL3SPR, EATERJUMPR3SPR, stepthink, false, false, 6, 0, 0, T_WeakProjectile, C_Eater, R_EaterAir, &s_eaterjump4};
statetype s_eaterjump4  = {EATERJUMPL2SPR, EATERJUMPR2SPR, stepthink, false, false, 6, 0, 0, T_WeakProjectile, C_Eater, R_EaterAir, &s_eaterjump1};
statetype s_eaterstun   = {EATERJUMPL1SPR, EATERJUMPL1SPR, think, false, false, 0, 0, 0, T_Projectile, 0, R_Stunned, &s_eaterstun2};
statetype s_eaterstun2  = {EATERSTUNSPR, EATERSTUNSPR, think, false, false, 0, 0, 0, T_Projectile, 0, R_Stunned, NULL};

statetype s_eatenbonus1 = {EATENBONUS1SPR, EATENBONUS1SPR, slide, false, false, 10, 0, 8, NULL, NULL, R_Draw, &s_eatenbonus2};
statetype s_eatenbonus2 = {EATENBONUS2SPR, EATENBONUS2SPR, slide, false, false, 10, 0, 8, NULL, NULL, R_Draw, &s_eatenbonus3};
statetype s_eatenbonus3 = {EATENBONUS3SPR, EATENBONUS3SPR, slide, false, false, 10, 0, 8, NULL, NULL, R_Draw, &s_eatenbonus4};
statetype s_eatenbonus4 = {EATENBONUS4SPR, EATENBONUS4SPR, slide, false, false, 10, 0, 8, NULL, NULL, R_Draw, NULL};

/*
===========================
=
= SpawnEater
=
===========================
*/

void SpawnEater(Sint16 x, Sint16 y)
{
	GetNewObj(false);
	new->obclass = treasureeaterobj;
	new->active = ac_yes;
	new->priority = 3;
	new->x = CONVERT_TILE_TO_GLOBAL(x);
	new->y = CONVERT_TILE_TO_GLOBAL(y) - 24*PIXGLOBAL;
	if (US_RndT() < 0x80)
	{
		new->xdir = 1;
	}
	else
	{
		new->xdir = -1;
	}
	new->ydir = 1;
	NewState(new, &s_eaterstand1);
}

/*
===========================
=
= T_EaterJump
=
===========================
*/

void T_EaterJump(objtype *ob)
{
	objtype *ob2;
	Uint16 x;
	Sint16 y;
	Uint16 far *map;
	Uint16 intile, rowdiff, width;

	ob->state = &s_eaterjump1;

	//jump straight up if below bonus object:
	for (ob2 = player->next; ob2; ob2 = ob2->next)
	{
		if (ob2->obclass == bonusobj && ob2->active == ac_yes
			&& ob2->right > ob->left && ob2->left < ob->right
			&& ob2->bottom < ob->top && ob2->bottom + 3*TILEGLOBAL > ob->top)
		{
			ob->xspeed = 0;
			ob->yspeed = -48;
			return;
		}
	}

	//jump straight up if below bonus tile:
	map = mapsegs[1] + mapbwidthtable[ob->tiletop-3]/2 + ob->tileleft;
	width = ob->tileright-ob->tileleft+1;
	rowdiff = mapwidth-width;
	for (y=0; y<3; y++, map+=rowdiff)
	{
		for (x=0; x<width; x++, map++)
		{
			intile = tinf[INTILE+*map];
			if (intile == INTILE_DROP || intile >= INTILE_BONUS100 && intile <= INTILE_AMMO)
			{
				ob->xspeed = 0;
				ob->yspeed = -48;
				return;
			}
		}
	}

	//vanish after having checked both directions:
	if (ob->temp1 >= 2)
	{
		// BUG? this doesn't play a sound
		ob->state = &s_eatertport1;
		return;
	}

	//jump in current direction if there is a floor in that direction:
	map = mapsegs[1] + mapbwidthtable[ob->tilebottom-2]/2 + ob->tilemidx;
	map += ob->xdir * 4;
	for (y=0; y<4; y++, map+=mapwidth)
	{
		if (tinf[NORTHWALL+*map])
		{
			ob->xspeed = ob->xdir * 20;
			ob->yspeed = -24;
			return;
		}
	}

	//couldn't jump in current direction, so turn around:
	if (++ob->temp1 == 2)
	{
		SD_PlaySound(SND_TREASUREEATERVANISH);
		ob->state = &s_eatertport1;
		return;
	}

	//jump in opposite direction:
	ob->xdir = -ob->xdir;
	ob->xspeed = ob->xdir * 20;
	ob->yspeed = -24;
}

/*
===========================
=
= T_EaterTeleport
=
===========================
*/

void T_EaterTeleport(objtype *ob)
{
	objtype *ob2;

	ob->temp1 = 0;
	for (ob2=player->next; ob2; ob2=ob2->next)
	{
		if (ob2->obclass == bonusobj)
		{
			ob->x = ob2->x - 8*PIXGLOBAL;
			ob->y = ob2->y;
			NewState(ob, &s_eatertport5);
			return;
		}
	}
	RemoveObj(ob);
}

/*
===========================
=
= C_Eater
=
===========================
*/

void C_Eater(objtype *ob, objtype *hit)
{
	if (hit->obclass == bonusobj)
	{
		//BUG? bonus object might be a key, and eating a key makes a level unwinnable
		hit->obclass = inertobj;
		hit->priority = 3;
		ChangeState(hit, &s_eatenbonus1);
		SD_PlaySound(SND_EATBONUS);
	}
	else if (hit->obclass == stunshotobj)
	{
		//basically StunObj(), but in different order:
		ob->temp1 = 0;
		ob->temp2 = 0;
		ob->temp3 = 0;
		ob->temp4 = ob->obclass;
		ob->obclass = stunnedobj;
		ExplodeShot(hit);
		ChangeState(ob, &s_eaterstun);

		ob->yspeed -= 16;
	}
}

/*
===========================
=
= EaterInTile
=
===========================
*/

void EaterInTile(objtype *ob)
{
	Uint16 x, y;
	Uint16 far *map;
	Uint16 rowdiff, intile;

	map = mapsegs[1] + mapbwidthtable[ob->tiletop]/2 + ob->tileleft;
	rowdiff = mapwidth-(ob->tileright-ob->tileleft+1);
	for (y=ob->tiletop; y<=ob->tilebottom; y++, map+=rowdiff)
	{
		for (x=ob->tileleft; x<=ob->tileright; x++, map++)
		{
			intile = tinf[INTILE + *map] & INTILE_TYPEMASK;
			if (intile == INTILE_DROP || intile >= INTILE_BONUS100 && intile <= INTILE_AMMO)
			{
				RF_MemToMap(&zeromap, 1, x, y, 1, 1);
				GetNewObj(true);
				new->obclass = inertobj;
				new->priority = 3;
				new->needtoclip = cl_noclip;
				new->x = CONVERT_TILE_TO_GLOBAL(x);
				new->y = CONVERT_TILE_TO_GLOBAL(y);
				new->active = ac_removable;
				ChangeState(new, &s_eatenbonus1);	//using ChangeState and not NewState is fine for noclipping objects
				//BUG? this doesn't play a sound
				break;
			}
		}
	}
}

/*
===========================
=
= R_EaterAir
=
===========================
*/

void R_EaterAir(objtype *ob)
{
	EaterInTile(ob);

	if (ob->hitnorth)
		ChangeState(ob, &s_eaterstand1);

	if (ob->hiteast || ob->hitwest)
	{
		ob->temp1++;
		ob->xdir = -ob->xdir;
		ob->xspeed = 0;
	}

	if (ob->hitnorth)	//BUG? maybe this was supposed to check hitsouth as well?
		ob->yspeed = 0;

	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}

/*
=============================================================================

						  MIMROCK

=============================================================================
*/

statetype s_mimrock   = {MIMROCKSPR, MIMROCKSPR, step, false, true, 20, 0, 0, T_MimrockWait, NULL, R_Walk, &s_mimrock};
statetype s_mimsneak1 = {MIMROCKWALKR1SPR, MIMROCKWALKL1SPR, step, false, true, 6, 64, 0, T_MimrockSneak, C_Mimrock, R_Walk, &s_mimsneak2};
statetype s_mimsneak2 = {MIMROCKWALKR2SPR, MIMROCKWALKL2SPR, step, false, true, 6, 64, 0, T_MimrockSneak, C_Mimrock, R_Walk, &s_mimsneak3};
statetype s_mimsneak3 = {MIMROCKWALKR3SPR, MIMROCKWALKL3SPR, step, false, true, 6, 64, 0, T_MimrockSneak, C_Mimrock, R_Walk, &s_mimsneak4};
statetype s_mimsneak4 = {MIMROCKWALKR4SPR, MIMROCKWALKL4SPR, step, false, true, 6, 64, 0, T_MimrockSneak, C_Mimrock, R_Walk, &s_mimsneak5};
statetype s_mimsneak5 = {MIMROCKWALKR1SPR, MIMROCKWALKL1SPR, step, false, true, 6, 64, 0, T_MimrockSneak, C_Mimrock, R_Walk, &s_mimsneak6};
statetype s_mimsneak6 = {MIMROCKWALKR2SPR, MIMROCKWALKL2SPR, step, false, true, 6, 64, 0, T_MimrockSneak, C_Mimrock, R_Walk, &s_mimrock};
statetype s_mimbonk1  = {MIMROCKJUMPL1SPR, MIMROCKJUMPR1SPR, stepthink, false, false, 24, 0, 0, T_WeakProjectile, C_MimLethal, R_MimAir, &s_mimbonk2};
statetype s_mimbonk2  = {MIMROCKJUMPL2SPR, MIMROCKJUMPR2SPR, stepthink, false, false, 10, 0, 0, T_WeakProjectile, C_MimLethal, R_MimAir, &s_mimbonk3};
statetype s_mimbonk3  = {MIMROCKJUMPL3SPR, MIMROCKJUMPR3SPR, think, false, false, 10, 0, 0, T_WeakProjectile, C_MimLethal, R_MimAir, &s_mimbonk2};
statetype s_mimbounce = {MIMROCKJUMPL3SPR, MIMROCKJUMPR3SPR, think, false, false, 10, 0, 0, T_Projectile, C_Mimrock, R_MimBounce, NULL};
statetype s_mimstun   = {MIMROCKJUMPL3SPR, MIMROCKJUMPL3SPR, think, false, false, 12, 0, 0, T_Projectile, NULL, R_Stunned, &s_mimstun2};
statetype s_mimstun2  = {MINROCKSTUNSPR, MINROCKSTUNSPR, think, false, false, 12, 0, 0, T_Projectile, NULL, R_Stunned, NULL};

/*
===========================
=
= SpawnMimrock
=
===========================
*/

void SpawnMimrock(Sint16 x, Sint16 y)
{
	GetNewObj(false);
	new->obclass = mimrockobj;
	new->active = ac_yes;
	new->priority = 3;
	new->x = CONVERT_TILE_TO_GLOBAL(x);
	new->y = CONVERT_TILE_TO_GLOBAL(y)+ -13*PIXGLOBAL;
	new->ydir = new->xdir = 1;
	NewState(new, &s_mimrock);
}

/*
===========================
=
= T_MimrockWait
=
===========================
*/

void T_MimrockWait(objtype *ob)
{
	if (abs(ob->bottom - player->bottom) > 5*TILEGLOBAL)
		return;

	if (abs(ob->x - player->x) > 3*TILEGLOBAL)
	{
		if (player->x < ob->x)
		{
			if (player->xdir == -1)
			{
				ob->xdir = -1;
				ob->state = &s_mimsneak1;
			}
		}
		else
		{
			if (player->xdir == 1)
			{
				ob->xdir = 1;
				ob->state = &s_mimsneak1;
			}
		}
	}
}

/*
===========================
=
= T_MimrockSneak
=
===========================
*/

void T_MimrockSneak(objtype *ob)
{
	if (abs(ob->bottom - player->bottom) > 5*TILEGLOBAL
		|| ob->xdir != player->xdir)
	{
		ob->state = &s_mimrock;
	}
	else if (abs(ob->x - player->x) < 4*TILEGLOBAL)
	{
		ob->xspeed = ob->xdir * 20;
		ob->yspeed = -40;
		ytry = ob->yspeed * tics;
		ob->state = &s_mimbonk1;
	}
}

/*
===========================
=
= C_Mimrock
=
===========================
*/

void C_Mimrock(objtype *ob, objtype *hit)
{
	if (hit->obclass == stunshotobj)
	{
		//basically StunObj(), but in different order:
		ob->temp1 = 0;
		ob->temp2 = 0;
		ob->temp3 = 0;
		ob->temp4 = ob->obclass;
		ob->obclass = stunnedobj;
		ExplodeShot(hit);
		ChangeState(ob, &s_mimstun);

		ob->yspeed -= 16;
	}
}

/*
===========================
=
= C_MimLethal
=
===========================
*/

void C_MimLethal(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		KillKeen();
	}
	else
	{
		C_Mimrock(ob, hit);
	}
}

/*
===========================
=
= R_MimAir
=
===========================
*/

void R_MimAir(objtype *ob)
{
	if (ob->hitnorth)
	{
		SD_PlaySound(SND_HELMETHIT);
		ob->yspeed = -20;
		ChangeState(ob, &s_mimbounce);
	}

	if (ob->hiteast || ob->hitwest)
		ob->xspeed = 0;

	if (ob->hitnorth || ob->hitsouth)
		ob->yspeed = 0;

	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}

/*
===========================
=
= R_MimBounce
=
===========================
*/

void R_MimBounce(objtype *ob)
{
	if (ob->hitnorth)
	{
		SD_PlaySound(SND_HELMETHIT);
		ChangeState(ob, &s_mimrock);
	}

	if (ob->hiteast || ob->hitwest)
		ob->xspeed = 0;

	if (ob->hitnorth)
		ob->yspeed = 0;

	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}

/*
=============================================================================

						  DOPEFISH

temp1 = blocked (cannot change xdir to chase Keen while this is non-zero)
temp2 = old x position
temp3 = old y position
temp4 = pointer to object being eaten
        (BUG: pointer may be invalid after loading a saved game!)

=============================================================================
*/

statetype s_dopefish1  = {DOPEFISHSWIML1SPR, DOPEFISHSWIMR1SPR, stepthink, false, false, 20, 0, 0, T_Dope, C_Dope, R_Fish, &s_dopefish2};
statetype s_dopefish2  = {DOPEFISHSWIML2SPR, DOPEFISHSWIMR2SPR, stepthink, false, false, 20, 0, 0, T_Dope, C_Dope, R_Fish, &s_dopefish1};
statetype s_dopeattack = {DOPEFISHHUNGRYLSPR, DOPEFISHHUNGRYRSPR, think, false, false, 0, 0, 0, T_DopeHunt, NULL, R_Draw, NULL};
statetype s_dopeeat    = {DOPEFISHSWIML1SPR, DOPEFISHSWIMR1SPR, step, false, false, 60, 0, 0, NULL, NULL, R_Draw, &s_dopeburp1};
statetype s_dopeburp1  = {DOPEFISHBURP1SPR, DOPEFISHBURP1SPR, step, false, false, 60, 0, 0, T_Burp, NULL, R_Draw, &s_dopeburp2};
statetype s_dopeburp2  = {DOPEFISHBURP2SPR, DOPEFISHBURP2SPR, step, false, false, 60, 0, 0, NULL, NULL, R_Draw, &s_dopereturn};
statetype s_dopereturn = {DOPEFISHSWIML1SPR, DOPEFISHSWIMR1SPR, think, false, false, 0, 0, 0, T_DopeReturn, NULL, R_Draw, &s_dopefish1};

statetype s_dopefood     = {SCHOOLFISHL1SPR, SCHOOLFISHR1SPR, think, false, false, 0, 0, 0, NULL, NULL, R_Draw, NULL};
statetype s_keendopefood = {SCUBAKEENDEAD1SPR, SCUBAKEENDEAD1SPR, think, false, false, 0, 0, 0, NULL, NULL, R_Draw, &s_keendieslow};
statetype s_keendieslow  = {-1, -1, step, false, false, 180, 0, 0, T_EatenKeen, NULL, R_Draw, &s_keendieslow};

statetype s_bubble1 = {BIGBUBBLE1SPR, BIGBUBBLE1SPR, think, false, false, 20, 0, 20, T_Bubble, NULL, R_Draw, &s_bubble2};
statetype s_bubble2 = {BIGBUBBLE2SPR, BIGBUBBLE2SPR, think, false, false, 20, 0, 20, T_Bubble, NULL, R_Draw, &s_bubble3};
statetype s_bubble3 = {BIGBUBBLE3SPR, BIGBUBBLE3SPR, think, false, false, 20, 0, 20, T_Bubble, NULL, R_Draw, &s_bubble4};
statetype s_bubble4 = {BIGBUBBLE4SPR, BIGBUBBLE4SPR, think, false, false, 20, 0, 20, T_Bubble, NULL, R_Draw, &s_bubble1};

/*
===========================
=
= SpawnDopefish
=
===========================
*/

void SpawnDopefish(Sint16 x, Sint16 y)
{
	GetNewObj(false);
	new->obclass = dopefishobj;
	new->active = ac_yes;
	new->priority = 2;
	new->needtoclip = cl_fullclip;
	new->x = CONVERT_TILE_TO_GLOBAL(x);
	new->y = CONVERT_TILE_TO_GLOBAL(y) + -3*TILEGLOBAL;
	if (US_RndT() < 0x80)
	{
		new->xdir = 1;
	}
	else
	{
		new->xdir = -1;
	}
	new->ydir = 1;
	NewState(new, &s_dopefish1);
}

/*
===========================
=
= T_EatenKeen
=
===========================
*/

void T_EatenKeen(objtype *ob)
{
	ob++;			// shut up compiler
	playstate = ex_died;
}

/*
===========================
=
= T_Dope
=
===========================
*/

void T_Dope(objtype *ob)
{
	if (ob->temp1 == 0)
	{
		if (ob->x < player->x)
		{
			ob->xdir = 1;
		}
		else
		{
			ob->xdir = -1;
		}
	}
	AccelerateXv(ob, ob->xdir, 10);

	if (ob->y < player->y)
	{
		AccelerateY(ob, 1, 10);
	}
	else
	{
		AccelerateY(ob, -1, 10);
	}
}

/*
===========================
=
= T_DopeHunt
=
===========================
*/

void T_DopeHunt(objtype *ob)
{
	objtype *target;
	Sint16 xdist, ydist;

	target = (objtype *)(ob->temp4);
	ydist = target->y - TILEGLOBAL - ob->y;
	if (ob->xdir == 1)
	{
		xdist = target->right + 2*PIXGLOBAL - ob->right;
	}
	else
	{
		xdist = target->left - 2*PIXGLOBAL - ob->left;
	}

	if (xdist < 0)
	{
		xtry = tics * -32;
		if (xtry < xdist)
			xtry = xdist;
	}
	else
	{
		xtry = tics * 32;
		if (xtry > xdist)
			xtry = xdist;
	}

	if (ydist < 0)
	{
		ytry = tics * -32;
		if (ytry < ydist)
			ytry = ydist;
	}
	else
	{
		ytry = tics * 32;
		if (ytry > ydist)
			ytry = ydist;
	}

	if (xtry == xdist && ytry == ydist)
	{
		if (target == player)
		{
			ChangeState(target, &s_keendieslow);
		}
		else if (target->state->nextstate)
		{
			ChangeState(target, target->state->nextstate);
		}
		else
		{
			RemoveObj(target);
		}
		ob->state = &s_dopeeat;
	}
}

/*
===========================
=
= T_DopeReturn
=
===========================
*/

void T_DopeReturn(objtype *ob)
{
	Sint16 xdist, ydist;

	ydist = ob->temp3 - ob->y;
	xdist = ob->temp2 - ob->x;

	if (xdist < 0)
	{
		xtry = tics * -32;
		if (xtry < xdist)
			xtry = xdist;
	}
	else
	{
		xtry = tics * 32;
		if (xtry > xdist)
			xtry = xdist;
	}

	if (ydist < 0)
	{
		ytry = tics * -32;
		if (ytry < ydist)
			ytry = ydist;
	}
	else
	{
		ytry = tics * 32;
		if (ytry > ydist)
			ytry = ydist;
	}

	if (xtry == xdist && ytry == ydist)
	{
		ob->state = ob->state->nextstate;
		ob->needtoclip = cl_fullclip;
	}
}

/*
===========================
=
= T_Burp
=
===========================
*/

void T_Burp(objtype *ob)
{
	GetNewObj(true);
	new->x = ob->x + 56*PIXGLOBAL;
	new->y = ob->y + 32*PIXGLOBAL;
	new->obclass = inertobj;
	new->priority = 3;
	new->active = ac_removable;
	new->needtoclip = cl_noclip;
	new->yspeed = -20;
	new->xspeed = 4;
	NewState(new, &s_bubble1);
	SD_PlaySound(SND_BURP);
}

/*
===========================
=
= T_Bubble
=
===========================
*/

void T_Bubble(objtype *ob)
{
	T_Velocity(ob);
	if (US_RndT() < tics * 16)
		ob->xspeed = -ob->xspeed;

	if (ob->y < 3*TILEGLOBAL)
		RemoveObj(ob);
}

/*
===========================
=
= C_Dope
=
===========================
*/

void C_Dope(objtype *ob, objtype *hit)
{
	if (hit->obclass == schoolfishobj)
	{
		ChangeState(hit, &s_dopefood);
	}
	else if (hit->obclass == keenobj && !godmode)
	{
		hit->obclass = inertobj;	//prevents other objects from killing Keen before he is fully swallowed
		hit->needtoclip = cl_noclip;
		SD_PlaySound(SND_KEENDEAD);
		ChangeState(hit, &s_keendopefood);
	}
	else
	{
		return;
	}

	ob->temp2 = ob->x;
	ob->temp3 = ob->y;
	ob->temp4 = (Sint16)hit;
	if (hit->midx < ob->midx)
	{
		ob->xdir = -1;
	}
	else
	{
		ob->xdir = 1;
	}
	ChangeState(ob, &s_dopeattack);
	ob->needtoclip = cl_noclip;
}

/*
===========================
=
= R_Fish
=
===========================
*/

void R_Fish(objtype *ob)	//for Dopefish and Schoolfish
{
	if ((ob->hitsouth || ob->hitnorth) && ob->temp1 == 0)
		ob->temp1++;

	if (ob->hiteast || ob->hitwest)
	{
		ob->xspeed = 0;
		ob->xdir = -ob->xdir;
		ob->temp1 = 1;
	}

	if (!ob->hitsouth && !ob->hitnorth)
		ob->temp1 = 0;

	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}

/*
=============================================================================

						  SCHOOLFISH

temp1 = blocked (cannot change xdir to chase Keen while this is non-zero)

=============================================================================
*/

statetype s_schoolfish1 = {SCHOOLFISHL1SPR, SCHOOLFISHR1SPR, stepthink, false, false, 20, 0, 0, T_SchoolFish, NULL, R_Fish, &s_schoolfish2};
statetype s_schoolfish2 = {SCHOOLFISHL2SPR, SCHOOLFISHR2SPR, stepthink, false, false, 20, 0, 0, T_SchoolFish, NULL, R_Fish, &s_schoolfish1};

/*
===========================
=
= SpawnSchoolfish
=
===========================
*/

void SpawnSchoolfish(Sint16 x, Sint16 y)
{
	GetNewObj(false);
	new->obclass = schoolfishobj;
	new->active = ac_yes;
	new->needtoclip = cl_fullclip;
	new->priority = 0;
	new->x = CONVERT_TILE_TO_GLOBAL(x);
	new->y = CONVERT_TILE_TO_GLOBAL(y);
	new->ydir = new->xdir = 1;
	NewState(new, &s_schoolfish1);
}

/*
===========================
=
= T_SchoolFish
=
===========================
*/

void T_SchoolFish(objtype *ob)
{
	if (ob->temp1 == 0)
	{
		if (ob->x < player->x)
		{
			ob->xdir = 1;
		}
		else
		{
			ob->xdir = -1;
		}
	}
	AccelerateXv(ob, ob->xdir, 10);

	if (ob->y < player->y)
	{
		AccelerateY(ob, 1, 10);
	}
	else
	{
		AccelerateY(ob, -1, 10);
	}
}

/*
=============================================================================

						  PIXIE (a.k.a. SPRITE)

=============================================================================
*/

statetype s_pixie       = {SPRITEFLOATSPR,  SPRITEFLOATSPR,  think, false, false, 10, 0, 0, T_Pixie, C_Lethal, R_Draw, &s_pixie};
statetype s_pixielook   = {SPRITEAIMLSPR,   SPRITEAIMRSPR,   step, false, false, 40, 0, 0, T_PixieCheck, C_Lethal, R_Draw, &s_pixie};
statetype s_pixieshoot  = {SPRITESHOOTLSPR, SPRITESHOOTRSPR, step, false, false, 40, 0, 0, T_PixieShoot, C_Lethal, R_Draw, &s_pixieshoot2};
statetype s_pixieshoot2 = {SPRITESHOOTLSPR, SPRITESHOOTRSPR, step, false, false, 30, 0, 0, NULL, C_Lethal, R_Draw, &s_pixie};
statetype s_pixiefire1  = {SPRITESHOT1SPR,  SPRITESHOT1SPR,  slide, false, false, 10, 64, 0, NULL, C_Lethal, R_Mshot, &s_pixiefire2};
statetype s_pixiefire2  = {SPRITESHOT2SPR,  SPRITESHOT2SPR,  slide, false, false, 10, 64, 0, NULL, C_Lethal, R_Mshot, &s_pixiefire3};
statetype s_pixiefire3  = {SPRITESHOT3SPR,  SPRITESHOT3SPR,  slide, false, false, 10, 64, 0, NULL, C_Lethal, R_Mshot, &s_pixiefire4};
statetype s_pixiefire4  = {SPRITESHOT4SPR,  SPRITESHOT4SPR,  slide, false, false, 10, 64, 0, NULL, C_Lethal, R_Mshot, &s_pixiefire1};

/*
===========================
=
= SpawnPixie
=
===========================
*/

void SpawnPixie(Sint16 x, Sint16 y)
{
	GetNewObj(false);
	new->obclass = pixieobj;
	new->active = ac_yes;
	new->priority = 0;
	new->needtoclip = cl_noclip;
	new->x = CONVERT_TILE_TO_GLOBAL(x);
	new->y = new->temp1 = CONVERT_TILE_TO_GLOBAL(y);
	new->ydir = new->xdir = 1;
	NewState(new, &s_pixie);
}

/*
===========================
=
= T_Pixie
=
===========================
*/

void T_Pixie(objtype *ob)
{
	AccelerateY(ob, ob->ydir, 8);
	if ((Sint16)(ob->temp1 - ob->y) > 2*PIXGLOBAL)
	{
		ob->ydir = 1;
	}
	if ((Sint16)(ob->y - ob->temp1) > 2*PIXGLOBAL)
	{
		ob->ydir = -1;
	}

	if (player->top < ob->bottom && player->bottom > ob->top)
	{
		if (player->x < ob->x)
		{
			ob->xdir = -1;
		}
		else
		{
			ob->xdir = 1;
		}
		ob->state = &s_pixielook;
	}
}

/*
===========================
=
= T_PixieCheck
=
===========================
*/

void T_PixieCheck(objtype *ob)
{
	if (player->top < ob->bottom && player->bottom > ob->top)
		ob->state = &s_pixieshoot;
}

/*
===========================
=
= T_PixieShoot
=
===========================
*/

void T_PixieShoot(objtype *ob)
{
	GetNewObj(true);
	new->x = ob->x;
	new->y = ob->y + 8*PIXGLOBAL;
	new->priority = 0;
	new->obclass = mshotobj;
	new->active = ac_removable;
	SD_PlaySound(SND_KEENFIRE);	//BUG?
	new->xdir = ob->xdir;
	NewState(new, &s_pixiefire1);
	SD_PlaySound(SND_SPRITEFIRE);
}

/*
===========================
=
= R_Mshot
=
===========================
*/

void R_Mshot(objtype *ob)
{
	if (ob->hitnorth || ob->hiteast || ob->hitsouth || ob->hitwest)
	{
		RemoveObj(ob);
	}
	else
	{
		RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
	}
}

/*
=============================================================================

						  MINE

=============================================================================
*/
statetype s_mine      = {MINESPR,         MINESPR,         think, false, false, 10, 0, 0, T_Platform, C_Mine, R_Draw, &s_mine};
statetype s_mineboom1 = {MINEEXPLODE1SPR, MINEEXPLODE1SPR, step, false, false, 30, 0, 0, NULL, NULL, R_Draw, &s_mineboom2};
statetype s_mineboom2 = {MINEEXPLODE2SPR, MINEEXPLODE2SPR, step, false, false, 30, 0, 0, NULL, NULL, R_Draw, NULL};

/*
===========================
=
= SpawnMine
=
===========================
*/

void SpawnMine(Sint16 x, Sint16 y, Sint16 dir)
{
	GetNewObj(false);
	new->obclass = mineobj;
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
	NewState(new, &s_mine);
}

/*
===========================
=
= C_Mine
=
===========================
*/

void C_Mine(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		ChangeState(ob, &s_mineboom1);
		SD_PlaySound(SND_MINEEXPLODE);
		KillKeen();
	}
}

/*
=============================================================================

						  PRINCESS LINDSEY

temp1 = initial y position

=============================================================================
*/
statetype s_lindsey1 = {LINDSEY1SPR, LINDSEY1SPR, stepthink, false, false, 20, 0, 0, T_Lindsey, NULL, R_Draw, &s_lindsey2};
statetype s_lindsey2 = {LINDSEY2SPR, LINDSEY2SPR, stepthink, false, false, 20, 0, 0, T_Lindsey, NULL, R_Draw, &s_lindsey3};
statetype s_lindsey3 = {LINDSEY3SPR, LINDSEY3SPR, stepthink, false, false, 20, 0, 0, T_Lindsey, NULL, R_Draw, &s_lindsey4};
statetype s_lindsey4 = {LINDSEY4SPR, LINDSEY4SPR, stepthink, false, false, 20, 0, 0, T_Lindsey, NULL, R_Draw, &s_lindsey1};

/*
===========================
=
= SpawnLindsey
=
===========================
*/

void SpawnLindsey(Sint16 x, Sint16 y)
{
	GetNewObj(false);
	new->obclass = lindseyobj;
	new->active = ac_yes;
	new->priority = 0;
	new->x = CONVERT_TILE_TO_GLOBAL(x);
	new->y = new->temp1 = CONVERT_TILE_TO_GLOBAL(y) - TILEGLOBAL;
	new->ydir = 1;
	NewState(new, &s_lindsey1);
}

/*
===========================
=
= T_Lindsey
=
===========================
*/

void T_Lindsey(objtype *ob)
{
	AccelerateY(ob, ob->ydir, 8);
	if (ob->temp1 - (Sint16)ob->y > 2*PIXGLOBAL)
	{
		ob->ydir = 1;
	}
	if ((Sint16)ob->y - ob->temp1 > 2*PIXGLOBAL)
	{
		ob->ydir = -1;
	}
}

/*
=============================================================================

						  DARTS

temp1 = direction

=============================================================================
*/

statetype s_dartthrower = {0, 0, step, false, false, 150, 0, 0, T_DartShoot, NULL, NULL, &s_dartthrower};
statetype s_dart1       = {DARTL1SPR, DARTR1SPR, slide, false, false, 6, 64, 0, NULL, C_Lethal, R_Mshot, &s_dart2};
statetype s_dart2       = {DARTL2SPR, DARTR2SPR, slide, false, false, 6, 64, 0, NULL, C_Lethal, R_Mshot, &s_dart1};
statetype s_dartup1     = {DARTU1SPR, DARTU1SPR, slide, false, false, 6, 0, 64, NULL, C_Lethal, R_Mshot, &s_dartup2};
statetype s_dartup2     = {DARTU2SPR, DARTU2SPR, slide, false, false, 6, 0, 64, NULL, C_Lethal, R_Mshot, &s_dartup1};
statetype s_dartdown1   = {DARTD1SPR, DARTD1SPR, slide, false, false, 6, 0, 64, NULL, C_Lethal, R_Mshot, &s_dartdown2};
statetype s_dartdown2   = {DARTD2SPR, DARTD2SPR, slide, false, false, 6, 0, 64, NULL, C_Lethal, R_Mshot, &s_dartdown1};

/*
===========================
=
= SpawnDartShooter
=
===========================
*/

void SpawnDartShooter(Sint16 x, Sint16 y, Sint16 dir)
{
	GetNewObj(false);
	new->obclass = inertobj;
	new->active = ac_yes;
	new->x = CONVERT_TILE_TO_GLOBAL(x);
	new->needtoclip = cl_noclip;
	new->y = CONVERT_TILE_TO_GLOBAL(y);
	new->temp1 = dir;
	switch (dir)
	{
	case 0:
		new->y -= 3*PIXGLOBAL;
		new->x += 9*PIXGLOBAL;
		new->shapenum = DARTU1SPR;
		break;
	case 1:
		new->x += 8*PIXGLOBAL;
		new->y += 5*PIXGLOBAL;
		new->shapenum = DARTR1SPR;
		break;
	case 2:
		new->x += 9*PIXGLOBAL;
		new->shapenum = DARTD1SPR;
		break;
	case 3:
		new->y += 7*PIXGLOBAL;
		new->x -= 3*PIXGLOBAL;
		new->shapenum = DARTL1SPR;
		break;
	}
	NewState(new, &s_dartthrower);
}

/*
===========================
=
= T_DartShoot
=
===========================
*/

void T_DartShoot(objtype *ob)
{
	GetNewObj(true);
	new->x = ob->x;
	new->y = ob->y;
	new->obclass = mshotobj;
	new->active = ac_removable;
	switch (ob->temp1)
	{
	case 0:
		new->xdir = 0;
		new->ydir = -1;
		NewState(new, &s_dartup1);
		break;
	case 1:
		new->xdir = 1;
		new->ydir = 0;
		NewState(new, &s_dart1);
		break;
	case 2:
		new->xdir = 0;
		new->ydir = 1;
		NewState(new, &s_dartdown1);
		break;
	case 3:
		new->xdir = -1;
		new->ydir = 0;
		NewState(new, &s_dart1);
		break;
	}
	SD_PlaySound(SND_SHOOTDART);
}

/*
===========================
=
= R_DartThrower
=
===========================
*/

void R_DartThrower(objtype *ob)	//never used
{
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}

/*
=============================================================================

						  SCUBA GEAR

=============================================================================
*/
statetype s_scuba = {SCUBASPR, SCUBASPR, step, false, false, 30000, 0, 0, NULL, C_Scuba, R_Draw, &s_scuba};

/*
===========================
=
= SpawnScuba
=
===========================
*/

void SpawnScuba(Sint16 x, Sint16 y)
{
	GetNewObj(false);
	new->obclass = scubaobj;
	new->active = ac_yes;
	new->x = CONVERT_TILE_TO_GLOBAL(x);
	new->y = CONVERT_TILE_TO_GLOBAL(y) + -TILEGLOBAL;
	NewState(new, &s_scuba);
}

/*
===========================
=
= C_Scuba
=
===========================
*/

void C_Scuba(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj && hit->hitnorth)
	{
		gamestate.wetsuit = true;
		SD_PlaySound(SND_MAKEFOOT);
		GotScuba();
		RF_ForceRefresh();
		playstate = ex_completed;
		ob++;			// shut up compiler
	}
}