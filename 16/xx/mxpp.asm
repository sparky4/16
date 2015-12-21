;-----------------------------------------------------------
;
; MXPP.ASM - Get/put pixel functions
; Copyright (c) 1993,1994 by Alessandro Scotti
;
;-----------------------------------------------------------
WARN    PRO
INCLUDE MODEX.DEF

PUBLIC  mxGetPixel
PUBLIC  mxPutPixel

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

EXTRN   mx_BytesPerLine : WORD
EXTRN   mx_VideoSegment : WORD
EXTRN   mx_ClipX1       : WORD
EXTRN   mx_ClipY1       : WORD
EXTRN   mx_ClipX2       : WORD
EXTRN   mx_ClipY2       : WORD

;-----------------------------------------------------------
;
; Gets a pixel.
;
; Input:
;       X, Y    = pixel coordinates
; Output:
;       pixel color
;
mxGetPixel      PROC    FAR
        ARG     Y:WORD,                 \
                X:WORD                  = ARG_SIZE
        ASSUME  ds:NOTHING
        .enter  0
        .push   ds, si

        xor     ax, ax
        mov     si, [X]
        cmp     si, [mx_ClipX1]
        jl      @@Exit
        cmp     si, [mx_ClipX2]
        jg      @@Exit
        mov     bx, [Y]
        cmp     bx, [mx_ClipY1]
        jl      @@Exit
        cmp     bx, [mx_ClipY2]
        jg      @@Exit

        mov     al, 04h                 ; Set read plane
        mov     ah, BYTE PTR [X]
        and     ah, 3
        mov     dx, GDC
        out     dx, ax

        mov     ds, [mx_VideoSegment]
        mov     ax, bx
        mul     [mx_BytesPerLine]
        .shr    si, 2
        add     si, ax

        mov     al, ds:[si]             ; Get pixel value
        xor     ah, ah

@@Exit:
        .pop    ds, si
        .leave  ARG_SIZE
mxGetPixel      ENDP

;-----------------------------------------------------------
;
; Puts a pixel of the specified color.
;
; Input:
;       X, Y    = pixel coordinates
;       Color   = pixel color
; Output:
;       none
;
mxPutPixel      PROC    FAR
        ARG     Color:BYTE:2,           \
                Y:WORD,                 \
                X:WORD                  = ARG_SIZE
        .enter  0
        .push   ds, si

        mov     si, [X]
        cmp     si, [mx_ClipX1]
        jl      @@Exit
        cmp     si, [mx_ClipX2]
        jg      @@Exit
        mov     ax, [Y]
        cmp     ax, [mx_ClipY1]
        jl      @@Exit
        cmp     ax, [mx_ClipY2]
        jg      @@Exit

        mov     ds, [mx_VideoSegment]
        mul     [mx_BytesPerLine]
        .shr    si, 2
        add     si, ax

        mov     cl, BYTE PTR [X]        ; Set write plane
        and     cl, 3
        mov     ax, 0102h
        shl     ah, cl
        mov     dx, TS
        out     dx, ax

        mov     al, [Color]             ; Write pixel
        mov     ds:[si], al

@@Exit:
        xor     ax, ax
        .pop    ds, si
        .leave  ARG_SIZE
mxPutPixel      ENDP

MX_TEXT         ENDS
END
