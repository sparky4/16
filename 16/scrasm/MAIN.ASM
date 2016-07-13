;;=======================================================================;;
;;                                                                       ;;
;; Scrolling Routines -- main program                                    ;;
;;                                                                       ;;
;; All other INC files are included here.  The main routines for the     ;;
;; frame-by-frame execution loop are also here.  Finally I tried to keep ;;
;; global variables stored in this file as well.                         ;;
;;                                                                       ;;
;;=======================================================================;;
                dosseg
                .model small
                .386

                .code
                extrn   ZTimerOn:far, ZTimerOff:far, ZTimerReport:far

INCLUDE constant.inc


DW_TABLE        MACRO   inc,num
                count = 0
                number = 0
                WHILE (count LT num)
                        DW      number
                        count = count + 1
                        number = number + inc
                        ENDM
                ENDM

DOSPRINT        MACRO   st
                mov     ah,9
                mov     dx,st
                int     21h
                ENDM

EVEN
Mult320         label   WORD
MultBufWidth    label   WORD
                DW_TABLE 320,200
MultVirtWidth   label   WORD
                DW_TABLE (VIRTUAL_WIDTH/4),200

INCLUDE palette.inc
INCLUDE keyb.inc
INCLUDE modex.inc
INCLUDE page.inc
INCLUDE init.inc
INCLUDE map.inc
;INCLUDE sprite.inc NOT FOR NOW
INCLUDE scroll.inc

;; Various segments that need to be filled in later...
EVEN
segVideo        dw      0A000h          ; videoram segment
segText         dw      0B800h          ; text segment
segMap          dw      -1              ; Map info segment
segTiles        dw      -1              ; Tile bitmap segment
segBuffer       dw      -1              ; Local 320x200 buffer segment
segCode         dw      -1              ; Code segment
segPSP          dw      -1              ; PSP segment
segPalette      dw      -1              ; Palette segment
segTextPal      dw      -1              ; Saved text palette

EVEN
bDoTransition   db      0

;; This routine is called for each frame.
;; Right now it just scrolls, but later all sprite animation would
;; occur here too.
EVEN
OneFrame        PROC    near
                call    Scroll          ; Scrolls the screen
;               call    AnimateSprites  ; prepares sprites on drawpage
                jmp     FlipPage        ; shows drawpage...
                ; no RET necessary
OneFrame        ENDP

;; Each frame -- call the frame motion code, then check for keyhit.
EVEN
MainLoop        PROC    NEAR
next_frame:     call    OneFrame
                JNKEY   next_frame
                JKEYP   kESC,all_done   ; ESC -> quit, always
                call    kprocCur
                mov     al,bDoTransition
                cmp     al,0
                je      next_frame
transition:     FLASH_OFF 16,segPalette
                mov     bDoTransition,0
                mov     ax,1
                sub     ax,nMap
                mov     nMap,ax         ; Flip maps

                call    LoadData
                call    update_full     ;<<<<
                call    OneFrame
                FLASH_ON 16,segPalette
                jmp     next_frame
all_done:       ret
MainLoop        ENDP

;; Beginning code -- Leaves text mode (saving the text screen) via
;;                   a fade.  It loads the map data and draws one
;;                   frame before it fades on.
Beginning       PROC    near
                NEW_PAL segTextPal
                PAL_SAVE segTextPal
                FADE_OFF 1,segTextPal
                call    SaveVideo
                MODEX_START             ; 320x200 Mode X graphics mode
                PAL_BLACK

                call    LoadData        ; This call will change...

                call    update_full     ;<<<<
                call    OneFrame
                FADE_ON 1,segPalette
                ret
Beginning       ENDP

;; Ending code -- restore to text mode via a flash
Ending          PROC    near
                FLASH_OFF 8,segPalette
                call    RestoreVideo
                FLASH_ON 8,segTextPal
                ret
Ending          ENDP

                .data

                .stack 2048

                END Initialize
