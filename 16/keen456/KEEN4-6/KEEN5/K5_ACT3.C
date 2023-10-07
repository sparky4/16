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
K5_ACT3.C
=========

Contains the following actor types (in this order):

- Shikadi Mine
- Robo Red
- Spirogrip
- Spindred
- Shikadi Master
- Shikadi
- Shockshund
- Sphereful
- Scottie
- QED

*/

#include "CK_DEF.H"

/*
=============================================================================

						  SHIKADI MINE

temp2 = x position of the "eye", in global units (relative to the mine sprite)
temp3 = y position of the "eye", in global units (relative to the mine sprite)
temp4 = sprite pointer for the "eye"

=============================================================================
*/

static Uint16 dopposite[] = {2, 3, 0, 1, 6, 7, 4, 5, 8};

statetype s_mine       = {SHIKADIMINESPR,       SHIKADIMINESPR,       think, false, false,  8, 0, 0, T_Mine, C_Solid, R_Mine, NULL};
statetype s_minecenter = {SHIKADIMINESPR,       SHIKADIMINESPR,       think, false, false,  0, 0, 0, T_MineCenter, C_Solid, R_Mine, &s_mineshift};
statetype s_mineshift  = {SHIKADIMINESPR,       SHIKADIMINESPR,       think, false, false,  0, 0, 0, T_MineShift, C_Solid, R_Mine, &s_mine};
statetype s_mineboom1  = {SHIKADIMINEPULSE1SPR, SHIKADIMINEPULSE1SPR, step,  false, false, 10, 0, 0, NULL, C_Solid, R_Draw, &s_mineboom2};
statetype s_mineboom2  = {SHIKADIMINEPULSE2SPR, SHIKADIMINEPULSE2SPR, step,  false, false, 10, 0, 0, NULL, C_Solid, R_Draw, &s_mineboom3};
statetype s_mineboom3  = {SHIKADIMINEPULSE1SPR, SHIKADIMINEPULSE1SPR, step,  false, false, 10, 0, 0, NULL, C_Solid, R_Draw, &s_mineboom4};
statetype s_mineboom4  = {SHIKADIMINEPULSE2SPR, SHIKADIMINEPULSE2SPR, step,  false, false, 10, 0, 0, T_MineFrag, C_Solid, R_Draw, &s_mineboom5};
statetype s_mineboom5  = {SHIKADIMINEBOOM1SPR,  SHIKADIMINEBOOM1SPR,  step,  false, false, 20, 0, 0, NULL, C_Spindread, R_Draw, &s_mineboom6};
statetype s_mineboom6  = {SHIKADIMINEBOOM2SPR,  SHIKADIMINEBOOM2SPR,  step,  false, false, 20, 0, 0, NULL, C_Spindread, R_Draw, NULL};
statetype s_minepiece  = {SHIKADIMINEPIECESPR,  SHIKADIMINEPIECESPR,  think, false, false,  8, 0, 0, T_Projectile, C_MineFrag, R_Bounce, NULL};

/*
===========================
=
= SpawnMine
=
===========================
*/

void SpawnMine(Uint16 tileX, Uint16 tileY)
{
	Sint16 i;

	GetNewObj(false);
	new->obclass = mineobj;
	new->active = ac_yes;
	new->needtoclip = cl_noclip;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY) + -(31*PIXGLOBAL + 1);
	new->temp2 = 16*PIXGLOBAL;
	new->temp3 = 13*PIXGLOBAL;
	NewState(new, &s_mineshift);
	new->xspeed = TILEGLOBAL;

	for (i=0; i<=3; i++)
	{
		if (Walk(new, i))
			break;
	}
}

/*
===========================
=
= MinePosCheck
=
===========================
*/

boolean MinePosCheck(Uint16 tileX, Uint16 tileY)
{
	Uint16 far *map;
	Uint16 x, y, tile;

	map = mapsegs[1] + mapbwidthtable[tileY]/2 + tileX;
	for (y=0; y<2; y++)
	{
		for (x=0; x<3; x++)
		{
			tile = *(map + y*mapwidth + x);
			if (tinf[tile+NORTHWALL] || tinf[tile+EASTWALL] || tinf[tile+SOUTHWALL] || tinf[tile+WESTWALL])
				return false;
		}
	}
	return true;
}

/*
===========================
=
= Walk
=
===========================
*/

boolean Walk(objtype *ob, Sint16 dir)
{
	Uint16 tileX, tileY;

	tileX = CONVERT_GLOBAL_TO_TILE(ob->x + xtry);
	tileY = CONVERT_GLOBAL_TO_TILE(ob->y + ytry);

	switch (dir)
	{
	case 0:
		if (MinePosCheck(tileX, tileY-1))
		{
			ob->xdir = 0;
			ob->ydir = -1;
			return true;
		}
		return false;
	case 1:
		if (MinePosCheck(tileX+1, tileY))
		{
			ob->xdir = 1;
			ob->ydir = 0;
			return true;
		}
		return false;
	case 2:
		if (MinePosCheck(tileX, tileY+1))
		{
			ob->xdir = 0;
			ob->ydir = 1;
			return true;
		}
		return false;
	case 3:
		if (MinePosCheck(tileX-1, tileY))
		{
			ob->xdir = -1;
			ob->ydir = 0;
			return true;
		}
		return false;
	default:
		Quit("Walk: Bad dir");
	}
	return false;
}

/*
===========================
=
= ChaseThink
=
===========================
*/

void ChaseThink(objtype *ob)
{
	Sint16 temp;
	Sint16 xdist, ydist, ydir, xdir;
	Sint16 olddir[2], oppdir;

	if (ob->xdir == 1)
	{
		olddir[0] = 1;
	}
	else if (ob->xdir == -1)
	{
		olddir[0] = 3;
	}
	else if (ob->ydir == -1)
	{
		olddir[0] = 0;
	}
	else if (ob->ydir == 1)
	{
		olddir[0] = 2;
	}
	oppdir = dopposite[olddir[0]];
	xdist = player->x - (ob->x + xtry);
	ydist = player->y - (ob->y + ytry);
	xdir = 8;
	ydir = 8;
	if (xdist > 0)
	{
		xdir = 1;
	}
	if (xdist < 0)
	{
		xdir = 3;
	}
	if (ydist > 0)
	{
		ydir = 2;
	}
	if (ydist < 0)
	{
		ydir = 0;
	}
	if (abs(ydist) > abs(xdist))
	{
		temp = xdir;
		xdir = ydir;
		ydir = temp;
	}
	if (xdir == oppdir)
	{
		xdir = 8;
	}
	if (ydir == oppdir)
	{
		ydir = 8;
	}
	if (ydir != 8 && Walk(ob, ydir))
	{
		return;
	}
	if (xdir != 8 && Walk(ob, xdir))
	{
		return;
	}
	if (Walk(ob, olddir[0]))
	{
		return;
	}
	if (US_RndT() > 0x80)
	{
		for (temp=0; temp<=3; temp++)
		{
			if (temp != oppdir && Walk(ob, temp))
				return;
		}
	}
	else
	{
		for (temp=3; temp>=0; temp--)
		{
			if (temp != oppdir && Walk(ob, temp))
				return;
		}
	}
	Walk(ob, oppdir);
}

/*
===========================
=
= T_Mine
=
===========================
*/

void T_Mine(objtype *ob)
{
	Sint16 oldxdir, oldydir;
	Sint16 xdist, ydist;
	Sint16 speed;

	xdist = ob->x - player->x;
	ydist = ob->y - player->y;
	if (xdist <= 2*TILEGLOBAL && xdist >= -5*TILEGLOBAL && ydist <= 3*TILEGLOBAL && ydist >= -5*PIXGLOBAL)
	{
		SD_PlaySound(SND_MINEEXPLODE);
		ob->state = &s_mineboom1;
		RF_RemoveSprite((void**)(&ob->temp4));
	}
	else
	{
		speed = tics * 10;
		if (ob->xspeed <= speed)
		{
			xtry = ob->xdir * ob->xspeed;
			ytry = ob->ydir * ob->xspeed;	// yes, this uses xspeed!
			speed -= ob->xspeed;
			oldxdir = ob->xdir;
			oldydir = ob->ydir;
			ChaseThink(ob);
			ob->xspeed = TILEGLOBAL;
			if (ob->xdir != oldxdir || ob->ydir != oldydir)
			{
				ob->state = &s_minecenter;
				return;
			}
		}
		ob->xspeed -= speed;
		xtry += ob->xdir * speed;
		ytry += ob->ydir * speed;
	}
}

/*
===========================
=
= C_Solid
=
===========================
*/

#pragma argsused
void C_Solid(objtype *ob, objtype *hit)
{
	if (hit->obclass == stunshotobj)
	{
		ExplodeShot(hit);
	}
}

/*
===========================
=
= C_MineFrag
=
===========================
*/

#pragma argsused
void C_MineFrag(objtype *ob, objtype *hit)
{
	if (hit->obclass == stunshotobj)
	{
		ExplodeShot(hit);
	}
	else if (hit->obclass == keenobj)
	{
		KillKeen();
	}
	else if (hit->obclass == qedobj)
	{
		SpawnFuseFlash(hit->tileleft, hit->tiletop);
		SpawnFuseFlash(hit->tileright, hit->tiletop);
		SpawnFuseFlash(hit->tileleft, hit->tilebottom);
		SpawnFuseFlash(hit->tileright, hit->tilebottom);
		RF_MapToMap(0, 0, 16, 11, 4, 2);
		RF_MapToMap(4, 0, 16, 13, 4, 2);
		SpawnDeadMachine();
		RemoveObj(hit);
	}
}

/*
===========================
=
= T_MineCenter
=
===========================
*/

void T_MineCenter(objtype *ob)
{
	Sint16 px, py, xdist, ydist;

	xdist = ob->x - player->x;
	ydist = ob->y - player->y;

	if (xdist <= 2*TILEGLOBAL && xdist >= -3*TILEGLOBAL && ydist <= 3*TILEGLOBAL && ydist >= -3*TILEGLOBAL)
	{
		//BUG? this doesn't play a sound
		ob->state = &s_mineboom1;
		RF_RemoveSprite((void**)&ob->temp4);
	}
	else
	{
		ob->needtoreact = true;

		px = 16*PIXGLOBAL;
		py = 13*PIXGLOBAL;

		if (ob->temp2 < px)
		{
			ob->temp2 = ob->temp2 + tics*4;
			if (ob->temp2 >= px)
			{
				ob->temp2 = px;
				ob->state = ob->state->nextstate;
			}
		}
		else if (ob->temp2 > px)
		{
			ob->temp2 = ob->temp2 - tics*4;
			if (ob->temp2 <= px)
			{
				ob->temp2 = px;
				ob->state = ob->state->nextstate;
			}
		}
		if (ob->temp3 < py)
		{
			ob->temp3 = ob->temp3 + tics*4;
			if (ob->temp3 >= py)
			{
				ob->temp3 = py;
				ob->state = ob->state->nextstate;
			}
		}
		else if (ob->temp3 > py)
		{
			ob->temp3 = ob->temp3 - tics*4;
			if (ob->temp3 <= py)
			{
				ob->temp3 = py;
				ob->state = ob->state->nextstate;
			}
		}
	}
}

/*
===========================
=
= T_MineShift
=
===========================
*/

void T_MineShift(objtype *ob)
{
	Sint16 px, py, xdist, ydist;

	xdist = ob->x - player->x;
	ydist = ob->y - player->y;

	if (xdist <= 2*TILEGLOBAL && xdist >= -3*TILEGLOBAL && ydist <= 3*TILEGLOBAL && ydist >= -3*TILEGLOBAL)
	{
		//BUG? this doesn't play a sound
		ob->state = &s_mineboom1;
		RF_RemoveSprite((void**)&ob->temp4);
	}
	else
	{
		ob->needtoreact = true;

		switch (ob->xdir)
		{
		case -1:
			px = 8*PIXGLOBAL;
			break;
		case 0:
			px = 16*PIXGLOBAL;
			break;
		case 1:
			px = 24*PIXGLOBAL;
		}
		switch (ob->ydir)
		{
		case -1:
			py = 5*PIXGLOBAL;
			break;
		case 0:
			py = 13*PIXGLOBAL;
			break;
		case 1:
			py = 21*PIXGLOBAL;
		}

		if (ob->temp2 < px)
		{
			ob->temp2 = ob->temp2 + tics*4;
			if (ob->temp2 >= px)
			{
				ob->temp2 = px;
				ob->state = ob->state->nextstate;
			}
		}
		else if (ob->temp2 > px)
		{
			ob->temp2 = ob->temp2 - tics*4;
			if (ob->temp2 <= px)
			{
				ob->temp2 = px;
				ob->state = ob->state->nextstate;
			}
		}
		if (ob->temp3 < py)
		{
			ob->temp3 = ob->temp3 + tics*4;
			if (ob->temp3 >= py)
			{
				ob->temp3 = py;
				ob->state = ob->state->nextstate;
			}
		}
		else if (ob->temp3 > py)
		{
			ob->temp3 = ob->temp3 - tics*4;
			if (ob->temp3 <= py)
			{
				ob->temp3 = py;
				ob->state = ob->state->nextstate;
			}
		}
	}
}

/*
===========================
=
= T_MineFrag
=
===========================
*/

void T_MineFrag(objtype *ob)
{
	SD_PlaySound(SND_MINEEXPLODE);

	GetNewObj(true);
	new->x = ob->x;
	new->y = ob->y;
	new->xspeed = -(US_RndT()>>3);
	new->yspeed = -48;
	NewState(new, &s_minepiece);

	GetNewObj(true);
	new->x = ob->x + TILEGLOBAL;
	new->y = ob->y;
	new->xspeed = (US_RndT()>>3);
	new->yspeed = -48;
	NewState(new, &s_minepiece);

	GetNewObj(true);
	new->x = ob->x;
	new->y = ob->y;
	new->xspeed = (US_RndT()>>4) + 40;
	new->yspeed = -24;
	NewState(new, &s_minepiece);

	GetNewObj(true);
	new->x = ob->x + TILEGLOBAL;
	new->y = ob->y;
	new->xspeed = -40 - (US_RndT()>>4);
	new->yspeed = -24;
	NewState(new, &s_minepiece);

	GetNewObj(true);
	new->x = ob->x;
	new->y = ob->y;
	new->xspeed = 24;
	new->yspeed = 16;
	NewState(new, &s_minepiece);

	GetNewObj(true);
	new->x = ob->x + TILEGLOBAL;
	new->y = ob->y;
	new->xspeed = 24;
	new->yspeed = 16;
	NewState(new, &s_minepiece);
}

/*
===========================
=
= R_Mine
=
===========================
*/

void R_Mine(objtype *ob)
{
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
	RF_PlaceSprite((void**)&ob->temp4, ob->x+ob->temp2, ob->y+ob->temp3, SHIKADIMINEEYESPR, spritedraw, 2);
}

/*
=============================================================================

						  ROBO RED

temp1 = number of shots to fire

=============================================================================
*/

statetype s_robored      = {ROBOREDLSPR,     ROBOREDRSPR,     step,      false, true,  6, 64, 0, T_RoboRed, C_RoboRed, R_Walk, &s_robored};
statetype s_roboredfire0 = {ROBOREDLSPR,     ROBOREDRSPR,     step,      true,  true, 40,  0, 0, NULL, C_Spindread, R_Draw, &s_roboredfire1};
statetype s_roboredfire1 = {ROBOREDLSPR,     ROBOREDRSPR,     step,      true,  true,  4, 64, 0, NULL, C_Spindread, R_Draw, &s_roboredfire2};
statetype s_roboredfire2 = {ROBOREDLSPR,     ROBOREDRSPR,     step,      false, true,  6,  0, 0, T_RoboShoot, C_Spindread, R_Draw, &s_roboredfire1};
statetype s_rshot1       = {ROBOSHOT1SPR,    ROBOSHOT1SPR,    stepthink, false, false,  8,  0, 0, T_Velocity, C_RShot, R_RShot, &s_rshot2};
statetype s_rshot2       = {ROBOSHOT2SPR,    ROBOSHOT2SPR,    stepthink, false, false,  8,  0, 0, T_Velocity, C_RShot, R_RShot, &s_rshot1};
statetype s_rshothit1    = {ROBOSHOTHIT1SPR, ROBOSHOTHIT1SPR, step,      false, false, 10,  0, 0, NULL, NULL, R_Draw, &s_rshothit2};
statetype s_rshothit2    = {ROBOSHOTHIT2SPR, ROBOSHOTHIT2SPR, step,      false, false, 10,  0, 0, NULL, NULL, R_Draw, NULL};

/*
===========================
=
= SpawnRoboRed
=
===========================
*/

void SpawnRoboRed(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->obclass = roboredobj;
	new->active = ac_yes;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY) + -4*TILEGLOBAL;
	if (US_RndT() < 0x80)
	{
		new->xdir = 1;
	}
	else
	{
		new->xdir = -1;
	}
	NewState(new, &s_robored);
}

/*
===========================
=
= T_RoboRed
=
===========================
*/

void T_RoboRed(objtype *ob)
{
	if (!(ob->x & (4*PIXGLOBAL)) && player->bottom > ob->top && player->top < ob->bottom && US_RndT() < 16)
	{
		if (ob->x > player->x)
		{
			ob->xdir = -1;
		}
		else
		{
			ob->xdir = 1;
		}
		ob->temp1 = 10;	// shoot 10 times
		ob->state = &s_roboredfire0;
	}
}

/*
===========================
=
= C_RoboRed
=
===========================
*/

void C_RoboRed(objtype *ob, objtype *hit)
{
	if (hit->obclass == stunshotobj)
	{
		ExplodeShot(hit);
		ob->xdir = (player->x > ob->x)? 1 : -1;
		ob->temp1 = 10;	// shoot 10 times
		ChangeState(ob, &s_roboredfire0);
	}
	else if (hit->obclass == keenobj)
	{
		KillKeen();
	}
}

/*
===========================
=
= T_RoboShoot
=
===========================
*/

void T_RoboShoot(objtype *ob)
{
	Uint16 x;

	if (--ob->temp1 == 0)
	{
		ob->state = &s_robored;
	}
	if (ob->xdir == 1)
	{
		x = ob->x + 56*PIXGLOBAL;
	}
	else
	{
		x = ob->x;
	}
	if (CheckSpawnShot(x, ob->y + 32*PIXGLOBAL, &s_rshot1) != -1)
	{
		new->xspeed = ob->xdir * 60;
		if (ob->temp1 & 1)
		{
			new->yspeed = 8;
		}
		else
		{
			new->yspeed = -8;
		}
		SD_PlaySound(SND_ENEMYSHOT);
		xtry = (ob->xdir == 1)? -4*PIXGLOBAL : 4*PIXGLOBAL;
	}
}

/*
===========================
=
= C_RShot
=
===========================
*/

void C_RShot(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		KillKeen();
		ChangeState(ob, &s_rshothit1);
	}
}

/*
===========================
=
= R_RShot
=
===========================
*/

void R_RShot(objtype *ob)
{
	if (ob->hitnorth || ob->hiteast || ob->hitsouth || ob->hitwest)
	{
		SD_PlaySound(SND_ENEMYSHOTEXPLODE);
		ChangeState(ob, &s_rshothit1);
	}
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}

/*
=============================================================================

						  SPIROGRIP

=============================================================================
*/

statetype s_gripsitd  = {SPIROSITDSPR,   SPIROSITDSPR,   step,  false, false, 150,   0,   0, NULL, C_Spindread, R_Draw, &s_gripjumpd};
statetype s_gripjumpd = {SPIROSITDSPR,   SPIROSITDSPR,   slide, false, false,  64,   0, -16, NULL, C_Spindread, R_Draw, &s_gripspin7};
statetype s_gripsitl  = {SPIROSITLSPR,   SPIROSITLSPR,   step,  false, false, 150,   0,   0, NULL, C_Spindread, R_Draw, &s_gripjumpl};
statetype s_gripjumpl = {SPIROSITLSPR,   SPIROSITLSPR,   slide, false, false,  64,  16,   0, NULL, C_Spindread, R_Draw, &s_gripspin1};
statetype s_gripsitr  = {SPIROSITRSPR,   SPIROSITRSPR,   step,  false, false, 150,   0,   0, NULL, C_Spindread, R_Draw, &s_gripjumpr};
statetype s_gripjumpr = {SPIROSITRSPR,   SPIROSITRSPR,   slide, false, false,  64, -16,   0, NULL, C_Spindread, R_Draw, &s_gripspin5};
statetype s_gripsitu  = {SPIROSITUSPR,   SPIROSITUSPR,   step,  false, false, 150,   0,   0, NULL, C_Spindread, R_Draw, &s_gripjumpu};
statetype s_gripjumpu = {SPIROSITUSPR,   SPIROSITUSPR,   slide, false, false,  64,   0,  16, NULL, C_Spindread, R_Draw, &s_gripspin3};
statetype s_gripspin1 = {SPIROSPINULSPR, SPIROSPINULSPR, step,  false, false,   8,   0,   0, NULL, C_Spindread, R_Draw, &s_gripspin2};
statetype s_gripspin2 = {SPIROSPINUSPR,  SPIROSPINUSPR,  step,  false, false,   8,   0,   0, T_SpiroLaunch, C_Spindread, R_Draw, &s_gripspin3};
statetype s_gripspin3 = {SPIROSPINURSPR, SPIROSPINURSPR, step,  false, false,   8,   0,   0, NULL, C_Spindread, R_Draw, &s_gripspin4};
statetype s_gripspin4 = {SPIROSPINRSPR,  SPIROSPINRSPR,  step,  false, false,   8,   0,   0, T_SpiroLaunch, C_Spindread, R_Draw, &s_gripspin5};
statetype s_gripspin5 = {SPIROSPINDRSPR, SPIROSPINDRSPR, step,  false, false,   8,   0,   0, NULL, C_Spindread, R_Draw, &s_gripspin6};
statetype s_gripspin6 = {SPIROSPINDSPR,  SPIROSPINDSPR,  step,  false, false,   8,   0,   0, T_SpiroLaunch, C_Spindread, R_Draw, &s_gripspin7};
statetype s_gripspin7 = {SPIROSPINDLSPR, SPIROSPINDLSPR, step,  false, false,   8,   0,   0, NULL, C_Spindread, R_Draw, &s_gripspin8};
statetype s_gripspin8 = {SPIROSPINLSPR,  SPIROSPINLSPR,  step,  false, false,   8,   0,   0, T_SpiroLaunch, C_Spindread, R_Draw, &s_gripspin1};
statetype s_gripflyd  = {SPIROSITDSPR,   SPIROSITDSPR,   slide, false, false,   0,   0,  48, NULL, C_Spindread, R_SpiroFly, &s_gripsitd};
statetype s_gripflyl  = {SPIROSITLSPR,   SPIROSITLSPR,   slide, false, false,   0, -48,   0, NULL, C_Spindread, R_SpiroFly, &s_gripsitl};
statetype s_gripflyr  = {SPIROSITRSPR,   SPIROSITRSPR,   slide, false, false,   0,  48,   0, NULL, C_Spindread, R_SpiroFly, &s_gripsitr};
statetype s_gripflyu  = {SPIROSITUSPR,   SPIROSITUSPR,   slide, false, false,   0,   0, -48, NULL, C_Spindread, R_SpiroFly, &s_gripsitu};

/*
===========================
=
= SpawnSpirogrip
=
===========================
*/

void SpawnSpirogrip(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->obclass = spirogripobj;
	new->active = ac_yes;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY) + -1*TILEGLOBAL;
	new->xdir = 1;
	new->ydir = 1;
	NewState(new, &s_gripsitd);
}

/*
===========================
=
= T_SpiroLaunch
=
===========================
*/

void T_SpiroLaunch(objtype *ob)
{
	if (US_RndT() <= 20)
	{
		SD_PlaySound(SND_SPIROFLY);
		if (ob->state == &s_gripspin2)
		{
			ob->state = &s_gripflyu;
		}
		else if (ob->state == &s_gripspin4)
		{
			ob->state = &s_gripflyr;
		}
		else if (ob->state == &s_gripspin6)
		{
			ob->state = &s_gripflyd;
		}
		else if (ob->state == &s_gripspin8)
		{
			ob->state = &s_gripflyl;
		}
	}
}

/*
===========================
=
= R_SpiroFly
=
===========================
*/

void R_SpiroFly(objtype *ob)
{
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
	if (ob->hitnorth || ob->hiteast || ob->hitsouth || ob->hitwest)
	{
		ChangeState(ob, ob->state->nextstate);
		SD_PlaySound(SND_SPIROGRAB);
	}
}

/*
=============================================================================

						  SPINDRED

temp1 = bounce counter

=============================================================================
*/

statetype s_spindred1     = {SPINDRED1SPR, SPINDRED1SPR, stepthink, false, false, 8, 0, 0, T_Spindread, C_Spindread, R_Spindread, &s_spindred2};
statetype s_spindred2     = {SPINDRED2SPR, SPINDRED2SPR, stepthink, false, false, 8, 0, 0, T_Spindread, C_Spindread, R_Spindread, &s_spindred3};
statetype s_spindred3     = {SPINDRED3SPR, SPINDRED3SPR, stepthink, false, false, 8, 0, 0, T_Spindread, C_Spindread, R_Spindread, &s_spindred4};
statetype s_spindred4     = {SPINDRED4SPR, SPINDRED4SPR, stepthink, false, false, 8, 0, 0, T_Spindread, C_Spindread, R_Spindread, &s_spindred1};

/*
===========================
=
= SpawnSpindread
=
===========================
*/

void SpawnSpindread(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->obclass = spindredobj;
	new->active = ac_yes;
	new->priority = 0;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY) + -8*PIXGLOBAL;
	new->ydir = 1;
	NewState(new, &s_spindred1);
}

/*
===========================
=
= T_Spindread
=
===========================
*/

void T_Spindread(objtype *ob)
{
	Sint32 i;

	// BUG: this might be executed twice during the same frame if the
	// object's animation/state changed during that frame, causing the
	// object to move at twice the speed during that frame!
	// To avoid that, return if ytry is not 0.

	for (i=lasttimecount-tics; i<lasttimecount; i++)
	{
		if (i & 1)
		{
			if (ob->ydir == 1)
			{
				if (ob->yspeed < 0 && ob->yspeed >= -3)
				{
					ytry += ob->yspeed;
					ob->yspeed = 0;
					return;
				}
				else
				{
					ob->yspeed += 3;
					if (ob->yspeed > 70)
					{
						ob->yspeed = 70;
					}
				}
			}
			else
			{
				if (ob->yspeed > 0 && ob->yspeed <= 3)
				{
					ytry += ob->yspeed;
					ob->yspeed = 0;
					return;
				}
				else
				{
					ob->yspeed -= 3;
					if (ob->yspeed < -70)
					{
						ob->yspeed = -70;
					}
				}
			}
		}
		ytry += ob->yspeed;
	}
}

/*
===========================
=
= C_Spindread
=
===========================
*/

#pragma argsused
void C_Spindread(objtype *ob, objtype *hit)
{
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
= R_Spindread
=
===========================
*/

void R_Spindread(objtype *ob)
{
	if (ob->hitsouth)
	{
		ob->yspeed = 0;
		if (ob->ydir == -1)
		{
			if (++ob->temp1 == 3)
			{
				ob->temp1 = 0;
				ob->yspeed = 68;
				ob->ydir = -ob->ydir;
				SD_PlaySound(SND_SPINDREDFLIP);
			}
			else
			{
				SD_PlaySound(SND_SPINDREDBOUNCE);
				ob->yspeed = 40;
			}
		}
	}
	if (ob->hitnorth)
	{
		ob->yspeed = 0;
		if (ob->ydir == 1)
		{
			if (++ob->temp1 == 3)
			{
				ob->temp1 = 0;
				ob->yspeed = -68;
				ob->ydir = -ob->ydir;
				SD_PlaySound(SND_BOUNCE);
			}
			else
			{
				SD_PlaySound(SND_SPINDREDBOUNCE);
				ob->yspeed = -40;
			}
		}
	}
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}

/*
=============================================================================

						  SHIKADI MASTER

temp1 = defines next action (0 = shoot, 1 = teleport)

=============================================================================
*/

statetype s_master1      = {MASTER1SPR,           MASTER1SPR,           step,      false, false,  8, 0, 0, NULL, C_Master, R_Draw, &s_master2};
statetype s_master2      = {MASTER2SPR,           MASTER2SPR,           step,      false, false,  8, 0, 0, NULL, C_Master, R_Draw, &s_master3};
statetype s_master3      = {MASTER3SPR,           MASTER3SPR,           step,      false, false,  8, 0, 0, NULL, C_Master, R_Draw, &s_master4};
statetype s_master4      = {MASTER4SPR,           MASTER4SPR,           step,      false, false,  8, 0, 0, T_Master, C_Master, R_Draw, &s_master1};
statetype s_mastershoot1 = {SHIKMASTERCASTLSPR,   SHIKMASTERCASTRSPR,   step,      false, false, 30, 0, 0, T_MasterShoot, C_Spindread, R_Draw, &s_mastershoot2};
statetype s_mastershoot2 = {SHIKMASTERCASTLSPR,   SHIKMASTERCASTRSPR,   step,      false, false,  8, 0, 0, NULL, C_Spindread, R_Draw, &s_master1};
statetype s_mastertport1 = {MASTERTELEPORT1SPR,   MASTERTELEPORT1SPR,   step,      false, true,  20, 0, 0, NULL, C_Spindread, R_Draw, &s_mastertport2};
statetype s_mastertport2 = {MASTERTELEPORT2SPR,   MASTERTELEPORT2SPR,   step,      false, true,  20, 0, 0, T_MasterTPort, C_Spindread, R_Draw, &s_mastertport3};
statetype s_mastertport3 = {MASTERTELEPORT2SPR,   MASTERTELEPORT2SPR,   think,     false, false,  0, 0, 0, T_Projectile, NULL, R_Land, &s_mastertport4};
statetype s_mastertport4 = {MASTERTELEPORT2SPR,   MASTERTELEPORT2SPR,   step,      false, false, 60, 0, 0, NULL, C_Spindread, R_Draw, &s_master1};
statetype s_mshot1       = {MASTERSHOT4SPR,       MASTERSHOT1SPR,       stepthink, false, false,  8, 0, 0, T_WeakProjectile, C_MShot, R_MShot, &s_mshot2};
statetype s_mshot2       = {MASTERSHOT3SPR,       MASTERSHOT2SPR,       stepthink, false, false,  8, 0, 0, T_WeakProjectile, C_MShot, R_MShot, &s_mshot3};
statetype s_mshot3       = {MASTERSHOT2SPR,       MASTERSHOT3SPR,       stepthink, false, false,  8, 0, 0, T_WeakProjectile, C_MShot, R_MShot, &s_mshot4};
statetype s_mshot4       = {MASTERSHOT1SPR,       MASTERSHOT4SPR,       stepthink, false, false,  8, 0, 0, T_WeakProjectile, C_MShot, R_MShot, &s_mshot1};
statetype s_mspray1      = {MASTERFLOORSPARK1SPR, MASTERFLOORSPARK1SPR, stepthink, false, false,  6, 0, 0, T_Projectile, C_MShot, R_MSpray, &s_mspray2};
statetype s_mspray2      = {MASTERFLOORSPARK2SPR, MASTERFLOORSPARK2SPR, stepthink, false, false,  6, 0, 0, T_Projectile, C_MShot, R_MSpray, &s_mspray3};
statetype s_mspray3      = {MASTERFLOORSPARK3SPR, MASTERFLOORSPARK3SPR, stepthink, false, false,  6, 0, 0, T_Projectile, C_MShot, R_MSpray, &s_mspray4};
statetype s_mspray4      = {MASTERFLOORSPARK4SPR, MASTERFLOORSPARK4SPR, stepthink, false, false,  6, 0, 0, T_Projectile, C_MShot, R_MSpray, &s_mspray1};

/*
===========================
=
= SpawnMaster
=
===========================
*/

void SpawnMaster(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->obclass = shikadimasterobj;
	new->active = ac_yes;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY) + -24*PIXGLOBAL;
	NewState(new, &s_master1);
}

/*
===========================
=
= T_Master
=
===========================
*/

void T_Master(objtype *ob)
{
	Sint16 randval;

	randval = US_RndT();
	if (randval < 0x40)
	{
		if (ob->temp1)
		{
			ob->state = &s_mastertport1;
			ob->temp1 = 0;
		}
		else
		{
			ob->temp1 = 1;
			if (player->x > ob->x)
			{
				ob->xdir = 1;
			}
			else
			{
				ob->xdir = -1;
			}
			ob->state = &s_mastershoot1;
		}
	}
}

/*
===========================
=
= T_MasterShoot
=
===========================
*/

void T_MasterShoot(objtype *ob)
{
	Uint16 x;

	if (ob->xdir == 1)
	{
		x = ob->x + 16*PIXGLOBAL;
	}
	else
	{
		x = ob->x;
	}
	if (CheckSpawnShot(x, ob->y+8*PIXGLOBAL, &s_mshot1) != -1)
	{
		new->xspeed = ob->xdir * 48;
		new->yspeed = -16;
		SD_PlaySound(SND_MASTERATTACK);
	}
}

/*
===========================
=
= C_Master
=
===========================
*/

void C_Master(objtype *ob, objtype *hit)
{
	if (hit->obclass == stunshotobj)
	{
		ExplodeShot(hit);
		ob->xdir = (player->x > ob->x)? 1 : -1;
		ob->temp1 = 1;
		ChangeState(ob, &s_mastershoot1);
	}
	else if (hit->obclass == keenobj)
	{
		KillKeen();
	}
}

/*
===========================
=
= T_MasterTPort
=
===========================
*/

void T_MasterTPort(objtype *ob)
{
	Uint16 tile;
	Sint16 tx, ty, redos, oldx, oldy;

	oldx = ob->x;
	oldy = ob->y;

	GetNewObj(true);
	new->x = ob->x;
	new->y = ob->y;
	new->xspeed = 48;
	NewState(new, &s_mspray1);	// BUG? new object is not made removable

	GetNewObj(true);
	new->x = ob->x;
	new->y = ob->y;
	new->xspeed = -48;
	NewState(new, &s_mspray1);	// BUG? new object is not made removable

	SD_PlaySound(SND_MASTERBLAST);

	redos = 0;
redo:
	if (++redos == 10)
	{
		US_RndT();	// call it, but ignore the result
		// probably to avoid repeatedly getting the same same "random" values
		// due to having an even number of US_RndT() calls in this routine and
		// an even number of elements in the random table.

		ob->state = &s_master1;
		ob->x = oldx - 1;
		ob->y = oldy;
		xtry = 1;
		ytry = 0;
	}
	else
	{
		tx = player->tilemidx - 16 + (US_RndT()>>3);
		ty = player->tilebottom - 16 + (US_RndT()>>3);
		if (tx < 2 || ty < 2 || tx > mapwidth-5 || ty > mapheight-5)
			goto redo;


		ob->x = CONVERT_TILE_TO_GLOBAL(tx);
		ob->y = CONVERT_TILE_TO_GLOBAL(ty);
		ob->tileleft = tx-1;
		ob->tileright = tx+4;
		ob->tiletop = ty-1;
		ob->tilebottom = ty+4;

		{
			Uint16 x, y;
			Uint16 far *map;
			Uint16 mapdelta;

			map = (Uint16 far *)mapsegs[1] + mapbwidthtable[ob->tiletop]/2 + ob->tileleft;
			mapdelta = mapwidth - (ob->tileright - ob->tileleft + 1);

			for (y = ob->tiletop; ob->tilebottom >= y; y++, map+=mapdelta)
			{
				for (x = ob->tileleft; ob->tileright >= x; x++)
				{
					tile = *map++;
					if ((tinf[tile+INTILE] & INTILE_FOREGROUND) || tinf[tile+NORTHWALL] || tinf[tile+EASTWALL]
						|| tinf[tile+SOUTHWALL] || tinf[tile+WESTWALL])
					{
						goto redo;
					}
				}
			}
			xtry = ytry = 0;
		}
	}
}

/*
===========================
=
= C_MShot
=
===========================
*/

void C_MShot(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		KillKeen();
		RemoveObj(ob);
	}
	else if (hit->obclass == stunshotobj)
	{
		ExplodeShot(hit);
		RemoveObj(ob);
	}
}

/*
===========================
=
= R_MShot
=
===========================
*/

void R_MShot(objtype *ob)
{
	if (ob->hiteast || ob->hitwest)
	{
		ob->xspeed = -ob->xspeed;
	}
	if (ob->hitsouth)
	{
		ob->yspeed = 0;
	}
	if (ob->hitnorth)
	{
		SD_PlaySound(SND_MASTERATTACK);
		ob->xspeed = 48;
		ChangeState(ob, &s_mspray1);

		GetNewObj(true);
		new->x = ob->x;
		new->y = ob->y;
		new->xspeed = -48;
		NewState(new, &s_mspray1);	// BUG? new object is not made removable
	}
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}

/*
===========================
=
= R_MSpray
=
===========================
*/

void R_MSpray(objtype *ob)
{
	if (ob->hiteast || ob->hitwest)
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

						  SHIKADI

temp1 = x tile position of the pole being grabbed (is set but never used)
temp2 = health
temp3 = flash countdown

=============================================================================
*/

statetype s_shikadi1     = {SHIKADI1SPR,      SHIKADI1SPR,      step,  false, true,  8,   0, 0, NULL, C_Shikadi, R_Shikadi, &s_shikadi2};
statetype s_shikadi2     = {SHIKADI2SPR,      SHIKADI2SPR,      step,  false, true,  8,   0, 0, NULL, C_Shikadi, R_Shikadi, &s_shikadi3};
statetype s_shikadi3     = {SHIKADI3SPR,      SHIKADI3SPR,      step,  false, true,  8,   0, 0, NULL, C_Shikadi, R_Shikadi, &s_shikadi4};
statetype s_shikadi4     = {SHIKADI4SPR,      SHIKADI4SPR,      step,  false, true,  8,   0, 0, NULL, C_Shikadi, R_Shikadi, &s_shikadiwalk1};
statetype s_shikadiwalk1 = {SHIKADIWALKL1SPR, SHIKADIWALKR1SPR, step,  false, true,  8, 128, 0, T_Shikadi, C_Shikadi, R_Shikadi, &s_shikadiwalk2};
statetype s_shikadiwalk2 = {SHIKADIWALKL2SPR, SHIKADIWALKR2SPR, step,  false, true,  8, 128, 0, T_Shikadi, C_Shikadi, R_Shikadi, &s_shikadiwalk3};
statetype s_shikadiwalk3 = {SHIKADIWALKL3SPR, SHIKADIWALKR3SPR, step,  false, true,  8, 128, 0, T_Shikadi, C_Shikadi, R_Shikadi, &s_shikadiwalk4};
statetype s_shikadiwalk4 = {SHIKADIWALKL4SPR, SHIKADIWALKR4SPR, step,  false, true,  8, 128, 0, T_Shikadi, C_Shikadi, R_Shikadi, &s_shikadiwalk1};
statetype s_shikadigrab  = {SHIKADIGRABLSPR,  SHIKADIGRABRSPR,  step,  false, true, 20,   0, 0, T_PoleShock, C_Shikadi, R_Shikadi, &s_shikadigrab2};
statetype s_shikadigrab2 = {SHIKADIGRABLSPR,  SHIKADIGRABRSPR,  step,  false, true, 20,   0, 0, NULL, C_Shikadi, R_Shikadi, &s_shikadiwalk1};
statetype s_shikadistun  = {SHIKADISTUNSPR,   SHIKADISTUNSPR,   think, false, false, 0,   0, 0, T_Projectile, NULL, R_Stunned, NULL};

statetype s_polespark1   = {SHIKADIPOLESPARK1SPR, SHIKADIPOLESPARK1SPR, stepthink, false, false,  0,   0, 0, T_PoleSpark, C_Lethal, R_Draw, &s_polespark2};
statetype s_polespark2   = {SHIKADIPOLESPARK1SPR, SHIKADIPOLESPARK1SPR, stepthink, false, false,  0,   0, 0, T_PoleSpark, C_Lethal, R_Draw, &s_polespark1};

/*
===========================
=
= SpawnShikadi
=
===========================
*/

void SpawnShikadi(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->obclass = shikadiobj;
	new->active = ac_yes;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY) + -1*TILEGLOBAL;
	new->temp2 = 4;	// health
	if (US_RndT() < 0x80)
	{
		new->xdir = 1;
	}
	else
	{
		new->xdir = -1;
	}
	NewState(new, &s_shikadi1);
}

/*
===========================
=
= T_Shikadi
=
===========================
*/

void T_Shikadi(objtype *ob)
{
	Uint16 tx, tile;

	if (player->state->contact == &KeenPosContact
		|| ob->bottom - player->bottom + TILEGLOBAL <= 2*TILEGLOBAL)
	{
		if (ob->x > player->x + TILEGLOBAL)
		{
			ob->xdir = -1;
		}
		else if (ob->x < player->x - TILEGLOBAL)
		{
			ob->xdir = 1;
		}
		if (ob->xdir == 1)
		{
			tx = ob->tileright;
		}
		else
		{
			tx = ob->tileleft;
		}

		if (player->tilemidx != tx)
			return;
	}
	else
	{
		if (US_RndT() < 0x10)
		{
			xtry = 0;
			ob->state = &s_shikadi1;
			return;
		}
		if ((ob->x & 0xFF) || !OnScreen(ob))
			return;

		if (ob->xdir == 1)
		{
			tx = ob->tileright;
		}
		else
		{
			tx = ob->tileleft;
		}
	}

	tile = *(mapsegs[1]+mapbwidthtable[ob->tiletop]/2 + tx);
	if (tinf[tile+INTILE] == INTILE_POLE)
	{
		ob->temp1 = tx;
		ob->state = &s_shikadigrab;
		xtry = 0;
		SD_PlaySound(SND_SHIKADIATTACK);
	}
}

/*
===========================
=
= C_Shikadi
=
===========================
*/

void C_Shikadi(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		KillKeen();
	}
	if (hit->obclass == stunshotobj)
	{
		if (--ob->temp2 == 0)	// handle health
		{
			ob->xspeed = 0;
			ob->yspeed = 0;
			StunObj(ob, hit, &s_shikadistun);
		}
		else
		{
			ob->temp3 = 2;	// draw white two times
			ob->needtoreact = true;
			ExplodeShot(hit);
		}
	}
}

/*
===========================
=
= T_PoleShock
=
===========================
*/

void T_PoleShock(objtype *ob)
{
	Uint16 x;

	ob->nothink = 2;
	if (ob->xdir == 1)
	{
		x = CONVERT_TILE_TO_GLOBAL(ob->tileright);
	}
	else
	{
		x = CONVERT_TILE_TO_GLOBAL(ob->tileleft);
	}

	GetNewObj(true);
	new->x = x;
	new->y = ob->y + 8*PIXGLOBAL;
	new->obclass = mshotobj;
	new->active = ac_removable;
	new->needtoclip = cl_noclip;
	NewState(new, &s_polespark1);
	if (ob->y < player->y)
	{
		new->ydir = 1;
	}
	else
	{
		new->ydir = -1;
	}
	SD_PlaySound(SND_SHIKADIATTACK);
}

/*
===========================
=
= T_PoleSpark
=
===========================
*/

void T_PoleSpark(objtype *ob)
{
	Uint16 tile;

	if (ytry == 0)
	{
		ytry = ob->ydir * 48;
		tile = *(mapsegs[1]+mapbwidthtable[ob->tiletop]/2 + ob->tilemidx);
		if (tinf[tile+INTILE] != INTILE_POLE)
		{
			ob->state = NULL;
		}
	}
}

/*
===========================
=
= R_Shikadi
=
===========================
*/

void R_Shikadi(objtype *ob)
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
	if (ob->temp3)
	{
		ob->temp3--;
		RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, maskdraw, ob->priority);
	}
	else
	{
		RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
	}
}

/*
=============================================================================

						  PET (a.k.a. SHOCKSHUND)

temp1 = countdown for sit animation
temp2 = health
temp3 = flash countdown

=============================================================================
*/

statetype s_petsit1   = {PETSIT1SPR,      PETSIT1SPR,      step,      false, true,   8,   0, 0, NULL, C_Pet, R_Pet, &s_petsit2};
statetype s_petsit2   = {PETSIT2SPR,      PETSIT2SPR,      step,      false, true,   8,   0, 0, T_PetSit, C_Pet, R_Pet, &s_petsit1};
statetype s_petbark1  = {PETBARKL1SPR,    PETBARKR1SPR,    step,      false, true,   8,   0, 0, NULL, C_Pet, R_Pet, &s_petbark2};
statetype s_petbark2  = {PETBARKL2SPR,    PETBARKR2SPR,    step,      false, true,   8,   0, 0, T_PetBark, C_Pet, R_Pet, &s_pet1};
statetype s_pet1      = {PETRUNL1SPR,     PETRUNR1SPR,     step,      false, true,   8, 128, 0, NULL, C_Pet, R_Pet, &s_pet2};
statetype s_pet2      = {PETRUNL2SPR,     PETRUNR2SPR,     step,      false, true,   8, 128, 0, NULL, C_Pet, R_Pet, &s_pet3};
statetype s_pet3      = {PETRUNL3SPR,     PETRUNR3SPR,     step,      false, true,   8, 128, 0, NULL, C_Pet, R_Pet, &s_pet4};
statetype s_pet4      = {PETRUNL4SPR,     PETRUNR4SPR,     step,      false, true,   8, 128, 0, T_Pet, C_Pet, R_Pet, &s_pet1};
statetype s_petjump   = {PETJUMPLSPR,     PETJUMPRSPR,     think,     false, false,  8, 128, 0, T_Projectile, C_Pet, R_PetJump, &s_pet2};
statetype s_pshot1    = {PETSPARK1SPR,    PETSPARK1SPR,    stepthink, false, false,  8,   0, 0, T_Velocity, C_PShot, R_PShot, &s_pshot2};
statetype s_pshot2    = {PETSPARK2SPR,    PETSPARK2SPR,    stepthink, false, false,  8,   0, 0, T_Velocity, C_PShot, R_PShot, &s_pshot1};
statetype s_pshothot1 = {PETSPARKHIT1SPR, PETSPARKHIT1SPR, step,      false, false, 10,   0, 0, NULL, NULL, R_Draw, &s_pshothot2};
statetype s_pshothot2 = {PETSPARKHIT2SPR, PETSPARKHIT2SPR, step,      false, false, 10,   0, 0, NULL, NULL, R_Draw, NULL};
statetype s_petstun   = {PETSTUNSPR,      PETSTUNSPR,      think,     false, false,  0,   0, 0, T_Projectile, NULL, R_Stunned, NULL};

/*
===========================
=
= SpawnPet
=
===========================
*/

void SpawnPet(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->obclass = petobj;
	new->active = ac_yes;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY) + -8*PIXGLOBAL;
	new->temp2 = 2;	// health
	new->xdir = new->ydir = 1;
	NewState(new, &s_pet1);
}

/*
===========================
=
= T_Pet
=
===========================
*/

void T_Pet(objtype *ob)
{
	if (ob->x > player->x)
	{
		ob->xdir = -1;
	}
	else
	{
		ob->xdir = 1;
	}
	if (US_RndT() < 0x10)
	{
		xtry = 0;
		ob->state = &s_petsit1;
		ob->temp1 = 10;	// repeat animation 10 times;
	}
	else
	{
		if (ob->bottom != player->bottom)
		{
			ob->state = &s_petjump;
			if (ob->xdir == 1)
			{
				ob->xspeed = 40;
			}
			else
			{
				ob->xspeed = -40;
			}
			ob->yspeed = -40;
		}
		if (US_RndT() < 0x80)
		{
			xtry = 0;
			ob->state = &s_petbark1;
		}
	}
}

/*
===========================
=
= T_PetSit
=
===========================
*/

void T_PetSit(objtype *ob)
{
	if (--ob->temp1 == 0)
	{
		ob->state = &s_pet1;
	}
}

/*
===========================
=
= T_PetBark
=
===========================
*/

void T_PetBark(objtype *ob)
{
	Uint16 x;

	if (ob->xdir == 1)
	{
		x = ob->x + 7*PIXGLOBAL;
	}
	else
	{
		x = ob->x;
	}
	if (CheckSpawnShot(x, ob->y+4*PIXGLOBAL, &s_pshot1) != -1)
	{
		new->xspeed = ob->xdir * 60;
		new->xdir = ob->xdir;
		SD_PlaySound(SND_SHOCKSHUNDBARK);
	}
}

/*
===========================
=
= C_Pet
=
===========================
*/

void C_Pet(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		KillKeen();
	}
	if (hit->obclass == stunshotobj)	// no 'else if' in the original code!
	{
		if (--ob->temp2 == 0)	// handle health
		{
			ob->xspeed = 0;
			ob->yspeed = 0;
			StunObj(ob, hit, &s_petstun);
		}
		else
		{
			ob->temp3 = 2;	// draw white two times
			ob->needtoreact = true;
			ExplodeShot(hit);
		}
	}
}

/*
===========================
=
= R_Pet
=
===========================
*/

void R_Pet(objtype *ob)
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
		if ((ob->xdir == 1 && player->x > ob->x) || (ob->xdir == -1 && player->x < ob->x))
		{
			ChangeState(ob, &s_petjump);
			if (ob->xdir == 1)
			{
				ob->xspeed = 40;
			}
			else
			{
				ob->xspeed = -40;
			}
			ob->yspeed = -40;
		}
		else
		{
			ob->x -= ob->xmove*2;
			ob->xdir = -ob->xdir;
			ob->nothink = US_RndT() >> 5;
			ChangeState(ob, ob->state);
		}
	}
	if (ob->temp3)
	{
		ob->temp3--;
		RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, maskdraw, ob->priority);
	}
	else
	{
		RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
	}
}

/*
===========================
=
= R_PetJump
=
===========================
*/

void R_PetJump(objtype *ob)
{
	if (ob->hitnorth)
	{
		ob->nothink = US_RndT() >> 5;
		ChangeState(ob, &s_pet1);
	}
	if (ob->temp3)
	{
		ob->temp3--;
		RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, maskdraw, ob->priority);
	}
	else
	{
		RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
	}
}

/*
===========================
=
= C_PShot
=
===========================
*/

void C_PShot(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		KillKeen();
		ChangeState(ob, &s_pshothot1);
	}
	else if (hit->obclass == stunshotobj)
	{
		ExplodeShot(hit);
		ChangeState(ob, &s_pshothot1);
	}
}

/*
===========================
=
= R_PShot
=
===========================
*/

void R_PShot(objtype *ob)
{
	if (ob->hitnorth || ob->hiteast || ob->hitsouth || ob->hitwest)
	{
		SD_PlaySound(SND_SHOCKBALLEXPLODE);
		ChangeState(ob, &s_pshothot1);
	}
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}

/*
=============================================================================

						  SPHEREFUL

temp1 ... temp4 = sprite pointers for the guards circling around the sphere

=============================================================================
*/

statetype s_sphereful1    = {SPHEREFUL1SPR, SPHEREFUL1SPR, stepthink, false, false, 6, 0, 0, T_Sphereful, C_Spindread, R_Sphereful, &s_sphereful2};
statetype s_sphereful2    = {SPHEREFUL2SPR, SPHEREFUL2SPR, stepthink, false, false, 6, 0, 0, T_Sphereful, C_Spindread, R_Sphereful, &s_sphereful3};
statetype s_sphereful3    = {SPHEREFUL3SPR, SPHEREFUL3SPR, stepthink, false, false, 6, 0, 0, T_Sphereful, C_Spindread, R_Sphereful, &s_sphereful4};
statetype s_sphereful4    = {SPHEREFUL4SPR, SPHEREFUL4SPR, stepthink, false, false, 6, 0, 0, T_Sphereful, C_Spindread, R_Sphereful, &s_sphereful1};

/*
===========================
=
= SpawnSphereful
=
===========================
*/

void SpawnSphereful(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->obclass = spherefulobj;
	new->needtoclip = cl_fullclip;
	new->active = ac_yes;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY) + -1*TILEGLOBAL;
	new->priority = 1;
	NewState(new, &s_sphereful1);
}

/*
===========================
=
= T_Sphereful
=
===========================
*/

void T_Sphereful(objtype *ob)
{
	Sint32 i;
	ob->needtoreact = true;

	//
	// this code could be executed twice during the same frame because the
	// object's animation/state changed during that frame, so don't update
	// anything if we already have movement for the current frame i.e. the
	// update code has already been executed this frame
	//
	if (xtry == 0 && ytry == 0)
	{
		for (i=lasttimecount-tics; i<lasttimecount; i++)
		{
			if (!(i & 0xF))
			{
				if (ob->yspeed < 8)
					ob->yspeed++;

				if (ob->x < player->x && ob->xspeed < 8)
					ob->xspeed++;

				if (ob->x > player->x && ob->xspeed > -8)
					ob->xspeed--;
			}
			ytry += ob->yspeed;
			xtry += ob->xspeed;
		}
	}
}

/*
===========================
=
= R_Sphereful
=
===========================
*/

void R_Sphereful(objtype *ob)
{
	static Uint16 circle[16] = {
		384, 369, 328, 265, 192, 119,  58,  15,
		  0,  15,  58, 119, 192, 265, 328, 369
	};

	Uint16 index, shapenum, priority;

	if (ob->hitwest || ob->hiteast)
	{
		ob->xspeed = -ob->xspeed;
		SD_PlaySound(SND_SPHEREFULBOUNCE);
	}
	if (ob->hitsouth)
	{
		ob->yspeed = -ob->yspeed;
		SD_PlaySound(SND_SPHEREFULBOUNCE);
	}

	if (ob->hitnorth)
	{
		ob->yspeed = -ob->yspeed;
		if (player->y < ob->y)
		{
			ob->yspeed--;
		}
		else
		{
			ob->yspeed++;
		}
		if (ob->yspeed > -4)
		{
			ob->yspeed = -4;
		}
		else if (ob->yspeed < -12)
		{
			ob->yspeed = -12;
		}
		SD_PlaySound(SND_BOUNCE);
	}
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);

	index = ((Uint16)lasttimecount / 8) & 0xF;
	shapenum = index / 4 + SPHEREGUARD1SPR;
	if (index >= 8)
	{
		priority = 2;
	}
	else
	{
		priority = 0;
	}
	RF_PlaceSprite((void**)&ob->temp1, ob->x+circle[index], ob->y+circle[index], shapenum, spritedraw, priority);
	RF_PlaceSprite((void**)&ob->temp2, ob->x+24*PIXGLOBAL-circle[index], ob->y+circle[index], shapenum, spritedraw, priority);

	index += 8;
	index &= 0xF;
	if (index >= 8)
	{
		priority = 2;
	}
	else
	{
		priority = 0;
	}
	RF_PlaceSprite((void**)&ob->temp3, ob->x+circle[index], ob->y+circle[index], shapenum, spritedraw, priority);
	RF_PlaceSprite((void**)&ob->temp4, ob->x+24*PIXGLOBAL-circle[index], ob->y+circle[index], shapenum, spritedraw, priority);
}

/*
=============================================================================

						  SCOTTIE

=============================================================================
*/

statetype s_scottie1    = {SCOTTIEWALKL1SPR, SCOTTIEWALKR1SPR, step,  false, true,  8, 128, 0, T_Scottie, C_Scottie, R_Walk, &s_scottie2};
statetype s_scottie2    = {SCOTTIEWALKL2SPR, SCOTTIEWALKR2SPR, step,  false, true,  8, 128, 0, T_Scottie, C_Scottie, R_Walk, &s_scottie3};
statetype s_scottie3    = {SCOTTIEWALKL3SPR, SCOTTIEWALKR3SPR, step,  false, true,  8, 128, 0, T_Scottie, C_Scottie, R_Walk, &s_scottie4};
statetype s_scottie4    = {SCOTTIEWALKL4SPR, SCOTTIEWALKR4SPR, step,  false, true,  8, 128, 0, T_Scottie, C_Scottie, R_Walk, &s_scottie1};
statetype s_scottieface = {SCOTTIEFACESPR,   SCOTTIEFACESPR,   step,  false, true, 30,   0, 0, NULL, C_Scottie, R_Walk, &s_scottie1};
statetype s_scottiestun = {SCOTTIESTUNSPR,   SCOTTIESTUNSPR,   think, false, false, 0,   0, 0, T_Projectile, NULL, R_Stunned, NULL};

/*
===========================
=
= SpawnScottie
=
===========================
*/

void SpawnScottie(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->obclass = scottieobj;
	new->active = ac_yes;
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
	NewState(new, &s_scottie1);
}

/*
===========================
=
= T_Scottie
=
===========================
*/

void T_Scottie(objtype *ob)
{
	if (US_RndT() < 0x10)
	{
		xtry = 0;
		if (US_RndT() < 0x80)
		{
			ob->xdir = 1;
		}
		else
		{
			ob->xdir = -1;
		}
		ob->state = &s_scottieface;
	}
}

/*
===========================
=
= C_Scottie
=
===========================
*/

void C_Scottie(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj && hit->state->contact)
	{
		ClipToSpriteSide(hit, ob);
	}
	else if (hit->obclass == stunshotobj)
	{
		StunObj(ob, hit, &s_scottiestun);
	}
}

/*
=============================================================================

						  QED

=============================================================================
*/

statetype s_qed = {-1, -1, step, false, true, 8, 128, 0, NULL, NULL, NULL, &s_qed};

/*
===========================
=
= SpawnQed
=
===========================
*/

void SpawnQed(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->obclass = qedobj;
	new->active = ac_yes;
	new->tileleft = tileX;
	new->tiletop = tileY;
	new->tileright = new->tileleft + 1;
	new->tilebottom = new->tiletop + 1;
	new->x = new->left = CONVERT_TILE_TO_GLOBAL(tileX) + -1*PIXGLOBAL;
	new->y = new->top = CONVERT_TILE_TO_GLOBAL(tileY) + -1*PIXGLOBAL;
	new->right = new->left + 34*PIXGLOBAL;
	new->bottom = new->top + 34*PIXGLOBAL;
	NewState(new, &s_qed);
}