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
K6_SPEC.C
=========

Contains (in this order):

- lump definition
- "Star Wars" crawl text
- level names & messages
- ScanInfoPlane() - for spawning the level objects and marking required sprites
- stunned state for Keen
- code to flip the big yellow switches
- messages for sandwich, rope and passcard
*/

#include "CK_DEF.H"

enum {
	CONTROLS_LUMP,    //  0
	KEEN_LUMP,        //  1
	SUGAR1_LUMP,      //  2
	SUGAR2_LUMP,      //  3
	SUGAR3_LUMP,      //  4
	SUGAR4_LUMP,      //  5
	SUGAR5_LUMP,      //  6
	SUGAR6_LUMP,      //  7
	ONEUP_LUMP,       //  8
	KEYGEM_LUMP,      //  9
	AMMO_LUMP,        // 10
	WORLDKEEN_LUMP,   // 11
	UNUSED1_LUMP,     // 12
	BLOOG_LUMP,       // 13
	RBLOOGLET_LUMP,   // 14
	YBLOOGLET_LUMP,   // 15
	BBLOOGLET_LUMP,   // 16
	GBLOOGLET_LUMP,   // 17
	PLATFORM_LUMP,    // 18
	GIK_LUMP,         // 19
	BLORB_LUMP,       // 20
	BOBBA_LUMP,       // 21
	BABOBBA_LUMP,     // 22
	BLOOGUARD_LUMP,   // 23
	FLECT_LUMP,       // 24
	BIP_LUMP,         // 25
	BIPSQUISHED_LUMP, // 26
	BIPSHIP_LUMP,     // 27
	NOSPIKE_LUMP,     // 28
	ORBATRIX_LUMP,    // 29
	CEILICK_LUMP,     // 30
	FLEEX_LUMP,       // 31
	HOOK_LUMP,        // 32
	SANDWICH_LUMP,    // 33
	LASER_LUMP,       // 34
	PASSCARD_LUMP,    // 35
	MOLLY_LUMP,       // 36

	NUMLUMPS=40       // Keen 6 has 3 unused lumps at the end
};

Uint16 lumpstart[NUMLUMPS] = {
	CONTROLS_LUMP_START,
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
	WORLDKEEN_LUMP_START,
	0,
	BLOOG_LUMP_START,
	RBLOOGLET_LUMP_START,
	YBLOOGLET_LUMP_START,
	BBLOOGLET_LUMP_START,
	GBLOOGLET_LUMP_START,
	PLATFORM_LUMP_START,
	GIK_LUMP_START,
	BLORB_LUMP_START,
	BOBBA_LUMP_START,
	BABOBBA_LUMP_START,
	BLOOGUARD_LUMP_START,
	FLECT_LUMP_START,
	BIP_LUMP_START,
	BIPSQUISHED_LUMP_START,
	BIPSHIP_LUMP_START,
	NOSPIKE_LUMP_START,
	ORBATRIX_LUMP_START,
	CEILICK_LUMP_START,
	FLEEX_LUMP_START,
	HOOK_LUMP_START,
	SANDWICH_LUMP_START,
	LASER_LUMP_START,
	PASSCARD_LUMP_START,
	MOLLY_LUMP_START
};

Uint16 lumpend[NUMLUMPS] = {
	CONTROLS_LUMP_END,
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
	WORLDKEEN_LUMP_END,
	0,
	BLOOG_LUMP_END,
	RBLOOGLET_LUMP_END,
	YBLOOGLET_LUMP_END,
	BBLOOGLET_LUMP_END,
	GBLOOGLET_LUMP_END,
	PLATFORM_LUMP_END,
	GIK_LUMP_END,
	BLORB_LUMP_END,
	BOBBA_LUMP_END,
	BABOBBA_LUMP_END,
	BLOOGUARD_LUMP_END,
	FLECT_LUMP_END,
	BIP_LUMP_END,
	BIPSQUISHED_LUMP_END,
	BIPSHIP_LUMP_END,
	NOSPIKE_LUMP_END,
	ORBATRIX_LUMP_END,
	CEILICK_LUMP_END,
	FLEEX_LUMP_END,
	HOOK_LUMP_END,
	SANDWICH_LUMP_END,
	LASER_LUMP_END,
	PASSCARD_LUMP_END,
	MOLLY_LUMP_END
};

boolean lumpneeded[NUMLUMPS];

#if GRMODE == EGAGR

char far swtext[] =
	"Episode Six\n"
	"\n"
	"Aliens Ate My\n"
	"Baby Sitter!\n"
	"\n"
	"While out in his\n"
	"backyard clubhouse,\n"
	"Billy's baby sitter\n"
	"Molly calls him for\n"
	"dinner. He continues\n"
	"working on his new\n"
	"wrist computer.\n"
	"\n"
	"Suddenly, there is a\n"
	"loud noise outside.\n"
	"\n"
	"Rushing out, Keen finds\n"
	"his baby sitter gone\n"
	"and a note on a patch\n"
	"of scorched grass.  The\n"
	"Bloogs of Fribbulus Xax\n"
	"are going to make a\n"
	"meal out of Molly!\n"
	"\n"
	"You've got to rescue\n"
	"her, because your\n"
	"parents will never\n"
	"believe you when you\n"
	"tell them...\n"
	"\n"
	"\"Aliens Ate My\n"
	"Baby Sitter!\"\n";

#endif

char far l0n[] = "Fribbulus Xax";
char far l1n[] = "Bloogwaters\nCrossing";
char far l2n[] = "Guard Post One";
char far l3n[] = "First Dome\nof Darkness";
char far l4n[] = "Second Dome\nof Darkness";
char far l5n[] = "The Bloogdome";
char far l6n[] = "Bloogton Mfg.,\nIncorporated";
char far l7n[] = "Bloogton Tower";
char far l8n[] = "Bloogfoods, Inc.";
char far l9n[] = "Guard Post Two";
char far l10n[] = "Bloogville";
char far l11n[] = "BASA";
char far l12n[] = "Guard Post Three";
char far l13n[] = "Bloogbase Rec\nDistrict";
char far l14n[] = "Bloogbase Mgmt.\nDistrict";
char far l15n[] = "Bloog Control Center";
char far l16n[] = "Blooglab";
char far l17n[] = "Bean-with-Bacon\nMegarocket";
char far l18n[] = "High Scores";

char far l0e[] = "Keen attacks\nFribbulus Xax";
char far l1e[] = "Keen hops across\nBloogwaters\nCrossing";
char far l2e[] = "Keen fights his way\nthrough Guard Post One";
char far l3e[] = "Keen crosses into the\nFirst Dome of Darkness";
char far l4e[] = "Keen dares to enter the\nSecond Dome of Darkness";
char far l5e[] = "Keen foolishly enters\nthe Bloogdome";
char far l6e[] = "Keen makes his way\ninto Bloogton\nManufacturing";
char far l7e[] = "Keen ascends\nBloogton Tower";
char far l8e[] = "Keen hungrily enters\nBloogfoods, Inc.";
char far l9e[] = "Keen smashes through\nGuard Post Two";
char far l10e[] = "Keen seeks thrills\nin Bloogville";
char far l11e[] = "Keen rockets into the\nBloog Aeronautics and\nSpace Administration";
char far l12e[] = "Keen boldly assaults\nGuard Post Three";
char far l13e[] = "Keen whoops it up in\nthe Bloogbae\nRecreational District";	// sic!
char far l14e[] = "Keen purposefully struts\ninto the Bloogbase\nManagement District";
char far l15e[] = "Keen bravely enters the\nBloog Control Center,\nlooking for Molly";
char far l16e[] = "Keen warily enters\nBlooglab Space\nStation";
char far l17e[] = "Keen returns to the\nBean-with-Bacon\nMegarocket";
char far l18e[] = "Keen is in the High\nScore screen. Call Id!";

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
	l15n,
	l16n,
	l17n,
	l18n
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
	l15e,
	l16e,
	l17e,
	l18e
};

Uint16 bonuslump[] = {
	KEYGEM_LUMP, KEYGEM_LUMP, KEYGEM_LUMP, KEYGEM_LUMP,
	SUGAR1_LUMP, SUGAR2_LUMP, SUGAR3_LUMP,
	SUGAR4_LUMP, SUGAR5_LUMP, SUGAR6_LUMP,
	ONEUP_LUMP, AMMO_LUMP, AMMO_LUMP, 0, 0
};

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
				SpawnWorldKeen(x, y);
				SpawnScore();
				lumpneeded[WORLDKEEN_LUMP] = true;
				CA_MarkGrChunk(SCOREBOXSPR);
				break;

			case 6:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 5:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 4:
				SpawnBloog(x, y);
				lumpneeded[BLOOG_LUMP] = true;
				break;

			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
				SpawnBlooglet(x, y, info-7);
				lumpneeded[(info-7) % 4 + RBLOOGLET_LUMP] = true;
				if (info > 10)
					lumpneeded[KEYGEM_LUMP] = true;
				break;

			case 15:
			case 16:
				SpawnGrappleSpot(x, y, info-15);
				break;

			// case 17 is not used

			case 20:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 19:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 18:
				SpawnFleex(x, y);
				lumpneeded[FLEEX_LUMP] = true;
				break;

			case 21:
			case 22:
			case 23:
			case 24:
				SpawnMolly(x, y);
				lumpneeded[MOLLY_LUMP] = true;
				break;

			case 25:
				RF_SetScrollBlock(x, y, true);
				break;

			case 26:
				RF_SetScrollBlock(x, y, false);
				break;

			case 27:
			case 28:
			case 29:
			case 30:
				SpawnPlatform(x, y, info-27);
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
				SpawnGoPlat(x, y, info-36);
				lumpneeded[PLATFORM_LUMP] = true;
				lumpneeded[BIPSQUISHED_LUMP] = true;	// why?
				break;

			case 40:
				SpawnSneakPlat(x, y);
				lumpneeded[PLATFORM_LUMP] = true;
				break;

			case 43:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 42:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 41:
				SpawnBobba(x, y);
				lumpneeded[BOBBA_LUMP] = true;
				break;

			case 44:
			case 45:
				SpawnSatelliteStop(x, y, info-44);
				break;

			// case 46 is not used

			case 49:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 48:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 47:
				SpawnNospike(x, y);
				lumpneeded[NOSPIKE_LUMP] = true;
				break;

			case 52:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 51:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 50:
				SpawnGik(x, y);
				lumpneeded[GIK_LUMP] = true;
				break;

			case 53:
			case 54:
			case 55:
			case 56:
				SpawnCannon(x, y, info-53);
				lumpneeded[LASER_LUMP] = true;
				break;

			case 69:
				if (gamestate.ammo >= 5)
					break;
				info = 68;
				// no break here!
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

			case 72:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 71:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 70:
				SpawnOrbatrix(x, y);
				lumpneeded[ORBATRIX_LUMP] = true;
				break;

			case 75:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 74:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 73:
				SpawnBipship(x, y);
				lumpneeded[BIP_LUMP]=lumpneeded[BIPSHIP_LUMP]=lumpneeded[BIPSQUISHED_LUMP] = true;
				break;

			case 78:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 77:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 76:
				SpawnFlect(x, y);
				lumpneeded[FLECT_LUMP] = true;
				break;

			case 81:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 80:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 79:
				SpawnBlorb(x, y);
				lumpneeded[BLORB_LUMP] = true;
				break;

			case 84:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 83:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 82:
				SpawnCeilick(x, y);
				lumpneeded[CEILICK_LUMP] = true;
				break;

			case 87:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 86:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 85:
				SpawnBlooguard(x, y);
				lumpneeded[BLOOGUARD_LUMP] = true;
				break;

			case 88:
				SpawnGrabbiter(x, y);
				// no additional lump needed - sprites are in WORLDKEEN_LUMP
				break;

			case 89:
				SpawnSatellite(x, y);
				// no additional lump needed - sprites are in WORLDKEEN_LUMP
				break;

			// case 90 is not used
			// cases 91 to 98 are direction arrows

			case 99:
				SpawnHook(x, y);
				lumpneeded[HOOK_LUMP] = true;
				break;

			case 100:
				SpawnSandwich(x, y);
				lumpneeded[SANDWICH_LUMP] = true;
				break;

			case 101:
				SpawnPasscard(x, y);
				lumpneeded[PASSCARD_LUMP] = true;
				break;

			case 104:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 103:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 102:
				SpawnBabobba(x, y);
				lumpneeded[BABOBBA_LUMP] = true;
				break;

			case 105:
			case 106:
				SpawnRocket(x, y, info-105);
				// no additional lump needed - sprites are in WORLDKEEN_LUMP
				break;
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
}

//============================================================================

statetype s_keenstun = {KEENSTUNSPR, KEENSTUNSPR, step, false, true, 60, 0, 0, T_Projectile, KeenContact, KeenStandReact, &s_keenstand};

//============================================================================

/*
===========================
=
= FlipBigSwitch
=
===========================
*/

void FlipBigSwitch(objtype *ob, boolean isup)
{
	Uint16 x, y;
	Uint16 far *map;
	Uint16 top, mid, bot;
	Uint16 *tileptr;
	Uint16 tile, tx, ty, xi, yi, offset, anim;
	Uint16 tiles[6];

	//
	// handle flipping the switch itself:
	//
	if (isup)
	{
		ty = ob->tilebottom;
	}
	else
	{
		ty = ob->tiletop - 2;
	}
	tx = ob->tileleft - 1;
	map = mapsegs[2] + mapbwidthtable[ty+1]/2 + tx + 1;
	while (*map == 0)
	{
		map++;
		tx++;
	}
	map = mapsegs[1] + mapbwidthtable[ty]/2 + tx;
	tileptr = tiles;
	for (y = 0; y < 3; y++, map += mapwidth)
	{
		for (x = 0; x < 2; tileptr++, x++)
		{
			tile = map[x];
			*tileptr = tile + (Sint8)tinf[tile+MANIM];
		}
	}
	RF_MemToMap(tiles, 1, tx, ty, 2, 3);

	tile = *(mapsegs[2]+mapbwidthtable[ty+1]/2 + tx + 1);
	x = tile >> 8;
	y = tile & 0xFF;
	SD_PlaySound(SND_USESWITCH);

	//
	// toggle whatever was linked to the switch (at tile x, y):
	//
	offset = mapbwidthtable[y]/2 + x;
	map = mapsegs[2] + offset;
	tile = *map;

	if (tile >= DIRARROWSTART && tile < DIRARROWEND)
	{
		// turn direction arrow:
		*map = arrowflip[tile-DIRARROWSTART] + DIRARROWSTART;
	}
	else
	{
		map = mapsegs[1] + offset;
		tile = *map;
		switch (tinf[tile+INTILE] & INTILE_TYPEMASK)
		{
		case INTILE_NOTHING:	// no special tiles
			mapsegs[2][offset] ^= PLATFORMBLOCK;
			break;

		case INTILE_BRIDGE:	// bridge
			for (yi=y; y+2 > yi; yi++)
			{
				map = mapsegs[1] + mapbwidthtable[yi]/2 + x - (yi != y);
				for (xi = x - (yi != y); xi < mapwidth; xi++)
				{
					tile = *map;
					map++;
					anim = tinf[tile + MANIM];
					if (!anim)
						break;
					tile += (Sint8)anim;
					RF_MemToMap(&tile, 1, xi, yi, 1, 1);
				}
			}
			break;

		case INTILE_FORCEFIELD:	// active force field
			map = mapsegs[1];
			top = *map;
			mid = *++map;
			bot = *++map;
			map = mapsegs[1] + mapbwidthtable[y+1]/2 + x;

			RF_MemToMap(&top, 1, x, y++, 1, 1);
			while (tinf[*map+INTILE] == INTILE_DEADLY)
			{
				RF_MemToMap(&mid, 1, x, y++, 1, 1);
				map += mapwidth;
			}
			RF_MemToMap(&bot, 1, x, y, 1, 1);
			break;

		case INTILE_FORCEFIELDEND:	// inactive force field
			map = mapsegs[1] + 3;
			top = *map;
			mid = *++map;
			bot = *++map;
			map = mapsegs[1] + mapbwidthtable[y+1]/2 + x;

			RF_MemToMap(&top, 1, x, y++, 1, 1);
			while (tinf[*map+INTILE] != INTILE_FORCEFIELDEND)
			{
				RF_MemToMap(&mid, 1, x, y++, 1, 1);
				map += mapwidth;
			}
			RF_MemToMap(&bot, 1, x, y, 1, 1);
		}
	}
}

//============================================================================

/*
===========================
=
= GotSandwich
=
===========================
*/

void GotSandwich(void)
{
	SD_WaitSoundDone();
	SD_PlaySound(SND_QUESTITEM);
	CA_UpLevel();	// kinda useless without CA_CacheMarks or CA_SetGrPurge
	// BUG: haven't made anything purgable here, caching the pic may cause an "out of memory" crash
	CA_CacheGrChunk(KEENTALK1PIC);

	US_CenterWindow(26, 8);
	VWB_DrawPic(WindowX+WindowW-48, WindowY, KEENTALK1PIC);
	WindowW -= 48;
	PrintY += 12;
	US_CPrint(
		"This is the second\n"
		"biggest sandwich\n"
		"I ever saw!\n"
		);
	VW_UpdateScreen();
	VW_WaitVBL(30);
	IN_ClearKeysDown();
	IN_Ack();
	CA_DownLevel();
	gamestate.sandwichstate = 1;
}

/*
===========================
=
= GotHook
=
===========================
*/

void GotHook(void)
{
	SD_WaitSoundDone();
	SD_PlaySound(SND_QUESTITEM);
	CA_UpLevel();	// kinda useless without CA_CacheMarks or CA_SetGrPurge
	// BUG: haven't made anything purgable here, caching the pic may cause an "out of memory" crash
	CA_CacheGrChunk(KEENTALK1PIC);

	US_CenterWindow(26, 8);
	VWB_DrawPic(WindowX+WindowW-48, WindowY, KEENTALK1PIC);
	WindowW -= 48;
	PrintY += 12;
	US_CPrint(
		"Wow! A rope and\n"
		"grappling hook!\n"
		"They look useful!\n"
		);
	VW_UpdateScreen();
	VW_WaitVBL(30);
	IN_ClearKeysDown();
	IN_Ack();
	CA_DownLevel();
	gamestate.hookstate = 1;
}

/*
===========================
=
= GotPasscard
=
===========================
*/

void GotPasscard(void)
{
	SD_WaitSoundDone();
	SD_PlaySound(SND_QUESTITEM);
	CA_UpLevel();	// kinda useless without CA_CacheMarks or CA_SetGrPurge
	// BUG: haven't made anything purgable here, caching the pic may cause an "out of memory" crash
	CA_CacheGrChunk(KEENTALK1PIC);

	US_CenterWindow(26, 8);
	VWB_DrawPic(WindowX+WindowW-48, WindowY, KEENTALK1PIC);
	WindowW -= 48;
	PrintY += 4;
	US_CPrint(
		"What's this? Cool!\n"
		"A passcard for\n"
		"the Bloogstar Rocket!\n"
		"(It can fly through\n"
		"their force field.)"
		);
	VW_UpdateScreen();
	VW_WaitVBL(30);
	IN_ClearKeysDown();
	IN_Ack();
	CA_DownLevel();
	gamestate.passcardstate = 1;
}
