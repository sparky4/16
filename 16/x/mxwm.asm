;-----------------------------------------------------------
;
; MXWM.ASM - Set write mode function
; Copyright (c) 1993,1994 by Alessandro Scotti
;
;-----------------------------------------------------------
WARN    PRO
INCLUDE MODEX.DEF

PUBLIC  mxWriteMode

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

;-----------------------------------------------------------
;
; Sets the write mode.
;
; Input:
;       Mode    = write mode (0,1,2,3)
; Output:
;       none
;
mxWriteMode     PROC    FAR
        ARG     Mode:BYTE:2     = ARG_SIZE
        .enter  0

        mov     dx, GDC
        mov     ah, [Mode]
        and     ah, 00000011b
        or      ah, 01000000b
        mov     al, 05h
        out     dx, ax

        .leave  ARG_SIZE
mxWriteMode     ENDP

MX_TEXT         ENDS
END
