;-----------------------------------------------------------
;
; MXSS.ASM - Split screen function
; Copyright (c) 1993,1994 by Alessandro Scotti
;
;-----------------------------------------------------------
;WARN    PRO
INCLUDE MODEX.DEF

PUBLIC  mxSplitScreen

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

;-----------------------------------------------------------
;
; Splits the screen.
;
; Input:
;       Line    = scan line at which screen has to be splitted
; Output:
;       none
;
mxSplitScreen   PROC    FAR
        ARG     Line:WORD       = ARG_SIZE
        ASSUME  ds:NOTHING
        .enter  0

; Modify the line compare value: bits 0-7 are in the Line Compare
; register (CRTC #18), bit 8 is in the Overflow Low register (CRTC #7)
; and bit 9 is in the Maximum Row Address register (CRTC #9)
        mov     ax, [Line]
        shl     ax, 1                   ; Adjust line for mode "X"
        mov     bh, ah
        mov     bl, ah
        and     bx, 0201h
        mov     cl, 4
        shl     bx, cl
        shl     bh, 1
        mov     dx, CRTC
; Write bits 0-7 to line compare register
        mov     ah, al
        mov     al, 18h
        out     dx, ax
; Write bit 8 to overflow register
        mov     al, 07h
        out     dx, al
        inc     dx
        in      al, dx
        dec     dx
        mov     ah, al
        and     ah, 11101111b
        or      ah, bl
        mov     al, 07h
        out     dx, ax
; Write bit 9 to maximum row address register
        mov     al, 09h
        out     dx, al
        inc     dx
        in      al, dx
        dec     dx
        mov     ah, al
        and     ah, 10111111b
        or      ah, bh
        mov     al, 09h
        out     dx, ax

        .leave  ARG_SIZE
mxSplitScreen   ENDP

MX_TEXT         ENDS
END
