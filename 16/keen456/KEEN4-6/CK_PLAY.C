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

#include "CK_DEF.H"

/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

ScanCode firescan = sc_Space;

boolean singlestep, jumpcheat, godmode, keenkilled;

exittype playstate;
gametype gamestate;

objtype *new, *check, *player, *scoreobj;

Uint16 originxtilemax;
Uint16 originytilemax;

ControlInfo c;
boolean button2, button3;	// never used

objtype dummyobj;

Sint16 invincible;

boolean oldshooting, showscorebox, joypad;

Sint16 groundslam;

boolean debugok;
boolean jumpbutton, jumpheld, pogobutton, pogoheld, firebutton, fireheld, upheld;


/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

objtype *obj;

Uint16 centerlevel;

Uint16 objectcount;
objtype objarray[MAXACTORS];
objtype *lastobj;
objtype *objfreelist;

Sint16 inactivateleft;
Sint16 inactivateright;
Sint16 inactivatetop;
Sint16 inactivatebottom;

#ifdef KEEN6Ev15
Uint16 __dummy__;	// never used, but must be present to recreate the original EXE
#endif

Uint16 extravbls;

Uint16 windowofs;
Sint16 vislines;
boolean scrollup;

Sint16 oldfirecount;

//===========================================================================

/*
==================
=
= CountObjects
=
==================
*/

void CountObjects(void)
{
	Uint16 activeobjects, inactiveobjects;
	objtype *ob;

	activeobjects = inactiveobjects = 0;
	for (ob=player; ob; ob=ob->next)
	{
		if (ob->active)
		{
			activeobjects++;
		}
		else
		{
			inactiveobjects++;
		}
	}
	VW_FixRefreshBuffer();
	US_CenterWindow(18, 4);
	PrintY += 7;
	US_Print("Active Objects :");
	US_PrintUnsigned(activeobjects);
	US_Print("\nInactive Objects:");
	US_PrintUnsigned(inactiveobjects);
	VW_UpdateScreen();
	IN_Ack();
}

/*
==================
=
= DebugMemory
=
==================
*/

void DebugMemory(void)
{
	VW_FixRefreshBuffer();
	US_CenterWindow(16, 7);
	US_CPrint("Memory Usage");
	US_CPrint("------------");
	US_Print("Total     :");
	US_PrintUnsigned((mminfo.mainmem+mminfo.EMSmem+mminfo.XMSmem)/1024);
	US_Print("k\nFree      :");
	US_PrintUnsigned(MM_UnusedMemory()/1024);
	US_Print("k\nWith purge:");
	US_PrintUnsigned(MM_TotalFree()/1024);
	US_Print("k\n");
	VW_UpdateScreen();
	IN_Ack();
#if GRMODE != CGAGR
	MM_ShowMemory();
#endif
}

/*
===================
=
= TestSprites
=
===================
*/

void TestSprites(void)
{
	Uint16 infox, infoy;
	Sint16 chunk, oldchunk;
	Sint16 shift;
	Uint16 infobottom, drawx;
	spritetabletype far *info;
	Uint8 _seg *block;
	Uint16 size;
	Uint16 scan;
	Uint32 totalsize;

	VW_FixRefreshBuffer();
	US_CenterWindow(30, 17);
	totalsize = 0;
	US_CPrint("Sprite Test");
	US_CPrint("-----------");
	infoy = PrintY;
	infox = (PrintX + 56) & ~7;
	drawx = infox + 40;
	US_Print("Chunk:\nWidth:\nHeight:\nOrgx:\nOrgy:\nXl:\nYl:\nXh:\nYh:\nShifts:\nMem:\n");
	infobottom = PrintY;
	chunk = STARTSPRITES;
	shift = 0;
	while (1)
	{
		if (chunk >= STARTSPRITES+NUMSPRITES)
		{
			chunk = STARTSPRITES+NUMSPRITES-1;
		}
		else if (chunk < STARTSPRITES)
		{
			chunk = STARTSPRITES;
		}
		info = &spritetable[chunk-STARTSPRITES];
		block = grsegs[chunk];
		VWB_Bar(infox, infoy, 40, infobottom-infoy, WHITE);
		PrintX = infox;
		PrintY = infoy;
		US_PrintUnsigned(chunk);
		US_Print("\n");
		PrintX = infox;
		US_PrintUnsigned(info->width);
		US_Print("\n");
		PrintX = infox;
		US_PrintUnsigned(info->height);
		US_Print("\n");
		PrintX = infox;
		US_PrintSigned(info->orgx);
		US_Print("\n");
		PrintX = infox;
		US_PrintSigned(info->orgy);
		US_Print("\n");
		PrintX = infox;
		US_PrintSigned(info->xl);
		US_Print("\n");
		PrintX = infox;
		US_PrintSigned(info->yl);
		US_Print("\n");
		PrintX = infox;
		US_PrintSigned(info->xh);
		US_Print("\n");
		PrintX = infox;
		US_PrintSigned(info->yh);
		US_Print("\n");
		PrintX = infox;
		US_PrintSigned(info->shifts);
		US_Print("\n");
		PrintX = infox;
		if (!block)
		{
			US_Print("-----");
		}
		else
		{
			size = ((spritetype far *)block)->sourceoffset[3] + ((spritetype far *)block)->planesize[3]*5;
			size = (size + 15) & ~15;	//round up to multiples of 16
			totalsize += size;	//useless: the value stored in 'totalsize' is never used
			US_PrintUnsigned(size);
			US_Print("=");
		}
		oldchunk = chunk;
		do
		{
			VWB_Bar(drawx, infoy, 110, infobottom-infoy, WHITE);
			if (block)
			{
				PrintX = drawx;
				PrintY = infoy;
				US_Print("Shift:");
				US_PrintUnsigned(shift);
				US_Print("\n");
				VWB_DrawSprite(drawx + 2*shift + 16, PrintY, chunk);
			}
			VW_UpdateScreen();
			scan = IN_WaitForKey();
			switch (scan)
			{
			case sc_UpArrow:
				chunk++;
				break;
			case sc_DownArrow:
				chunk--;
				break;
			case sc_PgUp:
				chunk += 10;
				if (chunk >= STARTSPRITES+NUMSPRITES)
				{
					chunk = STARTSPRITES+NUMSPRITES-1;
				}
				break;
			case sc_PgDn:
				chunk -= 10;
				if (chunk < STARTSPRITES)
				{
					chunk = STARTSPRITES;
				}
				break;
			case sc_LeftArrow:
				if (--shift == -1)
				{
					shift = 3;
				}
				break;
			case sc_RightArrow:
				if (++shift == 4)
				{
					shift = 0;
				}
				break;
			case sc_Escape:
				return;
			}

		} while (chunk == oldchunk);

	}
}

/*
===================
=
= PicturePause
=
===================
*/

void PicturePause(void)
{
	Uint16 source;
	Sint16 y;

//
// wait for a key press, abort if it's not Enter
//
	IN_ClearKeysDown();
	while (!LastScan);
	if (LastScan != sc_Enter)
	{
		IN_ClearKeysDown();
		return;
	}

	SD_PlaySound(SND_JUMP);
	SD_WaitSoundDone();

//
// rearrange onscreen image into base EGA layout, so that it
// can be grabbed correctly by an external screenshot tool
//
	source = displayofs + panadjust;

	VW_ColorBorder(15);	// white (can't use WHITE as parameter, since that's defined as 3 for CGA and this must use 15)
	VW_SetLineWidth(40);
	VW_SetScreen(0, 0);

	if (source < 0x10000l-200*64)
	{
	//
	// copy top line first
	//
		for (y=0; y<200; y++)
		{
			VW_ScreenToScreen(source+y*64, y*40, 40, 1);
		}
	}
	else
	{
	//
	// copy bottom line first
	//
		for (y=199; y>=0; y--)
		{
			VW_ScreenToScreen(source+y*64, y*40, 40, 1);
		}
	}

//
// shut down input manager so that screenshot tool can see input again
//
	IN_Shutdown();

	SD_PlaySound(SND_EXTRAKEEN);
	SD_WaitSoundDone();

//
// shut down the remaining ID managers, except VW (stay in graphics mode!)
//
	US_Shutdown();
	SD_Shutdown();
	IN_Shutdown();
	RF_Shutdown();
	CA_Shutdown();
	MM_Shutdown();

//
// wait until user hits Escape
//
	while (((bioskey(0) >> 8) & 0xFF) != sc_Escape);

//
// back to text mode and exit to DOS
//
	VW_Shutdown();
	exit(0);
}

/*
===================
=
= MaskOnTile
=
===================
*/

void MaskOnTile(Uint16 dest, Uint16 source)
{
	Sint16 i;
	Uint16 _seg *sourceseg;
	Uint16 _seg *destseg;
	Uint16 sourceval, maskindex, sourcemask;

	sourceseg = (grsegs+STARTTILE16M)[source];
	destseg = (grsegs+STARTTILE16M)[dest];
	for (i=0; i<64; i++)
	{
		maskindex = i & 15;
#ifdef KEEN6Ev15
		sourceval = sourceseg[16+i];
#else
		sourceval = (sourceseg+16)[i];
#endif
		sourcemask = sourceseg[maskindex];
		destseg[maskindex] &= sourcemask;
		destseg[16+i] &= sourcemask;
		destseg[16+i] |= sourceval;
	}
}

/*
===================
=
= WallDebug
=
===================
*/

void WallDebug(void)
{
	Sint16 i, val;

	VW_FixRefreshBuffer();
	US_CenterWindow(24, 3);
	US_PrintCentered("WORKING");
	VW_UpdateScreen();
	for (i=STARTTILE16M+108; i<STARTTILE16M+124; i++)
	{
		CA_CacheGrChunk(i);
	}
	for (i=0; i<NUMTILE16M; i++)
	{
		if (!grsegs[STARTTILE16M+i])
		{
			continue;
		}
		val = tinf[i+NORTHWALL] & 7;
		if (val)
		{
			MaskOnTile(i, val+107);
		}
		val = tinf[i+SOUTHWALL] & 7;
		if (val)
		{
			MaskOnTile(i, val+115);
		}
		val = tinf[i+EASTWALL] & 7;
		if (val > 1)
		{
			strcpy(str, "WallDebug: East wall other than 1:");
			itoa(i, str2, 10);
			strcat(str, str2);
			Quit(str);
		}
		if (val)
		{
			MaskOnTile(i, val+114);	//Note: val is always 1 here, so you could use 115 as 2nd arg
		}
		val = tinf[i+WESTWALL] & 7;
		if (val > 1)
		{
			strcpy(str, "WallDebug: West wall other than 1:");
			itoa(i, str2, 10);
			strcat(str, str2);
			Quit(str);
		}
		if (val)
		{
			MaskOnTile(i, val+122);	//Note: val is always 1 here, so you could use 123 as 2nd arg
		}
	}
}


//===========================================================================

/*
================
=
= DebugKeys
=
================
*/

boolean DebugKeys(void)
{
	Sint16 level, i, esc;

	if (Keyboard[sc_B] && ingame)		// B = border color
	{
		VW_FixRefreshBuffer();
		US_CenterWindow(24, 3);
		PrintY += 6;
		US_Print(" Border color (0-15):");
		VW_UpdateScreen();
		esc = !US_LineInput(px, py, str, NULL, true, 2, 0);
		if (!esc)
		{
			level = atoi(str);
			if (level >= 0 && level <= 15)
			{
				VW_ColorBorder(level);
			}
		}
		return true;
	}

	if (Keyboard[sc_C] && ingame)		// C = count objects
	{
		CountObjects();
		return true;
	}

	if (Keyboard[sc_D] && ingame)		// D = start / end demo record
	{
		if (DemoMode == demo_Off)
		{
			StartDemoRecord();
		}
		else if (DemoMode == demo_Record)
		{
			EndDemoRecord();
			playstate = ex_completed;
		}
		return true;
	}

	if (Keyboard[sc_E] && ingame)		// E = quit level
	{
		if (tedlevel)
		{
			TEDDeath();
		}
		playstate = ex_completed;
		//BUG? there is no return in this branch (should return false)
	}

	if (Keyboard[sc_G] && ingame)		// G = god mode
	{
		VW_FixRefreshBuffer();
		US_CenterWindow(12, 2);
		if (godmode)
		{
			US_PrintCentered("God mode OFF");
		}
		else
		{
			US_PrintCentered("God mode ON");
		}
		VW_UpdateScreen();
		IN_Ack();
		godmode ^= true;
		return true;
	}
	else if (Keyboard[sc_I])			// I = item cheat
	{
		VW_FixRefreshBuffer();
		US_CenterWindow(12, 3);
		US_PrintCentered("Free items!");
		for (i=0; i<4; i++)
		{
			gamestate.keys[i] = 99;
		}
		gamestate.ammo = 99;
#if defined KEEN4
		gamestate.wetsuit = true;
#elif defined KEEN5
		gamestate.keycard = true;
#elif defined KEEN6
		gamestate.passcardstate=gamestate.hookstate=gamestate.sandwichstate = 1;
#endif
		VW_UpdateScreen();
		IN_Ack();
		GivePoints(3000);
		return true;
	}
	else if (Keyboard[sc_J])			// J = jump cheat
	{
		jumpcheat ^= true;
		VW_FixRefreshBuffer();
		US_CenterWindow(18, 3);
		if (jumpcheat)
		{
			US_PrintCentered("Jump cheat ON");
		}
		else
		{
			US_PrintCentered("Jump cheat OFF");
		}
		VW_UpdateScreen();
		IN_Ack();
		return true;
	}
	else if (Keyboard[sc_M])			// M = memory info
	{
		DebugMemory();
		return true;
	}
	else if (Keyboard[sc_N])			// N = no clip
	{
		VW_FixRefreshBuffer();
		US_CenterWindow(18, 3);
		if (player->needtoclip)
		{
			US_PrintCentered("No clipping ON");
			player->needtoclip = cl_noclip;
		}
		else
		{
			US_PrintCentered("No clipping OFF");
			player->needtoclip = cl_midclip;
		}
		VW_UpdateScreen();
		IN_Ack();
		return true;
	}
	else if (Keyboard[sc_P])			// P = pause with no screen disruptioon
	{
		IN_ClearKeysDown();
		PicturePause();
		return true;
	}
	else if (Keyboard[sc_S] && ingame)	// S = slow motion
	{
		singlestep ^= true;
		VW_FixRefreshBuffer();
		US_CenterWindow(18, 3);
		if (singlestep)
		{
			US_PrintCentered("Slow motion ON");
		}
		else
		{
			US_PrintCentered("Slow motion OFF");
		}
		VW_UpdateScreen();
		IN_Ack();
		return true;
	}
	else if (Keyboard[sc_T])			// T = sprite test
	{
		TestSprites();
		return true;
	}
	else if (Keyboard[sc_V])			// V = extra VBLs
	{
		VW_FixRefreshBuffer();
		US_CenterWindow(30, 3);
		PrintY += 6;
		US_Print("  Add how many extra VBLs(0-8):");
		VW_UpdateScreen();
		esc = !US_LineInput(px, py, str, NULL, true, 2, 0);
		if (!esc)
		{
			level = atoi(str);
			if (level >= 0 && level <= 8)
			{
				extravbls = level;
			}
		}
		return true;
	}
	else if (Keyboard[sc_W] && ingame)	// W = warp to level
	{
		VW_FixRefreshBuffer();
		US_CenterWindow(26, 3);
		PrintY += 6;
		US_Print("  Warp to which level(1-18):");
		VW_UpdateScreen();
		esc = !US_LineInput(px, py, str, NULL, true, 2, 0);
		if (!esc)
		{
			level = atoi(str);
			if (level > 0 && level <= 18)
			{
				gamestate.mapon = level;
				playstate = ex_warped;
			}
		}
		return true;
	}
	else if (Keyboard[sc_Y])			// Y = wall debug
	{
		WallDebug();
		return true;
	}
	else if (Keyboard[sc_Z])			// Z = game over
	{
		gamestate.lives = 0;
		KillKeen();
		return false;
	}
	return false;
}

//===========================================================================

/*
================
=
= UserCheat
=
================
*/

void UserCheat(void)
{
	Sint16 i;

	for (i=sc_A; i<=sc_Z; i++)	//Note: this does NOT check the keys in alphabetical order!
	{
		if (i != sc_B && i != sc_A && i != sc_T && Keyboard[i])
		{
			return;
		}
	}
	US_CenterWindow(20, 7);
	PrintY += 2;
	US_CPrint(
		"Cheat Option!\n"
		"\n"
		"You just got all\n"
		"the keys, 99 shots,\n"
		"and an extra keen!");
	VW_UpdateScreen();
	IN_Ack();
	RF_ForceRefresh();
	gamestate.ammo = 99;
	gamestate.lives++;
#ifdef KEEN5
	gamestate.keycard = true;
#endif
	gamestate.keys[0] = gamestate.keys[1] = gamestate.keys[2] = gamestate.keys[3] = 1;
}

//===========================================================================

/*
=====================
=
= CheckKeys
=
=====================
*/

void CheckKeys(void)
{
	if (screenfaded)			// don't do anything with a faded screen
	{
		return;
	}

//
// Enter for status screen
//
	if (Keyboard[sc_Enter] || (GravisGamepad && GravisAction[ga_Status]))
	{
		StatusWindow();
		IN_ClearKeysDown();
		RF_ForceRefresh();
		lasttimecount = TimeCount;	// BUG: should be the other way around
	}

//
// pause key wierdness can't be checked as a scan code
//
	if (Paused)
	{
		SD_MusicOff();
		VW_FixRefreshBuffer();
		US_CenterWindow(8, 3);
		US_PrintCentered("PAUSED");
		VW_UpdateScreen();
		IN_Ack();
		RF_ForceRefresh();
		Paused = false;
		SD_MusicOn();
	}

#ifndef KEEN6
//
// F1 to enter help screens
//
	if (LastScan == sc_F1)
	{
		StopMusic();
		HelpScreens();
		StartMusic(gamestate.mapon);
		if (showscorebox)
		{
			scoreobj->temp2 = -1;
			scoreobj->temp1 = -1;
			scoreobj->temp3 = -1;
			scoreobj->temp4 = -1;
		}
		RF_ForceRefresh();
	}
#endif

	if (!storedemo)
	{
//
// F2-F7/ESC to enter control panel
//
		if (LastScan >= sc_F2 && LastScan <= sc_F7 || LastScan == sc_Escape)
		{
			VW_FixRefreshBuffer();
			StopMusic();
			US_ControlPanel();
			RF_FixOfs();
			StartMusic(gamestate.mapon);
			if (!showscorebox && scoreobj->sprite)
			{
				RF_RemoveSprite(&scoreobj->sprite);
			}
			if (showscorebox)
			{
				scoreobj->temp2 = -1;
				scoreobj->temp1 = -1;
				scoreobj->temp3 = -1;
				scoreobj->temp4 = -1;
			}
			IN_ClearKeysDown();
			if (restartgame)
			{
				playstate = ex_resetgame;
			}
			else if (!loadedgame)
			{
				RF_ForceRefresh();
			}
			if (abortgame)
			{
				abortgame = false;
				playstate = ex_abortgame;
			}
			if (loadedgame)
			{
				playstate = ex_loadedgame;
			}
			lasttimecount = TimeCount;	// BUG: should be the other way around
		}

//
// F9 boss key
//
		if (LastScan == sc_F9)
		{
			VW_Shutdown();
			SD_MusicOff();
			cputs("C:>");
			IN_ClearKeysDown();
			while (LastScan != sc_Escape);
			VW_SetScreenMode(GRMODE);
			VW_ColorBorder(bordercolor);
			RF_ForceRefresh();
			IN_ClearKeysDown();
			lasttimecount = TimeCount;	// BUG: should be the other way around
			SD_MusicOn();
		}
	}

//
// B-A-T cheat code
//
	if (Keyboard[sc_B] && Keyboard[sc_A] && Keyboard[sc_T])
	{
		UserCheat();
	}

//
// F10-? debug keys
//
	if (debugok && Keyboard[sc_F10])
	{
		if (DebugKeys())
		{
			RF_ForceRefresh();
			lasttimecount = TimeCount;	// BUG: should be the other way around
		}
	}

//
// Ctrl-S toggles sound (only in storedemo mode)
//
	if (storedemo && Keyboard[sc_Control] && LastScan == sc_S)
	{
		if (SoundMode != sdm_Off)
		{
			SD_SetSoundMode(sdm_Off);
			SD_SetMusicMode(smm_Off);
		}
		else
		{
			if (AdLibPresent)
			{
				SD_SetSoundMode(sdm_AdLib);
				QuietFX = false;
				SD_SetMusicMode(smm_AdLib);
			}
			else
			{
				SD_SetSoundMode(sdm_PC);
				SD_SetMusicMode(smm_Off);
			}
			CA_LoadAllSounds();
		}
	}

//
// Ctrl-Q quick quit
//
	if (Keyboard[sc_Control] && LastScan == sc_Q)
	{
		IN_ClearKeysDown();
		Quit(NULL);
	}
}

//===========================================================================

/*
==================
=
= PrintNumbers
=
==================
*/

void PrintNumbers(Sint16 x, Sint16 y, Sint16 maxlen, Sint16 basetile, Sint32 number)
{
	register Sint16 i;
	Sint16 len;
	char buffer[20];

	ltoa(number, buffer, 10);
	len = strlen(buffer);
	i = maxlen;
	while (i>len)
	{
		VWB_DrawTile8(x, y, basetile);
		i--;
		x += 8;
	}
	while (i>0)
	{
		VWB_DrawTile8(x, y, basetile+buffer[len-i]+(1-'0'));
		i--;
		x += 8;
	}
}

/*
==================
=
= DrawStatusWindow
=
==================
*/

#if GRMODE == CGAGR

#define BACKCOLOR WHITE
#define TEXTBACK BLACK
#define NUMBERBACK BLACK

#else

#define BACKCOLOR LIGHTGRAY
#define TEXTBACK WHITE
#define NUMBERBACK BLACK

#endif

void DrawStatusWindow(void)
{
	Sint16 off, x, y, w, h, i;
	Uint16 width, height;

	x = 64;
	y = 16;
	w = 184;
	h = 144;
	VWB_DrawTile8(x, y, 54);
	VWB_DrawTile8(x, y+h, 60);
	for (i=x+8; i<=x+w-8; i+=8)
	{
		VWB_DrawTile8(i, y, 55);
		VWB_DrawTile8(i, y+h, 61);
	}
	VWB_DrawTile8(i, y, 56);
	VWB_DrawTile8(i, y+h, 62);
	for (i=y+8; i<=y+h-8; i+=8)
	{
		VWB_DrawTile8(x, i, 57);
		VWB_DrawTile8(x+w, i, 59);
	}
	VWB_Bar(72, 24, 176, 136, BACKCOLOR);

	PrintY = 28;
	WindowX = 80;
	WindowW = 160;
	US_CPrint("LOCATION");
	VWB_Bar(79, 38, 162, 20, TEXTBACK);
#ifdef KEEN5
	if (mapon == 0 && player->y > 100*TILEGLOBAL)
		_fstrcpy(str, levelnames[13]);
	else
		_fstrcpy(str, levelnames[gamestate.mapon]);
#else
	_fstrcpy(str, levelnames[gamestate.mapon]);
#endif
	SizeText(str, &width, &height);
	PrintY = (20-height)/2+40-2;
	US_CPrint(str);

	PrintY = 61;
	WindowX = 80;
	WindowW = 64;
	US_CPrint("SCORE");
	VWB_Bar(79, 71, 66, 10, NUMBERBACK);
	PrintNumbers(80, 72, 8, 41, gamestate.score);

	PrintY = 61;
	WindowX = 176;
	WindowW = 64;
	US_CPrint("EXTRA");
	VWB_Bar(175, 71, 66, 10, NUMBERBACK);
	PrintNumbers(176, 72, 8, 41, gamestate.nextextra);

#if defined KEEN4
	PrintY = 85;
	WindowX = 80;
	WindowW = 64;
	US_CPrint("RESCUED");
	VWB_Bar(79, 95, 66, 10, NUMBERBACK);
	for (i = 0; i < gamestate.rescued; i++, off+=8)
	{
		VWB_DrawTile8(i*8 + 80, 96, 40);
	}
#elif defined KEEN5
	PrintY = 92;
	PrintX = 80;
	US_Print("KEYCARD");
	VWB_Bar(135, 91, 10, 10, NUMBERBACK);
	if (gamestate.keycard)
	{
		VWB_DrawTile8(136, 92, 40);
	}
#endif

	PrintY = 85;
	WindowX = 176;
	WindowW = 64;
	US_CPrint("LEVEL");
	VWB_Bar(175, 95, 66, 10, TEXTBACK);
	PrintY = 96;
	WindowX = 176;
	WindowW = 64;
	switch (gamestate.difficulty)
	{
	case gd_Easy:
		US_CPrint("Easy");
		break;
	case gd_Normal:
		US_CPrint("Normal");
		break;
	case gd_Hard:
		US_CPrint("Hard");
		break;
	}

#ifdef KEEN6
	PrintX = 80;
	PrintY = 96;
	US_Print("ITEMS");
	VWB_Bar(127, 95, 26, 10, NUMBERBACK);
	if (gamestate.sandwichstate == 1)
	{
		VWB_DrawTile8(128, 96, 2);
	}
	else
	{
		VWB_DrawTile8(128, 96, 1);
	}
	if (gamestate.hookstate == 1)
	{
		VWB_DrawTile8(136, 96, 4);
	}
	else
	{
		VWB_DrawTile8(136, 96, 3);
	}
	if (gamestate.passcardstate == 1)
	{
		VWB_DrawTile8(144, 96, 6);
	}
	else
	{
		VWB_DrawTile8(144, 96, 5);
	}
#endif

	PrintX = 80;
	PrintY = 112;
	US_Print("KEYS");
	VWB_Bar(119, 111, 34, 10, NUMBERBACK);
	for (i = 0; i < 4; i++)
	{
		if (gamestate.keys[i])
		{
			VWB_DrawTile8(i*8+120, 112, 36+i);
		}
	}

	PrintX = 176;
	PrintY = 112;
	US_Print("AMMO");
	VWB_Bar(215, 111, 26, 10, NUMBERBACK);
	PrintNumbers(216, 112, 3, 41, gamestate.ammo);

	PrintX = 80;
	PrintY = 128;
	US_Print("KEENS");
	VWB_Bar(127, 127, 18, 10, NUMBERBACK);
	PrintNumbers(128, 128, 2, 41, gamestate.lives);

	PrintX = 176;
	PrintY = 128;
	US_Print(DROPSNAME);
	VWB_Bar(224, 127, 16, 10, NUMBERBACK);
	PrintNumbers(224, 128, 2, 41, gamestate.drops);

#ifdef KEEN4
	VWB_Bar(79, 143, 66, 10, TEXTBACK);
	PrintY = 144;
	WindowX = 80;
	WindowW = 64;
	if (gamestate.wetsuit)
	{
		US_CPrint("Wetsuit");
	}
	else
	{
		US_CPrint("???");
	}
#endif

	// draw the tiles for "PRESS A KEY":
	for (i = 0; i < 10; i++)
	{
		VWB_DrawTile8(i*8+STATUS_PRESSKEY_X, 140, i+72);
		VWB_DrawTile8(i*8+STATUS_PRESSKEY_X, 148, i+82);
	}
}

/*
==================
=
= ScrollStatusWindow
=
==================
*/

void ScrollStatusWindow(void)
{
	Uint16 source, dest;
	Sint16 height;

	if (vislines > 152)
	{
		height = vislines - 152;
		source = windowofs + panadjust + 8;
		dest = bufferofs + panadjust + 8;
		VW_ScreenToScreen(source, dest, 192/BYTEPIXELS, height);
		VW_ClipDrawMPic((pansx+136)/BYTEPIXELS, -(16-height)+pansy, METALPOLEPICM);
		source = windowofs + panadjust + 16*SCREENWIDTH + 8*CHARWIDTH;
		dest = bufferofs + panadjust + height*SCREENWIDTH + 8;
		height = 152;
	}
	else
	{
		source = windowofs + panadjust + (152-vislines)*SCREENWIDTH + 16*SCREENWIDTH + 8*CHARWIDTH;
		dest = bufferofs + panadjust + 8;
		height = vislines;
	}
	if (height > 0)
	{
		VW_ScreenToScreen(source, dest, 192/BYTEPIXELS, height);
	}
	if (scrollup)
	{
		height = 168-vislines;
		source = masterofs + panadjust + vislines*SCREENWIDTH + 8;
		dest = bufferofs + panadjust + vislines*SCREENWIDTH + 8;
		VW_ScreenToScreen(source, dest, 192/BYTEPIXELS, height);
		height = vislines;
		source = windowofs + panadjust + 8 - 24/BYTEPIXELS;
		dest = bufferofs + panadjust + 8 - 24/BYTEPIXELS;
		if (height > 0)
			VW_ScreenToScreen(source, dest, 24/BYTEPIXELS, height);
	}
	else
	{
		height = vislines + -72;
		if (height > 0)
		{
			source = windowofs + panadjust + 8 - 24/BYTEPIXELS;
			dest = bufferofs + panadjust + 8 - 24/BYTEPIXELS;
			if (height > 0)
				VW_ScreenToScreen(source, dest, 24/BYTEPIXELS, height);
		}
	}
	if (vislines >= 72)
	{
		VW_ClipDrawMPic((pansx+40)/BYTEPIXELS, vislines-168+pansy, CORDPICM);
	}
	VW_UpdateScreen();
}

/*
==================
=
= StatusWindow
=
==================
*/

void StatusWindow(void)
{
#if GRMODE == CGAGR

	if (Keyboard[sc_A] && Keyboard[sc_2])
	{
		US_CenterWindow(20, 2);
		PrintY += 2;
		US_Print("Debug keys active");
		VW_UpdateScreen();
		IN_Ack();
		debugok = true;
	}

	WindowX = 0;
	WindowW = 320;
	WindowY = 0;
	WindowH = 200;
	DrawStatusWindow();
	VW_UpdateScreen();
	IN_ClearKeysDown();
	IN_Ack();

#else

	Uint16 oldbufferofs;

	WindowX = 0;
	WindowW = 320;
	WindowY = 0;
	WindowH = 200;

	if (Keyboard[sc_A] && Keyboard[sc_2])
	{
		US_CenterWindow(20, 2);
		PrintY += 2;
		US_Print("Debug keys active");
		VW_UpdateScreen();
		IN_Ack();
		debugok = true;
	}

	RF_Refresh();
	RFL_InitAnimList();
	oldbufferofs = bufferofs;
	bufferofs = windowofs = RF_FindFreeBuffer();
	VW_ScreenToScreen(displayofs, displayofs, 44, 224);	// useless (source and dest offsets are identical)
	VW_ScreenToScreen(displayofs, masterofs, 44, 224);
	VW_ScreenToScreen(displayofs, bufferofs, 44, 168);
	DrawStatusWindow();
	bufferofs = oldbufferofs;
	RF_Refresh();

	SD_PlaySound(SND_SHOWSTATUS);
	vislines = 16;
	scrollup = false;
	RF_SetRefreshHook(ScrollStatusWindow);

	while (true)
	{
		RF_Refresh();
		if (vislines == 168)
			break;
		vislines = vislines + tics*8;
		if (vislines > 168)
			vislines = 168;
	}

	RF_Refresh();
	RF_SetRefreshHook(NULL);
	IN_ClearKeysDown();
	IN_Ack();

	SD_PlaySound(SND_HIDESTATUS);
	vislines -= 16;
	scrollup = true;
	RF_SetRefreshHook(ScrollStatusWindow);

	while (true)
	{
		RF_Refresh();
		if (vislines == 0)
			break;
		vislines = vislines - tics*8;
		if (vislines < 0)
			vislines = 0;
	}

	RF_SetRefreshHook(NULL);

	scoreobj->x = 0;	//force scorebox to redraw?

#endif
}

//===========================================================================

/*
==================
=
= CenterActor
=
==================
*/

void CenterActor(objtype *ob)
{
	Uint16 orgx, orgy;

	centerlevel = 140;
	if (ob->x < 152*PIXGLOBAL)
	{
		orgx = 0;
	}
	else
	{
		orgx = ob->x - 152*PIXGLOBAL;
	}
	if (mapon == 0)
	{
		if (ob->y < 80*PIXGLOBAL)
		{
			orgy = 0;
		}
		else
		{
			orgy = ob->y - 80*PIXGLOBAL;
		}
	}
	else
	{
		if (ob->bottom < 140*PIXGLOBAL)
		{
			orgy = 0;
		}
		else
		{
			orgy = ob->bottom - 140*PIXGLOBAL;
		}
	}
	if (!scorescreenkludge)
	{
		RF_NewPosition(orgx, orgy);
	}

//
// update limits for onscreen and inactivate checks
//
	originxtilemax = (originxtile + PORTTILESWIDE) - 1;
	originytilemax = (originytile + PORTTILESHIGH) - 1;
	inactivateleft = originxtile - INACTIVATEDIST;
	if (inactivateleft < 0)
	{
		inactivateleft = 0;
	}
	inactivateright = originxtilemax + INACTIVATEDIST;
	if (inactivateright < 0)
	{
		inactivateright = 0;
	}
	inactivatetop = originytile - INACTIVATEDIST;
	if (inactivatetop < 0)
	{
		inactivatetop = 0;
	}
	inactivatebottom = originytilemax + INACTIVATEDIST;
	if (inactivatebottom < 0)
	{
		inactivatebottom = 0;
	}
}

//===========================================================================

/*
==================
=
= WorldScrollScreen
=
= Scroll if Keen is nearing an edge
=
==================
*/

void WorldScrollScreen(objtype *ob)
{
	Sint16 xscroll, yscroll;

	if (keenkilled)
		return;

	if (ob->left < originxglobal + 9*TILEGLOBAL)
	{
		xscroll = ob->left - (originxglobal + 9*TILEGLOBAL);
	}
	else if (ob->right > originxglobal + 12*TILEGLOBAL)
	{
		xscroll = ob->right + 16 - (originxglobal + 12*TILEGLOBAL);
	}
	else
	{
		xscroll = 0;
	}

	if (ob->top < originyglobal + 5*TILEGLOBAL)
	{
		yscroll = ob->top - (originyglobal + 5*TILEGLOBAL);
	}
	else if (ob->bottom > originyglobal + 7*TILEGLOBAL)
	{
		yscroll = ob->bottom - (originyglobal + 7*TILEGLOBAL);
	}
	else
	{
		yscroll = 0;
	}

	if (!xscroll && !yscroll)
		return;

//
// don't scroll more than one tile per frame
//
	if (xscroll >= 0x100)
	{
		xscroll = 0xFF;
	}
	else if (xscroll <= -0x100)
	{
		xscroll = -0xFF;
	}
	if (yscroll >= 0x100)
	{
		yscroll = 0xFF;
	}
	else if (yscroll <= -0x100)
	{
		yscroll = -0xFF;
	}

	RF_Scroll(xscroll, yscroll);

//
// update limits for onscreen and inactivate checks
//
	originxtilemax = (originxtile + PORTTILESWIDE) - 1;
	originytilemax = (originytile + PORTTILESHIGH) - 1;
	inactivateleft = originxtile - INACTIVATEDIST;
	if (inactivateleft < 0)
	{
		inactivateleft = 0;
	}
	inactivateright = originxtilemax + INACTIVATEDIST;
	if (inactivateright < 0)
	{
		inactivateright = 0;
	}
	inactivatetop = originytile - INACTIVATEDIST;
	if (inactivatetop < 0)
	{
		inactivatetop = 0;
	}
	inactivatebottom = originytilemax + INACTIVATEDIST;
	if (inactivatebottom < 0)
	{
		inactivatebottom = 0;
	}
}

//===========================================================================

/*
==================
=
= ScrollScreen
=
= Scroll if Keen is nearing an edge
= Set playstate to ex_completes
=
==================
*/

void ScrollScreen(objtype *ob)
{
	Sint16 xscroll, yscroll, pix, speed;
	Uint16 bottom;

	if (keenkilled)
		return;

//
// walked off edge of map?
//
	if (ob->left < originxmin || ob->right > originxmax + 320*PIXGLOBAL)
	{
		playstate = ex_completed;
		return;
	}

//
// fallen off bottom of world?
//
	if (ob->bottom > originymax + 13*TILEGLOBAL)
	{
		ob->y -= ob->bottom - (originymax + 13*TILEGLOBAL);
		SD_PlaySound(SND_PLUMMET);
		godmode = false;
		KillKeen();
		return;
	}

	xscroll=yscroll=0;

	if (ob->x < originxglobal + 9*TILEGLOBAL)
	{
		xscroll = ob->x - (originxglobal + 9*TILEGLOBAL);
	}
	else if (ob->x > originxglobal + 12*TILEGLOBAL)
	{
		xscroll = ob->x - (originxglobal + 12*TILEGLOBAL);
	}

	if (ob->state == &s_keenlookup2)
	{
		if (centerlevel+tics > 167)
		{
			pix = 167-centerlevel;
		}
		else
		{
			pix = tics;
		}
		centerlevel += pix;
		yscroll = CONVERT_PIXEL_TO_GLOBAL(-pix);
	}
	else if (ob->state == &s_keenlookdown3)
	{
		if (centerlevel-tics < 33)
		{
			pix = centerlevel + -33;
		}
		else
		{
			pix = tics;
		}
		centerlevel -= pix;
		yscroll = CONVERT_PIXEL_TO_GLOBAL(pix);
	}

#ifdef KEEN6
	if (groundslam)
	{
		static Sint16 shaketable[] = {0,
			 -64,  -64,  -64,  64,  64,  64,
			-200, -200, -200, 200, 200, 200,
			-250, -250, -250, 250, 250, 250,
			-250, -250, -250, 250, 250, 250
		};
		yscroll = yscroll + (bottom - (ob->bottom + shaketable[groundslam]));	// BUG: 'bottom' has not been initialized yet!
	}
	else
#endif
	if ( (ob->hitnorth || !ob->needtoclip || ob->state == &s_keenholdon))
	{
		if (  ob->state != &s_keenclimbup
			&& ob->state != &s_keenclimbup2
			&& ob->state != &s_keenclimbup3
			&& ob->state != &s_keenclimbup4)
		{
			yscroll += ob->ymove;
			bottom = originyglobal + yscroll + CONVERT_PIXEL_TO_GLOBAL(centerlevel);
			if (ob->bottom == bottom)
			{
				// player didn't move, no additional scrolling
			}
			else
			{
				if (ob->bottom < bottom)
				{
					pix = bottom - ob->bottom;
				}
				else
				{
					pix = ob->bottom - bottom;
				}
				speed = CONVERT_PIXEL_TO_GLOBAL(pix) >> 7;
				if (speed > 0x30)
				{
					speed = 0x30;
				}
				speed *= tics;
				if (speed < 0x10)
				{
					if (pix < 0x10)
					{
						speed = pix;
					}
					else
					{
						speed = 0x10;
					}
				}
				if (ob->bottom < bottom)
				{
					yscroll -= speed;
				}
				else
				{
					yscroll += speed;
				}
			}
		}
	}
	else
	{
		centerlevel = 140;
	}

	pix = (ob->bottom-32*PIXGLOBAL)-(originyglobal+yscroll);
	if (pix < 0)
	{
		yscroll += pix;
	}
	pix = (ob->bottom+32*PIXGLOBAL)-(originyglobal+yscroll+200*PIXGLOBAL);
	if (pix > 0)
	{
		yscroll += pix;
	}

	if (xscroll == 0 && yscroll == 0)
		return;

//
// don't scroll more than one tile per frame
//
	if (xscroll >= 0x100)
	{
		xscroll = 0xFF;
	}
	else if (xscroll <= -0x100)
	{
		xscroll = -0xFF;
	}
	if (yscroll >= 0x100)
	{
		yscroll = 0xFF;
	}
	else if (yscroll <= -0x100)
	{
		yscroll = -0xFF;
	}
	RF_Scroll(xscroll, yscroll);

//
// update limits for onscreen and inactivate checks
//
	originxtilemax = (originxtile + PORTTILESWIDE) - 1;
	originytilemax = (originytile + PORTTILESHIGH) - 1;
	inactivateleft = originxtile - INACTIVATEDIST;
	if (inactivateleft < 0)
	{
		inactivateleft = 0;
	}
	inactivateright = originxtilemax + INACTIVATEDIST;
	if (inactivateright < 0)
	{
		inactivateright = 0;
	}
	inactivatetop = originytile - INACTIVATEDIST;
	if (inactivatetop < 0)
	{
		inactivatetop = 0;
	}
	inactivatebottom = originytilemax + INACTIVATEDIST;
	if (inactivatebottom < 0)
	{
		inactivatebottom = 0;
	}
}

//===========================================================================


/*
#############################################################################

				  The objarray data structure

#############################################################################

Objarray contains structures for every actor currently playing.  The structure
is accessed as a linked list starting at *player, ending when ob->next ==
NULL.  GetNewObj inserts a new object at the end of the list, meaning that
if an actor spawns another actor, the new one WILL get to think and react the
same frame.  RemoveObj unlinks the given object and returns it to the free
list, but does not damage the objects ->next pointer, so if the current object
removes itself, a linked list following loop can still safely get to the
next element.

<backwardly linked free list>

#############################################################################
*/


/*
=========================
=
= InitObjArray
=
= Call to clear out the entire object list, returning them all to the free
= list.  Allocates a special spot for the player.
=
=========================
*/

void InitObjArray(void)
{
	Sint16 i;

	for (i=0; i<MAXACTORS; i++)
	{
		objarray[i].prev = &objarray[i+1];
		objarray[i].next = NULL;
	}

	objarray[MAXACTORS-1].prev = NULL;

	objfreelist = &objarray[0];
	lastobj = NULL;

	objectcount = 0;

//
// give the player and score the first free spots
//
	GetNewObj(false);
	player = new;
	GetNewObj(false);
	scoreobj = new;
}

//===========================================================================

/*
=========================
=
= GetNewObj
=
= Sets the global variable new to point to a free spot in objarray.
= The free spot is inserted at the end of the liked list
=
= When the object list is full, the caller can either have it bomb out or
= return a dummy object pointer that will never get used
=
= Returns -1 when list was full, otherwise returns 0.
=
=========================
*/

Sint16 GetNewObj(boolean usedummy)
{
	if (!objfreelist)
	{
		if (usedummy)
		{
			new = &dummyobj;
			return -1;
		}
		Quit("GetNewObj: No free spots in objarray!");
	}
	new = objfreelist;
	objfreelist = new->prev;
	memset(new, 0, sizeof(*new));
	if (lastobj)
	{
		lastobj->next = new;
	}
	new->prev = lastobj;	// new->next is allready NULL from memset

	new->active = ac_yes;
	new->needtoclip = cl_midclip;
	lastobj = new;

	objectcount++;
	return 0;
}

//===========================================================================

/*
=========================
=
= RemoveObj
=
= Add the given object back into the free list, and unlink it from it's
= neighbors
=
=========================
*/

void RemoveObj(objtype *ob)
{
	if (ob == player)
		Quit("RemoveObj: Tried to remove the player!");

//
// erase it from the refresh manager
//
	RF_RemoveSprite(&ob->sprite);
	if (ob->obclass == stunnedobj)
	{
		RF_RemoveSprite((void **)&ob->temp3);
	}

//
// fix the next object's back link
//
	if (ob == lastobj)
	{
		lastobj = ob->prev;
	}
	else
	{
		ob->next->prev = ob->prev;
	}

//
// fix the previous object's forward link
//
	ob->prev->next = ob->next;

//
// add it back in to the free list
//
	ob->prev = objfreelist;
	objfreelist = ob;
}

//==========================================================================

/*
====================
=
= GivePoints
=
= Grants extra men at 20k,40k,80k,160k,320k
=
====================
*/

void GivePoints(Uint16 points)
{
	gamestate.score += points;
	if (!DemoMode && gamestate.score >= gamestate.nextextra)
	{
		SD_PlaySound(SND_EXTRAKEEN);
		gamestate.lives++;
		gamestate.nextextra *= 2;
	}
}

//==========================================================================

/*
===================
=
= PollControls
=
===================
*/

void PollControls(void)
{
	IN_ReadControl(0, &c);
	if (c.yaxis != -1)
		upheld = false;

	if (GravisGamepad && !DemoMode)
	{
		jumpbutton = GravisAction[ga_Jump];
		pogobutton = GravisAction[ga_Pogo];
		firebutton = GravisAction[ga_Fire];
		if (!jumpbutton)
			jumpheld = false;
		if (!pogobutton)
			pogoheld = false;
		if (!firebutton)
			fireheld = false;
	}
	else if (oldshooting || DemoMode)
	{
		if (c.button0 && c.button1)
		{
			firebutton = true;
			jumpbutton = pogobutton = jumpheld = pogoheld = false;
		}
		else
		{
			firebutton = fireheld = false;
			if (c.button0)
			{
				jumpbutton = true;
			}
			else
			{
				jumpbutton = jumpheld = false;
			}
			if (c.button1)
			{
				if (oldfirecount <= 8)
				{
					oldfirecount = oldfirecount + tics;
				}
				else
				{
					pogobutton = true;
				}
			}
			else
			{
				if (oldfirecount != 0)
				{
					pogobutton = true;
				}
				else
				{
					pogobutton = pogoheld = false;
				}
				oldfirecount = 0;
			}
		}
	}
	else
	{
		jumpbutton = c.button0;
		pogobutton = c.button1;
		firebutton = Keyboard[firescan];
		if (!jumpbutton)
			jumpheld = false;
		if (!pogobutton)
			pogoheld = false;
		if (!firebutton)
			fireheld = false;
	}
}

//==========================================================================


/*
=================
=
= StopMusic
=
=================
*/

void StopMusic(void)
{
	Sint16 i;

	SD_MusicOff();
	for (i=0; i<LASTMUSIC; i++)
	{
		if (audiosegs[STARTMUSIC+i])
		{
#ifdef FIX_MUSIC_MEMORY_ISSUES
			//unlock any music blocks so that they can be purged
			MM_SetLock(&(memptr)audiosegs[STARTMUSIC+i], false);
#endif
			MM_SetPurge(&(memptr)audiosegs[STARTMUSIC+i], PURGE_FIRST);
		}
	}
}

//==========================================================================

/*
=================
=
= StartMusic
=
=================
*/

void StartMusic(Uint16 num)
{
	static Sint16 songs[] =
	{
#if defined KEEN4
		SHADOWS_MUS,
		KICKPANT_MUS,
		OASIS_MUS,
		OASIS_MUS,
		TOOHOT_MUS,
		TOOHOT_MUS,
		KICKPANT_MUS,
		OASIS_MUS,
		VEGGIES_MUS,
		VEGGIES_MUS,
		VEGGIES_MUS,
		TOOHOT_MUS,
		TOOHOT_MUS,
		TOOHOT_MUS,
		TOOHOT_MUS,
		TOOHOT_MUS,
		TOOHOT_MUS,
		VEGGIES_MUS,
		OASIS_MUS,
		-1
#elif defined KEEN5
		ROBOROCK_MUS,
		WEDNESDY_MUS,
		BREATHE_MUS,
		SPHEREFUL_MUS,
		TIGHTER_MUS,
		SPHEREFUL_MUS,
		TIGHTER_MUS,
		SPHEREFUL_MUS,
		TIGHTER_MUS,
		SPHEREFUL_MUS,
		TIGHTER_MUS,
		SNOOPING_MUS,
		FEARSOME_MUS,
		BAGPIPES_MUS,
		FANFARE_MUS,
		SKATING_MUS,
		ROCK_ME_MUS,
		HAVING_T_MUS,
		CAMEIN_MUS,
		SHIKAIRE_MUS,
#elif defined KEEN6
		ALIENATE_MUS,
		FASTER_MUS,
		BRERTAR_MUS,
		MAMSNAKE_MUS,
		MAMSNAKE_MUS,
		MAMSNAKE_MUS,
		METAL_MUS,
		TOFUTURE_MUS,
		METAL_MUS,
		BRERTAR_MUS,
		FASTER_MUS,
		TOFUTURE_MUS,
		BRERTAR_MUS,
		SPACFUNK_MUS,
		SPACFUNK_MUS,
		OMINOUS_MUS,
		TOFUTURE_MUS,
		WONDER_MUS,
		WONDER_MUS,
		WONDER_MUS
#endif
	};

	Sint16 song;
	boolean wasfaded;

	if (num >= ARRAYLENGTH(songs) && num != 0xFFFF)
	{
		Quit("StartMusic() - bad level number");
	}

#ifdef FIX_MUSIC_MEMORY_ISSUES
	StopMusic();
#else
	SD_MusicOff();
#endif

#ifdef KEEN4
	if (num == 0xFFFF)
	{
		song = WONDER_MUS;
	}
	else
	{
		song = songs[num];
	}
#else
	song = songs[num];
#endif

	if (song == -1 || MusicMode != smm_AdLib)
	{
		return;
	}

	MM_BombOnError(false);
	CA_CacheAudioChunk(STARTMUSIC+song);
	MM_BombOnError(true);
	if (mmerror)
	{
		mmerror = false;
		if (!DemoMode)
		{
			US_CenterWindow(20, 8);
			PrintY += 20;
			US_CPrint("Insufficient memory\nfor background music!");
			VW_UpdateScreen();
			wasfaded = screenfaded;
			if (wasfaded)
			{
				VW_SetDefaultColors();
			}
			IN_ClearKeysDown();
			IN_UserInput(3*TickBase, false);
			if (wasfaded)
			{
				VW_FadeOut();
			}
		}
	}
	else
	{
#ifdef FIX_MUSIC_MEMORY_ISSUES
		//The current music should be locked, so the memory manager will not
		//mess with it when compressing memory blocks in MM_SortMem().
		MM_SetLock(&(memptr)audiosegs[STARTMUSIC+song], true);
#endif
		SD_StartMusic((MusicGroup far *)audiosegs[STARTMUSIC+song]);
	}
}

//==========================================================================


/*
===================
=
= PlayLoop
=
===================
*/

void PlayLoop(void)
{
	objtype *check;

	StartMusic(gamestate.mapon);
	fireheld = pogoheld = upheld = jumpheld = false;
	ingame = true;
	playstate = ex_stillplaying;
	invincible = keenkilled = oldfirecount = 0;

	CenterActor(player);

	if (DemoMode)
	{
		US_InitRndT(false);
	}
	else
	{
		US_InitRndT(true);
	}
	TimeCount = lasttimecount = tics = 3;

	do
	{
		PollControls();

//
// go through state changes and propose movements
//
		for (obj=player; obj; obj=obj->next)
		{
			if (!obj->active && obj->tileright >= originxtile-1
				&& obj->tileleft <= originxtilemax+1 && obj->tiletop <= originytilemax+1
				&& obj->tilebottom >= originytile-1)
			{
				obj->needtoreact = true;
				obj->active = ac_yes;
			}
			if (obj->active)
			{
				if (obj->tileright < inactivateleft
					|| obj->tileleft > inactivateright
					|| obj->tiletop > inactivatebottom
					|| obj->tilebottom < inactivatetop)
				{
					if (obj->active == ac_removable)
					{
						RemoveObj(obj);
						continue;
					}
					else if (obj->active != ac_allways)
					{
						if (US_RndT() < tics*2 || screenfaded || loadedgame)
						{
							RF_RemoveSprite(&obj->sprite);
							if (obj->obclass == stunnedobj)
								RF_RemoveSprite((void **)&obj->temp3);
							obj->active = ac_no;
							continue;
						}
					}
				}
				StateMachine(obj);
			}
		}

		if (gamestate.riding)
		{
			HandleRiding(player);
		}

//
// check for and handle collisions between objects
//
		for (obj=player; obj; obj=obj->next)
		{
			if (obj->active)
			{
				for (check=obj->next; check; check=check->next)
				{
					if (!check->active)
					{
						continue;
					}
					if (obj->right > check->left && obj->left < check->right
						&& obj->top < check->bottom && obj->bottom > check->top)
					{
						if (obj->state->contact)
						{
							obj->state->contact(obj, check);
						}
						if (check->state->contact)
						{
							check->state->contact(check, obj);
						}
						if (obj->obclass == nothing)	//useless -- obclass is NOT set to nothing by RemoveObj
						{
							break;
						}
					}
				}
			}
		}

//
// check intiles
//
		if (mapon != 0)
		{
			CheckInTiles(player);
		}
		else
		{
			CheckWorldInTiles(player);
		}

//
// react to whatever happened, and post sprites to the refresh manager
//
		for (obj=player; obj; obj=obj->next)
		{
			if (!obj->active)
			{
				continue;
			}
			if (obj->tilebottom >= mapheight-1)
			{
				if (obj->obclass == keenobj)
				{
					playstate = ex_died;
				}
				else
				{
					RemoveObj(obj);
				}
				continue;
			}
			if (obj->needtoreact && obj->state->react)
			{
				obj->needtoreact = false;
				obj->state->react(obj);
			}
		}

//
// scroll the screen and update the score box
//
#ifdef KEEN4
		if (mapon != 0 && mapon != 17)
#else
		if (mapon != 0)
#endif
		{
			ScrollScreen(player);
		}
		else
		{
			WorldScrollScreen(player);
		}
		UpdateScore(scoreobj);
		if (loadedgame)
		{
			loadedgame = false;
		}

//
// update the screen and calculate the number of tics it took to execute
// this cycle of events (for adaptive timing of next cycle)
//
		RF_Refresh();

		if (invincible)
		{
			if ((invincible = invincible - tics) < 0)
				invincible = 0;
		}

#ifdef KEEN6
		if (groundslam)
		{
			if ((groundslam = groundslam - tics) < 0)
				groundslam = 0;
		}
#endif
//
// single step debug mode
//
		if (singlestep)
		{
			VW_WaitVBL(14);	//reduces framerate to 5 fps on VGA or 4.3 fps on EGA cards
			lasttimecount = TimeCount;
		}
//
// extra VBLs debug mode
//
		if (extravbls)
		{
			VW_WaitVBL(extravbls);
		}

//
// handle user inputs
//
		if (DemoMode == demo_Playback)
		{
			if (!screenfaded && IN_IsUserInput())
			{
				playstate = ex_completed;
				if (LastScan != sc_F1)
				{
					LastScan = sc_Space;
				}
			}
		}
		else if (DemoMode == demo_PlayDone)
		{
			playstate = ex_completed;
		}
		else
		{
			CheckKeys();
		}

//
// E-N-D cheat
//
		if (Keyboard[sc_E] && Keyboard[sc_N] && Keyboard[sc_D])
		{
#if defined KEEN4
			gamestate.rescued = 7;
			playstate = ex_rescued;
#elif defined KEEN5
			playstate = ex_qedbroke;
#elif defined KEEN6
			playstate = ex_molly;
#endif
		}

	} while (playstate == ex_stillplaying);

	ingame = false;
	StopMusic();
}