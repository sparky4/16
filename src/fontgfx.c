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

global_game_variables_t gvar;

void main(int argc, char near *argv[])
{
	//JMOJI
	static byte e;
	//word ri;
	byte pee[2];
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
	static word chx, chy, colpee;
	textInit();
	VGAmodeX(1, &gvar);
	/* setup camera and screen~ */
	gvar.video.page[0] = modexDefaultPage(&gvar.video.page[0]);
	//gvar.video.page[0].width += (16*2);
	//gvar.video.page[0].height += (16*2);
//++++	modexShowPage(&gvar.video.page[0]);
	//modexprint(16, 16, 1, 15, "wwww");
	//getch();
	chx=0;
	chy=0;
	colpee=32;
	for(e=0x00; e<=0xFE; e++)
	{
		if(chx+8>(gvar.video.page[0].width/2))
		{
			chx=0;
			chy+=8;
			sprintf(pee,"%u", colpee);
			modexprint(&gvar.video.page[0], 200, 200, 1, 47, 0, &pee, 1);
			//getch();
		}
		modexprint(&gvar.video.page[0], chx, chy, 1, 0, colpee, &e, 1);
		chx+=9;
		colpee++;
		if(colpee>=32+24) colpee=32;
	}
	//modexprint(100, 100, 1, 47, 0, "wwww");
	getch();
//	modexprint(0, 0, 1, 0, colpee, &rose);
//++++	modexprint(&gvar.video.page[0], 0, 0, 0, 0, colpee, &ibmlogo);
//	modexprintbig(&gvar.video.page[0], 0, 0, 1, colpee, 0, "IBM");
//	modexprint(0, 0, 1, 0, colpee, ROSE);
	getch();
	VGAmodeX(0, &gvar);
//	rosa=malloc(sizeof(ROSE));
//	(*rosa)=(byte)ROSE;
	printf("\n%s\n", rose);
	//printf("\nh=%d\n", '8');
//	printf("\n%c\n", e);
}
