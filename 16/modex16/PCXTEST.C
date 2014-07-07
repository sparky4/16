#include <stdio.h>
#include "modex16.h"

void main() {
    bitmap_t bmp;
    int index;
	int ch=0x0;

    bmp = modexLoadPcx("ed.pcx");
    modexEnter();

    /* fix up the palette and everything */
    modexPalUpdate(bmp.palette);

    /* don't show the drawing page */
    modexShowPage(VGA + PAGE_SIZE);

    /* clear and draw one sprite and one bitmap */
    modexClearRegion(VGA, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1);
    modexDrawBmp(VGA, 20, 20, &bmp, 1);
    modexDrawBmp(VGA, 100, 20, &bmp, 0);
    modexShowPage(VGA);
//    for(index = 0; index<500; index++) {
while(!kbhit()){ // conditions of screen saver
	modexWaitBorder();
			ch=getch();
			if(ch==0x71)break; // 'q'
			if(ch==0x1b)break; // 'ESC'
    }
    modexLeave();

    return;
}
