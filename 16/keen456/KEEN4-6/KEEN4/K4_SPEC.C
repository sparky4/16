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
K4_SPEC.C
=========

Contains (in this order):

- lump definition
- "Star Wars" crawl text
- level names & messages
- ScanInfoPlane() - for spawning the level objects and marking required sprites
- messages for Lindsey, Janitor, Oracle Members and more

- actor states & implementation for swimming Keen

*/

#include "CK_DEF.H"

enum {
	CONTROLS_LUMP,     //  0
	KEEN_LUMP,         //  1
	SUGAR1_LUMP,       //  2
	SUGAR2_LUMP,       //  3
	SUGAR3_LUMP,       //  4
	SUGAR4_LUMP,       //  5
	SUGAR5_LUMP,       //  6
	SUGAR6_LUMP,       //  7
	ONEUP_LUMP,        //  8
	AMMO_LUMP,         //  9
	WOLRDKEEN_LUMP,    // 10
	SLUG_LUMP,         // 11
	MADMUSHROOM_LUMP,  // 12
	UNUSED1_LUMP,      // 13
	LINDSEY_LUMP,      // 14
	INCHWORM_LUMP,     // 15
	EATER_LUMP,        // 16
	COUNCIL_LUMP,      // 17
	EGGBIRD_LUMP,      // 18
	MIMROCK_LUMP,      // 19
	DOPEFISH_LUMP,     // 20
	SCHOOLFISH_LUMP,   // 21
	ARACHNUT_LUMP,     // 22
	SKYPEST_LUMP,      // 23
	WORMOUTH_LUMP,     // 24
	LICK_LUMP,         // 25
	PLATFORM_LUMP,     // 26
	BOUNDER_LUMP,      // 27
	THUNDERCLOUD_LUMP, // 28
	BERKELOID_LUMP,    // 29
	KEYGEM_LUMP,       // 30
	DARTS_LUMP,        // 31
	SCUBAKEEN_LUMP,    // 32
	SPRITE_LUMP,       // 33
	MINE_LUMP,         // 34
	MOON_LUMP,         // 35
	EGG_LUMP,          // 36
	NUMLUMPS           // 37
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
	AMMO_LUMP_START,
	WORLDKEEN_LUMP_START,
	SLUG_LUMP_START,
	MADMUSHROOM_LUMP_START,
	0,
	LINDSEY_LUMP_START,
	INCHWORM_LUMP_START,
	EATER_LUMP_START,
	COUNCIL_LUMP_START,
	EGGBIRD_LUMP_START,
	MIMROCK_LUMP_START,
	DOPEFISH_LUMP_START,
	SCHOOLFISH_LUMP_START,
	ARACHNUT_LUMP_START,
	SKYPEST_LUMP_START,
	WORMOUTH_LUMP_START,
	LICK_LUMP_START,
	PLATFORM_LUMP_START,
	BOUNDER_LUMP_START,
	THUNDERCLOUD_LUMP_START,
	BERKELOID_LUMP_START,
	KEYGEM_LUMP_START,
	DARTS_LUMP_START,
	SCUBAKEEN_LUMP_START,
	SPRITE_LUMP_START,
	MINE_LUMP_START,
	MOON_LUMP_START,
	EGG_LUMP_START
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
	AMMO_LUMP_END,
	WORLDKEEN_LUMP_END,
	SLUG_LUMP_END,
	MADMUSHROOM_LUMP_END,
	0,
	LINDSEY_LUMP_END,
	INCHWORM_LUMP_END,
	EATER_LUMP_END,
	COUNCIL_LUMP_END,
	EGGBIRD_LUMP_END,
	MIMROCK_LUMP_END,
	DOPEFISH_LUMP_END,
	SCHOOLFISH_LUMP_END,
	ARACHNUT_LUMP_END,
	SKYPEST_LUMP_END,
	WORMOUTH_LUMP_END,
	LICK_LUMP_END,
	PLATFORM_LUMP_END,
	BOUNDER_LUMP_END,
	THUNDERCLOUD_LUMP_END,
	BERKELOID_LUMP_END,
	KEYGEM_LUMP_END,
	DARTS_LUMP_END,
	SCUBAKEEN_LUMP_END,
	SPRITE_LUMP_END,
	MINE_LUMP_END,
	MOON_LUMP_END,
	EGG_LUMP_END
};

boolean lumpneeded[NUMLUMPS];

#if GRMODE == EGAGR

char far swtext[] =
	"Episode Four\n"
	"\n"
	"Secret of the Oracle\n"
	"\n"
	"After delivering a\n"
	"crippling blow to the\n"
	"plans of Mortimer\n"
	"McMire and receiving\n"
	"the praise of the\n"
	"Vorticon race,\n"
	"Commander Keen\n"
	"returned to his home in\n"
	"the suburbs.\n"
	"\n"
	"Here he was forced to\n"
	"go to bed at an early\n"
	"hour, and to eat mashed\n"
	"potatoes.\n"
	"\n"
	"Months later, Billy\n"
	"tinkered around with\n"
	"his latest invention,\n"
	"the Photachyon\n"
	"Transceiver, or faster-\n"
	"than-light radio. After\n"
	"picking up a lot of bad\n"
	"alien sitcoms, he\n"
	"stumbled upon a strange\n"
	"message of terrible\n"
	"importance....\n";

#endif

char far l0n[] = "Shadowlands";
char far l1n[] = "Border Village";
char far l2n[] = "Slug Village";
char far l3n[] = "The Perilous Pit";
char far l4n[] = "Cave of the Descendents";
char far l5n[] = "Chasm of Chills";
char far l6n[] = "Crystalus";
char far l7n[] = "Hillville";
char far l8n[] = "Sand Yego";
char far l9n[] = "Miragia";
char far l10n[] = "Lifewater Oasis";
char far l11n[] = "Pyramid of the Moons";
char far l12n[] = "Pyramid of Shadows";
char far l13n[] = "Pyramid of the\nGnosticene Ancients";
char far l14n[] = "Pyramid of the Forbidden";
char far l15n[] = "Isle of Tar";
char far l16n[] = "Isle of Fire";
char far l17n[] = "Well of Wishes";
char far l18n[] = "Bean-with-Bacon\nMegarocket";

char far l0e[] = "Keen enters the\nShadowlands";
char far l1e[] = "Keen makes a run for\nthe Border Village";
char far l2e[] = "Keen slips into\nSlug Village";
char far l3e[] = "Keen plummets into\nthe The Perilous Pit";	// sic!
char far l4e[] = "Keen plods down into\nthe Cave of the\nDescendents";
char far l5e[] = "Keen shivers along\nthe Chasm of Chills";
char far l6e[] = "Keen reflects upon\nentering Crystalus";
char far l7e[] = "Keen stumbles upon\nHillville";
char far l8e[] = "Keen grits his teeth\nand enters Sand Yego";
char far l9e[] = "Keen disappears into\nMiragia";
char far l10e[] = "Keen crawls into\nLifewater Oasis";
char far l11e[] = "Keen backs into the\nPyramid of the Moons";
char far l12e[] = "Keen move silently in\nthe Pyramid of Shadows";	// sic!
char far l13e[] = "Keen reverently enters\nthe Pyramid of the\nGnosticene Ancients";
char far l14e[] = "Keen hesitantly crosses\ninto the Pyramid of the\nForbidden";
char far l15e[] = "Keen mucks along the\nIsle of Tar";
char far l16e[] = "Keen blazes across the\nIsle of Fire";
char far l17e[] = "Keen hopefully enters\nthe Well of Wishes";
char far l18e[] = "Keen launches into the\nBean-with-Bacon\nMegarocket";

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
	ONEUP_LUMP, AMMO_LUMP
};

//==========================================================================

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
	objtype *ob;
	Uint16 i, x, y, chunk;
	Sint16 info;
	Uint16 far *map;

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
				lumpneeded[WOLRDKEEN_LUMP] = true;
				CA_MarkGrChunk(SCOREBOXSPR);
				break;

			case 4:
				SpawnCouncil(x, y);
				lumpneeded[COUNCIL_LUMP] = true;
				break;

			case 50:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 49:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 5:
				SpawnBerkeloid(x, y);
				lumpneeded[BERKELOID_LUMP] = true;
				break;

			case 6:
				SpawnLindsey(x, y);
				lumpneeded[LINDSEY_LUMP] = true;
				break;

			case 52:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 51:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 7:
				SpawnWormMouth(x, y);
				lumpneeded[WORMOUTH_LUMP] = true;
				break;

			case 46:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 45:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 8:
				SpawnSkypest(x, y);
				lumpneeded[SKYPEST_LUMP] = true;
				break;

			case 9:
				SpawnCloudster(x, y);
				lumpneeded[THUNDERCLOUD_LUMP] = true;
				break;

			case 10:
				SpawnFoot(x, y);
				lumpneeded[INCHWORM_LUMP] = true;	// lump includes the foot sprite
				// Note: The smoke sprites aren't actually required for the foot!
				for (i=SMOKE1SPR; i<=SMOKE4SPR; i++)
				{
					CA_MarkGrChunk(i);
				}
				break;

			case 11:
				SpawnInchworm(x, y);
				lumpneeded[INCHWORM_LUMP] = true;
				for (i=SMOKE1SPR; i<=SMOKE4SPR; i++)
				{
					CA_MarkGrChunk(i);
				}
				break;

			case 12:
				SpawnBounder(x, y);
				lumpneeded[BOUNDER_LUMP] = true;
				break;

			case 13:
				SpawnEggbird(x, y);
				lumpneeded[EGGBIRD_LUMP] = true;
				lumpneeded[EGG_LUMP] = true;
				break;

			case 48:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 47:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 14:
				SpawnLick(x, y);
				lumpneeded[LICK_LUMP] = true;
				break;

			case 88:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 87:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 15:
				SpawnDopefish(x, y);
				lumpneeded[DOPEFISH_LUMP] = true;
				break;

			case 16:
				SpawnSchoolfish(x, y);
				lumpneeded[SCHOOLFISH_LUMP] = true;
				break;

			case 24:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 23:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 17:
				SpawnPixie(x, y);
				lumpneeded[SPRITE_LUMP] = true;
				break;

			case 18:
				SpawnEater(x, y);
				lumpneeded[EATER_LUMP] = true;
				break;

			case 19:
				SpawnMimrock(x, y);
				lumpneeded[MIMROCK_LUMP] = true;
				break;

			case 74:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 73:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 20:
				SpawnArachnut(x, y);
				lumpneeded[ARACHNUT_LUMP] = true;
				break;

			case 21:
				SpawnMadMushroom(x, y);
				lumpneeded[MADMUSHROOM_LUMP] = true;
				break;

			case 44:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 43:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 22:
				SpawnSlug(x, y);
				lumpneeded[SLUG_LUMP] = true;
				break;

			case 25:
				RF_SetScrollBlock(x, y, 1);
				break;

			case 26:
				RF_SetScrollBlock(x, y, 0);
				break;

			case 27:
			case 28:
			case 29:
			case 30:
				SpawnPlatform(x, y, info-27);
				lumpneeded[PLATFORM_LUMP] = true;;
				break;

			case 32:
				SpawnDropPlat(x, y);
				lumpneeded[PLATFORM_LUMP] = true;
				break;

			case 33:
				SpawnMiragia(x, y);
				break;

			case 34:
				if (gamestate.ammo < 5)
				{
					SpawnBonus(x, y, 11);
					lumpneeded[bonuslump[11]] = true;
				}
				break;

			case 35:
				SpawnScuba(x, y);
				CA_MarkGrChunk(SCUBASPR);
				break;

			case 42:
				SpawnSwimKeen(x, y);
				SpawnScore();
				lumpneeded[SCUBAKEEN_LUMP] = true;
				//mark pickup shapes:
				for (i=BONUS100SPR; i<=BONUSCLIPSPR; i++)
				{
					CA_MarkGrChunk(i);
				}
				CA_MarkGrChunk(SCOREBOXSPR);
				break;

			case 83:
			case 84:
			case 85:
			case 86:
				if (gamestate.difficulty < gd_Hard)
					break;
				SpawnDartShooter(x, y, info-83);
				lumpneeded[DARTS_LUMP] = true;
				break;

			case 79:
			case 80:
			case 81:
			case 82:
				if (gamestate.difficulty < gd_Normal)
					break;
				SpawnDartShooter(x, y, info-79);
				lumpneeded[DARTS_LUMP] = true;
				break;

			case 53:
			case 54:
			case 55:
			case 56:
				SpawnDartShooter(x, y, info-53);
				lumpneeded[DARTS_LUMP] = true;
				break;

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

			case 69:
			case 70:
			case 71:
			case 72:
				SpawnMine(x, y, info-69);
				lumpneeded[MINE_LUMP] = true;
				break;

			case 75:
				lumpneeded[MOON_LUMP] = true;
				break;

			case 78:
				if (gamestate.difficulty < gd_Hard)
					break;
			case 77:
				if (gamestate.difficulty < gd_Normal)
					break;
			case 76:
				SpawnEggbirdOut(x, y);
				lumpneeded[EGGBIRD_LUMP] = true;
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

/*
===========================
=
= PrincessLindsey
=
===========================
*/

char *lindseytext[2] =
{
	"There's gear to help\n"
	"you swim in Three-Tooth\n"
	"Lake. It is hidden in\n"
	"Miragia.\n"
	,
	"The way to the Pyramid\n"
	"of the Forbidden lies\n"
	"under the Pyramid of\n"
	"Moons.\n"
};

char *klindseytext[2] =
{
	"Thanks, your Highness!"
	,
	"Thanks for the\n"
	"mysterious clue,\n"
	"Princess!\n"
};

void PrincessLindsey(void)
{
	SD_WaitSoundDone();
	StopMusic();
	CA_UpLevel();
	CA_MarkGrChunk(LINDSEYPIC);
	CA_MarkGrChunk(KEENTALK1PIC);
	CA_MarkGrChunk(KEENTALK2PIC);
	CA_CacheMarks(NULL);
	VW_FixRefreshBuffer();

	US_CenterWindow(26, 8);
	VWB_DrawPic(WindowX, WindowY, LINDSEYPIC);
	PrintY += 6;
	WindowW -= 48;
	WindowX += 48;
	US_CPrint("Princess Lindsey says:\n");
	if (mapon == 7)
	{
		US_CPrint(lindseytext[0]);
	}
	else
	{
		US_CPrint(lindseytext[1]);
	}
	VW_UpdateScreen();
	SD_PlaySound(SND_MAKEFOOT);
	VW_WaitVBL(60);
	IN_ClearKeysDown();
	IN_Ack();

	US_CenterWindow(26, 8);
	VWB_DrawPic(WindowX+WindowW-48, WindowY, KEENTALK1PIC);
	WindowW -= 48;
	PrintY += 12;
	if (mapon == 7)
	{
		US_CPrint(klindseytext[0]);
	}
	else
	{
		US_CPrint(klindseytext[1]);
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
	StartMusic(gamestate.mapon);

	//reset scorebox (sprite may have been re-cached by CA_DownLevel)
	scoreobj->temp2 = -1;
	scoreobj->temp1 = -1;
	scoreobj->temp3 = -1;
	scoreobj->temp4 = -1;
}

//============================================================================

/*
===========================
=
= RescueJanitor
=
===========================
*/

char far jantext1[] =
	"Thanks for going to all\n"
	"that trouble, but I'm\n"
	"just the janitor for the\n"
	"High Council.";

char far jantext2[] =
	"I tried to tell the\n"
	"Shikadi that but they\n"
	"just wouldn't listen...";

char far keenjantext[] =
	"This had better\n"
	"be a joke.";

char far jantext3[] =
	"Sorry.  You aren't\n"
	"mad, are you?";

void RescueJanitor(void)
{
	char str[200];

	SD_WaitSoundDone();
	CA_UpLevel();
	CA_MarkGrChunk(ORACLEPIC);
	CA_MarkGrChunk(KEENTALK1PIC);
	CA_MarkGrChunk(KEENMADPIC);
	CA_CacheMarks(NULL);
	VW_FixRefreshBuffer();
	StartMusic(-1);
	
	US_CenterWindow(26, 8);
	VWB_DrawPic(WindowX, WindowY, ORACLEPIC);
	PrintY += 6;
	WindowW -= 48;
	WindowX += 48;
	_fstrcpy(str, jantext1);
	US_CPrint(str);
	VW_UpdateScreen();
	VW_WaitVBL(60);
	IN_ClearKeysDown();
	IN_Ack();

	US_CenterWindow(26, 8);
	VWB_DrawPic(WindowX, WindowY, ORACLEPIC);
	PrintY += 6;
	WindowW -= 48;
	WindowX += 48;
	_fstrcpy(str, jantext2);
	US_CPrint(str);
	VW_UpdateScreen();
	VW_WaitVBL(60);
	IN_ClearKeysDown();
	IN_Ack();

	US_CenterWindow(26, 8);
	VWB_DrawPic(WindowX+WindowW-48, WindowY, KEENTALK1PIC);
	WindowW -= 48;
	PrintY += 12;
	_fstrcpy(str, keenjantext);
	US_CPrint(str);
	VW_UpdateScreen();
	VW_WaitVBL(60);
	IN_ClearKeysDown();
	IN_Ack();

	US_CenterWindow(26, 8);
	VWB_DrawPic(WindowX, WindowY, ORACLEPIC);
	PrintY += 6;
	WindowW -= 48;
	WindowX += 48;
	_fstrcpy(str, jantext3);
	US_CPrint(str);
	VW_UpdateScreen();
	VW_WaitVBL(60);
	IN_ClearKeysDown();
	IN_Ack();

	US_CenterWindow(26, 8);
	VWB_DrawPic(WindowX+WindowW-48, WindowY, KEENTALK1PIC);
	VWB_DrawPic(WindowX+WindowW-40, WindowY+24, KEENMADPIC);
	VW_UpdateScreen();
	VW_WaitVBL(30);
	IN_ClearKeysDown();
	IN_Ack();

	StopMusic();
	CA_DownLevel();
	StartMusic(gamestate.mapon);

	//BUG: scorebox needs to be reset here (sprite may have been re-cached by CA_DownLevel)
}

//============================================================================

/*
===========================
=
= CanitSwim
=
===========================
*/

void CantSwim(void)
{
	SD_WaitSoundDone();
	CA_UpLevel();	// kinda useless without CA_CacheMarks or CA_SetGrPurge
	// BUG: haven't made anything purgable here, caching the pic may cause an "out of memory" crash
	CA_CacheGrChunk(KEENTALK1PIC);

	US_CenterWindow(26, 8);
	VWB_DrawPic(WindowX+WindowW-48, WindowY, KEENTALK1PIC);
	WindowW -= 48;
	PrintY += 12;
	US_CPrint("I can't swim!");
	VW_UpdateScreen();
	VW_WaitVBL(30);
	IN_ClearKeysDown();
	IN_Ack();
	CA_DownLevel();

	//Note: scorebox sprite has not been re-cached here (didn't use CA_CacheMarks or anything else that would have made the sprite purgable)
}

//============================================================================

/*
===========================
=
= GotScuba
=
===========================
*/

void GotScuba(void)
{
	SD_WaitSoundDone();
	CA_UpLevel();
	CA_MarkGrChunk(KEENTALK1PIC);
	CA_MarkGrChunk(KEENTALK2PIC);
	CA_CacheMarks(NULL);

	US_CenterWindow(26, 8);
	VWB_DrawPic(WindowX+WindowW-48, WindowY, KEENTALK1PIC);
	WindowW -= 48;
	PrintY += 12;
	US_CPrint(
		"Cool!  I can breathe\n"
		"under water now!"
		);
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

	//Note: scorebox sprite may have been re-cached by CA_DownLevel, but the level ends after this anyway
}

//============================================================================

/*
===========================
=
= RescuedMember
=
===========================
*/

char *keentext[] = {
	"No sweat, oh guardian\n"
	"of wisdom!"
	,
	"Sounds like a plan,\n"
	"bearded one!"
	,
	"No problemo."
	,
	"Great.  You know, you\n"
	"look a lot like the\n"
	"last guy I rescued..."
	,
	"Good idea, Gramps."
	,
	"May the road rise\n"
	"to meet your feet,\n"
	"Mr. Member."
	,
	"Wise plan of action,\n"
	"your ancientness."
	,
	"You're the last one,\n"
	"fella.  Let's both\n"
	"get back to the\n"
	"Oracle chamber!"
};

void RescuedMember(void)
{
	SD_WaitSoundDone();
	CA_UpLevel();
	CA_MarkGrChunk(ORACLEPIC);
	CA_MarkGrChunk(KEENTALK1PIC);
	CA_MarkGrChunk(KEENTALK2PIC);
	CA_CacheMarks(NULL);
	StartMusic(-1);
	VW_FixRefreshBuffer();

	US_CenterWindow(26, 8);
	VWB_DrawPic(WindowX, WindowY, ORACLEPIC);
	PrintY += 6;
	WindowW -= 48;
	WindowX += 48;
	if (mapon == 17)
	{
		US_CPrint(
			"Ggoh thig you sogh mg\n"
			"fgor regscuing mgge!\n"
			"I'gll regur tgo the\n"
			"Goracle chagber\n"
			"igmediatggely. Blub."
			);
	}
	else
	{
		US_CPrint(
			"Oh thank you so much\n"
			"for rescuing me!\n"
			"I'll return to the\n"
			"Oracle chamber\n"
			"immediately."
			);
	}
	VW_UpdateScreen();
	VW_WaitVBL(60);
	IN_ClearKeysDown();
	IN_Ack();

	US_CenterWindow(26, 8);
	VWB_DrawPic(WindowX+WindowW-48, WindowY, KEENTALK1PIC);
	WindowW -= 48;
	PrintY += 12;
	US_CPrint(keentext[gamestate.rescued]);
	VW_UpdateScreen();
	VW_WaitVBL(30);
	IN_ClearKeysDown();
	IN_Ack();

	VWB_DrawPic(WindowX+WindowW, WindowY, KEENTALK2PIC);
	VW_UpdateScreen();
	VW_WaitVBL(30);
	IN_ClearKeysDown();
	IN_Ack();

	gamestate.rescued++;
	CA_DownLevel();
	StopMusic();

	//Note: scorebox sprite may have been re-cached by CA_DownLevel, but the level ends after this anyway
}

/*
=============================================================================

						  SWIMMING KEEN

temp4 = counter for spawning bubbles

=============================================================================
*/

statetype s_keenswimslow1 = {SCUBAKEENL1SPR, SCUBAKEENR1SPR, stepthink, false, false, 50, 0, 0, T_KeenSwimSlow, C_KeenSwim, R_KeenSwim, &s_keenswimslow2};
statetype s_keenswimslow2 = {SCUBAKEENL2SPR, SCUBAKEENR2SPR, stepthink, false, false, 50, 0, 0, T_KeenSwimSlow, C_KeenSwim, R_KeenSwim, &s_keenswimslow1};
statetype s_keenswim1     = {SCUBAKEENL1SPR, SCUBAKEENR1SPR, stepthink, false, false, 50, 0, 0, T_KeenSwim, C_KeenSwim, R_KeenSwim, &s_keenswimslow2};
statetype s_keenswim2     = {SCUBAKEENL2SPR, SCUBAKEENR2SPR, stepthink, false, false, 50, 0, 0, T_KeenSwim, C_KeenSwim, R_KeenSwim, &s_keenswimslow1};
//Note: the die states for swimming Keen are in CK_KEEN.C and K4_ACT3.C (dopefish section)

statetype s_kbubble1  = {SMALLBUBBLE1SPR, SMALLBUBBLE1SPR, think, false, false, 20, 0, 24, T_Bubble, NULL, R_Draw, &s_kbubble1};
statetype s_kbubble2  = {SMALLBUBBLE2SPR, SMALLBUBBLE2SPR, think, false, false, 20, 0, 24, T_Bubble, NULL, R_Draw, &s_kbubble2};
statetype s_kbubble3  = {SMALLBUBBLE3SPR, SMALLBUBBLE3SPR, think, false, false, 20, 0, 24, T_Bubble, NULL, R_Draw, &s_kbubble3};
statetype s_kbubble4  = {SMALLBUBBLE4SPR, SMALLBUBBLE4SPR, think, false, false, 20, 0, 24, T_Bubble, NULL, R_Draw, &s_kbubble4};

/*
===========================
=
= SpawnSwimKeen
=
===========================
*/

void SpawnSwimKeen(Sint16 x, Sint16 y)
{
	player->obclass = keenobj;
	player->active = ac_allways;
	player->priority = 1;
	player->x = CONVERT_TILE_TO_GLOBAL(x);
	player->y = CONVERT_TILE_TO_GLOBAL(y);
	player->xdir = 1;
	player->ydir = 1;
	player->needtoclip = cl_fullclip;
	NewState(player, &s_keenswimslow1);
}

/*
===========================
=
= SpawnKbubble
=
===========================
*/

void SpawnKbubble(objtype *ob)
{
	ob->temp4 = 0;
	GetNewObj(true);
	if (ob->xdir == -1)
	{
		new->x = ob->x;
	}
	else
	{
		new->x = ob->x + 24*PIXGLOBAL;
	}
	new->y = ob->y;
	new->obclass = inertobj;
	new->priority = 3;
	new->active = ac_removable;
	new->needtoclip = cl_noclip;
	new->yspeed = -24;
	new->xspeed = 4;
	switch (US_RndT() / 64)
	{
	case 0:
		NewState(new, &s_kbubble1);
		break;
	case 1:
		NewState(new, &s_kbubble2);
		break;
	case 2:
		NewState(new, &s_kbubble3);
		break;
	case 3:
		NewState(new, &s_kbubble4);
		break;
	}
	SD_PlaySound(SND_BLUB);
}

/*
===========================
=
= T_KeenSwimSlow
=
===========================
*/

void T_KeenSwimSlow(objtype *ob)
{
	Sint32 i;
	Sint16 vx, vy, xc, yc;

	xc = ob->xspeed < 0;
	yc = ob->yspeed < 4;

	ob->temp4 = ob->temp4 + tics;
	if (ob->temp4 > 60)
		SpawnKbubble(ob);

	if (jumpbutton && !jumpheld)
	{
		jumpheld = true;
		if (c.xaxis)
			ob->xspeed = c.xaxis * 18;
		if (c.yaxis)
			ob->yspeed = c.yaxis * 18;
		ob->state = ob->state->nextstate;
	}
	if (c.xaxis)
		ob->xdir = c.xaxis;

	for (i = lasttimecount-tics; i < lasttimecount; i++)
	{
		if ((i & 7) == 0)
		{
			if (ob->xspeed > 12)
			{
				vx = -3;
			}
			else if (ob->xspeed > 0)
			{
				vx = -1;
			}
			else if (ob->xspeed > -12)
			{
				vx = 1;
			}
			else
			{
				vx = 3;
			}
			vx += c.xaxis;
			vx += c.xaxis;
			ob->xspeed += vx;

			if (c.xaxis == 0 && (ob->xspeed < 0) != xc)
				ob->xspeed = 0;

			if (ob->yspeed > 12)
			{
				vy = -3;
			}
			else if (ob->yspeed > 4)
			{
				vy = -1;
			}
			else if (ob->yspeed > -12)
			{
				vy = 1;
			}
			else
			{
				vy = 3;
			}
			vy += c.yaxis;
			vy += c.yaxis;
			ob->yspeed += vy;

			if (c.yaxis == 0 && ob->yspeed > 4 && yc)
				ob->yspeed = 0;
		}
		xtry += ob->xspeed;
		ytry += ob->yspeed;
	}
}

/*
===========================
=
= T_KeenSwim
=
===========================
*/

void T_KeenSwim(objtype *ob)	//never actually used
{
	ob->temp4 = ob->temp4 + tics;
	if (ob->temp4 > 60)
		SpawnKbubble(ob);

	if (jumpbutton && !jumpheld)
	{
		jumpheld = true;
		ob->xspeed = c.xaxis * 18;
		if (c.yaxis)
			ob->yspeed = c.yaxis * 18;

		if (ob->state == &s_keenswim1)
		{
			ob->state = &s_keenswim2;
		}
		else
		{
			ob->state = &s_keenswim1;
		}
	}

	xtry = xtry + ob->xspeed * tics;
	ytry = ytry + ob->yspeed * tics;
	if (xtry > 0)
	{
		ob->xdir = 1;
	}
	else if (xtry < 0)
	{
		ob->xdir = -1;
	}

	ytry = ytry + tics*4;
}

/*
===========================
=
= C_KeenSwim
=
===========================
*/

void C_KeenSwim(objtype *ob, objtype *hit)
{
	switch (hit->obclass)
	{
	case bonusobj:
		switch (hit->temp1)
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
			SD_PlaySound(bonussound[hit->temp1]);
			hit->obclass = inertobj;
			hit->priority = 3;
			hit->shapenum = bonussprite[hit->temp1];
			GivePoints(bonuspoints[hit->temp1]);
			if (hit->temp1 < 4)
			{
				gamestate.keys[hit->temp1]++;
			}
			else if (hit->temp1 == 10)
			{
				gamestate.lives++;
			}
			else if (hit->temp1 == 11)
			{
				gamestate.ammo += shotsinclip[gamestate.difficulty];
			}
			ChangeState(hit, &s_bonusrise);
			break;
		}
		break;

	case oracleobj:
		playstate = ex_rescued;
		break;
	}
	ob++;			// shut up compiler
}

/*
===========================
=
= R_KeenSwim
=
===========================
*/

void R_KeenSwim(objtype *ob)
{
	if (ob->hiteast && ob->xspeed < 0 || ob->hitwest && ob->xspeed > 0)
		ob->xspeed = 0;

	if (ob->hitnorth && ob->yspeed > 0 || ob->hitsouth && ob->yspeed < 0)
		ob->yspeed = 0;

	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}
