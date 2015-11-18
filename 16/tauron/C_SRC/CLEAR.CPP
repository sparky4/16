//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//=-                                                                         -=
//=-                   Tauron VGA Utilities Version 3.0                      -=
//=-                      Released September 20, 1998                        -=
//=-                                                                         -=
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//=- Copyright (c) 1997, 1998 by Jeff Morgan  =-= This code is FREE provided -=
//=- All Rights Reserved.                     =-= that you put my name some- -=
//=-                                          =-= where in your credits.     -=
//=- DISCLAIMER:                              =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//=- I assume no responsibility whatsoever for any effect that this package, -=
//=- the information contained therein or the use thereof has on you, your   -=
//=- sanity, computer, spouse, children, pets or anything else related to    -=
//=- you or your existance. No warranty is provided nor implied with this    -=
//=- source code.                                                            -=
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include "tauron.h"

void TextClear(char attrib)
{
   unsigned int bytes = Mode.width_bytes;
   asm {
   MOV AX,0B800H
   MOV ES,AX
   XOR DI,DI

   MOV AH,attrib
   MOV AL,' '
   MOV CX,bytes
   CLD
   REP STOSW
   }
}

void Clear04H()
{
   unsigned int bytes = Mode.width_bytes;
   asm {
   MOV AX,0B800H
   MOV ES,AX
   XOR DI,DI

   MOV AX,0AAAAH
   CLD
   MOV CX,bytes
   REP STOSW
   }
}

void Clear06H()
{
   unsigned int bytes = Mode.width_bytes;
   asm {
   MOV AX,0B800H
   MOV ES,AX
   XOR DI,DI

   MOV AX,0FFFFH
   CLD
   MOV CX,bytes
   REP STOSW
   }
}

void Clear0DH(char color)
{
   unsigned int bytes = Mode.width_bytes/2;
   asm {
   MOV AX,0A000H
   MOV ES,AX
   XOR DI,DI

   MOV CX,bytes
   }
CLEAR_LOOP:
   asm {
   MOV DX,03CEH
   MOV AX,0FF08H
   OUT DX,AX

   MOV AX,0
   XCHG WORD PTR ES:[DI],AX

   MOV DX,03C4H
   MOV AH,color
   MOV AL,2
   OUT DX,AX

   MOV WORD PTR ES:[DI],0FFFFH

   MOV AX,0F02H
   OUT DX,AX

   MOV DX,03CEH
   MOV AX,0FF08h
   OUT DX,AX

   ADD DI,2
   LOOP CLEAR_LOOP
   }
}

void PlanarClear(char Color)
{
   unsigned int bytes = Mode.width_bytes/2;
   asm {
   MOV AX,0A000H
   MOV ES,AX
   XOR DI,DI

   MOV CX,bytes
   }
CLEAR_LOOP:
   asm {
   MOV DX,03CEH
   MOV AX,0FF08H
   OUT DX,AX

   MOV AX,0
   XCHG WORD PTR ES:[DI],AX

   MOV DX,03C4H
   MOV AH,Color
   MOV AL,2
   OUT DX,AX

   MOV WORD PTR ES:[DI],0FFFFH

   MOV AX,0F02H
   OUT DX,AX

   MOV DX,03CEH
   MOV AX,0FF08h
   OUT DX,AX

   ADD DI,2
   LOOP CLEAR_LOOP
   }
}

void Clear13H(char Color)
{
   asm {
   MOV AX,0A000H
   MOV ES,AX
   XOR DI,DI

   MOV AL,Color
   MOV AH,AL
   CLD
   MOV CX,8000H
   REP STOSW
   }
}

void UnchainedClear(char Color)
{
   unsigned int bytes = Mode.width_bytes/2;
   asm {
   mov   dx, 03C4h         // 03c4h
   mov   al, 2             // Map Mask Register
   out   dx, al
   inc   dx
   mov   al,00001111B      // Select all planes to write to
   out   dx, al            // Doing this to clear all planes at once

   mov   ax,0A000H
   mov   es, ax
   XOR DI,DI               // set es:di = Screen Mem
   mov ah,Color            // move the color into ah
   mov al,ah               // copy the value to al
   mov   cx,bytes          // Number of words
   cld
   rep   stosw             // clear it
   }
}
