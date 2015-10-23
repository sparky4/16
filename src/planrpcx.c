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
	int plane;
	int px, py, i;
	px=x;
	py=y;
	// TODO Make this fast.  It's SLOOOOOOW
// 	for(plane=0; plane < 4; plane++) {
// 		i=0;
// 		modexSelectPlane(PLANE(plane+x));
// 		for(px = plane; px < p->width; px+=4) {
// 			offset=px;
// 			for(py=0; py<p->height/2; py++) {
// 				//SELECT_ALL_PLANES();
// 				if(!sprite || p->plane[offset])
// 					page->data = &(p->plane[offset][i++]);
// 				offset+=p->width;
// 				offset++;
// 			}
// 		}
// 	}
	for(plane=0; plane < 4; plane++) {
		i=0;
		modexSelectPlane(PLANE(plane+x));
		for(; y < p->height; y++) {
			//for(px=0; px < p->width; px++) {
				//printf("%02X ", (int) p->plane[plane][i++]);
				_fstrncpy(page->data + (((page->width/4) * (y+page->dy)) + ((x+page->dx) / 4)), &(p->plane[plane][i+=p->pwidth]), p->pwidth);
			//}
		}
		//getch();
		x=px;
		y=py;
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
	word px,py;
	sword baka;
	//char *pee;
	char *bakapeee;

	bakapeee = malloc(64);

 	if(argv[1]) bakapeee = argv[1];
 	else bakapeee = "data/koishi~.pcx";

// 	if(argv[2]) baka = atoi(argv[2]);
// 	else
baka = 1;
//0000	bmp = bitmapLoadPcx("data/koishi~~.pcx");
 	bmp = bitmapLoadPcx(bakapeee);
	//bmp = bitmapLoadPcx("data/koishi~.pcx");
	p = planar_buf_from_bitmap(&bmp);
	VGAmodeX(baka, &gvar);
	gvar.video.page[0]=modexDefaultPage(&gvar.video.page[0]);

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
//i=0;
//plane=0;
//	_fmemset(VGA, (int) p->plane[plane++][i++], p->width*p->height);
	//modexDrawBmp(&gvar.video.page[0], 0, 0, &bmp);
	DrawPBuf(&gvar.video.page[0], 0, 0, p, 0);
	while(!kbhit())
	{
		//if(argv[2]) pee = strcpy(VGA, &(p->plane[plane][24]));
	}//gvar.video.page[0].data
	VGAmodeX(0, &gvar);
	/*printf("\nmain=%Fp\n\n", &i);
	printf("bmp.data=%Fp\n", bmp.data);
	printf("*bmp.data=%Fp\n", *(bmp.data));
	printf("&bmp.data=%Fp\n", &(bmp.data));*/

	/* print out the contents of each plane */
	for(plane=0; plane < 4; plane++) {
		i=0;
		printf("Plane %d\n", plane);
		for(py=0; py < p->height; py++) {
			for(px=0; px < p->pwidth; px++) {
				printf("%02X ", (int) p->plane[plane][i++]);
			}
			printf("\n");
		}
	}
	fprintf(stderr,"\n%d\n", sizeof(p->plane));
	fprintf(stderr,"pw=%d\n", p->width);
	fprintf(stderr,"ph=%d\n", p->height);
	fprintf(stderr,"ppw=%d\n", p->pwidth);
	fprintf(stderr,"%d\n", sizeof(bmp));
	planar_buf_free(p);
// 	fprintf(stderr,"%s\n", *pee);
// 	fprintf(stderr, "CPU to VGA: %f\n", t1);
// 	fprintf(stderr, "VGA to VGA: %f\n", t2);
	fprintf(stderr, "gvar.video.page[0].width: %u\n", gvar.video.page[0].width);
	fprintf(stderr, "gvar.video.page[0].height: %u\n", gvar.video.page[0].height);
	return;
}
