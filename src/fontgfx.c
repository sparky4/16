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
#include <stdio.h>
#include <conio.h>
#include "lib/types.h"
//#include "lib/16text.h"
#include "lib/modex16.h"

#include <hw/cpu/cpu.h>
#include <hw/dos/dos.h>
#include <hw/vga/vga.h>

global_game_variables_t gvar;

void main(int argc, char near *argv[])
{
	//JMOJI
	static byte e;
	//word ri;
	byte pee[6]; // must be large enough for sprintf("%zc",...) and sprintf("%u",(byte value))
	static byte ibmlogo[]="IIIIIII  BBBBBBBBB    MMMMM       MMMMM\n\
IIIIIII  BBBBBBBBBBB  MMMMMM     MMMMMM\n\
  III     BBB    BBB   MMMMMM   MMMMMM\n\
  III     BBBBBBBBB    MMMMMMM MMMMMMM\n\
  III     BBBBBBBBB    MMM MMMMMMM MMM\n\
  III     BBB    BBB   MMM  MMMMM  MMM\n\
IIIIIII  BBBBBBBBBBB  MMMM   MMM   MMMM\n\
IIIIIII  BBBBBBBBB    MMMM    M    MMMM\n\
";
	static byte rose[]="                                              :. ,..\n\
                                            .' :~.':_.,\n\
                                          .'   ::.::'.'\n\
                                         :     ::'  .:\n\
                                       `.:    .:  .:/\n\
                                        `::--.:'.::'\n\
                                          |. _:===-'\n\
                                         / /\n\
                        ,---.---.    __,','\n\
                       (~`.  \   )   )','.,---..\n\
                        `v`\ | ,' .-'.:,'_____   `.\n\
                            )|/.-~.--~~--.   ~~~-. \ \n\
                          _/-'_.-~        ""---.._`.|\n\
                     _.-~~_.-~                    ""'\n\
              _..--~~_.(~~\n\
   __...---~~~_..--~~\n\
,'___...---~~~\n\
";
//	static byte *rosa;
	static word chx, chy, colpee, addr;
	textInit();

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
	VGAmodeX(1, 1, &gvar);
	/* setup camera and screen~ */
	gvar.video.page[0] = modexDefaultPage(&gvar.video.page[0]);
	gvar.video.page[0].width += (16*2);
	gvar.video.page[0].height += (16*2);
	modexShowPage(&gvar.video.page[0]);
	// NTS: We're in Mode-X now. printf() is useless. Do not use printf(). Or INT 10h text printing. Or DOS console output.
	//modexprint(16, 16, 1, 15, "wwww");
	//getch();
	chx=0;
	chy=0;
	colpee=32;
	/* position the cursor to home */
//	vga_moveto(0,0);
//	vga_sync_bios_cursor();
	for(e=0x00; e<=0xFE; e++)
	{
		if(chx+8>(gvar.video.page[0].width/2))
		{
			chx=0;
			chy+=8;
			sprintf(pee,"%u", colpee);
			modexprint(&gvar.video.page[0], 200, 200, 1, 47, 0, &pee);
			//getch();
		}
		sprintf(pee, "%zc", e);
		modexprint(&gvar.video.page[0], chx, chy, 1, 0, colpee, &e);
		chx+=8;
		colpee++;
		if(colpee>=32+24) colpee=32;
	}
	getch();
	modexprint(&gvar.video.page[0], gvar.video.page[0].width - (8*16)/*HACK: The rose ASCII is too wide for 320x240 so offset it to make sure the petals are visible*/, 8, 1, 45, 0, &rose);
	getch();
	//modexprint(100, 100, 1, 47, 0, "wwww");
//	modexprint(0, 0, 1, 0, colpee, &rose);
//++++	modexprint(&gvar.video.page[0], 0, 0, 0, 0, colpee, &ibmlogo);
//	modexprintbig(&gvar.video.page[0], 0, 0, 1, colpee, 0, "IBM");
//	modexprint(0, 0, 1, 0, colpee, ROSE);
//++++	getch();
	VGAmodeX(0, 1, &gvar);
//	rosa=malloc(sizeof(ROSE));
//	(*rosa)=(byte)ROSE;
	printf("\n%s\n", rose);
	//printf("\nh=%d\n", '8');
//	printf("\n%c\n", e);
}
