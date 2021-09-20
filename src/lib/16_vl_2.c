/* Project 16 Source Code~
 * Copyright (C) 2012-2021 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include "src/lib/16_vl.h"

#define PRINTTEXTBOXSTABLE

void VL_modexPrintTextBox(global_game_variables_t *gvar)
{
#define PRINTTEXTBOXW	gvar->video.page[0].width
#define PRINTTEXTBOXH	gvar->video.page[0].height-96
#define PRINTTEXTBOXY	160-48//112//184//gvar->video.page[0].height-100-16
//#define PRINTTEXTBOXSIZE	20480//51200
#define PRINTTEXTBOXHLINE		0xCD,	0xCD,	0xCD,	0xCD,	0xCD,	0xCD,	0xCD,	0xCD,	0xCD,	0xCD,	0xCD,	0xCD,	0xCD,	0xCD,	0xCD,	0xCD,	0xCD,	0xCD,	0xCD,	0xCD,	0xCD,	0xCD,	0xCD,	0xCD,	0xCD,	0xCD,	0xCD,	0xCD//,	0xCD,	0xCD,	0xCD,	0xCD
#define PRINTTEXTBOXHLINETOP		0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCB, 0xCD, PRINTTEXTBOXHLINE
#define PRINTTEXTBOXHLINEBOTTOM	0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCA, 0xCD, PRINTTEXTBOXHLINE
#define PRINTTEXTBOXSLINE		0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20//,	0x20,	0x20,	0x20,	0x20
#define PRINTTEXTBOXSTRA2CHAR	0x20//07
#define PRINTTEXTBOXSTRA2STR	PRINTTEXTBOXSTRA2CHAR, PRINTTEXTBOXSTRA2CHAR, PRINTTEXTBOXSTRA2CHAR, PRINTTEXTBOXSTRA2CHAR, PRINTTEXTBOXSTRA2CHAR, PRINTTEXTBOXSTRA2CHAR, PRINTTEXTBOXSTRA2CHAR, PRINTTEXTBOXSTRA2CHAR
	byte	str1[41] = { 0xC9, PRINTTEXTBOXHLINETOP, 0xBB, 0x00 };
#ifdef PRINTTEXTBOXSTABLE
	byte	str2[41] = { 0xBA, PRINTTEXTBOXSTRA2STR, 0xBA, PRINTTEXTBOXSLINE, 0xBA, 0x00 };
#else
	byte	str2[2] = { 0xBA, 0x00 },
		stra2[7] = { PRINTTEXTBOXSTRA2STR, 0x00 },
		strb2[] = {  0xBA, PRINTTEXTBOXSLINE, 0xBA, 0x00 };
#endif
	byte	str3[41] = { 0xC8, PRINTTEXTBOXHLINEBOTTOM, 0xBC, 0x00 };
	word x = gvar->video.page[/*!*/(gvar->video.p)].dx;	//(gv->video.page[(gv->video.p)].tlx) - // follow the screen
	word y = gvar->video.page[/*!*/(gvar->video.p)].dy;	//(gv->video.page[(gv->video.p)].tly) - // follow the screen
	word col = 3, bgcol = 0, type = 1;//w = 64, h = 8,
	word v = PRINTTEXTBOXY;	//vertical offset
	nibble i;
	boolean			done;
	ScanCode		scan;
//	word		huge	textboxbuff[PRINTTEXTBOXSIZE];
#ifndef PRINTTEXTBOXSTABLE
	word q;
#endif
	//backuppart
	modexCopyPageRegion(&gvar->video.page[1], &gvar->video.page[0],
			0,
			PRINTTEXTBOXY,
			0,
			PRINTTEXTBOXY,
			PRINTTEXTBOXW, PRINTTEXTBOXH);
//	memcpy(&textboxbuff, gvar->video.page[0].data, PRINTTEXTBOXSIZE);
// 		mxOutText(xpos+1, ypos+gvar.video.page[0].height-40, "|    |Chikyuu:$line1");
// 		mxOutText(xpos+1, ypos+gvar.video.page[0].height-32, "|    |$line2");
// 		mxOutText(xpos+1, ypos+gvar.video.page[0].height-24, "|    |$line3");
// 		mxOutText(xpos+1, ypos+gvar.video.page[0].height-16, "|    |$line4");
// 		mxOutText(xpos+1, ypos+gvar.video.page[0].height-8,  "========================================");

	//modexprint(*page, x, y, type, tlsw, color, bgcolor, sw, *str)
	sprintf(global_temp_status_text, "%s", str1);
	modexprint(&(gvar->video.page[(gvar->video.p)]), x, y+gvar->video.page[0].height-v, type, 1, col, bgcol, 1, global_temp_status_text); v-=8;
#ifdef PRINTTEXTBOXSTABLE
	sprintf(global_temp_status_text, "%s", str2); for(i=0;i<8;i++)
{	modexprint(&(gvar->video.page[(gvar->video.p)]), x, y+gvar->video.page[0].height-v, type, 1, col, bgcol, 1, global_temp_status_text); v-=8;	}
#else
	for(i=0;i<8;i++)
	{
	q=0;
	sprintf(global_temp_status_text, "%s", str2);	modexprint(&(gvar->video.page[(gvar->video.p)]), x, y+gvar->video.page[0].height-v, type, 1, col, bgcol, 1,		global_temp_status_text);	q+=8;
	sprintf(global_temp_status_text, "%s", stra2);	modexprint(&(gvar->video.page[(gvar->video.p)]), x+q, y+gvar->video.page[0].height-v, type, 1, col, 3, 1,		global_temp_status_text);	q+=40;
	sprintf(global_temp_status_text, "%s", strb2);	modexprint(&(gvar->video.page[(gvar->video.p)]), x+q-8, y+gvar->video.page[0].height-v, type, 1, col, bgcol, 1,	global_temp_status_text);
		v-=8;
	}
#endif
	sprintf(global_temp_status_text, "%s", str3);
	modexprint(&(gvar->video.page[(gvar->video.p)]), x, y+gvar->video.page[0].height-v, type, 1, col, bgcol, 1, global_temp_status_text);
	//PRINT TEXT
//		sprintf(global_temp_status_text, "%c", 0x07);	modexprint(&(gvar->video.page[(gvar->video.p)]), x, y+gvar->video.page[0].height-v, type, 1, col, bgcol, 1,		global_temp_status_text);
	for (i = 0,done = false;!done;)
	{
		while (!(scan = gvar->in.inst->LastScan))
		{}
//				SD_Poll();

		IN_ClearKey(scan);
		switch (scan)
		{
			default:
			case sc_Escape:
				done = true;
			break;
			case sc_Q:
			case sc_Enter:
				//PM_GetPage(i, gvar);
			break;
		}
	}

	modexCopyPageRegion(&gvar->video.page[0], &gvar->video.page[1],
			0,
			PRINTTEXTBOXY,
			0,
			PRINTTEXTBOXY,
			PRINTTEXTBOXW, PRINTTEXTBOXH);
//	memcpy(&gvar->video.page[0].data, &textboxbuff, PRINTTEXTBOXSIZE);
}
