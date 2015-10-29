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

    __asm {
		MOV AX, SCREEN_SEG      ; go to the VGA memory
		MOV ES, AX

		MOV DX, SC_INDEX	; point at the map mask register
		MOV AL, MAP_MASK	;
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
	word rowCounter;
	byte planeCounter = 4;

    __asm {
		MOV AX, SCREEN_SEG      ; go to the VGA memory
		MOV ES, AX

		MOV DX, SC_INDEX	; point at the map mask register
		MOV AL, MAP_MASK	;
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
