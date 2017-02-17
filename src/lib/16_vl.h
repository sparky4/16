/* Project 16 Source Code~
 * Copyright (C) 2012-2017 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
 *
 * This file is part of Project 16.
 *
 * Project 16 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Project 16 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 * write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */
/*
 * Functions for handling modex and doing other basic graphics stuff.
 */
#ifndef MODEX16_H
#define MODEX16_H

#include "src/lib/16_head.h"
//#include "src/lib/planar.h"
//#include "src/lib/modex16/16planar.h"
#include "src/lib/16text.h"
////#include "src/lib/modex16/320x240.h"
// #include "src/lib/modex16/320x200.h"
// #include "src/lib/modex16/256x192.h"
// #include "src/lib/modex16/192x144_.h"
// #include "src/lib/modex16/160x120.h"

#ifdef __WATCOMC__
#include <hw/cpu/cpu.h>
#include <hw/dos/dos.h>
#include <hw/vga/vga.h>
#include <hw/vga/vrl.h>
#include <hw/vga/vrs.h>
#endif

//320x240 = 20x15
//192x144 = 12x9

//temp defines
#define TILEWH	16
#define TILEWHD	TILEWH*2
#define QUADWH			TILEWH/2

//#define OLDBGPRESERVE

/* -========================== Types & Macros ==========================- */
#define PAGE_OFFSET(x,y) (((y)<<6)+((y)<<4)+((x)>>2))
#define PLANE(x) (1 << ((x) & 3))
#define SELECT_ALL_PLANES() outpw(0x03c4, 0xff02)

typedef union
{
	byte red;
	byte green;
	byte blue;
} rgb_t;

/* -======================= Constants & Vars ==========================- */
extern byte far*  VGA;  /* The VGA Memory */
#define SCREEN_SEG		0xa000
#define VIDEO_INT		0x10
#define SET_MODE		0x00
#define VGA_256_COLOR_MODE 	0x13
#define TEXT_MODE		0x03

#define AC_INDEX		0x03c0
#define SC_INDEX		0x03c4
#define SC_DATA			0x03c5
#define CRTC_INDEX		0x03d4
#define CRTC_DATA		0x03d5
#define GC_INDEX		0x03ce
#define MISC_OUTPUT		0x03c2
#define HIGH_ADDRESS 		0x0C
#define LOW_ADDRESS		0x0D
#define VRETRACE		0x08
//#define INPUT_STATUS_1		0x03da	defined in 16_head
#define DISPLAY_ENABLE		0x01
#define MAP_MASK		0x02
#define PAL_READ_REG			0x03C7   /* Color register, read address */
#define PAL_WRITE_REG		   0x03C8   /* Color register, write address */
#define PAL_DATA_REG			0x03C9   /* Color register, data port */
#define PAL_SIZE				(256 * 3)

/* -============================ Functions =============================- */
/* mode switching, page, and plane functions */
void VGAmodeX(sword vq, boolean cmem, global_game_variables_t *gv);
void modexEnter(sword vq, boolean cmem, global_game_variables_t *gv);
byte vgaGetMode();
void modexLeave();
void modexsetBaseXMode();
page_t modexDefaultPage(page_t *p);
page_t modexNextPage(page_t *p);
page_t modexNextPageFlexibleSize(page_t *p, word x, word y);
void modexCalcVmemRemain(video_t *video);
void VL_Initofs(video_t *video);
void modexHiganbanaPageSetup(video_t *video);
//void modexShowPage(page_t *page);
void VL_ShowPage(page_t *page, boolean vsync, boolean sr);
void modexPanPage(page_t *page, int dx, int dy);
void modexSelectPlane(byte plane);
void modexClearRegion(page_t *page, int x, int y, int w, int h, byte color);
/* moved to src/lib/modex16/16render.c */
void modexCopyPageRegion(page_t *dest, page_t *src, word sx, word sy, word dx, word dy, word width, word height);

/* Palette fade and flash effects */
void modexFadeOn(word fade, byte *palette);
void modexFadeOff(word fade, byte *palette);
void modexFlashOn(word fade, byte *palette);
void modexFlashOff(word fade, byte *palette);

/* palette loading and saving */
void modexPalSave(byte *palette);
//byte *modexNewPal();
void modexLoadPalFile(char *filename, byte *palette);
void VL_LoadPalFile(const char *filename, byte *palette);
void VL_LoadPalFilewithoffset(const char *filename, byte *palette, word o);
void VL_UpdatePaletteWrite(byte *palette, word o);
void modexSavePalFile(char *filename, byte *palette);

/* fixed palette functions */
void modexPalBlack();
void modexPalWhite();

/* utility functions */
void modexPalUpdate(byte *p);
void modexPalUpdate0(byte *p);
void modexPalOverscan(word col);
void modexputPixel(page_t *page, int x, int y, byte color);
byte modexgetPixel(page_t *page, int x, int y);

#if 0 // not needed anymore. maybe good for reference purposes though.
static inline void modexwritepixel(page_t *page, int x, int y, word addr, byte color)
{
	/* Each address accesses four neighboring pixels, so set
	   Write Plane Enable according to which pixel we want
	   to modify.  The plane is determined by the two least
	   significant bits of the x-coordinate: */
	modexSelectPlane(PLANE(x));
	//outp(SC_INDEX, 0x02);
	//outp(SC_DATA, 0x01 << (x & 3));

	/* The offset of the pixel into the video segment is
	   offset = (width * y + x) / 4, and write the given
	   color to the plane we selected above.  Heed the active
	   page start selection. */
	vga_state.vga_graphics_ram[addr] = color;
}
static inline byte modexreadPixel(page_t *page, int x, int y, word addr)
{
	/* Select the plane from which we must read the pixel color: */
	outpw(GC_INDEX, 0x04);
	outpw(GC_INDEX+1, x & 3);
	return vga_state.vga_graphics_ram[addr];
}
#endif

void modexDrawChar(page_t *page, int x/*for planar selection only*/, word t, word col, word bgcol, word addr);
void modexprint(page_t *page, sword x, sword y, word t, boolean tlsw, word col, word bgcol, const byte *str);
void modexprintbig(page_t *page, word x, word y, word t, word col, word bgcol, const byte *str);
void modexpdump(page_t *pee);
void modexcls(page_t *page, byte color, byte *Where);
void VL_PatternDraw(video_t *video, word pn, boolean sw, boolean allsw);
void modexWaitBorder();
void modexWaitBorder_start();
void modexWaitBorder_end();
void VL_PrintmodexmemInfo(video_t *v);

#endif
