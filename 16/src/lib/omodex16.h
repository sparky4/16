/*
 * Functions for handling omodex and doing other basic graphics stuff.
 */
#ifndef omodex16_H
#define omodex16_H
#include <conio.h>
#include "src/lib/16_head.h"
#include "16/src/lib/bitmap.h"

/* -========================== Types & Macros ==========================- */
#define PAGE_OFFSET(x,y) (((y)<<6)+((y)<<4)+((x)>>2))
//#define PLANE(x) (1<< (x&3))
#define SELECT_ALL_PLANES() outpw(0x03c4, 0xff02)

typedef struct {
    byte far* data;	/* the data for the page */
    word dx;		/* col we are viewing on the virtual screen */
    word dy;		/* row we are viewing on the virtual screen */
    word width;		/* virtual width of the page */
    word height;	/* virtual height of the page */
} opage_t;

/* -============================ Functions =============================- */
/* mode switching, page, and plane functions */
void omodexEnter();
void omodexLeave();
opage_t omodexDefaultPage();
opage_t omodexNextPage(opage_t *p);
void omodexShowPage(opage_t *page);
void omodexPanPage(opage_t *page, int dx, int dy);
void omodexSelectPlane(byte plane);
void omodexClearRegion(opage_t *page, int x, int y, int w, int h, byte color);
void omodexDrawBmp(opage_t *page, int x, int y, bitmap_t *bmp);
void omodexDrawBmpRegion(opage_t *page, int x, int y, int rx, int ry, int rw, int rh, bitmap_t *bmp);
void omodexDrawSprite(opage_t *page, int x, int y, bitmap_t *bmp);
void omodexDrawSpriteRegion(opage_t *page, int x, int y, int rx, int ry, int rw, int rh, bitmap_t *bmp);

/* Palette fade and flash effects */
void omodexFadeOn(word fade, byte *palette);
void omodexFadeOff(word fade, byte *palette);
void omodexFlashOn(word fade, byte *palette);
void omodexFlashOff(word fade, byte *palette);

/* palette loading and saving */
void omodexPalSave(byte *palette);
byte *omodexNewPal();
void omodexLoadPalFile(char *filename, byte **palette);
void omodexSavePalFile(char *filename, byte *palette);

/* fixed palette functions */
void omodexPalBlack();
void omodexPalWhite();

/* utility functions */
void omodexPalUpdate(byte *p);
void omodexWaitBorder();
int US_CheckParm(char *parm,char **strings);

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
//#define CRTC_INDEX		0x03d4
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
