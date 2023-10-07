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
K6_ACT1.C
=========

Contains the following actor types (in this order):

- some shared routines
- Bonus Items
- Grabbiter
- Rocket
- Grapple spots
- Satellite
- Quest Items (Sandwich, Grappling Hook, Passcard, Molly)
- Platforms
- falling platforms
- static platforms
- Goplat platforms
- Trick platforms
- Bloog
- Blooguard
- Blooglet

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
#ifdef KEEN6Ev15
	if (!wall)
	{
		return;
	}
	else
#else
	if (wall)
#endif
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

statetype s_bonus1    = {0, 0, step,      false, false, 20, 0, 0, T_Bonus, NULL, R_Draw, &s_bonus2};
statetype s_bonus2    = {0, 0, step,      false, false, 20, 0, 0, T_Bonus, NULL, R_Draw, &s_bonus1};
statetype s_bonusfly1 = {0, 0, stepthink, false, false, 20, 0, 0, T_FlyBonus, NULL, R_Draw, &s_bonusfly2};
statetype s_bonusfly2 = {0, 0, stepthink, false, false, 20, 0, 0, T_FlyBonus, NULL, R_Draw, &s_bonusfly1};
statetype s_bonusrise = {0, 0, slide,     false, false, 40, 0, 8, NULL, NULL, R_Draw, NULL};

statetype s_splash1   = {VIVASPLASH1SPR, VIVASPLASH1SPR, step, false, false, 8, 0, 0, NULL, NULL, R_Draw, &s_splash2};
statetype s_splash2   = {VIVASPLASH2SPR, VIVASPLASH2SPR, step, false, false, 8, 0, 0, NULL, NULL, R_Draw, &s_splash3};
statetype s_splash3   = {VIVASPLASH3SPR, VIVASPLASH3SPR, step, false, false, 8, 0, 0, NULL, NULL, R_Draw, &s_splash4};
statetype s_splash4   = {VIVASPLASH4SPR, VIVASPLASH4SPR, step, false, false, 8, 0, 0, NULL, NULL, R_Draw, NULL};

Uint16 bonusshape[] = {
	REDGEM1SPR, YELLOWGEM1SPR, BLUEGEM1SPR, GREENGEM1SPR,
	SUGAR1ASPR, SUGAR2ASPR, SUGAR3ASPR,
	SUGAR4ASPR, SUGAR5ASPR, SUGAR6ASPR,
	ONEUPASPR, STUNCLIP1SPR
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

						  GRABBITER

=============================================================================
*/

statetype s_grabbiter1      = {GRABBITER1SPR,      GRABBITER1SPR,      step, false, false, 12, 0, 0, NULL, C_Grabbiter, R_Draw, &s_grabbiter2};
statetype s_grabbiter2      = {GRABBITER2SPR,      GRABBITER2SPR,      step, false, false, 12, 0, 0, NULL, C_Grabbiter, R_Draw, &s_grabbiter1};
statetype s_grabbitersleep1 = {GRABBITERSLEEP1SPR, GRABBITERSLEEP1SPR, step, false, false, 12, 0, 0, NULL, NULL, R_Draw, &s_grabbitersleep2};
statetype s_grabbitersleep2 = {GRABBITERSLEEP2SPR, GRABBITERSLEEP2SPR, step, false, false, 12, 0, 0, NULL, NULL, R_Draw, &s_grabbitersleep1};

/*
===========================
=
= SpawnGrabbiter
=
===========================
*/

void SpawnGrabbiter(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->active = ac_yes;
	new->needtoclip = cl_noclip;
	new->priority = 2;
	new->obclass = grabbiterobj;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY);
	if (gamestate.sandwichstate == 2)
	{
		NewState(new, &s_grabbitersleep1);
	}
	else
	{
		NewState(new, &s_grabbiter1);
	}
}

/*
===========================
=
= C_Grabbiter
=
===========================
*/

void C_Grabbiter(objtype *ob, objtype *hit)
{
	// BUG: this is executed for every object, not just (Map-)Keen!
	switch (gamestate.sandwichstate)
	{
	case 0:
		CA_CacheGrChunk(KEENTALK1PIC);
		SD_PlaySound(SND_GRABBITER);
		VW_FixRefreshBuffer();
		US_CenterWindow(26, 8);
		VWB_DrawPic(WindowX+WindowW-48, WindowY, KEENTALK1PIC);
		WindowW -= 48;
		PrintY += 5;
		US_CPrint(
			"Oh, no!\n"
			"It's a slavering\n"
			"Grabbiter! He says,\n"
			"\"Get me lunch and\n"
			"I'll tell ya a secret!\""
			);
		VW_UpdateScreen();
		SD_PlaySound(SND_NOWAY);
		VW_WaitVBL(30);
		IN_ClearKeysDown();
		IN_Ack();
		RF_ForceRefresh();

		//push Keen back
		xtry = -hit->xmove;
		ytry = -hit->ymove;
		hit->xdir = hit->ydir = 0;
		ClipToWalls(hit);
		break;

	case 1:
		gamestate.sandwichstate++;
		CA_CacheGrChunk(KEENTALK1PIC);
		VW_FixRefreshBuffer();
		US_CenterWindow(26, 8);
		VWB_DrawPic(WindowX+WindowW-48, WindowY, KEENTALK1PIC);
		WindowW -= 48;
		PrintY += 2;
		US_CPrint(
			"The Grabbiter grabs\n"
			"the gigantic sandwich,\n"
			"downs it in one bite,\n"
			"and says,\"Here's your\n"
			"secret. Big meals\n"
			"make me sleepy!\n"	// BUG: quote is missing at the end
			);
		VW_UpdateScreen();
		VW_WaitVBL(30);
		IN_ClearKeysDown();
		IN_Ack();
		ChangeState(ob, &s_grabbitersleep1);
		RF_ForceRefresh();
	}
}

/*
=============================================================================

						  ROCKET

temp1 = direction
temp2 = countdown to next dir check

=============================================================================
*/

statetype s_rocket        = {ROCKETSPR,     ROCKETSPR,     think,     false, false, 0, 0, 0, NULL, C_Rocket, R_Draw, NULL};
statetype s_rocketfly1    = {ROCKETFLY1SPR, ROCKETFLY1SPR, stepthink, false, false, 8, 0, 0, T_RocketFly, C_RocketFly, R_Draw, &s_rocketfly2};
statetype s_rocketfly2    = {ROCKETFLY2SPR, ROCKETFLY2SPR, stepthink, false, false, 8, 0, 0, T_RocketFly, C_RocketFly, R_Draw, &s_rocketfly1};
statetype s_keenrocket    = {0,             0,             think,     false, false, 0, 0, 0, T_Rocket, NULL, R_Draw, NULL};

/*
===========================
=
= SpawnRocket
=
===========================
*/

void SpawnRocket(Uint16 tileX, Uint16 tileY, Uint16 state)
{
	if (gamestate.rocketstate == state)
	{
		GetNewObj(false);
		new->active = ac_yes;
		new->needtoclip = cl_noclip;
		new->priority = 3;
		new->obclass = rocketobj;
		new->x = CONVERT_TILE_TO_GLOBAL(tileX);
		new->y = CONVERT_TILE_TO_GLOBAL(tileY);
		NewState(new, &s_rocket);
	}
}

/*
===========================
=
= T_Rocket
=
===========================
*/

void T_Rocket(objtype *ob)
{
	ob->needtoreact = true;
}

/*
===========================
=
= C_Rocket
=
===========================
*/

void C_Rocket(objtype *ob, objtype *hit)
{
	// BUG: this is executed for every object, not just (Map-)Keen!
	switch (gamestate.passcardstate)
	{
	case 0:
		CA_CacheGrChunk(KEENTALK1PIC);
		VW_FixRefreshBuffer();
		US_CenterWindow(26, 8);
		VWB_DrawPic(WindowX+WindowW-48, WindowY, KEENTALK1PIC);
		WindowW -= 48;
		PrintY += 5;
		US_CPrint(
			"The door makes a loud\n"
			"blooping noise.\n"
			"It says,\n"
			"\"Passcard required\n"
			"for entry.\""
			);
		VW_UpdateScreen();
		SD_PlaySound(SND_NOWAY);
		VW_WaitVBL(30);
		IN_ClearKeysDown();
		IN_Ack();
		RF_ForceRefresh();

		//push Keen back
		xtry = -hit->xmove;
		ytry = -hit->ymove;
		hit->xdir = hit->ydir = 0;
		ClipToWalls(hit);
		break;

	case 1:
		ob->temp1 = arrow_North;
		ob->temp2 = TILEGLOBAL;
		ChangeState(ob, &s_rocketfly1);

		hit->x = ob->x;
		hit->y = ob->y + TILEGLOBAL;
		hit->needtoclip = cl_noclip;
		ChangeState(hit, &s_keenrocket);
		SD_PlaySound(SND_ROCKETSTART);
		SD_WaitSoundDone();
	}
}

/*
===========================
=
= C_RocketFly
=
===========================
*/

void C_RocketFly(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		hit->x = ob->x;
		hit->y = ob->y+TILEGLOBAL;
		ChangeState(hit, hit->state);
	}
}

/*
===========================
=
= T_RocketFly
=
===========================
*/

void T_RocketFly(objtype *ob)
{
	Uint16 move, tx, ty;
	Sint16 dir;

	//
	// this code could be executed twice during the same frame because the
	// object's animation/state changed during that frame, so don't update
	// anything if we already have movement for the current frame i.e. the
	// update code has already been executed this frame
	//
	if (xtry == 0 && ytry == 0)
	{
		if (!SD_SoundPlaying())
			SD_PlaySound(SND_ROCKETFLY);

		move = tics << 5;
		if (ob->temp2 > move)
		{
			ob->temp2 = ob->temp2 - move;
			dir = pdirx[ob->temp1];
			if (dir == 1)
			{
				xtry = move;
			}
			else if (dir == -1)
			{
				xtry = -move;
			}
			dir = pdiry[ob->temp1];
			if (dir == 1)
			{
				ytry = move;

			}
			else if (dir == -1)
			{
				ytry = -move;
			}
		}
		else
		{
			dir = pdirx[ob->temp1];
			if (dir == 1)
			{
				xtry = ob->temp2;
			}
			else if (dir == -1)
			{
				xtry = -ob->temp2;
			}
			dir = pdiry[ob->temp1];
			if (dir == 1)
			{
				ytry = ob->temp2;
			}
			else if (dir == -1)
			{
				ytry = -ob->temp2;
			}

			tx = CONVERT_GLOBAL_TO_TILE(ob->x + xtry);
			ty = CONVERT_GLOBAL_TO_TILE(ob->y + ytry);
			ob->temp1 = *(mapsegs[2]+mapbwidthtable[ty]/2 + tx)-DIRARROWSTART;
			if (ob->temp1 < arrow_North || ob->temp1 > arrow_None)
			{
				ob->x += xtry;
				ob->y += ytry;
				ChangeState(ob, &s_rocket);

				player->x = CONVERT_TILE_TO_GLOBAL(tx+1) + 1*PIXGLOBAL;
				player->y = CONVERT_TILE_TO_GLOBAL(ty+1);
				player->obclass = keenobj;
				player->shapenum = WORLDKEENR3SPR;
				player->needtoclip = cl_midclip;
				NewState(player, &s_worldkeen);
				gamestate.rocketstate ^= 1;
			}
			else
			{
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
}

/*
=============================================================================

						  GRAPPLE SPOT

temp1 = type (0 = top of cliff, 1 = bottom of cliff)

=============================================================================
*/

statetype s_grapplespot   = {-1,                 -1,                 think, false, false,  0, 0,  0, NULL, C_GrappleSpot, R_Draw, NULL};
statetype s_throwrope1    = {WORLDKEENTRHOW1SPR, WORLDKEENTRHOW1SPR, step,  false, false, 10, 0,  0, NULL, NULL, R_Draw, &s_throwrope2};
statetype s_throwrope2    = {WORLDKEENTRHOW2SPR, WORLDKEENTRHOW2SPR, step,  false, false,  8, 0,  0, T_ThrowRope, NULL, R_Draw, &s_worldkeen};
statetype s_climbrope1    = {WORLDKEENCLIMB1SPR, WORLDKEENCLIMB1SPR, slide, true,  false,  4, 0, 16, NULL, NULL, R_Draw, &s_climbrope2};
statetype s_climbrope2    = {WORLDKEENCLIMB2SPR, WORLDKEENCLIMB2SPR, slide, true,  false,  4, 0, 16, T_ClimbRope, NULL, R_Draw, &s_climbrope1};
statetype s_maprope       = {ROPETHROW2SPR,      ROPETHROW2SPR,      think, false, false,  0, 0,  0, NULL, NULL, R_Draw, NULL};
statetype s_mapropeshort  = {ROPETHROW1SPR,      ROPETHROW1SPR,      step,  false, false, 10, 0,  0, NULL, NULL, R_Draw, &s_mapropeshort};

/*
===========================
=
= SpawnGrappleSpot
=
===========================
*/

void SpawnGrappleSpot(Uint16 tileX, Uint16 tileY, Uint16 type)
{
	GetNewObj(false);
	new->active = ac_yes;
	new->needtoclip = cl_noclip;
	new->priority = 2;
	new->obclass = grapplespotobj;
	new->tileleft = new->tileright = tileX;
	new->tiletop = new->tilebottom = tileY;
	new->temp1 = type;
	new->x = new->left = CONVERT_TILE_TO_GLOBAL(tileX);
	new->right = new->left + TILEGLOBAL;
	if (type)
	{
		new->y = new->top = CONVERT_TILE_TO_GLOBAL(tileY+1)-1;
	}
	else
	{
		new->y = new->top = CONVERT_TILE_TO_GLOBAL(tileY);
	}
	new->bottom = new->top + 1;
	NewState(new, &s_grapplespot);

	if (gamestate.hookstate == 2 && type)
	{
		GetNewObj(false);
		new->active = ac_yes;
		new->needtoclip = cl_noclip;
		new->priority = 0;
		new->obclass = inertobj;
		new->x = CONVERT_TILE_TO_GLOBAL(tileX);
		new->y = CONVERT_TILE_TO_GLOBAL(tileY+1);
		NewState(new, &s_maprope);
	}
}

/*
===========================
=
= T_ThrowRope
=
===========================
*/

void T_ThrowRope(objtype *ob)
{
	GetNewObj(false);
	new->active = ac_yes;
	new->needtoclip = cl_noclip;
	new->priority = 0;
	new->obclass = inertobj;
	new->x = CONVERT_TILE_TO_GLOBAL(ob->tileright);
	new->y = ob->y - 2*TILEGLOBAL;
	NewState(new, &s_maprope);

	ob->obclass = keenobj;
	ob->shapenum = WORLDKEENU3SPR;
}

/*
===========================
=
= T_ClimbRope
=
===========================
*/

void T_ClimbRope(objtype *ob)
{
	if (--ob->temp4 == 0)
	{
		if (ob->ydir == 1)
		{
			ob->y += 3*PIXGLOBAL;
			ob->shapenum = WORLDKEEND3SPR;
		}
		else
		{
			ob->y -= 3*PIXGLOBAL;
			ob->shapenum = WORLDKEENU3SPR;
		}
		ob->obclass = keenobj;
		NewState(ob, &s_worldkeen);
	}
}

/*
===========================
=
= C_GrappleSpot
=
===========================
*/

void C_GrappleSpot(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		switch (gamestate.hookstate)
		{
		case 0:
			CA_CacheGrChunk(KEENTALK1PIC);
			VW_FixRefreshBuffer();
			US_CenterWindow(26, 8);
			VWB_DrawPic(WindowX+WindowW-48, WindowY, KEENTALK1PIC);
			WindowW -= 48;
			PrintY += 15;
			US_CPrint(
				"What a tall cliff!\n"
				"Wish I had a rope\n"
				"and grappling hook.\n"
				);
			VW_UpdateScreen();
			SD_PlaySound(SND_NOWAY);
			VW_WaitVBL(30);
			IN_ClearKeysDown();
			IN_Ack();
			RF_ForceRefresh();

			//push Keen back
			xtry = -hit->xmove;
			ytry = -hit->ymove;
			hit->xdir = hit->ydir = 0;
			ClipToWalls(hit);
			break;

		case 1:
			gamestate.hookstate++;
			SD_PlaySound(SND_THROWROPE);
			ChangeState(hit, &s_throwrope1);
			hit->obclass = inertobj;
			break;

		case 2:
			if (ob->temp1)
			{
				hit->y += 4*PIXGLOBAL;
				hit->temp4 = 6;
				hit->ydir = 1;
			}
			else
			{
				hit->y -= 4*PIXGLOBAL;
				hit->temp4 = 6;
				hit->ydir = -1;
			}
			NewState(hit, &s_climbrope1);
			hit->obclass = inertobj;
		}
	}
}

/*
=============================================================================

						  SATELLITE

temp1 = direction (satellite) / type (stop points)
temp2 = countdown to next dir check
temp3 = type of stop point touched (low byte: current; high byte: previous)
        is updated every frame and resets to 0 when no longer touching a spot
temp4 = type of last stop point passed over (1 or 2, never 0)
        is updated when no longer touching the spot

=============================================================================
*/

statetype s_satellitestopspot  = {-1,               -1,               think,     false, false,  0, 0, 0, NULL, NULL, NULL, NULL};
statetype s_worldkeensatellite = {WORLDKEENHANGSPR, WORLDKEENHANGSPR, think,     false, false,  0, 0, 0, NULL, NULL, R_WorldKeenSatellite, NULL};
statetype s_satellite1         = {SATELLITE1SPR,    SATELLITE1SPR,    stepthink, false, false, 10, 0, 0, T_Satellite, C_Satellite, R_Draw, &s_satellite2};
statetype s_satellite2         = {SATELLITE2SPR,    SATELLITE2SPR,    stepthink, false, false, 10, 0, 0, T_Satellite, C_Satellite, R_Draw, &s_satellite3};
statetype s_satellite3         = {SATELLITE3SPR,    SATELLITE3SPR,    stepthink, false, false, 10, 0, 0, T_Satellite, C_Satellite, R_Draw, &s_satellite4};
statetype s_satellite4         = {SATELLITE4SPR,    SATELLITE4SPR,    stepthink, false, false, 10, 0, 0, T_Satellite, C_Satellite, R_Draw, &s_satellite1};

/*
===========================
=
= SpawnSatelliteStop
=
===========================
*/

void SpawnSatelliteStop(Uint16 tileX, Uint16 tileY, Uint16 type)
{
	GetNewObj(false);
	new->active = ac_allways;
	new->needtoclip = cl_noclip;
	new->priority = 2;
	new->obclass = satellitestopobj;
	new->tileleft=new->tileright=tileX;
	new->tiletop=new->tilebottom=tileY;
	new->temp1 = (type ^ 1) + 1;	// type is either 0 or 1, so this just maps 0 to 2 and 1 to 1
	new->x=new->left = CONVERT_TILE_TO_GLOBAL(tileX);
	new->right = new->left + TILEGLOBAL;
	new->y=new->top = CONVERT_TILE_TO_GLOBAL(tileY);
	new->bottom = new->top + TILEGLOBAL;
	NewState(new, &s_satellitestopspot);
}

/*
===========================
=
= SpawnSatellite
=
===========================
*/

void SpawnSatellite(Uint16 tileX, Uint16 tileY)
{
	Sint16 dir;

	GetNewObj(false);
	new->needtoclip = cl_noclip;
	new->priority = 2;
	new->active = ac_allways;
	new->obclass = satelliteobj;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY);
	new->xdir = 0;
	new->ydir = 1;
	NewState(new, &s_satellite1);

	dir = arrow_West;
	(mapsegs[2]+mapbwidthtable[tileY]/2)[tileX] = (dir+DIRARROWSTART);
	new->temp1 = dir;
	new->temp2 = TILEGLOBAL;
	new->temp4 = 2;
}

/*
===========================
=
= T_Satellite
=
===========================
*/

void T_Satellite(objtype *ob)
{
	//
	// this code could be executed twice during the same frame because the
	// object's animation/state changed during that frame, so don't update
	// anything if we already have movement for the current frame i.e. the
	// update code has already been executed this frame
	//
	if (xtry == 0 && ytry == 0)
	{
		//
		// if current stop spot type is 0 (not touching a spot), but previous
		// type is not 0, then set temp4 to the previous type
		//
		if (!(ob->temp3 & 0xFF) && (ob->temp3 & 0xFF00))
		{
			ob->temp4 = ob->temp3 >> 8;
		}
		//
		// move current type into previous type and set current stop type to 0
		//
		ob->temp3 <<= 8;

		//
		// follow the arrow path like a GoPlat
		//
		T_GoPlat(ob);
	}
}

/*
===========================
=
= C_Satellite
=
===========================
*/

void C_Satellite(objtype *ob, objtype *hit)
{
	Sint16 temp;
	objtype *o;

	if (hit->state == &s_satellitestopspot)
	{
		ob->temp3 |= hit->temp1;
	}
	else if (hit->obclass == keenobj)
	{
		//
		// check if satellite has reaced a new stop spot
		//
		temp = ob->temp3 >> 8;
		if (temp && temp != ob->temp4)
		{
			SD_PlaySound(SND_GRABSATELLITE);
			//
			// update last spot value (don't grab or drop Keen until moved to the next spot)
			//
			ob->temp4 = temp;
			if (player->state == &s_worldkeensatellite)
			{
				//
				// drop Keen off at the current stop spot
				//
				for (o=player->next; o; o=o->next)
				{
					if (o->obclass == satellitestopobj && o->temp1 == temp)
					{
						hit->x = o->x;
						hit->y = o->y;
						hit->shapenum = WORLDKEENU3SPR;
						ChangeState(player, &s_worldkeen);
						hit->needtoclip = cl_midclip;
						return;
					}
				}
			}
			else
			{
				//
				// grab and carry Keen
				//
				hit->x = ob->x + 12*PIXGLOBAL;
				hit->y = ob->y + 16*PIXGLOBAL;
				hit->needtoclip = cl_noclip;
				ChangeState(player, &s_worldkeensatellite);
			}
		}
		else if (hit->state == &s_worldkeensatellite)
		{
			//
			// move Keen along with the satellite
			//
			hit->x = ob->x + 12*PIXGLOBAL;
			hit->y = ob->y + 16*PIXGLOBAL;
			ChangeState(hit, hit->state);
		}
	}
}

/*
===========================
=
= R_WorldKeenSatellite
=
===========================
*/

void R_WorldKeenSatellite(objtype *ob)
{
	RF_PlaceSprite(&ob->sprite, ob->x + 4*PIXGLOBAL, ob->y + 8*PIXGLOBAL, WORLDKEENHANGSPR, spritedraw, 1);
}

/*
=============================================================================

						  SANDWICH

=============================================================================
*/

statetype s_sandwich      = {SANDWICHSPR, SANDWICHSPR, think, false, false, 0, 0, 0, NULL, C_Molly, R_Draw, NULL};

/*
===========================
=
= SpawnSandwich
=
===========================
*/

void SpawnSandwich(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->needtoclip = cl_noclip;
	new->priority = 2;
	new->obclass = sandwichobj;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY);
	NewState(new, &s_sandwich);
}

/*
=============================================================================

						  GRAPPLING HOOK

=============================================================================
*/

statetype s_hook          = {HOOKSPR, HOOKSPR, think, false, false, 0, 0, 0, NULL, C_Molly, R_Draw, NULL};

/*
===========================
=
= SpawnHook
=
===========================
*/

void SpawnHook(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->needtoclip = cl_noclip;
	new->priority = 2;
	new->obclass = hookobj;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY);
	NewState(new, &s_hook);
}

/*
=============================================================================

						  PASSCARD

=============================================================================
*/

statetype s_passcard      = {PASSCARDSPR, PASSCARDSPR, think, false, false, 0, 0, 0, NULL, C_Molly, R_Draw, NULL};

/*
===========================
=
= SpawnPasscard
=
===========================
*/

void SpawnPasscard(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->needtoclip = cl_noclip;
	new->priority = 2;
	new->obclass = passcardobj;
	new->x = CONVERT_TILE_TO_GLOBAL(tileX);
	new->y = CONVERT_TILE_TO_GLOBAL(tileY);
	NewState(new, &s_passcard);
}

//============================================================================

/*
===========================
=
= C_Molly
=
===========================
*/

void C_Molly(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		switch (ob->obclass)
		{
		case sandwichobj:
			playstate = ex_sandwich;
			break;

		case hookobj:
			playstate = ex_hook;
			break;

		case passcardobj:
			playstate = ex_card;
			break;

		case mollyobj:
			playstate = ex_molly;
		}
	}
}

/*
=============================================================================

						  MOLLY

=============================================================================
*/

statetype s_molly1        = {MOLLY1SPR, MOLLY1SPR, step, false, false, 20, 0, 0, NULL, C_Molly, R_Draw, &s_molly2};
statetype s_molly2        = {MOLLY2SPR, MOLLY2SPR, step, false, false, 40, 0, 0, NULL, C_Molly, R_Draw, &s_molly3};
statetype s_molly3        = {MOLLY1SPR, MOLLY1SPR, step, false, false, 40, 0, 0, NULL, C_Molly, R_Draw, &s_molly4};
statetype s_molly4        = {MOLLY2SPR, MOLLY2SPR, step, false, false, 20, 0, 0, NULL, C_Molly, R_Draw, &s_molly1};

/*
===========================
=
= SpawnMolly
=
===========================
*/

void SpawnMolly(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->obclass = mollyobj;
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
	NewState(new, &s_molly1);
}

/*
=============================================================================

						  PLATFORM

=============================================================================
*/

statetype s_platform      = {PLATFORMSPR, PLATFORMSPR, think, false, false, 0, 0, 0, T_Platform, NULL, R_Draw, NULL};

/*
===========================
=
= SpawnPlatform
=
===========================
*/

void SpawnPlatform(Uint16 tileX, Uint16 tileY, Sint16 dir)
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

/*
=============================================================================

						  DROPPING PLATFORM

temp1 = initial y position

=============================================================================
*/

statetype s_dropplatsit  = {PLATFORMSPR, PLATFORMSPR, think,      false, false, 0, 0,   0, T_DropPlatSit, NULL, R_Draw, NULL};
statetype s_fallplatfall = {PLATFORMSPR, PLATFORMSPR, think,      false, false, 0, 0,   0, T_DropPlatFall, NULL, R_Draw, NULL};
statetype s_fallplatrise = {PLATFORMSPR, PLATFORMSPR, slidethink, false, false, 0, 0, -32, T_DropPlatRise, NULL, R_Draw, NULL};

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
			ob->state = &s_fallplatfall;
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
				ob->state = &s_fallplatrise;
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
		ob->state = &s_fallplatfall;
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

statetype s_staticplatform = {PLATFORMSPR, PLATFORMSPR, step, false, false, 32000, 0, 0, NULL, NULL, R_Draw, &s_staticplatform};

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
	NewState(new, &s_staticplatform);
}

/*
=============================================================================

						  GO PLATFORM

temp1 = direction
temp2 = countdown to next dir check
temp3 = sprite pointer for the Bip sprite

=============================================================================
*/

statetype s_goplat        = {PLATFORMSPR, PLATFORMSPR, think, false, false, 0, 0, 0, T_GoPlat, NULL, R_GoPlat, NULL};

/*
===========================
=
= SpawnGoPlat
=
===========================
*/

void SpawnGoPlat(Uint16 tileX, Uint16 tileY, Sint16 dir)
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
	NewState(new, &s_goplat);
	*(mapsegs[2]+mapbwidthtable[tileY]/2 + tileX) = dir + DIRARROWSTART;
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
	Sint16 dir;
	Uint16 tx, ty;

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

/*
===========================
=
= R_GoPlat
=
===========================
*/

void R_GoPlat(objtype *ob)
{
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
	RF_PlaceSprite((void**)&ob->temp3, ob->x+TILEGLOBAL, ob->y+TILEGLOBAL, ob->temp1+PLATBIP1SPR, spritedraw, ob->priority);
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

						  BLOOG

=============================================================================
*/

statetype s_bloogwalk1 = {BLOOGWALKL1SPR, BLOOGWALKR1SPR, step,  false, true, 10, 128, 0, T_BloogWalk, C_Bloog, R_Walk, &s_bloogwalk2};
statetype s_bloogwalk2 = {BLOOGWALKL2SPR, BLOOGWALKR2SPR, step,  false, true, 10, 128, 0, T_BloogWalk, C_Bloog, R_Walk, &s_bloogwalk3};
statetype s_bloogwalk3 = {BLOOGWALKL3SPR, BLOOGWALKR3SPR, step,  false, true, 10, 128, 0, T_BloogWalk, C_Bloog, R_Walk, &s_bloogwalk4};
statetype s_bloogwalk4 = {BLOOGWALKL4SPR, BLOOGWALKR4SPR, step,  false, true, 10, 128, 0, T_BloogWalk, C_Bloog, R_Walk, &s_bloogwalk1};
statetype s_bloogstun  = {BLOOGSTUNSPR,   BLOOGSTUNSPR,   think, false, false, 0,   0, 0, T_Projectile, NULL, R_Stunned, &s_bloogstun};

/*
===========================
=
= SpawnBloog
=
===========================
*/

void SpawnBloog(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->obclass = bloogobj;
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
	NewState(new, &s_bloogwalk1);
}

/*
===========================
=
= T_BloogWalk
=
===========================
*/

void T_BloogWalk(objtype *ob)
{
	if (US_RndT() < 0x20)
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
}

/*
===========================
=
= C_Bloog
=
===========================
*/

void C_Bloog(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		KillKeen();
	}
	else if (hit->obclass == stunshotobj)
	{
		StunObj(ob, hit, &s_bloogstun);
	}
}

/*
=============================================================================

						  BLOOGUARD

temp1 = flash countdown
temp2 = health

=============================================================================
*/

statetype s_blooguardwalk1   = {BLOOGUARDWALKL1SPR,  BLOOGUARDWALKR1SPR,  step,  false, true,  9, 128, 0, T_BlooguardWalk, C_Blooguard, R_Blooguard, &s_blooguardwalk2};
statetype s_blooguardwalk2   = {BLOOGUARDWALKL2SPR,  BLOOGUARDWALKR2SPR,  step,  false, true,  9, 128, 0, T_BlooguardWalk, C_Blooguard, R_Blooguard, &s_blooguardwalk3};
statetype s_blooguardwalk3   = {BLOOGUARDWALKL3SPR,  BLOOGUARDWALKR3SPR,  step,  false, true,  9, 128, 0, T_BlooguardWalk, C_Blooguard, R_Blooguard, &s_blooguardwalk4};
statetype s_blooguardwalk4   = {BLOOGUARDWALKL4SPR,  BLOOGUARDWALKR4SPR,  step,  false, true,  9, 128, 0, T_BlooguardWalk, C_Blooguard, R_Blooguard, &s_blooguardwalk1};
statetype s_blooguardattack1 = {BLOOGUARDSWINGL1SPR, BLOOGUARDSWINGR1SPR, step,  false, true, 30,   0, 0, NULL, C_Blooguard, R_Blooguard, &s_blooguardattack2};
statetype s_blooguardattack2 = {BLOOGUARDSWINGL2SPR, BLOOGUARDSWINGR2SPR, step,  false, true,  9,   0, 0, NULL, C_Blooguard, R_Blooguard, &s_blooguardattack3};
statetype s_blooguardattack3 = {BLOOGUARDSWINGL3SPR, BLOOGUARDSWINGR3SPR, step,  true,  true,  1,   0, 0, T_BlooguardAttack, C_Blooguard, R_Blooguard, &s_blooguardattack4};
statetype s_blooguardattack4 = {BLOOGUARDSWINGL3SPR, BLOOGUARDSWINGR3SPR, step,  false, true,  9,   0, 0, NULL, C_Blooguard, R_Blooguard, &s_blooguardwalk1};
statetype s_blooguardstun    = {BLOOGUARDSTUNSPR,    BLOOGUARDSTUNSPR,    think, false, false, 0,   0, 0, T_Projectile, NULL, R_Stunned, &s_blooguardstun};

/*
===========================
=
= SpawnBlooguard
=
===========================
*/

void SpawnBlooguard(Uint16 tileX, Uint16 tileY)
{
	GetNewObj(false);
	new->obclass = blooguardobj;
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
	new->temp2 = 3;	// health
	NewState(new, &s_blooguardwalk1);
}

/*
===========================
=
= T_BlooguardWalk
=
===========================
*/

void T_BlooguardWalk(objtype *ob)
{
	if (US_RndT() < 0x20)
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
	if ( ((ob->xdir == 1 && ob->x < player->x) || (ob->xdir == -1 && ob->x > player->x))
		&& ob->bottom == player->bottom && US_RndT() < 0x20)
	{
		ob->state = &s_blooguardattack1;
	}
}

/*
===========================
=
= T_BlooguardAttack
=
===========================
*/

#pragma argsused
void T_BlooguardAttack(objtype *ob)
{
	SD_PlaySound(SND_SMASH);
	groundslam = 23;
	if (player->hitnorth)
	{
		ChangeState(player, &s_keenstun);
	}
}

/*
===========================
=
= C_Blooguard
=
===========================
*/

void C_Blooguard(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		KillKeen();
	}
	if (hit->obclass == stunshotobj)	// not 'else if' in the original code
	{
		if (--ob->temp2 == 0)	// handle health
		{
			StunObj(ob, hit, &s_blooguardstun);
		}
		else
		{
			ob->temp1 = 2;	// draw white twice
			ob->needtoreact = true;
			ExplodeShot(hit);
		}
	}
}

/*
===========================
=
= R_Blooguard
=
===========================
*/

void R_Blooguard(objtype *ob)
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
	if (ob->temp1)
	{
		ob->temp1--;
		RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, maskdraw, ob->priority);
	}
	else
	{
		RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
	}
}

/*
=============================================================================

						  BLOOGLET

temp1 = type

=============================================================================
*/

// red Blooglet:
statetype s_rbloogletwalk1 = {RBLOOGLETWALKL1SPR, RBLOOGLETWALKR1SPR, step,  false, true,  5, 128, 0, T_BloogWalk, C_Blooglet, R_Walk, &s_rbloogletwalk2};
statetype s_rbloogletwalk2 = {RBLOOGLETWALKL2SPR, RBLOOGLETWALKR2SPR, step,  false, true,  5, 128, 0, T_BloogWalk, C_Blooglet, R_Walk, &s_rbloogletwalk3};
statetype s_rbloogletwalk3 = {RBLOOGLETWALKL3SPR, RBLOOGLETWALKR3SPR, step,  false, true,  5, 128, 0, T_BloogWalk, C_Blooglet, R_Walk, &s_rbloogletwalk4};
statetype s_rbloogletwalk4 = {RBLOOGLETWALKL4SPR, RBLOOGLETWALKR4SPR, step,  false, true,  5, 128, 0, T_BloogWalk, C_Blooglet, R_Walk, &s_rbloogletwalk1};
statetype s_rbloogletstun  = {RBLOOGLETSTUNSPR,   RBLOOGLETSTUNSPR,   think, false, false, 0,   0, 0, T_Projectile, NULL, R_Stunned, NULL};

// yellow Blooglet:
statetype s_ybloogletwalk1 = {YBLOOGLETWALKL1SPR, YBLOOGLETWALKR1SPR, step,  false, true,  5, 128, 0, T_BloogWalk, C_Blooglet, R_Walk, &s_ybloogletwalk2};
statetype s_ybloogletwalk2 = {YBLOOGLETWALKL2SPR, YBLOOGLETWALKR2SPR, step,  false, true,  5, 128, 0, T_BloogWalk, C_Blooglet, R_Walk, &s_ybloogletwalk3};
statetype s_ybloogletwalk3 = {YBLOOGLETWALKL3SPR, YBLOOGLETWALKR3SPR, step,  false, true,  5, 128, 0, T_BloogWalk, C_Blooglet, R_Walk, &s_ybloogletwalk4};
statetype s_ybloogletwalk4 = {YBLOOGLETWALKL4SPR, YBLOOGLETWALKR4SPR, step,  false, true,  5, 128, 0, T_BloogWalk, C_Blooglet, R_Walk, &s_ybloogletwalk1};
statetype s_ybloogletstun  = {YBLOOGLETSTUNSPR,   YBLOOGLETSTUNSPR,   think, false, false, 0,   0, 0, T_Projectile, NULL, R_Stunned, NULL};

// blue Blooglet:
statetype s_bbloogletwalk1 = {BBLOOGLETWALKL1SPR, BBLOOGLETWALKR1SPR, step,  false, true,  5, 128, 0, T_BloogWalk, C_Blooglet, R_Walk, &s_bbloogletwalk2};
statetype s_bbloogletwalk2 = {BBLOOGLETWALKL2SPR, BBLOOGLETWALKR2SPR, step,  false, true,  5, 128, 0, T_BloogWalk, C_Blooglet, R_Walk, &s_bbloogletwalk3};
statetype s_bbloogletwalk3 = {BBLOOGLETWALKL3SPR, BBLOOGLETWALKR3SPR, step,  false, true,  5, 128, 0, T_BloogWalk, C_Blooglet, R_Walk, &s_bbloogletwalk4};
statetype s_bbloogletwalk4 = {BBLOOGLETWALKL4SPR, BBLOOGLETWALKR4SPR, step,  false, true,  5, 128, 0, T_BloogWalk, C_Blooglet, R_Walk, &s_bbloogletwalk1};
statetype s_bbloogletstun  = {BBLOOGLETSTUNSPR,   BBLOOGLETSTUNSPR,   think, false, false, 0,   0, 0, T_Projectile, NULL, R_Stunned, NULL};

// green Blooglet:
statetype s_gbloogletwalk1 = {GBLOOGLETWALKL1SPR, GBLOOGLETWALKR1SPR, step,  false, true,  5, 128, 0, T_BloogWalk, C_Blooglet, R_Walk, &s_gbloogletwalk2};
statetype s_gbloogletwalk2 = {GBLOOGLETWALKL2SPR, GBLOOGLETWALKR2SPR, step,  false, true,  5, 128, 0, T_BloogWalk, C_Blooglet, R_Walk, &s_gbloogletwalk3};
statetype s_gbloogletwalk3 = {GBLOOGLETWALKL3SPR, GBLOOGLETWALKR3SPR, step,  false, true,  5, 128, 0, T_BloogWalk, C_Blooglet, R_Walk, &s_gbloogletwalk4};
statetype s_gbloogletwalk4 = {GBLOOGLETWALKL4SPR, GBLOOGLETWALKR4SPR, step,  false, true,  5, 128, 0, T_BloogWalk, C_Blooglet, R_Walk, &s_gbloogletwalk1};
statetype s_gbloogletstun  = {GBLOOGLETSTUNSPR,   GBLOOGLETSTUNSPR,   think, false, false, 0,   0, 0, T_Projectile, NULL, R_Stunned, NULL};

/*
===========================
=
= SpawnBlooglet
=
===========================
*/

void SpawnBlooglet(Uint16 tileX, Uint16 tileY, Sint16 type)
{
	GetNewObj(false);
	new->obclass = bloogletobj;
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
	new->temp1 = type;

	switch (type % 4)
	{
	case 0:
		NewState(new, &s_rbloogletwalk1);
		break;

	case 1:
		NewState(new, &s_ybloogletwalk1);
		break;

	case 2:
		NewState(new, &s_bbloogletwalk1);
		break;

	case 3:
		NewState(new, &s_gbloogletwalk1);
	}
}

/*
===========================
=
= C_Blooglet
=
===========================
*/

void C_Blooglet(objtype *ob, objtype *hit)
{
	static statetype *stunnedstate[4] = {
		&s_rbloogletstun,
		&s_ybloogletstun,
		&s_bbloogletstun,
		&s_gbloogletstun
	};
	Sint16 color;

	if (hit->obclass == keenobj && hit->state->contact)
	{
		playerkludgeclipcancel = true;
		ClipToSpriteSide(hit, ob);
		playerkludgeclipcancel = false;
	}
	else if (hit->obclass == stunshotobj)
	{
		color = ob->temp1 & 3;
		if (ob->temp1 > 3)
		{
			//
			// spawn a key gem
			//
			GetNewObj(false);
			new->needtoclip = cl_midclip;
			new->priority = 2;
			new->obclass = bonusobj;
			new->x = ob->x;
			new->y = ob->y;
			new->ydir = -1;
			new->yspeed = -40;
			new->temp1 = color;
			new->temp2=new->shapenum = bonusshape[color];
			new->temp3 = new->temp2 + 2;
			NewState(new, &s_bonusfly1);
			SD_PlaySound(SND_DROPKEY);
		}
		StunObj(ob, hit, stunnedstate[color]);
	}
}