;-----------------------------------------------------------
;
; MXPN.ASM - Panning function
; Copyright (c) 1993,1994 by Alessandro Scotti
;
;-----------------------------------------------------------
WARN    PRO
INCLUDE MODEX.DEF

PUBLIC  mxPan

EXTRN   mxWaitDisplay   : FAR
EXTRN   mxStartAddress  : FAR

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

EXTRN   mx_BytesPerLine : WORD

;-----------------------------------------------------------
;
; Moves the screen.
;
; Input:
;       X, Y    = new X, Y coordinates of view screen
; Output:
;       none
;
mxPan           PROC    FAR
        ARG     Y:WORD,                 \
                X:WORD                  = ARG_SIZE
        ASSUME  ds:NOTHING
        .enter  0

        mov     ax, [Y]
        mul     [mx_BytesPerLine]
        mov     dx, [X]
        shr     dx, 1
        shr     dx, 1
        add     ax, dx
        push    ax                      ; Push the start address
        call    mxWaitDisplay
        call    mxStartAddress

        mov     dx, 03DAh               ; Set the pixel pan register
        in      al, dx
        mov     dx, 03C0h
        mov     al, 33h
        out     dx, al
        mov     al, BYTE PTR [X]
        and     al, 3
        shl     al, 1
        out     dx, al

        xor     ax, ax
        .leave  ARG_SIZE
mxPan           ENDP

MX_TEXT         ENDS
END
