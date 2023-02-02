/* Project 16 Source Code~
 * Copyright (C) 2012-2023 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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

#include <stdio.h>
#include "src/lib/16_vl.h"
#include "src/lib/16_vlpal.h"
#include "src/lib/16_in.h"
#include "src/lib/scroll16.h"

#define LGQ 32
#define HGQ 55

static word far* clockw= (word far*) 0x046C; // 18.2hz clock //

void drawboxesmodex(page_t *pg)
{
	modexClearRegion(pg, 0, 0, pg->width, pg->height, 1);
	modexClearRegion(pg, 16, 16, pg->sw, pg->sh, 2);
	modexClearRegion(pg, 32, 32, pg->sw-32, pg->sh-32, 3);
	modexClearRegion(pg, 48, 48, pg->sw-64, pg->sh-64, 2);
}

void copyboxesmodex(page_t *page, boolean pn)
{
	modexCopyPageRegion(&page[pn], &page[!pn], 0, 0, 0, 0, page[pn].width, page[pn].height);
}

void main(int argc, char *argv[])
{
	static global_game_variables_t gvar;

	int i, j;
	word startclk, endclk;
	word k;

	//====word colo=LGQ;

	Startup16(&gvar);

	// save the palette //
	//====modexPalSave(pal);
	//====modexFadeOff(1, pal);
	//====modexPalBlack();

	IN_Default(0,&gvar.player[0],ctrl_Keyboard1, &gvar);
	EN_initPlayer(&gvar.player[0], &gvar.video);

	VL_Startup(&gvar);//VGAmodeX(1, 1, &gvar);
	modexPalBlack();

	// load our palette //
	VL_LoadPalFile("data/16.pal", &gvar.video.palette, &gvar);

	// overscan show //
	//modexPalOverscan(44+1);

	// set up the page, but with 16 pixels on all borders in offscreen mem //
	modexHiganbanaPageSetup(&gvar);
	ZC_MVSetup(&gvar.mv, &gvar.map, &gvar);

	// fill the page with one color, but with a black border //
	drawboxesmodex(&gvar.video.page[0]);
	copyboxesmodex(&gvar.video.page, 1);
	modexClearRegion(&gvar.video.page[2], 0, 0, gvar.video.page[2].sw, gvar.video.page[2].sh, 4);
	modexClearRegion(&gvar.video.page[3], 0, 0, gvar.video.page[3].sw, gvar.video.page[3].sh, 6);


	// fade in //
	//====modexFadeOn(1, pal2);

	i=0,k=0,j=0;
	startclk = *clockw;
	while(!gvar.in.inst->Keyboard[sc_Escape])
	{
		IN_ReadControl(&gvar.player[0], &gvar);
		ZC_panPageManual(&gvar.mv, &gvar.player, 0);
//			if(gvar.in.inst->Keyboard[sc_5)){ modexClearRegion(&gvar.video.page[1],  gvar.video.page[1].sw, 16, 8, 4, 45); }
// 			if(gvar.in.inst->Keyboard[sc_4)){
// 				modexClearRegion(&gvar.video.page[1], 16, 16, gvar.video.page[1].sw, gvar.video.page[1].sh, 128);
// 				modexClearRegion(&gvar.video.page[1], 32, 32, gvar.video.page[1].sw-32, gvar.video.page[1].sh-32, 42);
// 				modexClearRegion(&gvar.video.page[1], 48, 48, gvar.video.page[1].sw-64, gvar.video.page[1].sh-64, 128);
// 			}
		TAIL_PANKEYFUN;
		if(gvar.in.inst->Keyboard[12]) modexClearRegion(&gvar.video.page[0], (gvar.video.page[0].width/2)-4, (gvar.video.page[0].height/2)-16, 24, 32, 15);
		if(gvar.in.inst->Keyboard[13]) modexClearRegion(&gvar.video.page[1], (gvar.video.page[1].width/2)-4, (gvar.video.page[1].height/2)-16, 24, 32, 15);

		if(gvar.in.inst->Keyboard[25]){ modexpdump(gvar.video.sp, &gvar); IN_UserInput(1, &gvar); }//p
		if(gvar.in.inst->Keyboard[sc_I]){ drawboxesmodex(&gvar.video.page[gvar.video.sp]); IN_UserInput(1, &gvar); }//i
		if(gvar.in.inst->Keyboard[sc_O]){ copyboxesmodex(&gvar.video.page, !gvar.video.sp); IN_UserInput(1, &gvar); }//o
		VL_ShowPage(&gvar.video.page[gvar.video.sp], 0, 0);
		//ZC_ShowMV(&gvar.mv, 0, 0);
	}

	endclk = *clockw;

	// fade back to text mode //
	//====modexFadeOff(1, pal2);
	//====modexPalBlack();
	Shutdown16(&gvar);//VGAmodeX(0, 1, &gvar);
	printf("Project 16 vidtest.exe. This is just a test file!\n");
	printf("version %s\n", VERSION);
	VL_PrintmodexmemInfo(&gvar.video);
	printf("tx=%d	", gvar.mv[gvar.video.sp].tx); printf("ty=%d	", gvar.mv[gvar.video.sp].ty); printf("gvar.player.d=%d\n", gvar.player[0].enti.d);
	//====modexPalBlack();
	//====modexFadeOn(1, pal);
}
