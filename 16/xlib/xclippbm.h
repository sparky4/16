/*-----------------------------------------------------------------------
; MODULE XCLIPPBM
;
; This module was written by Matthew MacKenzie
; matm@eng.umd.edu
;
; Clipped transfer of planar bitmaps from system memory to video memory.
;
; Compile with TASM.
; C near-callable.
;
; ****** XLIB - Mode X graphics library                ****************
; ******                                               ****************
; ****** Written By Themie Gouthas                     ****************
;
;  Terminology & notes:
;         VRAM ==   Video RAM
;         SRAM ==   System RAM
;         X coordinates are in pixels unless explicitly stated
;
;----------------------------------------------------------------------*/

#ifndef _XCLIPPBM_H_
#define _XCLIPPBM_H_

/* unlike most global variables in Xlib, these are meant to be written to;
   in fact they start off uninitialized -- all values are in pixels */
extern int TopBound;
extern int BottomBound;
extern int LeftBound;
extern int RightBound;

#ifdef __cplusplus
extern "C" {
#endif


/* for both functions, a return value of 1 indicates that the entire
bitmap was outside the bounding box, while a value of 0 means that
something may have ended up on the screen */

/* copies a planar bitmap from SRAM to VRAM, with clipping */

 int x_clip_pbm (int X, int Y, int ScreenOffs, char far * Bitmap);

/* copies a planar bitmap from SRAM to VRAM, with clipping -- 0 bytes
   in the bitmap are not copied */

 int x_clip_masked_pbm (int X, int Y, int ScreenOffs, char far * Bitmap);

#ifdef __cplusplus
}
#endif


#endif
