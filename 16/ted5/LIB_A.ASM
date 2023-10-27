;====================================================
;
; Library ASM code
;
;====================================================
	IDEAL
	P386N
	MODEL	medium,C

extrn	xormask:WORD,yshift:WORD,CGAfont:WORD,VGAfont:WORD

	DATASEG

SCindex	=	3c4h
SCmapmask =	2
GCindex =      	3CEh
GCreadmap =	4
GCmode 	=	5

a	=	0
LABEL	CGAlookup WORD
PUBLIC	CGAlookup
	REPT	100
	dw	a,a XOR 2000h
a	=	a+80
	ENDM

a	=	0
LABEL	EGA1lookup WORD
PUBLIC	EGA1lookup
	REPT	200
	dw	a
a	=	a+40
	ENDM

a	=	0
LABEL	EGA2lookup WORD
PUBLIC	EGA2lookup
	REPT	480
	dw	a
a	=	a+80
	ENDM

a	=	0
LABEL	VGAlookup WORD
PUBLIC	VGAlookup
	REPT	200
	dw	a
a	=	a+320
	ENDM


	CODESEG

;====================================================
;
; Draw CGA chars
;
;====================================================
PROC CGAcharout
PUBLIC CGAcharout
ARG	x:WORD,y:WORD,chr:BYTE
USES si,di,ds
	mov	bx,[y]
	shl	bx,1
	shl	bx,1
	shl	bx,1
	shl	bx,1
	mov	di,[CGAlookup+bx]
	mov	ax,[x]
	shl	ax,1
	add	di,ax

	mov	al,[chr]
	xor	ah,ah
	shl	ax,1
	shl	ax,1
	shl	ax,1
	shl	ax,1
	mov	si,ax

	mov	ax,0b800h
	mov	es,ax
	mov	ds,[CGAfont]
	cld
	cmp	[ss:xormask],0
	ja	@@1

	REPT	4
	movsw
	add	di,2000h-2
	movsw
	sub	di,2000h-78
	ENDM
	jmp	@@exit
@@1:
	REPT	4
	lodsw
	xor	ax,0ffffh
	stosw
	add	di,2000h-2
	lodsw
	xor	ax,0ffffh
	stosw
	sub	di,2000h-78
	ENDM
@@exit:
	ret
ENDP

;====================================================
;
; Draw EGA chars
;
;====================================================
PROC EGAcharout
PUBLIC EGAcharout
ARG	x:WORD,y:WORD,chr:BYTE,video:WORD
USES si,di,ds,bp

	mov	dx,GCindex
	mov	ax,GCmode
	out	dx,ax

	mov	bx,[y]
	shl	bx,1
	shl	bx,1
	shl	bx,1
	add	bx,[yshift]
	shl	bx,1

	cmp	[video],1
	jne	@@0
	mov	cx,39
	mov	di,[EGA1lookup+bx]
	jmp	@@1
@@0:
	mov	cx,79
	mov	di,[EGA2lookup+bx]
@@1:
	add	di,[x]

	mov	ax,0a000h
	mov	es,ax
	mov	ds,ax

	mov	si,0f000h
	mov	al,[chr]
	xor	ah,ah
	shl	ax,1
	shl	ax,1
	shl	ax,1
	add	si,ax

	cld
	cmp	[ss:xormask],0
	ja	@@2

	mov	dx,GCindex
	mov	ax,GCmode OR 100h
	out	dx,ax
	mov	dx,SCindex
	mov	ax,SCmapmask OR 0f00h
	out	dx,ax

	REPT	8
	movsb
	add	di,cx
	ENDM
	jmp	@@exit

@@2:
ga	=	100h
gb	=	0
	mov	bp,si
	mov	bx,di
	REPT	4
	mov	dx,SCindex
	mov	ax,SCmapmask OR ga
	out	dx,ax
	mov	dx,GCindex
	mov	ax,GCreadmap OR gb
	out	dx,ax
	REPT	8
	lodsb
	xor	al,0ffh
	stosb
	add	di,cx
	ENDM
	mov	di,bx
	mov	si,bp
ga	=	ga*2
gb	=	gb+100h
	ENDM
@@exit:
	mov	dx,GCindex
	mov	ax,GCmode
	out	dx,ax
	ret
ENDP

;====================================================
;
; Draw VGA chars
;
;====================================================
PROC VGAcharout
PUBLIC VGAcharout
ARG	x:WORD,y:WORD,chr:BYTE
USES si,di,ds

	mov	bx,[y]
	shl	bx,1
	shl	bx,1
	shl	bx,1
	add	bx,[yshift]
	shl	bx,1
	mov	di,[VGAlookup+bx]

	mov	ax,[x]
	shl	ax,1
	shl	ax,1
	shl	ax,1
	add	di,ax

	mov     ah,[chr]
	xor	al,al
	shr	ax,1
	shr	ax,1	;[chr]*64!
	mov	si,ax

	mov	ax,0a000h
	mov	es,ax
	mov	ds,[VGAfont]

	cmp	[ss:xormask],0
	ja	@@1

	REPT	8
	REPT	4
	movsw
	ENDM
	add	di,320-8
	ENDM
	jmp	@@exit
@@1:
	REPT	8
	REPT	4
	lodsw
	xor	ax,0ffffh
	stosw
	ENDM
	add	di,320-8
	ENDM
@@exit:
	ret
ENDP

END
