#ifndef __16_VLPAL_H__
#define __16_VLPAL_H__

#include "src/lib/16_vl.h"

#define BEVERBOSEPALCHECK

#ifdef BEVERBOSEPALCHECK
#endif

typedef struct
{
	word width;
	word height;
	word offset;
} imgtestpal_t;

void VL_palette(imgtestpal_t *bmp, byte *p, word *i, word qp, word aqoffset);

#endif //__16_VLPAL_H__
