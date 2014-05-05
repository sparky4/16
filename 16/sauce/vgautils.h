#ifndef vgautils_h
#define vgautils_h

#define MODE13 0x13
#define TEXT 0x03
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200

#define COLOR_MASK 0x3c6
#define COLOR_REGISTER_RD 0x3c7
#define COLOR_REGISTER_WR 0x3c8
#define COLOR_DATA 0x3c9
#define VGACARD 0xA0000000
#define ROMCHRS 0xF000FA6E

#include<conio.h>
#include<stdlib.h>
#include<stdio.h>
#include<dos.h>
#include<math.h>

typedef struct rgbcolor_typ
   {
   unsigned char red;
   unsigned char green;
   unsigned char blue;
   }rgbcolor, *rgbcolor_ptr;

typedef struct rgb_palette_typ
   {
   int start_reg; //starting register to save
   int end_reg; // last register to save
   rgbcolor colors[256]; // palette storage
   }rgb_palette, *rgb_palette_ptr;

void setvgamode(int mode);
void write_rgbcolor_reg(int index, rgbcolor_ptr color);
rgbcolor_ptr read_rgbcolor_reg(int index, rgbcolor_ptr color);
void read_palette(int start_reg,int end_reg,rgb_palette_ptr pal);
void write_palette(int start_reg,int end_reg, rgb_palette_ptr pal);
void putchr(char out, int color,  unsigned char far* dest);

/***** syncvr() ***********************************
*  inline funtction that waits for verticle retrace
***************************************************/
#define syncvr() while(inp(0x3da) & 0x08);while(!(inp(0x3da) & 0x08));

#endif
