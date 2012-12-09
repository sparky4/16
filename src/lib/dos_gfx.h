#ifndef _DOSLIB_GFX_H_
#define _DOSLIB_GFX_H_
#include <iostream.h>
#include <stdlib.h>
#include "src\lib\lib_com.h"

#define SW		320 // width in pixels of vga mode
#define SH		200 // height in pixels of vga mode
#define NUM_COLORS	256 // number of colors in vga mode
//static lgq=NUM_COLORS/(1/8)
//static hgq=NUM_COLORS/(1/8)
/*#define SW		320
#define SH		200
#define NUM_COLORS	4*/ // cga
#define BONK		400
#define LGQ		32
#define HGQ		56
#define VMEM		0xA000 // = vga
//#define VMEM		0xB800 // = cga
//#define VMOD		0x13

void setvideo(byte mode, int vq);
void setvbuff(int vq);
void updatevbuff();
void cls(byte color, byte *Where);
void clearscr();
void plotpixel(int x, int y, byte color, byte *Where);
void plotpixelfast(int x, int y, byte color, byte *Where);
void BlockMove();
void etesuto(int x, int y);
void qtesuto(int x, int y, int color);
int colortest();
int colorz();
void ssd(int svq);
int ding(int q);

#endif/*_DOSLIB_GFX_H_*/
