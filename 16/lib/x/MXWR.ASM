;-----------------------------------------------------------
;
; MXWR.ASM - Wait vertical retrace function
; Copyright (c) 1993,1994 by Alessandro Scotti
;
;-----------------------------------------------------------
;WARN    PRO
INCLUDE MODEX.DEF

PUBLIC  mxWaitRetrace

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

;-----------------------------------------------------------
;
; Waits for vertical retrace start.
;
mxWaitRetrace   PROC    FAR
        mov     dx, STATUS
@@1:    in      al,dx
        test    al, 08h
        jz      @@1
        ret
mxWaitRetrace   ENDP

MX_TEXT         ENDS
END
