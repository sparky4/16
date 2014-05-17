;-----------------------------------------------------------
;
; MXPT.ASM - Convex polygon fill with texture mapping
; Copyright (c) 1994 by Alessandro Scotti
;
;-----------------------------------------------------------
;WARN    PRO
;NOWARN  RES
INCLUDE MODEX.DEF

PUBLIC  mxTexturePoly

;-----------------------------------------------------------
;
; "Local" definitions
;
TPOINT  STRUC
        X       DW      ?
        Y       DW      ?
TPOINT  ENDS

; Do NOT change order!
TSCAN   STRUC
        Y1      DW      ?
	IX1	DW	?
	IY1	DW	?
        Y2      DW      ?
        IX2     DW      ?
        IY2     DW      ?
TSCAN   ENDS

MAXSCANCOLUMNS  EQU     POLYSCANBUFSIZE / SIZE TSCAN

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

EXTRN   mx_VideoSegment : WORD
EXTRN   mx_CodeSegment  : WORD
EXTRN   mx_BytesPerLine : WORD
EXTRN   mx_ClipX1       : WORD
EXTRN   mx_ClipY1       : WORD
EXTRN   mx_ClipX2       : WORD
EXTRN   mx_ClipY2       : WORD
EXTRN   mx_ScanBuffer   : NEAR

; Global variables, to optimize allocation of local space
mx_Texture	DD	?
mx_TextureWidth DW	?
mx_XStepLo      DW      ?
mx_XStepHi      DW      ?
mx_XBump        DW      ?
mx_YStepLo      DW      ?
mx_YStepHi      DW      ?
mx_YBump        DW      ?
mx_TempX        DW      512 DUP(?)
mx_TempY        DW      512 DUP(?)

;-----------------------------------------------------------
;
; Scans an edge using the DDA (digital differential analyzer) algorithm.
; Also saves information for the texture scanner.
;
; Input:
;       DS:BX   = pointer to start point (X1, Y1)
;       DS:SI   = pointer to end point (X2, Y2)
;       ES:DI   = pointer to edge buffer
; Output:
;       ES:DI   = updated pointer to edge buffer
;
subScan         PROC NEAR
        mov     cx, ds:[si].X
        sub     cx, ds:[bx].X           ; Get width
        mov     es:[di].IX1, cx         ; Save width
        jg      @@1
        ret
@@1:
        push    bp                      ; Save BP

        mov     ax, ds:[si].Y
        mov     bx, ds:[bx].Y
        sub     ax, bx                  ; Get height
        jg      @@T2B                   ; Scan top to bottom
        jl      @@B2T                   ; Scan bottom to top

; Special case: vertical line
        mov     ax, bx
@@V:
        mov     es:[di].Y1, ax
        add     di, SIZE TSCAN
        dec     cx
        jnz     @@V
	jmp	@@Exit

; Scan top to bottom
@@T2B:
        cwd
        div     cx
        mov     bp, ax
        xor     ax, ax
        div     cx
        xchg    ax, bx                  ; BP:BX = fixed 16:16 step
        mov     dx, 8000h
@@T2BLoop:
        mov     es:[di].Y1, ax
        add     di, SIZE TSCAN
        add     dx, bx
        adc     ax, bp
        dec     cx
        jnz     @@T2BLoop
	jmp	@@Exit

; Scan bottom to top
@@B2T:
        neg     ax
        cwd
        div     cx
        mov     bp, ax
        xor     ax, ax
        div     cx
        xchg    ax, bx
        mov     dx, 8000h
@@B2TLoop:
        mov     es:[di].Y1, ax
        add     di, SIZE TSCAN
        sub     dx, bx
        sbb     ax, bp
        dec     cx
        jnz     @@B2TLoop

@@Exit:
        pop     bp
        ret
subScan         ENDP

;-----------------------------------------------------------
;
; Scans a texture edge.
;
; Input:
;       DS:BX   = pointer to start point (X1, Y1)
;       DS:SI   = pointer to end point (X2, Y2)
;       ES:DI   = pointer to edge buffer
;       CX      = number of steps
; Output:
;       ES:DI   = updated pointer to edge buffer
;
subScanTexture	PROC NEAR
	.push	bp
        .push   di, bx, cx              ; Save registers
        mov     ax, ds:[si].X           ; Scan X coordinate first
	mov	bx, ds:[bx].X
	sub	ax, bx
	jg	@@XP			; Positive
	jl	@@XN			; Negative

; Special case: constant
@@XCLoop:
	mov	es:[di].IX1, bx
	add	di, SIZE TSCAN
	dec	cx
	jnz	@@XCLoop
	jmp	@@ScanY

; Increasing step
@@XP:
	cwd
	div	cx
	mov	bp, ax
	xor	ax, ax
	div	cx
	xchg	ax, bx
	mov	dx, 8000h
@@XPLoop:
	mov	es:[di].IX1, ax
        add     di, SIZE TSCAN
        add     dx, bx
        adc     ax, bp
        dec     cx
	jnz	@@XPLoop
	jmp	@@ScanY

; Decreasing step
@@XN:
        neg     ax
        cwd
        div     cx
        mov     bp, ax
        xor     ax, ax
        div     cx
        xchg    ax, bx
        mov     dx, 8000h
@@XNLoop:
	mov	es:[di].IX1, ax
        add     di, SIZE TSCAN
        sub     dx, bx
        sbb     ax, bp
        dec     cx
	jnz	@@XNLoop

; Now scan the Y coordinate
@@ScanY:
        .pop    di, bx, cx              ; Restore registers
	mov	ax, ds:[si].Y
	mov	bx, ds:[bx].Y
	sub	ax, bx
	jg	@@YP			; Positive
	jl	@@YN			; Negative

; Special case: constant
	mov	ax, cx
@@YCLoop:
	mov	es:[di].IY1, bx
	add	di, SIZE TSCAN
	dec	ax
	jnz	@@YCLoop
	jmp	@@Exit

; Increasing step
@@YP:
	cwd
	div	cx
	mov	bp, ax
	xor	ax, ax
	div	cx
	xchg	ax, bx
	mov	dx, 8000h
@@YPLoop:
	mov	es:[di].IY1, ax
        add     di, SIZE TSCAN
        add     dx, bx
        adc     ax, bp
        dec     cx
	jnz	@@YPLoop
	jmp	@@Exit

; Decreasing step
@@YN:
        neg     ax
        cwd
        div     cx
        mov     bp, ax
        xor     ax, ax
        div     cx
        xchg    ax, bx
        mov     dx, 8000h
@@YNLoop:
	mov	es:[di].IY1, ax
        add     di, SIZE TSCAN
        sub     dx, bx
        sbb     ax, bp
        dec     cx
	jnz	@@YNLoop

@@Exit:
	.pop	bp
        ret
subScanTexture	ENDP

;-----------------------------------------------------------
;
; Fills a scan column with texture.
;
; Input:
;       DS:SI   = current TSCAN
;       ES:DI   = address of top pixel
;       CX      = number of pixels to write
; Output:
;       none
; Notes:
;       must preserve DS:SI, texture info in global variables.
;
subFillScan     PROC NEAR
        ASSUME  ds:NOTHING
        .push   ds, si, bp, cx

        movzx   ecx, cx                 ; Clear high word of ECX

        movzx   eax, ds:[si].IX2
        movzx   ebx, ds:[si].IX1
        sub     ax, bx
        jge     @@XP

;---------------------------------------
; Delta X is negative
@@XN:
        neg     ax                      ; Make AX positive
        shl     eax, 8                  ; Convert 8:8 fixed to 16:16
        cdq
        idiv    ecx
        ror     eax, 16
        mov     ebp, eax                ; Save X step into EBP

        movzx   eax, ds:[si].IY2
        movzx   esi, ds:[si].IY1
        sub     ax, si
        jge     @@XNYP

;-----------------------
; DeltaX < 0, DeltaY < 0
@@XNYN:
        neg     ax
        shl     eax, 8
        cdq
        idiv    ecx
        ror     eax, 16
        mov     dx, [mx_TextureWidth]
        mul     dl
        xchg    eax, esi                ; ESI = Y step
        ror     eax, 8
        mul     dl
        ror     ebx, 8
        add     bx, ax                  ; BX = start offset in texture
        lds     dx, [mx_Texture]
        movzx   edx, dx
        add     edx, ebx                ; EDX = X step lo:texture offset
        xchg    edx, esi                ; EDX = Y step
@@XNYNLoop:
        dec     cx
        jl      @@XNYNDone
        movzx   ax, ds:[si]
        mov     es:[di], al
        add     di, [mx_BytesPerLine]
        sub     esi, ebp
        sbb     si, 0
        sub     si, dx
        sub     eax, edx
        jnc     @@XNYNLoop
        sub     si, [mx_TextureWidth]
        jmp     @@XNYNLoop
@@XNYNDone:
        jmp     @@Exit

;-----------------------
; DeltaX < 0, DeltaY > 0
@@XNYP:
        shl     eax, 8
        cdq
        idiv    ecx
        ror     eax, 16
        mov     dx, [mx_TextureWidth]
        mul     dl
        xchg    eax, esi                ; ESI = Y step
        ror     eax, 8
        mul     dl
        ror     ebx, 8
        add     bx, ax                  ; BX = start offset in texture
        lds     dx, [mx_Texture]
        movzx   edx, dx
        add     edx, ebx                ; EDX = X step lo:texture offset
        xchg    edx, esi                ; EDX = Y step
@@XNYPLoop:
        dec     cx
        jl      @@XNYPDone
        movzx   ax, ds:[si]
        mov     es:[di], al
        add     di, [mx_BytesPerLine]
        sub     esi, ebp
        sbb     si, 0
        add     si, dx
        add     eax, edx
        jnc     @@XNYPLoop
        add     si, [mx_TextureWidth]
        jmp     @@XNYPLoop
@@XNYPDone:
        jmp     @@Exit

;---------------------------------------
; Delta X is positive
@@XP:
        shl     eax, 8                  ; Convert 8:8 fixed to 16:16
        cdq
        idiv    ecx
        ror     eax, 16
        mov     ebp, eax                ; Save X step into EBP

        movzx   eax, ds:[si].IY2
        movzx   esi, ds:[si].IY1
        sub     ax, si
        jge     @@XPYP

;-----------------------
; DeltaX > 0, DeltaY < 0
@@XPYN:
        neg     ax
        shl     eax, 8
        cdq
        idiv    ecx
        ror     eax, 16
        mov     dx, [mx_TextureWidth]
        mul     dl
        xchg    eax, esi                ; ESI = Y step
        ror     eax, 8
        mul     dl
        ror     ebx, 8
        add     bx, ax                  ; BX = start offset in texture
        lds     dx, [mx_Texture]
        movzx   edx, dx
        add     edx, ebx                ; EDX = X step lo:texture offset
        xchg    edx, esi                ; EDX = Y step
@@XPYNLoop:
        dec     cx
        jl      @@XPYNDone
        movzx   ax, ds:[si]
        mov     es:[di], al
        add     di, [mx_BytesPerLine]
        add     esi, ebp
        adc     si, 0
        sub     si, dx
        sub     eax, edx
        jnc     @@XPYNLoop
        sub     si, [mx_TextureWidth]
        jmp     @@XPYNLoop
@@XPYNDone:
        jmp     @@Exit

;-----------------------
; DeltaX > 0, DeltaY > 0
@@XPYP:
        shl     eax, 8
        cdq
        idiv    ecx
        ror     eax, 16
        mov     dx, [mx_TextureWidth]
        mul     dl
        xchg    eax, esi                ; ESI = Y step
        ror     eax, 8
        mul     dl
        ror     ebx, 8
        add     bx, ax                  ; BX = start offset in texture
        lds     dx, [mx_Texture]
        movzx   edx, dx
        add     edx, ebx                ; EDX = X step lo:texture offset
        xchg    edx, esi                ; EDX = Y step
@@XPYPLoop:
        dec     cx
        jl      @@XPYPDone
        movzx   ax, ds:[si]
        mov     es:[di], al
        add     di, [mx_BytesPerLine]
        add     esi, ebp
        adc     si, 0
        add     si, dx
        add     eax, edx
        jnc     @@XPYPLoop
        add     si, [mx_TextureWidth]
        jmp     @@XPYPLoop
@@XPYPDone:
        jmp     @@Exit

@@Exit:
        .pop    ds, si, bp, cx
        ASSUME  ds:NOTHING
        ret
subFillScan     ENDP

;-----------------------------------------------------------
;
; Fills a convex polygon with the specified color.
; Interpolates pixel colors using the Gouraud algorithm.
;
; Input:
;       Count   = number of vertexes
;       Map     = indexes of points and colors (integer)
;       Points  = array of points (integer X, Y coordinates)
;	TPoints = array of points inside texture (fixed 8:8 coordinates)
;	Texture = pointer to texture image
;	TWidth	= texture width in pixels
; Output:
;       none
; Notes:
;       vertexes must be in counterclockwise order, arrays are 0-based.
;
mxTexturePoly	PROC FAR
        ARG     TWidth:WORD,    \
		Texture:DWORD,	\
		TPoints:DWORD,	\
                Points:DWORD,   \
                Map:DWORD,      \
                Count:WORD      = ARG_SIZE
        LOCAL   WritePlane:BYTE:2,      \
                ScanOffsetT:WORD,       \
                ScanOffsetB:WORD,       \
                ScanCount:WORD,         \
                Holder:WORD,            \
                Height:WORD,            \
                MinIdxT:WORD,           \
                MinIdxB:WORD,           \
		MaxIdx:WORD,		\
                Width:WORD,             \
                BoxX1:WORD,             \
                BoxY1:WORD,             \
                BoxX2:WORD,             \
                BoxY2::WORD             = AUTO_SIZE
        .enter  AUTO_SIZE
        .push   ds, si, es, di
        ASSUME  ds:NOTHING

IF USE386 EQ FALSE
        jmp     @@Exit
ENDIF

; Check that at least three vertexes are specified
        mov     cx, [Count]
        cmp     cx, 3
        jb      @@Exit

;------------------------------
; Find bounding box for polygon
        les     di, [Map]               ; Point to map table
        lds     si, [Points]            ; Point to vertex array
        mov     [BoxX1], 32767
        mov     [BoxX2], -32768
        mov     [BoxY1], 32767
        mov     [BoxY2], -32768

        xor     dx, dx
@@MinMaxLoop:
        mov     bx, es:[di]             ; Get index of vertex
        .shl    bx, 2                   ; Get offset in point array
        add     bx, si

; Check X range
@@CheckMinX:
        mov     ax, ds:[bx].X           ; Get X coordinate
        cmp     ax, [BoxX1]
        jge     @@CheckMaxX
        mov     [BoxX1], ax
        mov     [MinIdxT], dx
        mov     [MinIdxB], dx
@@CheckMaxX:
        cmp     ax, [BoxX2]
        jle     @@CheckMinY
        mov     [BoxX2], ax
	mov	[MaxIdx], dx

; Check Y range
@@CheckMinY:
        mov     ax, ds:[bx].Y
        cmp     ax, [BoxY1]
        jge     @@CheckMaxY
        mov     [BoxY1], ax
@@CheckMaxY:
        cmp     ax, [BoxY2]
        jle     @@CheckDone
        mov     [BoxY2], ax

; Repeat thru all points
@@CheckDone:
        add     di, 2                   ; Next map entry
        add     dx, 2
        dec     cx
        jnz     @@MinMaxLoop

;---------------------------------
; Check if polygon is full clipped
        mov     ax, [BoxX2]
        cmp     ax, [mx_ClipX1]         ; Is poly full clipped?
        jl      @@Exit
        mov     bx, [BoxX1]
        cmp     bx, [mx_ClipX2]         ; Is poly full clipped?
        jg      @@Exit
        sub     ax, bx                  ; Get width
        jle     @@Exit                  ; Exit if not positive
        mov     ax, [BoxY2]
        cmp     ax, [mx_ClipY1]         ; Is poly full clipped?
        jl      @@Exit
        mov     bx, [BoxY1]
        cmp     bx, [mx_ClipY2]         ; Is poly full clipped?
        jg      @@Exit
        sub     ax, bx                  ; Get height
        jle     @@Exit                  ; Exit if not positive

	dec	[Count]
        shl     [Count], 1              ; We'll work with word offsets

;----------------------
; Scan top polygon edge
        mov     es, [mx_CodeSegment]
	mov	di, OFFSET mx_ScanBuffer
	mov	si, [MinIdxT]		; Offset of top point index
@@STLoop:
        mov     dx, si
        lds     bx, [Map]               ; DS:BX -> map table
	mov	ax, ds:[bx+si]		; Index of top point #1
        dec     si                      ; Next top point
        dec     si
        test    si, si
        jns     @@ST1
        mov     si, [Count]
@@ST1:
	mov	[MinIdxT], si		; Save new index of bottom point
        mov     cx, si
	mov	si, ds:[bx+si]		; Get index of top point #2
	.shl	ax, 2			; Convert indexes to offsets
	.shl	si, 2
        shl     cx, 1                   ; Save map points
        shl     dx, 1
        mov     dh, cl
        mov     es:[di].IY1, dx
	lds	bx, [Points]		; Get pointer to vertexes
	add	si, bx
	add	bx, ax
	call	subScan 		; Scan poly top edge
	mov	si, [MinIdxT]
	cmp	si, [MaxIdx]		; Reached last vertex?
	jne	@@STLoop		; No, continue
        mov     es:[di].IX1, -1         ; Mark end of scan

;-------------------------
; Scan bottom polygon edge
	mov	di, OFFSET mx_ScanBuffer + OFFSET Y2
	mov	si, [MinIdxB]
@@SBLoop:
        mov     dx, si
        lds     bx, [Map]               ; DS:BX -> map table
	mov	ax, ds:[bx+si]		; Index of bottom point #1
        inc     si                      ; Next bottom point
        inc     si
        cmp     si, [Count]
        jbe     @@SB1
        xor     si, si
@@SB1:
	mov	[MinIdxB], si		; Save new index of bottom point
        mov     cx, si
        mov     si, ds:[bx+si]          ; Get index of bottom point #2
	.shl	ax, 2			; Convert indexes to offsets
	.shl	si, 2
        shl     cx, 1                   ; Save map points
        shl     dx, 1
        mov     dh, cl
        mov     es:[di].IY1, dx
        lds     bx, [Points]            ; Get pointer to vertexes
	add	si, bx
	add	bx, ax
	call	subScan 		; Scan poly bottom edge
	mov	si, [MinIdxB]
	cmp	si, [MaxIdx]		; Reached last vertex?
	jne	@@SBLoop		; No, continue
        mov     es:[di].IX1, -1         ; Mark end of scan

;----------------------
; Scan top texture edge
	mov	di, OFFSET mx_ScanBuffer
        mov     ds, WORD PTR [TPoints][2]
@@TTLoop:
        mov     cx, es:[di].IX1
        test    cx, cx
        jle     @@TTDone
        mov     ax, es:[di].IY1
        mov     dl, ah
        xor     ah, ah
        xor     dh, dh
        mov     si, dx
        mov     bx, WORD PTR [TPoints][0]
	add	si, bx
	add	bx, ax
	call	subScanTexture		; Scan texture top edge
        jmp     @@TTLoop
@@TTDone:

;-------------------------
; Scan bottom texture edge
	mov	di, OFFSET mx_ScanBuffer + OFFSET Y2
@@TBLoop:
        mov     cx, es:[di].IX1
        test    cx, cx
        jle     @@TBDone
        mov     ax, es:[di].IY1
        mov     dl, ah
        xor     ah, ah
        xor     dh, dh
        mov     si, dx
        mov     bx, WORD PTR [TPoints][0]
	add	si, bx
	add	bx, ax
	call	subScanTexture		; Scan texture top edge
        jmp     @@TBLoop
@@TBDone:

;--------------------
; Clip left and right
        mov     si, OFFSET mx_ScanBuffer
        mov     ax, [BoxX1]
        mov     cx, [BoxX2]
        sub     cx, ax
        mov     bx, [mx_ClipX1]         ; CX = bounding box width
        sub     bx, ax
        jle     @@ClipL1                ; No need to clip left
        sub     cx, bx
        add     ax, bx                  ; BoxX1 = mx_ClipX1
        mov     [BoxX1], ax
        .shl    bx, 2                   ; Warning!!! This is an hand-coded
        add     si, bx                  ; multiply by the size of TSCAN
        shl     bx, 1
        add     si, bx
@@ClipL1:
        mov     bx, ax
        add     bx, cx                  ; Last scan column
        sub     bx, [mx_ClipX2]
        jle     @@ClipL2                ; No need to clip right
        sub     cx, bx                  ; Clip right
@@ClipL2:
        test    cx, cx                  ; Is clipped width positive?
        jle     @@Exit                  ; No, exit
        mov     [ScanCount], cx         ; Save number of columns to draw
        mov     [ScanOffsetT], si       ; Remember offset of (clipped) buffer
        mov     ds, [mx_CodeSegment]    ; DS:SI -> scan buffer

;------------------------------
; Check if Y clipping is needed
        mov     ax, [BoxY1]
        cmp     ax, [mx_ClipY1]
        jl      @@ClipTB                ; Need to clip top
        mov     ax, [BoxY2]
        cmp     ax, [mx_ClipY2]
        jg      @@ClipTB                ; Need to clip bottom
        jmp     @@ClipYExit             ; Skip Y clipping

;--------------------
; Clip top and bottom
@@ClipTB:
        mov     di, cx                  ; DI = scan count
        inc     di                      ; Increment count for pre-loop test
        sub     si, SIZE TSCAN
@@ClipYLoop:
        dec     di                      ; Any column left?
        jz      @@ClipYExit             ; No, exit
        add     si, SIZE TSCAN
        mov     ax, ds:[si].Y1          ; Y1
        mov     cx, ds:[si].Y2          ; Y2
        mov     dx, [mx_ClipY2]
        cmp     ax, dx                  ; Full clipped?
        jg      @@ClipYClip             ; Yes, skip this column
        cmp     cx, dx                  ; Need to clip bottom?
        jle     @@ClipY1                ; No, continue
; Clip bottom, update texture too
        mov     ds:[si].Y2, dx
        mov     bx, cx
        sub     bx, dx                  ; Number of pixels to clip
        sub     cx, ax                  ; Height
        jle     @@ClipYClip
        mov     ax, ds:[si].IX1         ; Update texture
        sub     ax, ds:[si].IX2
        imul    bx
        idiv    cx
        add     ds:[si].IX2, ax
        mov     ax, ds:[si].IY1
        sub     ax, ds:[si].IY2
        imul    bx
        idiv    cx
        add     ds:[si].IY2, ax
        mov     ax, ds:[si].Y1          ; Reload coordinates
        mov     cx, ds:[si].Y2          ;
@@ClipY1:
        mov     dx, [mx_ClipY1]
        cmp     cx, dx                  ; Full top clipped?
        jl      @@ClipYClip             ; Yes, skip
        sub     cx, ax                  ; Get height
        jle     @@ClipYClip             ; Skip if not positive
        cmp     ax, dx                  ; Need to clip top?
        jge     @@ClipYLoop             ; No, continue
; Clip top, update texture too
        mov     ds:[si].Y1, dx          ; Y1 = mx_ClipY1
        sub     dx, ax                  ; DX = number of pixels clipped
        cmp     cx, dx
        jbe     @@ClipYClip             ; Full clipped, skip
        mov     bx, dx                  ; Save pixel count
        mov     ax, ds:[si].IX2
        sub     ax, ds:[si].IX1
        imul    bx
        idiv    cx
        add     ds:[si].IX1, ax
        mov     ax, ds:[si].IY2
        sub     ax, ds:[si].IY1
        imul    bx
        idiv    cx
        add     ds:[si].IY1, ax
        jmp     @@ClipYLoop
@@ClipYClip:
        mov     ds:[si].Y1, -1          ; Mark column as clipped
        jmp     @@ClipYLoop
@@ClipYExit:

;-------------
; Draw columns
        mov     es, [mx_VideoSegment]
        mov     ax, [TWidth]
        mov     ds:[mx_TextureWidth], ax
        mov     ax, WORD PTR [Texture][0]
        mov     dx, WORD PTR [Texture][2]
        mov     WORD PTR ds:[mx_Texture][0], ax
        mov     WORD PTR ds:[mx_Texture][2], dx
        mov     si, [ScanOffsetT]
        mov     cl, BYTE PTR [BoxX1]    ; Init write plane
        and     cl, 03h
        mov     al, 11h
        shl     al, cl
        mov     [WritePlane], al
        .shr    [BoxX1], 2
@@DrawLoop:
        mov     ax, ds:[si].Y1
        test    ax, ax                  ; Was column clipped?
        js      @@DrawNext              ; Yes, skip
        mov     cx, ds:[si].Y2
        sub     cx, ax                  ; CX = height
        jle     @@DrawNext
        mul     [mx_BytesPerLine]       ; Get pixel address
        add     ax, [BoxX1]
        mov     di, ax
        mov     ah, [WritePlane]
        mov     al, 02h
        mov     dx, TS
        out     dx, ax
        call    subFillScan
@@DrawNext:
        rol     [WritePlane], 1
        adc     [BoxX1], 0              ; Bump pointer to video memory if needed
        add     si, SIZE TSCAN
        dec     [ScanCount]
        jnz     @@DrawLoop

@@Exit:
        xor     ax, ax
        .pop    ds, si, es, di
        .leave  ARG_SIZE
mxTexturePoly	ENDP

MX_TEXT         ENDS
END
