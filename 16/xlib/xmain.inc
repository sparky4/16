;-----------------------------------------------------------------------
;
; XMAIN - Include file
;
; XMAIN.ASM export functions
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


	 global _x_set_mode              :proc
	 global _x_page_flip             :proc
	 global _x_set_start_addr        :proc
	 global _x_set_splitscreen       :proc
	 global _x_text_mode             :proc
	 global _x_set_mode              :proc
         global _x_select_default_plane  :proc
	 global _x_hide_splitscreen      :proc
	 global _x_show_splitscreen      :proc
	 global _x_adjust_splitscreen    :proc
	 global _x_set_doublebuffer      :proc
	 global _x_set_tripplebuffer     :proc
	 global _x_set_cliprect          :proc
	 global _x_wait_vsync            :proc
