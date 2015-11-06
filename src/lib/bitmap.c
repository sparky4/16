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
#include <stdlib.h>
#include <malloc.h>
#include "src/lib/bitmap.h"

static void loadPcxStage1(FILE *file, bitmap_t *result) {
	long bufSize;
	int index;
	byte count, val;
	long int pos;

	/* read the header */
	fread(&head, sizeof(char), sizeof(struct pcxHeader), file);

	/* get the width and height */
	result->width = head.xmax - head.xmin + 1;
	result->height = head.ymax - head.ymin + 1;

	/* make sure this  is 8bpp */
	if(head.bpp != 8) {
		printf("I only know how to handle 8bpp pcx files!\n");
		fclose(file);
		exit(-2);
	}
}


static void loadPcxPalette(FILE *file, bitmap_t *result) {
	byte val;
	int index;

	/* handle the palette */
	fseek(file, -769, SEEK_END);
	val = fgetc(file);
	result->palette = modexNewPal();
	if(head.version == 5 && val == 12) {
	/* use the vga palette */
	for(index=0; !feof(file) && index < PAL_SIZE; index++) {
		val = fgetc(file);
		result->palette[index] = val >> 2;
	}
	} else {
	/* use the 16 color palette */
	for(index=0; index<48; index++) {
		result->palette[index]  = head.pal16[index];
	}
	}
}


bitmap_t
bitmapLoadPcx(char *filename) {
	FILE *file;
	bitmap_t result;
	dword bufSize;
	int index;
	byte count, val;

	/* open the PCX file for reading */
	file = fopen(filename, "rb");
	if(!file) {
		printf("Could not open %s for reading.\n", filename);
		exit(-2);
	}

	/* load the first part of the pcx file */
	loadPcxStage1(file, &result);

	/* allocate the buffer */
	//printf("%zu\n", _memmax());
	bufSize = (/*(dword)*/result.width * result.height);
	result.data = malloc(bufSize);
//	result.data = (byte far *)_fmalloc(bufSize);
//	result.data = (byte __huge *)halloc(bufSize, sizeof(byte));
	/*printf("&bufSize=%p\n", &bufSize);
	printf("&result.data=%p\n", result.data);
	printf("Size of block is %zu bytes\n", _msize(result.data));
	printf("Size of bufSize is %zu bytes\n", bufSize);
	printf("Size of result.width is %zu \n", result.width);
	printf("Size of result.height is %zu \n", result.height);
	printf("Dimensions of result is %lu\n", (dword)result.width*result.height);*/
	//exit(0);
	if(!result.data) {
		fprintf(stderr, "Could not allocate memory for bitmap data.");
		fclose(file);
		exit(-1);
	}

	/*  read the buffer in */
	index = 0;
	do {
	/* get the run length and the value */
	count = fgetc(file);
	if(0xC0 ==  (count & 0xC0)) { /* this is the run count */
		count &= 0x3f;
		val = fgetc(file);
	} else {
		val = count;
		count = 1;
	}

	/* write the pixel the specified number of times */
	for(; count && index < bufSize; count--,index++)  {
		result.data[index] = val;
	}
	} while(index < bufSize);
	printf("index=%d\n", index);

	loadPcxPalette(file, &result);

	fclose(file);

	return result;
}

//TODO: update!!
tileset_t
bitmapLoadPcxTiles(char *filename, word twidth, word theight) {
	tileset_t ts;
	FILE *file;
	bitmap_t result;
	int i;

	/* open the PCX file for reading */
	file = fopen(filename, "rb");
	if(!file) {
		printf("Could not open %s for reading.\n", filename);
		exit(-2);
	}

	/* load the first part of the pcx file */
	loadPcxStage1(file, &result);

	/* get the number of tiles and set up the result structure */
	ts.twidth = twidth;
	ts.theight = theight;
	ts.ntiles = (result.width/twidth) * (result.height/theight);
	ts.palette = result.palette;

	/* allocate the pixel storage for the tiles */
	ts.data = malloc(sizeof(byte*) * ts.ntiles);
	ts.data[0] = malloc(sizeof(byte) * ts.ntiles * twidth * theight);
	for(i=1; i < ts.ntiles; i++) {
		ts.data[i] = ts.data[i-1] + twidth * theight;
	}

	/* finish off the file */
	loadPcxPalette(file, &result);

	fclose(file);

	return ts;
}
