/*-----------------------------------------------------------------------
;
; XPAL - header file
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

#ifndef _XPAL_H_
#define _XPAL_H_

#define PAL_ROTATE_DOWN 0
#define PAL_ROTATE_UP   1

/* FUNCTIONS =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif


 void x_get_pal_raw(       /* Read DAC palette into raw buffer       */
              BYTE far * pal,
              WORD num_colrs,
              WORD start_index);

 void x_get_pal_struc(     /* Read DAC palette into annotated buffer */
              BYTE far * pal,
              WORD num_colrs,
              WORD start_index);

 void x_put_pal_raw(       /* Write DAC palette from raw buffer      */
              BYTE far * pal,
              WORD num_colrs,
              WORD start_index);

 void x_put_pal_struc(     /* Write DAC palette from annotated buffer*/
              BYTE far * pal);

 x_set_rgb(                /* Set the RGB components of a color index*/
              BYTE color,
              BYTE red_c,
              BYTE green_c,
              BYTE blue_c);

 x_rot_pal_raw(           /* Rotate a raw palette buffer             */
              BYTE far * pal,
              WORD direction,
              WORD num_colrs);

 void x_rot_pal_struc(    /* Rotate an anottated palette buffer      */
              BYTE far * pal,
              WORD direction);

 WORD x_cpcontrast_pal_struc(     /* Copy and contrast adjust annotated  */
              BYTE far *src_pal,  /*  palette buffer                     */
              BYTE far *dest_pal,
              BYTE Intensity);

 void x_transpose_pal_struc(  /* Write DAC palette from annotated type*/
              BYTE far * pal, /* buffer with a new offset             */
              WORD StartColor);

 void x_put_contrast_pal_struc( /* Write DAC palette from annotated */
              BYTE far * pal,   /* type buffer with specified intensity  */
              BYTE  intensity);

#ifdef __cplusplus
}
#endif


#endif


