/* Project 16 Source Code~
 * Copyright (C) 2012-2022 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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

#include "src/lib/16text.h"

/* this array holds the rom font descriptors */
font_t romFonts[4];
fontdata_t romFontsData;

static void getRomFontAddr(char fontNum, int index) {
    word fontSeg=0;
    word fontOff=0;

    __asm {
	        PUSH AX
	        PUSH BX
		PUSH BP
		MOV AX, 0x1130		;I can haz font info plz?
		MOV BH, fontNum         ;  where ur fontNum
		INT 0x10		;kthxbae
		MOV AX, ES		;save teh segmentz
		MOV BX, BP		;and all the base!
		POP BP			;u can haz ur frame back!
		MOV fontSeg, AX		;Storage
		MOV fontOff, BX		;Storage
		POP BX
		POP AX
    }
    romFonts[index].seg = fontSeg;
    romFonts[index].off = fontOff;

}

/* This function initializes the text engine */
void textInit() {
    getRomFontAddr(0x02, ROM_FONT_8x14);
    getRomFontAddr(0x03, ROM_FONT_8x8_LOW);
    getRomFontAddr(0x04, ROM_FONT_8x8_HIGH);
    getRomFontAddr(0x06, ROM_FONT_8x16);
    romFonts[ROM_FONT_8x14].charSize=14;
    romFonts[ROM_FONT_8x8_LOW].charSize=8;
    romFonts[ROM_FONT_8x8_HIGH].charSize=8;
    romFonts[ROM_FONT_8x16].charSize=16;
	//romFontsData
}
