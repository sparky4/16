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
#include <conio.h>
#include "src/lib/modex16.h"
#include "src/lib/planar.h"
//#include "src/lib/bitmap.h"

void main() {
	FILE *file;
    bitmap_t bmp;
    planar_buf_t *p;
    word size;
    int i;
    int plane;
    int x,y;
    //byte color;

    /* get the size we want */
    /*printf("Width: ");
    scanf("%d", &bmp.width);
    printf("Height: ");
    scanf("%d", &bmp.height);
    printf("Color: ");
    scanf("%x", &color);*/

    /* allocate the bmp and fill it with 42 */
    /*size = bmp.width * bmp.height;
    bmp.data = malloc(size);
    for(i=0; i<size; i++) {
	bmp.data[i] = color;
    }*/

	bmp = bitmapLoadPcx("data/koishi^^.pcx");

    /* create the planar buffer */
    p = planar_buf_from_bitmap(&bmp);

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
		/*__asm
		{
			mov ah,31h
			int 21h
		}*/

//	file = fopen("ptmp0.pcx", "wb");

    /* write the data to the file */
    //fwrite(p, 1, p->width*p->height, file);
    //fclose(file);
}
