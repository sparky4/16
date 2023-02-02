/* Project 16 Source Code~
 * Copyright (C) 2012-2023 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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
 * This is the text subsystem for project 16.
 * These functions provide for the rendering of text strings
 * using the BIOS fonts.
 * They copy the font characters directly to VGA.
 */
#ifndef TEXT_H
#define TEXT_H
#ifdef __WATCOMC__
#include <jstring.h>
#include <jctype.h>
#endif
#include "src/lib/16_head.h"

/* font information type */
typedef struct font {
    word  seg;       //segment for the font
    word  off;       //offset in the segment for the font
    byte  charSize;  //bytes in each character
} font_t;

typedef struct fontdata {
	byte z[9];
	byte col;	//color
	byte bgcol;		//bg color!
	word chw;		//char width
	byte l[1024];
} fontdata_t;

/* storage for the rom fonts */
extern font_t romFonts[4];
extern fontdata_t romFontsData;
#define ROM_FONT_8x14     0
#define ROM_FONT_8x8_LOW  1
#define ROM_FONT_8x8_HIGH 2
#define ROM_FONT_8x16     3

/* This function initializes the text engine */
void textInit();

#endif
