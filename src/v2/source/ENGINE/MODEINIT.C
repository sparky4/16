/*
Copyright (C) 1998 BJ Eirich (aka vecna)
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public Lic
See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <dpmi.h>
#include <sys\nearptr.h>
#include "verge.h"

void BIOS_SetVideoMode(int mode)
{
  REGISTERS r;
  SET_AX(r, mode);
  INTERRUPT(0x10, r);
}

// =============================== Mode 13h ===================================

int Mode13hShutdown(int i)
{
  if (i) BIOS_SetVideoMode(0x3);
  free(screen);
  return 0;
}

void InitMode13h(void)
{
  BIOS_SetVideoMode(0x13);

  DriverDesc = "320x200 (Mode 13h, linear)";
  sx=320,   sy=200;
  tx=20,    ty=13;
  cx1=0,    cy1=0;
  cx2=sx-1, cy2=sy-1;

  // setup function drivers
  ShowPage=LFB_ShowPage;
  ShutdownVideo=Mode13hShutdown;

  video=(byte *) 0xA0000 + __djgpp_conventional_base;
  vscreen=(byte *) valloc(sx*sy, "vscreen", 0);
  screen=vscreen;
  memset(screen, 0, sx*sy);
}

// ================================= Mode-X ==================================

#define SEQU_ADDR       0x3c4
#define CRTC_ADDR       0x3d4
#define MISC_OUTPUT     0x3c2

#define ATTRCON_ADDR    0x3c0
#define MISC_ADDR       0x3c2
#define VGAENABLE_ADDR  0x3c3
#define SEQ_ADDR        0x3c4
#define GRACON_ADDR     0x3ce
#define CRTC_ADDR       0x3d4
#define STATUS_ADDR     0x3da

typedef struct
{
  unsigned port;
  byte index, value;
} Register;

typedef Register *RegisterPtr;

Register scr256x256[] =
{
  { 0x3c2, 0x00, 0xe3},{ 0x3d4, 0x00, 0x5f},{ 0x3d4, 0x01, 0x3f},
  { 0x3d4, 0x02, 0x40},{ 0x3d4, 0x03, 0x82},{ 0x3d4, 0x04, 0x4A},
  { 0x3d4, 0x05, 0x9A},{ 0x3d4, 0x06, 0x23},{ 0x3d4, 0x07, 0xb2},
  { 0x3d4, 0x08, 0x00},{ 0x3d4, 0x09, 0x61},{ 0x3d4, 0x10, 0x0a},
  { 0x3d4, 0x11, 0xac},{ 0x3d4, 0x12, 0xff},{ 0x3d4, 0x13, 0x20},
  { 0x3d4, 0x14, 0x40},{ 0x3d4, 0x15, 0x07},{ 0x3d4, 0x16, 0x1a},
  { 0x3d4, 0x17, 0xa3},{ 0x3c4, 0x01, 0x01},{ 0x3c4, 0x04, 0x0e},
  { 0x3ce, 0x05, 0x40},{ 0x3ce, 0x06, 0x05},{ 0x3c0, 0x10, 0x41},
  { 0x3c0, 0x13, 0x00}
};

// ================================= Code ====================================

void plane(byte p)
{
  int hi=1<<p;
  outpw(0x03c4, (hi<<8)|0x02);
}

int ModeXShutdown(int i)
{
  if (i) BIOS_SetVideoMode(0x3);
  vfree(screen);
  return 0;
}

int ModeXShowPage()
{
  byte *s,*d;
  int x,y,k;
  int sx2;

  if (key[SCAN_ALT] && key[SCAN_X]) err("Exiting: ALT-X pressed.");
  RenderGUI();
  cpubyte=PFLIP;

  d=video;
  sx2=sx>>2;

  for (y=0; y<sy; ++y,d+=sx2)
  {
    s=screen+(y*sx);
    for (k=0; k<4; k++,s-=sx)
    {
      plane(k);
      for (x=0; x<sx2; x++,s+=4)
        d[x]=s[k];
    }
  }
  cpubyte=ETC;
  return 0;
}

static void outReg(Register r)
{
  switch (r.port)
  {
    // First handle special cases:

    case ATTRCON_ADDR:
      // reset read/write flip-flop
      inp(STATUS_ADDR);
      // ensure VGA output is enabled
      outp(ATTRCON_ADDR, r.index | 0x20);
      outp(ATTRCON_ADDR, r.value);
      break;

    case MISC_ADDR:
    case VGAENABLE_ADDR:
      //  directly to the port
      outp(r.port, r.value);
      break;

    case SEQ_ADDR:
    case GRACON_ADDR:
    case CRTC_ADDR:
    default:
      // index to port
      outp(r.port, r.index);
      // value to port+1
      outp(r.port + 1, r.value);
      break;
  }
}

// readyVgaRegs() does the initialization to make the VGA ready to
// accept any combination of configuration register settings.
//
// This involves enabling writes to index 0 to 7 of the CRT controller
// (port 0x3d4), by clearing the most significant bit (bit 7) of index
// 0x11.

static void readyVgaRegs(void)
{
  int v;

  outp(0x3d4, 0x11);
  v = inp(0x3d5) & 0x7f;
  outp(0x3d4, 0x11);
  outp(0x3d5, v);
}

// outRegArray sets n registers according to the array pointed to by r.
// First, indexes 0-7 of the CRT controller are enabled for writing.

static void outRegArray(Register *r, int n)
{
  readyVgaRegs();
  while (n--)
    outReg (*r++);
}

void Set256x256()
{
  DriverDesc = "256x256 (ModeX, planar)";

  outRegArray(scr256x256, sizeof(scr256x256) / sizeof (Register));

  sx=256,   sy=256;
  tx=17,    ty=16;
  cx1=0,    cy1=0;
  cx2=sx-1, cy2=sy-1;
}

void Set320x240()
{ char in_byte;

  DriverDesc = "320x240 (ModeX, planar)";

  outpw(SEQU_ADDR, 0x0604);
  memset((unsigned char *)0xA0000 + __djgpp_conventional_base,0,0x10000); // for clean mode switch
  outpw(SEQU_ADDR, 0x0110);
  outp(MISC_OUTPUT, 0xe3);
  outpw(SEQU_ADDR, 0x0300);
  outp(CRTC_ADDR, 0x11);
  in_byte = inp((CRTC_ADDR+1));
  in_byte = (in_byte && 0x7f);
  outp((CRTC_ADDR+1), in_byte);
  outpw(CRTC_ADDR, 0x0d06);
  outpw(CRTC_ADDR, 0x3e07);
  outpw(CRTC_ADDR, 0x4109);
  outpw(CRTC_ADDR, 0xea10);
  outpw(CRTC_ADDR, 0xac11);
  outpw(CRTC_ADDR, 0xdf12);
  outpw(CRTC_ADDR, 0x0014);
  outpw(CRTC_ADDR, 0xe715);
  outpw(CRTC_ADDR, 0x0616);
  outpw(CRTC_ADDR, 0xe317);

  sx=320,   sy=240;
  tx=20,    ty=15;
  cx1=0,    cy1=0;
  cx2=sx-1, cy2=sy-1;
}

void Set360x240()
{ char in_byte;

  DriverDesc = "360x240 (ModeX, planar)";

  outpw(SEQU_ADDR, 0x0604);
  memset((unsigned char *)0xA0000 + __djgpp_conventional_base,0,0x10000); // for clean mode switch
  outpw(SEQU_ADDR, 0x100);
  outp(MISC_OUTPUT, 0xe7);
  outpw(SEQU_ADDR, 0x300);
  outp(CRTC_ADDR, 0x11);
  in_byte = inp((CRTC_ADDR+1));
  in_byte = (in_byte && 0x7f);
  outp((CRTC_ADDR+1), in_byte);
  outpw(CRTC_ADDR, 0x6b00);
  outpw(CRTC_ADDR, 0x5901);
  outpw(CRTC_ADDR, 0x5a02);
  outpw(CRTC_ADDR, 0x8e03);
  outpw(CRTC_ADDR, 0x5e04);
  outpw(CRTC_ADDR, 0x8a05);
  outpw(CRTC_ADDR, 0x0d06);
  outpw(CRTC_ADDR, 0x3e07);
  outpw(CRTC_ADDR, 0x4109);
  outpw(CRTC_ADDR, 0xea10);
  outpw(CRTC_ADDR, 0xac11);
  outpw(CRTC_ADDR, 0xdf12);
  outpw(CRTC_ADDR, 0x2d13);
  outpw(CRTC_ADDR, 0x0014);
  outpw(CRTC_ADDR, 0xe715);
  outpw(CRTC_ADDR, 0x0616);
  outpw(CRTC_ADDR, 0xe317);

  sx=360,   sy=240;
  tx=23,    ty=15;
  cx1=0,    cy1=0;
  cx2=sx-1, cy2=sy-1;
}

void InitModeX(int xres, int yres)
{
  int found;

  BIOS_SetVideoMode(0x13);

  found=0;
  if (xres==320 && yres==240) { Set320x240(); found=1; }
  if (xres==360 && yres==240) { Set360x240(); found=1; }
  if (xres==256 && yres==256) { Set256x256(); found=1; }

  if (!found)
  {
    // resolution not found, bitch...
    err("Internal error: unknown ModeX resolution");
  }
  // resolution found..
  else
  {
    ShowPage=LFB_ShowPage;
    if (xres != 256 && yres != 256)
      ShowPage=ModeXShowPage;
    ShutdownVideo=ModeXShutdown;
  }

  video=(byte *) 0xA0000 + __djgpp_conventional_base;
  vscreen=(byte *) valloc(sx*sy, "vscreen", 0);
  screen=vscreen;
  memset(screen, 0, sx*sy);
}

// ================================= VESA ====================================

int VESAShutdown(int i)
{
  if (i) BIOS_SetVideoMode(0x3);
  free(screen);
  return 0;
}

void VESASetBank(int bank)
{
  REGISTERS r;
  SET_AX(r, 0x4f05);
  SET_BX(r, 0);
  SET_DX(r, bank);
  INTERRUPT(0x10, r);
}

int VESAShowPage(void)
{
  byte *s,*d;
  int y,bytes,bank;
  int span;

  if (key[SCAN_ALT] && key[SCAN_X]) err("Exiting: ALT-X pressed.");
  RenderGUI();
  cpubyte=PFLIP;

  s=screen;
  d=video;

  bank=0;
  bytes=65536;
  VESASetBank(bank++);

  // *** no padding exists now, can be rewritten smaller/faster - aen
  for (y=sy; y; y--)
  {
    if (bytes >= sx)
    { memcpy(d,s,sx);
      bytes-=sx;
      s+=sx;
      d+=sx;
    }
    else
    { memcpy(d,s,bytes);
      s+=bytes;
      span=sx-bytes;

      VESASetBank(bank++);
      d=video;
      bytes=65536;

      memcpy(d,s,span);
      bytes-=span;
      s+=span;
      d+=span;
    }
  }
  cpubyte=ETC;
  return 0;
}

void Set640x480()
{
  REGISTERS r;

  DriverDesc = "640x480 (VESA, Banked)";

  SET_BX(r, 0x0101);
  SET_AX(r, 0x4f02);
  INTERRUPT(0x10, r);

  sx=640;  sy=480;
  tx=40;   ty=30;
  cx1=0;   cy1=0;
  cx2=sx-1;cy2=sy-1;
}

void InitVESA(int xres, int yres)
{
  int found;

  found=0;
  if (xres==640 && yres==480) { Set640x480(); found=1; }

  if (!found)
    err("Internal error: unknown VESA resolution");

  // setup driver functions
  ShowPage = VESAShowPage; // reassign for banked VESA video blt
  ShutdownVideo = VESAShutdown;

  video=(byte *) 0xA0000 + __djgpp_conventional_base;
  vscreen=(byte *) valloc(sx*sy, "vscreen", 0);
  screen=vscreen;
  memset(screen, 0, sx*sy);
}

// ============================================================================

int InitVideo(int xres, int yres)
{
  int found;

  found=0;
  if (xres==320 && yres==200) { InitMode13h(); found=1; }
  if (xres==320 && yres==240) { InitModeX(xres,yres); found=1; }
  if (xres==360 && yres==240) { InitModeX(xres,yres); found=1; }
  if (xres==256 && yres==256) { InitModeX(xres,yres); found=1; }
  if (xres==640 && yres==480) { InitVESA(xres,yres); found=1; }

  if (found)
  {
    tsx=xres; tsy=yres;
    Logp("Sys: Initializing %s.", DriverDesc);
    InitMouse(tsx, tsy);
  }
  else
  {
    InitMode13h();
    tsx=320; tsy=200;
    Logp("Unknown video mode %dx%d; defaulting to mode 13h.", xres, yres);
    InitMouse(tsx, tsy);
  }
  LogDone();
  return found;
}
