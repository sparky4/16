;-----------------------------------------------------------------------
; MODULE XCLIPPBM
;
; This module was written by Matthew MacKenzie
; matm@eng.umd.edu
;
; Clipped transfer of planar bitmaps from system memory to video memory.
;
; Compile with TASM.
; C near-callable.
;
; ****** XLIB - Mode X graphics library                ****************
; ******                                               ****************
; ****** Written By Themie Gouthas                     ****************
;
; egg@dstos3.dsto.gov.au
; teg@bart.dsto.gov.au
;-----------------------------------------------------------------------

include xlib.inc
include xclippbm.inc


	.data

	align 2

; global clipping variables
_TopBound       dw      (?)
_BottomBound    dw      (?)
_LeftBound      dw      (?)
_RightBound     dw      (?)

; VGA plane masks
ColumnMask      db      011h,022h,044h,088h

; bit delay timers
LeftDelay       db      0, 1, 2, 4
RightDelay      db      0, 4, 2, 1


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; _x_clip_pbm
;
; C near-callable as:
;
; void x_clip_pbm (int X, int Y, int ScreenOffs, char far * Bitmap);
;
; Bitmap is a planar bitmap, in the regular Xlib format.
;
; ax, bx, cx, and dx go south.

	.code

	public _x_clip_pbm
	align   2
_x_clip_pbm proc
ARG     X:word, Y:word, ScreenOffs:word, Bitmap:dword
LOCAL   left_counter, right_counter,column,clipped_height,clipped_width,screen_pos,bitmap_pos,bitmap_size,VGA_mask,width_copy,height_temp,screen_width:word=LocalStk
; Tasm 1.0 does not allow the \ line continuation
;LOCAL   left_counter:word, right_counter:word, \
;		column:word, clipped_height:word, clipped_width:word, \
;		screen_pos:word, bitmap_pos:word, bitmap_size:word, \
;		VGA_mask:word, width_copy, height_temp:word, \
;		screen_width:word=LocalStk

	push bp
	mov bp, sp
	sub sp, LocalStk
	push si
	push di
	push ds

; sociopathic cases: are the clipping bounds out of order?
	mov ax, _TopBound
	cmp ax, _BottomBound
	jg @@GetOut
	mov ax, _LeftBound
	cmp ax, _RightBound
	jle @@ReasonableAndProper
@@GetOut:				; return a 1 -- no image drawn
	pop ds
	pop di
	pop si
	mov ax, 1
	mov sp, bp
	pop bp
	ret

@@ReasonableAndProper:

; we need to use both the tables in ds and the height and width of the bitmap
	les si, Bitmap

; vertical position
	xor cx, cx
	mov cl, byte ptr es:[si + 1] ; height of bitmap
	xor ax, ax
	mov al, byte ptr es:[si] ; width of bitmap
	mul cx
	mov bitmap_size, ax
	mov ax, Y
	cmp ax, _BottomBound ; top edge below clipping box?
	jg @@GetOut

	mov bx, cx
	add bx, ax
	dec bx              ; bottom edge = Y + height - 1
	cmp bx, _TopBound
	jl @@GetOut
	sub bx, _BottomBound ; bottom margin = bottom edge - BottomBound
	jle @@NoBottomBound
	sub cx, bx          ; clip bottom margin from height
@@NoBottomBound:
	mov bx, _TopBound
	sub bx, ax          ; top margin = TopBound - Y
	jle @@NoTopBound
	add ax, bx          ; top edge = Y + top margin
	sub cx, bx          ; clip top margin from height
	jmp @@KeepMargin
@@NoTopBound:
	xor bx, bx
@@KeepMargin:
	mov clipped_height, cx

	mul _ScrnLogicalByteWidth
	mov di, ax
	add di, ScreenOffs  ; row of upper-left corner of blit

	mov cl, byte ptr es:[si] ; width of bitmap (ch is still 0 from height)
	mov ax, cx
	mul bx
	add si, ax
	add si, 2           ; starting position in bitmap

; horizontal position
	mov width_copy, cx
	mov dx, X
	cmp dx, _RightBound
	jg @@GetOut
	mov dx, 0           ; unclipped value for right delay

	mov ax, cx
	shl ax, 2           ; width in pixels
	add ax, X
	dec ax              ; right edge = X + width in pixels - 1
	cmp ax, _LeftBound
	jl @@GetOut
	sub ax, _RightBound  ; right margin = right edge - RightBound
	jle @@NoRightBound
	mov bx, ax
	and bx, 3
	mov dl, RightDelay[bx]
	shr ax, 2
	sub cx, ax          ; subtract clipped bytes from width
@@NoRightBound:
	mov right_counter, dx
	mov dx, 0           ; unclipped value for left delay
	mov ax, _LeftBound
	sub ax, X           ; left margin = LeftBound - X
	jle @@NoLeftBound
	mov bx, ax
	and bx, 3
	mov dl, LeftDelay[bx]
	add ax, 3
	shr ax, 2           ; left margin/4, rounded up
	sub cx, ax          ; subtract clipped bytes from width
	add si, ax          ; move starting position in bitmap past margin
	add di, ax          ; move starting position on screen past margin
@@NoLeftBound:
	mov left_counter, dx
	mov clipped_width, cx

	mov ax, X           ; add x coordinate to screen position
	mov bx, ax
	sar ax, 2
	add di, ax

	mov dx, SC_INDEX
	mov al, MAP_MASK
	out dx, al
	inc dx

	and bx, 3           ; initial mask
	xor ax, ax
	mov al, ColumnMask[bx]
	mov VGA_mask, ax
	out dx, al          ; initial mask

	mov column, 4
	mov bitmap_pos, si
	mov screen_pos, di
	mov ax, _ScrnLogicalByteWidth
	mov screen_width, ax ; since we move ds

; we've used all our tables, so we can change ds to point to the bitmap,
; and es to point to the screen
	mov ds, word ptr [Bitmap + 2]
	mov ax, SCREEN_SEG
	mov es, ax

	cld                 ; strings go forward
	mov bx, width_copy
	sub bx, clipped_width ; bytes to advance one line in bitmap

; let's actually draw something for a change
@@WritePlane:
	mov ax, clipped_height
	mov height_temp, ax
	mov dx, screen_width
	sub dx, clipped_width ; bytes to advance one line across screen

@@WriteLine:
	mov cx, clipped_width
	shr cx, 1
	rep movsw           ; in they went, two by two...
	adc cx, 0
	rep movsb           ; catch stray last byte, if it's there
	add si, bx          ; advance one bitmap line
	add di, dx          ; advance one screen line

	dec height_temp
	jnz @@WriteLine

	dec column
	jz @@OuttaHere      ; only four columns per customer, please
	mov dx, SC_INDEX + 1
	rol byte ptr VGA_mask, 1
	adc screen_pos, 0   ; add to location if we've wrapped to plane 0
	mov al, byte ptr VGA_mask
	out dx, al          ; set VGA for next column

	shr right_counter, 1
	jnc @@NoRightCounter
	dec clipped_width   ; cut off right edge for later planes
	inc bx
@@NoRightCounter:
	shr left_counter, 1
	jnc @@NoLeftCounter
	inc clipped_width   ; add to left edge for later planes
	dec bx
	dec bitmap_pos
	dec screen_pos
@@NoLeftCounter:
	mov si, bitmap_pos
	add si, bitmap_size
	mov bitmap_pos, si
	mov di, screen_pos
	jmp @@WritePlane

@@OuttaHere:			; return a 0 -- something was inside the boundary
	pop ds
	pop di
	pop si
	mov ax, 0
	mov sp, bp
	pop bp
	ret
_x_clip_pbm endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; _x_clip_masked_pbm
;
; C near-callable as:
;
; void x_clip_masked_pbm (int X, int Y, int ScreenOffs, char far * Bitmap);
;
; Bitmap is a planar bitmap, in the regular Xlib format.
; The width of the bitmap cannot be greater than 90 bytes, however.
; Ain't that just criminal?
;
; ax, bx, cx, and dx go south.


; one branch per pixel is more than enough -- we'll unroll the line-writing
; loop all the way to try to get a little speed (at the expense, as usual,
; of a chunk of memory)

MaskedPoint macro offset
	mov al, [si + offset]
	or al, al
	jz $+6
	mov es:[di + offset], al
	endm

MaskedPointSize     equ     11

	public _x_clip_masked_pbm
	align   2
_x_clip_masked_pbm proc
ARG     X:word, Y:word, ScreenOffs:word, Bitmap:dword
; Tasm 1.0 does not allow the \ line continuation
LOCAL   left_counter,right_counter,column:word,clipped_height,screen_pos,bitmap_pos,bitmap_size,VGA_mask,width_copy,height_temp,screen_width:word=LocalStk
;LOCAL   left_counter:word, right_counter:word, \
;		column:word, clipped_height:word, \
;		screen_pos:word, bitmap_pos:word, bitmap_size:word, \
;		VGA_mask:word, width_copy, height_temp:word, \
;		screen_width:word=LocalStk
	push bp
	mov bp, sp
	sub sp, LocalStk
	push si
	push di
	push ds

; sociopathic cases: are the clipping bounds out of order?
	mov ax, _TopBound
	cmp ax, _BottomBound
	jg @@GetOut
	mov ax, _LeftBound
	cmp ax, _RightBound
	jle @@ReasonableAndProper
@@GetOut:				; return a 1 -- no image drawn
	pop ds
	pop di
	pop si
	mov ax, 1
	mov sp, bp
	pop bp
	ret

@@ReasonableAndProper:

; we need to use both the tables in ds and the height and width of the bitmap
	les si, Bitmap

; vertical position
	xor cx, cx
	mov cl, byte ptr es:[si + 1] ; height of bitmap
	xor ax, ax
	mov al, byte ptr es:[si] ; width of bitmap
	mul cx
	mov bitmap_size, ax
	mov ax, Y
	cmp ax, _BottomBound ; top edge below clipping box?
	jg @@GetOut

	mov bx, cx
	add bx, ax
	dec bx              ; bottom edge = Y + height - 1
	cmp bx, _TopBound
	jl @@GetOut
	sub bx, _BottomBound ; bottom margin = bottom edge - BottomBound
	jle @@NoBottomBound
	sub cx, bx          ; clip bottom margin from height
@@NoBottomBound:
	mov bx, _TopBound
	sub bx, ax          ; top margin = TopBound - Y
	jle @@NoTopBound
	add ax, bx          ; top edge = Y + top margin
	sub cx, bx          ; clip top margin from height
	jmp @@KeepMargin
@@NoTopBound:
	xor bx, bx
@@KeepMargin:
	mov clipped_height, cx

	mul _ScrnLogicalByteWidth
	mov di, ax
	add di, ScreenOffs  ; row of upper-left corner of blit

	mov cl, byte ptr es:[si] ; width of bitmap (ch is still 0 from height)
	mov ax, cx
	mul bx
	add si, ax
	add si, 2           ; starting position in bitmap

; horizontal position
	mov width_copy, cx
	mov dx, X
	cmp dx, _RightBound
	jg @@GetOut
	mov dx, 0           ; unclipped value for right delay

	mov ax, cx
	shl ax, 2           ; width in pixels
	add ax, X
	dec ax              ; right edge = X + width in pixels - 1
	cmp ax, _LeftBound
	jl @@GetOut
	sub ax, _RightBound ; right margin = right edge - RightBound
	jle @@NoRightBound
	mov bx, ax
	and bx, 3
	mov dl, RightDelay[bx]
	shr ax, 2
	sub cx, ax          ; subtract clipped bytes from width
@@NoRightBound:
	mov right_counter, dx
	mov dx, 0           ; unclipped value for left delay
	mov ax, _LeftBound
	sub ax, X           ; left margin = LeftBound - X
	jle @@NoLeftBound
	mov bx, ax
	and bx, 3
	mov dl, LeftDelay[bx]
	add ax, 3
	shr ax, 2           ; left margin/4, rounded up
	sub cx, ax          ; subtract clipped bytes from width
	add si, ax          ; move starting position in bitmap past margin
	add di, ax          ; move starting position on screen past margin
@@NoLeftBound:
	mov left_counter, dx
	mov ax, cx
	imul ax, (-1 * MaskedPointSize)
	add ax, offset @@LineDone + 2
	mov cx, ax          ; jump offset for plotting

	mov ax, X           ; add x coordinate to screen position
	mov bx, ax
	shr ax, 2
	add di, ax

	mov dx, SC_INDEX
	mov al, MAP_MASK
	out dx, al
	inc dx

	and bx, 3           ; initial mask
	xor ax, ax
	mov al, ColumnMask[bx]
	mov VGA_mask, ax
	out dx, al

	mov column, 4
	mov bitmap_pos, si
	mov screen_pos, di
	mov ax, _ScrnLogicalByteWidth
	mov screen_width, ax ; since we move ds

; we've used all our tables, so we can change ds to point to the bitmap,
; and es to point to the screen
	mov ds, word ptr [Bitmap + 2]
	mov ax, SCREEN_SEG
	mov es, ax

	mov bx, width_copy

; let's actually draw something for a change
	mov ax, clipped_height
	mov height_temp, ax
	mov dx, screen_width
	jmp cx

; 90 bottles of beer on the wall...
	PointLoop = 89
	rept 89
		MaskedPoint PointLoop
		PointLoop = PointLoop - 1
	endm
; one bottle of beer...

; final point needs a different branch offset, so we don't use MaskedPoint
	mov al, [si]
	or al, al
	jz @@LineDone
	mov es:[di], al

@@LineDone:
	add si, bx          ; advance one bitmap line
	add di, dx          ; advance one screen line
	dec height_temp
	jz @@PlaneDone
	jmp cx

@@PlaneDone:
	dec column
	jz @@OuttaHere      ; only four columns per customer, please
	mov dx, SC_INDEX + 1
	rol byte ptr VGA_mask, 1
	adc screen_pos, 0   ; move to new column if we've wrapped to plane 0
	mov al, byte ptr VGA_mask
	out dx, al          ; set VGA for next column

	shr right_counter, 1
	jnc @@NoRightCounter
	add cx, MaskedPointSize ; cut off right edge for later planes
@@NoRightCounter:
	shr left_counter, 1
	jnc @@NoLeftCounter
	sub cx, MaskedPointSize ; add to left edge for later planes
	dec bitmap_pos
	dec screen_pos
@@NoLeftCounter:
	mov si, bitmap_pos
	add si, bitmap_size
	mov bitmap_pos, si
	mov di, screen_pos

	mov ax, clipped_height
	mov height_temp, ax
	mov dx, screen_width
	jmp cx

@@OuttaHere:			; return a 0 -- something was inside the boundary
	pop ds
	pop di
	pop si
	mov ax, 0
	mov sp, bp
	pop bp
	ret
_x_clip_masked_pbm endp

	end

