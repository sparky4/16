/* Project 16 Source Code~
 * Copyright (C) 2012-2017 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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

static word far* clockw= (word far*) 0x046C; /* 18.2hz clock */

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
	static word paloffset=0;
	static global_game_variables_t gvar;

//	map_view_t mv[4];
	map_t map;

	int i, j;
	word startclk, endclk;
	word k;

	imgtestpal_t bmp1, bmp2;


	bmp1.width=bmp2.width=	40;
	bmp1.width=bmp2.height=	30;

	//====word colo=LGQ;

	// DOSLIB: check our environment
	/*probe_dos();

	// DOSLIB: what CPU are we using?
	// NTS: I can see from the makefile Sparky4 intends this to run on 8088 by the -0 switch in CFLAGS.
	//      So this code by itself shouldn't care too much what CPU it's running on. Except that other
	//      parts of this project (DOSLIB itself) rely on CPU detection to know what is appropriate for
	//      the CPU to carry out tasks. --J.C.
	cpu_probe();

	// DOSLIB: check for VGA
	if (!probe_vga()) {
		printf("VGA probe failed\n");
		return;
	}
	// hardware must be VGA or higher!
	if (!(vga_state.vga_flags & VGA_IS_VGA)) {
		printf("This program requires VGA or higher graphics hardware\n");
		return;
	}

	if (_DEBUG_INIT() == 0) {
#ifdef DEBUGSERIAL
		printf("WARNING: Failed to initialize DEBUG output\n");
#endif
	}
	_DEBUG("Serial debug output started\n"); // NTS: All serial output must end messages with newline, or DOSBox-X will not emit text to log
	_DEBUGF("Serial debug output printf test %u %u %u\n",1U,2U,3U);*/
	Startup16(&gvar);

	/* save the palette */
	//====modexPalSave(pal);
	//====modexFadeOff(1, pal);
	//====modexPalBlack();

	//IN_Startup();
	IN_Default(0,&gvar.player[0],ctrl_Keyboard1);
	EN_initPlayer(&gvar.player[0], &gvar.video);

	VGAmodeX(1, 1, &gvar);
	modexPalBlack();

	/* load our palette */
	VL_LoadPalFile("data/16.pal", &gvar.video.palette);
	bmp1.offset=(paloffset/3);
	VL_palette(&bmp1, &gvar.video.palette, &paloffset, 0, 0, &gvar);

	//VL_LoadPalFilewithoffset("data/default.pal", &palette, paloffset);
	modexLoadPalFile("data/16t.pal", &gvar.video.dpal);
	bmp2.offset=(paloffset/3);
	VL_palette(&bmp2, &gvar.video.dpal, &paloffset, 0, 0, &gvar);
	//====modexLoadPalFile("data/default.pal", &pal2);

	/* overscan show */
	//modexPalOverscan(44+1);

	/* set up the page, but with 16 pixels on all borders in offscreen mem */
	modexHiganbanaPageSetup(&gvar.video);
	ZC_MVSetup(&MVVAR, &map, &gvar);

	/* fill the page with one color, but with a black border */
	/*modexClearRegion(&gvar.video.page[1], 0, 0, gvar.video.page[1].width, gvar.video.page[1].height, 15);
	modexClearRegion(&gvar.video.page[1], 16, 16, gvar.video.page[1].sw, gvar.video.page[1].sh, 128);
	modexClearRegion(&gvar.video.page[1], 32, 32, gvar.video.page[1].sw-32, gvar.video.page[1].sh-32, 42);
	modexClearRegion(&gvar.video.page[1], 48, 48, gvar.video.page[1].sw-64, gvar.video.page[1].sh-64, 128);
	modexShowPage(&gvar.video.page[1]);
	modexClearRegion(&gvar.video.page[0], 0, 0, gvar.video.page[0].width, gvar.video.page[0].height, 15);
	modexClearRegion(&gvar.video.page[0], 16, 16, gvar.video.page[0].sw, gvar.video.page[0].sh, 128);
	modexClearRegion(&gvar.video.page[0], 32, 32, gvar.video.page[0].sw-32, gvar.video.page[0].sh-32, 42);
	modexClearRegion(&gvar.video.page[0], 48, 48, gvar.video.page[0].sw-64, gvar.video.page[0].sh-64, 128);
	modexShowPage(&gvar.video.page[0]);*/

	drawboxesmodex(&gvar.video.page[0]);
	copyboxesmodex(&gvar.video.page, 1);
	modexClearRegion(&gvar.video.page[2], 0, 0, gvar.video.page[2].sw, gvar.video.page[2].sh, 4);
	modexClearRegion(&gvar.video.page[3], 0, 0, gvar.video.page[3].sw, gvar.video.page[3].sh, 6);


	/* fade in */
	//====modexFadeOn(1, pal2);

	i=0,k=0,j=0;
	startclk = *clockw;
	while(!IN_KeyDown(sc_Escape))
	{
		IN_ReadControl(&gvar.player[0]);
		ZC_panPageManual(&MVVAR, &gvar.player, 0);
//			if(IN_KeyDown(sc_5)){ modexClearRegion(&gvar.video.page[1],  gvar.video.page[1].sw, 16, 8, 4, 45); }
// 			if(IN_KeyDown(sc_4)){
// 				modexClearRegion(&gvar.video.page[1], 16, 16, gvar.video.page[1].sw, gvar.video.page[1].sh, 128);
// 				modexClearRegion(&gvar.video.page[1], 32, 32, gvar.video.page[1].sw-32, gvar.video.page[1].sh-32, 42);
// 				modexClearRegion(&gvar.video.page[1], 48, 48, gvar.video.page[1].sw-64, gvar.video.page[1].sh-64, 128);
// 			}
		TAIL_PANKEYFUN;
		if(IN_KeyDown(12)) modexClearRegion(&gvar.video.page[0], (gvar.video.page[0].width/2)-4, (gvar.video.page[0].height/2)-16, 24, 32, 15);
		if(IN_KeyDown(13)) modexClearRegion(&gvar.video.page[1], (gvar.video.page[1].width/2)-4, (gvar.video.page[1].height/2)-16, 24, 32, 15);
/*====		if(IN_KeyDown(7)){
			for(i=0;i<3;i++)
			{
				pal2[i] = rand()%64;
				modexPalUpdate(pal2);
				colo++;
				if(colo>HGQ) colo=LGQ;
			}
//			if(i>PAL_SIZE) i=0;
		}//9*/
		if(IN_KeyDown(25)){ modexpdump(&gvar.video.page[gvar.video.sp]); IN_UserInput(1); }//p
		if(IN_KeyDown(sc_I)){ drawboxesmodex(&gvar.video.page[gvar.video.sp]); IN_UserInput(1); }//i
		if(IN_KeyDown(sc_O)){ copyboxesmodex(&gvar.video.page, !gvar.video.sp); IN_UserInput(1); }//o
		//VL_ShowPage(&gvar.video.page[gvar.video.sp], 0, 0);
		ZC_ShowMV(&MVVAR, 0, 0);
	}

	endclk = *clockw;

	/* fade back to text mode */
	//====modexFadeOff(1, pal2);
	//====modexPalBlack();
	VGAmodeX(0, 1, &gvar);
	Shutdown16(&gvar);
	printf("Project 16 test.exe. This is just a test file!\n");
	printf("version %s\n", VERSION);
	VL_PrintmodexmemInfo(&gvar.video);
	printf("tx=%d	", MVVAR[gvar.video.sp].tx); printf("ty=%d	", MVVAR[gvar.video.sp].ty); printf("gvar.player.d=%d\n", gvar.player[0].enti.d);
	printf("\n====\n");
	printf("0	paloffset=	%d\n", paloffset/3);
	printf("====\n\n");
	//IN_Shutdown();
	//====modexPalBlack();
	//====modexFadeOn(1, pal);
}
