#include <stdio.h>
#include "modex16.h"

void main() {
    bitmap_t bmp;
	//bitmap_t bmp2;
	//bitmap_t bmp3;
	//bitmap_t bmp4;
//	int index;
	//int ch=0x0;

    //bmp = modexLoadPcx("ed.pcx");
	//bmp = modexLoadPcx("w.pcx");
	//bmp2 = modexLoadPcx("q.pcx");
	printf("=\n");
	bmp = modexLoadPcx("chikyuu.pcx");
	printf("==\n");
    /*bmp2 = modexLoadPcx("ed2.pcx");
    bmp3 = modexLoadPcx("flower.pcx");
    bmp4 = modexLoadPcx("koishi^^.pcx");*/
	printf("===\n");
	modexEnter();
	printf("====\n");

    /* fix up the palette and everything */
	printf("=====\n");
	modexPalUpdate(bmp.palette);
	printf("======\n");

    /* don't show the drawing page */
//	printf("=======\n");
    modexShowPage(VGA + PAGE_SIZE);
//	printf("========\n");

    /* clear and draw one sprite and one bitmap */
//	printf("=========\n");
    modexClearRegion(VGA, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
//	printf("==========\n");
   	/*modexDrawBmp(VGA, 20, 20, &bmp2, 0);
    modexDrawBmp(VGA, 180, 20, &bmp, 1);
	modexDrawBmp(VGA, 180, 0, &bmp3, 1);
	modexDrawBmp(VGA, 170, 100, &bmp4, 1);*/
	modexDrawBmp(VGA, 0, 0, &bmp, 0);
	//modexDrawBmp(VGA, 100, 40, &bmp2, 1);
	//modexDrawBmp(VGA, 0, 20, &bmp3, 0);
	//modexDrawBmp(VGA, 120, 60, &bmp3, 1);
    modexShowPage(VGA);
//    for(index = 0; index<500; index++) {
	//while(1/*!kbhit()*/){ // conditions of screen saver
	while (!kbhit()){  /* Wait for a keystroke                         */
		modexWaitBorder();
				//ch=getch();
				//if(ch==0x71)break; // 'q'
				//if(ch==0x1b)break; // 'ESC'
	}
	(void) getch();   /* Clear the keyboard buffer                    */
	modexLeave();

    return;
}
