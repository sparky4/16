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

global_game_variables_t gvar;

void main() {
	int i, j;
	word start, end;
	byte *pal, *pal2=NULL;

	/* load our palette */
	modexLoadPalFile("data/default.pal", &pal2);

	/* save the palette */
	pal  = modexNewPal();
	modexPalSave(pal);
	modexFadeOff(1, pal);
	modexPalBlack();

	VGAmodeX(1, &gvar);
	modexPalBlack();

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


	start = *clockw;
	for(i=0; i<5; i++) {
	/* go right */
	for(j=0; j<32; j++) {
		gvar.video.page[0].dx++;
		modexShowPage(&gvar.video.page[0]);
	}
	/* go left */
	for(j=0; j<32; j++) {
		gvar.video.page[0].dx--;
		modexShowPage(&gvar.video.page[0]);
	}
	/* go up */
	for(j=0; j<32; j++) {
		gvar.video.page[0].dy++;
		modexShowPage(&gvar.video.page[0]);
	}

	/* go down */
	for(j=0; j<32; j++) {
		gvar.video.page[0].dy--;
		modexShowPage(&gvar.video.page[0]);
	}
	}

	end = *clockw;

	/* fade back to text mode */
	modexFadeOff(1, pal2);
	modexPalBlack();
	VGAmodeX(0, &gvar);
	modexPalBlack();
	modexFadeOn(1, pal);
}
