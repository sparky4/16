;-----------------------------------------------------------
;
; MXPI.ASM - Stretch image
; Copyright (c) 1994 by Alessandro Scotti
;
;-----------------------------------------------------------
WARN    PRO
NOWARN  RES
INCLUDE MODEX.DEF

PUBLIC  mxStretchImage

EXTRN   subClipBox              : NEAR

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
; Stretches and copies a "raw" image from memory to screen.
;
; Input:
;       Image     = pointer to image
;       X, Y      = coordinates of destination
;       Width     = width of image in pixels
;       Height    = height of image in pixels
;       NewWidth  = new width of image in pixels
;       NewHeight = new height of image in pixels
;       Op        = raster op (OP_xxx)
; Output:
;       none
;
mxStretchImage  PROC    FAR
        ARG     Op:WORD,                \
                NewHeight:WORD,         \
                NewWidth:WORD,          \
                Height:WORD,            \
                Width:WORD,             \
                Y:WORD,                 \
                X:WORD,                 \
                Image:DWORD             = ARG_SIZE
        LOCAL   PixelOffset:WORD,       \
                MoveFunction:WORD,      \
                ReadPlane:BYTE,         \
                OpInfo:BYTE,            \
                WidthStep:DWORD,        \
                HeightStep:DWORD,       \
                ImageLo:WORD,           \
                WritePlane:BYTE         = AUTO_SIZE
        ASSUME  ds:NOTHING
        .enter  AUTO_SIZE
        .push   ds, si, es, di

; Get width stretch factor
    IF USE386 EQ TRUE
        movzx   edx, [Width]
        xor     eax, eax
        movzx   ebx, [NewWidth]
        shl     ebx, 16
        idiv    ebx
        mov     [WidthStep], eax
    ELSE
        xor     dx, dx                  ; Width stretch factor
        mov     ax, [Width]
        mov     bx, [NewWidth]
        div     bx
        mov     WORD PTR WidthStep[2], ax
        xor     ax, ax
        div     bx
        mov     WORD PTR WidthStep[0], ax
    ENDIF
; Get height stretch factor
    IF USE386 EQ TRUE
        movzx   edx, [Height]
        xor     eax, eax
        movzx   ebx, [NewHeight]
        shl     ebx, 16
        idiv    ebx
        mov     [HeightStep], eax
    ELSE
        xor     dx, dx
        mov     ax, [Height]
        mov     bx, [NewHeight]
        div     bx
        mov     WORD PTR HeightStep[2], ax
        xor     ax, ax
        div     bx
        mov     WORD PTR HeightStep[0], ax
   ENDIF

; Clip image
        mov     bx, [X]
        mov     ax, [Y]
        mov     cx, [NewWidth]
        mov     dx, [NewHeight]
        call    subClipBox
        jc      @@Exit                  ; Full clipped
        mov     [NewWidth], cx
        mov     [NewHeight], dx
        sub     [X], bx
        sub     [Y], ax

; Get pixel address
        mul     [mx_BytesPerLine]
        mov     di, bx
        shr     di, 1
        shr     di, 1
        add     di, ax
        mov     [PixelOffset], di
        mov     es, [mx_VideoSegment]   ; ES:DI points to pixel
        and     bl, 03h
        mov     [ReadPlane], bl         ; Set read plane
        mov     cl, bl
        mov     al, 00010001b
        shl     al, cl
        mov     [WritePlane], al        ; Set write plane

; Relocate image origin if previously clipped
        mov     ax, [Y]
        test    ax, ax
        jz      @@OriginYDone
    IF USE386 EQ TRUE
        shl     eax, 16
        imul    [HeightStep]
        mov     ax, [Width]
        mul     dx
    ELSE
        mov     bx, ax
        mul     WORD PTR HeightStep[0]
        mov     cx, dx
        mov     ax, bx
        mul     WORD PTR HeightStep[2]
        add     ax, cx
        mul     [Width]
    ENDIF
        add     WORD PTR [Image], ax
@@OriginYDone:
        mov     ax, [X]
        test    ax, ax
        jz      @@OriginXDone
    IF USE386 EQ TRUE
        shl     eax, 16
        imul    [WidthStep]
        add     WORD PTR [Image], dx
    ELSE
        mov     bx, ax
        mul     WORD PTR WidthStep[0]
        mov     cx, dx
        mov     ax, bx
        mul     WORD PTR WidthStep[2]
        add     ax, cx
        add     WORD PTR [Image], ax
    ENDIF
@@OriginXDone:
        mov     ax, WORD PTR HeightStep[2]
        mul     [Width]
        mov     WORD PTR HeightStep[2], ax

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
        mov     ds, WORD PTR Image[2]
        xor     ax, ax
        mov     [ImageLo], ax
@@Loop:
        mov     si, WORD PTR Image[0]   ; Get pointer to image
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
        mov     cx, [NewHeight]
        mov     di, [PixelOffset]       ; ES:DI points to video memory
        mov     ah, [OpInfo]            ; Additional raster op info
        xor     bx, bx
        mov     dx, [mx_BytesPerLine]
        call    [MoveFunction]          ; Draw column
        inc     [ReadPlane]             ; Next read plane
        rol     [WritePlane], 1         ; Next write plane
        adc     [PixelOffset], 0        ; Update video offset if needed
        mov     dx, WORD PTR WidthStep[0]
        mov     ax, WORD PTR WidthStep[2]
        add     [ImageLo], dx
        adc     WORD PTR Image[0], ax   ; Next image column
        dec     [NewWidth]
        jnz     @@Loop                  ; Repeat for all columns

@@Exit:
        xor     ax, ax
        .pop    ds, si, es, di
        .leave  ARG_SIZE

;-----------------------------------------------------------
;
; Move functions, on entry:
;       AH = additional raster op info (e.g. transparent color)
;       BX = 0,
;       CX = pixel count,
;       DX = mx_BytesPerLine.
;
subMove         PROC    NEAR
@@Loop: mov     al, ds:[si]
        mov     es:[di], al
        add     di, dx
        dec     cx
        jz      @@Exit
        add     si, WORD PTR HeightStep[2]
        add     bx, WORD PTR HeightStep[0]
        jnc     @@Loop
        add     si, [Width]
        jmp     @@Loop
@@Exit: ret
subMove         ENDP
;
subAnd          PROC     NEAR
@@Loop: mov     al, ds:[si]
        and     es:[di], al
        add     di, dx
        dec     cx
        jz      @@Exit
        add     si, WORD PTR HeightStep[2]
        add     bx, WORD PTR HeightStep[0]
        jnc     @@Loop
        add     si, [Width]
        jmp     @@Loop
@@Exit: ret
subAnd          ENDP
;
subOr           PROC     NEAR
@@Loop: mov     al, ds:[si]
        or      es:[di], al
        add     di, dx
        dec     cx
        jz      @@Exit
        add     si, WORD PTR HeightStep[2]
        add     bx, WORD PTR HeightStep[0]
        jnc     @@Loop
        add     si, [Width]
        jmp     @@Loop
@@Exit: ret
subOr           ENDP
;
subXor          PROC     NEAR
@@Loop: mov     al, ds:[si]
        xor     es:[di], al
        add     di, dx
        dec     cx
        jz      @@Exit
        add     si, WORD PTR HeightStep[2]
        add     bx, WORD PTR HeightStep[0]
        jnc     @@Loop
        add     si, [Width]
        jmp     @@Loop
@@Exit: ret
subXor          ENDP
;
subTrans        PROC     NEAR
@@Loop: mov     al, ds:[si]
        cmp     al, ah
        je      @@Skip
        mov     es:[di], al
@@Skip:
        add     di, dx
        dec     cx
        jz      @@Exit
        add     si, WORD PTR HeightStep[2]
        add     bx, WORD PTR HeightStep[0]
        jnc     @@Loop
        add     si, [Width]
        jmp     @@Loop
@@Exit: ret
subTrans        ENDP
;
subAdd          PROC     NEAR
@@Loop: mov     al, ds:[si]
        add     es:[di], al
        add     di, dx
        dec     cx
        jz      @@Exit
        add     si, WORD PTR HeightStep[2]
        add     bx, WORD PTR HeightStep[0]
        jnc     @@Loop
        add     si, [Width]
        jmp     @@Loop
@@Exit: ret
subAdd          ENDP

mxStretchImage  ENDP

MX_TEXT         ENDS
END
