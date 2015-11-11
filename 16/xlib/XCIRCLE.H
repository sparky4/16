/*-----------------------------------------------------------------------
;
; XCIRCLE - header file
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

#ifndef _XCIRCLE_H_
#define _XCIRCLE_H_

#ifdef __cplusplus
extern "C" {
#endif


 void x_circle (WORD Left, WORD Top, WORD Diameter,
         WORD Color, WORD ScreenOffs);

 void x_filled_circle (WORD Left, WORD Top, WORD Diameter,
	 WORD Color, WORD ScreenOffs);


#ifdef __cplusplus
}
#endif


#endif

