/*
 * Functions and types for a planar image buffer.  
 * This is meant to be able to load into video memory faster.
 */
#include "bitmap.h"

#ifndef PLANAR_H
#define PLANAR_H
typedef struct {
  byte *plane[4];     /* 4 planes of image data */
  word width;         /* width of the image (spread across 4 planes) */
  word height;        /* height of the image (spread across 4 planes) */
  word pwidth;        /* the number of bytes in each plane */
} planar_buf_t;


/* creates a planar buffer from the bitmap data.
   The planar buffer is dynamically allocated, and should
   be destroyed with the planar_buf_free function when no longer
   needed.
 */
planar_buf_t *planar_buf_from_bitmap(bitmap_t *b);


/* allocates a planar buffer with specified dimensions */
planar_buf_t *planar_buf_alloc(word width, word height);


/* deallocates a planar buffer */
void planar_buf_free(planar_buf_t *p);
#endif
