/* Project 16 Source Code~
 * Copyright (C) 2012-2016 sparky4 & pngwen & andrius4669 & joncampbell123
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

#include "src/16.h"

global_game_variables_t gvar;
engi_stat_t engi_stat;
const char *cpus;
byte *dpal, *gpal;
player_t player[MaxPlayers];
//page_t screen;

void
main(int argc, char *argv[])
{
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

	//screen = modexDefaultPage();
	engi_stat = ENGI_RUN;
	textInit();

	/* save the palette */
	dpal = modexNewPal();
	modexPalSave(dpal);
//	modexFadeOff(4, dpal);
	//printf("pal load\n");
	//gpal = modexNewPal();
	/*modexPalSave(gpal);
	modexSavePalFile("data/g.pal", gpal);*/
//0000	printf("wwww loop wwww\n");
	VGAmodeX(1, 1, &gvar);
//	modexPalBlack();	//so player will not see loadings~
	IN_Startup();
	IN_Default(0,&player,ctrl_Joystick);
	//modexprint(&screen, 32, 32, 1, 2, 0, "a", 1);
	start_timer(&gvar);
	while(ENGI_EXIT != engi_stat)
	{
		IN_ReadControl(0,&player);
		if(IN_KeyDown(sc_Escape)) engi_stat = ENGI_EXIT;
		shinku(&gvar);
	}
	switch(detectcpu())
	{
		case 0: cpus = "8086/8088 or 186/88"; break;
		case 1: cpus = "286"; break;
		case 2: cpus = "386 or newer"; break;
		default: cpus = "internal error"; break;
	}
	VGAmodeX(0, 1, &gvar);
	printf("Project 16 16.exe. This is supposed to be the actual finished game executable!\n");
	printf("version %s\n", VERSION);
	printf("detected CPU type: %s\n", cpus);
	IN_Shutdown();
//	modexFadeOn(4, dpal);
}
