;-----------------------------------------------------------------------
; MODULE XLINE
;
; Line drawing functions.
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
include xline.inc

	.code


ModeXAddr       macro
	mov	cl,bl
	push    dx
	mov	dx,[_ScrnLogicalByteWidth]
	mul	dx
	pop	dx
	shr	bx,2
	add	bx,ax
	add     bx,[PgOffs]
	and	cl,3
	endm


;-----------------------------------------------------------------------
; _x_line
;
; Line drawing function for all MODE X 256 Color resolutions
; Based on code from "PC and PS/2 Video Systems" by Richard Wilton.
;
; Compile with Tasm.
; C callable.
;

_x_line 	proc
ARG     x1:word,y1:word,x2:word,y2:word,Color:word,PgOffs:word
LOCAL   vertincr:word,incr1:word,incr2:word,routine:word=LocalStk
	push	bp		; Set up stack frame
	mov	bp,sp
	sub	sp,LocalStk
	push	si
	push	di

	mov	ax,0a000h
	mov	es,ax

	mov	dx,SC_INDEX	; setup for plane mask access

; check for vertical line

	mov	si,[_ScrnLogicalByteWidth]
	mov	cx,[x2]
	sub	cx,[x1]
	jz	VertLine

; force x1 < x2

	jns	L01

	neg	cx

	mov	bx,[x2]
	xchg	bx,[x1]
	mov	[x2],bx

	mov	bx,[y2]
	xchg	bx,[y1]
	mov	[y2],bx

; calc dy = abs(y2 - y1)

L01:
	mov	bx,[y2]
	sub	bx,[y1]
	jnz	short skip
	jmp     HorizLine
skip:		jns	L03

	neg	bx
	neg	si

; select appropriate routine for slope of line

L03:
	mov	[vertincr],si
	mov	[routine],offset LoSlopeLine
	cmp	bx,cx
	jle	L04
	mov	[routine],offset HiSlopeLine
	xchg	bx,cx

; calc initial decision variable and increments

L04:
	shl	bx,1
	mov	[incr1],bx
	sub	bx,cx
	mov	si,bx
	sub	bx,cx
	mov	[incr2],bx

; calc first pixel address

	push	cx
	mov	ax,[y1]
	mov	bx,[x1]
	ModeXAddr
	mov	di,bx
	mov	al,1
	shl	al,cl
	mov	ah,al		; duplicate nybble
	shl	al,4
	add	ah,al
	mov	bl,ah
	pop	cx
	inc	cx
	jmp	[routine]

; routine for verticle lines

VertLine:
	mov	ax,[y1]
	mov	bx,[y2]
	mov	cx,bx
	sub	cx,ax
	jge	L31
	neg	cx
	mov	ax,bx

L31:
	inc	cx
	mov	bx,[x1]
	push	cx
	ModeXAddr

	mov	ah,1
	shl	ah,cl
	mov	al,MAP_MASK
	out	dx,ax
	pop	cx
	mov	ax, word ptr [Color]

; draw the line

L32:
	mov	es:[bx],al
	add	bx,si
	loop	L32
	jmp	Lexit

; routine for horizontal line

HorizLine:
	push	ds

	mov	ax,[y1]
	mov	bx,[x1]
	ModeXAddr

	mov	di,bx     ; set dl = first byte mask
	mov	dl,00fh
	shl	dl,cl

	mov	cx,[x2] ; set dh = last byte mask
	and	cl,3
	mov	dh,00eh
	shl	dh,cl
	not	dh

; determine byte offset of first and last pixel in line

	mov	ax,[x2]
	mov	bx,[x1]

	shr	ax,2     ; set ax = last byte column
	shr	bx,2    ; set bx = first byte column
	mov	cx,ax    ; cx = ax - bx
	sub	cx,bx

	mov	ax,dx    ; mov end byte masks to ax
	mov	dx,SC_INDEX ; setup dx for VGA outs
	mov     bx, [Color]

; set pixels in leftmost byte of line

	or	cx,cx      ; is start and end pt in same byte
	jnz	L42        ; no !
	and	ah,al      ; combine start and end masks
	jmp	short L44

L42:            push    ax
	mov     ah,al
	mov     al,MAP_MASK
	out     dx,ax
	mov	al,bl
	stosb
	dec	cx

; draw remainder of the line

L43:
	mov	ah,0Fh
	mov	al,MAP_MASK
	out	dx,ax
	mov	al,bl
	rep	stosb
	pop     ax

; set pixels in rightmost byte of line

L44:
	mov	al,MAP_MASK
	out	dx, ax
	mov     byte ptr es:[di],bl
	pop	ds
	jmp	short Lexit


; routine for dy >= dx (slope <= 1)

LoSlopeLine:
	mov	al,MAP_MASK
	mov	bh,byte ptr [Color]
L10:
	mov	ah,bl

L11:
	or	ah,bl
	rol	bl,1
	jc	L14

; bit mask not shifted out

	or	si,si
	jns	L12
	add	si,[incr1]
	loop	L11

	out	dx,ax
	mov	es:[di],bh
	jmp	short Lexit

L12:
	add	si,[incr2]
	out	dx,ax
	mov	es:[di],bh
	add	di,[vertincr]
	loop	L10
	jmp	short Lexit

; bit mask shifted out

L14:            out	dx,ax
	mov	es:[di],bh
	inc	di
	or	si,si
	jns	L15
	add	si,[incr1]
	loop	L10
	jmp	short Lexit

L15:
	add	si,[incr2]
	add	di,[vertincr]
	loop	L10
	jmp	short Lexit

; routine for dy > dx (slope > 1)

HiSlopeLine:
	mov	bx,[vertincr]
	mov	al,MAP_MASK
L21:            out	dx,ax
	push	ax
	mov	ax,[Color]
	mov	es:[di],al
	pop	ax
	add	di,bx

L22:
	or	si,si
	jns	L23

	add	si,[incr1]
	loop	L21
	jmp	short Lexit

L23:
	add	si,[incr2]
	rol	ah,1
	adc	di,0
lx21:	loop	L21

; return to caller

Lexit:
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret

_x_line	endp

end
