;-----------------------------------------------------------
;
; MXPB.ASM - Scan buffer for convex polygon fills
; Copyright (c) 1994 by Alessandro Scotti
;
;-----------------------------------------------------------
WARN    PRO
NOWARN  RES
INCLUDE MODEX.DEF

PUBLIC  mx_ScanBuffer

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

ALIGN   4

mx_ScanBuffer   LABEL
        DW      POLYSCANBUFSIZE DUP(?)

MX_TEXT         ENDS
END
