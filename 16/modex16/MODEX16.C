#include <dos.h>
#include <string.h>
#include <mem.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include "modex16.h"


byte far* VGA=(byte far*) 0xA0000000; 	/* this points to video memory. */

static void fadePalette(sbyte fade, sbyte start, word iter, byte *palette);
static byte tmppal[PAL_SIZE];
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
};


static void
vgaSetMode(byte mode)
{
  union REGS regs;

  regs.h.ah = SET_MODE;
  regs.h.al = mode;
  int86(VIDEO_INT, &regs, &regs);
}


/* -========================= Entry  Points ==========================- */
void
modexEnter() {
    word i;
    dword far*ptr=(dword far*)VGA;      /* used for faster screen clearing */
    word CRTParms[] = {
	0x0d06,		/* vertical total */
	0x3e07,		/* overflow (bit 8 of vertical counts) */
	0x4109,		/* cell height (2 to double-scan */
	0xea10,		/* v sync start */
	0xac11,		/* v sync end and protect cr0-cr7 */
	0xdf12,		/* vertical displayed */
	0x0014,		/* turn off dword mode */
	0xe715,		/* v blank start */
	0x0616,		/* v blank end */
	0xe317		/* turn on byte mode */
    };
    int CRTParmCount = sizeof(CRTParms) / sizeof(CRTParms[0]);

    /* TODO save current video mode and palette */
    vgaSetMode(VGA_256_COLOR_MODE);

    /* disable chain4 mode */
    outpw(SC_INDEX, 0x0604);

    /* synchronous reset while setting Misc Output */
    outpw(SC_INDEX, 0x0100);

    /* select 25 MHz dot clock & 60 Hz scanning rate */
    outp(MISC_OUTPUT, 0xe3);

    /* undo reset (restart sequencer) */
    outpw(SC_INDEX, 0x0300);

    /* reprogram the CRT controller */
    outp(CRTC_INDEX, 0x11); /* VSync End reg contains register write prot */
    outp(CRTC_DATA, 0x7f);  /* get current write protect on varios regs */

    /* send the CRTParms */
    for(i=0; i<CRTParmCount; i++) {
	outpw(CRTC_INDEX, CRTParms[i]);
    }

    /* clear video memory */
    outpw(SC_INDEX, 0x0f02);
    for(i=0; i<0x8000; i++) {
	ptr[i] = 0x0000;
    }
}


void
modexLeave() {
    /* TODO restore original mode and palette */
    vgaSetMode(TEXT_MODE);
}


void
modexShowPage(page_t page) {
    word high_address;
    word low_address;

    high_address = HIGH_ADDRESS | ((word)(page) & 0xff00);
    low_address  = LOW_ADDRESS  | ((word)(page) << 8);

    /* wait for appropriate timing */
    while ((inp(INPUT_STATUS_1) & DISPLAY_ENABLE));
    outpw(CRTC_INDEX, high_address);
    outpw(CRTC_INDEX, low_address);

    /*  wait for one retrace */
    while (!(inp(INPUT_STATUS_1) & VRETRACE)); 
}


void
modexPanPage(page_t *page, int dx, int dy) {
    /* TODO figure out how the $@#! you do horizontal panning */
    *page += dy * SCREEN_WIDTH;
}


void
modexSelectPlane(byte plane) {
    outp(SC_INDEX, MAP_MASK);          /* select plane */
    outp(SC_DATA,  plane);
}


void
modexClearRegion(page_t page, int x, int y, int w, int h, byte color) {
    byte plane;
    word endx = x + w;
    word endy = y + h;
    word dx, dy;

    /* TODO Make this fast.  It's SLOOOOOOW */
    for(plane=0; plane < 4; plane++) {
	modexSelectPlane(PLANE(plane+x));
	for(dx = x; dx < endx; dx+=4) {
	    for(dy=y; dy<endy; dy++) {
		page[PAGE_OFFSET(dx, dy)] = color;
	    }
	}
    }
}


void
modexDrawBmp(page_t page, int x, int y, bitmap_t *bmp, byte sprite) {
    byte plane;
    word px, py;
    word offset;

    /* TODO Make this fast.  It's SLOOOOOOW */
    for(plane=0; plane < 4; plane++) {
	modexSelectPlane(PLANE(plane+x));
	for(px = plane; px < bmp->width; px+=4) {
	    offset=px;
	    for(py=0; py<bmp->height; py++) {
		if(!sprite || bmp->data[offset])
		  page[PAGE_OFFSET(x+px, y+py)] = bmp->data[offset];
		offset+=bmp->width;
	    }
	}
    }
}


/* fade and flash */
void
modexFadeOn(word fade, byte *palette) {
    fadePalette(-fade, 64, 64/fade+1, palette);
}


void
modexFadeOff(word fade, byte *palette) {
    fadePalette(fade, 0, 64/fade+1, palette);
}


void
modexFlashOn(word fade, byte *palette) {
    fadePalette(fade, -64, 64/fade+1, palette);
}


void
modexFlashOff(word fade, byte *palette) {
    fadePalette(-fade, 0, 64/fade+1, palette);
}


static void
fadePalette(sbyte fade, sbyte start, word iter, byte *palette) {
    word i;
    byte dim = start;

    /* handle the case where we just update */
    if(iter == 0) {
	modexPalUpdate(palette);
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
        modexPalUpdate(tmppal);
	iter--;
	dim += fade;
    }
}


/* save and load */
void
modexPalSave(byte *palette) {
    int  i;

    outp(PAL_READ_REG, 0);	/* start at palette entry 0 */
    for(i=0; i<PAL_SIZE; i++) {
	palette[i] = inp(PAL_DATA_REG); /* read the palette data */
    }
}


byte *
modexNewPal() {
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
modexLoadPalFile(byte *filename, byte **palette) {
    FILE *file;
    byte *ptr;

    /* free the palette if it exists */
    if(*palette) {
	free(*palette);
    }

    /* allocate the new palette */
    *palette = modexNewPal();

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


void
modexSavePalFile(char *filename, byte *pal) {
    unsigned int i;
    FILE *file;

    /* open the file for writing */
    file = fopen(filename, "wb");
    if(!file) {
	printf("Could not open %s for writing\n", filename);
	exit(-2);
    }

    /* write the data to the file */
    fwrite(pal, 1, PAL_SIZE, file);
    fclose(file);
}


/* blanking */
void
modexPalBlack() {
    fadePalette(-1, 64, 1, tmppal);
}


void
modexPalWhite() {
    fadePalette(-1, -64, 1, tmppal);
}


/* utility */
void
modexPalUpdate(byte *p) {
    int i;
    modexWaitBorder();
    outp(PAL_WRITE_REG, 0);  /* start at the beginning of palette */
    for(i=0; i<PAL_SIZE/2; i++) {
	outp(PAL_DATA_REG, p[i]);
    }
    modexWaitBorder();	    /* waits one retrace -- less flicker */
    for(i=PAL_SIZE/2; i<PAL_SIZE; i++) {
	outp(PAL_DATA_REG, p[i]);
    }
}


void
modexWaitBorder() {
    while(inp(INPUT_STATUS_1)  & 8)  {
	/* spin */
    }

    while(!(inp(INPUT_STATUS_1)  & 8))  {
	/* spin */
    }
}


bitmap_t
modexLoadPcx(char *filename) {
    FILE *file;
    bitmap_t result;
    struct pcxHeader head;
    long bufSize;
    int index;
    byte count, val;

    /* open the PCX file for reading */
    file = fopen(filename, "rb");
    if(!file) {
	printf("Could not open %s for reading.\n", filename);
	exit(-2);
    }

    /* read the header */
    fread(&head, sizeof(char), sizeof(struct pcxHeader), file);

    /* make sure this  is 8bpp */
    if(head.bpp != 8) {
	printf("I only know how to handle 8bpp pcx files!\n");
	fclose(file);
	exit(-2);
    }

    /* allocate the buffer */
    result.width = head.xmax - head.xmin + 1;
    result.height = head.ymax - head.ymin + 1;
    bufSize = result.width * result.height;
    result.data = malloc(bufSize);
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

    /* handle the palette */
    fseek(file, -769, SEEK_END);
    val = fgetc(file);
    result.palette = modexNewPal();
    if(head.version == 5 && val == 12) {
	/* use the vga palette */
	for(index=0; !feof(file) && index < PAL_SIZE; index++) {
	    val = fgetc(file);
	    result.palette[index] = val >> 2;
	}
    } else {
	/* use the 16 color palette */
	for(index=0; index<48; index++) {
	    result.palette[index]  = head.pal16[index];
	}
    }

    fclose(file);

    return result;
}
