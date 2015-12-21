;-----------------------------------------------------------
;
; MXGP.ASM - Get palette function
; Copyright (c) 1993,1994 by Alessandro Scotti
;
;-----------------------------------------------------------
WARN    PRO
INCLUDE MODEX.DEF

PUBLIC  mxGetPalette

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

;-----------------------------------------------------------
;
; Returns the current setting of the VGA palette.
;
; Input:
;       Buffer          = pointer to palette data (R,G,B)
;       Start           = index of first color to get
;       Count           = number of color to get
; Output:
;       none
;
mxGetPalette    PROC    FAR
        ARG     Count:WORD,             \
                Start:WORD,             \
                Buffer:DWORD            = ARG_SIZE
        ASSUME  ds:NOTHING
        .enter  0
        .push   es, di

        les     di, [Buffer]
        mov     cx, [Count]
        mov     ax, [Start]
        mov     dx, 3C7h                ; PEL read address register
        out     dx, al
        inc     dx
        inc     dx
        cld
@@Loop:
        in      al, dx
        stosb
        in      al, dx
        stosb
        in      al, dx
        stosb
        loop    @@Loop                  ; Loop until done

        .pop    es, di
        .leave  ARG_SIZE
mxGetPalette    ENDP

MX_TEXT         ENDS
END
