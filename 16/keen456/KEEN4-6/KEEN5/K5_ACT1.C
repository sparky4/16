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
K5_ACT1.C
=========

Contains the following actor types (in this order):

- some shared routines
- Bonus Items
- Teleport and Fuse effects
- Platforms
- falling platforms
- static platforms
- Goplat platforms
- Volte Face
- sneaky platforms
- Turrets

*/

#include "CK_DEF.H"

/*
=============================================================================

						  SHARED STUFF

=============================================================================
*/

Sint16 pdirx[] = {0, 1, 0, -1, 1, 1, -1, -1};
Sint16 pdiry[] = {-1, 0, 1, 0, -1, 1, 1, -1};

/*
===========================
=
= CheckSpawnShot
=
===========================
*/

Sint16 CheckSpawnShot(Uint16 x, Uint16 y, statetype *state)
{
	if (GetNewObj(true) == -1)
		return -1;
	new->x = x;
	new->y = y;
	new->obclass = mshotobj;
	new->active = ac_removable;
	NewState(new, state);
	if (!CheckPosition(new))
	{
		RemoveObj(new);
		return -1;
	}
	return 0;
}

/*
===========================
=
= C_ClipSide
=
===========================
*/

void C_ClipSide(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		playerkludgeclipcancel = true;
		ClipToSpriteSide(hit, ob);
		playerkludgeclipcancel = false;
	}
}

/*
===========================
=
= C_ClipTop
=
===========================
*/

void C_ClipTop(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
		ClipToSpriteTop(hit, ob);
}

/*
===========================
=
= R_Land
=
===========================
*/

void R_Land(objtype *ob)
{
	if (ob->hiteast || ob->hitwest)
		ob->xspeed = 0;

	if (ob->hitsouth)
		ob->yspeed = 0;

	if (ob->hitnorth)
	{
		ob->yspeed = 0;
		if (ob->state->nextstate)
		{
			ChangeState(ob, ob->state->nextstate);
		}
		else
		{
			RemoveObj(ob);
			return;
		}
	}

	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}

/*
===========================
=
= R_Bounce
=
===========================
*/

void R_Bounce(objtype *ob)
{
	Uint16 wall,absx,absy,angle,newangle;
	Uint32 speed;


	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);

	if (ob->hiteast || ob->hitwest)
		ob->xspeed = -ob->xspeed/2;

	if (ob->hitsouth)
	{
		ob->yspeed = -ob->yspeed/2;
		return;
	}

	wall = ob->hitnorth;
	if (wall)
	{
		if (ob->yspeed < 0)
			ob->yspeed = 0;

		absx = abs(ob->xspeed);
		absy = ob->yspeed;
		if (absx>absy)
		{
			if (absx>absy*2)	// 22 degrees
			{
				angle = 0;
				speed = absx*286;	// x*sqrt(5)/2
			}
			else				// 45 degrees
			{
				angle = 1;
				speed = absx*362;	// x*sqrt(2)
			}
		}
		else
		{
			if (absy>absx*2)	// 90 degrees
			{
				angle = 3;
				speed = absy*256;
			}
			else
			{
				angle = 2;		// 67 degrees
				speed = absy*286;	// y*sqrt(5)/2
			}
		}
		if (ob->xspeed > 0)
			angle = 7-angle;

		speed >>= 1;
		newangle = bounceangle[ob->hitnorth][angle];
		switch (newangle)
		{
		case 0:
			ob->xspeed = speed / 286;
			ob->yspeed = -ob->xspeed / 2;
			break;
		case 1:
			ob->xspeed = speed / 362;
			ob->yspeed = -ob->xspeed;
			break;
		case 2:
			ob->yspeed = -(speed / 286);
			ob->xspeed = -ob->yspeed / 2;
			break;
		case 3:

		case 4:
			ob->xspeed = 0;
			ob->yspeed = -(speed / 256);
			break;
		case 5:
			ob->yspeed = -(speed / 286);
			ob->xspeed = ob->yspeed / 2;
			break;
		case 6:
			ob->xspeed = ob->yspeed = -(speed / 362);
			break;
		case 7:
			ob->xspeed = -(speed / 286);
			ob->yspeed = ob->xspeed / 2;
			break;

		case 8:
			ob->xspeed = -(speed / 286);
			ob->yspeed = -ob->xspeed / 2;
			break;
		case 9:
			ob->xspeed = -(speed / 362);
			ob->yspeed = -ob->xspeed;
			break;
		case 10:
			ob->yspeed = speed / 286;
			ob->xspeed = -ob->yspeed / 2;
			break;
		case 11:

		case 12:
			ob->xspeed = 0;
			ob->yspeed = -(speed / 256);
			break;
		case 13:
			ob->yspeed = speed / 286;
			ob->xspeed = ob->yspeed / 2;
			break;
		case 14:
			ob->xspeed = speed / 362;
			ob->yspeed = speed / 362;
			break;
		case 15:
			ob->xspeed = speed / 286;
			ob->yspeed = ob->xspeed / 2;
			break;
		}

		if (speed < 256*16)
		{
			ChangeState(ob, ob->state->nextstate);
		}
	}
}

/*
=============================================================================

						  BONUS ITEMS
temp1 = bonus type
temp2 = base shape number
temp3 = last animated shape number +1

=============================================================================
*/

statetype s_bonus1    = {0,            0,            step,      false, false, 20, 0, 0, T_Bonus, NULL, R_Draw, &s_bonus2};
statetype s_bonus2    = {0,            0,            step,      false, false, 20, 0, 0, T_Bonus, NULL, R_Draw, &s_bonus1};
statetype s_bonusfly1 = {0,            0,            stepthink, false, false, 20, 0, 0, T_FlyBonus, NULL, R_Draw, &s_bonusfly2};
statetype s_bonusfly2 = {0,            0,            stepthink, false, false, 20, 0, 0, T_FlyBonus, NULL, R_Draw, &s_bonusfly1};
statetype s_bonusrise = {0,            0,            slide,     false, false, 40, 0, 8, NULL, NULL, R_Draw, NULL};
statetype s_splash1   = {VIVAPOOF1SPR, VIVAPOOF1SPR, step,      false, false,  8, 0, 0, NULL, NULL, R_Draw, &s_splash2};
statetype s_splash2   = {VIVAPOOF2SPR, VIVAPOOF2SPR, step,      false, false,  8, 0, 0, NULL, NULL, R_Draw, &s_splash3};
statetype s_splash3   = {VIVAPOOF3SPR, VIVAPOOF3SPR, step,      false, false,  8, 0, 0, NULL, NULL, R_Draw, &s_splash4};
statetype s_splash4   = {VIVAPOOF4SPR, VIVAPOOF4SPR, step,      false, false,  8, 0, 0, NULL, NULL, R_Draw, NULL};

Uint16 bonusshape[] = {
	REDGEM1SPR, YELLOWGEM1SPR, BLUEGEM1SPR, GREENGEM1SPR,
	SUGAR1ASPR, SUGAR2ASPR, SUGAR3ASPR,
	SUGAR4ASPR, SUGAR5ASPR, SUGAR6ASPR,
	ONEUPASPR, STUNCLIP1SPR, DOORCARD1SPR
};

/*
===========================
=
= SpawnBonus
=
===========================
*/

void SpawnBonus(Uint16 tileX, Uint16 tileY, Uint16 type)
{
	GetNewObj(false);
	new->needtoclip = cl_noclip;
	new->priority = 2;
	new->obclass = bonusobj;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY);
	new->ydir = -1;
	new->temp1 = type;
	new->temp2=new->shapenum = bonusshape[type];
	new->temp3 = new->temp2+2;
	NewState(new, &s_bonus1);
}

/*
===========================
=
= SpawnSplash
=
===========================
*/

void SpawnSplash(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(true);
	new->needtoclip = cl_noclip;
	new->priority = 3;
	new->obclass = inertobj;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY);
	NewState(new, &s_splash1);
}

/*
===========================
=
= T_Bonus
=
===========================
*/

void T_Bonus(objtype *ob)
{
	if (++ob->shapenum == ob->temp3)
		ob->shapenum = ob->temp2;
}

/*
===========================
=
= T_FlyBonus
=
===========================
*/

void T_FlyBonus(objtype *ob)
{
	if (ob->hitnorth)
		ob->state = &s_bonus1;

	if (++ob->shapenum == ob->temp3)
		ob->shapenum = ob->temp2;

	DoGravity(ob);
}

/*
=============================================================================

						  TELEPORT EFFECTS

=============================================================================
*/

statetype s_teleport1     = {TELEPORTSPARK1SPR, TELEPORTSPARK1SPR, step, false, false, 6, 0, 0, NULL, NULL, R_Draw, &s_teleport2};
statetype s_teleport2     = {TELEPORTSPARK2SPR, TELEPORTSPARK2SPR, step, false, false, 6, 0, 0, NULL, NULL, R_Draw, &s_teleport1};
statetype s_teleportzap1  = {TELEPORTZAP1SPR,   TELEPORTZAP1SPR,   step, false, false, 6, 0, 0, NULL, NULL, R_Draw, &s_teleportzap2};
statetype s_teleportzap2  = {TELEPORTZAP2SPR,   TELEPORTZAP2SPR,   step, false, false, 6, 0, 0, NULL, NULL, R_Draw, &s_teleportzap1};

/*
===========================
=
= SpawnTeleport
=
===========================
*/

void SpawnTeleport(void)
{
	GetNewObj(true);
	new->priority = 3;
	new->needtoclip = cl_noclip;
	new->obclass = teleporterobj;
	new->x = CONVERT_TILE_TO_GLOBAL(player->tileleft) - 8*PIXGLOBAL;
	new->y = CONVERT_TILE_TO_GLOBAL(player->tilebottom) - 5*TILEGLOBAL;
	NewState(new, &s_teleport1);

	GetNewObj(true);
	new->priority = 3;
	new->needtoclip = cl_noclip;
	new->obclass = teleporterobj;
	new->x = CONVERT_TILE_TO_GLOBAL(player->tileleft);
	new->y = CONVERT_TILE_TO_GLOBAL(player->tiletop) - 8*PIXGLOBAL;
	NewState(new, &s_teleportzap1);

	SD_PlaySound(SND_TELEPORT);
}

/*
=============================================================================

						  FUSE FLASH

=============================================================================
*/

statetype s_fuseflash1    = {FUSEFLASH1SPR, FUSEFLASH1SPR, step, false, false, 10, 0, 0, NULL, NULL, R_Draw, &s_fuseflash2};
statetype s_fuseflash2    = {FUSEFLASH2SPR, FUSEFLASH2SPR, step, false, false, 20, 0, 0, NULL, NULL, R_Draw, &s_fuseflash3};
statetype s_fuseflash3    = {FUSEFLASH3SPR, FUSEFLASH3SPR, step, false, false, 10, 0, 0, NULL, NULL, R_Draw, NULL};

/*
===========================
=
= SpawnFuseFlash
=
===========================
*/

void SpawnFuseFlash(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(true);
	new->priority = 3;
	new->needtoclip = cl_noclip;
	new->obclass = teleporterobj;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX-1);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY);
	NewState(new, &s_fuseflash1);
	SD_PlaySound(SND_BIGSPARK);
}

/*
=============================================================================

						  DEAD MACHINE

=============================================================================
*/

statetype s_deadmachine   = {-1, -1, step, false, false, 60, 0, 0, T_DeadMachine, NULL, R_Draw, NULL};

/*
===========================
=
= SpawnDeadMachine
=
===========================
*/

void SpawnDeadMachine(void)
{
	GetNewObj(false);
	new->active = ac_allways;
	new->needtoclip = cl_noclip;
	NewState(new, &s_deadmachine);
}

/*
===========================
=
= T_DeadMachine
=
===========================
*/

#pragma argsused
void T_DeadMachine(objtype *ob)
{
	if (mapon == 12)
	{
		playstate = ex_qedbroke;
	}
	else
	{
		playstate = ex_fusebroke;
	}
}

/*
=============================================================================

						  PLATFORMS

=============================================================================
*/

statetype s_platform      = {PLATFORMSPR,  PLATFORMSPR,  think,     false, false, 0, 0, 0, T_Platform, NULL, R_Draw, NULL};
statetype s_slotplat1     = {SLOTPLAT1SPR, SLOTPLAT1SPR, stepthink, false, false, 0, 0, 0, T_Slotplat, NULL, R_Draw, &s_slotplat2};
statetype s_slotplat2     = {SLOTPLAT2SPR, SLOTPLAT2SPR, stepthink, false, false, 0, 0, 0, T_Slotplat, NULL, R_Draw, &s_slotplat1};
// BUG? the slotplat states have a tictime of 0, so they never transition to the next state

/*
===========================
=
= SpawnPlatform
=
===========================
*/

void SpawnPlatform(Uint16 tileX, Uint16 tileY, Sint16 dir, Sint16 type)
{
	GetNewObj(false);
	new->obclass = platformobj;
	new->active = ac_allways;
	new->priority = 0;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY);
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
	if (type)
	{
		new->x += 4*PIXGLOBAL;
		new->y += 4*PIXGLOBAL;
		NewState(new, &s_slotplat1);
	}
	else
	{
		NewState(new, &s_platform);
	}
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

	//
	// this code could be executed twice during the same frame because the
	// object's animation/state changed during that frame, so don't update
	// anything if we already have movement for the current frame i.e. the
	// update code has already been executed this frame
	//
	if (!xtry && !ytry)
	{
		xtry = ob->xdir * 12 * tics;
		ytry = ob->ydir * 12 * tics;

		if (ob->xdir == 1)
		{
			newpos = ob->right + xtry;
			newtile = CONVERT_GLOBAL_TO_TILE(newpos);
			if (ob->tileright != newtile)
			{
				if (*(mapsegs[2]+mapbwidthtable[ob->tiletop]/2 + newtile) == PLATFORMBLOCK)
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
				if (*(mapsegs[2]+mapbwidthtable[ob->tiletop]/2 + newtile) == PLATFORMBLOCK)
				{
					ob->xdir = 1;
					xtry = xtry + (TILEGLOBAL - (newpos & 0xFF));
				}
			}
		}
		else if (ob->ydir == 1)
		{
			newpos = ob->bottom + ytry;
			newtile = CONVERT_GLOBAL_TO_TILE(newpos);
			if (ob->tilebottom != newtile)
			{
				if (*(mapsegs[2]+mapbwidthtable[newtile]/2 + ob->tileleft) == PLATFORMBLOCK)
				{
					if (*(mapsegs[2]+mapbwidthtable[newtile-2]/2 + ob->tileleft) == PLATFORMBLOCK)
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
				if (*(mapsegs[2]+mapbwidthtable[newtile]/2 + ob->tileleft) == PLATFORMBLOCK)
				{
					if (*(mapsegs[2]+mapbwidthtable[newtile+2]/2 + ob->tileleft) == PLATFORMBLOCK)
					{
						ytry = 0;
						ob->needtoreact = true;
					}
					else
					{
						ob->ydir = 1;
						ytry = ytry + (TILEGLOBAL - (newpos & 0xFF));
					}
				}
			}
		}
	}
}

/*
===========================
=
= T_Slotplat
=
===========================
*/

void T_Slotplat(objtype *ob)
{
	Uint16 newpos, newtile;

	//
	// this code could be executed twice during the same frame because the
	// object's animation/state changed during that frame, so don't update
	// anything if we already have movement for the current frame i.e. the
	// update code has already been executed this frame
	//
	if (!xtry && !ytry)
	{
		xtry = ob->xdir * 12 * tics;
		ytry = ob->ydir * 12 * tics;

		if (ob->xdir == 1)
		{
			newpos = ob->right + xtry;
			newtile = CONVERT_GLOBAL_TO_TILE(newpos);
			if (ob->tileright != newtile)
			{
				if (*(mapsegs[2]+mapbwidthtable[ob->tiletop]/2 + newtile) == PLATFORMBLOCK)
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
				if (*(mapsegs[2]+mapbwidthtable[ob->tiletop]/2 + newtile) == PLATFORMBLOCK)
				{
					ob->xdir = 1;
					xtry = xtry + (TILEGLOBAL - (newpos & 0xFF));
				}
			}
		}
		else if (ob->ydir == 1)
		{
			newpos = ob->bottom + ytry;
			newtile = CONVERT_GLOBAL_TO_TILE(newpos);
			if (ob->tilebottom != newtile)
			{
				if (*(mapsegs[2]+mapbwidthtable[newtile]/2 + ob->tileleft + 1) == PLATFORMBLOCK)
				{
					if (*(mapsegs[2]+mapbwidthtable[newtile-2]/2 + ob->tileleft) == PLATFORMBLOCK)	// BUG? '+ 1' is missing after 'tileleft'
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
				if (*(mapsegs[2]+mapbwidthtable[newtile]/2 + ob->tileleft + 1) == PLATFORMBLOCK)
				{
					if (*(mapsegs[2]+mapbwidthtable[newtile+2]/2 + ob->tileleft + 1) == PLATFORMBLOCK)
					{
						ytry = 0;
						ob->needtoreact = true;
					}
					else
					{
						ob->ydir = 1;
						ytry = ytry + (TILEGLOBAL - (newpos & 0xFF));
					}
				}
			}
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

void SpawnDropPlat(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->obclass = platformobj;
	new->active = ac_allways;
	new->priority = 0;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y=new->temp1 = CONVERT_TILE_TO_GLOBAL(tileY);
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
		ytry = tics << 4;	//tics * 16;
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
	Uint16 newpos, newtile;

	DoGravity(ob);

#if 0
	// bugfix: don't skip a tile (this is present in Keen 4, but missing in 5 & 6)
	if (ytry >= 15*PIXGLOBAL)
		ytry = 15*PIXGLOBAL;
#endif

	newpos = ob->bottom + ytry;
	newtile = CONVERT_GLOBAL_TO_TILE(newpos);
	if (ob->tilebottom != newtile)
	{
		if (*(mapsegs[2]+mapbwidthtable[newtile]/2 + ob->tileleft) == PLATFORMBLOCK)
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

/*
=============================================================================

						  STATIC PLATFORM

temp1 = initial y position (is set but never used)

=============================================================================
*/

statetype s_statplat    = {PLATFORMSPR, PLATFORMSPR, step, false, false, 32000, 0, 0, NULL, NULL, R_Draw, &s_statplat};

/*
===========================
=
= SpawnStaticPlat
=
===========================
*/

void SpawnStaticPlat(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->obclass = platformobj;
	new->active = ac_yes;
	new->priority = 0;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y=new->temp1 = CONVERT_TILE_TO_GLOBAL(tileY);
	new->xdir = 0;
	new->ydir = 1;
	new->needtoclip = cl_noclip;
	NewState(new, &s_statplat);
}

/*
=============================================================================

						  GO PLATFORMS

temp1 = direction
temp2 = countdown to next dir check

=============================================================================
*/

statetype s_goplat        = {PLATFORMSPR,  PLATFORMSPR,  think,     false, false, 0, 0, 0, T_GoPlat, NULL, R_Draw, NULL};
statetype s_slotgoplat1   = {SLOTPLAT1SPR, SLOTPLAT1SPR, stepthink, false, false, 0, 0, 0, T_GoSlotPlat, NULL, R_Draw, &s_slotgoplat2};
statetype s_slotgoplat2   = {SLOTPLAT2SPR, SLOTPLAT2SPR, stepthink, false, false, 0, 0, 0, T_GoSlotPlat, NULL, R_Draw, &s_slotgoplat1};
// BUG? the slotgoplat states have a tictime of 0, so they never transition to the next state

/*
===========================
=
= SpawnGoPlat
=
===========================
*/

void SpawnGoPlat(Uint16 tileX, Uint16 tileY, Sint16 dir, Sint16 type)
{
	GetNewObj(false);
	new->obclass = platformobj;
	new->active = ac_allways;
	new->priority = 0;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY);
	new->xdir = 0;
	new->ydir = 1;
	new->needtoclip = cl_noclip;
	if (type)
	{
		new->x += 4*PIXGLOBAL;
		new->y += 4*PIXGLOBAL;
		NewState(new, &s_slotgoplat1);
	}
	else
	{
		NewState(new, &s_goplat);
	}
	*(mapsegs[2]+mapbwidthtable[tileY]/2 + tileX) = DIRARROWSTART + dir;
	new->temp1 = dir;
	new->temp2 = TILEGLOBAL;
}

/*
===========================
=
= T_GoPlat
=
===========================
*/

void T_GoPlat(objtype *ob)
{
	Uint16 move;
	Uint16 tx, ty;
	Sint16 dir;

	//
	// this code could be executed twice during the same frame because the
	// object's animation/state changed during that frame, so don't update
	// anything if we already have movement for the current frame i.e. the
	// update code has already been executed this frame
	//
	if (!xtry && !ytry)
	{
		move = tics * 12;
		if (ob->temp2 > move)
		{
			ob->temp2 = ob->temp2 - move;

			dir = pdirx[ob->temp1];
			if (dir == 1)
			{
				xtry = xtry + move;
			}
			else if (dir == -1)
			{
				xtry = xtry + -move;
			}

			dir = pdiry[ob->temp1];
			if (dir == 1)
			{
				ytry = ytry + move;
			}
			else if (dir == -1)
			{
				ytry = ytry + -move;
			}
		}
		else
		{
			dir = pdirx[ob->temp1];
			if (dir == 1)
			{
				xtry += ob->temp2;
			}
			else if (dir == -1)
			{
				xtry += -ob->temp2;
			}

			dir = pdiry[ob->temp1];
			if (dir == 1)
			{
				ytry += ob->temp2;
			}
			else if (dir == -1)
			{
				ytry += -ob->temp2;
			}

			tx = CONVERT_GLOBAL_TO_TILE(ob->x + xtry);
			ty = CONVERT_GLOBAL_TO_TILE(ob->y + ytry);
			ob->temp1 = *(mapsegs[2]+mapbwidthtable[ty]/2 + tx) - DIRARROWSTART;
			if (ob->temp1 < arrow_North || ob->temp1 > arrow_None)
			{
				char error[60] = "Goplat moved to a bad spot: ";
				char buf[5] = "";

				strcat(error, itoa(ob->x, buf, 16));
				strcat(error, ",");
				strcat(error, itoa(ob->y, buf, 16));
				Quit(error);
			}

			move -= ob->temp2;
			ob->temp2 = TILEGLOBAL - move;

			dir = pdirx[ob->temp1];
			if (dir == 1)
			{
				xtry = xtry + move;
			}
			else if (dir == -1)
			{
				xtry = xtry - move;
			}

			dir = pdiry[ob->temp1];
			if (dir == 1)
			{
				ytry = ytry + move;
			}
			else if (dir == -1)
			{
				ytry = ytry - move;
			}
		}
	}
}

/*
===========================
=
= T_GoSlotPlat
=
===========================
*/

void T_GoSlotPlat(objtype *ob)
{
	Uint16 move;
	Uint16 tx, ty;
	Sint16 dir;

	//
	// this code could be executed twice during the same frame because the
	// object's animation/state changed during that frame, so don't update
	// anything if we already have movement for the current frame i.e. the
	// update code has already been executed this frame
	//
	if (!xtry && !ytry)
	{
		move = tics * 12;
		if (ob->temp2 > move)
		{
			ob->temp2 = ob->temp2 - move;

			dir = pdirx[ob->temp1];
			if (dir == 1)
			{
				xtry = xtry + move;
			}
			else if (dir == -1)
			{
				xtry = xtry + -move;
			}

			dir = pdiry[ob->temp1];
			if (dir == 1)
			{
				ytry = ytry + move;
			}
			else if (dir == -1)
			{
				ytry = ytry + -move;
			}
		}
		else
		{
			dir = pdirx[ob->temp1];
			if (dir == 1)
			{
				xtry += ob->temp2;
			}
			else if (dir == -1)
			{
				xtry += -ob->temp2;
			}

			dir = pdiry[ob->temp1];
			if (dir == 1)
			{
				ytry += ob->temp2;
			}
			else if (dir == -1)
			{
				ytry += -ob->temp2;
			}

			tx = CONVERT_GLOBAL_TO_TILE(ob->x + xtry + 4*PIXGLOBAL);
			ty = CONVERT_GLOBAL_TO_TILE(ob->y + ytry + 4*PIXGLOBAL);
			ob->temp1 = *(mapsegs[2]+mapbwidthtable[ty]/2 + tx) - DIRARROWSTART;
			if (ob->temp1 < arrow_North || ob->temp1 > arrow_None)
			{
				Quit("Goplat moved to a bad spot!");
			}

			move -= ob->temp2;
			ob->temp2 = TILEGLOBAL - move;

			dir = pdirx[ob->temp1];
			if (dir == 1)
			{
				xtry = xtry + move;
			}
			else if (dir == -1)
			{
				xtry = xtry - move;
			}

			dir = pdiry[ob->temp1];
			if (dir == 1)
			{
				ytry = ytry + move;
			}
			else if (dir == -1)
			{
				ytry = ytry - move;
			}
		}
	}
}

/*
=============================================================================

						  VOLTEFACE

temp1 = direction
temp2 = countdown to next dir check

=============================================================================
*/

statetype s_volte1     = {VOLTEFACE1SPR,    VOLTEFACE1SPR,    stepthink, false, false,   6, 0, 0, T_Volte, C_Volte, R_Draw, &s_volte2};
statetype s_volte2     = {VOLTEFACE2SPR,    VOLTEFACE2SPR,    stepthink, false, false,   6, 0, 0, T_Volte, C_Volte, R_Draw, &s_volte3};
statetype s_volte3     = {VOLTEFACE3SPR,    VOLTEFACE3SPR,    stepthink, false, false,   6, 0, 0, T_Volte, C_Volte, R_Draw, &s_volte4};
statetype s_volte4     = {VOLTEFACE4SPR,    VOLTEFACE4SPR,    stepthink, false, false,   6, 0, 0, T_Volte, C_Volte, R_Draw, &s_volte1};
statetype s_voltestun  = {VOLTEFACESTUNSPR, VOLTEFACESTUNSPR, step,      false, false, 300, 0, 0, NULL, NULL, R_Draw, &s_volte1};

/*
===========================
=
= SpawnVolte
=
===========================
*/

void SpawnVolte(Uint16 tileX, Uint16 tileY)
{
	Uint16 dir;
	Uint16 far *map;

	GetNewObj(false);
	new->obclass = volteobj;
	new->active = ac_allways;
	new->priority = 2;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY);
	new->needtoclip = cl_noclip;
	NewState(new, &s_volte1);
	map = mapsegs[2] + mapbwidthtable[tileY]/2 + tileX;
	if (map[-1] == DIRARROWSTART + arrow_East)
	{
		dir = arrow_East;
	}
	else if (map[1] == DIRARROWSTART + arrow_West)
	{
		dir = arrow_West;
	}
	else if (*(map-mapwidth) == DIRARROWSTART + arrow_South)
	{
		dir = arrow_South;
	}
	else if (*(map+mapwidth) == DIRARROWSTART + arrow_North)
	{
		dir = arrow_North;
	}
	map[0] = dir + DIRARROWSTART;
	new->temp1 = dir;
	new->temp2 = TILEGLOBAL;
}

/*
===========================
=
= T_Volte
=
===========================
*/

void T_Volte(objtype *ob)
{
	Uint16 move;
	Uint16 tx, ty;
	Sint16 dir;

	//
	// this code could be executed twice during the same frame because the
	// object's animation/state changed during that frame, so don't update
	// anything if we already have movement for the current frame i.e. the
	// update code has already been executed this frame
	//
	if (!xtry && !ytry)
	{
		move = tics << 5;
		if (ob->temp2 > move)
		{
			ob->temp2 = ob->temp2 - move;

			dir = pdirx[ob->temp1];
			if (dir == 1)
			{
				xtry = xtry + move;
			}
			else if (dir == -1)
			{
				xtry = xtry + -move;
			}

			dir = pdiry[ob->temp1];
			if (dir == 1)
			{
				ytry = ytry + move;
			}
			else if (dir == -1)
			{
				ytry = ytry + -move;
			}
		}
		else
		{
			dir = pdirx[ob->temp1];
			if (dir == 1)
			{
				xtry += ob->temp2;
			}
			else if (dir == -1)
			{
				xtry += -ob->temp2;
			}

			dir = pdiry[ob->temp1];
			if (dir == 1)
			{
				ytry += ob->temp2;
			}
			else if (dir == -1)
			{
				ytry += -ob->temp2;
			}

			tx = CONVERT_GLOBAL_TO_TILE(ob->x + xtry);
			ty = CONVERT_GLOBAL_TO_TILE(ob->y + ytry);
			ob->temp1 = *(mapsegs[2]+mapbwidthtable[ty]/2 + tx) - DIRARROWSTART;
			if (ob->temp1 < arrow_North || ob->temp1 > arrow_None)
			{
				char error[60] = "Volte moved to a bad spot: ";
				char buf[5] = "";

				strcat(error, itoa(ob->x, buf, 16));
				strcat(error, ",");
				strcat(error, itoa(ob->y, buf, 16));
				Quit(error);
			}

			move -= ob->temp2;
			ob->temp2 = TILEGLOBAL - move;

			dir = pdirx[ob->temp1];
			if (dir == 1)
			{
				xtry = xtry + move;
			}
			else if (dir == -1)
			{
				xtry = xtry - move;
			}

			dir = pdiry[ob->temp1];
			if (dir == 1)
			{
				ytry = ytry + move;
			}
			else if (dir == -1)
			{
				ytry = ytry - move;
			}
		}
	}
}

/*
===========================
=
= C_Volte
=
===========================
*/

void C_Volte(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		KillKeen();
	}
	else if (hit->obclass == stunshotobj)
	{
		ExplodeShot(hit);
		ChangeState(ob, &s_voltestun);
	}
}

/*
=============================================================================

						  SNEAKY PLATFORM

temp1 = initial x position (is set but never used)

=============================================================================
*/

statetype s_sneakplatsit    = {PLATFORMSPR, PLATFORMSPR, think, false, false,  0,   0, 0, T_SneakPlat, NULL, R_Draw, NULL};
statetype s_sneakplatdodge  = {PLATFORMSPR, PLATFORMSPR, slide, false, false, 48,  32, 0, NULL, NULL, R_Draw, &s_sneakplatreturn};
statetype s_sneakplatreturn = {PLATFORMSPR, PLATFORMSPR, slide, false, false, 96, -16, 0, NULL, NULL, R_Draw, &s_sneakplatsit};

/*
===========================
=
= SpawnSneakPlat
=
===========================
*/

void SpawnSneakPlat(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->obclass = platformobj;
	new->active = ac_allways;
	new->priority = 0;
	new->x=new->temp1 = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY);
	new->xdir = 0;
	new->ydir = 1;
	new->needtoclip = cl_noclip;
	NewState(new, &s_sneakplatsit);
}

/*
===========================
=
= T_SneakPlat
=
===========================
*/

void T_SneakPlat(objtype *ob)
{
	Sint16 dist;

	if (player->state != &s_keenjump1)
		return;

	if (player->xdir == 1)
	{
		dist = ob->left-player->right;
		if (dist > 4*TILEGLOBAL || dist < 0)
			return;
	}
	else
	{
		dist = player->left-ob->right;
		if (dist > 4*TILEGLOBAL || dist < 0)
			return;
	}
	dist = player->y - ob->y;
	if (dist < -6*TILEGLOBAL || dist > 6*TILEGLOBAL)
		return;

	ob->xdir = player->xdir;
	ob->state = &s_sneakplatdodge;
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
	new->active = ac_removable;
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