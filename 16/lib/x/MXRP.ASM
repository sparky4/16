;-----------------------------------------------------------
;
; MXRP.ASM - Rotate palette function
; Copyright (c) 1994 by Alessandro Scotti
;
;-----------------------------------------------------------
;WARN    PRO
INCLUDE MODEX.DEF

PUBLIC  mxRotatePalette

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

;-----------------------------------------------------------
;
; Rotates the palette of the specified number of colors.
;
; Input:
;       Palette = pointer to palette
;       Count   = number of colors to rotate
;       Step    = step size
; Output:
;       none
;
; Note: if Step is positive palette is rotated left to right, otherwise
;       right to left.
;
mxRotatePalette PROC    FAR
        ARG     Step:WORD,      \
                Count:WORD,     \
                Palette:DWORD   = ARG_SIZE
        LOCAL   Holder:BYTE:768 = AUTO_SIZE
        ASSUME  ds:NOTHING
        .enter  AUTO_SIZE
        .push   ds, si, es, di

        mov     bx, [Count]
        add     bx, bx
        add     bx, [Count]             ; BX = Count*3

        lds     si, [Palette]           ; DS:SI -> palette
        push    ss
        pop     es
        lea     di, Holder              ; ES:DI -> local space
        cld

        mov     ax, [Step]
        mov     dx, ax
        test    ax, ax
        jz      @@Exit                  ; Nothing to do, exit
        jl      @@RightToLeft

@@LeftToRight:
        add     ax, ax
        add     dx, ax                  ; DX = Step*3
        sub     bx, dx                  ; BX = (Count-Step)*3
        add     si, bx
        push    si
        mov     cx, dx
        rep     movsb
        mov     es, WORD PTR Palette[2]
        mov     di, si
        dec     di                      ; ES:DI -> last byte of palette
        pop     si
        dec     si
        mov     cx, bx
        std
        rep     movsb
        push    ss
        pop     ds
        lea     si, Holder
        les     di, [Palette]
        mov     cx, dx
        cld
        rep     movsb
        jmp     @@Exit

@@RightToLeft:
        add     ax, ax
        add     dx, ax
        neg     dx                      ; DX = Step*3
        sub     bx, dx                  ; BX = (Count-Step)*3
        mov     cx, dx
        rep     movsb
        les     di, [Palette]
        mov     cx, bx
        rep     movsb
        push    ss
        pop     ds
        lea     si, Holder
        mov     cx, dx
        rep     movsb

@@Exit:
        .pop    ds, si, es, di
        .leave  ARG_SIZE
mxRotatePalette ENDP

MX_TEXT         ENDS
END
