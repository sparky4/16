;-----------------------------------------------------------------------
; MODULE XRECT
;
; Rectangle functions all MODE X 256 Color resolutions
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
;-----------------------------------------------------------------------


include xlib.inc
include xrect.inc


	.data
; Plane masks for clipping left and right edges of rectangle.
        LeftClipPlaneMask       db      00fh,00eh,00ch,008h
	RightClipPlaneMask      db      00fh,001h,003h,007h
	.code

;---------------------------------------------------------------------------
; Mode X (320x240, 256 colors) rectangle solid colour fill routine.
;
; Based on code originally published in DDJ Mag by M. Abrash
;
; with TASM 2. C near-callable as:
;
;    void x_rect_fill_clipped(int StartX, int StartY, int EndX, int EndY,
;       unsigned int PageBase, unsigne int color);
;
;


_x_rect_fill_clipped proc
ARG     StartX:word,StartY:word,EndX:word,EndY:word,PageBase:word,Color:word
	push bp              ;preserve caller's stack frame
	mov  bp,sp           ;point to local stack frame
	push si              ;preserve caller's register variables
	push di

	mov   dx,[_TopClip]           ; Compare u.l. Y coord with Top
	mov   cx,[_BottomClip]
	mov   ax,[StartY]
	mov   bx,[EndY]
        cmp   dx,ax
        jle   @@CheckBottomClip
	cmp   dx,bx
	jg    @@NotVisible
	mov   [StartY],dx

@@CheckBottomClip:
	cmp   cx,bx
	jg    @@CheckLeftClip
	cmp   cx,ax
	jl    @@NotVisible
	mov   [EndY],cx

@@CheckLeftClip:
	mov   dx,[_LeftClip]           ; Compare u.l. Y coord with Top
	mov   cx,[_RightClip]
	mov   ax,[StartX]
	mov   bx,[EndX]
	sal   dx,2
	sal   cx,2
	cmp   dx,ax
	jle   @@CheckRightClip
	cmp   dx,bx
	jg    @@NotVisible
	mov   [StartX],dx

@@CheckRightClip:
	cmp   cx,bx
	jg    RFClipDone
	cmp   cx,ax
	jl    @@NotVisible
	mov   [EndX],cx
	jmp   RFClipDone

@@NotVisible:
	mov   ax,1
	pop   di                          ; restore registers
	pop   si
	pop   bp
	ret
_x_rect_fill_clipped endp



;---------------------------------------------------------------------------
; Mode X (320x240, 256 colors) rectangle solid colour fill routine.
;
; Based on code originally published in DDJ Mag by M. Abrash
;
; with TASM 2. C near-callable as:
;
;    void x_rect_fill(int StartX, int StartY, int EndX, int EndY,
;       unsigned int PageBase, unsigne int color);
;
;


_x_rect_fill proc
ARG     StartX:word,StartY:word,EndX:word,EndY:word,PageBase:word,Color:word
	push bp              ;preserve caller's stack frame
	mov  bp,sp           ;point to local stack frame
	push si              ;preserve caller's register variables
	push di

RFClipDone:
	cld
	mov  ax,[_ScrnLogicalByteWidth]
	mul  [StartY]            ;offset in page of top rectangle scan line
	mov  di,[StartX]
	sar  di,2                ;X/4 = offset of first rectangle pixel in scan
	add  di,ax               ;offset of first rectangle pixel in page
	add  di,[PageBase]       ;offset of first rectangle pixel in
			         ; display memory
	mov  ax,SCREEN_SEG       ;point ES:DI to the first rectangle
	mov  es,ax               ; pixel's address
	mov  dx,SC_INDEX         ;set the Sequence Controller Index to
	mov  al,MAP_MASK         ; point to the Map Mask register
	out  dx,al
	inc  dx                  ;point DX to the SC Data register
	mov  si,[StartX]
	and  si,0003h                    ;look up left edge plane mask
	mov  bh,LeftClipPlaneMask[si]    ; to clip & put in BH
	mov  si,[EndX]
	and  si,0003h                    ;look up right edge plane
	mov  bl,RightClipPlaneMask[si]   ; mask to clip & put in BL

	mov  cx,[EndX]                   ;calculate # of addresses across rect
	mov  si,[StartX]
	cmp  cx,si
	jle  @@FillDone                  ;skip if 0 or negative width
	dec  cx
	and  si,not 011b
	sub  cx,si
	sar  cx,2                 ;# of addresses across rectangle to fill - 1
	jnz  @@MasksSet           ;there's more than one byte to draw
	and  bh,bl                ;there's only one byte, so combine the left
                                  ; and right edge clip masks
@@MasksSet:
	mov  si,[EndY]
	sub  si,[StartY]            ;BX = height of rectangle
	jle  @@FillDone             ;skip if 0 or negative height
	mov  ah,byte ptr [Color]    ;color with which to fill
	mov  bp,[_ScrnLogicalByteWidth]  ;stack frame isn't needed any more
	sub  bp,cx                  ;distance from end of one scan line to start
	dec  bp                     ; of next
@@FillRowsLoop:
	push cx                     ;remember width in addresses - 1
	mov  al,bh                  ;put left-edge clip mask in AL
	out  dx,al                  ;set the left-edge plane (clip) mask
	mov  al,ah                  ;put color in AL
	stosb                       ;draw the left edge
	dec  cx                     ;count off left edge byte
	js   @@FillLoopBottom       ;that's the only byte
	jz   @@DoRightEdge          ;there are only two bytes
	mov  al,00fh                ;middle addresses drawn 4 pixels at a pop
	out  dx,al                  ;set the middle pixel mask to no clip
	mov  al,ah                  ;put color in AL
	rep  stosb                  ;draw middle addresses four pixels apiece
@@DoRightEdge:
	mov  al,bl                  ;put right-edge clip mask in AL
	out  dx,al                  ;set the right-edge plane (clip) mask
	mov  al,ah                  ;put color in AL
        stosb                       ;draw the right edge
@@FillLoopBottom:
	add  di,bp                  ;point to start of the next scan line of
                                    ; the rectangle
	pop  cx                     ;retrieve width in addresses - 1
	dec  si                     ;count down scan lines
	jnz  @@FillRowsLoop
@@FillDone:
	pop  di                     ;restore caller's register variables
	pop  si
	pop  bp                     ;restore caller's stack frame
        ret
_x_rect_fill endp



;---------------------------------------------------------------------------
; Mode X (320x240, 256 colors) rectangle 4x4 pattern fill routine.
; Upper left corner of pattern is always aligned to a multiple-of-4
; row and column. Works on all VGAs. Uses approach of copying the
; pattern to off-screen display memory, then loading the latches with
; the pattern for each scan line and filling each scan line four
; pixels at a time. Fills up to but not including the column at EndX
; and the row at EndY. No clipping is performed. All ASM code tested
;
;
; Based on code originally published in DDJ Mag by M. Abrash
;
;
;  C near-callable as:
;
;    void x_rect_pattern_clipped(int StartX, int StartY, int EndX, int EndY,
;       unsigned int PageBase, char far * Pattern);
;
;

_x_rect_pattern_clipped proc
ARG     StartX:word,StartY:word,EndX:word,EndY:word,PageBase:word,Pattern:dword
LOCAL   NextScanOffset:word,RectAddrWidth:word,Height:word=LocalStk
	push bp                       ;preserve caller's stack frame
	mov  bp,sp                    ;point to local stack frame
	sub  sp,LocalStk              ;allocate space for local vars
	push si                       ;preserve caller's register variables
	push di
	push ds

	mov   dx,[_TopClip]           ; Compare u.l. Y coord with Top
	mov   cx,[_BottomClip]
	mov   ax,[StartY]
	mov   bx,[EndY]
	cmp   dx,ax
	jle   @@CheckBottomClip
	cmp   dx,bx
	jg    @@NotVisible
	mov   [StartY],dx

@@CheckBottomClip:
	cmp   cx,bx
	jg    @@CheckLeftClip
	cmp   cx,ax
	jl    @@NotVisible
	mov   [EndY],cx

@@CheckLeftClip:
	mov   dx,[_LeftClip]           ; Compare u.l. Y coord with Top
	mov   cx,[_RightClip]
	mov   ax,[StartX]
	mov   bx,[EndX]
	sal   dx,2
	sal   cx,2
	cmp   dx,ax
	jle   @@CheckRightClip
	cmp   dx,bx
	jg    @@NotVisible
	mov   [StartX],dx

@@CheckRightClip:
	cmp   cx,bx
	jg    RPClipDone
	cmp   cx,ax
	jl    @@NotVisible
	mov   [EndX],cx
	jmp   RPClipDone

@@NotVisible:
	mov   ax,1
	pop   ds
	pop   di                          ; restore registers
	pop   si
	mov   sp,bp
	pop   bp
	ret

_x_rect_pattern_clipped endp

;---------------------------------------------------------------------------
; Mode X (320x240, 256 colors) rectangle 4x4 pattern fill routine.
; Upper left corner of pattern is always aligned to a multiple-of-4
; row and column. Works on all VGAs. Uses approach of copying the
; pattern to off-screen display memory, then loading the latches with
; the pattern for each scan line and filling each scan line four
; pixels at a time. Fills up to but not including the column at EndX
; and the row at EndY. No clipping is performed. All ASM code tested
;
;
; Based on code originally published in DDJ Mag by M. Abrash
;
;
;  C near-callable as:
;
;    void x_rect_pattern(int StartX, int StartY, int EndX, int EndY,
;       unsigned int PageBase, char far * Pattern);



_x_rect_pattern proc
ARG     StartX:word,StartY:word,EndX:word,EndY:word,PageBase:word,Pattern:dword
LOCAL   NextScanOffset:word,RectAddrWidth:word,Height:word=LocalStk
	push bp                       ;preserve caller's stack frame
	mov  bp,sp                    ;point to local stack frame
	sub  sp,LocalStk              ;allocate space for local vars
	push si                       ;preserve caller's register variables
	push di
	push ds

RPClipDone:
	cld
	mov  ax,SCREEN_SEG            ;point ES to display memory
	mov  es,ax
				      ;copy pattern to display memory buffer
	lds  si,dword ptr [Pattern]   ;point to pattern to fill with
	mov  di,PATTERN_BUFFER        ;point ES:DI to pattern buffer
	mov  dx,SC_INDEX              ;point Sequence Controller Index to
	mov  al,MAP_MASK              ; Map Mask
	out  dx,al
	inc  dx                       ;point to SC Data register
	mov  cx,4                     ;4 pixel quadruplets in pattern
@@DownloadPatternLoop:
	mov  al,1                     ;
	out  dx,al                    ;select plane 0 for writes
        movsb                         ;copy over next plane 0 pattern pixel
	dec  di                       ;stay at same address for next plane
	mov  al,2                     ;
	out  dx,al                    ;select plane 1 for writes
	movsb                         ;copy over next plane 1 pattern pixel
	dec  di                       ;stay at same address for next plane
	mov  al,4                     ;
	out  dx,al                    ;select plane 2 for writes
        movsb                         ;copy over next plane 2 pattern pixel
	dec  di                       ;stay at same address for next plane
	mov  al,8                     ;
	out  dx,al                    ;select plane 3 for writes
        movsb                         ;copy over next plane 3 pattern pixel
                                      ; and advance address
        loop @@DownloadPatternLoop
        pop  ds

	mov  dx,GC_INDEX              ;set the bit mask to select all bits
	mov  ax,00000h+BIT_MASK       ; from the latches and none from
	out  dx,ax                    ; the CPU, so that we can write the
                                      ; latch contents directly to memory
	mov  ax,[StartY]              ;top rectangle scan line
	mov  si,ax
	and  si,011b                  ;top rect scan line modulo 4
	add  si,PATTERN_BUFFER        ;point to pattern scan line that
				      ; maps to top line of rect to draw
	mov  dx,[_ScrnLogicalByteWidth]
	mul  dx                       ;offset in page of top rect scan line
	mov  di,[StartX]
	mov  bx,di
	sar  di,2             ;X/4 = offset of first rectangle pixel in scan
	add  di,ax                    ;offset of first rectangle pixel in page
	add  di,[PageBase]            ;offset of first rectangle pixel in
                                      ; display memory
	and  bx,0003h                 ;look up left edge plane mask
	mov  ah,LeftClipPlaneMask[bx] ; to clip
	mov  bx,[EndX]
	and  bx,0003h                  ;look up right edge plane
	mov  al,RightClipPlaneMask[bx] ; mask to clip
	mov  bx,ax                     ;put the masks in BX

	mov  cx,[EndX]                 ;calculate # of addresses across rect
	mov  ax,[StartX]
	cmp  cx,ax
	jle  @@FillDone                ;skip if 0 or negative width
	dec  cx
	and  ax,not 011b
	sub  cx,ax
	sar  cx,2                 ;# of addresses across rectangle to fill - 1
	jnz  @@MasksSet           ;there's more than one pixel to draw
	and  bh,bl                ;there's only one pixel, so combine the left
                                  ; and right edge clip masks
@@MasksSet:
	mov  ax,[EndY]
	sub  ax,[StartY]          ;AX = height of rectangle
	jle  @@FillDone           ;skip if 0 or negative height
	mov  [Height],ax
	mov  ax,[_ScrnLogicalByteWidth]
	sub  ax,cx                ;distance from end of one scan line to start
	dec  ax                   ; of next
	mov  [NextScanOffset],ax
	mov  [RectAddrWidth],cx   ;remember width in addresses - 1
	mov  dx,SC_INDEX+1        ;point to Sequence Controller Data reg
                                  ; (SC Index still points to Map Mask)
@@FillRowsLoop:
	mov  cx,[RectAddrWidth]   ;width across - 1
	mov  al,es:[si]           ;read display memory to latch this scan
                                  ; line's pattern
	inc  si                   ;point to the next pattern scan line, wrapping
	jnz  short @@NoWrap       ; back to the start of the pattern if
	sub  si,4                 ; we've run off the end
@@NoWrap:
	mov  al,bh                ;put left-edge clip mask in AL
	out  dx,al                ;set the left-edge plane (clip) mask
        stosb                     ;draw the left edge (pixels come from latches;
                                  ; value written by CPU doesn't matter)
	dec  cx                   ;count off left edge address
	js   @@FillLoopBottom     ;that's the only address
	jz   @@DoRightEdge        ;there are only two addresses
	mov  al,00fh              ;middle addresses drawn 4 pixels at a pop
	out  dx,al                ;set middle pixel mask to no clip
	rep  stosb                ;draw middle addresses four pixels apiece
                                  ; (from latches; value written doesn't matter)
@@DoRightEdge:
	mov  al,bl                ;put right-edge clip mask in AL
	out  dx,al                ;set the right-edge plane (clip) mask
        stosb                     ;draw the right edge (from latches; value
                                  ; written doesn't matter)
@@FillLoopBottom:
	add  di,[NextScanOffset]  ;point to the start of the next scan
				  ; line of the rectangle
	dec  word ptr [Height]    ;count down scan lines
	jnz  @@FillRowsLoop
@@FillDone:
	mov  dx,GC_INDEX+1        ;restore the bit mask to its default,
	mov  al,0ffh              ; which selects all bits from the CPU
	out  dx,al                ; and none from the latches (the GC
				  ; Index still points to Bit Mask)

	pop  di                   ;restore caller's register variables
	pop  si
	mov  sp,bp                ;discard storage for local variables
	pop  bp                   ;restore caller's stack frame
        ret
_x_rect_pattern endp

;-----------------------------------------------------------------------
; Mode X (320x240, 256 colors) display memory to display memory copy
; routine. Left edge of source rectangle modulo 4 must equal left edge
; of destination rectangle modulo 4. Works on all VGAs. Uses approach
; of reading 4 pixels at a time from the source into the latches, then
; writing the latches to the destination. Copies up to but not
; including the column at SrcEndX and the row at SrcEndY. No
; clipping is performed. Results are not guaranteed if the source and
; destination overlap.
;
;
; Based on code originally published in DDJ Mag by M. Abrash
;
;C near-callable as:
;    void x_cp_vid_rect(int SrcStartX, int SrcStartY,
;       int SrcEndX, int SrcEndY, int DestStartX,
;       int DestStartY, unsigned int SrcPageBase,
;       unsigned int DestPageBase, int SrcBitmapWidth,
;       int DestBitmapWidth);

_x_cp_vid_rect proc
	ARG SrcStartX:word,SrcStartY:word,SrcEndX:word,SrcEndY:word,DestStartX:word,DestStartY:word,SrcPageBase:word,DestPageBase:word,SrcBitmapW:word,DestBitmapW:word
	LOCAL SrcNextOffs:word,DestNextOffs:word,RectAddrW:word,Height:word=LocalStk
	push    bp                  ;preserve caller's stack frame
	mov     bp,sp               ;point to local stack frame
	sub     sp,LocalStk         ;allocate space for local vars
	push    si                  ;preserve caller's register variables
	push    di
        push    ds

        cld
	mov     dx,GC_INDEX         ;set the bit mask to select all bits
	mov     ax,00000h+BIT_MASK  ; from the latches and none from
	out dx,ax                   ; the CPU, so that we can write the
				    ; latch contents directly to memory
	mov     ax,SCREEN_SEG       ;point ES to display memory
        mov     es,ax
	mov     ax,[DestBitmapW]
	shr     ax,2                ;convert to width in addresses
	mul     [DestStartY]        ;top dest rect scan line
	mov     di,[DestStartX]
	sar     di,2                ;X/4 = offset of first dest rect pixel in
				    ; scan line
	add     di,ax               ;offset of first dest rect pixel in page
	add     di,[DestPageBase]   ;offset of first dest rect pixel
				    ; in display memory
	mov     ax,[SrcBitmapW]
	sar     ax,2                ;convert to width in addresses
	mul     [SrcStartY]      ;top source rect scan line
	mov     si,[SrcStartX]
        mov     bx,si
	sar     si,2              ;X/4 = offset of first source rect pixel in
				  ; scan line
	add     si,ax             ;offset of first source rect pixel in page
	add     si,[SrcPageBase]  ;offset of first source rect
				  ; pixel in display memory
	and     bx,0003h                     ;look up left edge plane mask
	mov     ah,LeftClipPlaneMask[bx]  ; to clip
	mov     bx,[SrcEndX]
	and     bx,0003h                     ;look up right edge plane
	mov     al,RightClipPlaneMask[bx] ; mask to clip
	mov     bx,ax                        ;put the masks in BX

	mov     cx,[SrcEndX]              ;calculate # of addresses across
	mov     ax,[SrcStartX]            ; rect
        cmp     cx,ax
	jle     @@CopyDone                   ;skip if 0 or negative width
        dec     cx
        and     ax,not 011b
        sub     cx,ax
	sar     cx,2             ;# of addresses across rectangle to copy - 1
	jnz     @@MasksSet       ;there's more than one address to draw
	and     bh,bl            ;there's only one address, so combine the left
				 ; and right edge clip masks
@@MasksSet:
	mov     ax,[SrcEndY]
	sub     ax,[SrcStartY]     ;AX = height of rectangle
	jle     @@CopyDone         ;skip if 0 or negative height
	mov     [Height],ax
	mov     ax,[DestBitmapW]
	sar     ax,2               ;convert to width in addresses
	sub     ax,cx              ;distance from end of one dest scan line
	dec     ax                 ; to start of next
	mov     [DestNextOffs],ax
	mov     ax,[SrcBitmapW]
	sar     ax,2               ;convert to width in addresses
	sub     ax,cx              ;distance from end of one source scan line
	dec     ax                 ; to start of next
	mov     [SrcNextOffs],ax
	mov     [RectAddrW],cx     ;remember width in addresses - 1
	mov     dx,SC_INDEX+1      ;point to Sequence Controller Data reg
				   ; (SC Index still points to Map Mask)
	mov     ax,es              ;DS=ES=screen segment for MOVS
        mov     ds,ax
@@CopyRowsLoop:
	mov     cx,[RectAddrW]     ;width across - 1
	mov     al,bh              ;put left-edge clip mask in AL
	out     dx,al              ;set the left-edge plane (clip) mask
	movsb                      ;copy the left edge (pixels go through
				   ; latches)
	dec     cx                 ;count off left edge address
	js      @@CopyLoopBottom   ;that's the only address
	jz      @@DoRightEdge      ;there are only two addresses
	mov     al,00fh            ;middle addresses are drawn 4 pix per go
	out     dx,al              ;set the middle pixel mask to no clip
	rep     movsb              ;draw the middle addresses four pix per go
				   ; (pixels copied through latches)
@@DoRightEdge:
        mov     al,bl   ;put right-edge clip mask in AL
        out     dx,al   ;set the right-edge plane (clip) mask
        movsb           ;draw the right edge (pixels copied through
                        ; latches)
@@CopyLoopBottom:
	add     si,[SrcNextOffs]   ;point to the start of
	add     di,[DestNextOffs]  ; next source & dest lines
	dec     word ptr [Height]  ;count down scan lines
	jnz     @@CopyRowsLoop
@@CopyDone:
	mov     dx,GC_INDEX+1   ;restore the bit mask to its default,
        mov     al,0ffh         ; which selects all bits from the CPU
	out     dx,al           ; and none from the latches (the GC
                                ; Index still points to Bit Mask)
        pop     ds
	pop     di              ;restore caller's register variables
        pop     si
	mov     sp,bp           ;discard storage for local variables
	pop     bp              ;restore caller's stack frame
        ret
_x_cp_vid_rect  endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Copy a rectangular region of a VGA screen, with x coordinates
; rounded to the nearest byte -- source and destination may overlap.
;
; C near-callable as:
;
; void x_shift_rect (WORD SrcLeft, WORD SrcTop,
;                    WORD SrcRight, WORD SrcBottom,
;                    WORD DestLeft, WORD DestTop, WORD ScreenOffs);
;
; SrcRight is rounded up, and the left edges are rounded down, to ensure
; that the pixels pointed to by the arguments are inside the rectangle.
;
; The width of the rectangle in bytes (width in pixels / 4)
; cannot exceed 255.
;
; ax, bx, cx, dx, and es eat hot lead.
;
; This function was written by Matthew MacKenzie
; matm@eng.umd.edu

	align   2
_x_shift_rect proc
ARG     SrcLeft,SrcTop,SrcRight,SrcBottom,DestLeft,DestTop,ScreenOffs:word
LOCAL   width_temp:word=LocalStk

	push bp
	mov bp, sp
	sub sp, LocalStk
	push si
	push di
	push ds

	; find values for width & x motion
	mov si, SrcLeft     ; source x in bytes
	sar si, 2

	mov di, DestLeft    ; destination x in bytes
	sar di, 2

	mov bx, SrcRight    ; right edge of source in bytes, rounded up
	add bx, 3
	sar bx, 2
	sub bx, si
	mov ax, bx          ; width - 1
	inc bx              ; we'll use this as an offset for moving up or down
	mov width_temp, bx

	cld                 ; by default, strings increment

	cmp si, di
	jge @@MovingLeft

; we're moving our rectangle right, so we copy it from right to left
	add si, ax          ; source & destination will start on the right edge
	add di, ax
	neg bx
	std                 ; strings decrement

@@MovingLeft:

; find values for height & y motion
	mov cx, _ScrnLogicalByteWidth ; bytes to move to advance one line
	mov ax, SrcTop
	mov dx, DestTop     ; default destination y
	cmp ax, dx
	jge @@MovingUp

; we're moving our rectangle down, so we copy it from bottom to top
	mov ax, SrcBottom   ; source starts at bottom
	add dx, ax          ; add (height - 1) to destination y
	sub dx, SrcTop
	neg cx              ; advance up screen rather than down

@@MovingUp:
	push dx             ; save destination y during multiply
	mul _ScrnLogicalByteWidth
	add si, ax          ; add y in bytes to source
	pop ax              ; restore destination y
	mul _ScrnLogicalByteWidth
	add di, ax          ; add y in bytes to destination

	sub cx, bx          ; final value for moving up or down

	add si, ScreenOffs  ; source & destination are on the same screen
	add di, ScreenOffs

	mov dx, SC_INDEX    ; set map mask to all four planes
	mov ax, 00f02h
	out dx, ax

	mov dx, GC_INDEX    ; set bit mask to take data from latches
	mov ax, BIT_MASK    ;  rather than CPU
	out dx, ax

	mov ax, SCREEN_SEG  ; source and destination are VGA memory
	mov es, ax
	mov ds, ax

	mov ah, byte ptr width_temp ; width in bytes should fit in 8 bits

	mov bx, SrcBottom   ; height - 1
	sub bx, SrcTop

	mov dx, cx          ; bytes to add to advance one line

	xor ch, ch          ; ready to rock

@@LineLoop:
	mov cl, ah          ; load width in bytes
	rep movsb           ; move 4 pixels at a time using latches (YOW!)

	add si, dx          ; advance source by one line
	add di, dx          ; advance destination by one line

	dec bx              ; line counter
	jge @@LineLoop      ; 0 still means one more to go

	mov dx, GC_INDEX + 1; set bit mask to take data from CPU (normal setting)
	mov al, 0ffh
	out dx, al

; kick
	pop ds
	pop di
	pop si
	mov sp, bp
	pop bp

	ret
_x_shift_rect endp

	end

