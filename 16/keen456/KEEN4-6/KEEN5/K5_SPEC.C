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
K5_SPEC.C
=========

Contains (in this order):

- lump definition
- "Star Wars" crawl text
- level names & messages
- Airlock opening & closing code
- ScanInfoPlane() - for spawning the level objects and marking required sprites
- game over animation
- messages for breaking fuses
*/

#include "CK_DEF.H"

enum {
	HELP_LUMP,        //  0
	CONTROLS_LUMP,    //  1
	KEENTALK_LUMP,    //  2
	LOADING_LUMP,     //  3
	PADDLE_LUMP,      //  4
	KEEN_LUMP,        //  5
	SUGAR1_LUMP,      //  6
	SUGAR2_LUMP,      //  7
	SUGAR3_LUMP,      //  8
	SUGAR4_LUMP,      //  9
	SUGAR5_LUMP,      // 10
	SUGAR6_LUMP,      // 11
	ONEUP_LUMP,       // 12
	KEYGEM_LUMP,      // 13
	AMMO_LUMP,        // 14
	LASER_LUMP,       // 15
	WORLDKEEN_LUMP,   // 16
	MASTER_LUMP,      // 17
	SHIKADI_LUMP,     // 18
	SHOCKSHUND_LUMP,  // 19
	SPHEREFUL_LUMP,   // 20
	SPARKY_LUMP,      // 21
	MINE_LUMP,        // 22
	SLICESTAR_LUMP,   // 23
	ROBORED_LUMP,     // 24
	SPIRO_LUMP,       // 25
	AMPTON_LUMP,      // 26
	VOLTE_LUMP,       // 27
	SLOTPLAT_LUMP,    // 28
	SPINDRED_LUMP,    // 29
	SHELLEY_LUMP,     // 30
	PLATFORM_LUMP,    // 31
	SMALLPLAT_LUMP,   // 32
	KEYCARD_LUMP,     // 33
	SCOTTIE_LUMP,     // 34
	FUSE_LUMP,        // 35
	STAREXPLODE_LUMP, // 36
	TELEPORT_LUMP,    // 37

	NUMLUMPS=42       // Keen 5 has 4 unused lumps at the end
};

Uint16 lumpstart[NUMLUMPS] = {
	HELP_LUMP_START,
	CONTROLS_LUMP_START,
	KEENTALK_LUMP_START,
	LOADING_LUMP_START,
	PADDLE_LUMP_START,
	KEEN_LUMP_START,
	SUGAR1_LUMP_START,
	SUGAR2_LUMP_START,
	SUGAR3_LUMP_START,
	SUGAR4_LUMP_START,
	SUGAR5_LUMP_START,
	SUGAR6_LUMP_START,
	ONEUP_LUMP_START,
	KEYGEM_LUMP_START,
	AMMO_LUMP_START,
	LASER_LUMP_START,
	WORLDKEEN_LUMP_START,
	MASTER_LUMP_START,
	SHIKADI_LUMP_START,
	SHOCKSHUND_LUMP_START,
	SPHEREFUL_LUMP_START,
	SPARKY_LUMP_START,
	MINE_LUMP_START,
	SLICESTAR_LUMP_START,
	ROBORED_LUMP_START,
	SPIRO_LUMP_START,
	AMPTON_LUMP_START,
	VOLTE_LUMP_START,
	SLOTPLAT_LUMP_START,
	SPINDRED_LUMP_START,
	SHELLEY_LUMP_START,
	PLATFORM_LUMP_START,
	MINIPLAT_LUMP_START,
	KEYCARD_LUMP_START,
	SCOTTIE_LUMP_START,
	FUSE_LUMP_START,
	STAREXPLODE_LUMP_START,
	TELEPORT_LUMP_START
};

Uint16 lumpend[NUMLUMPS] = {
	HELP_LUMP_END,
	CONTROLS_LUMP_END,
	KEENTALK_LUMP_END,
	LOADING_LUMP_END,
	PADDLE_LUMP_END,
	KEEN_LUMP_END,
	SUGAR1_LUMP_END,
	SUGAR2_LUMP_END,
	SUGAR3_LUMP_END,
	SUGAR4_LUMP_END,
	SUGAR5_LUMP_END,
	SUGAR6_LUMP_END,
	ONEUP_LUMP_END,
	KEYGEM_LUMP_END,
	AMMO_LUMP_END,
	LASER_LUMP_END,
	WORLDKEEN_LUMP_END,
	MASTER_LUMP_END,
	SHIKADI_LUMP_END,
	SHOCKSHUND_LUMP_END,
	SPHEREFUL_LUMP_END,
	SPARKY_LUMP_END,
	MINE_LUMP_END,
	SLICESTAR_LUMP_END,
	ROBORED_LUMP_END,
	SPIRO_LUMP_END,
	AMPTON_LUMP_END,
	VOLTE_LUMP_END,
	SLOTPLAT_LUMP_END,
	SPINDRED_LUMP_END,
	SHELLEY_LUMP_END,
	PLATFORM_LUMP_END,
	MINIPLAT_LUMP_END,
	KEYCARD_LUMP_END,
	SCOTTIE_LUMP_END,
	FUSE_LUMP_END,
	STAREXPLODE_LUMP_END,
	TELEPORT_LUMP_END
};

boolean lumpneeded[NUMLUMPS];

#if GRMODE == EGAGR

char far swtext[] =
	"Episode Five\n"
	"\n"
	"The Armageddon Machine\n"
	"\n"
	"After learning the\n"
	"location of the secret\n"
	"Shikadi base, Keen\n"
	"jumped in the trusty\n"
	"Bean-with-Bacon\n"
	"Megarocket and blasted\n"
	"across interstellar\n"
	"space.\n"
	"\n"
	"Seventy-five furious\n"
	"games of Paddle War\n"
	"later, Keen dropped\n"
	"out of lightspeed near\n"
	" the Korath system.\n"
	"\n"
	"He flew toward the\n"
	"planet, keeping it\n"
	"between him and the\n"
	"base.\n"
	"\n"
	"Pulling up underside\n"
	"and docking at the\n"
	"Ion Ventilation System,\n"
	"Keen must destroy the\n"
	"Shikadi Armageddon\n"
	"Machine before it\n"
	"explodes and destroys\n"
	"the Milky Way!  He\n"
	"steps into the dark\n"
	"ventilation duct and\n"
	"begins his most\n"
	"dangerous adventure\n"
	"yet...\n";

#endif

char far l0n[] = "Omegamatic";
char far l1n[] = "Ion Ventilation System";
char far l2n[] = "Security Center";
char far l3n[] = "Defense Tunnel Vlook";
char far l4n[] = "Energy Flow Systems";
char far l5n[] = "Defense Tunnel Burrh";
char far l6n[] = "Regulation\nControl Center";
char far l7n[] = "Defense Tunnel Sorra";
char far l8n[] = "Neutrino\nBurst Injector";
char far l9n[] = "Defense Tunnel Teln";
char far l10n[] = "Brownian\nMotion Inducer";
char far l11n[] = "Gravitational\nDamping Hub";
char far l12n[] = "Quantum\nExplosion Dynamo";
char far l13n[] = "Korath III Base";
char far l14n[] = "BWBMegarocket";
char far l15n[] = "High Scores";

char far l0e[] = "Keen purposefully\nwanders about the\nOmegamatic";
char far l1e[] = "Keen investigates the\nIon Ventilation System";
char far l2e[] = "Keen struts through\nthe Security Center";
char far l3e[] = "Keen invades\nDefense Tunnel Vlook";
char far l4e[] = "Keen engages\nEnergy Flow Systems";
char far l5e[] = "Keen barrels into\nDefense Tunnel Burrh";
char far l6e[] = "Keen goes nuts in\nthe Regulation\nControl Center";
char far l7e[] = "Keen regrets entering\nDefense Tunnel Sorra";
char far l8e[] = "Keen blows through\nthe Neutrino\nBurst Injector";
char far l9e[] = "Keen trots through\nDefense Tunnel Teln";
char far l10e[] = "Keen breaks into\nthe Brownian\nMotion Inducer";
char far l11e[] = "Keen hurries through\nthe Gravitational\nDamping Hub";
char far l12e[] = "Keen explodes into\nthe Quantum\nExplosion Dynamo";
char far l13e[] = "Keen faces danger\nin the secret\nKorath III Base";
char far l14e[] = "Keen will not be\nin the BWBMegarocket";
char far l15e[] = "Keen unexplainedly\nfinds himself by\ntheHigh Scores";	// sic!

char far *levelnames[GAMELEVELS] = {
	l0n,
	l1n,
	l2n,
	l3n,
	l4n,
	l5n,
	l6n,
	l7n,
	l8n,
	l9n,
	l10n,
	l11n,
	l12n,
	l13n,
	l14n,
	l15n
};

char far *levelenter[GAMELEVELS] = {
	l0e,
	l1e,
	l2e,
	l3e,
	l4e,
	l5e,
	l6e,
	l7e,
	l8e,
	l9e,
	l10e,
	l11e,
	l12e,
	l13e,
	l14e,
	l15e
};

Uint16 bonuslump[] = {
	KEYGEM_LUMP, KEYGEM_LUMP, KEYGEM_LUMP, KEYGEM_LUMP,
	SUGAR1_LUMP, SUGAR2_LUMP, SUGAR3_LUMP,
	SUGAR4_LUMP, SUGAR5_LUMP, SUGAR6_LUMP,
	ONEUP_LUMP, AMMO_LUMP, KEYCARD_LUMP, 0, 0
};

//============================================================================

/*
===========================
=
= OpenMapDoor
=
===========================
*/

void OpenMapDoor(Sint16 tileX, Sint16 tileY)
{
	Sint16 x, y;
	Uint16 tiles[2][2];

	for (y=0; y<2; y++)
	{
		for (x=0; x<2; x++)
		{
			tiles[y][x] = *(mapsegs[1]+mapbwidthtable[y]/2 + x + 10);
		}
	}
	RF_MemToMap(&tiles[0][0], 1, tileX, tileY, 2, 2);
}

/*
===========================
=
= CloseMapDoor
=
===========================
*/

void CloseMapDoor(Sint16 tileX, Sint16 tileY)
{
	Sint16 x, y;
	Uint16 tiles[2][2];

	for (y=0; y<2; y++)
	{
		for (x=0; x<2; x++)
		{
			tiles[y][x] = *(mapsegs[1]+mapbwidthtable[y]/2 + x);
		}
	}
	RF_MemToMap(&tiles[0][0], 1, tileX, tileY, 2, 2);
}

//============================================================================

/*
==========================
=
= ScanInfoPlane
=
= Spawn all actors and mark down special places
=
==========================
*/

void ScanInfoPlane(void)
{
	Uint16 i, x, y, chunk;
	Sint16 info;
	Uint16 far *map;
	objtype *ob;

	InitObjArray();                  // start spawning things with a clean slate

	memset(lumpneeded, 0, sizeof(lumpneeded));
	gamestate.numfuses = 0;

	map = mapsegs[2];

	for (y=0; y<mapheight; y++)
	{
		for (x=0; x<mapwidth; x++)
		{
			info = *map++;

			if (info == 0)
				continue;

			switch (info)
			{
			case 1:
				SpawnKeen(x, y, 1);
				SpawnScore();
				lumpneeded[KEEN_LUMP] = true;
				CA_MarkGrChunk(SCOREBOXSPR);
				break;

			case 2:
				SpawnKeen(x, y, -1);
				SpawnScore();
				lumpneeded[KEEN_LUMP] = true;
				CA_MarkGrChunk(SCOREBOXSPR);
				break;

			case 3:
				SpawnScore();
				lumpneeded[WORLDKEEN_LUMP] = true;
				CA_MarkGrChunk(SCOREBOXSPR);
				if (playstate == ex_portout)
					break;
				SpawnWorldKeen(x, y);
				break;

			case 26:
				if (playstate != ex_portout)
					break;
				SpawnWorldKeenPort(x, y);
				break;

			case 6:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 5:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 4:
				SpawnSparky(x, y);
				lumpneeded[SPARKY_LUMP] = true;
				break;

			case 9:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 8:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 7:
				SpawnMine(x, y);
				lumpneeded[MINE_LUMP] = true;
				break;

			case 12:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 11:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 10:
				SpawnSlicestarSlide(x, y, 0);
				lumpneeded[SLICESTAR_LUMP] = true;
				break;

			case 15:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 14:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 13:
				SpawnRoboRed(x, y);
				lumpneeded[ROBORED_LUMP] = true;
				break;

			case 18:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 17:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 16:
				SpawnSpirogrip(x, y);
				lumpneeded[SPIRO_LUMP] = true;
				break;

			case 21:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 20:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 19:
				SpawnSlicestarBounce(x, y);
				lumpneeded[SLICESTAR_LUMP] = true;
				break;

			case 24:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 23:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 22:
				SpawnSlicestarSlide(x, y, 1);
				lumpneeded[SLICESTAR_LUMP] = true;
				break;

			case 25:
				RF_SetScrollBlock(x, y, true);
				break;

			// case 26 (teleported map keen) is handled above

			case 27:
			case 28:
			case 29:
			case 30:
				SpawnPlatform(x, y, info-27, 0);
				lumpneeded[PLATFORM_LUMP] = true;
				break;

			// case 31 is the block icon

			case 32:
				SpawnDropPlat(x, y);
				lumpneeded[PLATFORM_LUMP] = true;
				break;

			case 35:
				SpawnStaticPlat(x, y);
				lumpneeded[PLATFORM_LUMP] = true;
				break;
			case 34:
				if (gamestate.difficulty > gd_Normal)
					break;
				SpawnStaticPlat(x, y);
				lumpneeded[PLATFORM_LUMP] = true;
				break;
			case 33:
				if (gamestate.difficulty > gd_Easy)
					break;
				SpawnStaticPlat(x, y);
				lumpneeded[PLATFORM_LUMP] = true;
				break;

			case 36:
			case 37:
			case 38:
			case 39:
				SpawnGoPlat(x, y, info-36, 0);
				lumpneeded[PLATFORM_LUMP] = true;
				break;

			case 40:
				SpawnSneakPlat(x, y);
				lumpneeded[PLATFORM_LUMP] = true;
				break;

			case 41:
				if (gamestate.mapon == 12)
				{
					gamestate.numfuses = 4;
					SpawnQed(x, y);
				}
				else
				{
					gamestate.numfuses++;
				}
				lumpneeded[FUSE_LUMP] = true;
				break;

			case 44:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 43:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 42:
				SpawnAmpton(x, y);
				lumpneeded[AMPTON_LUMP] = true;
				break;

			case 53:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 49:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 45:
				SpawnCannon(x, y, 0);
				lumpneeded[LASER_LUMP] = true;
				break;

			case 54:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 50:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 46:
				SpawnCannon(x, y, 1);
				lumpneeded[LASER_LUMP] = true;
				break;

			case 55:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 51:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 47:
				SpawnCannon(x, y, 2);
				lumpneeded[LASER_LUMP] = true;
				break;

			case 56:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 52:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 48:
				SpawnCannon(x, y, 3);
				lumpneeded[LASER_LUMP] = true;
				break;

			case 69:
				if (gamestate.ammo >= 5)
					break;
				info = 68;	// spawn ammo
				//no break here!
			case 57:
			case 58:
			case 59:
			case 60:
			case 61:
			case 62:
			case 63:
			case 64:
			case 65:
			case 66:
			case 67:
			case 68:
				SpawnBonus(x, y, info-57);
				lumpneeded[bonuslump[info-57]] = true;
				break;
			case 70:
				SpawnBonus(x, y, info-58);
				lumpneeded[bonuslump[info-58]] = true;
				break;

			case 73:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 72:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 71:
				SpawnVolte(x, y);
				lumpneeded[VOLTE_LUMP] = true;
				break;

			case 76:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 75:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 74:
				SpawnShelly(x, y);
				lumpneeded[SHELLEY_LUMP] = true;
				break;

			case 79:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 78:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 77:
				SpawnSpindread(x, y);
				lumpneeded[SPINDRED_LUMP] = true;
				break;

			case 80:
			case 81:
			case 82:
			case 83:
				SpawnGoPlat(x, y, info-80, 1);
				lumpneeded[SLOTPLAT_LUMP] = true;
				break;

			case 84:
			case 85:
			case 86:
			case 87:
				SpawnPlatform(x, y, info-84, 1);
				lumpneeded[SLOTPLAT_LUMP] = true;
				break;

			case 90:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 89:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 88:
				SpawnMaster(x, y);
				lumpneeded[MASTER_LUMP] = true;
				break;

			// cases 91 to 98 are direction arrows

			case 101:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 100:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 99:
				SpawnShikadi(x, y);
				lumpneeded[SHIKADI_LUMP] = true;
				break;

			case 104:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 103:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 102:
				SpawnPet(x, y);
				lumpneeded[SHOCKSHUND_LUMP] = true;
				break;

			case 107:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 106:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 105:
				SpawnSphereful(x, y);
				lumpneeded[SPHEREFUL_LUMP] = true;
				break;

			// cases 108 to 123 are wall collision icons for the WallDebug cheat

			case 124:
				SpawnScottie(x, y);
				lumpneeded[SCOTTIE_LUMP] = true;
				break;

			case 125:
				lumpneeded[TELEPORT_LUMP] = true;

			}
		}
	}

	for (ob = player; ob; ob = ob->next)
	{
		if (ob->active != ac_allways)
			ob->active = ac_no;
	}

	for (i = 0; i < NUMLUMPS; i++)
	{
		if (lumpneeded[i])
		{
			for (chunk = lumpstart[i]; chunk <= lumpend[i]; chunk++)
			{
				CA_MarkGrChunk(chunk);
			}
		}
	}

	// Keen 5 addition to PatchWorldMap (PatchWorldMap is shared across Keen 4-6)
	if (gamestate.mapon == 0)
	{
		info = CONVERT_GLOBAL_TO_TILE(player->y);
		if (info < 75 || info > 100)
		{
			CloseMapDoor(24, 76);
			OpenMapDoor(22, 55);
		}
		if (gamestate.leveldone[4] && gamestate.leveldone[6] && gamestate.leveldone[8] && gamestate.leveldone[10]
			&& (info > 39 || info > 100) )
		{
			OpenMapDoor(26, 55);
		}
		if (info <= 39 || info > 100)
		{
			OpenMapDoor(24, 30);
		}
	}
}

/*
=============================================================================

						  GAME OVER SEQUENCE

=============================================================================
*/

/*
------------------------------------------------------------------------------
The galaxy explosion chunk contains data in the following format:

struct {
	Uint16 posx[4000];
	Uint16 velx[4000];
	Uint16 posy[4000];
	Uint16 vely[4000];
};

The values are stored as "fixed point" numbers (divide each number by 128 to
get the amount in pixel units). The code in MoveStars basically does the
following:

1.	set all pixels on the screen to black

2.	for each of the 4000 entries do:

2.1	x := posx[i] + velx[i]

2.2	if x (in pixels) is < 0 or > 320 then go to 2.8

2.3	posx[i] := x

2.4	y := posy[i] + vely[i]

2.5	if y (in pixels) is < 0 or > 200 then go to 2.8

2.6	posy[i] := y

2.7	draw a white pixel at position (x, y) on the screen

2.8	continue with the next entry

------------------------------------------------------------------------------
*/

#if GRMODE == CGAGR

Uint8 plotpixels[8] = {0xC0, 0x30, 0x0C, 0x03};

/*
===========================
=
= MoveStars   CGA
=
===========================
*/

void MoveStars(void)
{
	asm {
		mov	ax, screenseg;
		mov	es, ax;
		mov	ds, word ptr grsegs + 2*GALAXY;
		mov	cx, 2000h;
		xor	ax, ax;
		xor	di, di;
		rep stosw;
		mov	si, 7998;
	}
l1:
	asm {
		mov	ax, [si];		// get posx
		add	ax, [si+8000];		// add velx
		cmp	ax, 40960;		// check if new posx is on screen
		ja 	l2;
		mov	[si], ax;		// set new posx
		mov	bx, [si+16000];		// get posy
		add	bx, [si+24000];		// add vely
		cmp	bx, 25600;		// check if new posy is on screen
		ja 	l2;
		mov	[si+16000], bx;		// set new posy
		mov	cl, 7;
		shr	bx, cl;
		shl	bx, 1;
		mov	di, word ptr ss:ylookup[bx];
		mov	bx, ax;
		mov	cl, 9;
		shr	ax, cl;
		add	di, ax;
		mov	cl, 7;
		shr	bx, cl;
		and	bx, 3;
		mov	al, BYTE PTR ss:plotpixels[bx];
		or		es:[di], al;		// draw the pixel
	}
l2:
	asm {
		sub	si, 2;
		jns	l1;
		mov	ax, ss;
		mov	ds, ax;
	}
}

/*
===========================
=
= GameOver   CGA
=
===========================
*/

void GameOver(void)
{
	Sint16 i;

	FreeGraphics();
	CA_CacheGrChunk(MILKYWAYPIC);
	CA_CacheGrChunk(GALAXY);
	CA_CacheGrChunk(GAMEOVERPIC);
	RF_FixOfs();
	VW_ClearVideo(BLACK);
	VWB_DrawPic(0, 0, MILKYWAYPIC);
	VW_UpdateScreen();

	SD_WaitSoundDone();
	SD_PlaySound(SND_GAMEOVER2);
	SD_WaitSoundDone();
	SD_PlaySound(SND_GAMEOVER1);

	IN_ClearKeysDown();
	VW_SetLineWidth(80);

	for (i=0; i<60; i++)
	{
		lasttimecount = TimeCount;
		MoveStars();
		VW_UpdateScreen();

		do {} while (TimeCount-lasttimecount < 4);

		if (LastScan)
			break;
	}

	VW_SetLineWidth(SCREENWIDTH);
	VW_ClearVideo(BLACK);
	StartMusic(18);
	VWB_DrawPic(32, 80, GAMEOVERPIC);
	VW_UpdateScreen();
	IN_UserInput(24*TickBase, false);
	StopMusic();
}

//============================================================================

#else

Uint16 dim[18] = {8, 8, 7, 15, 7, 8, 0, 8, 7, 15, 7, 8, 0, 0, 0, 0, 0, 0};
Uint16 bright[18] = {7, 7, 7, 7, 7, 15, 7, 8, 0, 7, 15, 7, 8, 0, 0, 0, 0, 0};
Uint8 galaxycolors[17] = {0, 1, 2, 3, 4, 5, 6, 7, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 3};
Uint8 plotpixels[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

/*
===========================
=
= MoveStars   EGA
=
===========================
*/

void MoveStars(Uint16 dest)
{
	asm {
		mov	dx, GC_INDEX;
		mov	al, GC_BITMASK;
		out	dx, al;
		inc	dx;
		mov	ds, word ptr grsegs + 2*GALAXY;
		mov	ax, 0A000h;
		mov	es, ax;
		mov	cx, 1000h;
		xor	ax, ax;
		mov	di, dest;
		rep stosw;
		mov	si, 7998;
	}
l1:
	asm {
		mov	ax, [si];		// get posx
		add	ax, [si+8000];		// add velx
		cmp	ax, 40960;		// check if new posx is on screen
		ja 	l2;
		mov	[si], ax;		// set new posx
		mov	bx, [si+16000];		// get posy
		add	bx, [si+24000];		// add vely
		cmp	bx, 25600;		// check if new posy is on screen
		ja 	l2;
		mov	[si+16000], bx;		// set new posy
		mov	cl, 7;
		shr	bx, cl;
		shl	bx, 1;
		mov	di, word ptr ss:ylookup[bx];
		add	di, dest;
		mov	bx, ax;
		mov	cl, 10;
		shr	ax, cl;
		add	di, ax;
		mov	cl, 7;
		shr	bx, cl;
		and	bx, 7;
		mov	al, BYTE PTR ss:plotpixels[bx];
		out	dx, al;
		mov	al, 0Fh;
		xchg	al, es:[di];		// draw the pixel
	}
l2:
	asm {
		sub	si, 2;
		jns	l1;
		mov	ax, ss;
		mov	ds, ax;
		mov	al, 0FFh;
		out	dx, al;
	}
}

/*
===========================
=
= SetCrtc   EGA
=
===========================
*/

void SetCrtc(Uint16 addr)
{
	asm {
		cli;
		mov	cx, addr;
		mov	dx, CRTC_INDEX;
		mov	al, CRTC_STARTHIGH;
		out	dx, al;
		inc	dx;
		mov	al, ch;
		out	dx, al;
		dec	dx;
		mov	al, CRTC_STARTLOW;
		out	dx, al;
		inc	dx;
		mov	al, cl;
		out	dx, al;
		sti;
	}
}

/*
===========================
=
= GameOver   EGA
=
===========================
*/

void GameOver(void)
{
	Sint16 i;

	FreeGraphics();
	VW_FadeOut();
	CA_CacheGrChunk(MILKYWAYPIC);
	CA_CacheGrChunk(GALAXY);
	CA_CacheGrChunk(GAMEOVERPIC);
	VW_SetLineWidth(40);
	VW_SetScreen(0, 0);
	bufferofs = 0;
	VW_ClearVideo(0);
	VW_DrawPic(0, 0, MILKYWAYPIC);
	VW_ScreenToScreen(0, 0x2000, 40, 200);
	VW_FadeIn();
	IN_ClearKeysDown();
	SD_PlaySound(SND_GAMEOVER2);

	for (i=0; i<18; i++)
	{
		galaxycolors[8] = dim[i];
		galaxycolors[7] = bright[i];

		SetPalette(galaxycolors);

		VW_WaitVBL(10);
		if (LastScan)
			goto gameover;
	}

	EGAWRITEMODE(2);
	EGAMAPMASK(15);
	SD_PlaySound(SND_GAMEOVER1);

	for (i=0; i<30; i++)
	{
		lasttimecount = TimeCount;
		MoveStars(0x2000);
		SetCrtc(0x2000);
		do {} while (TimeCount-lasttimecount < 4);

		lasttimecount = TimeCount;
		MoveStars(0);
		SetCrtc(0);
		do {} while (TimeCount-lasttimecount < 4);

		if (LastScan)
			goto gameover;
	}

gameover:
	EGAWRITEMODE(0);
	VW_ClearVideo(BLACK);
	VW_SetLineWidth(SCREENWIDTH);
	VW_SetDefaultColors();
	RF_FixOfs();
	StartMusic(18);
	VWB_DrawPic(32, 80, GAMEOVERPIC);
	VW_UpdateScreen();
	IN_UserInput(24*TickBase, false);
	StopMusic();
}

#endif

//============================================================================

/*
===========================
=
= FinishedFuse
=
===========================
*/

void FinishedFuse(void)
{
	SD_WaitSoundDone();
	CA_UpLevel();
#if 0
	// bugfix:
	CA_ClearMarks();	// don't cache more than we actually need here
#endif
	CA_MarkGrChunk(KEENTALK1PIC);
	CA_MarkGrChunk(KEENTALK2PIC);
	CA_CacheMarks(NULL);

	VW_FixRefreshBuffer();
	US_CenterWindow(26, 8);
	WindowW -= 48;
	VWB_DrawPic(WindowX+WindowW, WindowY, KEENTALK1PIC);
	PrintY += 12;
	if (gamestate.mapon == 13)
	{
		US_CPrint(
			"I wonder what that\n"
			"fuse was for....\n"
			);
	}
	else
	{
		US_CPrint(
			"One of the four\n"
			"machines protecting the\n"
			"main elevator shaft--\n"
			"toast!\n"
			);
	}
	VW_UpdateScreen();
	VW_WaitVBL(30);
	IN_ClearKeysDown();
	IN_Ack();

	VWB_DrawPic(WindowX+WindowW, WindowY, KEENTALK2PIC);
	VW_UpdateScreen();
	VW_WaitVBL(30);
	IN_ClearKeysDown();
	IN_Ack();

	CA_DownLevel();
	StopMusic();
}