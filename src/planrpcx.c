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

void
DrawPBuf(page_t *page, int x, int y, planar_buf_t *p, byte sprite)
{
	word plane;
	word px, py;
	word offset;
	word i;

	// TODO Make this fast.  It's SLOOOOOOW
	for(plane=0; plane < 4; plane++) {
		i=0;
		modexSelectPlane(PLANE(plane+x));
			for(px = plane; px < p->width; px+=4) {
				offset=px;
				for(py=0; py<p->height/2; py++) {
					//SELECT_ALL_PLANES();
					if(!sprite || p->plane[offset])
						page->data = &(p->plane[offset][i++]);
				offset+=p->width;
				offset++;
		}
	}
	}
}


void main(int argc, char *argv[])
{
	bitmap_t bmp;
	planar_buf_t *p;
	int i;
	//word start;
	int plane;
	//float t1, t2;
	int x,y;
	sword bakapee;

	if(argv[1]) bakapee = atoi(argv[1]);
	else bakapee = 1;
//0000	bmp = bitmapLoadPcx("data/koishi~~.pcx");
//	bmp = bitmapLoadPcx("data/chikyuu.pcx");
	bmp = bitmapLoadPcx("data/koishi^^.pcx");
	p = planar_buf_from_bitmap(&bmp);
	VGAmodeX(bakapee, &gvar);
	gvar.video.page[0]=modexDefaultPage(&gvar.video.page[0]);
	gvar.video.page[0].sw+=32;
	gvar.video.page[0].sh+=32;

	/* fix up the palette and everything */
	modexPalUpdate1(bmp.palette);

	/* clear and draw one sprite and one bitmap */
	modexClearRegion(&gvar.video.page[0], 0, 0, gvar.video.page[0].sw, gvar.video.page[0].sh, 1);

	/* update display~*/
	gvar.video.page[0].dx+=32;
	gvar.video.page[0].dy+=32;
	modexShowPage(&gvar.video.page[0]);

	/* non sprite comparison */
// 	start = *clockw;
// 		oldDrawBmp(VGA, 20, 20, &bmp, 0);

// 	start = *clockw;
//0000		modexDrawBmp(&gvar.video.page[0], 20, 20, &bmp);
// 		modexDrawBmp(&gvar.video.page[0], 160, 120, &bmp);
// 	t1 = (*clockw-start) /18.2;

// 	start = *clockw;
//0000		modexCopyPageRegion(&gvar.video.page[0], &gvar.video.page[0], 20, 20, 128, 20, 64, 64);
// 		modexCopyPageRegion(&gvar.video.page[0], &gvar.video.page[0], 0, 0, 0, 0, 320, 240);
// 	t2 = (*clockw-start)/18.2;


// 	start = *clockw;
// 		oldDrawBmp(VGA, 20, 20, &bmp, 1);


// 	start = *clockw;
//0000		modexDrawSprite(&gvar.video.page[0], 20, 20, &bmp);
// 		modexDrawSprite(&gvar.video.page[0], 160, 120, &bmp);
	//_fmemset(MK_FP(0xA000, 0), (int)p->plane, gvar.video.page[0].sw*(gvar.video.page[0].sh*2));
	//modexDrawBmp(&gvar.video.page[0], 0, 0, &bmp);
	while(!kbhit())
	{
		DrawPBuf(&gvar.video.page[0], 0, 0, p, 0);
	}
	VGAmodeX(0, &gvar);
	/*printf("\nmain=%Fp\n\n", &i);
	printf("bmp.data=%Fp\n", bmp.data);
	printf("*bmp.data=%Fp\n", *(bmp.data));
	printf("&bmp.data=%Fp\n", &(bmp.data));*/

	printf("\n%d\n", sizeof(p->plane));
	printf("%d\n", sizeof(bmp));

	/* print out the contents of each plane */
	for(plane=0; plane < 4; plane++) {
		i=0;
		printf("Plane %d\n", plane);
		for(y=0; y < p->height; y++) {
			for(x=0; x < p->pwidth; x++) {
				printf("%02X ", (int) p->plane[plane][i++]);
			}
			printf("\n");
		}
	}
	fprintf(stderr, "\n");
// 	fprintf(stderr, "CPU to VGA: %f\n", t1);
// 	fprintf(stderr, "VGA to VGA: %f\n", t2);
	fprintf(stderr, "gvar.video.page[0].width: %u\n", gvar.video.page[0].width);
	fprintf(stderr, "gvar.video.page[0].height: %u\n", gvar.video.page[0].height);
	return;
}
