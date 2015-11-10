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
#include "src/lib/modex16/16planar.h"

/*
 *============================================================================
 */

static void loadPcxpbufStage1(FILE *file, planar_buf_t *result) {
	int index;
	byte count, val;
	long int pos;

//word w=0;
//fprintf(stderr, "\nplanarLoadPcx: ");
//fprintf(stderr, "%u ", w++);
	/* read the header */
	fread(&head, sizeof(char), sizeof(struct pcxHeader), file);
//fprintf(stderr, "%u ", w++);
	/* get the width and height */
	result->width = head.xmax - head.xmin + 1;
	result->height = head.ymax - head.ymin + 1;
	result->pwidth = result->width / 4 + (result->width%4 ? 1 : 0);
//fprintf(stderr, "%u ", w++);
	/* make sure this  is 8bpp */
	if(head.bpp != 8) {
		fprintf(stderr, "I only know how to handle 8bpp pcx files!\n");
		fclose(file);
		exit(-2);
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

/* allocates a planar buffer with specified dimensions */
static planar_buf_t
pbuf_alloc(word width, word height) {
	planar_buf_t p;
	int i;

	/* allocate the structure and populate sizes */
	//p=malloc(sizeof(planar_buf_t));
	p.width  = width;
	p.height = height;
	p.pwidth = width / 4 + (width%4 ? 1 : 0);
	//p.pwidth = width / 4 + (width%4 ? 1 : 0);

	/* allocate the planes */
	for(i=0; i<4; i++) {
		p.plane[i] = _fmalloc(p.height * p.pwidth);
	}

	return p;
}

/* allocates a planar buffer with specified dimensions */
static void
pbuf_alloc0(planar_buf_t *p, word width, word height) {
	int i;

	/* allocate the structure and populate sizes */
	p=_fmalloc(sizeof(planar_buf_t));
	p->width  = width;
	p->height = height;
	p->pwidth = width / 4 + (width%4 ? 1 : 0);
	//p.pwidth = width / 4 + (width%4 ? 1 : 0);

	/* allocate the planes */
	for(i=0; i<4; i++) {
		p->plane[i] = _fmalloc(p->height * p->pwidth);
	}
}

/*	sparky4's functions~	*/
planar_buf_t planarLoadPcx(char *filename)
{
	FILE *file;
	planar_buf_t result;
	dword bufSize;
	word index[4], plane;
	byte count, val;

	word px,py,i,pla;

/*word w=0;
fprintf(stderr, "\nplanarLoadPcx: ");
fprintf(stderr, "%u ", w++);*/
	/* open the PCX file for reading */
	file = fopen(filename, "rb");
//fprintf(stderr, "%u ", w++);
	if(!file) {
		fprintf(stderr, "Could not open %s for reading.\n", filename);
		exit(-2);
	}
//fprintf(stderr, "%u ", w++);
	/* load the first part of the pcx file */
	loadPcxpbufStage1(file, &result);
//fprintf(stderr, "%u ", w++);
	/* allocate the buffer */
	bufSize = ((dword)result.width * result.height);
	result = pbuf_alloc(result.width, result.height);
	//pbuf_alloc0(&result, result.width, result.height);

//fprintf(stderr, "%u ", w++);
// 	printf("&bufSize=%p\n", &bufSize);
// 	printf("&result.data=%p\n", result.plane);
// 	printf("Size of block is %zu bytes\n", _msize(result.plane));
// 	printf("Size of bufSize is %zu bytes\n", bufSize);
// 	printf("Size of result.width is %zu \n", result.width);
// 	printf("Size of result.height is %zu \n", result.height);
// 	printf("Dimensions of result is %lu\n", (dword)result.width*result.height);
// 	//exit(0);
	if(!result.plane) {
		fprintf(stderr, "Could not allocate memory for bitmap data.");
		fclose(file);
		exit(-1);
	}
//fprintf(stderr, "read the buffer %u ", w++);
	/*  read the buffer in */
	index[0] = 0,index[1]=0,index[2]=0,index[3]=0;
	/* start on the first plane */
	plane=0;
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

// if(index[plane]==0 && plane==0) fprintf(stdout, "Val dump of %u[%u]	&&	count=%02X:\n", index[plane], plane, count);
//fprintf(stdout, "Val dump of %u[%u]	&&	count=%02X:\n", index[plane], plane, count);
// fprintf(stdout, "%02X ", val);
// if(index[plane]==result.pwidth-1) fprintf(stdout, "\n");

	/* write the pixel the specified number of times */
//fprintf(stderr, "\nputting in memory~ %u\n", w++);
	for(; count && (index[0]+index[1]+index[2]+index[3]) < bufSize; count--,index[plane]++)  {
		switch (plane)
		{
			case 4:
				plane=0;
			break;
			case 0:
			case 1:
			case 2:
				plane++;
			break;
			default:
				plane=0;
			break;
		}
		// copy to each plane
		result.plane[plane][index[plane]]=val;
//fprintf(stdout, "count=%02X	index=%u	plane=%u	", count, index[plane], plane);
//fprintf(stdout, "index val=%02X	val=%02X\n", result.plane[plane][index[plane]], val);
	}
// fprintf(stdout, "\nindex=%lu		bufsize=%lu\n\n", (dword)(index[0]+index[1]+index[2]+index[3]),  bufSize);
	} while((index[0]+index[1]+index[2]+index[3]) < bufSize);
	loadPcxpbufPalette(file, &result);
	fclose(file);

// 	//dump value!!
// 	for(pla=0; pla < 4; pla++) {
// 		i=0;
// 		printf("Plane %d\n", pla);
// 		for(py=0; py < result.height; py++) {
// 			for(px=0; px < result.pwidth; px++) {
// 				printf("%02X ", (int) result.plane[pla][i++]);
// 			}
// 			printf("\n");
// 		}
// 	}
// fprintf(stderr, "\n\n%s	count=%d	index=%d	plane=%d\n", filename, count, (dword)(index[0]+index[1]+index[2]+index[3]), pla);
// exit(0);
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
	loadPcxpbufStage1(file, &result);

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
	//++++loadPcxPalette(file, &result);

	fclose(file);

	return ts;
}
