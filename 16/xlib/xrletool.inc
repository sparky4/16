;-----------------------------------------------------------------------
;
; XRLETOOL - Include file
;
; XRLETOOL.ASM export functions
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


	 global _x_buff_RLEncode        :proc
         global _x_buff_RLE_size        :proc
	 global _x_buff_RLDecode        :proc
	 global _x_file_RLEncode        :proc
	 global _x_file_RLDecode        :proc

         global _RLE_last_buff_offs     :word
	 
