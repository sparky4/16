;-----------------------------------------------------------
;
; MXHL.ASM - Horizontal line mask utility
; Copyright (c) 1993,1994 by Alessandro Scotti
;
;-----------------------------------------------------------
;WARN    PRO
INCLUDE MODEX.DEF

PUBLIC  subHorizontalLineInfo

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

EXTRN   mx_BytesPerLine : WORD

tblLeftSize     DW      00h, 03h, 02h, 01h
tblLeftMask     DB      00h, 0Eh, 0Ch, 08h
tblRightMask    DB      00h, 01h, 03h, 07h
tblPatchMask    DB      02h, 06h

;-----------------------------------------------------------
;
; Prepares register for fast horizontal line drawing.
;
; Input:
;       BX, AX  = X, Y address of left pixel
;       CX      = line width
; Output:
;       DI      = left pixel offset in video memory
;       AL      = left block write plane mask (0 = none)
;       AH      = right block write plane mask (0 = none)
;       CX      = center block width in 4-pixel blocks
;
subHorizontalLineInfo   PROC NEAR
        ASSUME  ds:NOTHING

        mul     [mx_BytesPerLine]
        mov     di, bx
        shr     di, 1
        shr     di, 1
        add     di, ax                  ; Offset of left pixel

        and     bx, 03h
        mov     al, tblLeftMask[bx]
        shl     bx, 1
        sub     cx, tblLeftSize[bx]
        jge     @@1                     ; Ok to continue

; Special case: start and end in the middle of a 4-pixel block.
; There are only three cases:
;    Pixels     Left mask       CX      CX+2    Patch mask      Result
; 1)  ..o.        ..xx          -1      1       .xx.            ..x.
; 2)  .oo.        .xxx          -1      1       .xx.            .xx.
; 3)  .o..        .xxx          -2      0       .x..            .x..
; All other cases are automatically handled with the standard masks.
        mov     bx, cx
        inc     bx
        inc     bx
        and     al, tblPatchMask[bx]    ; Combine masks
        xor     ah, ah                  ; No right block
        xor     cx, cx                  ; No center block
        jmp     @@Exit

@@1:
        mov     bx, cx
        and     bx, 03h
        mov     ah, tblRightMask[bx]
        shr     cx, 2

@@Exit:
        ret
subHorizontalLineInfo   ENDP

MX_TEXT         ENDS
END
