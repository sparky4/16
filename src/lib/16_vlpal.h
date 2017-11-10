#ifndef __16_VLPAL_H__
#define __16_VLPAL_H__

#include "src/lib/16_vl.h"
#include "src/lib/16_vlpld.h"
#include "16/modex16/bitmap.h"


typedef struct
{
	word width;
	word height;
	word offset;
} imgtestpal_t;

void VL_modexPalUpdate(bitmap_t *bmp, word *i, word qp, word aqpp, global_game_variables_t *gv);
void VL_palette(imgtestpal_t *bmp, byte *p, word *i, word qp, word aqoffset, word *apal, global_game_variables_t *gv);


#endif //__16_VLPAL_H__
