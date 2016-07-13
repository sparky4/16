;-----------------------------------------------------------------------
;
; XPBITMAP - Include file
;
; This module was written by Matthew MacKenzie
; matm@eng.umd.edu
;
; XPBITMAP.ASM export functions
;
; ****** XLIB - Mode X graphics library                ****************
; ******                                               ****************
; ****** Written By Themie Gouthas                     ****************
;
; egg@dstos3.dsto.gov.au
; teg@bart.dsto.gov.au
;-----------------------------------------------------------------------

	global _x_clip_masked_pbm     :proc
	global _x_clip_pbm            :proc

	global _LeftBound             :word
	global _RightBound            :word
	global _TopBound              :word
	global _BottomBound           :word

