;-----------------------------------------------------------
;
; MXWP.ASM - Set write/read plane functions
; Copyright (c) 1993,1994 by Alessandro Scotti
;
;-----------------------------------------------------------
WARN    PRO
INCLUDE MODEX.DEF

PUBLIC  mxWritePlane
PUBLIC  mxReadPlane

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

;-----------------------------------------------------------
;
; Sets the write plane(s).
;
; Input:
;       Plane   = write plane(s) to set (bit 0 enables plane 0,
;                 bit 1 enables plane 1 and so on, different planes
;                 may be selected at the same time)
; Output:
;       none
;
mxWritePlane    PROC    FAR
        ARG     Plane:BYTE:2    = ARG_SIZE
        .enter  0

        mov     ah, [Plane]
        and     ah, 00001111b           ; Mask off unused bits
        mov     al, 02h
        mov     dx, TS
        out     dx, ax

        .leave  ARG_SIZE
mxWritePlane    ENDP

;-----------------------------------------------------------
;
; Sets the read plane.
;
; Input:
;       Plane   = read plane to set (0,1,2,3)
; Output:
;       none
;
mxReadPlane     PROC    FAR
        ARG     Plane:BYTE:2    = ARG_SIZE
        ASSUME  ds:NOTHING
        .enter  0
        mov     al, 04h
        mov     ah, [Plane]
        and     ah, 0000011b            ; Mask off unused bits
        mov     dx, GDC
        out     dx, ax
        .leave  ARG_SIZE
mxReadPlane     ENDP

MX_TEXT         ENDS
END
