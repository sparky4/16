/*
 * This is the text subsystem for project 16.
 * These functions provide for the rendering of text strings
 * using the BIOS fonts.
 * They copy the font characters directly to VGA.
 */
#ifndef TEXT_H
#define TEXT_H
#include <jstring.h>
#include <jctype.h>
#include "src/lib/types.h"

/* font information type */
typedef struct font {
    word  seg;       //segment for the font
    word  off;       //offset in the segment for the font
    byte  charSize;  //bytes in each character
} font_t;

/* storage for the rom fonts */
extern font_t romFonts[4];
#define ROM_FONT_8x14     0
#define ROM_FONT_8x8_LOW  1
#define ROM_FONT_8x8_HIGH 2
#define ROM_FONT_8x16     3

/* This function initializes the text engine */
void textInit();

#endif
