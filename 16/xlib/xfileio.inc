;-----------------------------------------------------------------------
;
; XFILEIO - Include file
;
; XFILEIO.ASM export functions
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

	    global _f_open            :proc
	    global _f_close           :proc
            global _f_read            :proc
            global _f_write           :proc
            global _f_readfar         :proc
            global _f_writefar        :proc
	    global _f_seek            :proc
	    global _f_tell            :proc
	    global _f_filelength      :proc

	    global _file_err          :word