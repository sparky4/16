/*
 * Implimentation of the planar buffer files.
 */
#include <stdlib.h>
#include "planar.h"

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
