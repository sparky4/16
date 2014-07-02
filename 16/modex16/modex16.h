/*
 * Functions for handling modex and doing other basic graphics stuff.
 */
#ifndef MODEX16_H
#define MODEX16_H
#include <conio.h>
#include "types.h"

/* -========================== Types & Macros ==========================- */
#define PAGE_OFFSET(x,y) ((y<<6)+(y<<4)+(x>>2))
#define PLANE(x) (1<< (x&3))
typedef byte far* page_t;
#define SELECT_ALL_PLANES() outpw(0x03c4, 0xff02)

/* -============================ Functions =============================- */
void modexEnter();
void modexLeave();
void modexShowPage(page_t page);
void modexPanPage(page_t *page, int dx, int dy);
void modexSelectPlane(byte plane);
void modexClearRegion(page_t page, int x, int y, int w, int h, byte color);

/* -======================= Constants & Vars ==========================- */
extern byte far*  VGA;  /* The VGA Memory */
#define VIDEO_INT		0x10
#define SET_MODE		0x00
#define VGA_256_COLOR_MODE 	0x13
#define TEXT_MODE		0x03
#define SCREEN_WIDTH		320
#define SCREEN_HEIGHT		240
#define PAGE_SIZE		(word)(SCREEN_WIDTH/4 * SCREEN_HEIGHT)

#define SC_INDEX		0x03c4
#define SC_DATA			0x03c5
#define CRTC_INDEX		0x03d4
#define CRTC_DATA		0x03d5
#define MISC_OUTPUT		0x03c2
#define HIGH_ADDRESS 		0x0C
#define LOW_ADDRESS		0x0D
#define VRETRACE		0x08
#define INPUT_STATUS_1		0x03da
#define DISPLAY_ENABLE		0x01
#define MAP_MASK		0x02
#endif
