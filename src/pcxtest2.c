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
#include <dos.h>
#include <string.h>
#include "src/lib/modex16.h"
#include "src/lib/bitmap.h"
#include "src/lib/planar.h"

global_game_variables_t gvar;

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
	page.width+=32;
	page.height+=32;
//0000	bmp = bitmapLoadPcx("data/koishi~~.pcx");
//	bmp = bitmapLoadPcx("data/chikyuu.pcx");
	bmp = bitmapLoadPcx("data/koishi^^.pcx");
//	bmp = bitmapLoadPcx("16/PCX_LIB/chikyuu.pcx");
	p = planar_buf_from_bitmap(&bmp);
	VGAmodeX(1, &gvar);

	/* fix up the palette and everything */
	modexPalUpdate1(bmp.palette);

	/* clear and draw one sprite and one bitmap */
	modexClearRegion(&page, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1);

	/* update display~*/
	page.dx+=16;
	page.dy+=16;
	modexShowPage(&page);

	/* non sprite comparison */
	start = *clockw;
	//for(i=0; i<100 ;i++) {
		oldDrawBmp(VGA, 20, 20, &bmp, 0);
	//}

	start = *clockw;
	//for(i=0; i<100 ;i++) {
//0000		modexDrawBmp(&page, 20, 20, &bmp);
		modexDrawBmp(&page, 160, 120, &bmp);
	//}
	t1 = (*clockw-start) /18.2;

	start = *clockw;
	//for(i=0; i<100; i++) {
//0000		modexCopyPageRegion(&page, &page, 20, 20, 128, 20, 64, 64);
		modexCopyPageRegion(&page, &page, 0, 0, 0, 0, 320, 240);
	//}
	t2 = (*clockw-start)/18.2;


	start = *clockw;
	//for(i=0; i<100 ;i++) {
		oldDrawBmp(VGA, 20, 20, &bmp, 1);
	//}


	start = *clockw;
	//for(i=0; i<100 ;i++) {
//0000		modexDrawSprite(&page, 20, 20, &bmp);
		modexDrawSprite(&page, 160, 120, &bmp);
	//}
	//_fmemset(MK_FP(0xA000, 0), (int)p->plane, SCREEN_WIDTH*(SCREEN_HEIGHT*2));
	//modexDrawBmp(&page, 0, 0, &bmp);
	while(!kbhit())
	{
		//DrawPBuf(&page, 0, 0, p, 0);
	}
	VGAmodeX(0, &gvar);
	/*printf("\nmain=%Fp\n\n", &i);
	printf("bmp.data=%Fp\n", bmp.data);
	printf("*bmp.data=%Fp\n", *(bmp.data));
	printf("&bmp.data=%Fp\n", &(bmp.data));

	printf("\n%d\n", sizeof(p->plane));
	printf("%d\n", sizeof(bmp));*/

	/*for(i=0; i<(320*240); i++)
	{
		fprintf(stdout, "%d", bmp.data[i]);
		if(i%bmp.width==0) fprintf(stdout, "\n");
	}*/
	printf("CPU to VGA: %f\n", t1);
	printf("VGA to VGA: %f\n", t2);
	printf("page.width: %u\n", page.width);
	printf("page.height: %u\n", page.height);
	return;
}
