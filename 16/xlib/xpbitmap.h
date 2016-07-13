/*-----------------------------------------------------------------------
;
; XPBITMAP - header file
;
;
;
; ****** XLIB - Mode X graphics library                ****************
; ******                                               ****************
; ****** Written By Themie Gouthas                     ****************
; ****** Aeronautical Research Laboratory              ****************
; ****** Defence Science and Technology Organisation   ****************
; ****** Australia                                     ****************
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

#ifndef _XPBITMAP_H_
#define _XPBITMAP_H_


/* FUNCTIONS =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif


 void x_put_masked_pbm(  /* Copy a planar bitmap from SRAM masking */
              WORD X,    /* only non zero pixels to VRAM           */
              WORD Y,
              WORD ScrnOffs,
              BYTE far * Bitmap);

 void x_flip_masked_pbm(  /* Copy a planar bitmap from SRAM masking */
              WORD X,     /* only non zero pixels to VRAM. Bitmap   */
              WORD Y,     /* is mirrored.                           */
              WORD ScrnOffs,
              BYTE far * Bitmap,
              WORD orientation);

 void x_put_pbm(         /* Copy a planar bitmap from SRAM to VRAM */
              WORD X,
              WORD Y,
              WORD ScrnOffs,
              BYTE far * Bitmap);

 void x_flip_pbm(         /* Copy a planar bitmap from SRAM to VRAM */
              WORD X,
              WORD Y,
              WORD ScrnOffs,
              BYTE far * Bitmap,
              WORD orientation);

 void x_get_pbm(         /* Copy a planar bitmap from VRAM to SRAM */
              WORD X,
              WORD Y,
              BYTE Bw,
              BYTE Bh,
              WORD ScrnOffs,
              BYTE far * Bitmap);


#ifdef __cplusplus
}
#endif


#endif


