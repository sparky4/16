;-----------------------------------------------------------
;
; MXSA.ASM - Start address function
; Copyright (c) 1993,1994 by Alessandro Scotti
;
;-----------------------------------------------------------
;WARN    PRO
INCLUDE MODEX.DEF

PUBLIC  mxStartAddress

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

;-----------------------------------------------------------
;
; Modifies the starting address of video memory.
;
; Input:
;       StartAddr       = new start address of video memory
; Output:
;       none
;
mxStartAddress  PROC    FAR
        ARG     StartAddr:WORD  = ARG_SIZE
        ASSUME  ds:NOTHING
        .enter  0

        mov     bx, [StartAddr]
        mov     dx, CRTC
        mov     al, 0Ch                 ; Linear Starting Address high
        mov     ah, bh
        cli
        out     dx, ax
        mov     al, 0Dh                 ; Linear Starting Address low
        mov     ah, bl
        out     dx, ax
        sti

        .leave  ARG_SIZE
mxStartAddress  ENDP

MX_TEXT         ENDS
END
