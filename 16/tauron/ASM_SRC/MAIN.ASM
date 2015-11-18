;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;=-                                                                         -=
;=-                   Tauron VGA Utilities Version 3.0                      -=
;=-                      Released September 20, 1998                        -=
;=-                                                                         -=
;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;=- Copyright (c) 1997, 1998 by Jeff Morgan  =-= This code is FREE provided -=
;=- All Rights Reserved.                     =-= that you put my name some- -=
;=-                                          =-= where in your credits.     -=
;=- DISCLAIMER:                              =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;=- I assume no responsibility whatsoever for any effect that this package, -=
;=- the information contained therein or the use thereof has on you, your   -=
;=- sanity, computer, spouse, children, pets or anything else related to    -=
;=- you or your existance. No warranty is provided nor implied with this    -=
;=- source code.                                                            -=
;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
.MODEL  SMALL
.386
JUMPS
LOCALS
.STACK  200h
.DATA

ATTRCON_ADDR      EQU       03C0H
MISC_ADDR         EQU       03C2H
VGAENABLE_ADDR    EQU       03C3H
SEQ_ADDR          EQU       03C4H
GRACON_ADDR       EQU       03CEH
CRTC_ADDR         EQU       03D4H
STATUS_ADDR       EQU       03DAH

; Videomode defines
VMODE_00H         EQU       00H
VMODE_01H         EQU       00H
VMODE_02H         EQU       03H
VMODE_03H         EQU       03H
VMODE_04H         EQU       04H
VMODE_05H         EQU       04H
VMODE_06H         EQU       06H
VMODE_07H         EQU       07H
VMODE_0DH         EQU       0DH
VMODE_0EH         EQU       0EH
VMODE_0FH         EQU       0FH
VMODE_10H         EQU       10H
VMODE_11H         EQU       11H
VMODE_12H         EQU       12H
VMODE_13H         EQU       13H
VMODE_CHAIN4      EQU       14H
VMODE_MODE_X      EQU       15H
VMODE_MODE_A      EQU       16H
VMODE_MODE_B      EQU       17H
VMODE_MODE_C      EQU       18H
VMODE_MODE_D      EQU       19H
VMODE_MODE_E      EQU       1AH
VMODE_MODE_F      EQU       1BH
VMODE_MODE_G      EQU       1CH
VMODE_MODE_H      EQU       1DH
VMODE_MODE_I      EQU       1EH
VMODE_MODE_J      EQU       1FH
VMODE_MODE_K      EQU       20H
VMODE_MODE_L      EQU       21H
VMODE_MODE_M      EQU       22H
VMODE_MODE_N      EQU       23H

;These next four lines must be included for the text mode stuff to work.
oldmode db ?
oldmisc db ?
oldmask db ?
oldmem db ?

PALETTE LABEL BYTE
DB  0,  0,  0,  0,  0, 42,  0, 42,  0,  0, 42, 42, 42,  0,  0, 42,  0, 42
DB 42, 21,  0, 42, 42, 42, 21, 21, 21, 21, 21, 63, 21, 63, 21, 21, 63, 63
DB 63, 21, 21, 63, 21, 63, 63, 63, 21, 63, 63, 63,  0,  0,  0,  5,  5,  5
DB  8,  8,  8, 11, 11, 11, 14, 14, 14, 17, 17, 17, 20, 20, 20, 24, 24, 24
DB 28, 28, 28, 32, 32, 32, 36, 36, 36, 40, 40, 40, 45, 45, 45, 50, 50, 50
DB 56, 56, 56, 63, 63, 63,  0,  0, 63, 16,  0, 63, 31,  0, 63, 47,  0, 63
DB 63,  0, 63, 63,  0, 47, 63,  0, 31, 63,  0, 16, 63,  0,  0, 63, 16,  0
DB 63, 31,  0, 63, 47,  0, 63, 63,  0, 47, 63,  0, 31, 63,  0, 16, 63,  0
DB  0, 63,  0,  0, 63, 16,  0, 63, 31,  0, 63, 47,  0, 63, 63,  0, 47, 63
DB  0, 31, 63,  0, 16, 63, 31, 31, 63, 39, 31, 63, 47, 31, 63, 55, 31, 63
DB 63, 31, 63, 63, 31, 55, 63, 31, 47, 63, 31, 39, 63, 31, 31, 63, 39, 31
DB 63, 47, 31, 63, 55, 31, 63, 63, 31, 55, 63, 31, 47, 63, 31, 39, 63, 31
DB 31, 63, 31, 31, 63, 39, 31, 63, 47, 31, 63, 55, 31, 63, 63, 31, 55, 63
DB 31, 47, 63, 31, 39, 63, 45, 45, 63, 49, 45, 63, 54, 45, 63, 58, 45, 63
DB 63, 45, 63, 63, 45, 58, 63, 45, 54, 63, 45, 49, 63, 45, 45, 63, 49, 45
DB 63, 54, 45, 63, 58, 45, 63, 63, 45, 58, 63, 45, 54, 63, 45, 49, 63, 45
DB 45, 63, 45, 45, 63, 49, 45, 63, 54, 45, 63, 58, 45, 63, 63, 45, 58, 63
DB 45, 54, 63, 45, 49, 63,  0,  0, 28,  7,  0, 28, 14,  0, 28, 21,  0, 28
DB 28,  0, 28, 28,  0, 21, 28,  0, 14, 28,  0,  7, 28,  0,  0, 28,  7,  0
DB 28, 14,  0, 28, 21,  0, 28, 28,  0, 21, 28,  0, 14, 28,  0,  7, 28,  0
DB  0, 28,  0,  0, 28,  7,  0, 28, 14,  0, 28, 21,  0, 28, 28,  0, 21, 28
DB  0, 14, 28,  0,  7, 28, 14, 14, 28, 17, 14, 28, 21, 14, 28, 24, 14, 28
DB 28, 14, 28, 28, 14, 24, 28, 14, 21, 28, 14, 17, 28, 14, 14, 28, 17, 14
DB 28, 21, 14, 28, 24, 14, 28, 28, 14, 24, 28, 14, 21, 28, 14, 17, 28, 14
DB 14, 28, 14, 14, 28, 17, 14, 28, 21, 14, 28, 24, 14, 28, 28, 14, 24, 28
DB 14, 21, 28, 14, 17, 28, 20, 20, 28, 22, 20, 28, 24, 20, 28, 26, 20, 28
DB 28, 20, 28, 28, 20, 26, 28, 20, 24, 28, 20, 22, 28, 20, 20, 28, 22, 20
DB 28, 24, 20, 28, 26, 20, 28, 28, 20, 26, 28, 20, 24, 28, 20, 22, 28, 20
DB 20, 28, 20, 20, 28, 22, 20, 28, 24, 20, 28, 26, 20, 28, 28, 20, 26, 28
DB 20, 24, 28, 20, 22, 28,  0,  0, 16,  4,  0, 16,  8,  0, 16, 12,  0, 16
DB 16,  0, 16, 16,  0, 12, 16,  0,  8, 16,  0,  4, 16,  0,  0, 16,  4,  0
DB 16,  8,  0, 16, 12,  0, 16, 16,  0, 12, 16,  0,  8, 16,  0,  4, 16,  0
DB  0, 16,  0,  0, 16,  4,  0, 16,  8,  0, 16, 12,  0, 16, 16,  0, 12, 16
DB  0,  8, 16,  0,  4, 16,  8,  8, 16, 10,  8, 16, 12,  8, 16, 14,  8, 16
DB 16,  8, 16, 16,  8, 14, 16,  8, 12, 16,  8, 10, 16,  8,  8, 16, 10,  8
DB 16, 12,  8, 16, 14,  8, 16, 16,  8, 14, 16,  8, 12, 16,  8, 10, 16,  8
DB  8, 16,  8,  8, 16, 10,  8, 16, 12,  8, 16, 14,  8, 16, 16,  8, 14, 16
DB  8, 12, 16,  8, 10, 16, 11, 11, 16, 12, 11, 16, 13, 11, 16, 15, 11, 16
DB 16, 11, 16, 16, 11, 15, 16, 11, 13, 16, 11, 12, 16, 11, 11, 16, 12, 11
DB 16, 13, 11, 16, 15, 11, 16, 16, 11, 15, 16, 11, 13, 16, 11, 12, 16, 11
DB 11, 16, 11, 11, 16, 12, 11, 16, 13, 11, 16, 15, 11, 16, 16, 11, 15, 16
DB 11, 13, 16, 11, 12, 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
DB  0,  0,  0,  0,  0,  0,  0,  0,  0, 63, 63, 63

; Must be included for any videomode to be set.
include modes.inc

MAIN_MSG LABEL BYTE
DB 'Tauron VGA Utilities v3.0',13,10
DB 'Copyright (c) 1998 Jeffrey Morgan.  All Rights Reserved.',13,10
DB 'Select a video mode to test: (Modes with a ! are not implemented)',13,10
DB 'Key              Videomode   (Modes with a % are unchained)',13,10
DB '================================================================================'
DB ' 0) 00H, 01H -- 40 x 25 x 16         %F) Mode D   --  360 x 200 x 256',13,10
DB ' 1) Mode L   -- 40 x 43 x 16         %G) Mode E   --  360 x 240 x 256',13,10
DB ' 2) Mode M   -- 40 x 50 x 16         %H) Mode F   --  360 x 350 x 256',13,10
DB '!3) 07H      -- 80 x 25 x 2          %I) Mode G   --  360 x 400 x 256',13,10
DB ' 4) 02H, 03H -- 80 x 25 x 16         %J) Mode H   --  360 x 480 x 256',13,10
DB ' 5) Mode J   -- 80 x 43 x 16          K) 06H      -- 640 x 200 x 2',13,10
DB ' 6) Mode K   -- 80 x 50 x 16         !L) 0FH      -- 640 x 350 x 2',13,10 
DB ' 7) 04H, 05H -- 320 x 200 x 4         M) 11H      -- 640 x 480 x 2',13,10 
DB ' 8) 0DH      -- 320 x 200 x 16        N) 0EH      -- 640 x 200 x 16',13,10
DB ' 9) 13H      -- 320 x 200 x 256       O) 10H      -- 640 x 350 x 16',13,10
DB '%A) Chain 4  --  320 x 200 x 256      P) Mode I   -- 640 x 400 x 16',13,10
DB '%B) Mode X   --  320 x 240 x 256      Q) 12H      -- 640 x 480 x 16',13,10
DB '%C) Mode A   --  320 x 350 x 256',13,10
DB '%D) Mode B   --  320 x 400 x 256      R) Dual Font Test',13,10
DB '%E) Mode C   --  320 x 480 x 256',13,10
DB '===============================================================================',13,10
DB 'Hit ',39,'ESC',39,' to exit.  Press a key [0-9 or A-O]>$'

TEXTMSG_1 DB 218,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,191,0
TEXTMSG_2 DB 179,'Text Mode: 00H        ',179,0 ; +13
TEXTMSG_3 DB 179,'Width: 80   Height: 25',179,0 ; +8
TEXTMSG_4 DB 179,'Bytes per screen: 4000',179,0 ; +19
TEXTMSG_5 DB 179,'Number of pages: 8    ',179,0
TEXTMSG_6 DB 192,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,217,0

TEXTMSG_7 DB 218,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,191,0
TEXTMSG_8 DB 179,'Text Mode J           ',179,0 ; +11
TEXTMSG_9 DB 179,'Width: 80   Height: 43',179,0 ; +21, +22
TEXTMSG_A DB 179,'Bytes per screen: 3440',179,0 ; +19, +20, +21, +22
TEXTMSG_B DB 179,'Number of pages: 8    ',179,0
TEXTMSG_C DB 192,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,217,0

TEXTMSG_D DB 218,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,191,0
TEXTMSG_E DB 179,' DUAL FONT MODE ',179,0 ;
TEXTMSG_F DB 192,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,217,0

FONT_8x8_OFF DW ?
FONT_8x14_OFF DW ?

; Attribute for text charcters
ATT DB 1FH

FONT1 LABEL BYTE
include font1.asm
FONT2 LABEL BYTE
include font2.asm
.CODE

IODELAY MACRO
   REPT 8
      JMP $+2
   ENDM
ENDM

Start:
   MOV AX,@DATA
   MOV DS,AX

   ; This must be here to ensure that the fonts are properly found because
   ; the location of the fonts in ROM is different on every VGA BIOS.
   ; Get location of the fonts for Modes J and K
   MOV AH,11h
   MOV AL,30h
   MOV BH,03h
   INT 10H
   MOV FONT_8x8_OFF,BP

   ; Get location of the fonts for Modes 00H, 01H, 02H, 03H, and 07H
   MOV AH,11h
   MOV AL,30h
   MOV BH,06h
   INT 10H
   MOV FONT_8x14_OFF,BP

   ; Reset the mode and clear the screen
   MOV AX,0003H
   INT 10H

   ; Print out the message
   MOV DX,OFFSET MAIN_MSG
   MOV AH,9
   INT 21H

MAIN_LOOP:

   ; Wait for a keypress
   MOV AH,0
   INT 16H

   CMP AL,27
   JE QUIT_PROGRAM

   CMP AL,'0'
   JE MODE_00_TEST

   CMP AL,'1'
   JE MODE_L_TEST

   CMP AL,'2'
   JE MODE_M_TEST

   CMP AL,'4'
   JE MODE_03_TEST

   CMP AL,'5'
   JE MODE_J_TEST

   CMP AL,'6'
   JE MODE_K_TEST

   CMP AL,'7'
   JE MODE_04_TEST

   CMP AL,'8'
   JE MODE_0D_TEST

   CMP AL,'9'
   JE MODE_13_TEST

   CMP AL,'a'
   JE MODE_C4_TEST
   CMP AL,'A'
   JE MODE_C4_TEST

   CMP AL,'b'
   JE MODE_X_TEST
   CMP AL,'B'
   JE MODE_X_TEST

   CMP AL,'c'
   JE MODE_A_TEST
   CMP AL,'C'
   JE MODE_A_TEST

   CMP AL,'d'
   JE MODE_B_TEST
   CMP AL,'D'
   JE MODE_B_TEST

   CMP AL,'e'
   JE MODE_C_TEST
   CMP AL,'E'
   JE MODE_C_TEST

   CMP AL,'f'
   JE MODE_D_TEST
   CMP AL,'F'
   JE MODE_D_TEST

   CMP AL,'g'
   JE MODE_E_TEST
   CMP AL,'G'
   JE MODE_E_TEST

   CMP AL,'h'
   JE MODE_F_TEST
   CMP AL,'H'
   JE MODE_F_TEST

   CMP AL,'i'
   JE MODE_G_TEST
   CMP AL,'I'
   JE MODE_G_TEST

   CMP AL,'j'
   JE MODE_H_TEST
   CMP AL,'J'
   JE MODE_H_TEST

   CMP AL,'k'
   JE MODE_06_TEST
   CMP AL,'K'
   JE MODE_06_TEST

   CMP AL,'m'
   JE MODE_11_TEST
   CMP AL,'M'
   JE MODE_11_TEST

   CMP AL,'n'
   JE MODE_0E_TEST
   CMP AL,'N'
   JE MODE_0E_TEST

   CMP AL,'o'
   JE MODE_10_TEST
   CMP AL,'O'
   JE MODE_10_TEST

   CMP AL,'p'
   JE MODE_I_TEST
   CMP AL,'P'
   JE MODE_I_TEST

   CMP AL,'q'
   JE MODE_12_TEST
   CMP AL,'Q'
   JE MODE_12_TEST

   CMP AL,'r'
   JE DUAL_TEST
   CMP AL,'R'
   JE DUAL_TEST
   JMP MAIN_LOOP

; ====================== New Video Mode ====================================
MODE_00_TEST:
   PUSH VMODE_00H
   CALL SETVIDEOMODE

   CALL TEXTTEST

   ; Wait for a keypress
   MOV AH,0
   INT 16H
   JMP RESET

; ====================== New Video Mode ====================================
MODE_03_TEST:
   PUSH VMODE_03H
   CALL SETVIDEOMODE

   CALL TEXTTEST

   ; Wait for a keypress
   MOV AH,0
   INT 16H
   JMP RESET

; ====================== New Video Mode ====================================
MODE_04_TEST:
   PUSH VMODE_04H
   CALL SETVIDEOMODE

   PUSH 0AAAAH
   CALL CLEAR_B8000H

   ; Wait for a keypress
   MOV AH,0
   INT 16H
   JMP RESET

; ====================== New Video Mode ====================================
MODE_06_TEST:
   PUSH VMODE_06H
   CALL SETVIDEOMODE

   PUSH 0FFFFH
   CALL CLEAR_B8000H

   ; Wait for a keypress
   MOV AH,0
   INT 16H
   JMP RESET

; ====================== New Video Mode ====================================
MODE_0D_TEST:
   PUSH VMODE_0DH
   CALL SETVIDEOMODE

   PUSH 12
   CALL CLEAR0DH

   ; Wait for a keypress
   MOV AH,0
   INT 16H
   JMP RESET

; ====================== New Video Mode ====================================
MODE_0E_TEST:
   PUSH VMODE_0EH
   CALL SETVIDEOMODE

   CALL MODETEST

   ; Wait for a keypress
   MOV AH,0
   INT 16H
   JMP RESET

; ====================== New Video Mode ====================================
MODE_10_TEST:
   PUSH VMODE_10H
   CALL SETVIDEOMODE

   CALL MODETEST

   ; Wait for a keypress
   MOV AH,0
   INT 16H
   JMP RESET

; ====================== New Video Mode ====================================
MODE_11_TEST:
   PUSH VMODE_11H
   CALL SETVIDEOMODE

   CALL MODETEST

   ; Wait for a keypress
   MOV AH,0
   INT 16H
   JMP RESET

; ====================== New Video Mode ====================================
MODE_12_TEST:
   PUSH VMODE_12H
   CALL SETVIDEOMODE

   CALL MODETEST

   ; Wait for a keypress
   MOV AH,0
   INT 16H
   JMP RESET

; ====================== New Video Mode ====================================
MODE_13_TEST:
   PUSH VMODE_13H
   CALL SETVIDEOMODE

   CALL MODETEST

   ; Wait for a keypress
   MOV AH,0
   INT 16H
   JMP RESET

; ====================== New Video Mode ====================================
MODE_I_TEST:
   PUSH VMODE_MODE_I
   CALL SETVIDEOMODE

   CALL MODETEST

   ; Wait for a keypress
   MOV AH,0
   INT 16H
   JMP RESET

; ====================== New Video Mode ====================================
MODE_C4_TEST:
   PUSH VMODE_CHAIN4
   CALL SETVIDEOMODE

   CALL MODETEST

   ; Wait for a keypress
   MOV AH,0
   INT 16H
   JMP RESET

; ====================== New Video Mode ====================================
MODE_X_TEST:
   PUSH VMODE_MODE_X
   CALL SETVIDEOMODE

   CALL MODETEST

   ; Wait for a keypress
   MOV AH,0
   INT 16H
   JMP RESET

; ====================== New Video Mode ====================================
MODE_A_TEST:
   PUSH VMODE_MODE_A
   CALL SETVIDEOMODE

   CALL MODETEST

   ; Wait for a keypress
   MOV AH,0
   INT 16H
   JMP RESET

; ====================== New Video Mode ====================================
MODE_B_TEST:
   PUSH VMODE_MODE_B
   CALL SETVIDEOMODE

   CALL MODETEST

   ; Wait for a keypress
   MOV AH,0
   INT 16H
   JMP RESET

; ====================== New Video Mode ====================================
MODE_C_TEST:
   PUSH VMODE_MODE_C
   CALL SETVIDEOMODE

   CALL MODETEST

   ; Wait for a keypress
   MOV AH,0
   INT 16H
   JMP RESET

; ====================== New Video Mode ====================================
MODE_D_TEST:
   PUSH VMODE_MODE_D
   CALL SETVIDEOMODE

   CALL MODETEST

   ; Wait for a keypress
   MOV AH,0
   INT 16H
   JMP RESET

; ====================== New Video Mode ====================================
MODE_E_TEST:
   PUSH VMODE_MODE_E
   CALL SETVIDEOMODE

   CALL MODETEST

   ; Wait for a keypress
   MOV AH,0
   INT 16H
   JMP RESET

; ====================== New Video Mode ====================================
MODE_F_TEST:
   PUSH VMODE_MODE_F
   CALL SETVIDEOMODE

   CALL MODETEST

   ; Wait for a keypress
   MOV AH,0
   INT 16H
   JMP RESET

; ====================== New Video Mode ====================================
MODE_G_TEST:
   PUSH VMODE_MODE_G
   CALL SETVIDEOMODE

   CALL MODETEST

   ; Wait for a keypress
   MOV AH,0
   INT 16H
   JMP RESET

; ====================== New Video Mode ====================================
MODE_H_TEST:
   PUSH VMODE_MODE_H
   CALL SETVIDEOMODE

   CALL MODETEST

   ; Wait for a keypress
   MOV AH,0
   INT 16H
   JMP RESET

; ====================== New Video Mode ====================================
MODE_J_TEST:
   PUSH VMODE_MODE_J
   CALL SETVIDEOMODE

   CALL TEXTTEST

   ; Wait for a keypress
   MOV AH,0
   INT 16H
   JMP RESET

; ====================== New Video Mode ====================================
MODE_K_TEST:
   PUSH VMODE_MODE_K
   CALL SETVIDEOMODE

   CALL TEXTTEST

   ; Wait for a keypress
   MOV AH,0
   INT 16H
   JMP RESET

; ====================== New Video Mode ====================================
MODE_L_TEST:
   PUSH VMODE_MODE_L
   CALL SETVIDEOMODE

   CALL TEXTTEST

   ; Wait for a keypress
   MOV AH,0
   INT 16H
   JMP RESET

; ====================== New Video Mode ====================================
MODE_M_TEST:
   PUSH VMODE_MODE_M
   CALL SETVIDEOMODE

   CALL TEXTTEST

   ; Wait for a keypress
   MOV AH,0
   INT 16H
   JMP RESET

; ====================== New Video Mode ====================================
DUAL_TEST:
   CALL DUALTEST

RESET:
   ; Reset the mode and clear the screen
   MOV AX,0003H
   INT 10H

   ; Print out the message
   MOV DX,OFFSET MAIN_MSG
   MOV AH,9
   INT 21H
   JMP MAIN_LOOP

; ====================== New Video Mode ====================================
QUIT_PROGRAM:
   ; Set the BIOS mode 03h
   MOV AX,0003H
   INT 10H

   ; Exit to DOS
   MOV AX,4C00H
   INT 21H

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

LOADFONT PROC
   PUSH BP
   MOV BP,SP
   PUSHA
 mov dx,GRACON_ADDR                     ;get graphics port
 mov al,5                               ;get write mode reg
 out dx,al                              ;select the reg
 IODELAY                                ;delay a bit
 inc dx                                 ;change DX
 in al,dx                               ;get value
 IODELAY                                ;pause
 mov [oldmode],al                       ;store it
 dec dx                                 ;restore DX
 mov al,6                               ;get misc reg
 out dx,al                              ;select the reg
 IODELAY                                ;delay a bit
 inc dx                                 ;change DX
 in al,dx                               ;get value
 IODELAY                                ;pause
 mov [oldmisc],al                       ;store it
 dec dx                                 ;restore DX
 mov dx,SEQ_ADDR                        ;get sequencer port
 mov al,2                               ;get map mask reg
 out dx,al                              ;select the reg
 IODELAY                                ;delay a bit
 inc dx                                 ;change DX
 in al,dx                               ;get value
 IODELAY                                ;pause
 mov [oldmask],al                       ;store it
 dec dx                                 ;restore DX
 mov al,4                               ;get memory selector reg
 out dx,al                              ;select the reg
 IODELAY                                ;delay a bit
 inc dx                                 ;change DX
 in al,dx                               ;get value
 IODELAY                                ;pause
 mov [oldmem],al                        ;store it

 mov dx,GRACON_ADDR                     ;select graphics port
 mov al,5                               ;get write mode reg
 mov ah,[oldmode]                       ;get old value
 and ah,0fch                            ;mask it
 out dx,ax                              ;set new value
 IODELAY                                ;pause
 mov al,6                               ;get misc reg
 mov ah,[oldmisc]                       ;get old value
 and ah,0f1h                            ;mask it
 or ah,4                                ;set a flag
 out dx,ax                              ;set it
 IODELAY                                ;pause
 mov dx,SEQ_ADDR                        ;select sequencer port
 mov al,2                             ;get mask reg
 mov ah,4                               ;get new value
 out dx,ax                              ;set it
 IODELAY                                ;pause
 mov al,4                              ;get memory reg
 mov ah,[oldmem]                        ;get old value
 or ah,4                                ;set flag
 out dx,ax                              ;set value
 IODELAY                                ;pause

   PUSH DS
   PUSH ES

   MOV AX,[BP+8]
   MOV ES,AX
   MOV DI,[BP+10]

   MOV AX,[BP+4]
   MOV DS,AX
   MOV SI,[BP+6]

@@LOOP1:
   PUSH DI
   MOV CL,[BP+12]
@@LOOP2:
   MOVSB
   DEC CL
   JNZ @@LOOP2

   MOV CL,32
   SUB CL,byte ptr [BP+12]
@@LOOP3:
   MOV AX,00H
   STOSB
   DEC CL
   JNZ @@LOOP3

   POP DI
   ADD DI,32

   DEC CH
   JNZ @@LOOP1

   POP ES
   POP DS

 mov dx,GRACON_ADDR                     ;get graphics port
 mov al,5                               ;get 1st reg
 mov ah,[oldmode]                       ;get old value
 out dx,ax                              ;set it
 IODELAY                                ;pause
 mov al,6                               ;get 2nd reg
 mov ah,[oldmisc]                       ;get old value
 out dx,ax                              ;set it
 IODELAY                                ;pause
 mov dx,SEQ_ADDR                        ;get seq port
 mov al,2                               ;get 3rd reg
 mov ah,[oldmask]                       ;get old value
 out dx,ax                              ;set it
 IODELAY                                ;pause
 mov al,4                               ;get 4th reg
 mov ah,[oldmem]                        ;get old value
 out dx,ax                              ;set it
 IODELAY                                ;pause

   POPA
   POP BP
   RET 10
LOADFONT ENDP

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
include clear.asm
include modes.asm
include tests.asm
include dual.asm
;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

END Start
