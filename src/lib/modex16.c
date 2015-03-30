#include <dos.h>
#include <string.h>
#include <mem.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include "src\lib\modex16.h"


byte far* VGA=(byte far*) 0xA0000000; 	/* this points to video memory. */

static void fadePalette(sbyte fade, sbyte start, word iter, byte *palette);
static byte tmppal[PAL_SIZE];

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


page_t
modexDefaultPage() {
    page_t page;

    /* default page values */
    page.data = VGA;
    page.dx = 0;
    page.dy = 0;
    page.width = SCREEN_WIDTH;
    page.height = SCREEN_HEIGHT;

    return page;
}

/* returns the next page in contiguous memory
 * the next page will be the same size as p, by default
 */
page_t
modexNextPage(page_t *p) {
    page_t result;

    result.data = p->data + (p->width/4)*p->height;  /* compute the offset */
    result.dx = 0;
    result.dy = 0;
    result.width = p->width;
    result.height = p->height;

    return result;
}


void
modexShowPage(page_t *page) {
    word high_address;
    word low_address;
    word offset;
    byte crtcOffset;

    /* calculate offset */
    offset = (word) page->data;
    offset += page->dy * (page->width >> 2 );
    offset += page->dx >> 2;

    /* calculate crtcOffset according to virtual width */
    crtcOffset = page->width >> 3;

    high_address = HIGH_ADDRESS | (offset & 0xff00);
    low_address  = LOW_ADDRESS  | (offset << 8);

    /* wait for appropriate timing and then program CRTC */
    while ((inp(INPUT_STATUS_1) & DISPLAY_ENABLE));
    outpw(CRTC_INDEX, high_address);
    outpw(CRTC_INDEX, low_address);
    outp(CRTC_INDEX, 0x13);
    outp(CRTC_DATA, crtcOffset);

    /*  wait for one retrace */
    while (!(inp(INPUT_STATUS_1) & VRETRACE)); 

    /* do PEL panning here */
    outp(AC_INDEX, 0x33);
    outp(AC_INDEX, (page->dx & 0x03) << 1);
}


void
modexPanPage(page_t *page, int dx, int dy) {
    page->dx = dx;
    page->dy = dy;
}


void
modexSelectPlane(byte plane) {
    outp(SC_INDEX, MAP_MASK);          /* select plane */
    outp(SC_DATA,  plane);
}


void
modexClearRegion(page_t *page, int x, int y, int w, int h, byte  color) {
    word pageOff = (word) page->data;
    word xoff=x/4;       /* xoffset that begins each row */
    word scanCount=w/4;  /* number of iterations per row (excluding right clip)*/
    word poffset = pageOff + y*(page->width/4) + xoff; /* starting offset */
    word nextRow = page->width/4-scanCount-1;  /* loc of next row */
    byte lclip[] = {0x0f, 0x0e, 0x0c, 0x08};  /* clips for rectangles not on 4s */
    byte rclip[] = {0x00, 0x01, 0x03, 0x07};
    byte left = lclip[x&0x03];
    byte right = rclip[(x+w)&0x03];

    /* handle the case which requires an extra group */
    if((x & 0x03) && !((x+w) & 0x03)) {
      right=0x0f;
    }

    __asm {
		MOV AX, SCREEN_SEG      ; go to the VGA memory
		MOV ES, AX
		MOV DI, poffset		; go to the first pixel
		MOV DX, SC_INDEX	; point to the map mask
		MOV AL, MAP_MASK
		OUT DX, AL
		INC DX
		MOV AL, color		; get ready to write colors
	SCAN_START:
		MOV CX, scanCount	; count the line
		MOV BL, AL		; remember color
		MOV AL, left		; do the left clip
		OUT DX, AL		; set the left clip
		MOV AL, BL		; restore color
		STOSB			; write the color
		DEC CX
		JZ SCAN_DONE		; handle 1 group stuff

		;-- write the main body of the scanline
		MOV BL, AL	  	; remember color
		MOV AL, 0x0f		; write to all pixels
		OUT DX, AL
		MOV AL, BL		; restore color
		REP STOSB		; write the color
	SCAN_DONE:
		MOV BL, AL		; remeber color
		MOV AL, right
		OUT DX, AL		; do the right clip
		MOV AL, BL		; restore color
		STOSB			; write pixel
		ADD DI, nextRow		; go to the next row
		DEC h
		JNZ SCAN_START
    }
}


void
modexDrawBmp(page_t *page, int x, int y, bitmap_t *bmp) {
    /* draw the region (the entire freakin bitmap) */
    modexDrawBmpRegion(page, x, y, 0, 0, bmp->width, bmp->height, bmp);
}


void
modexDrawBmpRegion(page_t *page, int x, int y,
                   int rx, int ry, int rw, int rh, bitmap_t *bmp) {
    word poffset = (word) page->data  + y*(page->width/4) + x/4;
    byte *data = bmp->data;//+bmp->offset;
    word bmpOffset = (word) data + ry * bmp->width + rx;
    word width = rw;
    word height = rh;
    byte plane = 1 << ((byte) x & 0x03);
    word scanCount = width/4 + (width%4 ? 1 :0);
    word nextPageRow = page->width/4 - scanCount;
    word nextBmpRow = (word) bmp->width - width;
    word rowCounter;
    byte planeCounter = 4;

	//code is a bit slow here
    __asm {
		MOV AX, SCREEN_SEG      ; go to the VGA memory
		MOV ES, AX

		MOV DX, SC_INDEX	; point at the map mask register
		MOV AL, MAP_MASK	;
		OUT DX, AL		;

	PLANE_LOOP:
		MOV DX, SC_DATA		; select the current plane
		MOV AL, plane		;
		OUT DX, AL		;

		;-- begin plane painting
		MOV AX, height		; start the row counter
		MOV rowCounter, AX	; 
		MOV DI, poffset		; go to the first pixel
		MOV SI, bmpOffset	; go to the bmp pixel
	ROW_LOOP:
		MOV CX, width		; count the columns
	SCAN_LOOP:
		MOVSB			; copy the pixel
		SUB CX, 3		; we skip the next 3
		ADD SI, 3		; skip the bmp pixels
		LOOP SCAN_LOOP		; finish the scan

		MOV AX, nextPageRow
		ADD DI, AX		; go to the next row on screen
		MOV AX, nextBmpRow
		ADD SI, AX		; go to the next row on bmp

		DEC rowCounter
		JNZ ROW_LOOP		; do all the rows
		;-- end plane painting

		MOV AL, plane		; advance to the next plane
		SHL AL, 1		;
		AND AL, 0x0f		; mask the plane properly
		MOV plane, AL		; store the plane

		INC bmpOffset		; start bmp at the right spot

		DEC planeCounter
		JNZ PLANE_LOOP		; do all 4 planes
    }
}


void
modexDrawPlanarBuf(page_t *page, int x, int y, planar_buf_t *bmp) {
    /* TODO - adapt from test code */
	int plane;
	for(plane=0; plane < 4; plane++)
	{
		//fack
	}
}


void
modexDrawSprite(page_t *page, int x, int y, bitmap_t *bmp) {
    /* draw the whole sprite */
    modexDrawSpriteRegion(page, x, y, 0, 0, bmp->width, bmp->height, bmp);
}

void
modexDrawSpriteRegion(page_t *page, int x, int y,
		      int rx, int ry, int rw, int rh, bitmap_t *bmp) {
    word poffset = (word)page->data + y*(page->width/4) + x/4;
    byte *data = bmp->data;//+bmp->offset;
    word bmpOffset = (word) data + ry * bmp->width + rx;
    word width = rw;
    word height = rh;
    byte plane = 1 << ((byte) x & 0x03);
    word scanCount = width/4 + (width%4 ? 1 :0);
    word nextPageRow = page->width/4 - scanCount;
    word nextBmpRow = (word) bmp->width - width;
    word rowCounter;
    byte planeCounter = 4;

    __asm {
		MOV AX, SCREEN_SEG      ; go to the VGA memory
		MOV ES, AX

		MOV DX, SC_INDEX	; point at the map mask register
		MOV AL, MAP_MASK	;
		OUT DX, AL		;

	PLANE_LOOP:
		MOV DX, SC_DATA		; select the current plane
		MOV AL, plane		;
		OUT DX, AL		;

		;-- begin plane painting
		MOV AX, height		; start the row counter
		MOV rowCounter, AX	; 
		MOV DI, poffset		; go to the first pixel
		MOV SI, bmpOffset	; go to the bmp pixel
	ROW_LOOP:
		MOV CX, width		; count the columns
	SCAN_LOOP:
		LODSB
		DEC SI
		CMP AL, 0
		JNE DRAW_PIXEL		; draw non-zero pixels

		INC DI			; skip the transparent pixel
		ADD SI, 1
		JMP NEXT_PIXEL
	DRAW_PIXEL:
		MOVSB			; copy the pixel
	NEXT_PIXEL:
		SUB CX, 3		; we skip the next 3
		ADD SI, 3		; skip the bmp pixels
		LOOP SCAN_LOOP		; finish the scan

		MOV AX, nextPageRow
		ADD DI, AX		; go to the next row on screen
		MOV AX, nextBmpRow
		ADD SI, AX		; go to the next row on bmp

		DEC rowCounter
		JNZ ROW_LOOP		; do all the rows
		;-- end plane painting

		MOV AL, plane		; advance to the next plane
		SHL AL, 1		;
		AND AL, 0x0f		; mask the plane properly
		MOV plane, AL		; store the plane

		INC bmpOffset		; start bmp at the right spot

		DEC planeCounter
		JNZ PLANE_LOOP		; do all 4 planes
    }
}


/* copy a region of video memory from one page to another.
 * It assumes that the left edge of the tile is the same on both
 * regions and the memory areas do not overlap.
 */
void
modexCopyPageRegion(page_t *dest, page_t *src,
		    word sx, word sy,
		    word dx, word dy,
		    word width, word height)
{
    word doffset = (word)dest->data + dy*(dest->width/4) + dx/4;
    word soffset = (word)src->data + sy*(src->width/4) + sx/4;
    word scans   = width/4;
    word nextSrcRow = src->width/4 - scans - 1;
    word nextDestRow = dest->width/4 - scans - 1;
    byte lclip[] = {0x0f, 0x0e, 0x0c, 0x08};  /* clips for rectangles not on 4s */
    byte rclip[] = {0x0f, 0x01, 0x03, 0x07};
    byte left = lclip[sx&0x03];
    byte right = rclip[(sx+width)&0x03];

    __asm {
		MOV AX, SCREEN_SEG	; work in the vga space
		MOV ES, AX		;
		MOV DI, doffset		;
		MOV SI, soffset		;

		MOV DX, GC_INDEX	; turn off cpu bits
		MOV AX, 0008h		;
		OUT DX, AX

		MOV AX, SC_INDEX	; point to the mask register
		MOV DX, AX		;
		MOV AL, MAP_MASK	;
		OUT DX, AL		;
		INC DX			;

	ROW_START:
		PUSH DS
		MOV AX, ES
		MOV DS, AX
		MOV CX, scans		; the number of latches

		MOV AL, left		; do the left column
		OUT DX, AL		;
		MOVSB			;
		DEC CX			;

		MOV AL, 0fh		; do the inner columns
		OUT DX, AL
		REP MOVSB		; copy the pixels

		MOV AL, right		; do the right column
		OUT DX, AL
		MOVSB
		POP DS

		MOV AX, SI		; go the start of the next row
		ADD AX, nextSrcRow	;
		MOV SI, AX		;
		MOV AX, DI		;
		ADD AX, nextDestRow	;
		MOV DI, AX		;

		DEC height		; do the rest of the actions
		JNZ ROW_START		;

		MOV DX, GC_INDEX+1	; go back to CPU data
		MOV AL, 0ffh		; none from latches
		OUT DX, AL		;
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
	modexPalUpdate2(palette);
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
        modexPalUpdate2(tmppal);
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
modexPalUpdate(bitmap_t *bmp, word *i, word qp, word aqpp)
{
	byte *p = bmp->palette;
	word w=0;
	word q=0;
	word qq=0;
	word ii;
	static word a[256] = { 0 };
	word z=0,aq=0,aa=0;
	word pp=0,spee=0,ppee=0;
	sword aqpw;

//	if(qp>0) printf("(*i)=%02d\n", (*i));
	modexWaitBorder();
	if((*i)==0) outp(PAL_WRITE_REG, 0);  /* start at the beginning of palette */
	else if(qp==0)
	{
		q=(*i);
	}
	else
	{
		q=(*i);
		qq=(*i)/3;
//		printf("q: %02d\n", (q));
//		printf("qq: %02d\n", (qq));
		//printf("	(*i)-q=%02d\n", (*i)-q);
//		printf("================\n");
		outp(PAL_WRITE_REG, qq);  /* start at the beginning of palette */
	}
	if((*i)<PAL_SIZE/2 && w==0)
	{
		for(; (*i)<PAL_SIZE/2; (*i)++)
		{
			//if(i%3==0 && (p[i+5]==p[i+4] && p[i+4]==p[i+3] && p[i+3]==p[i+2] && p[i+2]==p[i+1] && p[i+1]==p[i] && p[i+5]==p[i]))
//____			if((qp>0)&&((*i)-q)%3==0 && (p[((*i)-q)]==p[((*i)-q)+3] && p[((*i)-q)+1]==p[((*i)-q)+4] && p[((*i)-q)+2]==p[((*i)-q)+5])) outp(PAL_DATA_REG, p[(*i)-q]); else
			if(((((*i)-q)%3==0) || ((qp>0)&&((*i)-(bmp->offset*3))%3==0)) && (p[((*i)-q)]==p[((*i)-q)+3] && p[((*i)-q)+1]==p[((*i)-q)+4] && p[((*i)-q)+2]==p[((*i)-q)+5]))
			{
				if(qp>0)
				{
					(*i)-=(aqpp*3);
					aqpw=aqpp-1;
					outp(PAL_WRITE_REG, qq+(((*i)+(aqpw*3)-(bmp->offset*3))/3));
					for(ii=aqpp; ii>0; ii--)
					{
						outp(PAL_DATA_REG, p[((((*i)+((aqpp-ii)*3))+((aqpp+ii)*3))-(bmp->offset*3))]);
						outp(PAL_DATA_REG, p[((((*i)+((aqpp-ii)*3))+((aqpp+ii)*3)+1)-(bmp->offset*3))]);
						outp(PAL_DATA_REG, p[((((*i)+((aqpp-ii)*3))+((aqpp+ii)*3)+2)-(bmp->offset*3))]);
//						printf("position	=	%d\n", qq+(((*i)+(aqpw*3)-(bmp->offset*3))/3));
/*if(qp>0){ //printf("[%d]", p[((*i)-q)]);	printf("[%d]", p[((*i)-q)+1]);	printf("[%d]", p[((*i)-q)+2]);	printf("[%d]", p[((*i)-q)+3]);			printf("[%d]", p[((*i)-q)+4]);			printf("[%d]", p[((*i)-q)+5]);			printf("	%d [%d]\n", (*i), p[((*i)-q)]); }
printf("[%d]", p[((((*i)+((aqpp-ii)*3)))-(bmp->offset*3))]);
printf("[%d]", p[((((*i)+((aqpp-ii)*3))+1)-(bmp->offset*3))]);
printf("[%d] | ", p[((((*i)+((aqpp-ii)*3))+2)-(bmp->offset*3))]);
printf("[%d]", p[((((*i)+((aqpp-ii)*3))+3)-(bmp->offset*3))]);
printf("[%d]", p[((((*i)+((aqpp-ii)*3))+4)-(bmp->offset*3))]);
printf("[%d]", p[((((*i)+((aqpp-ii)*3))+5)-(bmp->offset*3))]);
printf("	%d [%d]\n",((((*i)+((aqpp-ii)*3))+((aqpp+ii)*3))-(bmp->offset*3))/3, p[((((*i)+((aqpp-ii)*3))+((aqpp+ii)*3))-(bmp->offset*3))]); }*/
						//printf("%d\n", ((*i)+((ii)*3))/3);
						//printf("ii=%d\n", ii);
						//printf("aqpp=%d\n", aqpp);
						//printf("			%d\n", ((*i)+((aqpp-ii)*3))/3);
					}
					//printf("	%d\n",((((*i)+((aqpp-ii)*3)))-(bmp->offset*3)));
					//printf("	%d\n",((((*i)+((aqpp-ii)*3))+1)-(bmp->offset*3)));
					//printf("	%d\n",((((*i)+((aqpp-ii)*3))+2)-(bmp->offset*3)));
					//printf("(*i)=%d\n", (*i));
				}
				//printf("[%d]", p[((*i)-q)]);	printf("[%d]", p[((*i)-q)+1]);	printf("[%d]", p[((*i)-q)+2]);	printf("[%d]", p[((*i)-q)+3]);			printf("[%d]", p[((*i)-q)+4]);			printf("[%d]", p[((*i)-q)+5]);			printf("	%d [%d]\n", (*i), p[((*i)-q)]);
				w++;
				break;
			}
			else
			{
				if(qp==0) outp(PAL_DATA_REG, p[(*i)-q]);
				else outp(PAL_DATA_REG, p[((*i)-(bmp->offset*3))]);
			}
		}
	}
	modexWaitBorder();	    /* waits one retrace -- less flicker */
	if((*i)>=PAL_SIZE/2 && w==0)
	{
		for(; (*i)<PAL_SIZE; (*i)++)
		{
//____			if((qp>0)&&((*i)-q)%3==0 && (p[((*i)-q)]==p[((*i)-q)+3] && p[((*i)-q)+1]==p[((*i)-q)+4] && p[((*i)-q)+2]==p[((*i)-q)+5])) outp(PAL_DATA_REG, p[(*i)-q]); else
			if(((((*i)-q)%3==0) || ((qp>0)&&((*i)-(bmp->offset*3))%3==0)) && (p[((*i)-q)]==p[((*i)-q)+3] && p[((*i)-q)+1]==p[((*i)-q)+4] && p[((*i)-q)+2]==p[((*i)-q)+5]))
			{
				if(qp>0)
				{
					(*i)-=(aqpp*3);
					aqpw=aqpp-1;
					outp(PAL_WRITE_REG, qq+(((*i)+(aqpw*3)-(bmp->offset*3))/3));
					for(ii=aqpp; ii>0; ii--)
					{
						outp(PAL_DATA_REG, p[((((*i)+((aqpp-ii)*3))+((aqpp+ii)*3))-(bmp->offset*3))]);
						outp(PAL_DATA_REG, p[((((*i)+((aqpp-ii)*3))+((aqpp+ii)*3)+1)-(bmp->offset*3))]);
						outp(PAL_DATA_REG, p[((((*i)+((aqpp-ii)*3))+((aqpp+ii)*3)+2)-(bmp->offset*3))]);
//						printf("position	=	%d\n", qq+(((*i)+(aqpw*3)-(bmp->offset*3))/3));
/*if(qp>0){ //printf("[%d]", p[((*i)-q)]);	printf("[%d]", p[((*i)-q)+1]);	printf("[%d]", p[((*i)-q)+2]);	printf("[%d]", p[((*i)-q)+3]);			printf("[%d]", p[((*i)-q)+4]);			printf("[%d]", p[((*i)-q)+5]);			printf("	%d [%d]\n", (*i), p[((*i)-q)]); }
printf("[%d]", p[((((*i)+((aqpp-ii)*3)))-(bmp->offset*3))]);
printf("[%d]", p[((((*i)+((aqpp-ii)*3))+1)-(bmp->offset*3))]);
printf("[%d] | ", p[((((*i)+((aqpp-ii)*3))+2)-(bmp->offset*3))]);
printf("[%d]", p[((((*i)+((aqpp-ii)*3))+3)-(bmp->offset*3))]);
printf("[%d]", p[((((*i)+((aqpp-ii)*3))+4)-(bmp->offset*3))]);
printf("[%d]", p[((((*i)+((aqpp-ii)*3))+5)-(bmp->offset*3))]);
printf("	%d [%d]\n",((((*i)+((aqpp-ii)*3))+((aqpp+ii)*3))-(bmp->offset*3))/3, p[((((*i)+((aqpp-ii)*3))+((aqpp+ii)*3))-(bmp->offset*3))]); }*/
						//printf("%d\n", ((*i)+((ii)*3))/3);
						//printf("ii=%d\n", ii);
						//printf("aqpp=%d\n", aqpp);
						//printf("			%d\n", ((*i)+((aqpp-ii)*3))/3);
					}
					//printf("	%d\n",((((*i)+((aqpp-ii)*3)))-(bmp->offset*3)));
					//printf("	%d\n",((((*i)+((aqpp-ii)*3))+1)-(bmp->offset*3)));
					//printf("	%d\n",((((*i)+((aqpp-ii)*3))+2)-(bmp->offset*3)));
					//printf("(*i)=%d\n", (*i));
				}
				//printf("[%d]", p[((*i)-q)]);	printf("[%d]", p[((*i)-q)+1]);	printf("[%d]", p[((*i)-q)+2]);	printf("[%d]", p[((*i)-q)+3]);			printf("[%d]", p[((*i)-q)+4]);			printf("[%d]", p[((*i)-q)+5]);			printf("	%d [%d]\n", (*i), p[((*i)-q)]);
				w++;
				break;
			}
			else
			{
				if(qp==0) outp(PAL_DATA_REG, p[(*i)-q]);
				else outp(PAL_DATA_REG, p[((*i)-(bmp->offset*3))]);
			}
		}
	}

	//palette checker~
	if(q>0 && qp==0)
	{
		long lq;
		long bufSize = (bmp->width * bmp->height);
		chkcolor(bmp, &q, &a, &aa, &z);

		/*printf("z=%d\n", z/3);
		printf("q+z=%d\n", (q+z)/3);
		printf("z-ppee=%d\n", (z-ppee)/3);
		printf("q=%d\n", q/3);
		printf("aa=%d\n", aa);*/

		aq=0; pp = q; ppee=q;
aqpee:
		while(aq<=aa)
		{
			//printf("a[%02d]=(%d)", aq, a[aq]);
			if(a[aq]==0) aq++;
			else{ aqpp++; break; }
		}

/*		printf("aq=%02d\n", aq);
		printf("z=%02d\n", z/3);
		printf("(z/3)-aqpp=%02d\n", (z/3)-aqpp);
		printf("aqpp=%02d\n", aqpp);*/

	for(lq=0; lq<bufSize; lq++)
	{
		if(bmp->data[lq]+bmp->offset==aq)
		{
			//printf("\n%02d\n", bmp->data[lq]);
			//printf("\n%02d\n", bmp->offset);
			//printf("\naq=	%02d\n", aq);
			//printf("a[aq]=	%02d\n", a[aq]);
			//bmp->data[lq]=((bmp->data[lq]+bmp->offset)-a[aq]);
			bmp->data[lq]=a[aq];
			//printf("_%d \n", bmp->data[lq]);
		}
		else if(bmp->data[lq]+bmp->offset < (z/3)-aqpp)
		{
			if(bmp->data[lq]+bmp->offset >= aq) bmp->data[lq]=(bmp->data[lq]+bmp->offset)-aqpp;
			else bmp->data[lq]+=(bmp->offset);
		}

		//printf("%02d ", bmp->data[lq]);
		//if(lq > 0 && lq%bmp->width==0) printf("\n");
	}

	while(pp<=(aq*3))
	{
		if(((pp/3)==aq || spee>0))
		{
			/*printf("spee=%d\n", spee);
			printf("		pp=%02d	", pp/3);
			printf("old	bmp: [%d]", bmp->palette[(pp-ppee)]);
			printf("[%d]", bmp->palette[(pp-ppee)+1]);
			printf("[%d]\n", bmp->palette[(pp-ppee)+2]);*/
			//if(spee==0) printf("\npp=%02d\n\n", pp/3);
			bmp->palette[(pp-ppee)]=		bmp->palette[(pp-ppee)+3];
			bmp->palette[(pp-ppee)+1]=	bmp->palette[(pp-ppee)+4];
			bmp->palette[(pp-ppee)+2]=	bmp->palette[(pp-ppee)+5];
			if(spee==0) spee++;
		}
		/*printf("		pp=%02d	", pp/3);
		printf("	bmp: [%d]", bmp->palette[(pp-ppee)]);
		printf("[%d]", bmp->palette[(pp-ppee)+1]);
		printf("[%d]\n", bmp->palette[(pp-ppee)+2]);*/
		pp+=3;
	}

	//update the palette~
	//printf("	aqpp=		%d\n", aqpp);
	modexPalUpdate(bmp, &ppee, 1, aqpp);
	(*i)=ppee;
	//printf("	aqpp=	%d\n", aqpp);
	//printf("	ppee=	%d\n", ppee);

	/*printf(".\n");
	printf("aqpp=	%02d\n", aqpp/3);
	printf("aq=	%02d\n", aq);
	printf("aa=	%02d\n", aa);
	printf("		ppee=	%02d\n", ppee);*/

	if(aq<aa){ /*printf("~~~~\n"); */ppee=q; aq++; goto aqpee; }
	/*printf("ppee=%d\n", ppee);
	printf("pp=%d\n", pp);
	printf("q=%d\n", q);
	printf("(*i)=%d\n", (*i));*/

	}
}

void
modexPalUpdate2(byte *p)
{
	int i;
	modexWaitBorder();
	outp(PAL_WRITE_REG, 0);  /* start at the beginning of palette */
	for(i=0; i<PAL_SIZE/2; i++)
	{
		outp(PAL_DATA_REG, p[i]);
	}
	modexWaitBorder();	    /* waits one retrace -- less flicker */
	for(; i<PAL_SIZE; i++)
	{
		outp(PAL_DATA_REG, p[(i)]);
	}
}

//color checker~
//i want to make another vesion that checks the palette when the palette is being appened~
void chkcolor(bitmap_t *bmp, word *q, word *a, word *aa, word *z)
{
		byte *pal;
		word zz=0;
		pal = modexNewPal();
		modexPalSave(pal);
		//printf("q: %02d\n", (*q));

		//check palette for dups
		for((*z)=0; (*z)<PAL_SIZE; (*z)+=3)
		{
//			printf("\n		z: %d\n", (*z));
//			printf("		q: %d\n", (*q));
//			printf("		z+q: %d\n\n", ((*z)+(*q)));
			if((*z)%3==0)
			{
				if(pal[(*z)]==pal[(*z)+3] && pal[(*z)+1]==pal[(*z)+4] && pal[(*z)+2]==pal[(*z)+5])
				{
//					printf("\n%d	[%02d][%02d][%02d]\n", (*z), pal[(*z)], pal[(*z)+1], pal[(*z)+2]);
//					printf("%d	[%02d][%02d][%02d]\n\n", (*z)+3, pal[(*z)+3], pal[(*z)+4], pal[(*z)+5]);
					break;
				}
				else for(zz=0; zz<(*q); zz+=3)
				{
					//zq=(zz+(q-3));
					//printf("zz: %02d\n", zz/3);
					if(zz%3==0)
					{
						if(pal[((*z)+(*q))]==pal[((*z)+(*q))+3] && pal[((*z)+(*q))+1]==pal[((*z)+(*q))+4] && pal[((*z)+(*q))+2]==pal[((*z)+(*q))+5])
						{
//							printf("\nzq1:%d[%02d][%02d][%02d]\n", (zz+q), pal[(zz+q)], pal[(zz+q)+1], pal[(zz+q)+2]);
//							printf("zq2:%d[%02d][%02d][%02d]\n\n", (zz+q)+3, pal[(zz+q)+3], pal[(zz+q)+4], pal[(zz+q)+5]);
							break;
						}
						else if(pal[zz]==pal[((*z)+(*q))] && pal[zz+1]==pal[((*z)+(*q))+1] && pal[zz+2]==pal[((*z)+(*q))+2])
						{
//							printf("\n\nwwwwwwwwwwwwwwww\n");
//							printf("	zq: %d	[%02d][%02d][%02d] value that is needing to be changed~\n", ((*z)+(*q))/3, pal[((*z)+(*q))], pal[((*z)+(*q))+1], pal[((*z)+(*q))+2]);
//							printf("	zz: %d	[%02d][%02d][%02d] value that the previous value is going to change to~\n", (zz)/3, pal[zz], pal[zz+1], pal[zz+2]);
//							//printf("	zv: %d	[%02d][%02d][%02d] wwww\n", (zz-z+q)/3, pal[(zz-z+q)], pal[(zz-z+q)+1], pal[(zz-z+q)+2]);
//							printf("	z : %d	[%02d][%02d][%02d] offset value~\n", (*z)/3, pal[(*z)], pal[(*z)+1], pal[(*z)+2]);
							a[((*z)+(*q))/3]=zz/3;
							(*aa)=((*z)+(*q))/3;
//							printf("\n		aa: %d\n\n", (*aa));
//							printf("	a[%02d]=(%02d) offset array i think the palette should be updated again~\n", ((*z)+(*q))/3, a[((*z)+(*q))/3]);
//							printf("wwwwwwwwwwwwwwww\n\n");
						}
						/*else
						{
							printf("================\n");
							printf("zq: %d	[%02d][%02d][%02d]\n", ((*z)+(*q))/3, pal[((*z)+(*q))], pal[((*z)+(*q))+1], pal[((*z)+(*q))+2]);
							printf("zz: %d	[%02d][%02d][%02d]\n", (zz)/3, pal[zz], pal[zz+1], pal[zz+2]);
							printf("z : %d	[%02d][%02d][%02d]\n", (*z)/3, pal[(*z)], pal[(*z)+1], pal[(*z)+2]);
							printf("================\n");
						}*/
						//printf("[%d]", (zz+q));
					}
				}
				//printf("\nz:	%d\n", z);
				//printf("q:	%d\n", q);
				//printf("zz:	%d\n", zz);
			}
		}
		free(pal);
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
