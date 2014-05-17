;-----------------------------------------------------------
;
; MXSC.ASM - Set color function
; Copyright (c) 1994 by Alessandro Scotti
;
;-----------------------------------------------------------
;WARN    PRO
INCLUDE MODEX.DEF

PUBLIC  mxSetColor

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

;-----------------------------------------------------------
;
; Updates the selected DAC register.
;
; Input:
;       Index   = index of color to set
;       R, G, B = color components
; Output:
;       none
;
mxSetColor      PROC FAR
        ARG     B:BYTE:2,       \
                G:BYTE:2,       \
                R:BYTE:2,       \
                Index:WORD      = ARG_SIZE
        .enter  0
        .push   ds, si

        mov     ax, [Index]
        mov     dx, 3C8h                ; PEL write address register
        out     dx, al
        inc     dx

        mov     al, [R]
        out     dx, al
        mov     al, [G]
        out     dx, al
        mov     al, [B]
        out     dx, al

        .pop    ds, si
        .leave  ARG_SIZE
mxSetColor      ENDP

MX_TEXT         ENDS
END
