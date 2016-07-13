/*-----------------------------------------------------------------------
;
; XPBMCLIP - header file
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

#ifndef _XPBMCLIP_H_
#define _XPBMCLIP_H_


/* FUNCTIONS =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif


 int x_put_masked_pbm_clipx(  /* Copy a planar bitmap from SRAM masking */
	      int X,          /* only non zero pixels to VRAM           */
	      int Y,          /* Supports clipping in the X direction   */
	      WORD ScrnOffs,
	      BYTE far * Bitmap);


 int x_put_masked_pbm_clipy(  /* Copy a planar bitmap from SRAM masking */
	      int X,          /* only non zero pixels to VRAM           */
	      int Y,          /* Supports clipping in the Y direction   */
	      WORD ScrnOffs,
	      BYTE far * Bitmap);

 int x_put_masked_pbm_clipxy( /* Copy a planar bitmap from SRAM masking */
	      int X,          /* only non zero pixels to VRAM           */
	      int Y,          /* Supports clipping in the Y direction   */
	      WORD ScrnOffs,
	      BYTE far * Bitmap);

 int x_put_pbm_clipx(         /* Copy a planar bitmap from SRAM masking */
	      int X,          /* Supports clipping in the x direction   */
	      int Y,
	      WORD ScrnOffs,
	      BYTE far * Bitmap);

 int x_put_pbm_clipy(   /* Copy a planar bitmap from SRAM masking */
	      int X,    /* Supports clipping in the Y direction   */
	      int Y,
	      WORD ScrnOffs,
	      BYTE far * Bitmap);

 int x_put_pbm_clipx(   /* Copy a planar bitmap from SRAM masking */
	      int X,    /* Supports clipping in the X direction   */
	      int Y,
	      WORD ScrnOffs,
	      BYTE far * Bitmap);

 int x_put_pbm_clipxy(   /* Copy a planar bitmap from SRAM masking */
	      int X,     /* Supports clipping in the X&Y directions */
	      int Y,
	      WORD ScrnOffs,
	      BYTE far * Bitmap);


#ifdef __cplusplus
}
#endif


#endif


