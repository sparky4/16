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

void main() {
    int i, j;
    word start, end;
    page_t page, page2;
    byte *pal, *pal2=NULL;

    /* load our palette */
    modexLoadPalFile("data/default.pal", &pal2);

    /* save the palette */
    pal  = modexNewPal();
    modexPalSave(pal);
    modexFadeOff(1, pal);
    modexPalBlack();

    modexEnter();
    modexPalBlack();

    /* set up the page, but with 16 pixels on all borders in offscreen mem */
    page=modexDefaultPage();
    page2 = modexNextPage(&page);
    page.width += 32;
    page.height += 32;


    /* fill the page with one color, but with a black border */
    modexShowPage(&page2);
    modexClearRegion(&page, 16, 16, SCREEN_WIDTH, SCREEN_HEIGHT, 128);
    modexClearRegion(&page, 32, 32, SCREEN_WIDTH-32, SCREEN_HEIGHT-32, 42);
    modexClearRegion(&page, 48, 48, SCREEN_WIDTH-64, SCREEN_HEIGHT-64, 128);
    modexShowPage(&page);

    /* fade in */
    modexFadeOn(1, pal2);


    start = *clockw;
    for(i=0; i<5; i++) {
	/* go right */
	for(j=0; j<32; j++) {
	    page.dx++;
	    modexShowPage(&page);
	}
	/* go left */
	for(j=0; j<32; j++) {
	    page.dx--;
	    modexShowPage(&page);
	}
	/* go up */
	for(j=0; j<32; j++) {
	    page.dy++;
	    modexShowPage(&page);
	}

	/* go down */
	for(j=0; j<32; j++) {
	    page.dy--;
	    modexShowPage(&page);
	}
    }

    end = *clockw;

    /* fade back to text mode */
    modexFadeOff(1, pal2);
    modexPalBlack();
    modexLeave();
    modexPalBlack();
    modexFadeOn(1, pal);
}
