;-----------------------------------------------------------
;
; MXRA.ASM - Row address
; Copyright (c) 1993,1994 by Alessandro Scotti
;
;-----------------------------------------------------------
WARN    PRO
INCLUDE MODEX.DEF

PUBLIC  mxRowAddress

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

;-----------------------------------------------------------
;
; Sets the row address register.
;
; Input:
;       RowAddress      = row size in words
; Output:
;       none
;
mxRowAddress    PROC    FAR
        ARG     RowAddress:BYTE:2       = ARG_SIZE
        ASSUME  ds:NOTHING
        .enter  0
        mov     dx, CRTC
        mov     al, 13h
        mov     ah, [RowAddress]
        out     dx, ax
        xor     ax, ax
        .leave  ARG_SIZE
mxRowAddress    ENDP

MX_TEXT         ENDS
END
