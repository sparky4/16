/* Project 16 Source Code~
 * Copyright (C) 2012-2015 sparky4 & pngwen & andrius4669
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
//#include <conio.h>

#include "src/lib/types.h"
#include "src/lib/bitmap.h"
#include "src/lib/planar.h"
#include "src/lib/16_head.h"
#include "src/lib/16text.h"
#include "src/lib/modex16/320x240.h"
#include "src/lib/modex16/320x200.h"
#include "src/lib/modex16/256x192.h"
#include "src/lib/modex16/192x144_.h"
#include "src/lib/modex16/160x120.h"

//320x240 = 20x15
//192x144 = 12x9

//temp defines
#define TILEWH 16
#define QUADWH			TILEWH/2

/* -========================== Types & Macros ==========================- */
#define PAGE_OFFSET(x,y) (((y)<<6)+((y)<<4)+((x)>>2))
#define PLANE(x) (1<< (x&3))
#define SELECT_ALL_PLANES() outpw(0x03c4, 0xff02)

typedef union
{
	byte red;
	byte green;
	byte blue;
} rgb_t;

/* -============================ Functions =============================- */
/* mode switching, page, and plane functions */
void VGAmodeX(sword vq, global_game_variables_t *gv);
void modexEnter(sword vq, global_game_variables_t *gv);
long vgaGetMode();
void modexLeave();
void modexsetBaseXMode(page_t *page);
page_t modexDefaultPage(page_t *p);
page_t modexNextPage(page_t *p);
page_t modexNextPageFlexibleSize(page_t *p, word x, word y);
void modexShowPage(page_t *page);
void modexPanPage(page_t *page, int dx, int dy);
void modexSelectPlane(byte plane);
void modexClearRegion(page_t *page, int x, int y, int w, int h, byte color);
void modexDrawPBufRegion(page_t *page, int x, int y, int rx, int ry, int rw, int rh, planar_buf_t *p, boolean sprite);
void modexDrawPBuf(page_t *page, int x, int y, planar_buf_t *p, boolean sprite);
void oldDrawBmp(byte far* page, int x, int y, bitmap_t *bmp, byte sprite);
void modexDrawBmp(page_t *page, int x, int y, bitmap_t *bmp);
void modexDrawBmpRegion(page_t *page, int x, int y, int rx, int ry, int rw, int rh, bitmap_t *bmp);
void modexDrawPlanarBuf(page_t *page, int x, int y, planar_buf_t *bmp);
void modexDrawSprite(page_t *page, int x, int y, bitmap_t *bmp);
void modexDrawSpriteRegion(page_t *page, int x, int y, int rx, int ry, int rw, int rh, bitmap_t *bmp);
void modexCopyPageRegion(page_t *dest, page_t *src, word sx, word sy, word dx, word dy, word width, word height);

/* Palette fade and flash effects */
void modexFadeOn(word fade, byte *palette);
void modexFadeOff(word fade, byte *palette);
void modexFlashOn(word fade, byte *palette);
void modexFlashOff(word fade, byte *palette);

/* palette loading and saving */
void modexPalSave(byte *palette);
byte *modexNewPal();
void modexLoadPalFile(char *filename, byte **palette);
void modexSavePalFile(char *filename, byte *palette);

/* fixed palette functions */
void modexPalBlack();
void modexPalWhite();

/* utility functions */
void modexPalUpdate(bitmap_t *bmp, word *i, word qp, word aqoffset);
void modexPalUpdate1(byte *p);
void modexPalUpdate0(byte *p);
void modexPalOverscan(byte *p, word col);
void chkcolor(bitmap_t *bmp, word *q, word *a, word *aa, word *z, word *i/*, word *offset*/);
void modexputPixel(page_t *page, int x, int y, byte color);
byte modexgetPixel(page_t *page, int x, int y);
void modexhlin(page_t *page, word xl, word xh, word y, word color);
void modexprint(page_t *page, word x, word y, word t, word col, word bgcol, const byte *str);
void modexprintbig(page_t *page, word x, word y, word t, word col, word bgcol, const byte *str);
void pdump(page_t *pee);
void cls(page_t *page, byte color, byte *Where);
void modexWaitBorder();

/* -======================= Constants & Vars ==========================- */
extern byte far*  VGA;  /* The VGA Memory */
//extern int old_mode;
#define SCREEN_SEG		0xa000
#define VIDEO_INT		0x10
#define SET_MODE		0x00
#define VGA_256_COLOR_MODE 	0x13
#define TEXT_MODE		0x03
//#define SCREEN_WIDTH		320
//#define SCREEN_HEIGHT		240
//#define PAGE_SIZE		(word)(SCREEN_WIDTH/4 * SCREEN_HEIGHT)

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
#define INPUT_STATUS_1		0x03da
#define DISPLAY_ENABLE		0x01
#define MAP_MASK		0x02
#define PAL_READ_REG			0x03C7   /* Color register, read address */
#define PAL_WRITE_REG		   0x03C8   /* Color register, write address */
#define PAL_DATA_REG			0x03C9   /* Color register, data port */
#define PAL_SIZE				(256 * 3)
#endif
