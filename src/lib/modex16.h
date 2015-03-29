/*
 * Functions for handling modex and doing other basic graphics stuff.
 */
#ifndef MODEX16_H
#define MODEX16_H
#include <conio.h>
#include "src\lib\types.h"
#include "src\lib\bitmap.h"
#include "src\lib\planar.h"

/* -========================== Types & Macros ==========================- */
#define PAGE_OFFSET(x,y) (((y)<<6)+((y)<<4)+((x)>>2))
#define PLANE(x) (1<< (x&3))
#define SELECT_ALL_PLANES() outpw(0x03c4, 0xff02)

typedef struct {
    byte far* data;	/* the data for the page */
    word dx;		/* col we are viewing on the virtual screen */
    word dy;		/* row we are viewing on the virtual screen */
    word width;		/* virtual width of the page */
    word height;	/* virtual height of the page */
} page_t;

/* -============================ Functions =============================- */
/* mode switching, page, and plane functions */
void modexEnter();
void modexLeave();
page_t modexDefaultPage();
page_t modexNextPage(page_t *p);
void modexShowPage(page_t *page);
void modexPanPage(page_t *page, int dx, int dy);
void modexSelectPlane(byte plane);
void modexClearRegion(page_t *page, int x, int y, int w, int h, byte color);
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
void modexPalUpdate(byte *p, word *i);
void modexPalUpdate2(byte *p);
void modexWaitBorder();

/* -======================= Constants & Vars ==========================- */
extern byte far*  VGA;  /* The VGA Memory */
#define SCREEN_SEG		0xa000
#define VIDEO_INT		0x10
#define SET_MODE		0x00
#define VGA_256_COLOR_MODE 	0x13
#define TEXT_MODE		0x03
#define SCREEN_WIDTH		320
#define SCREEN_HEIGHT		240
#define PAGE_SIZE		(word)(SCREEN_WIDTH/4 * SCREEN_HEIGHT)

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
#define PAL_READ_REG            0x03C7   /* Color register, read address */
#define PAL_WRITE_REG           0x03C8   /* Color register, write address */
#define PAL_DATA_REG            0x03C9   /* Color register, data port */
#define PAL_SIZE                (256 * 3)
#endif
