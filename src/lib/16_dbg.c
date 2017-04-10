#include "src/lib/16_dbg.h"

#ifdef __DEBUG__
#ifdef __DEBUG_MM__
boolean dbg_debugmm=0;
#endif
#ifdef __DEBUG_PM__
boolean dbg_debugpm=0;
#endif
#ifdef __DEBUG_CA__
boolean dbg_debugca=0;
#endif
#ifdef __DEBUG_InputMgr__
boolean dbg_testkeyin=0,dbg_testcontrolnoisy=0,dbg_nointest=0;
#endif
#ifdef __DEBUG_MAP__
boolean dbg_maptext=0;
byte *dbg_mapdata;
#endif
#ifdef __DEBUG_RF__
boolean dbg_pagenorendermap=0,dbg_pagedelayrendermap=0;
#endif
#ifdef __DEBUG_SPRI__
boolean dbg_delayanimation=0;
#endif
#endif	//debug


/*
================
=
= ShapeTest
=
================
*/

#pragma warn -pia
void ShapeTest_(global_game_variables_t *gvar)
{
extern	word	NumDigi;
extern	word	_seg *DigiList;
static	char	buf[10];

	boolean			done;
	ScanCode		scan;
	int				i,j;
//	dword		l; word	k,x;
	memptr			addr;
	PageListStruct	far *page;

//	CenterWindow(20,16);
//	VW_UpdateScreen();
	for (i = 0,done = false;!done;)
	{
//		US_ClearWindow();
//		sound = -1;

		page = &(gvar->pm.PMPages[i]);
//		US_Print(" Page #");
//		US_PrintUnsigned(i);
		printf(" Page #%u", i);
//++		if (i < (gvar->pm.fi.PMSpriteStart))
//			US_Print(" (Wall)");
//++			printf(" (Wall)");
/*		else if (i < (gvar->pm.fi.PMSoundStart))
//			US_Print(" (Sprite)");
			printf(" (Sprite)");
		else if (i == (gvar->pm.fi.ChunksInFile - 1))
//			US_Print(" (Sound Info)");
			printf(" (Sound Info)");
		else
//			US_Print(" (Sound)");
			printf(" (Sound)");*/

//		US_Print("\n XMS: ");
		printf("\n XMS: ");
		if (page->xmsPage != -1)
//			US_PrintUnsigned(page->xmsPage);
			printf("%u", page->xmsPage);
		else
//			US_Print("No");
			printf("No");

//		US_Print("\n Main: ");
		if (page->mainPage != -1)
//			US_PrintUnsigned(page->mainPage);
			printf("\n Main: %u", page->mainPage);
		else if (page->emsPage != -1)
		{
//			US_Print("EMS ");
//			US_PrintUnsigned(page->emsPage);
			printf("EMS %u", page->emsPage);
		}
		else
//			US_Print("No");
			printf("No");

//		US_Print("\n Last hit: ");
//		US_PrintUnsigned(page->lastHit);
		printf("\n Last hit: %u", page->lastHit);

//		US_Print("\n Address: ");
		printf("\n Address: ");
		addr = PM_GetPageAddress(i, gvar);
		sprintf(buf,"0x%04x",(word)addr);
//		US_Print(buf);
		printf("%s", buf);

		if (addr)
		{
/*			if (i < PMSpriteStart)
			{
			//
			// draw the wall
			//
				bufferofs += 32*SCREENWIDTH;
				postx = 128;
				postwidth = 1;
				postsource = ((long)((unsigned)addr))<<16;
				for (x=0;x<64;x++,postx++,postsource+=64)
				{
					wallheight[postx] = 256;
					FarScalePost ();
				}
				bufferofs -= 32*SCREENWIDTH;
			}
			else if (i < PMSoundStart)
			{
			//
			// draw the sprite
			//
				bufferofs += 32*SCREENWIDTH;
				SimpleScaleShape (160, i-PMSpriteStart, 64);
				bufferofs -= 32*SCREENWIDTH;
			}
			else if (i == ChunksInFile - 1)
			{
				US_Print("\n\n Number of sounds: ");
				US_PrintUnsigned(NumDigi);
				for (l = j = k = 0;j < NumDigi;j++)
				{
					l += DigiList[(j * 2) + 1];
					k += (DigiList[(j * 2) + 1] + (PMPageSize - 1)) / PMPageSize;
				}
				US_Print("\n Total bytes: ");
				US_PrintUnsigned(l);
				US_Print("\n Total pages: ");
				US_PrintUnsigned(k);
			}
			else
			{
				byte far *dp = (byte far *)MK_FP(addr,0);
				for (j = 0;j < NumDigi;j++)
				{
					k = (DigiList[(j * 2) + 1] + (PMPageSize - 1)) / PMPageSize;
					if
					(
						(i >= PMSoundStart + DigiList[j * 2])
					&&	(i < PMSoundStart + DigiList[j * 2] + k)
					)
						break;
				}
				if (j < NumDigi)
				{
//					sound = j;
					US_Print("\n Sound #");
					US_PrintUnsigned(j);
					US_Print("\n Segment #");
					US_PrintUnsigned(i - PMSoundStart - DigiList[j * 2]);
				}
				for (j = 0;j < page->length;j += 32)
				{
					byte v = dp[j];
					int v2 = (unsigned)v;
					v2 -= 128;
					v2 /= 4;
					if (v2 < 0)
						VWB_Vlin(WindowY + WindowH - 32 + v2,
								WindowY + WindowH - 32,
								WindowX + 8 + (j / 32),BLACK);
					else
						VWB_Vlin(WindowY + WindowH - 32,
								WindowY + WindowH - 32 + v2,
								WindowX + 8 + (j / 32),BLACK);
				}
			}*/
			printf("\naddr ok\n");
		}

//		VW_UpdateScreen();

		while (!(scan = gvar->in.inst->LastScan))
		{}
//			SD_Poll();

		IN_ClearKey(scan);
		switch (scan)
		{
		case sc_LeftArrow:
			if (i)
				i--;
			break;
		case sc_RightArrow:
			if (++i >= (gvar->pm.fi.ChunksInFile))
				i--;
			break;
		case sc_W:	// Walls
			i = 0;
			break;
		case sc_S:	// Sprites
//			i = (gvar->pm.fi.PMSpriteStart);
			break;
		case sc_D:	// Digitized
//			i = (gvar->pm.fi.PMSoundStart);
			break;
		case sc_I:	// Digitized info
			i = (gvar->pm.fi.ChunksInFile - 1);
			break;
		case sc_L:	// Load all pages
			for (j = 0;j < (gvar->pm.fi.ChunksInFile);j++)
				PM_GetPage(j, gvar);
			break;
		case sc_P:
//			if (sound != -1)
//				SD_PlayDigitized(sound);
			break;
		case sc_Escape:
			done = true;
			break;
		case sc_Enter:
			PM_GetPage(i, gvar);
			break;
		}
	}
	//SD_StopDigitized();
}
#pragma warn +pia
