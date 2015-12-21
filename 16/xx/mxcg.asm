;-----------------------------------------------------------
;
; MXCG.ASM - Color to gray conversion
; Copyright (c) 1994 by Alessandro Scotti
;
;-----------------------------------------------------------
WARN    PRO
INCLUDE MODEX.DEF

PUBLIC  mxColorToGray

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

;-----------------------------------------------------------
;
; Converts RGB colors to gray shades.
;
; Input:
;       CPal    = pointer to color palette
;       GPal    = pointer to destination (gray) palette
;       Count   = number of colors to convert
; Output:
;       none
;
; Note: CPal and GPal may point to the same buffer.
;
mxColorToGray   PROC    FAR
        ARG     Count:WORD,     \
                DPal:DWORD,     \
                SPal:DWORD      = ARG_SIZE
        ASSUME  ds:NOTHING
        .enter  0
        .push   ds, si, es, di

        mov     cx, [Count]
        jcxz    @@Exit
        lds     si, [SPal]
        les     di, [DPal]
        cld
; We use the standard formula
;       gray=(red*30 + green*59 + blue*11)/100
; in the equivalent form
;       gray=(red*77 + green*151 + blue*28)/256
; which doesn't need the last divide.
        mov     bx, 77 SHL 8 + 151
@@Loop:
        lodsb                           ; Red
        mul     bh
        mov     dx, ax
        lodsb                           ; Green
        mul     bl
        add     dx, ax
        lodsb                           ; Blue
        mov     ah, 28
        mul     ah
        add     ax, dx
        mov     al, ah
        stosw                           ; Save new RGB
        stosb
        loop    @@Loop

@@Exit:
        .pop    ds, si, es, di
        .leave  ARG_SIZE
mxColorToGray   ENDP

MX_TEXT         ENDS
END
