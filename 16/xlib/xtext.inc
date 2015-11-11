;-----------------------------------------------------------------------
;
; XPOINT - Include file
;
; XPOINT.ASM export functions
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


	 global _x_text_init             :proc
	 global _x_set_font              :proc
	 global _x_char_put              :proc
	 global _x_register_userfont     :proc
	 global _x_get_char_width        :proc

	 global _CharHeight              :byte
         global _CharWidth               :byte
         global _FontPtr                 :word:2
         global _FirstChar               :byte
	 global _UserFontPtr             :word:2
	 global _UserChHeight            :byte
	 global _UserChWidth             :byte
	 global _UserFirstCh             :byte
