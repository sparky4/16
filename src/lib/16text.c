#include "16text.h"

/* this array holds the rom font descriptors */
font_t romFonts[4];


static void getRomFontAddr(char fontNum, int index) {
    word fontSeg;
    word fontOff;

    __asm {
		PUSH BP
		MOV AX, 0x1130		;I can haz font info plz?
		MOV BH, fontNum         ;  where ur fontNum
		INT 0x10		;kthxbae
		MOV AX, ES		;save teh segmentz
		MOV BX, BP		;and all the base!
		POP BP			;u can haz ur frame back!
		MOV fontSeg, AX		;Storage
		MOV fontOff, BX		;Storage
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
}
