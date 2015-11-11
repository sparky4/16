/*-----------------------------------------------------------------------
;
; XCBITMAP - header file
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

#ifndef _XCBITMAP_H_
#define _XCBITMAP_H_

#ifdef __cplusplus
extern "C" {
#endif



 int x_compile_bitmap (        /* Compile a linear bitmap to generate  */
    WORD logical_screen_width, /* machine code to plot it at any */
    char far * bitmap,         /* required screen coordinates FAST. */
    char far * output);

 int x_sizeof_cbitmap (        /* Find the size of the code which a  */
    WORD logical_screen_width, /* bitmap would result in, if it  */
    char far * bitmap);        /* were compiled (used for allocation). */

 void x_put_cbitmap (    /* Displays a compiled bitmap generated */
    WORD XPos,           /* by x_compile_bitmap.                 */
    WORD YPos,
    WORD PageOffset,
    char far * CompiledBitmap);

#ifdef __cplusplus
}
#endif


#endif
