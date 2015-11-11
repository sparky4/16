;-----------------------------------------------------------------------
; MODULE XMAIN
;
; Initialization, panning and split screen functions for all MODE X 256
; Color resolutions
;
; Compile with Tasm.
; C callable.
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
;  26-9-92:          Pel panning code added
;  Dates forgotten:  Numerous ;^)
;  05-10-93:         Timer synchronized vsync handling extensions
;                      and tripple buffering - Tore Jahn Bastiansen
;                      (toreba@ifi.uio.no) for the
;-----------------------------------------------------------------------


include xlib.inc
include xmain.inc


	.data


; Mode X CRTC register tweaks for various resolutions


LABEL X256Y200 word
		db      0e3h    ; dot clock
		db      8       ; Number of CRTC Registers to update
	dw      05f00h  ; horz total
	dw      03f01h  ; horz displayed
	dw      04202h  ; start horz blanking
	dw      09f03h  ; end horz blanking
	dw      04c04h  ; start h sync
	dw      00005h  ; end h sync
	dw      00014h  ; turn off dword mode
	dw      0e317h  ; turn on byte mode
	dw      256
	dw      200


LABEL X256Y240 word
	db      0e3h    ; dot clock
	db      16      ; Number of CRTC Registers to update
	dw      05f00h  ; horz total
	dw      03f01h  ; horz displayed
	dw      04202h  ; start horz blanking
	dw      09f03h  ; end horz blanking
	dw      04c04h  ; start h sync
	dw      00005h  ; end h sync
	dw      00d06h  ; vertical total
	dw      03e07h  ; overflow (bit 8 of vertical counts)
	dw      04109h  ; cell height (2 to double-scan)
	dw      0ea10h  ; v sync start
	dw      0ac11h  ; v sync end and protect cr0-cr7
	dw      0df12h  ; vertical displayed
	dw      00014h  ; turn off dword mode
	dw      0e715h  ; v blank start
	dw      00616h  ; v blank end
	dw      0e317h  ; turn on byte mode
	dw      256
		dw      240


X320Y200 label  word
	db      00      ; 0e3h    ; dot clock
	db      02      ; Number of CRTC Registers to update
	dw      00014h  ; turn off dword mode
	dw      0e317h  ; turn on byte mode
	dw      320     ; width
	dw      200     ; height

X320Y240 label  word
	db      0e3h    ; dot clock
	db      10      ; Number of CRTC Registers to update
	dw      00d06h  ; vertical total
	dw      03e07h  ; overflow (bit 8 of vertical counts)
	dw      04109h  ; cell height (2 to double-scan)
	dw      0ea10h  ; v sync start
	dw      0ac11h  ; v sync end and protect cr0-cr7
	dw      0df12h  ; vertical displayed
	dw      00014h  ; turn off dword mode
	dw      0e715h  ; v blank start
	dw      00616h  ; v blank end
	dw      0e317h  ; turn on byte mode
	dw      320     ; width
	dw      240     ; height

X360Y200 label  word
	db      0e7h    ; dot clock
	db      08      ; Number of CRTC Registers to update
	dw      06b00h  ; horz total
	dw      05901h  ; horz displayed
	dw      05a02h  ; start horz blanking
	dw      08e03h  ; end horz blanking
	dw      05e04h  ; start h sync
	dw      08a05h  ; end h sync
	dw      00014h  ; turn off dword mode
	dw      0e317h  ; turn on byte mode
	dw      360     ; width
	dw      200     ; height

X360Y240  label word
	db      0e7h    ; dot clock
	db      17      ; Number of CRTC Registers to update
	dw      06b00h  ; horz total
	dw      05901h  ; horz displayed
	dw      05a02h  ; start horz blanking
	dw      08e03h  ; end horz blanking
	dw      05e04h  ; start h sync
	dw      08a05h  ; end h sync
	dw      00d06h  ; vertical total
	dw      03e07h  ; overflow (bit 8 of vertical counts)
	dw      04109h  ; cell height (2 to double-scan)
	dw      0ea10h  ; v sync start
	dw      0ac11h  ; v sync end and protect cr0-cr7
	dw      0df12h  ; vertical displayed
	dw      02d13h  ; offset;
	dw      00014h  ; turn off dword mode
	dw      0e715h  ; v blank start
	dw      00616h  ; v blank end
	dw      0e317h  ; turn on byte mode
	dw      360
	dw      240

X376Y282 label word
	db      0e7h
	db      18
	dw      06e00h  ; horz total
	dw      05d01h  ; horz displayed
	dw      05e02h  ; start horz blanking
	dw      09103h  ; end horz blanking
	dw      06204h  ; start h sync
	dw      08f05h  ; end h sync
	dw      06206h  ; vertical total
	dw      0f007h  ; overflow
	dw      06109h  ; cell height
	dw      0310fh  ;
	dw      03710h  ; v sync start
	dw      08911h  ; v sync end and protect cr0-cr7
	dw      03312h  ; vertical displayed
	dw      02f13h  ; offset
	dw      00014h  ; turn off dword mode
	dw      03c15h  ; v blank start
	dw      05c16h  ; v blank end
	dw      0e317h  ; turn on byte mode
	dw      376
	dw      282

LABEL X256Y400 word
	db      0e3h    ; dot clock
	db      8       ; Number of CRTC Registers to update
	dw      05f00h  ; horz total
	dw      03f01h  ; horz displayed
	dw      04202h  ; start horz blanking
	dw      09f03h  ; end horz blanking
	dw      04c04h  ; start h sync
	dw      00005h  ; end h sync
	dw      04009h  ; cell height
	dw      00014h  ; turn off dword mode
	dw      0e317h  ; turn on byte mode
	dw      256
	dw      400


LABEL X256Y480 word
	db      0e3h    ; dot clock
	db      16      ; Number of CRTC Registers to update
		dw      05f00h  ; horz total
		dw      03f01h  ; horz displayed
	dw      04202h  ; start horz blanking
	dw      09f03h  ; end horz blanking
	dw      04c04h  ; start h sync
	dw      00005h  ; end h sync
	dw      00d06h  ; vertical total
	dw      03e07h  ; overflow (bit 8 of vertical counts)
	dw      04009h  ; cell height (2 to double-scan)
	dw      0ea10h  ; v sync start
	dw      0ac11h  ; v sync end and protect cr0-cr7
	dw      0df12h  ; vertical displayed
	dw      00014h  ; turn off dword mode
	dw      0e715h  ; v blank start
	dw      00616h  ; v blank end
	dw      0e317h  ; turn on byte mode
	dw      256
	dw      480



X320Y400 label  word
	db      0e3h    ; dot clock
	db      03      ; Number of CRTC Registers to update
	dw      04009h  ; cell height
	dw      00014h  ; turn off dword mode
	dw      0e317h  ; turn on byte mode
	dw      320     ; width
	dw      400     ; height

X320Y480 label  word
	db      0e3h    ; dotclock
	db      10      ; Number of CRTC Registers to update
	dw      00d06h  ; vertical total
	dw      03e07h  ; overflow (bit 8 of vertical counts)
	dw      04009h  ; cell height (2 to double-scan)
	dw      0ea10h  ; v sync start
	dw      0ac11h  ; v sync end and protect cr0-cr7
	dw      0df12h  ; vertical displayed
	dw      00014h  ; turn off dword mode
	dw      0e715h  ; v blank start
	dw      00616h  ; v blank end
	dw      0e317h  ; turn on byte mode
	dw      320     ; width
	dw      480     ; height

X360Y400 label  word
	db      0e7h    ; dot clock
	db      09      ; Number of CRTC Registers to update
	dw      06b00h  ; horz total
	dw      05901h  ; horz displayed
	dw      05a02h  ; start horz blanking
	dw      08e03h  ; end horz blanking
	dw      05e04h  ; start h sync
	dw      08a05h  ; end h sync
	dw      04009h  ; cell height
	dw      00014h  ; turn off dword mode
	dw      0e317h  ; turn on byte mode
	dw      360     ; width
	dw      400     ; height



X360Y480  label word
	db      0e7h
	db      17
	dw      06b00h  ; horz total
	dw      05901h  ; horz displayed
	dw      05a02h  ; start horz blanking
	dw      08e03h  ; end horz blanking
	dw      05e04h  ; start h sync
	dw      08a05h  ; end h sync
	dw      00d06h  ; vertical total
	dw      03e07h  ; overflow
	dw      04009h  ; cell height
	dw      0ea10h  ; v sync start
	dw      0ac11h  ; v sync end and protect cr0-cr7
	dw      0df12h  ; vertical displayed
	dw      02d13h  ; offset
	dw      00014h  ; turn off dword mode
	dw      0e715h  ; v blank start
	dw      00616h  ; v blank end
	dw      0e317h  ; turn on byte mode
	dw      360
	dw      480

X360Y360  label word
	db      0e7h
	db      15
	dw      06b00h  ; horz total
	dw      05901h  ; horz displayed
	dw      05a02h  ; start horz blanking
	dw      08e03h  ; end horz blanking
	dw      05e04h  ; start h sync
	dw      08a05h  ; end h sync
	dw      04009h  ; cell height
	dw      08810h  ; v sync start
	dw      08511h  ; v sync end and protect cr0-cr7
	dw      06712h  ; vertical displayed
	dw      02d13h  ; offset
	dw      00014h  ; turn off dword mode
	dw      06d15h  ; v blank start
	dw      0ba16h  ; v blank end
	dw      0e317h  ; turn on byte mode
	dw      360
	dw      360


X376Y308 label word
	db      0e7h
	db      18
	dw      06e00h  ; horz total
	dw      05d01h  ; horz displayed
	dw      05e02h  ; start horz blanking
	dw      09103h  ; end horz blanking
	dw      06204h  ; start h sync
	dw      08f05h  ; end h sync
	dw      06206h  ; vertical total
	dw      00f07h  ; overflow
	dw      04009h  ;
	dw      0310fh  ;
	dw      03710h  ; v sync start
	dw      08911h  ; v sync end and protect cr0-cr7
	dw      03312h  ; vertical displayed
	dw      02f13h  ; offset
	dw      00014h  ; turn off dword mode
	dw      03c15h  ; v blank start
	dw      05c16h  ; v blank end
	dw      0e317h  ; turn on byte mode
	dw      376
	dw      308

X376Y564 label word
	db      0e7h
	db      18
	dw      06e00h  ; horz total
	dw      05d01h  ; horz displayed
	dw      05e02h  ; start horz blanking
	dw      09103h  ; end horz blanking
	dw      06204h  ; start h sync
	dw      08f05h  ; end h sync
	dw      06206h  ; vertical total
	dw      0f007h  ; overflow
	dw      06009h  ;
	dw      0310fh  ;
	dw      03710h  ; v sync start
	dw      08911h  ; v sync end and protect cr0-cr7
	dw      03312h  ; vertical displayed
	dw      02f13h  ; offset
	dw      00014h  ; turn off dword mode
	dw      03c15h  ; v blank start
	dw      05c16h  ; v blank end
	dw      0e317h  ; turn on byte mode
	dw      376
	dw      564

LAST_X_MODE         equ    13
ModeTable label word    ; Mode X tweak table
	dw      offset X320Y200
	dw      offset X320Y240
	dw      offset X360Y200
	dw      offset X360Y240
	dw      offset X376Y282
	dw      offset X320Y400
	dw      offset X320Y480
	dw      offset X360Y400
	dw      offset X360Y480
	dw      offset X360Y360
	dw      offset X376Y308
	dw      offset X376Y564
	dw      offset X256Y200
	dw      offset X256Y240


PARAMS label byte

	_CurrXMode               dw 0   ; Current graphics mode index
	_InGraphics              db 0   ; Flag indicating graphics activity
	_ScrnPhysicalByteWidth   dw 0   ; Physical width in bytes of screen
	_ScrnPhysicalPixelWidth  dw 0   ; Physical width in pixels of screen
	_ScrnPhysicalHeight      dw 0   ; Physical Height of screen
	_ErrorValue              db 0   ; Set after function calls


	_SplitScrnActive         db 0   ; Flag indicating Split scrn activity
	_DoubleBufferActive      dw 0   ; Flag indicating double buffering
	_TrippleBufferActive     dw 0   ; Flag indicating tripple buffering

	_SplitScrnScanLine       dw 0   ; Split Screen's starting scan line
	_SplitScrnVisibleHeight  dw 0   ; Split Screen's height on screen

	_SplitScrnOffs           dw 0   ; Offset in video ram of Split Screen
					; always = 0
	_Page0_Offs              dw 0   ; Ofset in video ram of Main virtual
					; screen ( = 0 if no split screen
					; otherwise = offset of first byte
					; after split screen
	_Page1_Offs              dw 0   ; Ofset in video ram of Second virtual
					; screen ( = 0 if no split screen
					; otherwise = offset of first byte
					; after split screen
					; = Page0_Offs if Doubble buffering
					; not enabled
	_Page2_Offs              dw 0

	_NonVisual_Offs          dw 0   ; Ofset in video ram of first byte
					; of non visible ram
	_ScrnLogicalByteWidth    dw 0   ; Logical width in bytes of screen
	_ScrnLogicalPixelWidth   dw 0   ; Logical width in pixels of screen
	_ScrnLogicalHeight       dw 0   ; Logical Height of screen

	_MaxScrollX              dw 0   ; Max X start position of Physical
					; screen within virtual screen (in
					; bytes)
	_MaxScrollY              dw 0   ; Max Y start position of Physical
					; screen within virtual screen

	_VisiblePageIdx          dw 0   ; Index of currently visible D.B.
					; page

	PageAddrTable label word
	_VisiblePageOffs         dw 0   ; Table containing starting offsets
	_HiddenPageOffs          dw 0   ; of the double buffer pages
	_WaitingPageOffs                 dw 0

	_TopClip                 dw 0   ; Clipping Rectangle
	_BottomClip              dw 0   ;
	_LeftClip                dw 0   ; Left/Right coordinates in bytes
	_RightClip               dw 0   ;
	_PhysicalStartByteX      dw 0   ; X byte coord of physical screen
					; relative to virtual virtual screen
	_PhysicalStartPixelX     dw 0   ; X pixel coord of physical screen
					; relative to virtual screen
	_PhysicalStartY          dw 0   ; Y pixel coord of physical screen
					; relative to virtual screen

; NEW
	_VsyncHandlerActive     dw      0
	_MouseRefreshFlag       dw      0
	_MouseVsyncHandler      dd      0
	_StartAddressFlag       dw      0
	_WaitingStartLow        dw      0
	_WaitingStartHigh       dw      0
	_WaitingPelPan          dw      0
	_VsyncPaletteStart      dw      0
	_VsyncPaletteCount      dw      0
	_VsyncPaletteBuffer     label  byte
				db  768  dup(?)


PARAMS_END label byte

PARAM_COUNT equ ($-PARAMS)


; Index/data pairs for CRT Controller registers that differ between
; mode 13h and mode X.

	;Pelpan values for 0,1,2,3 pixel panning to the left, respectively
	PelPanMask              db      000h,002h,004h,006h

DoubleScanFlag db ?     ; Flag to indicate double scanned mode

	.code

;-------------------------------------------------------------------------
; Local Logical Screen Width setting function
; cx = Requitrd Logical Width
;
; WARNING: no registers are preserved

SetLogicalScrWidth proc
	mov   dx,CRTC_INDEX
	mov   al,CRTC_OFFSET
	out   dx,al
	inc   dx

	mov   ax,cx
	cmp   ax,[_ScrnPhysicalPixelWidth]; Is logical width >= physical width
	jge   @@ValidLogicalWidth          ; yes - continue
	mov   ax,bx                        ; no - set logical width = physical

@@ValidLogicalWidth:
	shr   ax,3
	out   dx,al

	; The EXACT logical pixel width may not have been possible since
	; it should be divisible by 8. Round down to the closest possible
	; width and update the status variables

	shl   ax,1
	mov   bx,ax
	mov   [_ScrnLogicalByteWidth],ax  ; Store the byte width of virtual
	mov   [_RightClip],ax             ; Set default Right clip column
					  ; screen
	sub   ax,[_ScrnPhysicalByteWidth] ; Calculate and store Max X position
	shl   ax,2                        ; of physical screen in virtual
	mov   [_MaxScrollX],ax            ; screen in pixels
	mov   ax,bx                       ; set ax to byte width of virt scrn
	shl   ax,2                        ; convert to pixels
	mov   [_ScrnLogicalPixelWidth],ax ; store virt scrn pixel width
	mov   cx,ax                       ; save ax (return value)

	; calculate no. non split screen rows in video ram

	mov   ax,0ffffh                ; cx = Maximum video ram offset
	sub   dx,dx                    ; DX:AX is divide operand,  set DX = 0
	div   bx                       ; divide ax by ScrnLogicalByteWidth
	mov   [_ScrnLogicalHeight],ax  ; Save Screen Logical Height
	mov   [_BottomClip],ax         ; Set default bottom clip row
	sub   ax,[_ScrnPhysicalHeight] ; Update the maximum Y position of
	mov   [_MaxScrollY],ax         ; Physical screen in logical screen
	mov   ax,cx                    ; restore ax (return value)

	; calculate initial NonVisual
	mov  ax,[_ScrnLogicalByteWidth]
	mul  [_ScrnPhysicalHeight]
	mov  [_NonVisual_Offs],ax

@@Done: ret
SetLogicalScrWidth endp

clear_vram proc
		push  di
	mov   dx,SC_INDEX
	mov   ax,0f02h
	out   dx,ax               ; enable writes to all four planes
	mov   ax,SCREEN_SEG       ; now clear all display memory, 8 pixels
	mov   es,ax               ; at a time
	sub   di,di               ; point ES:DI to display memory

	WaitVsyncEnd

	sub   ax,ax               ; clear to zero-value pixels
	mov   cx,0FFFFh           ; # of words in display memory
	rep   stosw               ; clear all of display memory
		pop   di
	ret
clear_vram endp



;-----------------------------------------------------------------------
; Mode X graphics mode set with a virtual screen
;   logical screen width.
; C near-callable as:
;
;    int x_set_mode(unsigned int mode,unsigned int WidthInPixels);
;
; returns the actual width of the allocated virtual screen in pixels
; if a valid mode was selected otherwise returns -1
;
; Saves virtual screen pixel width in _ScrnLogicalPixelWidth.
; Saves virtual screen byte  width in _ScrnLogicalByteWidth.
; Physical screen dimensions are set in _ScrnPhysicalPixelWidth,
; _ScrnPhysicalByteWidth and _ScrnPhysicalHeight
;
;
; Modes:  0  = 320 x 200  (256 color)  NOTE: Some of these modes require
;         1  = 320 x 240  (256 color)     vertical size adjustment.
;         2  = 360 x 200  (256 color)
;         3  = 360 x 240  (256 color)
;         4  = 320 x 400  (256 color)
;         5  = 320 x 480  (256 color)
;         6  = 360 x 200  (256 color)
;         7  = 360 x 480  (256 color)
;         8  = 360 x 360  (256 color)
;         9  = 376 x 308  (256 color)
;         10 = 376 x 564  (256 color)
;
; Written by Themie Gouthas,
; parts adapted from M. Abrash code.
;------------------------------------------------------------------------
_x_set_mode proc
	ARG   mode:word,logicalscrwidth:word
	push  bp      ;preserve caller's stack frame
	mov   bp,sp

	push  si      ;preserve C register vars
	push  di      ; (don't count on BIOS preserving anything)

	cld
	mov   ax,ds
	mov   es,ax
	mov   di,offset PARAMS
	xor   ax,ax
	mov   cx,PARAM_COUNT
	rep   stosb

	mov   cx,[mode]
	cmp   cx,LAST_X_MODE        ; have we selected a valid mode
	jle   @@ValidMode           ; Yes !

	mov   [_InGraphics],FALSE   ; No return -1
	mov   ax,-1
	pop   di
	pop   si
	pop   bp
	ret

@@ValidMode:

	mov   [_CurrXMode],cx
	mov   [_InGraphics],TRUE

	xor   al,al
	cmp   cx,3
	jg    @@SetDoubleScanFlag
	mov   al,TRUE
@@SetDoubleScanFlag:
	mov   [DoubleScanFlag],al
	push  cx                    ; some bios's dont preserve cx

	call  clear_vram

	mov   ax,13h                ; let the BIOS set standard 256-color
	int   10h                   ;  mode (320x200 linear)


	pop   cx

	mov   dx,SC_INDEX
	mov   ax,0604h
	out   dx,ax                 ; disable chain4 mode
	mov   ax,0100h
	out   dx,ax                 ; synchronous reset while setting Misc
				    ;  Output for safety, even though clock
				    ;  unchanged

	mov   bx,offset ModeTable
	shl   cx,1
	add   bx,cx
	mov   si, word ptr [bx]
	lodsb

	or    al,al
	jz    @@DontSetDot
	mov   dx,MISC_OUTPUT
	out   dx,al               ; select the dot clock and Horiz
				  ;  scanning rate
@@DontSetDot:
	mov   dx,SC_INDEX
	mov   ax,0300h
	out   dx,ax               ; undo reset (restart sequencer)


	mov   dx,CRTC_INDEX       ; reprogram the CRT Controller
	mov   al,11h              ; VSync End reg contains register write
	out   dx,al               ; protect bit
	inc   dx                  ; CRT Controller Data register
	in    al,dx               ; get current VSync End register setting
	and   al,07fh             ; remove write protect on various
	out   dx,al               ; CRTC registers
	dec   dx                  ; CRT Controller Index
	cld
	xor   cx,cx
	lodsb
	mov   cl,al

@@SetCRTParmsLoop:
	lodsw                     ; get the next CRT Index/Data pair
	out   dx,ax               ; set the next CRT Index/Data pair
	loop  @@SetCRTParmsLoop

	mov   dx,SC_INDEX
	mov   ax,0f02h
	out   dx,ax               ; enable writes to all four planes
	mov   ax,SCREEN_SEG       ; now clear all display memory, 8 pixels
	mov   es,ax               ; at a time
	sub   di,di               ; point ES:DI to display memory
	sub   ax,ax               ; clear to zero-value pixels
	mov   cx,8000h            ; # of words in display memory
	rep   stosw               ; clear all of display memory

	;  Set pysical screen dimensions

	lodsw                               ; Load scrn pixel width
	mov   [_ScrnPhysicalPixelWidth],ax  ;  from tweak table and store
	mov   [_SplitScrnScanLine],ax       ; No splitscrn ==
					    ; splitscrn=PhysicalscrnHeight
	mov   bx,ax                         ; Copy width for later use
	shr   ax,2                          ; Convert to byte width
	mov   [_ScrnPhysicalByteWidth],ax   ; Store for later use
	lodsw                               ; Load Screen Phys. Height
	mov   [_ScrnPhysicalHeight],ax      ; Store for later use


	;  Mode X is set, now set the required logical page width.

	mov     cx,[logicalscrwidth]

	call    SetLogicalScrWidth

	pop     di      ;restore C register vars
	pop     si
	pop     bp      ;restore caller's stack frame
	ret
_x_set_mode endp

;----------------------------------------------------------------------
; Mode X (256 color mode) set default access video plane
;
; C near-callable as:
;    void x_select_default_plane(unsigned char plane);
;
; Enables Read/Write access to a plane using general memory access
; methods
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
_x_select_default_plane proc
ARG Plane:byte
	push bp
	mov  bp,sp       ; set up stack frame
	mov  cl,byte ptr [Plane]

	; SELECT WRITE PLANE
	and  cl,011b              ;CL = plane
	mov  ax,0100h + MAP_MASK  ;AL = index in SC of Map Mask reg
	shl  ah,cl                ;set only the bit for the required
				  ; plane to 1
	mov  dx,SC_INDEX          ;set the Map Mask to enable only the
	out  dx,ax                ; pixel's plane

	; SELECT READ PLANE
	mov  ah,cl                ;AH = plane
	mov  al,READ_MAP          ;AL = index in GC of the Read Map reg
	mov  dx,GC_INDEX          ;set the Read Map to read the pixel's
	out  dx,ax                ; plane

	pop  bp
	ret
_x_select_default_plane endp


;----------------------------------------------------------------------
; Mode X (256 color mode) Set Mode X split screen starting row
; The split screen resides on the bottom half of the screen and has a
; starting address of A000:0000
;
; C near-callable as:
;    void x_set_splitscreen(unsigned int line);
;
; Updates _Page0_Offs to reflect the existence of the split screen region
; ie _MainScrnOffset is set to the offset of the first pixel beyond the split
; screen region
;
; Written by Themie Gouthas
;----------------------------------------------------------------------

_x_set_splitscreen proc
	ARG Line:word
	push bp
	mov  bp,sp       ; set up stack frame
	push si

	xor  si,si       ; si=0 -> x virtual page start coord

	cmp  [_DoubleBufferActive],0
	jne   @@error

	cmp  [_SplitScrnActive],0
	je   @@NotPreviouslyCalled

@@error:
	mov  [_ErrorValue],ERROR
	pop  si
	pop  bp          ; Return if previously called
	ret

@@NotPreviouslyCalled:

	; Turn on split screen pal pen suppression, so the split screen
	; wo'nt be subject to pel panning as is the non split screen portion.

	mov  dx,INPUT_STATUS_0
	in   al,dx                  ; Reset the AC Index/Data toggle to
					;  index state
	mov  al,AC_MODE_CONTROL+20h ; Bit 5 set to prevent screen blanking
	mov  dx,AC_INDEX            ; Point AC to Index/Data register
	out  dx,al
	inc  dx                     ; Point to AC Data reg (for reads only)
	in   al,dx                  ; Get the current AC Mode Control reg
	or   al,20h                 ; Enable split scrn Pel panning suppress.
	dec  dx                     ; Point to AC Index/Data reg (for writes only)
	out  dx,al                  ; Write the new AC Mode Control setting
					;  with split screen pel panning
					;  suppression turned on

	mov  [_PhysicalStartByteX],ax   ; Set the Phisical screen start
	mov  [_PhysicalStartPixelX],ax  ; offset within virtual screen
	mov  [_PhysicalStartY],ax
	mov  [_SplitScrnActive],TRUE
	mov  ax,[Line]
	jns  @@NotNeg    ; Check that Split Scrn start scan line is +ve

	mov  ax,0        ; Since -ve set to 0

@@NotNeg:
	mov  [_SplitScrnScanLine],ax   ; save the scanline



	or    [DoubleScanFlag],0
	jz    @@NotDoubleScanned
	shl   ax,1
	dec   ax
@@NotDoubleScanned:
	;mov  cl,[DoubleScanFlag]
	;shl  ax,cl            ; Mode X 200 and 240 line modes are actually
				  ; 400 and 480 lines that are double scanned
				  ; so for start scanline multiply required ModeX
				  ; scan line by 2 if its a double scanned mode


	mov  bx,ax            ; save the scanline


	WaitVsyncStart        ; wait for vertical retrace

	cli                   ; Dont allow register setting to be interrupted
	mov  dx,CRTC_INDEX
	mov  ah,bl
	mov  al,LINE_COMPARE
	out  dx,ax            ; Bits 7-0 of the split screen scan line

	mov  ah,bh
	and  ah,1
	shl  ah,4
	mov  al,OVERFLOW     ; Bit 4 of overflow register = Bit 8 of split
	out  dx,al           ; screen scan line,
	inc  dx              ; So using readability of VGA registers
	in   al,dx           ; Read the OVERFLOW register, and set the
	and  al, not 10h     ; bit corresponding to Bit 8 (above)
	or   al,ah
	out  dx,al

	dec  dx
	mov  ah,bh
	and  ah,2
	ror  ah,3
	mov  al,MAX_SCAN_LINE  ; Bit 6 of max scan line register =
	out  dx,al             ; Bit 9 of split screen scan line
	inc  dx                ; As we did before, update the apropriate
	in   al,dx             ; bit without disturbing the rest
	and  al, not 40h
	or   al,ah
	out  dx,al
	sti                    ; Registers are set, so interrupts are safe

	mov  ax,[_ScrnPhysicalHeight]     ; Determine where the first byte
	sub  ax,[_SplitScrnScanLine]      ; of the non split screen video ram
	mov  [_SplitScrnVisibleHeight],ax ; starts and store it for reference

	mov  bx,[_ScrnLogicalByteWidth]
	mul  bx
	mov  [_Page0_Offs],ax
	mov  [_Page1_Offs],ax
	mov  [_Page2_Offs],ax

	; calculate no. non split screen rows in video ram
	mov  cx,0ffffh             ; cx = Maximum video ram offset
	sub  cx,ax                 ; cx = cx - _Page0_Offs
	xchg cx,ax                 ; swap cx and ax
	sub  dx,dx                 ; DX:AX is divide operand,  set DX = 0
	div  bx                    ; divide ax (prev cx) by
				   ; ScrnLogicalByteWidth

	mov  [_ScrnLogicalHeight],ax     ; Save Screen Logical Height
	cmp   ax,[_BottomClip]
	jle   @@BottomClipOK             ; Adjust Clip Rectangle if necessary
	mov   [_BottomClip],ax
@@BottomClipOK:
	sub  ax,[_SplitScrnScanLine]     ; Update the maximum Y position of
	mov  [_MaxScrollY],ax            ; Physical screen in logical screen

	xchg cx,ax                       ; restore original ax (MainScrnOfs)
	mov  bh,al                       ; Set the visible screen start address
	mov  ch,ah                       ; to the top left corner of the virtual
	jmp  StartAddrEntry              ; screen
_x_set_splitscreen      endp


;-----------------------------------------------------------------------
; Mode X (256 color mode) Page flip primer
; No clipping is performed.
; C near-callable as:
;
;    void x_page_flip(unsigned int x, unsigned int y);
;
; Swaps visible and hidden page offsets and then executes the SetStartAddr
; to achieve a page flip.
;
; SEE x_set_start_addr below
;
; Written by Themie Gouthas
;------------------------------------------------------------------------

_x_page_flip proc
	ARG x:word,y:word
	push  bp                  ;preserve caller's stack frame
	mov   bp,sp               ;point to local stack frame
	push  si

	mov  si,[x]
	mov  ax,[_ScrnLogicalByteWidth]     ; Calculate Offset increment
	mov  cx,[y]
	mul  cx                             ; for Y
	cmp  [_DoubleBufferActive],TRUE     ; Do we have double buffering ?
	je   @@DoubleBuffer
	cmp  [_TrippleBufferActive],TRUE
	jne   PageFlipEntry1

; TrippleBuffer
	mov  bx,[_HiddenPageOffs]
	xchg bx,[_VisiblePageOffs]
	xchg bx,[_WaitingPageOffs]
	mov  [_HiddenPageOffs],bx
	mov  bx,[_VisiblePageIdx]
	inc  bx
	cmp  bx,3
	jne  @@IdxOk
	xor  bx,bx
@@IdxOk:
	mov  [_VisiblePageIdx],bx
	jmp  short PageFlipEntry2
@@DoubleBuffer:
	mov  bx,[_HiddenPageOffs]
	xchg bx,[_VisiblePageOffs]          ; Swap the Page Offsete
	xchg [_HiddenPageOffs],bx
	xor  [_VisiblePageIdx],01h          ; Set the Visible page index
	jmp  short PageFlipEntry2
_x_page_flip endp


;-----------------------------------------------------------------------
; Mode X (256 color mode) Set Mode X non split screen start address
;   of logical screen.
; C near-callable as:
;
;    void x_set_start_addr(unsigned int x, unsigned int y);
;
; Params: StartOffset is offset of first byte of logical screen ram
;           (Useful if you want to double buffer by splitting your non
;            split screen video ram into 2 pages)
;        X,Y coordinates of the top left hand corner of the physical screen
;           within the logical screen
;           X must not exceed (Logical screen width - Physical screen width)
;           Y must not exceed (Logical screen height - Physical screen height)
;
;
; Written by Themie Gouthas,
; Parts addapted from M. Abrash code published in DDJ Mag.
;------------------------------------------------------------------------
_x_set_start_addr proc
	ARG x:word,y:word
	push bp
	mov  bp,sp
	push si

	mov  si,[x]
	mov  ax,[_ScrnLogicalByteWidth]     ; Calculate Offset increment
	mov  cx,[y]                         ; for Y
	mul  cx
	cmp  [_DoubleBufferActive],TRUE     ; Do we have double buffering ?
	je   @@PageResolution
	cmp  [_TrippleBufferActive],TRUE
	je   @@PageResolution
PageFlipEntry1:
	add  ax,[_Page0_Offs]               ; no - add page 0 offset
	jmp  short @@AddColumn

PageFlipEntry2:

	mov  [_PhysicalStartPixelX],si
	mov  [_PhysicalStartY],cx

@@PageResolution:
	add  ax,[_VisiblePageOffs]          ; Add visible page offset

@@AddColumn:
	mov  cx,si
	shr  cx,2
	mov  [_PhysicalStartByteX],cx
	add  ax,cx                          ; add the column offset for X
	mov  bh,al                          ; setup CRTC start addr regs and
						; values in word registers for
	mov  ch,ah                          ; fast word outs

StartAddrEntry:
	mov  bl,ADDR_LOW
	mov  cl,ADDR_HIGH
	and  si,0003h             ; select pel pan register value for the
	mov  ah,PelPanMask[si]    ; required x coordinate
	mov  al,PEL_PANNING+20h
	mov  si,ax

	cmp  [_VsyncHandlerActive],TRUE
	jne   @@NoVsyncHandler
; NEW STUFF
@@WaitLast:
	cmp   [_StartAddressFlag],0
	jne   @@WaitLast
	cli
	mov  [_WaitingStartLow],bx
	mov  [_WaitingStartHigh],cx
	mov  [_WaitingPelPan],si
	mov  [_StartAddressFlag],1
	sti
	jmp  short @@Return

@@NoVsyncHandler:
	mov  dx,INPUT_STATUS_0    ;Wait for trailing edge of Vsync pulse
@@WaitDE:
	in   al,dx
	test al,01h
	jnz  @@WaitDE            ;display enable is active low (0 = active)

	mov  dx,CRTC_INDEX
	mov  ax,bx
	cli
	out  dx,ax               ;start address low
	mov  ax,cx
	out  dx,ax               ;start address high
	sti

; Now wait for vertical sync, so the other page will be invisible when
; we start drawing to it.
	mov  dx,INPUT_STATUS_0    ;Wait for trailing edge of Vsync pulse
@@WaitVS:
	in   al,dx
	test al,08h
	jz @@WaitVS           ;display enable is active low (0 = active)


	mov  dx,AC_INDEX
	mov  ax,si                ; Point the attribute controller to pel pan
	cli
	out  dx,al                ; reg. Bit 5 also set to prevent blanking
	mov  al,ah
	out  dx,al                ; load new Pel Pan setting.
	sti

@@Return:
	mov  [_ErrorValue],OK
	pop  si
	pop  bp
	ret
_x_set_start_addr  endp


;-----------------------------------------------------------------------
; Mode X (256 color mode) Mode X split screen hide
; C near-callable as:
;
;    void x_hide_splitscreen()
;
; Hides an existing split screen by setting its starting scan line to
; the last physical screen scan line
;
; WARNING: Only to be used if SplitScrnLine has been previously called
; WARNING: DO NOT USE with mode 5-11 (320x400-376x564). The memory for
;          the initial split screen is reserved and the size limitations
;          of these modes means any change in the split screen scan line
;          will encroach on the split screen ram
;
; Written by Themie Gouthas
;------------------------------------------------------------------------

_x_hide_splitscreen proc
	push bp
	mov  bp,sp

	cmp  [_SplitScrnActive],TRUE
	je   @@SplitScreenEnabled

@@error:
	mov  [_ErrorValue],ERROR
	pop  bp
	ret

@@SplitScreenEnabled:
	cmp  [_CurrXMode],4          ; Do nothing for Modes > 2
	jg   @@error
	mov  bx,[_ScrnPhysicalHeight]

	mov  ax,[_ScrnLogicalHeight]
	sub  ax,bx
	mov  [_MaxScrollY],ax
	xor  ax,ax
	mov  [_SplitScrnVisibleHeight],ax

	or    [DoubleScanFlag],0
	jz    @@NotDoubleScanned
	shl   bx,1
	dec   bx
@@NotDoubleScanned:
	;mov  cl,[DoubleScanFlag]  ; Compensate for double scanned modes
	;shl  bx,cl

	WaitVsyncStart               ; wait for vertical retrace

	cli                 ; Dont allow register setting to be interrupted
	mov  dx,CRTC_INDEX
	mov  ah,bl
	mov  al,LINE_COMPARE
	out  dx,ax          ; Bits 7-0 of the split screen scan line

	mov  ah,bh
	and  ah,1
	shl  ah,4
	mov  al,OVERFLOW  ; Bit 4 of overflow register = Bit 8 of split
	out  dx,al        ; screen scan line,
	inc  dx           ; So using readability of VGA registers
	in   al,dx        ; Read the OVERFLOW register, and set the
	and  al, not 10h  ; bit corresponding to Bit 8 (above)
	or   al,ah
	out  dx,al

	dec  dx
	mov  ah,bh
	and  ah,2
	ror  ah,3
	mov  al,MAX_SCAN_LINE  ; Bit 6 of max scan line register =
	out  dx,al             ; Bit 9 of split screen scan line
	inc  dx                ; As we did before, update the apropriate
	in   al,dx             ; bit without disturbing the rest
	and  al, not 40h
	or   al,ah
	out  dx,al
	sti                  ; Registers are set, so interrupts are safe

@@done:

	mov  [_ErrorValue],OK
	pop  bp
	ret
_x_hide_splitscreen endp

;-----------------------------------------------------------------------
; Mode X (256 color mode) Mode X split screen show
; C near-callable as:
;
;    void x_show_splitscreen()
;
; Restores split screen start scan line to the initial split screen
; starting scan line as set by SplitScrnLine.
;
; WARNING: Only to be used if SplitScrnLine has been previously called
; WARNING: DO NOT USE with mode 5-11 (320x400-376x564). The memory for
;          the initial split screen is reserved and the size limitations
;          of these modes means any change in the split screen scan line
;          will encroach on the split screen ram
;          Update: Now disabled for these modes
;
; Written by Themie Gouthas
;------------------------------------------------------------------------


_x_show_splitscreen proc
	push bp
	mov  bp,sp

	cmp  [_SplitScrnActive],TRUE
	je   @@SplitScreenEnabled

@@error:
	mov  [_ErrorValue],ERROR
	pop  bp
	ret

@@SplitScreenEnabled:
	cmp  [_CurrXMode],4          ; Do nothing for Modes > 2
	jg   @@error

	mov  bx,[_SplitScrnScanLine]
	mov  ax,[_ScrnLogicalHeight] ; Update Max Scroll Y
	sub  ax,bx
	mov  [_MaxScrollY],ax

	mov  ax,[_ScrnPhysicalHeight]
	sub  ax,bx
	mov  [_SplitScrnVisibleHeight],ax

	or    [DoubleScanFlag],0
	jz    @@NotDoubleScanned
	shl   bx,1
	dec   bx
@@NotDoubleScanned:
	;mov  cl,[DoubleScanFlag]  ; Compensate for double scanned modes
	;shl  bx,cl
	WaitVsyncStart               ; wait for vertical retrace

	cli                          ; Dont allow register setting to be interrupted
	mov  dx,CRTC_INDEX
	mov  ah,bl
	mov  al,LINE_COMPARE
	out  dx,ax                  ; Bits 7-0 of the split screen scan line

	mov  ah,bh
	and  ah,1
	shl  ah,4
	mov  al,OVERFLOW  ; Bit 4 of overflow register = Bit 8 of split
	out  dx,al        ; screen scan line,
	inc  dx           ; So using readability of VGA registers
	in   al,dx        ; Read the OVERFLOW register, and set the
	and  al, not 10h  ; bit corresponding to Bit 8 (above)
	or   al,ah
	out  dx,al

	dec  dx
	mov  ah,bh
	and  ah,2
	ror  ah,3
	mov  al,MAX_SCAN_LINE  ; Bit 6 of max scan line register =
	out  dx,al             ; Bit 9 of split screen scan line
	inc  dx                ; As we did before, update the apropriate
	in   al,dx             ; bit without disturbing the rest
	and  al, not 40h
	or   al,ah
	out  dx,al
	sti                  ; Registers are set, so interrupts are safe

@@Done:
	mov  [_ErrorValue],0
	pop  bp
	ret
_x_show_splitscreen endp


;-----------------------------------------------------------------------
; Mode X (256 color mode) Modify Mode X split screen starting scan line
; C near-callable as:
;
;    void x_adjust_splitscreen(unsigned int ScanLine)
;
; Sets the split screen start scan line to a new scan line. Valid scan lines
; are between the initial split screen starting scan line and the last
; physical screen scan line.
;
; WARNING: Only to be used if SplitScrnLine has been previously called
; WARNING: DO NOT USE with mode 5-11 (320x400-376x564). The memory for
;          the initial split screen is reserved and the size limitations
;          of these modes means any change in the split screen scan line
;          will encroach on the split screen ram
;          Update: Now disabled for these modes
;
;
; Written by Themie Gouthas
;------------------------------------------------------------------------


_x_adjust_splitscreen proc
	ARG   ScanLine
	push bp
	mov  bp,sp

	cmp  [_SplitScrnActive],TRUE
	je   @@SplitScreenEnabled

@@error:
	mov  [_ErrorValue],ERROR
	pop  bp
	ret

@@SplitScreenEnabled:
	cmp  [_CurrXMode],4          ; Do nothing for Modes > 2
	jg   @@error
	mov  bx,[ScanLine]            ; Is the required starting scan line
	cmp  bx,[_SplitScrnScanLine]  ; valid ?
	js   @@Done                   ; No - Then do nothing

@@ValidScanLine:

	mov  ax,[_ScrnLogicalHeight] ; Update Max Scroll Y
	sub  ax,bx
	mov  [_MaxScrollY],ax

	mov  ax,[_ScrnPhysicalHeight]
	sub  ax,bx
	mov  [_SplitScrnVisibleHeight],ax

	or    [DoubleScanFlag],0
	jz    @@NotDoubleScanned
	shl   bx,1
	dec   bx
@@NotDoubleScanned:
	;mov  cl,[DoubleScanFlag]   ; Compensate for double scanned modes
	;shl  bx,cl

	WaitVsyncStart      ; wait for vertical retrace

	cli                 ; Dont allow register setting to be interrupted

	mov  dx,CRTC_INDEX
	mov  ah,bl
	mov  al,LINE_COMPARE
	out  dx,ax          ; Bits 7-0 of the split screen scan line

	mov  ah,bh
	and  ah,1
	shl  ah,4
	mov  al,OVERFLOW  ; Bit 4 of overflow register = Bit 8 of split
	out  dx,al        ; screen scan line,
	inc  dx           ; So using readability of VGA registers
	in   al,dx        ; Read the OVERFLOW register, and set the
	and  al, not 10h  ; bit corresponding to Bit 8 (above)
	or   al,ah
	out  dx,al

	dec  dx
	mov  ah,bh
	and  ah,2
	ror  ah,3
	mov  al,MAX_SCAN_LINE  ; Bit 6 of max scan line register =
	out  dx,al             ; Bit 9 of split screen scan line
	inc  dx                ; As we did before, update the apropriate
	in   al,dx             ; bit without disturbing the rest
	and  al, not 40h
	or   al,ah
	out  dx,al
	sti                    ; Registers are set, so interrupts are safe
@@Done:
	mov  [_ErrorValue],OK
	pop   bp
	ret
_x_adjust_splitscreen endp



;-----------------------------------------------------------------------
; Mode X (256 color mode) Enable DoubleBuffering on non split screen area
; C near-callable as:
;
;    int x_set_doublebuffer(unsigned int PageHeight);
;
; Params: PageHeight is the height of the virtual screen to double buffer
;         Returns the closest possible height to the specified.
;
; Sets up two double buffering virtual pages
; GLOBAL variables set:
;
;      _Page1_Offs              Offset of second virtual page
;      _NonVisual_Offs          Offset of first non visible video ram byte
;      _DoubleBufferActive      Flag
;      _PageAddrTable           Table of Double buffering pages start offsets
;      _ScrnLogicalHeight       Logical height of the double buffering pages
;
;
; Written by Themie Gouthas
;------------------------------------------------------------------------


_x_set_doublebuffer proc
	   ARG PageHeight:word
	   push  bp
	   mov   bp,sp

	   cmp   [_DoubleBufferActive],0
	   je    @@OkToContinue
@error:
	   mov   [_ErrorValue],ERROR
	   pop   bp
	   ret

@@OkToContinue:
	   mov   [_VisiblePageIdx],0     ; Set visible Page to 0
	   mov   ax,[_ScrnLogicalHeight] ; Set Maximum D.B. Page height to
	   shr   ax,1                    ;   _ScrnLogicalHeight / 2

	   mov   bx,[PageHeight]         ; Is the require D.B. Page Height
	   cmp   ax,bx                   ;  > the Maximum  D.B. Page Height ?

	   js    @@InvalidHeight         ; no  - jump
	   mov   ax,bx                   ; yes - Set the D.B. Page height to
					 ;       to the maximum allowed.

@@InvalidHeight:
	   mov   [_ScrnLogicalHeight],ax    ; Update logical screen height to
					;  reflect the height of a D.B. page
	   cmp   ax,[_BottomClip]
	   jle   @@BottomClipOK             ; Adjust Clip Rectangle if necessary
	   mov   [_BottomClip],ax
@@BottomClipOK:
	   push  ax
	   mul   [_ScrnLogicalByteWidth]    ; Calculate the offset of the second
	   mov   cx,ax                      ;  D.B. Page in video ram
	   mov   bx,[_Page0_Offs]
	   mov   [_VisiblePageOffs],bx

	   add   ax,bx
	   mov   [_Page1_Offs],ax           ; Save it
	   mov   [_HiddenPageOffs],ax

	   add   ax,cx                      ; Calculate the offset of first byte
	   mov   [_NonVisual_Offs],ax       ;  beyond the D.B. pages and save it
	   mov   [_DoubleBufferActive],TRUE ; Set flag indicating D.B'ing mode on

	   pop   ax
	   sub   ax,[_ScrnPhysicalHeight]
	   add   ax,[_SplitScrnVisibleHeight]
	   mov   [_MaxScrollY],ax

	   mov   ax,dx                      ; return the D.B. pages' height
	   mov   [_ErrorValue],OK
	   pop   bp
	   ret
_x_set_doublebuffer endp


;-----------------------------------------------------------------------
; Mode X (256 color mode) Enable TrippleBuffering on non split screen area
; C near-callable as:
;
;    int x_set_tripplebuffer(unsigned int PageHeight);
;
; Params: PageHeight is the height of the virtual screen to tripple buffer
;         Returns the closest possible height to the specified.
;
; Sets up two tripple buffering virtual pages
; GLOBAL variables set:
;
;      _Page1_Offs              Offset of second virtual page
;      _Page2_Offs              Offset of third virtual page
;      _NonVisual_Offs          Offset of first non visible video ram byte
;      _DoubleBufferActive      Flag
;      _PageAddrTable           Table of Double buffering pages start offsets
;      _ScrnLogicalHeight       Logical height of the double buffering pages
;
;
; Almost written by Tore Bastiansen (cut & paste from _x_set_doublebuffer)
;------------------------------------------------------------------------

_x_set_tripplebuffer proc
	   ARG PageHeight:word
	   push  bp
	   mov   bp,sp

	   cmp   [_DoubleBufferActive],0
	   jne   @@Error
	   cmp   [_TrippleBufferActive],0
	   je    @@OkToContinue
@@Error:
	   mov   [_ErrorValue],ERROR
	   pop   bp
	   ret

@@OkToContinue:
	   mov   [_VisiblePageIdx],0     ; Set visible Page to 0
	   mov   ax,[_ScrnLogicalHeight] ; Set Maximum T.B. Page height to
	   mov   bx,3
	   xor   dx,dx
	   idiv  bx                      ;   _ScrnLogicalHeight / 3

	   mov   bx,[PageHeight]         ; Is the require T.B. Page Height
	   cmp   ax,bx                   ;  > the Maximum  T.B. Page Height ?

	   js    @@InvalidHeight         ; no  - jump
	   mov   ax,bx                   ; yes - Set the T.B. Page height to
									 ;       to the maximum allowed.

@@InvalidHeight:
	   mov   [_ScrnLogicalHeight],ax    ; Update logical screen height to
										;  reflect the height of a T.B. page
	   cmp   ax,[_BottomClip]
	   jle   @@BottomClipOK             ; Adjust Clip Rectangle if necessary
	   mov   [_BottomClip],ax
@@BottomClipOK:
	   push  ax
	   mul   [_ScrnLogicalByteWidth]    ; Calculate the offset of the second
	   mov   cx,ax                      ;  D.B. Page in video ram
	   mov   bx,[_Page0_Offs]
	   mov   [_VisiblePageOffs],bx

	   add   ax,bx
	   mov   [_Page1_Offs],ax           ; Save it
	   mov   [_HiddenPageOffs],ax

	   add   ax,cx
	   mov   [_Page2_Offs],ax            ; Save the other it ?
	   mov   [_WaitingPageOffs],ax

	   add   ax,cx                       ; Calculate the offset of first byte
	   mov   [_NonVisual_Offs],ax        ;  beyond the D.B. pages and save it
	   mov   [_TrippleBufferActive],TRUE ; Set flag indicating T.B'ing mode on

	   pop   ax
	   sub   ax,[_ScrnPhysicalHeight]
	   add   ax,[_SplitScrnVisibleHeight]
	   mov   [_MaxScrollY],ax

	   mov   ax,dx                      ; return the D.B. pages' height
	   mov   [_ErrorValue],OK
	   pop   bp
	   ret
_x_set_tripplebuffer endp


;-----------------------------------------------------------------------
; Set Clipping rectangle
; C callable as:
;
;
;    int x_set_cliprect(WORD left,WORD top, WORD right, WORD bottom);
;
;
; NOTE clipping is byte oriented. "left" and "right" are in bytes not pixels.
;    Only selected functions perform any clipping at all.
;
; Written by Themie Gouthas
;------------------------------------------------------------------------

_x_set_cliprect proc
ARG left:word,top:word,right:word,bottom:word
       push  bp
       mov   bp,sp
       mov   ax,[left]
       mov   bx,[right]
       cmp   bx,ax
       jns   @@CorrectXOrder
       xchg  bx,ax
@@CorrectXOrder:
       mov   [_LeftClip],ax
       mov   [_RightClip],bx
	   mov   ax,[top]
       mov   bx,[bottom]
       cmp   bx,ax
	   jns   @@CorrectYOrder
       xchg  bx,ax
@@CorrectYOrder:
       mov   [_TopClip],ax
       mov   [_BottomClip],bx
       pop   bp
       ret
_x_set_cliprect endp


;----------------------------------------------------------------------
; Return to text mode
;
_x_text_mode proc
       push  bp
       call  clear_vram
       mov   ax,03h        ; Restore Text Mode
       int   10h

       pop   bp
       ret
_x_text_mode endp

;-----------------------------------------------------------------------
; Wait for Vertical sync
_x_wait_vsync proc
	push  bp
	WaitVsyncStart
	pop   bp
	ret
_x_wait_vsync endp


	end
