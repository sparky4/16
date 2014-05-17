;-----------------------------------------------------------
;
; MXGV.ASM - Get version function
; Copyright (c) 1993,1994 by Alessandro Scotti
;
;-----------------------------------------------------------
;WARN    PRO
INCLUDE MODEX.DEF

PUBLIC  mxGetVersion

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

;-----------------------------------------------------------
;
; Returns the library version.
;
mxGetVersion    PROC    FAR
        mov     ax, MXVERSION
        ret
mxGetVersion    ENDP

MX_TEXT         ENDS
END
