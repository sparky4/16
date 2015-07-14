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
	byte e;
	word chx, chy, colpee;
	textInit();
	modexEnter();
	//modexprint(16, 16, 1, 15, "wwww");
	//getch();
	chx=0;
	chy=0;
	colpee=0;
	for(e=' '; e<='~'; e++)
	{
		if(chx+8>(SCREEN_WIDTH/2)-1)
		{
			chx=0;
			chy+=8;
			modexprint(200, 200, 1, 47, 0, &e);
		}
		modexprint(chx, chy, 1, 0, colpee, &e);
		chx+=9;
		colpee++;
	}
	modexprint(100, 100, 1, 47, 0, "wwww");
	getch();
	modexLeave();
	printf("\n\n%u %c", '');
	printf("\n%c\n", e);
}
