; ====================================================================
; Entry points:
; ====================================================================
MODEX_START     MACRO
                mov     ax,13h  ;let the BIOS set standard 256-color
                int     10h     ; mode (320x200 linear)
;               PALETTE_BLACK
                call    ModifyForX
                ENDM

; ====================================================================
; This is MODE-X code from Dr. Dobb's Journal, by Michael Abrash.
; I modified it from 320x240 back to 320x200, and then to 512 virtual
; width, for scrolling purposes.
; ====================================================================

; Mode X (320x240, 256 colors) mode set routine. Works on all VGAs.
; ****************************************************************
; * Revised 6/19/91 to select correct clock; fixes vertical roll *
; * problems on fixed-frequency (IBM 851X-type) monitors.        *
; ****************************************************************
; Modified from public-domain mode set code by John Bridges.

; Index/data pairs for CRT Controller registers that differ between
; mode 13h and mode X.
CRTParms label  word
;       dw      00d06h  ;vertical total
;       dw      03e07h  ;overflow (bit 8 of vertical counts)
;       dw      04109h  ;cell height (2 to double-scan)
;       dw      0ea10h  ;v sync start
;       dw      0ac11h  ;v sync end and protect cr0-cr7
;       dw      0df12h  ;vertical displayed = 480
        dw      00014h  ;turn off dword mode                    *
;       dw      0e715h  ;v blank start
;       dw      00616h  ;v blank end
        dw      0e317h  ;turn on byte mode                      *

        dw      (VIRTUAL_WIDTH*32)+13h  ; width of screen = VWid   NEW
;       dw      09012h  ;vertical displayed = 400 (already like this)
CRT_PARM_LENGTH equ     (($-CRTParms)/2)

ModifyForX      PROC    near
                mov     dx,SC_INDEX
                mov     ax,0604h
                out     dx,ax           ;disable chain4 mode
                mov     ax,0100h
                out     dx,ax           ;synchronous reset while setting Misc
                                        ; Output for safety, even though clock
                                        ; unchanged
                mov     dx,MISC_OUTPUT
                mov     al,0e3h
                out     dx,al           ;select 25 MHz dot clock & 60 Hz scanning rate

                mov     dx,SC_INDEX
                mov     ax,0300h
                out     dx,ax           ;undo reset (restart sequencer)

                mov     dx,CRTC_INDEX   ;reprogram the CRT Controller
                mov     al,11h          ;VSync End reg contains register write
                out     dx,al           ; protect bit
                inc     dx              ;CRT Controller Data register
                in      al,dx           ;get current VSync End register setting
                and     al,7fh          ;remove write protect on various
                out     dx,al           ; CRTC registers
                dec     dx              ;CRT Controller Index
                cld
                push    cs
                pop     ds
                mov     si,offset CRTParms ;point to CRT parameter table
                mov     cx,CRT_PARM_LENGTH ;# of table entries
SetCRTParmsLoop:
                lodsw                   ;get the next CRT Index/Data pair
                out     dx,ax           ;set the next CRT Index/Data pair
                loop    SetCRTParmsLoop

                mov     dx,SC_INDEX
                mov     ax,0f02h
                out     dx,ax           ;enable writes to all four planes
                mov     ax,SCREEN_SEG   ;now clear all display memory, 8 pixels
                mov     es,ax           ; at a time
                sub     di,di           ;point ES:DI to display memory
                sub     ax,ax           ;clear to zero-value pixels
                mov     cx,8000h        ;# of words in display memory
            rep stosw                   ;clear all of display memory

                ret
ModifyForX      ENDP
