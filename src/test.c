/* Project 16 Source Code~
 * Copyright (C) 2012-2016 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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
#include "src/lib/modex16.h"
#include "src/lib/16_in.h"
#include "src/lib/scroll16.h"
#include "src/lib/bakapee.h"

global_game_variables_t gvar;
player_t player[MaxPlayers];
map_view_t mv[4];
pan_t pan;

void main(int argc, char *argv[])
{
	int i, j;
	word startclk, endclk;
	word k;
	byte *pal, *pal2;
	sword bakapee;

	word colo=LGQ;

	//argument
	if(argv[1]) bakapee = atoi(argv[1]);
	else bakapee = 1;

	// DOSLIB: check our environment
	probe_dos();

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
	_DEBUGF("Serial debug output printf test %u %u %u\n",1U,2U,3U);

	/* save the palette */
	pal  = modexNewPal();
	modexPalSave(pal);
	modexFadeOff(1, pal);
	modexPalBlack();

	IN_Startup();
	IN_Default(0,&player,ctrl_Keyboard1);
	IN_initplayer(&player, 0);

	textInit();
	VGAmodeX(bakapee, 1, &gvar);
	modexPalBlack();

	/* load our palette */
	modexLoadPalFile("data/default.pal", &pal2);

	/* overscan show */
	//modexPalOverscan(44+1);

	/* set up the page, but with 16 pixels on all borders in offscreen mem */
	modexHiganbanaPageSetup(&gvar.video);
	for(i=0;i<gvar.video.num_of_pages;i++)
	{
		mv[i].page = &gvar.video.page[i];
		mv[i].video = &gvar.video;
		mv[i].pan	= &pan;
		mv[i].tx	= 0;
		mv[i].ty	= 0;
	}
//  	gvar.video.page[0]=modexDefaultPage(&gvar.video.page[0]);
//  	gvar.video.page[1] = modexNextPage(&gvar.video.page[0]);


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
	modexClearRegion(&gvar.video.page[0], 0, 0, gvar.video.page[0].width, gvar.video.page[0].height, 15);
	modexClearRegion(&gvar.video.page[0], 16, 16, gvar.video.page[0].sw, gvar.video.page[0].sh, 128);
	modexClearRegion(&gvar.video.page[0], 32, 32, gvar.video.page[0].sw-32, gvar.video.page[0].sh-32, 42);
	modexClearRegion(&gvar.video.page[0], 48, 48, gvar.video.page[0].sw-64, gvar.video.page[0].sh-64, 128);
	modexCopyPageRegion(&gvar.video.page[1], &gvar.video.page[0], 0, 0, 0, 0, gvar.video.page[0].width, gvar.video.page[0].height);
	modexClearRegion(&gvar.video.page[2], 0, 0, gvar.video.page[2].sw, gvar.video.page[2].sh, 47);
	modexClearRegion(&gvar.video.page[3], 0, 0, gvar.video.page[3].sw, gvar.video.page[3].sh, 45);

	/* fade in */
	modexFadeOn(1, pal2);

	i=0,k=0,j=0,pan.pn=1;
	startclk = *clockw;
	while(!IN_KeyDown(sc_Escape))
	{
		IN_ReadControl(0,&player);
		panpagemanual(mv, player, 0);

		/*if(i<5){
		switch (k)
		{
			case 0:
				pee:
				// go right
				gvar.video.page[pan.pn].dx++;
				if(i==5){ if(j>=31){ i++; j=0; goto baka; }else j++; }else
				if(j>=32){ k++; j=0; }else j++;
			break;
			case 1:
				// go left
				gvar.video.page[pan.pn].dx--;
				if(j>=32){ k++; j=0; }else j++;
			break;
			case 2:
				// go up
				gvar.video.page[pan.pn].dy++;
				if(j>=32){ k++; j=0; }else j++;
			break;
			case 3:
				// go down
				gvar.video.page[pan.pn].dy--;
				if(j>=32){ k=0; j=0; i++; }else j++;
			break;
			default:

			break;
		}}else{
			if(i==5) goto pee;
			baka:
			i++;
			modexClearRegion(&gvar.video.page[1], 0, gvar.video.page[0].height/2, gvar.video.page[0].width-32, 16, 45);*/
// 			if(IN_KeyDown(6))
// 			{
// 				modexClearRegion(&gvar.video.page[1],  gvar.video.page[1].sw, 16, 8, 4, 45);
// 			}
// 			if(IN_KeyDown(4+1)){
// 				modexClearRegion(&gvar.video.page[1], 16, 16, gvar.video.page[1].sw, gvar.video.page[1].sh, 128);
// 				modexClearRegion(&gvar.video.page[1], 32, 32, gvar.video.page[1].sw-32, gvar.video.page[1].sh-32, 42);
// 				modexClearRegion(&gvar.video.page[1], 48, 48, gvar.video.page[1].sw-64, gvar.video.page[1].sh-64, 128);
// 			}
		//}
		if(IN_KeyDown(1+1)){ pan.pn=0; }
		if(IN_KeyDown(2+1)){ pan.pn=1; }
		if(IN_KeyDown(3+1)){ pan.pn=2; }
		if(IN_KeyDown(4+1)){ pan.pn=3; }
		if(IN_KeyDown(12)) modexClearRegion(&gvar.video.page[0], (gvar.video.page[0].width/2)-4, (gvar.video.page[0].height/2)-16, 24, 32, 15);
		if(IN_KeyDown(13)) modexClearRegion(&gvar.video.page[1], (gvar.video.page[1].width/2)-4, (gvar.video.page[1].height/2)-16, 24, 32, 15);
		if(IN_KeyDown(7)){
			for(i=0;i<3;i++)
			{
				pal2[i] = rand()%64;
				modexPalUpdate1(pal2);
				colo++;
				if(colo>HGQ) colo=LGQ;
			}
//			if(i>PAL_SIZE) i=0;
		}//9
		if(IN_KeyDown(25)){
			modexpdump(&gvar.video.page[pan.pn]);
			IN_UserInput(1,1);
		}//p
		modexShowPage(&gvar.video.page[pan.pn]);
	}

	endclk = *clockw;

	/* fade back to text mode */
	modexFadeOff(1, pal2);
	modexPalBlack();
	VGAmodeX(0, 1, &gvar);
	printf("Project 16 test.exe. This is just a test file!\n");
	printf("version %s\n", VERSION);
	modexprintmeminfo(&gvar.video);
	printf("tx=%d	", mv[pan.pn].tx); printf("ty=%d	", mv[pan.pn].ty); printf("player.d=%d\n", player[0].d);
	IN_Shutdown();
	modexPalBlack();
	modexFadeOn(1, pal);
}
