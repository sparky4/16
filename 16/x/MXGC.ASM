;-----------------------------------------------------------
;
; MXGC.ASM - Get color function
; Copyright (c) 1994 by Alessandro Scotti
;
;-----------------------------------------------------------
WARN    PRO
INCLUDE MODEX.DEF

PUBLIC  mxGetColor

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

;-----------------------------------------------------------
;
; Returns the current setting of the selected DAC register.
;
; Input:
;       Index   = color index (0-255)
;       R, G, B = byte pointers to red, green and blue
; Output:
;       none
;
mxGetColor      PROC FAR
        ARG     B:DWORD,        \
                G:DWORD,        \
                R:DWORD,        \
                Index:WORD      = ARG_SIZE
        .enter  0
        .push   ds, si

        mov     ax, [Index]
        mov     dx, 3C7h                ; PEL read address register
        out     dx, al
        inc     dx
        inc     dx

        lds     si, [R]
        in      al, dx
        mov     ds:[si], al
        lds     si, [G]
        in      al, dx
        mov     ds:[si], al
        lds     si, [B]
        in      al, dx
        mov     ds:[si], al

        .pop    ds, si
        .leave  ARG_SIZE
mxGetColor      ENDP

MX_TEXT         ENDS
END
