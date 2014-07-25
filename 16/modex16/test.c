#include "modex16.h"
#include <stdio.h>

word far* clock= (word far*) 0x046C; /* 18.2hz clock */

void main() {
    int i, j;
    word start, end;
    page_t page, page2;
    float elapsed;
    byte *pal, *pal2=NULL;

    /* load our palette */
    modexLoadPalFile("gfx.pal", &pal2);

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


    start = *clock;
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

    end = *clock;

    /* fade back to text mode */
    modexFadeOff(1, pal2);
    modexPalBlack();
    modexLeave();
    modexPalBlack();
    modexFadeOn(1, pal);
}
