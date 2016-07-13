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
; egg@dstos3.dsto.gov.au
; teg@bart.dsto.gov.au
;
;  Terminology & notes:
;         VRAM ==   Video RAM
;         SRAM ==   System RAM
;         X coordinates are in pixels unless explicitly stated
;
;-----------------------------------------------------------------------*/

#ifndef _XPOINT_H_
#define _XPOINT_H_


/* FUNCTIONS =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif


 void x_put_pix(           /* Draw a pixel on the screen */
             WORD X,
             WORD Y,
             WORD PageOffset,
             WORD Color);

 WORD x_get_pix(          /* Read a pixel from the screen */
             WORD X,
             WORD Y,
	     WORD PageBase);


#ifdef __cplusplus
}
#endif


#endif


