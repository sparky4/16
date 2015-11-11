/*-----------------------------------------------------------------------
;
; XRECT - header file
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

#ifndef _XRECT_H_
#define _XRECT_H_


/* FUNCTIONS =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

 void x_rect_pattern(      /* draw a pattern filled rectangle      */
         WORD StartX,
         WORD StartY,
         WORD EndX,
         WORD EndY,
         WORD PageBase,
	 BYTE far * Pattern);

 void x_rect_pattern_clipped(  /* draw a pattern filled clipped   */
	 WORD StartX,          /* rectangle                       */
         WORD StartY,
         WORD EndX,
         WORD EndY,
         WORD PageBase,
	 BYTE far * Pattern);

 void x_rect_fill(         /* draw a single colour filled rectangle */
         WORD StartX,
         WORD StartY,
         WORD EndX,
         WORD EndY,
         WORD PageBase,
	 WORD color);

 void x_rect_fill_clipped(   /* draw a single colour filled */
	 WORD StartX,        /* and clipped rectangle       */
         WORD StartY,
         WORD EndX,
         WORD EndY,
         WORD PageBase,
	 WORD color);

 void x_cp_vid_rect(       /* Copy rect region within VRAM          */
         WORD SourceStartX,
         WORD SourceStartY,
         WORD SourceEndX,
         WORD SourceEndY,
         WORD DestStartX,
         WORD DestStartY,
         WORD SourcePageBase,
         WORD DestPageBase,
         WORD SourceBitmapWidth,
         WORD DestBitmapWidth);

/* Copy a rectangular region of a VGA screen, with x coordinates
 rounded to the nearest byte -- source and destination may overlap. */

 void x_shift_rect (WORD SrcLeft, WORD SrcTop,
			  WORD SrcRight, WORD SrcBottom,
			  WORD DestLeft, WORD DestTop, WORD ScreenOffs);

#ifdef __cplusplus
}
#endif


#endif


