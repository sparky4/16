/*-----------------------------------------------------------------------
;
; XCBITM32 - header file
;
;   This module was written by Matthew MacKenzie
;   matm@eng.umd.edu
;
;
; ****** XLIB - Mode X graphics library                ****************
; ******                                               ****************
; ****** Written By Themie Gouthas                     ****************
;
; egg@dstos3.dsto.gov.au
; teg@bart.dsto.gov.au
;
;----------------------------------------------------------------------*/
/* Notice that there is no function in this module to plot a
   compiled bitmap.  Use x_put_cbitmap in module XCBITMAP. */

/* This module is only for 386+ class computers. */

#ifndef _XCBITM32_H_
#define _XCBITM32_H_

extern int x_compile_bitmap_32 (      /* Compile a linear bitmap to generate  */
			  WORD logical_screen_width, /* machine code to plot it at any */
			  char far * bitmap,      /* required screen coordinates FAST. */
			  char far * output);

extern int x_sizeof_cbitmap_32 (        /* Find the size of the code which a  */
			  WORD logical_screen_width, /* bitmap would result in, if it  */
			  char far * bitmap);  /* were compiled (used for allocation). */

#endif
