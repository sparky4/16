#include <stdio.h>
#include "modex16.h"

void main() {
    bitmap_t bmp;
	bitmap_t bmp2;
	bitmap_t bmp3;
    int index;
	int ch=0x0;

    bmp = modexLoadPcx("ed.pcx");
    bmp2 = modexLoadPcx("ed2.pcx");
    bmp3 = modexLoadPcx("koishi^^.pcx");
    modexEnter();

    /* fix up the palette and everything */
    //modexPalUpdate(bmp.palette);
    modexPalUpdate(bmp2.palette);

    /* don't show the drawing page */
    modexShowPage(VGA + PAGE_SIZE);

    /* clear and draw one sprite and one bitmap */
    modexClearRegion(VGA, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
   	modexDrawBmp(VGA, 20, 20, &bmp2, 0);
    modexDrawBmp(VGA, 180, 20, &bmp, 1);
    modexDrawBmp(VGA, 170, 100, &bmp3, 0);
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
