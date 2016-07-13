;-----------------------------------------------------------------------
;
; XPAL - Include file
;
; XPAL.ASM export functions
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

         global _x_get_pal_struc	 :proc
	 global _x_get_pal_raw	         :proc

         global _x_put_pal_struc         :proc
	 global _x_put_pal_raw           :proc
	 global _x_set_rgb               :proc

         global _x_rot_pal_struc         :proc
	 global _x_rot_pal_raw           :proc


	 global _x_put_contrast_pal_struc:proc

	 global _x_transpose_pal_struc   :proc

	 global _x_cpcontrast_pal_struc	 :proc