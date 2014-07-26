#include "modex16.h"
#include <stdio.h>

word far* clock= (word far*) 0x046C; /* 18.2hz clock */

void main() {
	bitmap_t bmp;
	bitmap_t bmp2;
    int i, j;
    word start, end;
    page_t page, page2;
    float elapsed;
    byte *pal, *pal2=NULL;

	/* load pcx file */
	bmp = modexLoadPcx("ed.pcx");
	bmp2 = modexLoadPcx("koishi~.pcx");
	modexPalUpdate(bmp2.palette);

    /* load our palette */
    modexLoadPalFile("gfx.pal", &pal2);

    /* save the palette */
    pal  = modexNewPal();
    modexPalSave(pal);
    modexFadeOff(1, pal);
    modexPalBlack();

    setvideo(1);
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

	modexDrawSprite(&page, 48, 48, &bmp);
    modexDrawSprite(&page, 100, 100, &bmp2);
    //modexDrawBmp(&page, xb, yb, &bmp);

    /* fade in */
    modexFadeOn(1, pal2);

modexPalUpdate(bmp2.palette);

    start = *clock;
    //for(i=0; i<5; i++) {
    while (!kbhit()){  /* Wait for a keystroke                         */
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

    (void) getch();   /* Clear the keyboard buffer                    */
    end = *clock;

    /* fade back to text mode */
    modexFadeOff(1, pal2);
    modexPalBlack();
    setvideo(0);
    modexPalBlack();
    modexFadeOn(1, pal);
}
