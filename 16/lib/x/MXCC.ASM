;-----------------------------------------------------------
;
; MXCC.ASM - Fast clip line function
; Copyright (c) 1994 by Alessandro Scotti
;
;-----------------------------------------------------------
;WARN    PRO
;NOWARN  RES
INCLUDE MODEX.DEF

PUBLIC  xsubClipLine

MX_TEXT SEGMENT USE16 PARA PUBLIC 'CODE'
        ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

EXTRN   mx_BytesPerLine : WORD
EXTRN   mx_VideoSegment : WORD

EXTRN   mx_ClipX1       : WORD
EXTRN   mx_ClipY1       : WORD
EXTRN   mx_ClipX2       : WORD
EXTRN   mx_ClipY2       : WORD

tblGroups       LABEL WORD
        DW      10,     tbl00
        DW      10,     tbl10
        DW      9,      tbl20
        DW      -1,     0
        DW      10,     tbl40
        DW      10,     tbl50
        DW      9,      tbl60
        DW      -1,     0
        DW      6,      tbl80
        DW      6,      tbl90
        DW      5,      tblA0
        DW      -1,     0
        DW      -1,     0
        DW      -1,     0
        DW      -1,     0
        DW      -1,     0
tbl00   DW cc00, cc01, cc02, ccFF, cc04, cc05, cc06, ccFF, cc08, cc09, cc0A
tbl10   DW cc10, ccFF, cc12, ccFF, cc14, ccFF, cc16, ccFF, cc18, ccFF, cc1A
tbl20   DW cc20, cc21, ccFF, ccFF, cc24, cc25, ccFF, ccFF, cc28, cc29
tbl40   DW cc40, cc41, cc42, ccFF, ccFF, ccFF, ccFF, ccFF, cc48, cc49, cc4A
tbl50   DW cc50, ccFF, cc52, ccFF, ccFF, ccFF, ccFF, ccFF, cc58, ccFF, cc5A
tbl60   DW cc60, cc61, ccFF, ccFF, ccFF, ccFF, ccFF, ccFF, cc68, cc69
tbl80   DW cc80, cc81, cc82, ccFF, cc84, cc85, cc86
tbl90   DW cc90, ccFF, cc92, ccFF, cc94, ccFF, cc96
tblA0   DW ccA0, ccA1, ccFF, ccFF, ccA4, ccA5

ccTT:   clc
        ret
ccFF:   stc
        ret

; Group 00 -------------------------------------------------
;
cc00:
        clc
        ret
cc01:
        jmp     ClipQLeft
cc02:
        jmp     ClipQRight
cc04:
        jmp     ClipQTop
cc05:
        call    ClipQLeft
        cmp     si, [mx_ClipY1]
        jge     ccTT
        jmp     ClipQTop
cc06:
        call    ClipQRight
        cmp     si, [mx_ClipY1]
        jge     ccTT
        jmp     ClipQTop
cc08:
        jmp     ClipQBottom
cc09:
        call    ClipQLeft
        cmp     si, [mx_ClipY2]
        jle     ccTT
        jmp     ClipQBottom
cc0A:
        call    ClipQRight
        cmp     si, [mx_ClipY2]
        jle     ccTT
        jmp     ClipQBottom

; Group 10 -------------------------------------------------
;
cc10FF:
        stc
        ret
cc10TT:
        clc
        ret
cc10:
        jmp     ClipPLeft
cc12:
        call    ClipPLeft
        jmp     ClipQRight
cc14:
        call    ClipPLeft
        cmp     bx, [mx_ClipY1]
        jl      cc10FF
        jmp     ClipQTop
cc16:
        call    ClipPLeft
        cmp     bx, [mx_ClipY1]
        jl      cc10FF
        call    ClipQTop
        cmp     cx, [mx_ClipX2]
        jle     cc10TT
        jmp     ClipQRight
cc18:
        call    ClipPLeft
        cmp     bx, [mx_ClipY2]
        jg      cc10FF
        jmp     ClipQBottom
cc1A:
        call    ClipPLeft
        cmp     bx, [mx_ClipY2]
        jg      cc10FF
        call    ClipQBottom
        cmp     cx, [mx_ClipX2]
        jle     cc10TT
        jmp     ClipQRight

; Group 20 -------------------------------------------------
;
cc20TT:
        clc
        ret
cc20FF:
        stc
        ret
cc20:
        jmp     ClipPRight
cc21:
        call    ClipPRight
        jmp     ClipQLeft
cc24:
        call    ClipPRight
        cmp     bx, [mx_ClipY1]
        jl      cc20FF
        jmp     ClipQTop
cc25:
        call    ClipPRight
        cmp     bx, [mx_ClipY1]
        jl      cc20FF
        call    ClipQTop
        cmp     cx, [mx_ClipX1]
        jge     cc20TT
        jmp     ClipQLeft
cc28:
        call    ClipPRight
        cmp     bx, [mx_ClipY2]
        jg      cc20FF
        jmp     ClipQBottom
cc29:
        call    ClipPRight
        cmp     bx, [mx_ClipY2]
        jg      cc20FF
        call    ClipQBottom
        cmp     cx, [mx_ClipX1]
        jge     cc20TT
        jmp     ClipQLeft

; Group 40 -------------------------------------------------
;
cc40TT:
        clc
        ret
cc40FF:
        stc
        ret
cc40:
        jmp     ClipPTop
cc41:
        call    ClipPTop
        cmp     di, [mx_ClipX1]
        jl      cc40FF
        call    ClipQLeft
        cmp     si, [mx_ClipY1]
        jge     cc40TT
        jmp     ClipQTop
cc42:
        call    ClipPTop
        cmp     di, [mx_ClipX2]
        jg      cc40FF
        jmp     ClipQRight
cc48:
        call    ClipPTop
        jmp     ClipQBottom
cc49:
        call    ClipPTop
        cmp     di, [mx_ClipX1]
        jl      cc40FF
        call    ClipQLeft
        cmp     si, [mx_ClipY2]
        jle     cc40TT
        jmp     ClipQBottom
cc4A:
        call    ClipPTop
        cmp     di, [mx_ClipX2]
        jg      cc40FF
        call    ClipQRight
        cmp     si, [mx_ClipY2]
        jle     cc40TT
        jmp     ClipQBottom


; Group 50 -------------------------------------------------
;
cc50TT:
        clc
        ret
cc50FF:
        stc
        ret
cc50:
        call    ClipPLeft
        cmp     bx, [mx_ClipY1]
        jge     cc50TT
        jmp     ClipPTop
cc52:
        call    ClipQRight
        cmp     si, [mx_ClipY1]
        jl      cc50FF
        call    ClipPTop
        cmp     di, [mx_ClipX1]
        jge     cc50TT
        jmp     ClipPLeft
cc58:
        call    ClipQBottom
        cmp     cx, [mx_ClipX1]
        jl      cc50FF
        call    ClipPTop
        cmp     di, [mx_ClipX1]
        jge     cc50TT
        jmp     ClipPLeft
cc5A:
        call    ClipPLeft
        cmp     bx, [mx_ClipY2]
        jg      cc50FF
        call    ClipQRight
        cmp     bx, [mx_ClipY1]
        jl      cc50FF
        cmp     si, [mx_ClipY2]
        jle     cc50TT
        jmp     ClipQBottom

; Group 60 -------------------------------------------------
;
cc60TT:
        clc
        ret
cc60FF:
        stc
        ret
cc60:
        call    ClipPRight
        cmp     bx, [mx_ClipY1]
        jge     cc60TT
        jmp     ClipPTop
cc61:
        call    ClipQLeft
        cmp     si, [mx_ClipY2]
        jl      cc60FF
        call    ClipPTop
        cmp     di, [mx_ClipX2]
        jle     cc60TT
        jmp     ClipPRight
cc68:
        call    ClipQBottom
        cmp     cx, [mx_ClipX2]
        jg      cc60FF
        call    ClipPRight
        cmp     bx, [mx_ClipY1]
        jge     cc60TT
        jmp     ClipPTop
cc69:
        call    ClipQLeft
        cmp     si, [mx_ClipY1]
        jl      cc60FF
        call    ClipPRight
        cmp     bx, [mx_ClipY2]
        jg      cc60FF
        cmp     si, [mx_ClipY2]
        jle     cc69_1
        call    ClipQBottom
cc69_1:
        cmp     bx, [mx_ClipY1]
        jge     cc60TT
        jmp     ClipPTop

; Group 80 -------------------------------------------------
;
cc80TT:
        clc
        ret
cc80FF:
        stc
        ret
cc80:
        jmp     ClipPBottom
cc81:
        call    ClipPBottom
        cmp     di, [mx_ClipX1]
        jl      cc80FF
        jmp     ClipQLeft
cc82:
        call    ClipPBottom
        cmp     di, [mx_ClipX2]
        jg      cc80FF
        jmp     ClipQRight
cc84:
        call    ClipPBottom
        jmp     ClipQTop
cc85:
        call    ClipPBottom
        cmp     di, [mx_ClipX1]
        jl      cc80FF
        call    ClipQLeft
        cmp     si, [mx_ClipY1]
        jge     cc80FF
        jmp     ClipQTop
cc86:
        call    ClipPBottom
        cmp     di, [mx_ClipX2]
        jg      cc80FF
        call    ClipQRight
        cmp     si, [mx_ClipY1]
        jge     cc80TT
        jmp     ClipQTop

; Group 90 -------------------------------------------------
;
cc90TT:
        clc
        ret
cc90FF:
        stc
        ret
cc90:
        call    ClipPLeft
        cmp     bx, [mx_ClipY2]
        jle     cc90TT
        jmp     ClipPBottom
cc92:
        call    ClipQRight
        cmp     si, [mx_ClipY2]
        jg      cc90FF
        call    ClipPBottom
        cmp     di, [mx_ClipX1]
        jge     cc90TT
        jmp     ClipPLeft
cc94:
        call    ClipQTop
        cmp     cx, [mx_ClipX1]
        jl      cc90FF
        call    ClipPLeft
        cmp     bx, [mx_ClipY2]
        jle     cc90TT
        jmp     ClipPBottom
cc96:
        call    ClipPLeft
        cmp     bx, [mx_ClipY1]
        jl      cc90FF
        call    ClipQRight
        cmp     si, [mx_ClipY2]
        jg      cc90FF
        cmp     bx, [mx_ClipY2]
        jle     cc96_1
        call    ClipPBottom
cc96_1:
        cmp     si, [mx_ClipY1]
        jge     cc90TT
        jmp     ClipQTop

; Group A0 -------------------------------------------------
;
ccA0TT:
        clc
        ret
ccA0FF:
        stc
        ret
ccA0:
        call    ClipPRight
        cmp     bx, [mx_ClipY2]
        jle     ccA0TT
        jmp     ClipPBottom
ccA1:
        call    ClipQLeft
        cmp     si, [mx_ClipY2]
        jg      ccA0FF
        call    ClipPBottom
        cmp     di, [mx_ClipX2]
        jle     ccA0TT
        jmp     ClipPRight
ccA4:
        call    ClipQTop
        cmp     cx, [mx_ClipX2]
        jg      ccA0FF
        call    ClipPRight
        cmp     bx, [mx_ClipY2]
        jle     ccA0TT
        jmp     ClipPBottom
ccA5:
        call    ClipQLeft
        cmp     si, [mx_ClipY2]
        jg      ccA0FF
        call    ClipPRight
        cmp     bx, [mx_ClipY1]
        jl      ccA0FF
        cmp     si, [mx_ClipY1]
        jge     ccA5_1
        call    ClipQTop
ccA5_1:
        cmp     bx, [mx_ClipY2]
        jle     ccA0TT
        jmp     ClipPBottom

; Y1 = (Y2-Y1)*(mx_ClipX1-X1)/(X2-X1)+Y1 = (SI-BX)*(mx_ClipX1-DI)/(CX-DI)+BX
; X1 = mx_ClipX1
ClipPLeft:
        mov     ax, si
        sub     ax, bx
        mov     dx, [mx_ClipX1]
        sub     dx, di
        imul    dx
        mov     bp, cx
        sub     bp, di
        idiv    bp
        add     bx, ax
        mov     di, [mx_ClipX1]
        clc
        ret

; Y1 = (Y2-Y1)*(mx_ClipX2-X1)/(X2-X1)+Y1 = (SI-BX)*(mx_ClipX2-DI)/(CX-DI)+BX
; X1 = mx_ClipX2
ClipPRight:
        mov     ax, si
        sub     ax, bx
        mov     dx, [mx_ClipX2]
        sub     dx, di
        imul    dx
        mov     bp, cx
        sub     bp, di
        idiv    bp
        add     bx, ax
        mov     di, [mx_ClipX2]
        clc
        ret

; X1 = (X2-X1)*(mx_ClipY2-Y1)/(Y2-Y1)+X1 = (CX-DI)*(mx_ClipY2-BX)/(SI-BX)+DI
; Y1 = mx_ClipY2
ClipPBottom:
        mov     ax, cx
        sub     ax, di
        mov     dx, [mx_ClipY2]
        sub     dx, bx
        imul    dx
        mov     bp, si
        sub     bp, bx
        idiv    bp
        add     di, ax
        mov     bx, [mx_ClipY2]
        clc
        ret

; X1 = (X2-X1)*(mx_ClipY1-Y1)/(Y2-Y1)+X1 = (CX-DI)*(mx_ClipY1-BX)/(SI-BX)+DI
; Y1 = mx_ClipY1
ClipPTop:
        mov     ax, cx
        sub     ax, di
        mov     dx, [mx_ClipY1]
        sub     dx, bx
        imul    dx
        mov     bp, si
        sub     bp, bx
        idiv    bp
        add     di, ax
        mov     bx, [mx_ClipY1]
        clc
        ret

; Y2 = (Y1-Y2)*(mx_ClipX1-X2)/(X1-X2)+Y2 = (BX-SI)*(mx_ClipX1-CX)/(DI-CX)+SI
; X2 = mx_ClipX1
ClipQLeft:
        mov     ax, bx
        sub     ax, si
        mov     dx, [mx_ClipX1]
        sub     dx, cx
        imul    dx
        mov     bp, di
        sub     bp, cx
        idiv    bp
        add     si, ax
        mov     cx, [mx_ClipX1]
        clc
        ret

; Y2 = (Y1-Y2)*(mx_ClipX2-X2)/(X1-X2)+Y2 = (BX-SI)*(mx_ClipX2-CX)/(DI-CX)+SI
; X2 = mx_ClipX1
ClipQRight:
        mov     ax, bx
        sub     ax, si
        mov     dx, [mx_ClipX2]
        sub     dx, cx
        imul    dx
        mov     bp, di
        sub     bp, cx
        idiv    bp
        add     si, ax
        mov     cx, [mx_ClipX2]
        clc
        ret

; X2 = (X1-X2)*(mx_ClipY2-Y2)/(Y1-Y2)+X2 = (DI-CX)*(mx_ClipY2-SI)/(BX-SI)+CX
; Y2 = mx_ClipY1
ClipQBottom:
        mov     ax, di
        sub     ax, cx
        mov     dx, [mx_ClipY2]
        sub     dx, si
        imul    dx
        mov     bp, bx
        sub     bp, si
        idiv    bp
        add     cx, ax
        mov     si, [mx_ClipY2]
        clc
        ret

; X2 = (X1-X2)*(mx_ClipY1-Y2)/(Y1-Y2)+X2 = (DI-CX)*(mx_ClipY1-SI)/(BX-SI)+CX
; Y2 = mx_ClipY1
ClipQTop:
        mov     ax, di
        sub     ax, cx
        mov     dx, [mx_ClipY1]
        sub     dx, si
        imul    dx
        mov     bp, bx
        sub     bp, si
        idiv    bp
        add     cx, ax
        mov     si, [mx_ClipY1]
        clc
        ret

;-----------------------------------------------------------
;
; Checks the coordinates of a line against the active
; clip region.
; Uses the Sobkow-Pospisil-Yang (SPY) algorithm: this was
; supposed to be twice as fast as Cohen-Sutherland, but my
; tests show only a very small increase in speed and a noticeable
; increase of the program size! Maybe this is caused by the
; slow speed of VGA cards, so probably a better test should
; be performed with lines drawn in RAM.
;
; Input:
;       AX, BX  = X1, Y1
;       CX, DX  = X2, Y2
; Output:
;       CF      = set if line is full clipped
;       AX, BX  = clipped X1, Y1
;       CX, DX  = clipped X2, Y2
; Note:
;       destroys SI, DI
;
xsubClipLine     PROC    NEAR
        push    bp
        xor     si, si                  ; SPY code

        cmp     dx, [mx_ClipY2]
        jle     @@1
        or      si, 08h
        jmp     @@2
@@1:
        cmp     dx, [mx_ClipY1]
        jge     @@2
        or      si, 04h
@@2:

        cmp     cx, [mx_ClipX2]
        jle     @@3
        or      si, 02h
        jmp     @@4
@@3:
        cmp     cx, [mx_ClipX1]
        jge     @@4
        or      si, 01h
@@4:

        cmp     bx, [mx_ClipY2]
        jle     @@5
        or      si, 80h
        jmp     @@6
@@5:
        cmp     bx, [mx_ClipY1]
        jge     @@6
        or      si, 40h
@@6:

        cmp     ax, [mx_ClipX2]
        jle     @@7
        or      si, 20h
        jmp     @@8
@@7:
        cmp     ax, [mx_ClipX1]
        jge     @@8
        or      si, 10h
@@8:

        mov     di, si
        and     di, 000Fh               ; Index of procedure
        and     si, 00F0h
        .shr    si, 2                   ; Index of group (times 4)
        cmp     di, cs:tblGroups[si]    ; Is index within range?
        jg      @@Exit                  ; No, line is full clipped
        mov     si, cs:tblGroups[si+2]  ; Get offset of group table
        shl     di, 1                   ; We must index word elements
        add     si, di                  ; Make full offset
        mov     di, ax                  ; Move X1 to DI and free AX
        mov     si, cs:[si]             ; Get subroutine address
        xchg    dx, si                  ; Move Y2 to SI and free DX
        call    dx                      ; Call the proper subroutine
        mov     ax, di                  ; Restore AX to X1
        mov     dx, si                  ; Restore DX to Y2
        pop     bp
        ret

@@Exit:
        pop     bp
        stc
        ret
xsubClipLine     ENDP

MX_TEXT         ENDS
END
