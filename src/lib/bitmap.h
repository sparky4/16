/*
 * Functions and types for loading and manipulating bitmaps.
 */
#ifndef BITMAP_H
#define BITMAP_H
#include "src/lib/types.h"
#include "src/lib/lib_head.h"

typedef struct {
	byte near *data;
	word width;
	word height;
	byte *palette;
	word offset;
} bitmap_t;

typedef struct {
	byte **data;
	word ntiles;   /* the number of tiles */
	word twidth;   /* width of the tiles */
	word theight;  /* height of the tiles */
	byte *palette; /* palette for the tile set */
} tileset_t;

bitmap_t bitmapLoadPcx(char *filename);
tileset_t bitmapLoadPcxTiles(char *filename, word twidth, word theight);
#endif
