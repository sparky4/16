;-----------------------------------------------------------
;
; MXGM.ASM - Gamma correction
; Copyright (c) 1994 by Alessandro Scotti
;
;-----------------------------------------------------------
;WARN    PRO
INCLUDE MODEX.DEF

PUBLIC  mxGammaCorrect

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

mx_tblGamma     LABEL BYTE
    DB  00, 10, 14, 17, 19, 21, 23, 24, 26, 27, 28, 29, 31, 32, 33, 34
    DB  35, 36, 37, 37, 38, 39, 40, 41, 41, 42, 43, 44, 44, 45, 46, 46
    DB  47, 48, 48, 49, 49, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55
    DB  56, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63

;-----------------------------------------------------------
;
; Correct palette colors (gamma is 2.3).
;
; Input:
;       CPal    = pointer to color palette
;       GPal    = pointer to destination (gamma corrected) palette
;       Count   = number of colors to convert
; Output:
;       none
;
; Note: CPal and GPal may point to the same buffer.
;
mxGammaCorrect  PROC    FAR
        ARG     Count:WORD,     \
                DPal:DWORD,     \
                SPal:DWORD      = ARG_SIZE
        ASSUME  ds:NOTHING
        .enter  0
        .push   ds, si, es, di

        mov     cx, [Count]
        jcxz    @@Exit                  ; Exit now if nothing to do
        lds     si, [SPal]
        les     di, [DPal]
        mov     bx, OFFSET mx_tblGamma  ; Setup BX for XLAT instruction
        cld
        mov     ax, cx                  ; AX = Count
        add     cx, cx                  ; CX = Count*2
        add     cx, ax                  ; CX = Count*3
@@Loop:
        lodsb
        xlat    mx_tblGamma
        stosb
        loop    @@Loop

@@Exit:
        .pop    ds, si, es, di
        .leave  ARG_SIZE
mxGammaCorrect  ENDP

MX_TEXT         ENDS
END
