;-----------------------------------------------------------
;
; MXIT.ASM - Initialization/termination functions
; Copyright (c) 1993,1994 by Alessandro Scotti
;
;-----------------------------------------------------------
;WARN    PRO
INCLUDE MODEX.DEF

PUBLIC  mxInit
PUBLIC  mxTerm

PUBLIC  mx_VideoSegment
PUBLIC  mx_CodeSegment

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

DB      'MODEX library - Copyright (c) 1992-1994 Alessandro Scotti'

mx_VideoSegment DW      0A000h
mx_CodeSegment  DW      SEG MX_TEXT

;-----------------------------------------------------------
;
; Initialization.
;
; Input:
;       none
; Output:
;       AX      = 0 on success
;
mxInit  PROC FAR
        LOCAL   Result:WORD,            \
                VideoSeg:WORD,          \
                CStoDSalias:WORD        = AUTO_SIZE
        ASSUME  ds:NOTHING
        .enter  AUTO_SIZE
        .push   ds, si, es, di

        mov     [Result], -1            ; Assume an error
        mov     [VideoSeg], 0A000h      ; Real mode video segment
        mov     [CStoDSalias], cs       ; Real mode data alias for CS

; Check if running in protected mode under DPMI
        mov     ax, 1686h
        int     2Fh
        or      ax, ax
         jnz    @@1                     ; DPMI not found, continue in real mode

; Get a data alias for CS
        mov     ax, 000Ah               ; DMPI: create data alias
        mov     bx, cs
        int     31h
         jc     @@Exit                  ; Exit if service failed
        mov     [CStoDSalias], ax       ; Save data alias for CS
; Get a protected-mode selector for the video segment
        mov     ax, 0002h
        mov     bx, 0A000h              ; Real mode segment of video
        int     31h                     ; DPMI: get segment selector
         jc     @@Exit                  ; Exit if service failed
        mov     [VideoSeg], ax          ; Save protected mode video selector

; Initialize variables
@@1:
        mov     ds, [CStoDSalias]
        ASSUME  ds:MX_TEXT
        mov     [mx_CodeSegment], ds
        mov     ax, [VideoSeg]
        mov     [mx_VideoSegment], ax

; Don't bother with VGA check for now...

        mov     [Result], 0

@@Exit:
        mov     ax, [Result]
        .pop    ds, si, es, di
;        .leave
mxInit  ENDP

;-----------------------------------------------------------
;
; Termination.
;
; Input:
;       none
; Output:
;       always 0.
;
mxTerm  PROC FAR
        ASSUME  ds:NOTHING
        xor     ax, ax
        ret
mxTerm  ENDP

MX_TEXT         ENDS
END
