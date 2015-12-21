;-----------------------------------------------------------
;
; MXWD.ASM - Wait display function
; Copyright (c) 1993,1994 by Alessandro Scotti
;
;-----------------------------------------------------------
WARN    PRO
INCLUDE MODEX.DEF

PUBLIC  mxWaitDisplay

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

;-----------------------------------------------------------
;
; Waits for display start.
;
mxWaitDisplay   PROC    FAR
        mov     dx, STATUS
@@1:    in      al, dx
        test    al, 08h
        jnz     @@1
        ret
mxWaitDisplay   ENDP

MX_TEXT         ENDS
END
