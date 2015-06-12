#include <dos.h>
#include <string.h>
#include <mem.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include "src/lib/modex16.h"


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
	modexPalUpdate1(palette);
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
        modexPalUpdate1(tmppal);
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
modexPalUpdate(bitmap_t *bmp, word *i, word qp, word aqoffset)
{
	byte *p = bmp->palette;
	word w=0;
	word q=0;
	word qq=0;
	static word a[PAL_SIZE];	//palette array of change values!
	word z=0, aq=0, aa=0, pp=0;

	modexWaitBorder();
	if((*i)==0)
	{
		memset(a, -1, sizeof(a));
		outp(PAL_WRITE_REG, 0);  /* start at the beginning of palette */
	}
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
		outp(PAL_WRITE_REG, qq);  /* start at the beginning of palette */
	}
	if((*i)<PAL_SIZE/2 && w==0)
	{
		for(; (*i)<PAL_SIZE/2; (*i)++)
		{
			//if(i%3==0 && (p[i+5]==p[i+4] && p[i+4]==p[i+3] && p[i+3]==p[i+2] && p[i+2]==p[i+1] && p[i+1]==p[i] && p[i+5]==p[i]))
//____			if((qp>0)&&((*i)-q)%3==0 && (p[((*i)-q)]==p[((*i)-q)+3] && p[((*i)-q)+1]==p[((*i)-q)+4] && p[((*i)-q)+2]==p[((*i)-q)+5])) outp(PAL_DATA_REG, p[(*i)-q]); else
			if(((((*i)-q)%3==0)) && (p[((*i)-q)]==p[((*i)-q)+3] && p[((*i)-q)+1]==p[((*i)-q)+4] && p[((*i)-q)+2]==p[((*i)-q)+5]))
			{
				w++;
				break;
			}
			else if(qp>0 && (*i)>=(qp) && (*i)<((qp)+3))
			{
				//printf("qp=%d\n", qp);
				//printf("		(*i)=%d	a[%d]=%d\n", (*i), qp, a[qp]);
				printf("		%d's color=%d\n", (*i), (a[qp])-(bmp->offset*3)+qp);
				//outp(PAL_DATA_REG, p[((a[qp])-(bmp->offset*3)+qp)]);// fix this shit!
				if((*i)+1==(qp)+3){ w++; /*(*i)++;*/ break; }
			}
			else
			{
				if(bmp->offset==0 && (*i)<3 && q==0) outp(PAL_DATA_REG, 0);
				else
				if(qp==0) outp(PAL_DATA_REG, p[(*i)-q]);
				else{ //outp(PAL_DATA_REG, p[((*i)-(bmp->offset*3)+qp)]);
				printf("p[]=%d	qp=%d	p[]-qp=%d\n", ((*i)-(bmp->offset*3)), qp, ((*i)-(bmp->offset*3))+qp); }
			}
		}
		//if(qp>0) printf("qp=%d\n", qp);
		//if(qp>0) printf("						(*i)=%d\n", (*i)/3);
	}
	modexWaitBorder();	    /* waits one retrace -- less flicker */
	if((*i)>=PAL_SIZE/2 && w==0)
	{
		for(; (*i)<PAL_SIZE; (*i)++)
		{
//____			if((qp>0)&&((*i)-q)%3==0 && (p[((*i)-q)]==p[((*i)-q)+3] && p[((*i)-q)+1]==p[((*i)-q)+4] && p[((*i)-q)+2]==p[((*i)-q)+5])) outp(PAL_DATA_REG, p[(*i)-q]); else
			if(((((*i)-q)%3==0)) && (p[((*i)-q)]==p[((*i)-q)+3] && p[((*i)-q)+1]==p[((*i)-q)+4] && p[((*i)-q)+2]==p[((*i)-q)+5]))
			{
				w++;
				break;
			}
			else if(qp>0 && (*i)>=(qp) && (*i)<((qp)+3))
			{
				//printf("qp=%d\n", qp);
				//printf("		(*i)=%d	a[%d]=%d\n", (*i), qp, a[qp]);
				printf("		%d's color=%d\n", (*i), (a[qp]-(bmp->offset*3)+qp));
				//outp(PAL_DATA_REG, p[((a[qp])-(bmp->offset*3)+qp)]);// fix this shit!
				if((*i)+1==(qp)+3){ w++; /*(*i)++;*/ break; }
			}
			else
			{
				if(qp==0) outp(PAL_DATA_REG, p[(*i)-q]);
				else{ //outp(PAL_DATA_REG, p[((*i)-(bmp->offset*3)+qp)]);
				printf("p[]=%d	qp=%d	p[]-qp=%d\n", ((*i)-(bmp->offset*3)), qp, ((*i)-(bmp->offset*3))+qp); }
			}
		}
		//printf("						(*i)=%d\n", (*i)/3);
	}

printf("\nqqqqqqqq\n\n");

	//palette checker~
	if(q>0 && qp==0)
	{
		long lq;
		long bufSize = (bmp->width * bmp->height);
		pp = q;
		//printf("1(*i)=%02d\n", (*i)/3);
		//printf("1z=%02d\n", z/3);
		chkcolor(bmp, &q, &a, &aa, &z, i);
		//printf("2(*i)=%02d\n", (*i)/3);
		//printf("2z=%02d\n", z/3);
		aq=0;
aqpee:
		while(aq<=aa)
		{
//			printf("a[%02d]=(%d)\n", aq, a[aq]);
			if(a[aq]==-1) aq++;
			else { aqoffset++; break; }
		}
//update the image data here!
	for(lq=0; lq<bufSize; lq++)
	{
				/*
									note to self
									use a[qp] instead of bmp->offset for this spot!
									NO! wwww
				*/

				/*
				Facking bloody point the values of the changed palette to correct values.... major confusion! wwww
				*/

		//(offset/bmp->offset)*bmp->offset


		//printf("%02d ",bmp->data[lq]+bmp->offset);
		//if(lq > 0 && lq%bmp->width==0) printf("\n");
		//printf("%02d_", bmp->data[lq]+bmp->offset);
		/*if(bmp->data[lq]+bmp->offset==aq)
		{
			//printf("%02d", bmp->data[lq]);
			//printf("\n%02d\n", bmp->offset);
			printf("aq=%02d	", aq);
			printf("a[aq]=%02d	", a[aq]);
			printf("a[aq]+aqpp=%02d	", a[aq]+aqpp);
			printf("a[aq]-aqpp=%02d\n", a[aq]-aqpp);
			//bmp->data[lq]=((bmp->data[lq]+bmp->offset)-a[aq]);
//++++			bmp->data[lq]=a[aq]-aqpp;
//			printf("_%d ", bmp->data[lq]);
			//if(lq > 0 && lq%bmp->width==0) printf("\n");
		}
		else if(bmp->data[lq]+bmp->offset < ((*i)/3)-aqpp)
		{
			if(bmp->data[lq]+bmp->offset >= aq)
			{
				bmp->data[lq]=(bmp->data[lq]+bmp->offset)-aqpp;//-((z-(*i))/3);
				//printf("_%d ", bmp->data[lq]+bmp->offset)-aqpp-((z-(*i))/3);
			}
			else bmp->data[lq]+=(bmp->offset-aqpp);
		}*/

		//printf("%02d`", bmp->data[lq]);
		//if(lq > 0 && lq%bmp->width==0) printf("\n");
	}

//printf("		aq=%02d\n", aq);
//printf("		aa=%02d\n", aa);

	//update the palette~
	modexPalUpdate(bmp, &pp, aq, aqoffset);
	(*i)=pp;

	if(aq<aa){ pp=q; aq++; goto aqpee; }
	}
}

void
modexPalUpdate1(byte *p)
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

void
modexPalUpdate0(byte *p)
{
	int i;
	modexWaitBorder();
	outp(PAL_WRITE_REG, 0);  /* start at the beginning of palette */
	for(i=0; i<PAL_SIZE/2; i++)
	{
		outp(PAL_DATA_REG, rand());
	}
	modexWaitBorder();	    /* waits one retrace -- less flicker */
	for(; i<PAL_SIZE; i++)
	{
		outp(PAL_DATA_REG, rand());
	}
}

//color checker~
//i want to make another vesion that checks the palette when the palette is being appened~
void chkcolor(bitmap_t *bmp, word *q, word *a, word *aa, word *z, word *i/*, word *offset*/)
{
		byte *pal;
		word zz=0;
		pal = modexNewPal();
		modexPalSave(pal);
		//printf("q: %02d\n", (*q));
		printf("chkcolor start~\n");
		printf("1				(*z): %d\n", (*z)/3);
		printf("1				(*i): %d\n", (*i)/3);
//		printf("1 offset of color in palette	(*q): %d\n", (*q)/3);
		printf("wwwwwwwwwwwwwwww\n");
		//check palette for dups
		for(; (*z)<PAL_SIZE; (*z)+=3)
		{
			//printf("\n		z: %d\n", (*z));
			//printf("		q: %d\n", (*q));
			//printf("		z+q: %d\n\n", ((*z)+(*q)));
			//if((*z)%3==0)
			//{
//----				if(pal[(*z)]==pal[(*z)+3] && pal[(*z)+1]==pal[(*z)+4] && pal[(*z)+2]==pal[(*z)+5])
				if((*z)==(*i))
				{
//					printf("\n%d	[%02d][%02d][%02d]\n", (*z), pal[(*z)], pal[(*z)+1], pal[(*z)+2]);
//					printf("%d	[%02d][%02d][%02d]\n\n", (*z)+3, pal[(*z)+3], pal[(*z)+4], pal[(*z)+5]);
//0000					(*z)-=3;
					break;
				}
				else for(zz=0; zz<(*q); zz+=3)
				{
					//printf("zz: %02d\n", zz/3);
					if(zz%3==0)
					{
						if(pal[((*z)+(*q))]==pal[((*z)+(*q))+3] && pal[((*z)+(*q))+1]==pal[((*z)+(*q))+4] && pal[((*z)+(*q))+2]==pal[((*z)+(*q))+5])	//break if duplicate colors found in palette because it have reached the end of the current data of the palette
						{
//							(*z)-=3;
//							(*i)-=3;
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
//++++							(*i)--;
//							(*z)--;
							//expand dong here
/*
planned features that i plan to implement~
image that has values on the pallete list!
wwww
no... wait.... no wwww
*/
							//for(zzii=0; zzii<3; zzii++)
							//{
								//printf("z+q: %d\n\n", ((*z)+(*q)));
								a[(((*z)+(*q)))]=zz;
							//}
							(*aa)=(((*z)+(*q)));
							printf("!!					a[%02d]: %d\n", (((*z)+(*q))/3), zz/3);
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
		}
		printf("wwwwwwwwwwwwwwww\n");
		printf("2				(*z): %d\n", (*z)/3);
		printf("2				(*i): %d\n", (*i)/3);
//		printf("2 offset of color in palette	(*q): %d\n", (*q)/3);
		printf("chkcolor end~\n");
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

/*****************************************************************************
find 8x8 font in VGA BIOS ROM
*****************************************************************************/
byte far *bios_8x8_font(void)
{
	byte far *font;
	regs_t regs;

/* use BIOS INT 10h AX=1130h to find font #3 (8x8) in ROM */
	memset(&regs, 0, sizeof(regs)); /* for Watcom C */
	regs.w.ax = 0x1130;
	regs.w.bx = 0x0300;
	intr(0x10, &regs);
	font = (byte far *)MK_FP(regs.w.es, regs.w.bp);
	return font;
}

/*****************************************************************************
*****************************************************************************/
void bputs(page_t *pee, int x, int y, const char *s)
{
	byte far *font;
	byte far *fontoffset;

	font = bios_8x8_font();
	//printf("font=%Fp\n", font);
	for(; *s != '\0'; s++)
	{
		//src.raster = font + 8 * (*s);
		//BLOODY!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!111111111111!!!11!!11!111!11!!1111!!111!11!!1!!!11!11!!1!!111!11!!
//		(*(bmp->data)) = (*(font + 8 * (*s)));

		//printf("fontoffset=%Fp\n", font + 8 * (*s));
		//printf("*fontoffset=%s\n", *(font + 8 * (*s)));
		//printf("w.data=%Fp\n", (w.data));
		//printf("*w.data=%s\n", *(w.data));
		//blit1(&src, bmp, x, y);
//		modexDrawSprite(page, x, y, bmp);
//		modexDrawBmp(page, x, y, bmp);
//		printf("%x\n", (*(font + 8 * (*s))));
		//_fmemset(VGA, *(font + 8 * (*s)), _msize(font));
		//draw text?!?! wwww
		modexClearRegion(pee, x, y, 8, 8, 4);
		x += 8;
	}
//	printf("\n");
}
