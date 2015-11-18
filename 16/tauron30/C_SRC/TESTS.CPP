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
#include "tauron.h"
#define ABS(a)   ((a < 0) ? -a : a)
#define SGN(a)   ((a < 0) ? -1 : 1)

void Pixel13H(int x, int y, char color)
{
   int width = Mode.width;
   asm {
   MOV AX,0A000H   //    video memory segment number
   MOV ES,AX       //    place it in es

   MOV DX,03C4H
   MOV AL,2
   OUT DX,AL
   INC DX

   MOV AL,1
   MOV CX,x
   AND CX,3
   SHL AL,CL
   OUT DX,AL

   XOR DI,DI

// Calculate the Offset
   mov ax,width  // width
   mul y         // (Y * width))
   mov bx,x      // (X + (Y * width))
   add ax,bx
// Done!

   ADD DI,AX
   mov ah,color   //    move the Color into ah
   mov es:[di],ah //    move the value to the screen
   }
}

void pixel(int x, int y, char color)
{
   int width = Mode.width;
   if (Mode.mode == MODE13H)
      Pixel13H(x,y,color);
   else if (Mode.attrib & TVU_UNCHAINED) {
   asm {
   MOV AX,0A000H   //    video memory segment number
   MOV ES,AX       //    place it in es

   MOV DX,03C4H
   MOV AL,2
   OUT DX,AL
   INC DX

   MOV AL,1
   MOV CX,x
   AND CX,3
   SHL AL,CL
   OUT DX,AL

   XOR DI,DI

// Calculate the Offset
   mov ax,width  // width / 4
   SHR AX,2
   mul y         // (Y * (width / 4))
   mov bx,x      // (X / 4) + (Y * (width / 4))
   shr bx,2
   add ax,bx
// Done!

   ADD DI,AX
   mov ah,color   //    move the Color into ah
   mov es:[di],ah //    move the value to the screen
   }}
   else if (Mode.attrib & TVU_PLANAR)
   {
   asm {
   MOV AX,0A000H   //    video memory segment number
   MOV ES,AX       //    place it in es

   MOV BX,x        //    X Value
   MOV CX,BX
   MOV AX,y        //    Y Value
   MOV SI,80
   MUL SI
   SHR BX,3        //    /8
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
   MOV AH,color
   MOV AL,2
   OUT DX,AX

   MOV BYTE PTR ES:[DI],0FFh

   MOV AX,0F02H
   OUT DX,AX

   MOV DX,03CEH
   MOV AX,0FF08h
   OUT DX,AX
   }}
}

// This is Bresenham's Line Drawing Algorithm
void drawline(int x1, int y1, int x2, int y2, char col)
{
   int d, x, y, ax, ay, sx, sy, dx, dy;

   dx = x2-x1;
   ax = ABS(dx) << 1;
   sx = SGN(dx);
   dy = y2-y1;
   ay = ABS(dy) << 1;
   sy = SGN(dy);

   x = x1;
   y = y1;
   if( ax > ay )
   {
      d = ay - (ax >> 1);
      while( x != x2 )
      {
	      pixel( x, y, col );
  	      if( d >= 0 )
	      {
	         y += sy;
	         d -= ax;
	      }
	      x += sx;
	      d += ay;
      }
   }
   else
   {
      d = ax - (ay >> 1);
      while( y != y2 )
      {
	      pixel( x, y, col );
	      if( d >= 0 )
	      {
	         x += sx;
	         d -= ay;
	      }
	      y += sy;
	      d += ax;
      }
   }
   return;
}

void drawrect(int x1, int y1, int x2, int y2, char color)
{
   drawline(x1,y1,x2,y1,color);
   drawline(x1,y2,x2,y2,color);
   drawline(x1,y1,x1,y2,color);
   drawline(x2,y1,x2,y2+1,color);
}

void hline(int x1, int x2, int y, char color)
{
   drawline(x1,y,x2,y,color);
}

void vline(int y1, int y2, int x, char color)
{
   drawline(x,y1,x,y2,color);
}

void fillrect(int x1, int y1, int x2, int y2, char color)
{
   for (int i = y1; i < y2; i++)
      hline(x1,x2,i,color);
}

void ModeTest()
{
   if (Mode.mode == MODE13H)
   {
      Clear13H(0);
   }
   else if (Mode.attrib & TVU_PLANAR)
   {
      PlanarClear(0);
   }
   else if (Mode.attrib & TVU_UNCHAINED)
   {
      UnchainedClear(0);
   }

   /* Print mode stats
    *
    */

   drawrect(0,0,Mode.width-1,Mode.height-1,15);
   for (int i = 50; i < Mode.height; i += 100)
   {
      hline(0,10,i,15);
      hline(0,25,i+50,15);
   }

   for (int i = 40; i < Mode.width; i += 80)
   {
      vline(0,15,i,15);
      vline(0,30,i+40,15);
   }

   if (Mode.colors == 2)
   {
      int x,y;
      x = Mode.width - 420;
      y = Mode.height - 220;
      fillrect(x,y,x+200,y+200,0);
      x+=200;
      fillrect(x,y,x+200,y+200,15);
      drawrect(x-200,y,x+200,y+200,15);
   }
   else if (Mode.colors == 16)
   {
      int x,y;
      x = Mode.width - 522;
      y = Mode.height - 135;
      for (int i = 0; i < 8; i++)
      {
         fillrect(x,y,x+64,y+64,i);
         x+=64;
      }
      x = Mode.width - 522;
      y += 64;
      for (int i = 8; i < 16; i++)
      {
         fillrect(x,y,x+64,y+64,i);
         x+=64;
      }
      drawrect(x-512,y-64,x,y+64,15);
   }
   else if (Mode.colors == 256)
   {
      int x,y,k;
      x = Mode.width - 165;
      y = Mode.height - 165;
      k = 0;
      for (int i = 0; i < 16; i++)
      {
         for (int j = 0; j < 16; j++)
         {
            fillrect(x,y,x+10,y+10,k);
            x+=10;
            k++;
         }
         x = Mode.width - 165;
         y += 10;
      }
      drawrect(Mode.width - 165,Mode.height - 165,Mode.width -5,Mode.height - 5,15);
   }
}

void TextTest()
{
   TextClear(0x1F);
   gotoxy(1,1);
   if (Mode.mode == MODE00H)
   {
      // ** NOTE ** the gotoxy's here are a hack so that i could use the
      // standard C functions to program this mode.  Normally you would just
      // calculate the proper address and display the text but since DOS still
      // thinks we are in mode 03h (it checks the BIOS), this is necessary.
      gotoxy(1,1);
      printf("ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n");
      gotoxy(41,1);
      printf("³Text Mode: 00H        ³\n");
      gotoxy(1,2);
      printf("³Width: 40   Height: 25³\n");
      gotoxy(41,2);
      printf("³Bytes per screen: 2000³\n");
      gotoxy(1,3);
      printf("³Number of pages: 8    ³\n");
      gotoxy(41,3);
      printf("ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n");
   }
   else if (Mode.mode == MODE03H)
   {
      printf("ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n");
      printf("³Text Mode: 03H        ³\n");
      printf("³Width: 80   Height: 25³\n");
      printf("³Bytes per screen: 4000³\n");
      printf("³Number of pages: 8    ³\n");
      printf("ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n");
   }
   else if (Mode.mode == MODE_J)
   {
      printf("ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n");
      printf("³Text Mode K           ³\n");
      printf("³Width: 80   Height: 43³\n");
      printf("³Bytes per screen: 3440³\n");
      printf("³Number of pages: 8    ³\n");
      printf("ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n");
   }
   else if (Mode.mode == MODE_K)
   {
      printf("ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n");
      printf("³Text Mode K           ³\n");
      printf("³Width: 80   Height: 50³\n");
      printf("³Bytes per screen: 4000³\n");
      printf("³Number of pages: 8    ³\n");
      printf("ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n");
   }
   else if (Mode.mode == MODE_L)
   {
      // ** NOTE ** the gotoxy's here are a hack so that i could use the
      // standard C functions to program this mode.  Normally you would just
      // calculate the proper address and display the text but since DOS still
      // thinks we are in mode 03h (it checks the BIOS), this is necessary.
      printf("ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n");
      gotoxy(41,1);
      printf("³Text Mode L           ³\n");
      gotoxy(1,2);
      printf("³Width: 40   Height: 43³\n");
      gotoxy(41,2);
      printf("³Bytes per screen: 3440³\n");
      gotoxy(1,3);
      printf("³Number of pages: 8    ³\n");
      gotoxy(41,3);
      printf("ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n");
   }
   else if (Mode.mode == MODE_M)
   {
      // ** NOTE ** the gotoxy's here are a hack so that i could use the
      // standard C functions to program this mode.  Normally you would just
      // calculate the proper address and display the text but since DOS still
      // thinks we are in mode 03h (it checks the BIOS), this is necessary.
      printf("ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n");
      gotoxy(41,1);
      printf("³Text Mode K           ³\n");
      printf("³Width: 40   Height: 50³\n");
      gotoxy(1,2);
      gotoxy(41,2);
      printf("³Bytes per screen: 4000³\n");
      gotoxy(1,3);
      printf("³Number of pages: 8    ³\n");
      gotoxy(41,3);
      printf("ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n");
   }
}
