;-----------------------------------------------------------
;
; MXLN.ASM - Start line function
; Copyright (c) 1994 by Alessandro Scotti
;
;-----------------------------------------------------------
WARN    PRO
NOWARN  RES
INCLUDE MODEX.DEF

PUBLIC  mxStartLine

MX_TEXT SEGMENT USE16 PARA PUBLIC 'CODE'
        ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

EXTRN   mx_BytesPerLine : WORD

;-----------------------------------------------------------
;
; Changes the video start address to the specified line.
;
; Input:
;       Line    = new start line
; Output:
;       none
;
mxStartLine     PROC FAR
        ARG     Line:WORD       = ARG_SIZE
        .enter  0
        ASSUME  ds:NOTHING

        mov     ax, [Line]              ; Get video offset
        mul     [mx_BytesPerLine]
        xchg    ax, bx                  ; Copy it into BX

; Wait display
        mov     dx, STATUS
@@1:    in      al, dx
        test    al, 08h
        jnz     @@1

; Set starting address
        mov     dx, CRTC
        mov     al, 0Ch                 ; Linear Starting Address high
        mov     ah, bh
        cli
        out     dx, ax
        mov     al, 0Dh                 ; Linear Starting Address low
        mov     ah, bl
        out     dx, ax
        sti

; Wait retrace
        mov     dx, STATUS
@@2:    in      al,dx
        test    al, 08h
        jz      @@2

        xor     ax, ax
        .leave  ARG_SIZE
MX_TEXT         ENDS
END
