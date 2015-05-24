#include <stdio.h>
#include <dos.h>
#include <string.h>
#include "src/lib/modex16.h"
#include "src/lib/bitmap.h"
#include "src/lib/planar.c"

word far* clockw= (word far*) 0x046C; /* 18.2hz clock */

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

/*
void
DrawPBuf(page_t *page, int x, int y, planar_buf_t *p, byte sprite)
{
	byte plane;
	word px, py;
	word offset;

	// TODO Make this fast.  It's SLOOOOOOW
	for(plane=0; plane < 4; plane++) {
	modexSelectPlane(PLANE(plane+x));
	for(px = plane; px < p->width; px+=4) {
		offset=px;
		for(py=0; py<p->height/2; py++) {
		SELECT_ALL_PLANES();
		if(!sprite || p->plane[offset])
			page->data = p->plane;
		//offset+=p->width;
		//offset++;
		}
	}
	}
}
*/

void main() {
	bitmap_t bmp;
	planar_buf_t *p;
	int i;
	page_t page;
	word start;
	int plane;
	float t1, t2;

	page=modexDefaultPage();

//0000	bmp = bitmapLoadPcx("data/koishi~~.pcx");
//	bmp = bitmapLoadPcx("data/chikyuu.pcx");
	bmp = bitmapLoadPcx("data/koishi^^.pcx");
//	bmp = bitmapLoadPcx("16/PCX_LIB/chikyuu.pcx");
//0000	p = planar_buf_from_bitmap(&bmp);
	modexEnter();

	/* fix up the palette and everything */
//++++	modexPalUpdate(&bmp, 0, 0, 0);

	/* clear and draw one sprite and one bitmap */
	modexClearRegion(&page, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1);

	/* non sprite comparison */
	/*0000start = *clockw;
	for(i=0; i<100 ;i++) {
		oldDrawBmp(VGA, 20, 20, &bmp, 0);
	}

	start = *clockw;
	for(i=0; i<100 ;i++) {
//0000		modexDrawBmp(&page, 20, 20, &bmp);
		modexDrawBmp(&page, 0, 0, &bmp);
	}
	t1 = (*clockw-start) /18.2;

	start = *clockw;
	for(i=0; i<100; i++) {
//0000		modexCopyPageRegion(&page, &page, 20, 20, 128, 20, 64, 64);
		modexCopyPageRegion(&page, &page, 0, 0, 0, 0, 320, 240);
	}
	t2 = (*clockw-start)/18.2;


	start = *clockw;
	for(i=0; i<100 ;i++) {
		oldDrawBmp(VGA, 20, 20, &bmp, 1);
	}


	start = *clockw;
	for(i=0; i<100 ;i++) {
//0000		modexDrawSprite(&page, 20, 20, &bmp);
		modexDrawSprite(&page, 0, 0, &bmp);
	}*/
	//_fmemset(MK_FP(0xA000, 0), (int)p->plane, SCREEN_WIDTH*(SCREEN_HEIGHT*2));
	modexDrawBmp(&page, 0, 0, &bmp);
	while(!kbhit())
	{
		//DrawPBuf(&page, 0, 0, p, 0);
	}
	modexLeave();

	printf("\n%d\n", sizeof(p->plane));
	printf("%d\n", sizeof(bmp));

	/*for(i=0; i<(320*240); i++)
	{
		fprintf(stdout, "%d", bmp.data[i]);
		if(i%320==0) fprintf(stdout, "\n");
	}*/
//0000	printf("CPU to VGA: %f\n", t1);
//0000	printf("VGA to VGA: %f\n", t2);
	return;
}
