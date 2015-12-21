;
; MATH.INC - Include file for THREED.ASM
;

; 3-dimensional point, coordinates in fixed format (16:16)
;
TPOINT  STRUC
        X       DD      ?
        Y       DD      ?
        Z       DD      ?
TPOINT  ENDS

; 2-dimensional point, coordinates in integer format
;
TIMAGEPOINT     STRUC
        IX      DW      ?
        IY      DW      ?
TIMAGEPOINT     ENDS

; Fixed-point divide: EAX = EAX / arg
;
.xdiv   MACRO   arg
        xor     edx, edx
        shld    edx, eax, 16
        shl     eax, 16
        idiv    arg
ENDM

; Fixed-point multiply: EAX = EAX * arg
;
.xmul   MACRO   arg
        imul    arg
        shrd    eax, edx, 16
ENDM
