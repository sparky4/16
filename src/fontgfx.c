/* Project 16 Source Code~
 * Copyright (C) 2012-2015 sparky4 & pngwen & andrius4669
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

void main(int argc, char near *argv[])
{
	//JMOJI
	static byte e;
	byte pee[2];
	static word chx, chy, colpee;
	textInit();
	modexEnter();
	//modexprint(16, 16, 1, 15, "wwww");
	//getch();
	chx=0;
	chy=0;
	colpee=32;
	for(e=0x00; e<=0xFE; e++)
	{
		if(chx+8>(SCREEN_WIDTH/2)-16)
		{
			chx=0;
			chy+=8;
			sprintf(pee,"%u", colpee);
			modexprint(200, 200, 1, 47, 0, &pee);
			//getch();
		}
		modexprint(chx, chy, 1, 0, colpee, &e);
		chx+=9;
		colpee++;
		if(colpee>=32+24) colpee=32;
	}
	//modexprint(100, 100, 1, 47, 0, "wwww");
	getch();
	modexLeave();
	//printf("\nh=%d\n", '8');
	//printf("\n%c\n", e);
}
