/*-----------------------------------------------------------------------
;
; XFILL - header file
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

#ifndef _XFILL_H_
#define _XFILL_H_


/* FUNCTIONS =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

int x_flood_fill(int x,
                 int y,
                 unsigned ofs,
                 int color
                 );

int x_boundary_fill(int x,
                    int y,
                    unsigned ofs,
                    int boundary,
                    int color
                 );


#ifdef __cplusplus
}
#endif

#endif


