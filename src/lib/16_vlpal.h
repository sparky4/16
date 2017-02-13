#ifndef __16_VLPAL_H__
#define __16_VLPAL_H__

#include "src/lib/16_vlpald.h"
#include "src/lib/16_vl.h"


typedef struct
{
	word width;
	word height;
	word offset;
} imgtestpal_t;

void VL_modexPalUpdate(bitmap_t *bmp, word *i, word qp, word aqpp);
void VL_palette(imgtestpal_t *bmp, byte *p, word *i, word qp, word aqoffset);
//void chkcolor(bitmap_t *bmp, word *q, word *a, word *aa, word *z);


#endif //__16_VLPAL_H__
