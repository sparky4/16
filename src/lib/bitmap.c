#include <stdio.h>
#include <stdlib.h>
#include "src/lib/bitmap.h"
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
    long bufSize;
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
    bufSize = result.width * result.height;
    result.data = _fmalloc(bufSize);	//it breaks right here~
    if(!result.data) {
	printf("Could not allocate memory for bitmap data.");
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

    loadPcxPalette(file, &result);

    fclose(file);

    return result;
}


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
    ts.data = _fmalloc(sizeof(byte*) * ts.ntiles);
    ts.data[0] = _fmalloc(sizeof(byte) * ts.ntiles * twidth * theight);
    for(i=1; i < ts.ntiles; i++) {
	ts.data[i] = ts.data[i-1] + twidth * theight;
    }

    /* finish off the file */
    loadPcxPalette(file, &result);

    fclose(file);

    return ts;
}
