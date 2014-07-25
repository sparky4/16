#include <stdio.h>
#include "modex16.h"

word far* clock= (word far*) 0x046C; /* 18.2hz clock */

void
oldDrawBmp(byte far* page, int x, int y, bitmap_t *bmp, byte sprite) {
    byte plane;
    word px, py;
    word offset;

    /* TODO Make this fast.  It's SLOOOOOOW */
    for(plane=0; plane < 4; plane++) {
	modexSelectPlane(PLANE(plane+x));
	for(px = plane; px < bmp->width; px+=4) {
	    offset=px;
	    for(py=0; py<bmp->height; py++) {
		if(!sprite || bmp->data[offset])
		  page[PAGE_OFFSET(x+px, y+py)] = bmp->data[offset];
		offset+=bmp->width;
	    }
	}
    }
}

void main() {
    bitmap_t bmp;
    int i;
    float t1, t2, t3, t4;
    word start;
    page_t page;

    page=modexDefaultPage();

    bmp = modexLoadPcx("ed.pcx");
    modexEnter();

    /* fix up the palette and everything */
    modexPalUpdate(bmp.palette);

    /* clear and draw one sprite and one bitmap */
    modexClearRegion(&page, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1);

    /* non sprite comparison */
    start = *clock;
    for(i=0; i<100 ;i++) {
      oldDrawBmp(VGA, 20, 20, &bmp, 0);
    }
    t1 = (*clock-start) / 18.2;

    start = *clock;
    for(i=0; i<100 ;i++) {
      modexDrawBmp(&page, 20, 20, &bmp);
    }
    t2 = (*clock-start) / 18.2;

    start = *clock;
    for(i=0; i<100 ;i++) {
      oldDrawBmp(VGA, 20, 20, &bmp, 1);
    }
    t3 = (*clock-start) / 18.2;

    start = *clock;
    for(i=0; i<100 ;i++) {
      modexDrawSprite(&page, 20, 20, &bmp);
    }
    t4 = (*clock-start) / 18.2;
    modexLeave();

    printf("Old non-sprite: %f\n", t1);
    printf("New non-sprite: %f\n", t2);
    printf("Old Sprite: %f\n", t3);
    printf("New Sprite: %f\n", t4);

    return;
}
