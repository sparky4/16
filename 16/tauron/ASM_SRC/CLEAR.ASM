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

TEXTCLEAR PROC
   PUSH BP
   MOV BP,SP

   MOV AX,0B800H
   MOV ES,AX
   XOR DI,DI

   MOV AH,byte ptr [bp+4]
   MOV AL,' '
   MOV CX,Mode.width_bytes
   CLD
   REP STOSW

   POP BP
   RET 2
TEXTCLEAR ENDP

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CLEAR_B8000H PROC
   PUSH BP
   MOV BP,SP
   PUSHA

   MOV AX,0B800H
   MOV ES,AX
   XOR DI,DI

   MOV AX,WORD PTR [BP+4]
   CLD
   MOV CX,Mode.width_bytes
   REP STOSW

   POPA
   POP BP
   RET 2
CLEAR_B8000H ENDP

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
; Mode 0Dh clear screen
CLEAR0DH PROC
   PUSH BP
   MOV BP,SP

   MOV AX,0A000H
   MOV ES,AX
   XOR DI,DI

   MOV CX,2000H
CLEAR_LOOP:
   MOV DX,03CEH
   MOV AX,0FF08H
   OUT DX,AX

   MOV EAX,0
   XCHG DWORD PTR ES:[DI],EAX

   MOV DX,03C4H
   MOV AH,BYTE PTR [BP+4]
   MOV AL,2
   OUT DX,AX

   MOV DWORD PTR ES:[DI],0FFFFFFFFH

   MOV AX,0F02H
   OUT DX,AX

   MOV DX,03CEH
   MOV AX,0FF08h
   OUT DX,AX

   ADD DI,4
   LOOP CLEAR_LOOP

   POP BP
   RET 2
CLEAR0DH ENDP

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
; Mode 0Eh clear screen
CLEAR0EH PROC
   PUSH BP
   MOV BP,SP

   MOV AX,0A000H
   MOV ES,AX
   XOR DI,DI

   MOV CX,4000H
CLEAR_LOOP2:
   MOV DX,03CEH
   MOV AX,0FF08H
   OUT DX,AX

   MOV EAX,0
   XCHG DWORD PTR ES:[DI],EAX

   MOV DX,03C4H
   MOV AH,BYTE PTR [BP+4]
   MOV AL,2
   OUT DX,AX

   MOV DWORD PTR ES:[DI],0FFFFFFFFH

   MOV AX,0F02H
   OUT DX,AX

   MOV DX,03CEH
   MOV AX,0FF08h
   OUT DX,AX

   ADD DI,4
   LOOP CLEAR_LOOP2

   POP BP
   RET 2
CLEAR0EH ENDP

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CLEAR11H PROC
   MOV AX,0A000H
   MOV ES,AX
   XOR DI,DI

   MOV AX,0FFFFH
   CLD
   MOV CX,4B00H
   REP STOSW
   RET
CLEAR11H ENDP

; 0d 0e 11 have bytes /2
;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

PLANARCLEAR PROC
   PUSH BP
   MOV BP,SP

   MOV AX,0A000H
   MOV ES,AX
   XOR DI,DI

   MOV CX,Mode.width_bytes
   SHR CX,1
CLEAR_LOOP3:

   MOV DX,03CEH
   MOV AX,0FF08H
   OUT DX,AX

   MOV AX,0
   XCHG WORD PTR ES:[DI],AX

   MOV DX,03C4H
   MOV AH,byte ptr [bp+4]
   MOV AL,2
   OUT DX,AX

   MOV WORD PTR ES:[DI],0FFFFH

   MOV AX,0F02H
   OUT DX,AX

   MOV DX,03CEH
   MOV AX,0FF08h
   OUT DX,AX

   ADD DI,2
   LOOP CLEAR_LOOP3

   POP BP
   RET 2
PLANARCLEAR ENDP

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CLEAR13H PROC
   PUSH BP
   MOV BP,SP

   MOV AX,0A000H
   MOV ES,AX
   XOR DI,DI

   MOV AL,byte ptr [bp+4]
   MOV AH,AL
   CLD
   MOV CX,8000H
   REP STOSW

   POP BP
   RET 2
CLEAR13H ENDP

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

UNCHAINEDCLEAR PROC
   PUSH BP
   MOV BP,SP

   mov   dx, 03C4h         ; 03c4h
   mov   al, 2             ; Map Mask Register
   out   dx, al
   inc   dx
   mov   al,00001111B      ; Select all planes to write to
   out   dx, al            ; Doing this to clear all planes at once

   mov   ax,0A000H
   mov   es, ax
   XOR DI,DI               ; set es:di = Screen Mem
   mov ah,byte ptr [bp+4]  ; move the color into ah
   mov al,ah               ; copy the value to al
   mov cx,Mode.width_bytes ; Number of words
   SHR CX,1
   cld
   rep   stosw             ; clear it

   POP BP
   RET 2
UNCHAINEDCLEAR ENDP

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
