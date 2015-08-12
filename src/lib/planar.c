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
/*
 * Implimentation of the planar buffer files.
 */
#include <stdlib.h>
#include "src/lib/planar.h"

/* creates a planar buffer from the bitmap data.
   The planar buffer is dynamically allocated, and should
   be destroyed with the planar_buf_free function when no longer
   needed.
 */
planar_buf_t *
planar_buf_from_bitmap(bitmap_t *b) {
    planar_buf_t *p;
    int plane, bi, pi, x, y;

    /* allocate the buffer */
    p = planar_buf_alloc(b->width, b->height);

    /* copy the bitmap data into the planar format */
    bi=0;
    pi=0;
    for(y=0; y < b->height; y++) {
	/* start on the first plane */
	plane=0;
	for(x=0; x < b->width; x++) {
	    /* copy to each plane */
	    p->plane[plane++][pi]=b->data[bi++];

	    /* handle the completion of 4 planes. */
	    if(plane==4) {
		plane=0;
		pi++;
	    }
	}

	/* correct for images not divisible by 4 */
	if(plane) pi++;
    }

    return p;
}


/* allocates a planar buffer with specified dimensions */
planar_buf_t *
planar_buf_alloc(word width, word height) {
    planar_buf_t *p;
    int i;

    /* allocate the structure and populate sizes */
    p=malloc(sizeof(planar_buf_t));
    p->width  = width;
    p->height = height;
    p->pwidth = width / 4 + (width%4 ? 1 : 0);

    /* allocate the planes */
    for(i=0; i<4; i++) {
	p->plane[i] = malloc(p->height * p->pwidth);
    }

    return p;
}


/* deallocates a planar buffer */
void
planar_buf_free(planar_buf_t *p) {
    int i;

    /* free the planes */
    for(i=0; i<4; i++) {
	free(p->plane[i]);
    }

    /* free the structure */
    free(p);
}
