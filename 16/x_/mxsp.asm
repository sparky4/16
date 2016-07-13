;-----------------------------------------------------------
;
; MXSP.ASM - Set palette function
; Copyright (c) 1993,1994 by Alessandro Scotti
;
;-----------------------------------------------------------
WARN    PRO
INCLUDE MODEX.DEF

PUBLIC  mxSetPalette

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

;-----------------------------------------------------------
;
; Updates the VGA palette.
;
; Input:
;       Buffer          = pointer to palette data (R,G,B)
;       Start           = index of first color to set
;       Count           = number of color to set
; Output:
;       none
;
mxSetPalette    PROC    FAR
        ARG     Count:WORD,             \
                Start:WORD,             \
                Buffer:DWORD            = ARG_SIZE
        ASSUME  ds:NOTHING
        .enter  0
        .push   ds, si

        lds     si, [Buffer]
        mov     cx, [Count]
        mov     ax, [Start]
        mov     dx, 3C8h                ; PEL write address register
        out     dx, al
        inc     dx
        cld
        cli                             ; Disable interrupts
@@Loop:
        lodsb
        out     dx, al                  ; Red
        lodsb
        out     dx, al                  ; Green
        lodsb
        out     dx, al                  ; Blue
        loop    @@Loop                  ; Loop until done
        sti                             ; Enable interrupts

        .pop    ds, si
        .leave  ARG_SIZE
mxSetPalette    ENDP

MX_TEXT         ENDS
END
