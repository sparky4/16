/*;IFDEF s
;  DISPLAY "XLIB04 Small Model"
;  .model small
;ELSE
;  IFDEF c
;    DISPLAY "XLIB04 Compact Model"
;    .model compact
;  ELSE
;     IFDEF l
;       DISPLAY "XLIB04 Large Model"
;      .model large
;     ELSE
;       DISPLAY "WARNING: Model was not defined at the command line."
;       DISPLAY "         Using default small model ie /ds         "
;       DISPLAY "         Include in TASM commandline either /ds, /dc or /dl"
;;       .model small
;     ENDIF
;  ENDIF
;ENDIF
*/
