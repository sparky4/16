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
; 3C4H, 03H
; Bit positions for font maps:
;
; +--7--+--6--+--5--+--4--+--3--+--2--+--1--+--0--+
; |     |     | SAH | SBH |    SA     |    SB     |
; +-----+-----+-----+-----+-----+-----+-----+-----+
;
; SA  - bits 1, 0 of Character Map A
; SAH - bit 2 (high order) of Charcter Map A
;
; SB  - bits 1, 0 of Character Map B
; SBH - bit 2 (high order) of Charcter Map B
;

; Points to fonts for Map A              Memory Address
DFM_MAPA_FONT1     EQU     00H          ;  0K
DFM_MAPA_FONT2     EQU     04H          ; 16K
DFM_MAPA_FONT3     EQU     08H          ; 32K
DFM_MAPA_FONT4     EQU     0CH          ; 48K
DFM_MAPA_FONT5     EQU     20H          ;  8K
DFM_MAPA_FONT6     EQU     24H          ; 24K
DFM_MAPA_FONT7     EQU     28H          ; 40K
DFM_MAPA_FONT8     EQU     2CH          ; 56K

; Points to fonts for Map B
DFM_MAPB_FONT1     EQU     00H          ;  0K
DFM_MAPB_FONT2     EQU     01H          ; 16K
DFM_MAPB_FONT3     EQU     02H          ; 32K
DFM_MAPB_FONT4     EQU     03H          ; 48K
DFM_MAPB_FONT5     EQU     10H          ;  8K
DFM_MAPB_FONT6     EQU     11H          ; 24K
DFM_MAPB_FONT7     EQU     12H          ; 40K
DFM_MAPB_FONT8     EQU     13H          ; 56K


; To set dual font mode, just set the character map select register to point
; to 2 different fonts.  There can be up to 8 fonts resident in memory at
; once but only 2 can be active at a time.
;
; ** NOTE ** If you set both font maps the same you are no longer in dual font
; mode.  But what this allows you to do is to put up to 8 fonts in VGA memory
; and switch between them at any time.
;
; Both fonts were made with my font editor.  I just read the BIOS and 
; modified the second font so a solid line goes through the middle of it.
;
; This procedure sets up dual font mode and loads 2 fonts into video memory.
SETDUAL PROC

   ; Sequencer address
   MOV DX,SEQ_ADDR

   ; 2 different font maps
   MOV AH,DFM_MAPA_FONT1
   OR AH,DFM_MAPB_FONT5

   ; Sequencer register
   MOV AL,03H

   ; Send it
   OUT DX,AX

   ; Load font 1 into 0K
   PUSH 16                 ; Bytes per character
   PUSH 0                  ; Offset of font in Video Memory
   PUSH 0A000H
   PUSH OFFSET FONT1       ; Offset of Font in our Data Segment
   PUSH DS
   CALL LOADFONT

   ; Load font 2 into 8K
   PUSH 16                 ; Bytes per character
   PUSH 0                  ; Offset of font in Video Memory
   PUSH 0A200H
   PUSH OFFSET FONT2       ; Offset of Font in our Data Segment
   PUSH DS
   CALL LOADFONT

   RET
SETDUAL ENDP

; This procedure reset the character map select register to point to only
; one font.
UNSETDUAL PROC

   ; Set both fonts to point to font 1
   MOV DX,SEQ_ADDR
   MOV AX,0003H
   OUT DX,AX

   RET
UNSETDUAL ENDP

; In dual font modes, the palette is in effect half of what it is.  The 
; bottom 8 colors for the first font, and the top 8 colors for the second 
; font.
;
; What I do here is to set them to the same 8 colors.
; Black, Blue, Green, Red, Purple, Yellow, Light Gray, and White
;
SETDUALPALETTE PROC

   PUSH 0
   PUSH 0
   PUSH 0
   PUSH 0
   CALL SETPAL

   PUSH 42
   PUSH 0
   PUSH 0
   PUSH 1
   CALL SETPAL

   PUSH 0
   PUSH 42
   PUSH 0
   PUSH 2
   CALL SETPAL

   PUSH 42
   PUSH 42
   PUSH 0
   PUSH 3
   CALL SETPAL

   PUSH 0
   PUSH 0
   PUSH 42
   PUSH 4
   CALL SETPAL

   PUSH 21
   PUSH 63
   PUSH 63
   PUSH 5
   CALL SETPAL

   PUSH 42
   PUSH 42
   PUSH 42
   PUSH 6
   CALL SETPAL

   PUSH 63
   PUSH 63
   PUSH 63
   PUSH 7
   CALL SETPAL

   PUSH 0
   PUSH 0
   PUSH 0
   PUSH 8
   CALL SETPAL

   PUSH 42
   PUSH 0
   PUSH 0
   PUSH 9
   CALL SETPAL

   PUSH 0
   PUSH 42
   PUSH 0
   PUSH 10
   CALL SETPAL

   PUSH 42
   PUSH 42
   PUSH 0
   PUSH 11
   CALL SETPAL

   PUSH 0
   PUSH 0
   PUSH 42
   PUSH 12
   CALL SETPAL

   PUSH 21
   PUSH 63
   PUSH 63
   PUSH 13
   CALL SETPAL

   PUSH 42
   PUSH 42
   PUSH 42
   PUSH 14
   CALL SETPAL

   PUSH 63
   PUSH 63
   PUSH 63
   PUSH 15
   CALL SETPAL

   RET
SETDUALPALETTE ENDP

DUALTEST PROC

   MOV SI,OFFSET MODE03H
   CALL SETMODE

   CALL SETDUAL
   CALL SETDUALPALETTE

   PUSH 1FH        
   CALL TEXTCLEAR

   ; Attribute to print text in.
   ;
   ; In dual font mode, attrubute bit 3 is the deciding factor as to which
   ; font is printed.  0001X111B where X is 0 for font 1 or 1 for font 2.
   ; The Charcter Map Select Register determines where font 1 and font 2
   ; reside.
   ; 00011111B - Font 2, Blue Background, with color 7 text.
   MOV ATT, 1Fh    

   MOV BX,5
   MOV AX,0
   MOV SI,OFFSET TEXTMSG_D
   CALL PRINTSTRING
   MOV BX,6
   MOV AX,0
   MOV SI,OFFSET TEXTMSG_E
   CALL PRINTSTRING
   MOV BX,7
   MOV AX,0
   MOV SI,OFFSET TEXTMSG_F
   CALL PRINTSTRING

   ; 00010111B - Font 1, Blue Background, with color 7 text.
   MOV ATT, 17h

   MOV BX,0
   MOV AX,0
   MOV SI,OFFSET TEXTMSG_D
   CALL PRINTSTRING
   MOV BX,1
   MOV AX,0
   MOV SI,OFFSET TEXTMSG_E
   CALL PRINTSTRING
   MOV BX,2
   MOV AX,0
   MOV SI,OFFSET TEXTMSG_F
   CALL PRINTSTRING

   ; Wait for a keypress
   MOV AH,0
   INT 16H

   CALL UNSETDUAL
   RET
DUALTEST ENDP

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
