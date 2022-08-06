/*
 * File: PALETTE.C
 * Purpose: This file contains routines for manipulating the VGA palette.
 */
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "palette.h"

static void fadePalette(byte fade, byte start, word iter, byte *palette);
static byte tmppal[PAL_SIZE];

/* fade and flash */
void
fadeOn(word fade, byte *palette) {
    fadePalette(-fade, 64, 64/fade+1, palette);
}


void
fadeOff(word fade, byte *palette) {
    fadePalette(fade, 0, 64/fade+1, palette);
}


void
fadeOffHalf(word fade, byte *palette) {
	fadePaletteHalf(fade, 0, 64/fade+1, palette);
}


void
flashOn(word fade, byte *palette) {
    fadePalette(fade, -64, 64/fade+1, palette);
}


void
flashOff(word fade, byte *palette) {
    fadePalette(-fade, 0, 64/fade+1, palette);
}


static void
fadePalette(byte fade, byte start, word iter, byte *palette) {
    word i;
    byte dim = start;

    /* handle the case where we just update */
    if(iter == 0) {
	palUpdate(palette);
	return;
    }

    while(iter > 0) {  /* FadeLoop */
	for(i=0; i<PAL_SIZE; i++) { /* loadpal_loop */
	    tmppal[i] = palette[i] - dim;
	    if(tmppal[i] > 127) {
		tmppal[i] = 0;
	    } else if(tmppal[i] > 63) {
		tmppal[i] = 63;
	    }
	}
        palUpdate(tmppal);
	iter--;
	dim += fade;
    }
}

static void
fadePaletteHalf(byte fade, byte start, word iter, byte *palette) {
    word i;
    byte dim = start;

    /* handle the case where we just update */
    if(iter == 0) {
	palUpdate(palette);
	return;
    }

    while(iter > iter/2) {  /* FadeLoop */
	for(i=0; i<PAL_SIZE; i++) { /* loadpal_loop */
	    tmppal[i] = palette[i] - dim;
	    if(tmppal[i] > 127) {
		tmppal[i] = 0;
	    } else if(tmppal[i] > 63) {
		tmppal[i] = 63;
	    }
	}
        palUpdate(tmppal);
	iter--;
	dim += fade;
    }
}

/* save and load */
void
palSave(byte *palette) {
    int  i;

    outp(PEL_READ_REG, 0);	/* start at palette entry 0 */
    for(i=0; i<PAL_SIZE; i++) {
	palette[i] = inp(PEL_DATA_REG); /* read the palette data */
    }
}


byte *
newPal() {
    byte *ptr;
    ptr = malloc(PAL_SIZE);

    /* handle errors */
    if(!ptr) {
	printf("Could not allocate palette.\n");
	exit(-1);
    }

    return ptr;
}


void
loadPaletteFile(byte *filename, byte **palette) {
    FILE *file;
    byte *ptr;

    /* free the palette if it exists */
    if(*palette) {
	free(*palette);
    }

    /* allocate the new palette */
    *palette = newPal();

    /* open the file */
    file = fopen(filename, "rb");
    if(!file) {
	printf("Could not open palette file: %s\n", filename);
	exit(-2);
    }

    /* read the file */
    ptr = *palette;
    while(!feof(file)) {
	*ptr++ = fgetc(file);
    }

    fclose(file);
}


/* blanking */
void
palBlack() {
    fadePalette(-1, 64, 1, tmppal);
}


void
palWhite() {
    fadePalette(-1, -64, 1, tmppal);
}


/* utility */
void
palUpdate(byte *p) {
    int i;
    waitborder();
    outp(PEL_WRITE_REG, 0);  /* start at the beginning of palette */
    for(i=0; i<PAL_SIZE/2; i++) {
	outp(PEL_DATA_REG, p[i]);
    }
    waitborder();	    /* waits one retrace -- less flicker */
    for(i=PAL_SIZE/2; i<PAL_SIZE; i++) {
	outp(PEL_DATA_REG, p[i]);
    }
}


void
waitborder() {
    while(inp(INPUT_STATUS_1)  & 8)  {
	/* spin */
    }

    while(!(inp(INPUT_STATUS_1)  & 8))  {
	/* spin */
    }
}
