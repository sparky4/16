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
#include "src/lib/modex16.h"
#include "src/lib/16_in.h"

global_game_variables_t gvar;
player_t player[MaxPlayers];

void main(int argc, char *argv[])
{
	int i, j, k;
	word start, end;
	byte *pal, *pal2;
	sword bakapee;

	//argument
	if(argv[1]) bakapee = atoi(argv[1]);
	else bakapee = 1;

	/* load our palette */
	modexLoadPalFile("data/default.pal", &pal2);

	/* save the palette */
	pal  = modexNewPal();
	modexPalSave(pal);
	modexFadeOff(1, pal);
	modexPalBlack();

	VGAmodeX(bakapee, &gvar);
	modexPalBlack();

	IN_Startup();
	IN_Default(0,&player,ctrl_Joystick);

	/* set up the page, but with 16 pixels on all borders in offscreen mem */
	gvar.video.page[0]=modexDefaultPage(&gvar.video.page[0]);
	gvar.video.page[1] = modexNextPage(&gvar.video.page[0]);
	gvar.video.page[0].width += 32;
	gvar.video.page[0].height += 32;


	/* fill the page with one color, but with a black border */
	modexShowPage(&gvar.video.page[1]);
	modexClearRegion(&gvar.video.page[0], 16, 16, gvar.video.page[0].sw, gvar.video.page[0].sh, 128);
	modexClearRegion(&gvar.video.page[0], 32, 32, gvar.video.page[0].sw-32, gvar.video.page[0].sh-32, 42);
	modexClearRegion(&gvar.video.page[0], 48, 48, gvar.video.page[0].sw-64, gvar.video.page[0].sh-64, 128);
	modexShowPage(&gvar.video.page[0]);

	/* fade in */
	modexFadeOn(1, pal2);

	i=0,k=0,j=0;
	start = *clockw;
	while(!IN_KeyDown(sc_Escape) && i<5)
	{
		IN_ReadControl(0,&player);
		switch (k)
		{
			case 0:
				/* go right */
				gvar.video.page[0].dx++;
				modexShowPage(&gvar.video.page[0]);
				if(j>=32){ k++; j=0; }else j++;
			break;
			case 1:
				/* go left */
				gvar.video.page[0].dx--;
				modexShowPage(&gvar.video.page[0]);
				if(j>=32){ k++; j=0; }else j++;
			break;
			case 2:
				/* go up */
				gvar.video.page[0].dy++;
				modexShowPage(&gvar.video.page[0]);
				if(j>=32){ k++; j=0; }else j++;
			break;
			case 3:
				/* go down */
				gvar.video.page[0].dy--;
				modexShowPage(&gvar.video.page[0]);
				if(j>=32){ k=0; j=0; i++; }else j++;
			break;
			default:

			break;
		}
	}

	end = *clockw;

	/* fade back to text mode */
	modexFadeOff(1, pal2);
	modexPalBlack();
	VGAmodeX(0, &gvar);
	IN_Shutdown();
	modexPalBlack();
	modexFadeOn(1, pal);
}
