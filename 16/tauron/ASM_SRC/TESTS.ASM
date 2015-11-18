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

PIXEL PROC
   PUSH BP
   MOV BP,SP
   PUSHA

   TEST Mode.attrib,TVU_PLANAR
   JNZ PLANAR_PIXEL
   CMP Mode.videomode,VMODE_13H
   JE MODE_13H_PIXEL

; Unchained Modes Pixel
   MOV AX,0A000H   ;    video memory segment number
   MOV ES,AX       ;    place it in es

   MOV DX,03C4H
   MOV AL,2
   OUT DX,AL
   INC DX

   MOV AL,1
   MOV CX,word ptr [bp+4]
   AND CX,3
   SHL AL,CL
   OUT DX,AL

   XOR DI,DI

; Calculate the Offset
   mov ax,Mode.mode_width  ; width / 4
   SHR AX,2
   mul word ptr [bp+6]         ; (Y * (width / 4))
   mov bx,word ptr [bp+4]      ; (X / 4) + (Y * (width / 4))
   shr bx,2
   add ax,bx
; Done!

   ADD DI,AX
   mov ah,byte ptr [bp+8]   ;    move the Color into ah
   mov es:[di],ah ;    move the value to the screen
   JMP PIXEL_OUT

PLANAR_PIXEL:
   MOV AX,0A000H   ;    video memory segment number
   MOV ES,AX       ;    place it in es

   MOV BX,word ptr [bp+4]        ;    X Value
   MOV CX,BX
   MOV AX,word ptr [bp+6]        ;    Y Value
   MOV SI,80
   MUL SI
   SHR BX,3        ;    /8
   ADD AX,BX
   MOV DI,AX

   AND CL,7
   XOR CL,7
   MOV AH,1

   SHL AH,CL

   MOV DX,03CEH
   MOV AL,8
   OUT DX,AX

   MOV AL,0
   XCHG ES:[DI],AL

   MOV DX,03C4H
   MOV AH,byte ptr [bp+8]
   MOV AL,2
   OUT DX,AX

   MOV BYTE PTR ES:[DI],0FFh

   MOV AX,0F02H
   OUT DX,AX

   MOV DX,03CEH
   MOV AX,0FF08h
   OUT DX,AX
   JMP PIXEL_OUT

MODE_13H_PIXEL:

   MOV AX,0A000H   ;    video memory segment number
   MOV ES,AX       ;    place it in es
   XOR DI,DI

; Calculate the Offset
   mov ax,Mode.mode_width      ; width
   mul word ptr [bp+6]         ; (Y * width))
   mov bx,word ptr [bp+4]      ; (X + (Y * width))
   add ax,bx
; Done!

   ADD DI,AX
   mov ah,byte ptr [bp+8]   ;    move the Color into ah
   mov es:[di],ah ;    move the value to the screen

PIXEL_OUT:

   POPA
   POP BP
   RET 6
PIXEL ENDP

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

HLINE PROC
   PUSH BP
   MOV BP,SP
   PUSHA

   MOV CX,WORD PTR [BP+4]
HLINE_LOOP:
   PUSH WORD PTR [BP+10]   ; Color
   PUSH WORD PTR [BP+8]    ; Y value
   PUSH CX                 ; X value
   CALL PIXEL
   INC CX
   CMP CX,WORD PTR [BP+6]
   JLE HLINE_LOOP

   POPA
   POP BP
   RET 8
HLINE ENDP

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

VLINE PROC
   PUSH BP
   MOV BP,SP
   PUSHA

   MOV CX,WORD PTR [BP+4]
VLINE_LOOP:
   PUSH WORD PTR [BP+10]   ; Color
   PUSH CX                 ; Y value
   PUSH WORD PTR [BP+8]    ; X value
   CALL PIXEL
   INC CX
   CMP CX,WORD PTR [BP+6]
   JLE VLINE_LOOP

   POPA
   POP BP
   RET 8
VLINE ENDP

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

DRAWRECT PROC
   PUSH BP
   MOV BP,SP

   PUSH WORD PTR [BP+12]
   PUSH WORD PTR [BP+6]
   PUSH WORD PTR [BP+8]
   PUSH WORD PTR [BP+4]
   CALL HLINE

   PUSH WORD PTR [BP+12]
   PUSH WORD PTR [BP+10]
   PUSH WORD PTR [BP+8]
   PUSH WORD PTR [BP+4]
   CALL HLINE

   PUSH WORD PTR [BP+12]
   PUSH WORD PTR [BP+4]
   PUSH WORD PTR [BP+10]
   PUSH WORD PTR [BP+6]
   CALL VLINE

   PUSH WORD PTR [BP+12]
   PUSH WORD PTR [BP+8]
   PUSH WORD PTR [BP+10]
   PUSH WORD PTR [BP+6]
   CALL VLINE

   POP BP
   RET 10
DRAWRECT ENDP

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

FILLRECT PROC
   PUSH BP
   MOV BP,SP
   PUSHA

   MOV CX,WORD PTR [BP+6]
HLINE_LOOP2:
   PUSH WORD PTR [BP+12]   ; Color
   PUSH CX                 ; Y1 value
   PUSH WORD PTR [BP+8]    ; X2 value
   PUSH WORD PTR [BP+4]    ; X1 value
   CALL HLINE

   INC CX
   CMP CX,WORD PTR [BP+10] ; Y2 value
   JLE HLINE_LOOP2

   POPA
   POP BP
   RET 10
FILLRECT ENDP

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

MODETEST PROC
   PUSH BP
   MOV BP,SP
   SUB SP,2

   TEST Mode.attrib,TVU_PLANAR
   JNZ PLANAR_CLEAR
   CMP Mode.videomode,VMODE_13H
   JNE UNCHAINED_CLEAR

   PUSH 0
   CALL CLEAR13H
   JMP CLEAROUT

PLANAR_CLEAR:
   PUSH 0
   CALL PLANARCLEAR
   JMP CLEAROUT

UNCHAINED_CLEAR:
   PUSH 0
   CALL UNCHAINEDCLEAR

CLEAROUT:

   PUSH 15

   MOV AX,Mode.mode_height
   DEC AX
   PUSH AX

   MOV AX,Mode.mode_width
   DEC AX
   PUSH AX

   PUSH 0
   PUSH 0
   CALL DRAWRECT

   MOV CX,50
LINE_LOOP:
   PUSH 15
   PUSH CX
   PUSH 10
   PUSH 0
   CALL HLINE
   ADD CX,50

   PUSH 15
   PUSH CX
   PUSH 25
   PUSH 0
   CALL HLINE
   ADD CX,50

   CMP CX,Mode.mode_height
   JLE LINE_LOOP

   MOV CX,40
LINE_LOOP2:
   PUSH 15
   PUSH CX
   PUSH 15
   PUSH 0
   CALL VLINE
   ADD CX,40

   PUSH 15
   PUSH CX
   PUSH 30
   PUSH 0
   CALL VLINE
   ADD CX,40

   CMP CX,Mode.mode_width
   JLE LINE_LOOP2

   CMP Mode.colors,16
   JE SETBLOCK16
   CMP Mode.colors,256
   JE SETBLOCK256

   MOV AX,Mode.mode_width
   SUB AX,420
   MOV DI,AX
   ADD DI,200
   MOV BX,Mode.mode_height
   SUB BX,220
   MOV SI,BX
   ADD SI,200

   PUSH 0    ; Color
   PUSH SI    ; Y2
   PUSH DI    ; X2
   PUSH BX    ; Y1
   PUSH AX    ; X1
   CALL FILLRECT
   ADD AX,200
   ADD DI,200
   PUSH 15    ; Color
   PUSH SI    ; Y2
   PUSH DI    ; X2
   PUSH BX    ; Y1
   PUSH AX    ; X1
   CALL FILLRECT

   MOV AX,Mode.mode_width
   SUB AX,420
   MOV DI,AX
   ADD DI,400
   MOV BX,Mode.mode_height
   SUB BX,220
   MOV SI,BX
   ADD SI,200

   PUSH 15    ; Color
   PUSH SI    ; Y2
   PUSH DI    ; X2
   PUSH BX    ; Y1
   PUSH AX    ; X1
   CALL DRAWRECT

   JMP BLOCKOUT
SETBLOCK16:

   MOV AX,Mode.mode_width
   SUB AX,522
   MOV DI,AX
   ADD DI,64

   MOV BX,Mode.mode_height
   SUB BX,135
   MOV SI,BX
   ADD SI,64
   MOV CX,0
   MOV DX,0
LINE1_LOOP1:

   PUSH DX    ; Color
   PUSH SI    ; Y2
   PUSH DI    ; X2
   PUSH BX    ; Y1
   PUSH AX    ; X1
   CALL FILLRECT
   ADD AX,64
   ADD DI,64
   INC CX
   INC DX
   CMP CX,8
   JL LINE1_LOOP1

   MOV AX,Mode.mode_width
   SUB AX,522
   MOV DI,AX
   ADD DI,64
   ADD SI,64
   ADD BX,64
   MOV CX,0
LINE1_LOOP2:

   PUSH DX    ; Color
   PUSH SI    ; Y2
   PUSH DI    ; X2
   PUSH BX    ; Y1
   PUSH AX    ; X1
   CALL FILLRECT
   ADD AX,64
   ADD DI,64
   INC CX
   INC DX
   CMP CX,8
   JL LINE1_LOOP2

   MOV AX,Mode.mode_width
   SUB AX,522
   MOV DI,Mode.mode_width
   SUB DI,10
   MOV BX,Mode.mode_height
   SUB BX,135
   MOV SI,Mode.mode_height
   SUB SI,7
   PUSH 15    ; Color
   PUSH SI    ; Y2
   PUSH DI    ; X2
   PUSH BX    ; Y1
   PUSH AX    ; X1
   CALL DRAWRECT
   JMP BLOCKOUT

SETBLOCK256:

   MOV WORD PTR [BP-2],0
   MOV BX,Mode.mode_height
   SUB BX,165
   MOV SI,BX
   ADD SI,10
   MOV DX,0
LINE2_LOOP2:

   MOV AX,Mode.mode_width
   SUB AX,165
   MOV DI,Mode.mode_width
   SUB DI,155

   MOV CX,0
LINE2_LOOP1:

   PUSH DX    ; Color
   PUSH SI    ; Y2
   PUSH DI    ; X2
   PUSH BX    ; Y1
   PUSH AX    ; X1
   CALL FILLRECT
   ADD AX,10
   ADD DI,10
   INC CX
   INC DX
   CMP CX,16
   JL LINE2_LOOP1

   ADD BX,10
   ADD SI,10
   INC WORD PTR [BP-2]
   CMP WORD PTR [BP-2],16
   JL LINE2_LOOP2

   MOV AX,Mode.mode_width
   SUB AX,165
   MOV DI,Mode.mode_width
   SUB DI,5
   MOV BX,Mode.mode_height
   SUB BX,165
   MOV SI,Mode.mode_height
   SUB SI,5
   PUSH 15    ; Color
   PUSH SI    ; Y2
   PUSH DI    ; X2
   PUSH BX    ; Y1
   PUSH AX    ; X1
   CALL DRAWRECT

BLOCKOUT:
   ADD SP,2
   POP BP
   RET
MODETEST ENDP

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

PUTCHAR PROC
   PUSH BP
   MOV BP,SP
   SUB SP,2
   PUSHA

   CMP Mode.videomode,VMODE_00H
   JE SET2
   CMP Mode.videomode,VMODE_MODE_L
   JE SET2
   CMP Mode.videomode,VMODE_MODE_M
   JE SET2
   MOV BYTE PTR [BP-2],160
   JMP NOSET2
SET2:
   MOV BYTE PTR [BP-2],80
   JMP NOSET2
NOSET2:

   MOV AX,0B800h
   MOV ES,AX

   MOV AX,WORD PTR [BP+6] ; Y value
   MOV BL,BYTE PTR [BP-2]
   MUL BL
   SHL WORD PTR [BP+4],1
   ADD AX,WORD PTR [BP+4] ; X value
   MOV DI,AX
   MOV AH,ATT
   MOV AL,BYTE PTR [BP+8] ; character
   CLD
   STOSW

   POPA
   ADD SP,2
   POP BP
   RET 6
PUTCHAR ENDP

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

PRINTSTRING PROC

TEXT_LOOP:
   XOR DX,DX
   MOV DL,BYTE PTR [SI]
   PUSH DX
   PUSH BX
   PUSH AX
   CALL PUTCHAR

   INC AX
   INC SI
   CMP BYTE PTR [SI],0
   JNE TEXT_LOOP
   RET
PRINTSTRING ENDP

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

TEXTTEST PROC
   PUSH 1FH
   CALL TEXTCLEAR

   MOV AH,2h
   MOV BH,0
   MOV DX,Mode.mode_height
   MOV DH,DL
   MOV DL,0
   INT 10h

   CMP Mode.videomode,VMODE_00H
   JE SET_TEXT1
   CMP Mode.videomode,VMODE_MODE_J
   JE SET_TEXT2
   CMP Mode.videomode,VMODE_MODE_K
   JE SET_TEXT3
   CMP Mode.videomode,VMODE_MODE_L
   JE SET_TEXT4
   CMP Mode.videomode,VMODE_MODE_M
   JE SET_TEXT5
   ; Mode 03h
   MOV TEXTMSG_2+13,'3'
   MOV TEXTMSG_3+8,'8'
   MOV TEXTMSG_4+19,'4'
   JMP NOSET
SET_TEXT1:
   ; Mode 00h
   MOV TEXTMSG_2+13,'0'
   MOV TEXTMSG_3+8,'4'
   MOV TEXTMSG_4+19,'2'
   JMP NOSET
SET_TEXT2:
   ; Mode J
   MOV TEXTMSG_8+11,'J'
   MOV TEXTMSG_9+21,'4'
   MOV TEXTMSG_9+22,'3'
   MOV TEXTMSG_A+19,'3'
   MOV TEXTMSG_A+20,'4'
   MOV TEXTMSG_A+21,'4'
   MOV TEXTMSG_A+22,'0'
   JMP NOSET
SET_TEXT3:
   ; Mode K
   MOV TEXTMSG_8+11,'K'
   MOV TEXTMSG_9+21,'5'
   MOV TEXTMSG_9+22,'0'
   MOV TEXTMSG_A+19,'4'
   MOV TEXTMSG_A+20,'0'
   MOV TEXTMSG_A+21,'0'
   MOV TEXTMSG_A+22,'0'
   JMP NOSET
SET_TEXT4:
   ; Mode L
   MOV TEXTMSG_8+11,'L'
   MOV TEXTMSG_9+8,'4'
   MOV TEXTMSG_9+21,'4'
   MOV TEXTMSG_9+22,'3'
   MOV TEXTMSG_A+19,'3'
   MOV TEXTMSG_A+20,'4'
   MOV TEXTMSG_A+21,'4'
   MOV TEXTMSG_A+22,'0'
   JMP NOSET
SET_TEXT5:
   ; Mode M
   MOV TEXTMSG_8+11,'M'
   MOV TEXTMSG_9+8,'4'
   MOV TEXTMSG_9+21,'5'
   MOV TEXTMSG_9+22,'0'
   MOV TEXTMSG_A+19,'4'
   MOV TEXTMSG_A+20,'0'
   MOV TEXTMSG_A+21,'0'
   MOV TEXTMSG_A+22,'0'
NOSET:

   CMP Mode.videomode,VMODE_00H
   JE WRITE1
   CMP Mode.videomode,VMODE_03H
   JE WRITE1
   CMP Mode.videomode,VMODE_MODE_J
   JE WRITE2
   JMP WRITE2

WRITE1:
   MOV BX,0
   MOV AX,0
   MOV SI,OFFSET TEXTMSG_1
   CALL PRINTSTRING
   MOV BX,1
   MOV AX,0
   MOV SI,OFFSET TEXTMSG_2
   CALL PRINTSTRING
   MOV BX,2
   MOV AX,0
   MOV SI,OFFSET TEXTMSG_3
   CALL PRINTSTRING
   MOV BX,3
   MOV AX,0
   MOV SI,OFFSET TEXTMSG_4
   CALL PRINTSTRING
   MOV BX,4
   MOV AX,0
   MOV SI,OFFSET TEXTMSG_5
   CALL PRINTSTRING
   MOV BX,5
   MOV AX,0
   MOV SI,OFFSET TEXTMSG_6
   CALL PRINTSTRING
   JMP WRITEOUT

WRITE2:
   MOV BX,0
   MOV AX,0
   MOV SI,OFFSET TEXTMSG_7
   CALL PRINTSTRING
   MOV BX,1
   MOV AX,0
   MOV SI,OFFSET TEXTMSG_8
   CALL PRINTSTRING
   MOV BX,2
   MOV AX,0
   MOV SI,OFFSET TEXTMSG_9
   CALL PRINTSTRING
   MOV BX,3
   MOV AX,0
   MOV SI,OFFSET TEXTMSG_A
   CALL PRINTSTRING
   MOV BX,4
   MOV AX,0
   MOV SI,OFFSET TEXTMSG_B
   CALL PRINTSTRING
   MOV BX,5
   MOV AX,0
   MOV SI,OFFSET TEXTMSG_C
   CALL PRINTSTRING
WRITEOUT:
   RET
TEXTTEST ENDP

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
