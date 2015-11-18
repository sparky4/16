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
#include "modes_c.inc"
#include "palette.inc"

void setpalette4();
void setpalette16();
void setpalette256();
Vmode Mode;

#define SEQ_ADDR 				0x03C4
#define GRACON_ADDR 			0x03CE
#define CRTC_ADDR 			0x03D4

void ReadBIOSfont(int fontnum, int bytesperchar)
{
   char far *biosfont,*vidmem;
   struct REGPACK reg;
   unsigned char oldmode,oldmisc,oldmem,oldmask;
   unsigned char newmode,newmisc,newmem;

   // get the location of the font stroed in BIOS
   reg.r_ax = 0x1130;
   reg.r_bx = fontnum << 8;
   intr(0x10, &reg);

   // Make a pointer to the font
   biosfont = (char far *)MK_FP( reg.r_es, reg.r_bp);
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
   for (int i = 0; i < 256; i++)
   {
      for (int j = 0; j < bytesperchar; j++)
      {
         *vidmem++ = *biosfont++;
      }
      for (int k = 0; k < 32-bytesperchar; k++)
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

void SetMode(unsigned int regs)
{
   asm {
   MOV SI, regs

   // Send MISC regs
   MOV DX,MISC_ADDR
   MOV AL,[SI]
   OUT DX,AL
   INC SI

   MOV DX,STATUS_ADDR
   MOV AL,[SI]
   OUT DX,AL
   INC SI

   // Send SEQ regs
   MOV CX,0
REG_LOOP:
   MOV DX,SEQ_ADDR
   MOV AL,CL
   OUT DX,AL

   MOV DX,SEQ_ADDR
   INC DX
   MOV AL,[SI]
   OUT DX,AL

   INC SI
   INC CX
   CMP CL,5
   JL REG_LOOP

   // Clear Protection bits
   MOV AH,0EH
   MOV AL,11H
   AND AH,7FH
   MOV DX,CRTC_ADDR
   OUT DX,AX

   // Send CRTC regs
   MOV CX,0
REG_LOOP2:
   MOV DX,CRTC_ADDR
   MOV AL,CL
   OUT DX,AL

   MOV DX,CRTC_ADDR
   INC DX
   MOV AL,[SI]
   OUT DX,AL

   INC SI
   INC CX
   CMP CL,25
   JL REG_LOOP2

   // Send GRAPHICS regs
   MOV CX,0
REG_LOOP3:
   MOV DX,GRACON_ADDR
   MOV AL,CL
   OUT DX,AL

   MOV DX,GRACON_ADDR
   INC DX
   MOV AL,[SI]
   OUT DX,AL

   INC SI
   INC CX
   CMP CL,9
   JL REG_LOOP3

   MOV DX,STATUS_ADDR
   IN AL,DX

   // Send ATTRCON regs
   MOV CX,0
REG_LOOP4:
   MOV DX,ATTRCON_ADDR
   IN AX,DX

   MOV AL,CL
   OUT DX,AL

   MOV AL,[SI]
   OUT DX,AL

   INC SI
   INC CX
   CMP CL,21
   JL REG_LOOP4

   MOV AL,20H
   OUT DX,AL
   }
}

void SetVideoMode(int mode)
{
   Mode.mode = mode;
   if (mode == MODE00H)                        // 40 x 25 x 16
   {
      SetMode((unsigned int)&mode00h);
      setpalette16();
      ReadBIOSfont(6,16);

      Mode.width = 40;
      Mode.height = 25;
      Mode.width_bytes = 1000;
      Mode.colors = 16;
      Mode.attrib = TVU_TEXT;
   }
   else if (mode == MODE03H)                   // 80 x 25 x 16
   {
      SetMode((unsigned int)&mode03h);
      setpalette16();
      ReadBIOSfont(6,16);

      Mode.width = 80;
      Mode.height = 25;
      Mode.width_bytes = 2000;
      Mode.colors = 16;
      Mode.attrib = TVU_TEXT;
   }
   else if (mode == MODE04H)                   // 320 x 200 x 4
   {
      SetMode((unsigned int)&mode04h);
      setpalette4();

      Mode.width = 320;
      Mode.height = 200;
      Mode.width_bytes = 8192;
      Mode.colors = 4;
      Mode.attrib = TVU_GRAPHICS;
   }
   else if (mode == MODE06H)                    // 640 x 200 x 2
   {
      SetMode((unsigned int)&mode06h);

      Mode.width = 640;
      Mode.height = 200;
      Mode.width_bytes = 8192;
      Mode.colors = 2;
      Mode.attrib = TVU_GRAPHICS;
   }
   else if (mode == MODE07H)                    // 80 x 25 x 2
   {
      SetMode((unsigned int)&mode07h);

      Mode.width = 80;
      Mode.height = 25;
      Mode.width_bytes = 2000;
      Mode.colors = 2;
      Mode.attrib = TVU_TEXT | TVU_MONOCHROME;
   }
   else if (mode == MODE0DH)                    // 320 x 200 x 16
   {
      SetMode((unsigned int)&mode0Dh);
      setpalette16();

      Mode.width = 320;
      Mode.height = 200;
      Mode.width_bytes = 8000;
      Mode.colors = 16;
      Mode.attrib = TVU_GRAPHICS | TVU_PLANAR;
   }
   else if (mode == MODE0EH)                    // 640 x 200 x 16
   {
      SetMode((unsigned int)&mode0Eh);
      setpalette16();

      Mode.width = 640;
      Mode.height = 200;
      Mode.width_bytes = 16000;
      Mode.colors = 16;
      Mode.attrib = TVU_GRAPHICS | TVU_PLANAR;
   }
   else if (mode == MODE0FH)                    // 640 x 350 x 2
   {
      SetMode((unsigned int)&mode0Fh);

      Mode.width = 640;
      Mode.height = 350;
      Mode.width_bytes = 28000;
      Mode.colors = 2;
      Mode.attrib = TVU_GRAPHICS | TVU_MONOCHROME;
   }
   else if (mode == MODE10H)                    // 640 x 350 x 16
   {
      SetMode((unsigned int)&mode10h);
      setpalette16();

      Mode.width = 640;
      Mode.height = 350;
      Mode.width_bytes = 28000;
      Mode.colors = 16;
      Mode.attrib = TVU_GRAPHICS | TVU_PLANAR;
   }
   else if (mode == MODE11H)                    // 640 x 480 x 2
   {
      SetMode((unsigned int)&mode11h);

      Mode.width = 640;
      Mode.height = 480;
      Mode.width_bytes = 38400u;
      Mode.colors = 2;
      Mode.attrib = TVU_GRAPHICS | TVU_PLANAR;
   }
   else if (mode == MODE12H)                    // 640 x 480 x 16
   {
      SetMode((unsigned int)&mode12h);
      setpalette16();

      Mode.width = 640;
      Mode.height = 480;
      Mode.width_bytes = 38400u;
      Mode.colors = 16;
      Mode.attrib = TVU_GRAPHICS | TVU_PLANAR;
   }
   else if (mode == MODE13H)                    // 320 x 200 x 256
   {
      SetMode((unsigned int)&mode13h);
      setpalette256();

      Mode.width = 320;
      Mode.height = 200;
      Mode.width_bytes = 64000u;
      Mode.colors = 256;
      Mode.attrib = TVU_GRAPHICS;
   }
   else if (mode == CHAIN4)                     // unchained 320 x 200 x 256
   {
      SetMode((unsigned int)&modeC4);
      setpalette256();

      Mode.width = 320;
      Mode.height = 200;
      Mode.width_bytes = 16000;
      Mode.colors = 256;
      Mode.attrib = TVU_GRAPHICS | TVU_UNCHAINED;
   }
   else if (mode == MODE_X)                     // unchained 320 x 240 x 256
   {
      SetMode((unsigned int)&modeC4);

      outportb(MISC_ADDR,0xE3);
      // turn off write protect
      outport(CRTC_ADDR,0x2C11);
      // vertical total
      outport(CRTC_ADDR,0x0D06);
      // overflow register
      outport(CRTC_ADDR,0x3E07);
      // vertical retrace start
      outport(CRTC_ADDR,0xEA10);
      // vertical retrace end AND wr.prot
      outport(CRTC_ADDR,0xAC11);
      // vertical display enable end
      outport(CRTC_ADDR,0xDF12);
      // start vertical blanking
      outport(CRTC_ADDR,0xE715);
      // end vertical blanking
      outport(CRTC_ADDR,0x0616);

      setpalette256();
      Mode.width = 320;
      Mode.height = 240;
      Mode.width_bytes = 19200;
      Mode.colors = 256;
      Mode.attrib = TVU_GRAPHICS | TVU_UNCHAINED;
   }
   else if (mode == MODE_A)                     // unchained 320 x 350 x 256
   {
      SetMode((unsigned int)&modeC4);

      // turn off double scanning mode
      outportb(CRTC_ADDR,9);
      outportb(CRTC_ADDR+1,inportb(CRTC_ADDR+1) & ~0x1F);
      // change the vertical resolution flags to 350
      outportb(MISC_ADDR,(inportb(0x3CC) & ~0xC0) | 0x80);
      // turn off write protect
      outport(CRTC_ADDR,0x2C11);
      // vertical total
      outport(CRTC_ADDR,0xBF06);
      // overflow register
      outport(CRTC_ADDR,0x1F07);
      // vertical retrace start
      outport(CRTC_ADDR,0x8310);
      // vertical retrace end AND wr.prot
      outport(CRTC_ADDR,0x8511);
      // vertical display enable end
      outport(CRTC_ADDR,0x5D12);
      // start vertical blanking
      outport(CRTC_ADDR,0x6315);
      // end vertical blanking
      outport(CRTC_ADDR,0xBA16);

      setpalette256();
      Mode.width = 320;
      Mode.height = 350;
      Mode.width_bytes = 28000u;
      Mode.colors = 256;
      Mode.attrib = TVU_GRAPHICS | TVU_UNCHAINED;
   }
   else if (mode == MODE_B)                     // unchained 320 x 400 x 256
   {
      SetMode((unsigned int)&modeC4);
      // turn off double scanning mode
      outportb(CRTC_ADDR,9);
      outportb(CRTC_ADDR+1,inportb(CRTC_ADDR+1) & ~0x1F);
      // change the vertical resolution flags to 400
      outportb(MISC_ADDR,(inportb(0x3CC) & ~0xC0) | 0x40);

      setpalette256();
      Mode.width = 320;
      Mode.height = 400;
      Mode.width_bytes = 32000;
      Mode.colors = 256;
      Mode.attrib = TVU_GRAPHICS | TVU_UNCHAINED;
   }
   else if (mode == MODE_C)                     // unchained 320 x 480 x 256
   {
      SetMode((unsigned int)&modeC4);

      // turn off double scanning mode
      outportb(CRTC_ADDR,9);
      outportb(CRTC_ADDR+1,inportb(CRTC_ADDR+1) & ~0x1F);
      // change the vertical resolution flags to 480
      outportb(MISC_ADDR,(inportb(0x3CC) & ~0xC0) | 0xC0);
      // turn off write protect
      outport(CRTC_ADDR,0x2C11);
      // vertical total
      outport(CRTC_ADDR,0x0D06);
      // overflow register
      outport(CRTC_ADDR,0x3E07);
      // vertical retrace start
      outport(CRTC_ADDR,0xEA10);
      // vertical retrace end AND wr.prot
      outport(CRTC_ADDR,0xAC11);
      // vertical display enable end
      outport(CRTC_ADDR,0xDF12);
      // start vertical blanking
      outport(CRTC_ADDR,0xE715);
      // end vertical blanking
      outport(CRTC_ADDR,0x0616);

      setpalette256();
      Mode.width = 320;
      Mode.height = 480;
      Mode.width_bytes = 38400u;
      Mode.colors = 256;
      Mode.attrib = TVU_GRAPHICS | TVU_UNCHAINED;
   }
   else if (mode == MODE_D)                     // unchained 360 x 200 x 256
   {
      SetMode((unsigned int)&mode13h);

      // Turn off Chain 4
      outport(SEQ_ADDR,0x0604);
      // Activate a synchronous reset
      outport(SEQ_ADDR,0x0100);
      // Select 28 mhz pixel clock
      outportb(MISC_ADDR,0xE7);
      // Release synchronous reset
      outport(SEQ_ADDR,0x0300);

      // change the vertical resolution flags to 400
      outportb(MISC_ADDR,(inportb(0x3CC) & ~0xC0) | 0x40);

      // turn off write protect
      outport(CRTC_ADDR,0x2C11);

      outport(CRTC_ADDR,0x6B00);
      outport(CRTC_ADDR,0x5901);
      outport(CRTC_ADDR,0x5A02);
      outport(CRTC_ADDR,0x8E03);
      outport(CRTC_ADDR,0x5E04);
      outport(CRTC_ADDR,0x8A05);
      outport(CRTC_ADDR,0x0008);
      outport(CRTC_ADDR,0xC009);
      outport(CRTC_ADDR,0x000A);
      outport(CRTC_ADDR,0x000B);
      outport(CRTC_ADDR,0x000C);
      outport(CRTC_ADDR,0x000D);
      outport(CRTC_ADDR,0x000E);
      outport(CRTC_ADDR,0x000F);
      outport(CRTC_ADDR,0xAC11);
      outport(CRTC_ADDR,0x2D13);
      outport(CRTC_ADDR,0x0014);
      outport(CRTC_ADDR,0xE317);
      outport(CRTC_ADDR,0xFF18);

      setpalette256();
      Mode.width = 360;
      Mode.height = 200;
      Mode.width_bytes = 18000u;
      Mode.colors = 256;
      Mode.attrib = TVU_GRAPHICS | TVU_UNCHAINED;
   }
   else if (mode == MODE_E)                     // unchained 360 x 240 x 256
   {
      SetMode((unsigned int)&mode13h);

      // Turn off Chain 4
      outport(SEQ_ADDR,0x0604);
      // Activate a synchronous reset
      outport(SEQ_ADDR,0x0100);
      // Select 28 mhz pixel clock
      outportb(MISC_ADDR,0xE7);
      // Release synchronous reset
      outport(SEQ_ADDR,0x0300);

      // change the vertical resolution flags to 480
      outportb(MISC_ADDR,(inportb(0x3CC) & ~0xC0) | 0xC0);

      // turn off write protect
      outport(CRTC_ADDR,0x2C11);

      outport(CRTC_ADDR,0x6B00);
      outport(CRTC_ADDR,0x5901);
      outport(CRTC_ADDR,0x5A02);
      outport(CRTC_ADDR,0x8E03);
      outport(CRTC_ADDR,0x5E04);
      outport(CRTC_ADDR,0x8A05);
      outport(CRTC_ADDR,0x0D06);
      outport(CRTC_ADDR,0x3E07);
      outport(CRTC_ADDR,0x0008);
      outport(CRTC_ADDR,0xC009);
      outport(CRTC_ADDR,0x000A);
      outport(CRTC_ADDR,0x000B);
      outport(CRTC_ADDR,0x000C);
      outport(CRTC_ADDR,0x000D);
      outport(CRTC_ADDR,0x000E);
      outport(CRTC_ADDR,0x000F);
      outport(CRTC_ADDR,0xEA10);
      outport(CRTC_ADDR,0xAC11);
      outport(CRTC_ADDR,0xDF12);
      outport(CRTC_ADDR,0x2D13);
      outport(CRTC_ADDR,0x0014);
      outport(CRTC_ADDR,0xE715);
      outport(CRTC_ADDR,0x0616);
      outport(CRTC_ADDR,0xE317);
      outport(CRTC_ADDR,0xFF18);

      setpalette256();
      Mode.width = 360;
      Mode.height = 240;
      Mode.width_bytes = 21600;
      Mode.colors = 256;
      Mode.attrib = TVU_GRAPHICS | TVU_UNCHAINED;
   }
   else if (mode == MODE_F)                     // unchained 360 x 350 x 256
   {
      SetMode((unsigned int)&mode13h);

      // Turn off Chain 4
      outport(SEQ_ADDR,0x0604);
      // Activate a synchronous reset
      outport(SEQ_ADDR,0x0100);
      // Select 28 mhz pixel clock
      outportb(MISC_ADDR,0xE7);
      // Release synchronous reset
      outport(SEQ_ADDR,0x0300);

      // change the vertical resolution flags to 350
      outportb(MISC_ADDR,(inportb(0x3CC) & ~0xC0) | 0x80);

      // turn off write protect
      outport(CRTC_ADDR,0x2C11);

      outport(CRTC_ADDR,0x6B00);
      outport(CRTC_ADDR,0x5901);
      outport(CRTC_ADDR,0x5A02);
      outport(CRTC_ADDR,0x8E03);
      outport(CRTC_ADDR,0x5E04);
      outport(CRTC_ADDR,0x8A05);
      outport(CRTC_ADDR,0xBF06);
      outport(CRTC_ADDR,0x1F07);
      outport(CRTC_ADDR,0x0008);
      outport(CRTC_ADDR,0x4009);
      outport(CRTC_ADDR,0x000A);
      outport(CRTC_ADDR,0x000B);
      outport(CRTC_ADDR,0x000C);
      outport(CRTC_ADDR,0x000D);
      outport(CRTC_ADDR,0x000E);
      outport(CRTC_ADDR,0x000F);
      outport(CRTC_ADDR,0x8310);
      outport(CRTC_ADDR,0x8511);
      outport(CRTC_ADDR,0x5D12);
      outport(CRTC_ADDR,0x2D13);
      outport(CRTC_ADDR,0x0014);
      outport(CRTC_ADDR,0x6315);
      outport(CRTC_ADDR,0xBA16);
      outport(CRTC_ADDR,0xE317);
      outport(CRTC_ADDR,0xFF18);

      setpalette256();
      Mode.width = 360;
      Mode.height = 350;
      Mode.width_bytes = 31500;
      Mode.colors = 256;
      Mode.attrib = TVU_GRAPHICS | TVU_UNCHAINED;
   }
   else if (mode == MODE_G)                     // unchained 360 x 400 x 256
   {
      SetMode((unsigned int)&mode13h);

      // Turn off Chain 4
      outport(SEQ_ADDR,0x0604);
      // Activate a synchronous reset
      outport(SEQ_ADDR,0x0100);
      // Select 28 mhz pixel clock
      outportb(MISC_ADDR,0xE7);
      // Release synchronous reset
      outport(SEQ_ADDR,0x0300);

      // change the vertical resolution flags to 400
      outportb(MISC_ADDR,(inportb(0x3CC) & ~0xC0) | 0x40);

      // turn off write protect
      outport(CRTC_ADDR,0x2C11);

      outport(CRTC_ADDR,0x6B00);
      outport(CRTC_ADDR,0x5901);
      outport(CRTC_ADDR,0x5A02);
      outport(CRTC_ADDR,0x8E03);
      outport(CRTC_ADDR,0x5E04);
      outport(CRTC_ADDR,0x8A05);
      outport(CRTC_ADDR,0x0008);
      outport(CRTC_ADDR,0x4009);
      outport(CRTC_ADDR,0x000A);
      outport(CRTC_ADDR,0x000B);
      outport(CRTC_ADDR,0x000C);
      outport(CRTC_ADDR,0x000D);
      outport(CRTC_ADDR,0x000E);
      outport(CRTC_ADDR,0x000F);
      outport(CRTC_ADDR,0xAC11);
      outport(CRTC_ADDR,0x2D13);
      outport(CRTC_ADDR,0x0014);
      outport(CRTC_ADDR,0xE317);
      outport(CRTC_ADDR,0xFF18);

      setpalette256();
      Mode.width = 360;
      Mode.height = 400;
      Mode.width_bytes = 36000u;
      Mode.colors = 256;
      Mode.attrib = TVU_GRAPHICS | TVU_UNCHAINED;
   }
   else if (mode == MODE_H)                     // unchained 360 x 480 x 256
   {
      SetMode((unsigned int)&mode13h);

      // Turn off Chain 4
      outport(SEQ_ADDR,0x0604);
      // Activate a synchronous reset
      outport(SEQ_ADDR,0x0100);
      // Select 28 mhz pixel clock
      outportb(MISC_ADDR,0xE7);
      // Release synchronous reset
      outport(SEQ_ADDR,0x0300);

      // change the vertical resolution flags to 480
      outportb(MISC_ADDR,(inportb(0x3CC) & ~0xC0) | 0xC0);

      // turn off write protect
      outport(CRTC_ADDR,0x2C11);

      outport(CRTC_ADDR,0x6B00);
      outport(CRTC_ADDR,0x5901);
      outport(CRTC_ADDR,0x5A02);
      outport(CRTC_ADDR,0x8E03);
      outport(CRTC_ADDR,0x5E04);
      outport(CRTC_ADDR,0x8A05);
      outport(CRTC_ADDR,0x0D06);
      outport(CRTC_ADDR,0x3E07);
      outport(CRTC_ADDR,0x0008);
      outport(CRTC_ADDR,0x4009);
      outport(CRTC_ADDR,0x000A);
      outport(CRTC_ADDR,0x000B);
      outport(CRTC_ADDR,0x000C);
      outport(CRTC_ADDR,0x000D);
      outport(CRTC_ADDR,0x000E);
      outport(CRTC_ADDR,0x000F);
      outport(CRTC_ADDR,0xEA10);
      outport(CRTC_ADDR,0xAC11);
      outport(CRTC_ADDR,0xDF12);
      outport(CRTC_ADDR,0x2D13);
      outport(CRTC_ADDR,0x0014);
      outport(CRTC_ADDR,0xE715);
      outport(CRTC_ADDR,0x0616);
      outport(CRTC_ADDR,0xE317);
      outport(CRTC_ADDR,0xFF18);

      setpalette256();
      Mode.width = 360;
      Mode.height = 480;
      Mode.width_bytes = 43200u;
      Mode.colors = 256;
      Mode.attrib = TVU_GRAPHICS | TVU_UNCHAINED;
   }
   else if (mode == MODE_I)                     // 640 x 400 x 16
   {
      SetMode((unsigned int)&mode10h);
      asm {

         MOV DX,03CCH
         IN AL,DX
         AND AL,03FH
         OR AL,40H

         MOV DX,03C2H
         OUT DX,AL

         MOV DX,CRTC_ADDR
         MOV AX,9C10H
         OUT DX,AX

         MOV AX,8311H
         OUT DX,AX

         MOV AX,8F12H
         OUT DX,AX

         MOV AX,9615H
         OUT DX,AX

         MOV AX,0B916H
         OUT DX,AX
      }
      setpalette16();
      Mode.width = 640;
      Mode.height = 400;
      Mode.width_bytes = 32000;
      Mode.colors = 16;
      Mode.attrib = TVU_GRAPHICS | TVU_PLANAR;
   }
   else if (mode == MODE_J)                    // 80 x 43 x 16
   {
      SetMode((unsigned int)&modeJ);
      ReadBIOSfont(3,8);

      Mode.width = 80;
      Mode.height = 43;
      Mode.width_bytes = 3440;
      Mode.colors = 16;
      Mode.attrib = TVU_TEXT;
   }
   else if (mode == MODE_K)                    // 80 x 50 x 16
   {
      SetMode((unsigned int)&modeK);
      ReadBIOSfont(3,8);

      Mode.width = 80;
      Mode.height = 50;
      Mode.width_bytes = 4000;
      Mode.colors = 16;
      Mode.attrib = TVU_TEXT;
   }
   else if (mode == MODE_L)                    // 40 x 43 x 16
   {
      SetMode((unsigned int)&modeL);
      ReadBIOSfont(3,8);

      Mode.width = 40;
      Mode.height = 43;
      Mode.width_bytes = 4000;
      Mode.colors = 16;
      Mode.attrib = TVU_TEXT;
   }
   else if (mode == MODE_M)                    // 40 x 50 x 16
   {
      SetMode((unsigned int)&modeM);
      ReadBIOSfont(3,8);

      Mode.width = 40;
      Mode.height = 50;
      Mode.width_bytes = 4000;
      Mode.colors = 16;
      Mode.attrib = TVU_TEXT;
   }
}

void setpal(int color, char r, char g, char b)
{
   asm {
   // Send color
   MOV AX,color
   MOV DX,03C8H
   OUT DX,AL

   // Write R value
   MOV DX,03C9H
   MOV AL,r
   OUT DX,AL

   // Write G value
   MOV DX,03C9H
   MOV AL,g
   OUT DX,AL

   // Write B value
   MOV DX,03C9H
   MOV AL,b
   OUT DX,AL
   }
}

void setpalette4()
{
   setpal( 0,  0,  0,  0);
   setpal( 1,  0, 42, 42);
   setpal( 2, 42,  0, 42);
   setpal( 3, 63, 63, 63);
}

void setpalette16()
{
   int j = 0;
   for (int i = 0; i < 48; i+=3)
   {
      setpal(j, Pal[i], Pal[i+1], Pal[i+2]);
      j++;
   }
}

void setpalette256()
{
   int j = 0;
   for (int i = 0; i < 768; i+=3)
   {
      setpal(j, Pal[i], Pal[i+1], Pal[i+2]);
      j++;
   }
}
