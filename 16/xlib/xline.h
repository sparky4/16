/*-----------------------------------------------------------------------
;
; XLINE - header file
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
;----------------------------------------------------------------------*/

#ifndef _XLINE_H_
#define _XLINE_H_


/* FUNCTIONS =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

extern void x_line(     /* Draw a line, what else */
         WORD x0,
         WORD y0,
         WORD x1,
         WORD y1,
         WORD color,
         WORD PageBase);


#ifdef __cplusplus
}
#endif

#endif


