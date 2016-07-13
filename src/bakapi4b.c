/* Project 16 Source Code~
 * Copyright (C) 2012-2016 sparky4 & pngwen & andrius4669
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
 * You screen.heightould have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 * write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "src/bakapi.h"

/*
 * BAKAPEE!
 */
global_game_variables_t gvar;
static bakapee_t bakapee;
word key,d,xpos,ypos,xdir,ydir;
int ch=0x0;

void
main(int argc, char *argvar[])
{
	// main variables values
	d=4; // switch variable
	key=2; // default screensaver number
	xpos=TILEWH*2;
	ypos=TILEWH*2;
	xdir=1;
	ydir=1;

	VGAmodeX(1, &gvar);
	bakapee.xx = rand()&0%gvar.video.page[0].width;
	bakapee.yy = rand()&0%gvar.video.page[0].height;
	bakapee.gq = 0;
	bakapee.sx=0;
	bakapee.sy=0;
	bakapee.bakax=0;
	bakapee.bakay=0;
	bakapee.coor=0;

	/* setup camera and screen~ */
	gvar.video.page[0] = modexDefaultPage(&gvar.video.page[0]);
	gvar.video.page[0].width += (TILEWH*2);
	gvar.video.page[0].height += (TILEWH*2);
	textInit();

	modexShowPage(&gvar.video.page[0]);

// screen savers
	while(d>0)	// on!
	{
		if(!kbhit())
		{ // conditions of screen saver
			ding(&gvar.video.page[0], &bakapee, key);
		}
		else
		{
			VGAmodeX(0, &gvar);
			// user imput switch
			fprintf(stderr, "xx=%d	yy=%d\n", bakapee.xx, bakapee.yy);
			printf("Enter 1, 2, 3, 4, or 6 to run a screensaver, or enter 0 to quit.\n", getch());  // prompt the user
			scanf("%d", &key);
			//if(key==3){xx=yy=0;} // crazy screen saver wwww
			if(key==0){ d=0; }else{
				gvar.video.page[0] = modexDefaultPage(&gvar.video.page[0]);
				gvar.video.page[0].width += (TILEWH*2);
				gvar.video.page[0].height += (TILEWH*2);
				VGAmodeX(1, &gvar);

				modexShowPage(&gvar.video.page[0]);
			}
		}
	}
	printf("bakapi ver. 1.04.13.04\nis made by sparky4ÅiÅÜÉ÷ÅÖÅj feel free to use it ^^\nLicence: GPL v3\n");
}
//pee!
