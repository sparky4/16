/*-----------------------------------------------------------------------
;
; XPOINT - header file
;
;
;
; ****** XLIB - Mode X graphics library                ****************
; ******                                               ****************
; ****** Written By Themie Gouthas                     ****************
;
; Macros written by Mathew Mackenzie
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

#ifndef _XBMTOOLS_H_
#define _XBMTOOLS_H_

#define BM_WIDTH_ERROR 1

#define LBMHeight(lbitmap) lbitmap[1]
#define LBMWidth(lbitmap) lbitmap[0]
#define PBMHeight(pbitmap) pbitmap[1]
#define PBMWidth(pbitmap) (pbitmap[0]<<2)
#define PBMByteWidth(pbitmap) pbitmap[0]

#define LBMPutPix(x,y,lbitmap,color)  \
	lbitmap[2 + (x) + (y) * LBMWidth(lbitmap)] = color

#define LBMGetPix(x,y,lbitmap)  \
	(lbitmap[2 + (x) + (y) * LBMWidth(lbitmap)])



/* FUNCTIONS =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

 int x_pbm_to_bm(           /* Convert from planar bitmap to linear */
   char far * source_pbm,
   char far * dest_bm);

 int x_bm_to_pbm(           /* Convert from linear bitmap to planar */
   char far * source_pbm,
   char far * dest_bm);

#ifdef __cplusplus
}
#endif


#endif


