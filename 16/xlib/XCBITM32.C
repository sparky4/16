#include "xbmtools.h"

/* these are here instead of the header file because they're not part
   of the library interface */

// rol al
#define ROL_AL          0xc0d0
// mov [si + disp8], imm8
#define SHORT_STORE_8   0x44c6
// mov [si + disp16], imm8
#define STORE_8         0x84c6
// mov [si + disp8], imm16
#define SHORT_STORE_16  0x44c7
// mov [si + disp16], imm16
#define STORE_16        0x84c7
// adc si, imm8
#define ADC_SI_IMMED    0xd683
// out dx, al
#define OUT_AL          0xee
// ret
#define RETURN          0xcb
// 32-bit prefix
#define DWORD_PREFIX    0x66

#define Emitb(x) *(output + output_used) = (x); output_used++

#define Emitw(x) *(output + output_used) = (x) & 255; \
				 *(output + output_used + 1) = (x) >> 8; \
				 output_used += 2


int x_compile_bitmap_32 (int logical_screen_width, char far * bitmap,
				char far * output)
{
	int pix0, pix1, pix2, pix3, numpix, pos;
	int column = 0, set_column = 0;
	int scanx = 0, scany = 0;
	int output_used = 0;

	int height = LBMHeight(bitmap);
	int width = LBMWidth(bitmap);
	int margin = width - 1;
	int margin2 = margin - 4;
	int margin4 = margin - 12;

	while (column < 4) {
		numpix = 1;
		pix0 = LBMGetPix(scanx, scany, bitmap);
		if (pix0 != 0) {
			if (set_column != column) {
				do {
					Emitw (ROL_AL);
					Emitw (ADC_SI_IMMED);
					Emitb (0);
					set_column++;
				} while (set_column != column);
				Emitb (OUT_AL);
			}
			if (scanx <= margin2) {
				pix1 = LBMGetPix(scanx + 4, scany, bitmap);
				if ((pix1 != 0)
				  &&(scanx <= margin4)) {
					numpix = 2;
					pix2 = LBMGetPix(scanx + 8, scany, bitmap);
					pix3 = LBMGetPix(scanx + 12, scany, bitmap);
					if ((pix2 != 0) && (pix3 != 0)) {
						numpix = 4;
						Emitb (DWORD_PREFIX);
					}
				}
			}
			pos = (scany * logical_screen_width) + (scanx >> 2) - 128;
			if ((pos >= -128) && (pos <= 127)) {
				if (numpix == 1) {
					Emitw (SHORT_STORE_8);
					Emitb (pos);
					Emitb (pix0);
				} else {
					Emitw (SHORT_STORE_16);
					Emitb (pos);
					Emitb (pix0);
					Emitb (pix1);
					if (numpix == 4) {
						Emitb (pix2);
						Emitb (pix3);
					}
				}
			} else {
				if (numpix == 1) {
					Emitw (STORE_8);
					Emitw (pos);
					Emitb (pix0);
				} else {
					Emitw (STORE_16);
					Emitw (pos);
					Emitb (pix0);
					Emitb (pix1);
					if (numpix == 4) {
						Emitb (pix2);
						Emitb (pix3);
					}
				}
			}
		}
		scanx += (numpix << 2);
		if (scanx > margin) {
			scanx = column;
			scany++;
			if (scany == height) {
				scany = 0;
				column++;
			}
		}
	}
	Emitb(RETURN);
	return(output_used);
}


int x_sizeof_cbitmap_32 (int logical_screen_width, char far * bitmap)
{
	int pix0, pix1, pix2, pix3, numpix, pos;
	int column = 0, set_column = 0;
	int scanx = 0, scany = 0;
	int output_used = 1;

	int height = LBMHeight(bitmap);
	int width = LBMWidth(bitmap);
	int margin = width - 1;
	int margin2 = margin - 4;
	int margin4 = margin - 12;

	while (column < 4) {
		numpix = 1;
		pix0 = LBMGetPix(scanx, scany, bitmap);
		if (pix0 != 0) {
			if (set_column != column) {
				do {
					output_used += 5;
					set_column++;
				} while (set_column != column);
				output_used++;
			}
			if (scanx <= margin2) {
				pix1 = LBMGetPix(scanx + 4, scany, bitmap);
				if ((pix1 != 0)
				  &&(scanx <= margin4)) {
					numpix = 2;
					pix2 = LBMGetPix(scanx + 8, scany, bitmap);
					pix3 = LBMGetPix(scanx + 12, scany, bitmap);
					if ((pix2 != 0) && (pix3 != 0)) {
						numpix = 4;
						output_used++;
					}
				}
			}
			pos = (scany * logical_screen_width) + (scanx >> 2) - 128;
			if ((pos >= -128) && (pos <= 127)) {
				if (numpix == 1) {
					output_used += 4;
				} else {
					output_used += 5;
					if (numpix == 4)
						output_used += 2;
				}
			} else {
				if (numpix == 1) {
					output_used += 5;
				} else {
					output_used += 6;
					if (numpix == 4)
						output_used += 2;
				}
			}
		}
		scanx += (numpix << 2);
		if (scanx > margin) {
			scanx = column;
			scany++;
			if (scany == height) {
				scany = 0;
				column++;
			}
		}
	}
	return(output_used);
}

