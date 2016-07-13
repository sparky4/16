/*-----------------------------------------------------------------------
;
; XVBITMAP - header file
;
;
;
; ****** XLIB - Mode X graphics library                ****************
; ******                                               ****************
; ****** Written By Themie Gouthas                     ****************
;
; egg@dstos3.dsto.gov.au
; teg@bart.dsto.gov.au
;
;  Terminology & notes:
;         VRAM ==   Video RAM
;         SRAM ==   System RAM
;         X coordinates are in pixels unless explicitly stated
;
;-----------------------------------------------------------------------*/

#ifndef _XVBITMAP_H_
#define _XVBITMAP_H_


/* FUNCTIONS =========================================================== */


#ifdef __cplusplus
extern "C" {
#endif



void x_put_masked_vbm(  /* Copy a planar bitmap from SRAM masking */
	int X,          /* only non zero pixels to VRAM           */
	int Y,
        WORD ScrnOffs,
	BYTE far * VBitmap);

int x_put_masked_vbm_clipx(/* Copy a planar bitmap from SRAM masking */
	int X,          /* only non zero pixels to VRAM. Clip X     */
	int Y,
	WORD ScrnOffs,
	BYTE far * VBitmap);

int x_put_masked_vbm_clipy(/* Copy a planar bitmap from SRAM masking */
	int X,          /* only non zero pixels to VRAM. Clip Y    */
	int Y,
	WORD ScrnOffs,
	BYTE far * VBitmap);

int x_put_masked_vbm_clipxy(/* Copy a planar bitmap from SRAM masking */
	int X,          /* only non zero pixels to VRAM. Clip X & Y  */
	int Y,
	WORD ScrnOffs,
	BYTE far * VBitmap);

char far * x_make_vbm(
	char far *lbm,
	WORD *VramStart);

#ifdef __cplusplus
}
#endif


#endif


