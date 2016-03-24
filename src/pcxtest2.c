/* Project 16 Source Code~
 * Copyright (C) 2012-2016 sparky4 & pngwen & andrius4669 & joncampbell123
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
	word start;
	int plane;
	float t1, t2;

//0000	bmp = bitmapLoadPcx("data/koishi~~.pcx");
//	bmp = bitmapLoadPcx("data/chikyuu.pcx");
	bmp = bitmapLoadPcx("data/koishi^^.pcx");
//	bmp = bitmapLoadPcx("16/PCX_LIB/chikyuu.pcx");
	p = planar_buf_from_bitmap(&bmp);
	VGAmodeX(1, &gvar);
	gvar.video.page[0]=modexDefaultPage(&gvar.video.page[0]);
	gvar.video.page[0].sw+=32;
	gvar.video.page[0].sh+=32;

	/* fix up the palette and everything */
	modexPalUpdate1(bmp.palette);

	/* clear and draw one sprite and one bitmap */
	modexClearRegion(&gvar.video.page[0], 0, 0, gvar.video.page[0].sw, gvar.video.page[0].sh, 1);

	/* update display~*/
	gvar.video.page[0].dx+=16;
	gvar.video.page[0].dy+=16;
	modexShowPage(&gvar.video.page[0]);

	/* non sprite comparison */
	start = *clockw;
	//for(i=0; i<100 ;i++) {
		oldDrawBmp(VGA, 20, 20, &bmp, 0);
	//}

	start = *clockw;
	//for(i=0; i<100 ;i++) {
//0000		modexDrawBmp(&gvar.video.page[0], 20, 20, &bmp);
		modexDrawBmp(&gvar.video.page[0], 160, 120, &bmp);
	//}
	t1 = (*clockw-start) /18.2;

	start = *clockw;
	//for(i=0; i<100; i++) {
//0000		modexCopyPageRegion(&gvar.video.page[0], &gvar.video.page[0], 20, 20, 128, 20, 64, 64);
		modexCopyPageRegion(&gvar.video.page[0], &gvar.video.page[0], 0, 0, 0, 0, 320, 240);
	//}
	t2 = (*clockw-start)/18.2;


	start = *clockw;
	//for(i=0; i<100 ;i++) {
		oldDrawBmp(VGA, 20, 20, &bmp, 1);
	//}


	start = *clockw;
	//for(i=0; i<100 ;i++) {
//0000		modexDrawSprite(&gvar.video.page[0], 20, 20, &bmp);
		modexDrawSprite(&gvar.video.page[0], 160, 120, &bmp);
	//}
	//_fmemset(MK_FP(0xA000, 0), (int)p->plane, gvar.video.page[0].sw*(gvar.video.page[0].sh*2));
	//modexDrawBmp(&gvar.video.page[0], 0, 0, &bmp);
	while(!kbhit())
	{
		//DrawPBuf(&gvar.video.page[0], 0, 0, p, 0);
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
	printf("gvar.video.page[0].width: %u\n", gvar.video.page[0].width);
	printf("gvar.video.page[0].height: %u\n", gvar.video.page[0].height);
	return;
}
