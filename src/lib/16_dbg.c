/* Project 16 Source Code~
 * Copyright (C) 2012-2020 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
 *
 * This file is part of Project 16.
 *
 * Project 16 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Project 16 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 * write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

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
boolean dbg_testkeyin=0,dbg_testcontrolnoisy=0,dbg_nointest=0,dbg_joymousedelta=0;
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

#ifdef __WATCOMC__
#ifdef __DEBUG_VL__

//===========================================================================

/*
================
=
= ShowPalVarSync
=
================
*/

//#define SHOWPALVARIMODQUAD	((spv.i)%QUADWH)
#define SHOWPALVARIMOD			((spv.i)%TILEWH)
#define SHOWPALVARIDIV			((spv.i)/TILEWH)
#define SHOWPALVARIMODIFCOND	if((!SHOWPALVARIMOD) && spv.i)
#define SHOWPALVARIDIVIFCOND		if((!SHOWPALVARIDIV) && spv.i)
//<PAL_SIZE/3 && spv.i)
#define SHOWPALVARPALSIZELIMIT	255

spv_t ShowPalVarSync (spv_t *spv)
{
	spv_t		result;
	result = *spv;

//	if(result.paly<result.palq+TILEWH*8) result.paly+=result.mult;
//	if(result.palx<result.palq+TILEWH*12) result.palx+=result.mult;
//	modexClearRegion(&gvar->video.page[0], result.palx+TILEWH, result.paly+TILEWH, result.mult, result.mult, result.i);
	return result;
}

/*
================
=
= ShowPalVal
=
================
*/

void ShowPalVal (global_game_variables_t *gvar)
{
	boolean			done				,err;
	ScanCode		scan;
	spv_t			spv;
	word ccolor = 3, xpos = gvar->video.page[0].dx, ypos = gvar->video.page[0].dy;

	spv.mult=(QUADWH);
	spv.palq=(spv.mult)*TILEWH;
	spv.i = 0;

//	IN_UserInput(1, gvar);
	modexpdump(0, gvar);

	for (spv.oi = 1,done = false;!done;)
	{
		SHOWPALVARIMODIFCOND
		{
			if(err) printf("SHOWPALVARIMODIFCOND\n");
			if(spv.i==SHOWPALVARPALSIZELIMIT) spv.paly=(TILEWH*8)+spv.mult*SHOWPALVARIDIV;
			spv.palx=(TILEWH*12);
		}else	spv.palx=(TILEWH*12)+spv.mult*SHOWPALVARIMOD;
		if(spv.i<SHOWPALVARPALSIZELIMIT) SHOWPALVARIDIVIFCOND
		{
			if(err) printf("SHOWPALVARIDIVIFCOND\n");
			spv.paly=(TILEWH*8);
		}else	spv.paly=(TILEWH*8)+spv.mult*SHOWPALVARIDIV;

		if((spv.palx<TILEWH*12 || spv.paly<TILEWH*8) && spv.i) err=true;
		else err = false;
		if(err){ printf("ERR	"); printf("%ux%u	%u\n", spv.palx, spv.paly, spv.i); }

		if(spv.oi!=spv.i)
		{
			modexpdump(0, gvar);
			modexClearRegion(&gvar->video.page[0], spv.palx+TILEWH, spv.paly+TILEWH, spv.mult, spv.mult, 5);
			modexClearRegion(&gvar->video.page[0], spv.palx+TILEWH+1, spv.paly+TILEWH+1, spv.mult-2, spv.mult-2, spv.i);
			spv.oi = spv.i;
		}

#define SHOWPALVARPRINT modexprint(&(gvar->video.page[0]), xpos, ypos, 1, 1, ccolor, 8, gvar->video.VL_Started, global_temp_status_text); ypos+=8;
		sprintf(global_temp_status_text, "%03u", spv.i); SHOWPALVARPRINT
		sprintf(global_temp_status_text, "r %03u", gvar->video.palette[(spv.i*3)+0]/*>>2*/); SHOWPALVARPRINT
		sprintf(global_temp_status_text, "g %03u", gvar->video.palette[(spv.i*3)+1]/*>>2*/); SHOWPALVARPRINT
		sprintf(global_temp_status_text, "b %03u", gvar->video.palette[(spv.i*3)+2]/*>>2*/); SHOWPALVARPRINT
		xpos = gvar->video.page[0].dx; ypos = gvar->video.page[0].dy;

		while (!(scan = gvar->in.inst->LastScan))
		{}

		IN_ClearKey(scan);
		switch (scan)
		{
		case sc_LeftArrow:
			if (spv.i > 0)
			{
				spv.i--;
			}
			else
			{
				spv.palx=gvar->video.page[0].sw-spv.mult;
				spv.paly=gvar->video.page[0].sh-spv.mult;
				spv.i = SHOWPALVARPALSIZELIMIT;
			}
		break;
		case sc_RightArrow:
			if (spv.i < SHOWPALVARPALSIZELIMIT)
			{
				spv.i++;
			}
				else
			{
				spv.palx=TILEWH*12;
				spv.paly=TILEWH*8;
				spv.i = 0;
			}
		break;
		case sc_UpArrow:
			if (spv.i > 0)
			{
				spv.i-=16;
			}
			else
			{
				spv.palx=gvar->video.page[0].sw-spv.mult;
				spv.paly=gvar->video.page[0].sh-spv.mult;
				spv.i = SHOWPALVARPALSIZELIMIT;
			}
		break;
		case sc_DownArrow:
			if (spv.i < SHOWPALVARPALSIZELIMIT)
			{
				spv.i+=16;
			}
				else
			{
				spv.palx=TILEWH*12;
				spv.paly=TILEWH*8;
				spv.i = 0;
			}
		break;
		case sc_W:	// Walls
			spv.i = 0;
			break;
		case sc_Escape:
			done = true;
			break;
		}
	}
//	IN_UserInput(1, gvar);
}
#endif	//debug vl
#endif	//watcomc
