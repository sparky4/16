;-----------------------------------------------------------------------
;
; XLIB - Include file
;
; Global equates and variables
;
;
; ****** XLIB - Mode X graphics library                ****************
; ******                                               ****************
; ****** Written By Themie Gouthas                     ****************
;
; egg@dstos3.dsto.gov.au
; teg@bart.dsto.gov.au
;
; MODIFICATIONS:
;  26-9-92 :       Pel panning code added
;  23-10-92:       Added clip rectangle code
;-----------------------------------------------------------------------

;

;.global
;.8086

; First lets find out what memory model to use
;.model huge
;include model.inc


AC_INDEX        equ  03c0h   ;Attribute controller index register
MISC_OUTPUT     equ  03c2h   ;Miscellaneous Output register
SC_INDEX        equ  03c4h   ;Sequence Controller Index
GC_INDEX        equ  03ceh   ; Graphics controller Index
CRTC_INDEX      equ  03d4h   ;CRT Controller Index
DAC_READ_INDEX  equ  03c7h   ;
DAC_WRITE_INDEX equ  03c8h   ;
DAC_DATA        equ  03c9h   ;
INPUT_STATUS_0 equ   03dah   ;Input status 0 register


SCREEN_SEG    equ    0a000h  ;segment of display memory in mode X

MAP_MASK      equ    02h     ;index in SC of Map Mask register
READ_MAP      equ    04h     ;index in GC of the Read Map register
BIT_MASK      equ    08h     ;index in GC of Bit Mask register

OVERFLOW      equ    07h     ; CRTC overflow register index
MAX_SCAN_LINE equ    09h     ; CRTC maximum scan line register index
ADDR_HIGH     equ    0ch     ;Index of Start Address High reg in CRTC
ADDR_LOW      equ    0dh     ;                       Low
CRTC_OFFSET   equ    13h     ; CRTC offset register index
UNDERLINE     equ    14h     ; CRTC underline location register index
MODE_CONTROL  equ    17h     ; CRTC mode control register index
LINE_COMPARE  equ    18h     ; CRTC line compare reg. index (bits 0-7 of
			     ; split screen scan line

AC_MODE_CONTROL equ    10h   ; Index of Mode COntrol register in AC
PEL_PANNING     equ    13h   ; Pel panning register index in AC

PATTERN_BUFFER equ 0fffch    ;offset in screen memory of pattern buffer

TRUE    equ 1
FALSE   equ 0


OK             equ 0
ERROR          equ 1


;-----------------------------------------------------------------------
; Macro to wait for the vertical retrace leading edge

WaitVsyncStart   macro
	LOCAL WaitNotVsync,WaitVsync
	mov     dx,INPUT_STATUS_0
WaitNotVsync:
	in      al,dx
	test    al,08h
	jnz     WaitNotVsync
WaitVsync:
	in      al,dx
	test    al,08h
	jz      WaitVsync
	endm

;-----------------------------------------------------------------------
; Macro to wait for the vertical retrace trailing edge

WaitVsyncEnd    macro
	LOCAL WaitNotVsync,WaitVsync
	mov     dx,INPUT_STATUS_0
WaitVsync2:
	in      al,dx
	test    al,08h
	jz      WaitVsync2
WaitNotVsync2:
	in      al,dx
	test    al,08h
	jnz     WaitNotVsync2
	endm

;--- Word out macro ------------------------------------------

     WORDOUT  macro
     IFDEF nw
       out  dx,al
       inc  dx
       xchg al,ah
       out  dx,al
       xchg al,ah
       dec  dx
     ELSE
       out  dx,al
     ENDIF
     endm

;------------------------------------------------------------------------
; Global variables - XMAIN exports
;
	global _InGraphics              :byte
	global _CurrXMode               :word
	global _ScrnPhysicalByteWidth   :word
	global _ScrnPhysicalPixelWidth  :word
	global _ScrnPhysicalHeight      :word
	global _ErrorValue              :byte

	global _SplitScrnOffs           :word
	global _SplitScrnScanLine       :word
	global _SplitScrnVisibleHeight  :word
	global _Page0_Offs              :word
	global _Page1_Offs              :word
	global _Page2_Offs              :word
	global _ScrnLogicalByteWidth    :word
	global _ScrnLogicalPixelWidth   :word
	global _ScrnLogicalHeight       :word

	global _MaxScrollX              :word
	global _MaxScrollY              :word
	global _DoubleBufferActive      :word
	global _TrippleBufferActive     :word
	global _VisiblePageIdx          :word
	global _VisiblePageOffs         :word
	global _HiddenPageOffs          :word
	global _WaitingPageOffs         :word
	global _NonVisual_Offs          :word
	global _TopClip                 :word
	global _BottomClip              :word
	global _LeftClip                :word
	global _RightClip               :word

        global _PhysicalStartByteX      :word
	global _PhysicalStartPixelX     :word
	global _PhysicalStartY          :word

	global _VsyncHandlerActive      :word
	global _MouseRefreshFlag        :word
	global _MouseVsyncHandler       :dword
	global _StartAddressFlag        :word
	global _WaitingStartLow         :word
	global _WaitingStartHigh        :word
	global _WaitingPelPan           :word
	global _VsyncPaletteStart       :word
	global _VsyncPaletteCount       :word
	global _VsyncPaletteBuffer      :byte
