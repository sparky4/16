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
#include "src/lib/modex16/planar16.h"
#include "src/lib/modex16.h"

static struct pcxHeader {
	byte id;
	byte version;
	byte encoding;
	byte bpp;
	word xmin;
	word ymin;
	word xmax;
	word ymax;
	word hres;
	word vres;
	byte pal16[48];
	byte res1;
	word bpplane;
	word palType;
	word hScreenSize;
	word vScreenSize;
	byte padding[54];
} head;


static void loadPcxpbufStage1(FILE *file, planar_buf_t *result) {
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
		fprintf(stderr, "I only know how to handle 8bpp pcx files!\n");
		fclose(file);
		//exit(-2);
	}
}

static void loadPcxpbufPalette(FILE *file, planar_buf_t *result) {
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

/*	sparky4's functions~	*/
planar_buf_t planarLoadPcx(char *filename)
{
	FILE *file;
	planar_buf_t result;
	dword bufSize;
	int index, plane, x, y;
	byte count, val;
	word q;

	/* open the PCX file for reading */
	file = fopen(filename, "rb");
	if(!file) {
		fprintf(stderr, "Could not open %s for reading.\n", filename);
		//exit(-2);
	}

	/* load the first part of the pcx file */
	loadPcxpbufStage1(file, &result);

	/* allocate the buffer */
	bufSize = (/*(dword)*/result.width * result.height);
	//result = pbuf_alloc(result.width, result.height);
	if(!result.plane[0]) {
		fprintf(stderr, "Could not allocate memory for bitmap data.");
		fclose(file);
		//exit(-1);
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

	// start on the first plane
	plane=0;
	/* write the pixel the specified number of times */
	for(; count && index < bufSize; count--,index++)  {
		switch (plane)
		{
			case 4:
				plane=0;
			break;
		}
		// copy to each plane
		result.plane[plane++][index]=val;
	}
	} while(index < bufSize);

	//++++loadPcxpbufPalette(file, &result);
	fclose(file);
	return result;

}

//TODO: update!!
tileset_t
planarLoadPcxTiles(char *filename, word twidth, word theight) {
	tileset_t ts;
	FILE *file;
	planar_buf_t result;
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
	/*ts.data = malloc(sizeof(byte*) * ts.ntiles);
	ts.data[0] = malloc(sizeof(byte) * ts.ntiles * twidth * theight);
	for(i=1; i < ts.ntiles; i++) {
		ts.data[i] = ts.data[i-1] + twidth * theight;
	}*/

	/* finish off the file */
	loadPcxPalette(file, &result);

	fclose(file);

	return ts;
}
