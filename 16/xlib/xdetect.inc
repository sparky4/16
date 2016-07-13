;-----------------------------------------------------------------------
;
; XDETECT - Include file
;
; XDETECT.ASM export functions
;
;
;
; ****** XLIB - Mode X graphics library                ****************
; ******                                               ****************
; ****** Written By Themie Gouthas                     ****************
;
; egg@dstos3.dsto.gov.au
; teg@bart.dsto.gov.au
;-----------------------------------------------------------------------
	 global _MouseButtonCount        :word
	 global _MouseVersion            :word
	 global _MouseType               :byte
	 global _MouseIRQ                :byte


	 global _x_graphics_card        :proc
	 global _x_processor            :proc
	 global _x_coprocessor          :proc
	 global _x_mousedriver          :proc

