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
// 3C4H, 03H
// Bit positions for font maps:
//
// +--7--+--6--+--5--+--4--+--3--+--2--+--1--+--0--+
// |     |     | SAH | SBH |    SA     |    SB     |
// +-----+-----+-----+-----+-----+-----+-----+-----+
//
// SA  - bits 1, 0 of Character Map A
// SAH - bit 2 (high order) of Charcter Map A
//
// SB  - bits 1, 0 of Character Map B
// SBH - bit 2 (high order) of Charcter Map B
//
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include "tauron.h"
#include "font1.h"
#include "font2.h"
extern unsigned char mode03h[62];
void LoadFonts();

// Points to fonts for Map A              Memory Address
#define DFM_MAPA_FONT1     0x00          //  0K
#define DFM_MAPA_FONT2     0x04          // 16K
#define DFM_MAPA_FONT3     0x08          // 32K
#define DFM_MAPA_FONT4     0x0C          // 48K
#define DFM_MAPA_FONT5     0x20          //  8K
#define DFM_MAPA_FONT6     0x24          // 24K
#define DFM_MAPA_FONT7     0x28          // 40K
#define DFM_MAPA_FONT8     0x2C          // 56K

// Points to fonts for Map B
#define DFM_MAPB_FONT1     0x00          //  0K
#define DFM_MAPB_FONT2     0x01          // 16K
#define DFM_MAPB_FONT3     0x02          // 32K
#define DFM_MAPB_FONT4     0x03          // 48K
#define DFM_MAPB_FONT5     0x10          //  8K
#define DFM_MAPB_FONT6     0x11          // 24K
#define DFM_MAPB_FONT7     0x12          // 40K
#define DFM_MAPB_FONT8     0x13          // 56K


// To set dual font mode, just set the character map select register to point
// to 2 different fonts.  There can be up to 8 fonts resident in memory at
// once but only 2 can be active at a time.
//
// ** NOTE ** If you set both font maps the same you are no longer in dual font
// mode.  But what this allows you to do is to put up to 8 fonts in VGA memory
// and switch between them at any time.
//
// Both fonts were made with my font editor.  I just read the BIOS and
// modified the second font so a solid line goes through the middle of it.
//
// This procedure sets up dual font mode and loads 2 fonts into video memory.
void SetDual()
{
   outport(SEQ_ADDR, ((DFM_MAPA_FONT1 | DFM_MAPB_FONT5) << 8) | 0x03);

   // Load font 1 into 0K and font 2 into 8K
	LoadFonts();
}

// This procedure reset the character map select register to point to only
// one font.
void UnSetDual()
{
   outport(0x3C4,0x0003);
}

// In dual font modes, the palette is in effect half of what it is.  The 
// bottom 8 colors for the first font, and the top 8 colors for the second 
// font.
//
// What I do here is to set them to the same 8 colors.
// Black, Blue, Green, Red, Purple, Yellow, Light Gray, and White
//
void SetDualPalette()
{
	// Font 1
   setpal( 0,  0,  0,  0);
   setpal( 1,  0,  0, 42);
   setpal( 2,  0, 42,  0);
   setpal( 3,  0, 42, 42);
   setpal( 4, 42,  0,  0);
   setpal( 5, 63, 63, 21);
   setpal( 6, 42, 42, 42);
   setpal( 7, 63, 63, 63);
   // Font 2
   setpal( 8,  0,  0,  0);
   setpal( 9,  0,  0, 42);
   setpal(10,  0, 42,  0);
   setpal(11,  0, 42, 42);
   setpal(12, 42,  0,  0);
   setpal(13, 63, 63, 21);
   setpal(14, 42, 42, 42);
   setpal(15, 63, 63, 63);
}

void pchar(int x, int y, char c, char att)
{
   char far *vidmem;
   int off;

   // Make a pointer to the font
   vidmem = (char far *)MK_FP( 0xB800, 0x0000);
   off = y * 160 + x * 2;
   vidmem += off;
   *vidmem++ = c;
   *vidmem = att;
}

void print(int x, int y, char *Text, char attrib)
{
	for (int i = 0; i < strlen(Text); i++)
   	pchar(x++,y,Text[i],attrib);
}

void DualTest()
{
	SetMode((unsigned int)&mode03h);
   TextClear(0x1F);
   SetDual();
   SetDualPalette();

   /* ** NOTE ** when trying to print text on the screen, do not use the printf
    * text functions.  For some reason they do not select the proper VGA font.
    */

	print(0,0,"ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿",0x17);
   print(0,1,"³ DUAL FONT MODE ³",0x17);
   print(0,2,"ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ",0x17);

	print(0,4,"ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿",0x1F);
   print(0,5,"³ DUAL FONT MODE ³",0x1F);
   print(0,6,"ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ",0x1F);

   getch();
   UnSetDual();
   textcolor(7);
   textbackground(0);
}


void LoadFonts()
{
   char far *vidmem;
   int l = 0;
   unsigned char oldmode,oldmisc,oldmem,oldmask;
   unsigned char newmode,newmisc,newmem;

   // Make a pointer to the font
   vidmem = (char far *)MK_FP( 0xA000, 0x0000);

   // Store the OLD 'Mode Register' value
   outportb(GRACON_ADDR,5);
   oldmode = inportb(GRACON_ADDR+1);
   // Store the OLD 'Miscellaneous Register' value
   outportb(GRACON_ADDR,6);
   oldmisc = inportb(GRACON_ADDR+1);
   // Store the OLD 'Mask Map' value
   outportb(SEQ_ADDR,2);
   oldmask = inportb(SEQ_ADDR+1);
   // Store the OLD 'Memory Mode' value
   outportb(SEQ_ADDR,4);
   oldmem = inportb(SEQ_ADDR+1);

   // Write the NEW 'Mode Register' value
   newmode = (oldmode & 0xFC);
   outport(GRACON_ADDR, (newmode << 8) | 0x05);
   // Write the NEW 'Miscellaneous Register' value
   newmisc = ((oldmisc & 0xF1)|4);
   outport(GRACON_ADDR, (newmisc << 8) | 0x06);
   // Write the NEW 'Mask Map' value
   outport(SEQ_ADDR, 0x0402);
   // Write the NEW 'Memory Mode' value
   newmem = (oldmem | 4);
   outport(SEQ_ADDR, (newmem << 8) | 0x04);

   // Copy the font from BIOS
   l = 0;
   for (int i = 0; i < 256; i++)
   {
      for (int j = 0; j < 16; j++)
      {
         *vidmem++ = Font1[l++];
      }
      for (int k = 0; k < 16; k++)
      {
         *vidmem++ = 0x00;
      }
   }

   vidmem = (char far *)MK_FP( 0xA000, 0x2000);
   l = 0;
   for (int i = 0; i < 256; i++)
   {
      for (int j = 0; j < 16; j++)
      {
         *vidmem++ = Font2[l++];
      }
      for (int k = 0; k < 16; k++)
      {
         *vidmem++ = 0x00;
      }
   }

   // Write the OLD 'Mode Register' value
   outport(GRACON_ADDR, (oldmode << 8) | 0x05);
   // Write the OLD 'Miscellaneous Register' value
   outport(GRACON_ADDR, (oldmisc << 8) | 0x06);
   // Write the OLD 'Mask Map' value
   outport(SEQ_ADDR,(oldmask << 8) | 0x02);
   // Write the OLD 'Memory Mode' value
   outport(SEQ_ADDR, (oldmem << 8) | 0x04);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=