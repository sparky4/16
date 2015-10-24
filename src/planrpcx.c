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
bitmap_t bmp;
planar_buf_t *p;

void main(int argc, char *argv[])
{
	int i;
	word start;
	int plane;
	float t1, t2;
	int x,y;
	word px,py;
	sword baka;
	char *bakapeee;

	byte l[1024];
	word j,chw,xp,col,bgcol;


	bakapeee = malloc(64);

	if(argv[1]) bakapeee = argv[1];
	else bakapeee = "data/koishi~.pcx";

//	if(argv[2]) baka = atoi(argv[2]);
//	else
baka = 1;

	bmp = bitmapLoadPcx(bakapeee);
	p = planar_buf_from_bitmap(&bmp);
	VGAmodeX(baka, &gvar);
	gvar.video.page[0]=modexDefaultPage(&gvar.video.page[0]);

	/* fix up the palette and everything */
	modexPalUpdate1(bmp.palette);

	/* clear and draw one sprite and one bitmap */
	modexClearRegion(&gvar.video.page[0], 0, 0, gvar.video.page[0].sw, gvar.video.page[0].sh, 0);

	/* update display~*/
	gvar.video.page[0].dx+=32;
	gvar.video.page[0].dy+=32;
	modexShowPage(&gvar.video.page[0]);

	/* non sprite comparison */
	start = *clockw;
// 		oldDrawBmp(VGA, 20, 20, &bmp, 0);
	for(i=0; i<100 ;i++) {
		modexDrawBmp(&gvar.video.page[0], 32, 32, &bmp);
	}
	t1 = (*clockw-start) /18.2;
// 	start = *clockw;
// 		modexCopyPageRegion(&gvar.video.page[0], &gvar.video.page[0], 0, 0, 0, 0, 320, 240);
// 	t2 = (*clockw-start)/18.2;
	start = *clockw;
	for(i=0; i<100 ;i++) {
		DrawPBuf(&gvar.video.page[0], 0, 0, p, 0);
	}
	t2 = (*clockw-start) /18.2;
	while(!kbhit())
	{
	}
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
	col=0x0d, bgcol=0;
	for(i=0; i<8; i++)
	{
		/*modexSelectPlane(PLANE(x));
		j=1<<8;
		*bakapee=(l[i] & j ? col:bgcol);
		_fmemcpy(page->data + (((page->width/4) * (y+page->dy+i)) + ((x+page->dx+chw) / 4)), bakapee, 8);*/
		j=4<<8;
		fprintf(stderr, "j<<=%u\n", j);
		xp=0;
		while(j)
		{
			//modexputPixel(page, x+xp+chw, y+i, l[i] & j ? col:bgcol);
			//fprintf(stderr, "%u", l[i] & j ? col:bgcol);
			xp++;
			j>>=4;
			fprintf(stderr, "	j>>=%u\n", j);
		}
		//fprintf(stderr, "\n");
	}
	chw += xp;
	fprintf(stderr,"Project 16 planrpcx.exe. This is just a test file!\n");
	fprintf(stderr,"version %s\n", VERSION);
	fprintf(stderr,"%d\n", sizeof(p->plane));
	fprintf(stderr,"pw=%d\n", p->width);
	fprintf(stderr,"ph=%d\n", p->height);
	fprintf(stderr,"ppw=%d\n", p->pwidth);
	fprintf(stderr,"%d\n", sizeof(bmp));
	fprintf(stderr,"%dx%d\n", gvar.video.page[0].sw-(p->width), gvar.video.page[0].sh-(p->height));
	planar_buf_free(p);
	free(bakapeee);
	fprintf(stderr, "modexDrawBmp:	%f\n", t1);
	fprintf(stderr, "DrawPBuf:	%f\n", t2);
	fprintf(stderr, "gvar.video.page[0].width: %u\n", gvar.video.page[0].width);
	fprintf(stderr, "gvar.video.page[0].height: %u\n", gvar.video.page[0].height);
	return;
}
