;-----------------------------------------------------------
;
; MXPI.ASM - Put image
; Copyright (c) 1993,1994 by Alessandro Scotti
;
;-----------------------------------------------------------
WARN    PRO
NOWARN  RES
INCLUDE MODEX.DEF

PUBLIC  mxPutImage

EXTRN   subClipImage            : NEAR

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

EXTRN   mx_VideoSegment : WORD
EXTRN   mx_BytesPerLine : WORD

mxTable LABEL   WORD                    ; Raster ops
        DW      subMove
        DW      subAnd
        DW      subOr
        DW      subXor
        DW      subTrans
        DW      subAdd

;-----------------------------------------------------------
;
; Move functions.
; Note: loops unrolled and optimized for CX even, no check for CX = 0.
;
subMove         PROC    NEAR
        shr     cx, 1                   ; Make CX even
        jc      @@Odd                   ; Special case if odd byte
@@Loop: movsb
        add     si, 3
        movsb
        add     si, 3
        dec     cx
        jnz     @@Loop
@@Exit: ret
@@Odd:  movsb
        add     si, 3
        jcxz    @@Exit
        jmp     @@Loop
subMove         ENDP
;
subAnd          PROC     NEAR
        shr     cx, 1
        jc      @@Odd
@@Loop: mov     al, ds:[si]
        mov     ah, ds:[si+4]
        and     es:[di], ax
        inc     di
        inc     di
        add     si, 8
        dec     cx
        jnz     @@Loop
@@Exit: ret
@@Odd:  lodsb
        and     es:[di], al
        inc     di
        add     si, 3
        jcxz    @@Exit
        jmp     @@Loop
subAnd          ENDP
;
subOr           PROC     NEAR
        shr     cx, 1
        jc      @@Odd
@@Loop: mov     al, ds:[si]
        mov     ah, ds:[si+4]
        or      es:[di], ax
        inc     di
        inc     di
        add     si, 8
        dec     cx
        jnz     @@Loop
@@Exit: ret
@@Odd:  lodsb
        or      es:[di], al
        inc     di
        add     si, 3
        jcxz    @@Exit
        jmp     @@Loop
subOr           ENDP
;
subXor          PROC     NEAR
        shr     cx, 1
        jc      @@Odd
@@Loop: mov     al, ds:[si]
        mov     ah, ds:[si+4]
        xor     es:[di], ax
        inc     di
        inc     di
        add     si, 8
        dec     cx
        jnz     @@Loop
@@Exit: ret
@@Odd:  lodsb
        xor     es:[di], al
        inc     di
        add     si, 3
        jcxz    @@Exit
        jmp     @@Loop
subXor          ENDP
;
subTrans        PROC     NEAR
@@Loop: mov     al, ds:[si]
        cmp     al, ah
        je      @@Skip
        mov     es:[di], al
@@Skip: inc     di
        add     si, 4
        dec     cx
        jnz     @@Loop
@@Exit: ret
subTrans        ENDP
;
subAdd          PROC     NEAR
@@Loop: mov     al, ds:[si]
        add     es:[di], al
        inc     di
        add     si, 4
        dec     cx
        jnz     @@Loop
        ret
subAdd          ENDP

;-----------------------------------------------------------
;
; Copies a "raw" image from memory to screen.
;
; Input:
;       Image   = pointer to image
;       X, Y    = coordinates of destination
;       Width   = width of image in pixels
;       Height  = height of image in pixels
;       Op      = raster op (OP_xxx)
; Output:
;       none
;
mxPutImage      PROC    FAR
        ARG     Op:WORD,                \
                Height:WORD,            \
                Width:WORD,             \
                Y:WORD,                 \
                X:WORD,                 \
                Image:DWORD             = ARG_SIZE
        LOCAL   PlaneWidth:WORD:4,      \
                PixelOffset:WORD,       \
                MoveFunction:WORD,      \
                Count:BYTE,             \
                ReadPlane:BYTE,         \
                OpInfo:BYTE,            \
                WritePlane:BYTE         = AUTO_SIZE
        ASSUME  ds:NOTHING
        .enter  AUTO_SIZE
        .push   ds, si, es, di

; Clip image
        mov     bx, [X]
        mov     ax, [Y]
        mov     cx, [Width]
        mov     dx, [Height]
        call    subClipImage
        jc      @@Exit                  ; Full clipped
        mov     [Height], dx
        add     WORD PTR Image[0], si   ; Skip clipped pixels

; Get pixel address
        mul     [mx_BytesPerLine]
        mov     di, bx
        shr     di, 1
        shr     di, 1
        add     di, ax
        mov     [PixelOffset], di
        mov     es, [mx_VideoSegment]   ; ES:DI points to pixel
        and     bl, 03h
        mov     [ReadPlane], bl

; Compute extra bytes and width count for each plane
        mov     bx, cx
        shr     bx, 1
        shr     bx, 1                   ; Width for each plane
        and     cl, 03h
        mov     al, 00001000b
        shr     al, cl
        mov     si, 3 SHL 1
@@PatchLoop:
        mov     PlaneWidth[si], bx
        shr     al, 1
        adc     bx, 0
        dec     si
        dec     si
        jge     @@PatchLoop

; Setup planes for output to VGA registers
        mov     cl, [ReadPlane]
        mov     al, 00010001b
        shl     al, cl
        mov     [WritePlane], al

; Install move function
        mov     bx, [Op]
        mov     [OpInfo], bh            ; Remember additional info if needed
        xor     bh, bh
        cmp     bl, OP_ADD
        jbe     @@SetMoveFunction
        xor     bl, bl
@@SetMoveFunction:
        shl     bx, 1
        mov     ax, mxTable[bx]
        mov     [MoveFunction], ax

; Put image
        cld
        mov     [Count], 4              ; Four planes
        lea     bx, PlaneWidth          ; SS:[BX] = width in bytes for plane
        mov     ds, WORD PTR Image[2]
@@PlaneLoop:
        cmp     WORD PTR ss:[bx], 0     ; Exit if nothing more to do
        je      @@Exit                  ; (also, never try to move zero bytes!)
        mov     si, WORD PTR Image[0]
        mov     ah, [WritePlane]
        and     ah, 0Fh
        mov     al, 02h
        mov     dx, TS
        out     dx, ax                  ; Select write plane
        mov     ah, [ReadPlane]
        and     ah, 03h
        mov     al, 04h
        mov     dx, GDC
        out     dx, ax                  ; Select read plane
        mov     dx, [Height]
        mov     di, [PixelOffset]
@@Loop:
        push    si
        push    di
        mov     cx, WORD PTR ss:[bx]    ; Number of bytes to move
        mov     ah, [OpInfo]            ; Transparent color for subTrans
        call    [MoveFunction]
        pop     di
        pop     si
        add     si, [Width]             ; Go to next image line
        add     di, [mx_BytesPerLine]   ; Go to next screen row
        dec     dx
        jnz     @@Loop                  ; Repeat for all lines
        inc     bx
        inc     bx                      ; Select width for next plane
        inc     [ReadPlane]
        rol     [WritePlane], 1
        adc     [PixelOffset], 0
        inc     WORD PTR Image[0]
        dec     [Count]
        jnz     @@PlaneLoop             ; Repeat for all planes

@@Exit:
        xor     ax, ax
        .pop    ds, si, es, di
        .leave  ARG_SIZE
mxPutImage      ENDP

MX_TEXT         ENDS
END
