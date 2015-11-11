/*-----------------------------------------------------------------------
;
; XPOLYGON - header file
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

#ifndef _XPOLYGON_H_
#define _XPOLYGON_H_


typedef struct {
  int X;
  int Y;
} far VERTEX;

/* FUNCTIONS =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

void x_triangle(     /* Draw a triangle */
         int x0,
         int y0,
         int x1,
         int y1,
         int x2,
         int y2,
         WORD color,
         WORD PageBase);

void x_polygon(     /* Draw a convex polygon */
         VERTEX *vertices,
         int  num_vertices,
         WORD color,
         WORD PageBase);


#ifdef __cplusplus
}
#endif

#endif


