/* Project 16 Source Code~
 * Copyright (C) 2012-2017 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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
/*
 * Render data code~
 */

#include "src/lib/16render.h"

//TODO! ADD CLIPPING!!
//memory management needs to be added
//void
//modexDrawBmpRegion	(page_t *page, int x, int y, int rx, int ry, int rw, int rh, bitmap_t *bmp)
void modexDrawPBufRegion	(page_t *page, int x, int y, int rx, int ry, int rw, int rh, planar_buf_t *p, boolean sprite)
{
	word plane;
	int i;
	const int px=x+page[0].dx;
	const int py=y+page[0].dy;
	const int prw = rw/4;
	int prh;

	//fine tuning
	rx+=1;
	ry+=1;

	//^^;
	#define PEEE	rx-prw
	#define PE		(p->pwidth)
	if(rh<p->height) prh = (PE*(ry-4));
	else if(rh==p->height) prh = (PE*(ry));
	y=py;
	x=px;
	//printf("%d,%d p(%d,%d) r(%d,%d) rwh(%d,%d)\n", x, y, px, py, rx, ry, rw, rh);
	for(plane=0; plane < 4; plane++) {
		i=PEEE+prh;
		omodexSelectPlane(PLANE(plane-1));
		for(; y < py+rh; y++) {
				_fmemcpy(page->data + (((page->width/4) * (y)) + ((x) / 4)), &(p->plane[plane][i]), prw);
				i+=PE;
		}
		x=px;
		y=py;
	}
}


/*temp*/
void
modexDrawPBuf(page_t *page, int x, int y, planar_buf_t *p, boolean sprite)
{
	modexDrawPBufRegion	(page, x, x, 0, 0, p->width, p->height, p, sprite);
	/*
	sword plane;
	int i;
// 	byte near *buff;
	const int px=x+page[0].dx;
	const int py=y+page[0].dy;
	x=px;
	y=py;
// 	buff = _nmalloc(p->pwidth+1);
	// TODO Make this fast.  It's SLOOOOOOW
// 	for(plane=0; plane < 4; plane++) {
// 		i=0;
// 		modexSelectPlane(PLANE(plane+x));
// 		for(px = plane; px < p->width; px+=4) {
// 			offset=px;
// 			for(py=0; py<p->height/2; py++) {
// 				//SELECT_ALL_PLANES();
// 				if(!sprite || p->plane[offset])
// 					page->data = &(p->plane[offset][i++]);
// 				offset+=p->width;
// 				offset++;
// 			}
// 		}
// 	}
	for(plane=0; plane < 4; plane++) {
		i=0;
		modexSelectPlane(PLANE(plane-1));
		for(; y < py+p->height; y++) {
			//for(px=0; px < p->width; px++) {
				//printf("%02X ", (int) p->plane[plane][i++]);
//				_fmemcpy(buff, &(p->plane[plane][i+=p->pwidth]), p->pwidth);
//				printf("buff %u==%s\n", y, *buff);
//				_fmemcpy(page->data + (((page->width/4) * (y+page[0].dy)) + ((x+page[0].dx) / 4)), buff, p->pwidth);
				_fmemcpy(page->data + (((page->width/4) * y) + (x / 4)), &(p->plane[plane][i+=p->pwidth]), p->pwidth);
			//}
		}
//getch();
		x=px;
		y=py;
	}
// 	_nfree(buff);*/
}

void
oldDrawBmp(byte far* page, int x, int y, bitmap_t *bmp, byte sprite)
{
	byte plane;
	word px, py;
	word offset;

	/* TODO Make this fast.  It's SLOOOOOOW */
	for(plane=0; plane < 4; plane++) {
		omodexSelectPlane(PLANE(plane+x));
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

//* normal versions *//
void
modexDrawBmp(page_t *page, int x, int y, bitmap_t *bmp) {
    /* draw the region (the entire freakin bitmap) */
    modexDrawBmpRegion(page, x, y, 0, 0, bmp->width, bmp->height, bmp);
}

void
modexDrawBmpRegion(page_t *page, int x, int y,
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
	word rowCounter=0;
	byte planeCounter = 4;

    __asm {
		MOV AX, SCREEN_SEG      ; go to the VGA memory
		MOV ES, AX

		MOV DX, SC_INDEX	; point at the map mask register
		MOV AL, SC_MAPMASK	;
		OUT DX, AL	      ;

	PLANE_LOOP:
		MOV DX, SC_DATA	 ; select the current plane
		MOV AL, plane	   ;
		OUT DX, AL	      ;

		;-- begin plane painting
		MOV AX, height	  ; start the row counter
		MOV rowCounter, AX      ;
		MOV DI, poffset	 ; go to the first pixel
		MOV SI, bmpOffset       ; go to the bmp pixel
	ROW_LOOP:
		MOV CX, width	   ; count the columns
	SCAN_LOOP:
		MOVSB		   ; copy the pixel
		SUB CX, 3	       ; we skip the next 3
		ADD SI, 3	       ; skip the bmp pixels
		LOOP SCAN_LOOP	  ; finish the scan

		MOV AX, nextPageRow
		ADD DI, AX	      ; go to the next row on screen
		MOV AX, nextBmpRow
		ADD SI, AX	      ; go to the next row on bmp

		DEC rowCounter
		JNZ ROW_LOOP	    ; do all the rows
		;-- end plane painting
		MOV AL, plane	   ; advance to the next plane
		SHL AL, 1	       ;
		AND AL, 0x0f	    ; mask the plane properly
		MOV plane, AL	   ; store the plane

		INC bmpOffset	   ; start bmp at the right spot

		DEC planeCounter
		JNZ PLANE_LOOP	  ; do all 4 planes
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
	word rowCounter=0;
	byte planeCounter = 4;

    __asm {
		MOV AX, SCREEN_SEG      ; go to the VGA memory
		MOV ES, AX

		MOV DX, SC_INDEX	; point at the map mask register
		MOV AL, SC_MAPMASK	;
		OUT DX, AL	      ;

	PLANE_LOOP:
		MOV DX, SC_DATA	 ; select the current plane
		MOV AL, plane	   ;
		OUT DX, AL	      ;

		;-- begin plane painting
		MOV AX, height	  ; start the row counter
		MOV rowCounter, AX      ;
		MOV DI, poffset	 ; go to the first pixel
		MOV SI, bmpOffset       ; go to the bmp pixel
	ROW_LOOP:
		MOV CX, width	   ; count the columns
	SCAN_LOOP:
		LODSB
		DEC SI
		CMP AL, 0
		JNE DRAW_PIXEL	  ; draw non-zero pixels

		INC DI		  ; skip the transparent pixel
		ADD SI, 1
		JMP NEXT_PIXEL
	DRAW_PIXEL:
		MOVSB		   ; copy the pixel
	NEXT_PIXEL:
		SUB CX, 3	       ; we skip the next 3
		ADD SI, 3	       ; skip the bmp pixels
		LOOP SCAN_LOOP	  ; finish the scan

		MOV AX, nextPageRow
		ADD DI, AX	      ; go to the next row on screen
		MOV AX, nextBmpRow
		ADD SI, AX	      ; go to the next row on bmp

		DEC rowCounter
		JNZ ROW_LOOP	    ; do all the rows
		;-- end plane painting

		MOV AL, plane	   ; advance to the next plane
		SHL AL, 1	       ;
		AND AL, 0x0f	    ; mask the plane properly
		MOV plane, AL	   ; store the plane

		INC bmpOffset	   ; start bmp at the right spot

		DEC planeCounter
		JNZ PLANE_LOOP	  ; do all 4 planes
    }
}

//* planar buffer versions *//
void
modexDrawBmpPBuf(page_t *page, int x, int y, planar_buf_t *bmp) {
    /* draw the region (the entire freakin bitmap) */
    modexDrawBmpPBufRegion(page, x, y, 0, 0, bmp->width, bmp->height, bmp);
}

void
modexDrawBmpPBufRegion(page_t *page, int x, int y,
		   int rx, int ry, int rw, int rh, planar_buf_t *bmp) {
	word poffset = (word) page->data  + y*(page->width/4) + x/4;
	byte *data = bmp->plane[0];
	word bmpOffset = (word) data + ry * bmp->width + rx;
	word width = rw;
	word height = rh;
	byte plane = 1 << ((byte) x & 0x03);
	word scanCount = width/4 + (width%4 ? 1 :0);
	word nextPageRow = page->width/4 - scanCount;
	word nextBmpRow = (word) bmp->width - width;
	word rowCounter=0;
	byte planeCounter = 4;

    __asm {
		MOV AX, SCREEN_SEG      ; go to the VGA memory
		MOV ES, AX

		MOV DX, SC_INDEX	; point at the map mask register
		MOV AL, SC_MAPMASK	;
		OUT DX, AL	      ;

	PLANE_LOOP:
		MOV DX, SC_DATA	 ; select the current plane
		MOV AL, plane	   ;
		OUT DX, AL	      ;

		;-- begin plane painting
		MOV AX, height	  ; start the row counter
		MOV rowCounter, AX      ;
		MOV DI, poffset	 ; go to the first pixel
		MOV SI, bmpOffset       ; go to the bmp pixel
	ROW_LOOP:
		MOV CX, width	   ; count the columns
	SCAN_LOOP:









		MOVSB		   ; copy the pixel

		SUB CX, 3	       ; we skip the next 3
		ADD SI, 3	       ; skip the bmp pixels
		LOOP SCAN_LOOP	  ; finish the scan

		MOV AX, nextPageRow
		ADD DI, AX	      ; go to the next row on screen
		MOV AX, nextBmpRow
		ADD SI, AX	      ; go to the next row on bmp

		DEC rowCounter
		JNZ ROW_LOOP	    ; do all the rows
		;-- end plane painting

		MOV AL, plane	   ; advance to the next plane
		SHL AL, 1	       ;
		AND AL, 0x0f	    ; mask the plane properly
		MOV plane, AL	   ; store the plane

		INC bmpOffset	   ; start bmp at the right spot

		DEC planeCounter
		JNZ PLANE_LOOP	  ; do all 4 planes
    }
}

void
modexDrawSpritePBuf(page_t *page, int x, int y, planar_buf_t *bmp) {
    /* draw the whole sprite */
    modexDrawSpritePBufRegion(page, x, y, 0, 0, bmp->width, bmp->height, bmp);
}

void
modexDrawSpritePBufRegion(page_t *page, int x, int y,
		      int rx, int ry, int rw, int rh, planar_buf_t *bmp) {
	word poffset = (word)page->data + y*(page->width/4) + x/4;
	byte *data = bmp->plane[0];
	word bmpOffset = (word) data + ry * bmp->width + rx;
	word width = rw;
	word height = rh;
	byte plane = 1 << ((byte) x & 0x03);
	word scanCount = width/4 + (width%4 ? 1 :0);
	word nextPageRow = page->width/4 - scanCount;
	word nextBmpRow = (word) bmp->width - width;
	word rowCounter=0;
	byte planeCounter = 4;

    __asm {
		MOV AX, SCREEN_SEG      ; go to the VGA memory
		MOV ES, AX

		MOV DX, SC_INDEX	; point at the map mask register
		MOV AL, SC_MAPMASK	;
		OUT DX, AL	      ;

	PLANE_LOOP:
		MOV DX, SC_DATA	 ; select the current plane
		MOV AL, plane	   ;
		OUT DX, AL	      ;

		;-- begin plane painting
		MOV AX, height	  ; start the row counter
		MOV rowCounter, AX      ;
		MOV DI, poffset	 ; go to the first pixel
		MOV SI, bmpOffset       ; go to the bmp pixel
	ROW_LOOP:
		MOV CX, width	   ; count the columns
	SCAN_LOOP:
		LODSB
		DEC SI
		CMP AL, 0
		JNE DRAW_PIXEL	  ; draw non-zero pixels

		INC DI		  ; skip the transparent pixel
		ADD SI, 1
		JMP NEXT_PIXEL
	DRAW_PIXEL:
		MOVSB		   ; copy the pixel
	NEXT_PIXEL:
		SUB CX, 3	       ; we skip the next 3
		ADD SI, 3	       ; skip the bmp pixels
		LOOP SCAN_LOOP	  ; finish the scan

		MOV AX, nextPageRow
		ADD DI, AX	      ; go to the next row on screen
		MOV AX, nextBmpRow
		ADD SI, AX	      ; go to the next row on bmp

		DEC rowCounter
		JNZ ROW_LOOP	    ; do all the rows
		;-- end plane painting

		MOV AL, plane	   ; advance to the next plane
		SHL AL, 1	       ;
		AND AL, 0x0f	    ; mask the plane properly
		MOV plane, AL	   ; store the plane

		INC bmpOffset	   ; start bmp at the right spot

		DEC planeCounter
		JNZ PLANE_LOOP	  ; do all 4 planes
    }
}
