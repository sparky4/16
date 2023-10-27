;====================================================
;
; TED5 ASM Routines
;
;====================================================
	IDEAL
	P386N
	MODEL	medium,C

extrn	XMSdriver:DWORD
extrn	videomode:WORD,xmshandle:WORD,XMSlookup:WORD
extrn	tilenum:WORD,lasticon:WORD,firsticon:WORD
extrn	CGAlookup:WORD,EGA1lookup:WORD,EGA2lookup:WORD,VGAlookup:WORD

	DATASEG

SCindex	=	3c4h
SCmapmask =	2
GCindex =      	3CEh
GCreadmap =	4
GCmode 	=	5

CombTable dw	CombineCGA,CombineEGA,CombineEGA,CombineVGA
DrawTable dw	DrawTCGA,DrawTEGA1,DrawTEGA2,DrawTVGA
OverTable dw	OverlayCGA,OverlayEGA,OverlayEGA,OverlayVGA

cgacolors db	55h,0aah
egacolors dw	0,0ffffh

oldh	db	0
oldy	dw	0
horzadd dw	0
wsize	dw	1,2,4
hsize	dw	8,16,32
twidth	dw	0
theight	dw	0
bmlen	dw	0
masklen	dw	0

csize	dw	32,64,256
esize	dw	32,128,512
vsize	dw	64,256,1024

cmsize	dw	64,128,512
emsize	dw	40,160,640
vmsize	dw	128,512,2048

	PUBLIC	tdata
tdata	db	2048 dup(?)
tdata1	db	2048 dup(?)

;
; XMS move structure
;
blen	dw	0,0
shandle	dw	0
soff	dd	0
dhandle dw	0
doff	dd    	tdata1


Csparse	dw	csp8,csp16,csp32
Esparse	dw	esp8,esp16,esp32
Vsparse	dw	vsp8,vsp16,vsp32

csp8	db	0ffh,0ffh
	REPT	7
	db	0c0h,0
	ENDM
csp16	db	4 dup (0ffh)
	REPT	15
	db	0c0h
	db	3 dup (0)
	ENDM
csp32	db	8 dup (0ffh)
	REPT	31
	db	0c0h
	db	7 dup(0)
	ENDM

LABEL	esp8	BYTE
	REPT	4
	db      0ffh
	db	7 dup (80h)
	ENDM
LABEL	esp16	BYTE
	REPT	4
	db	0ffh,0ffh
	REPT	15
	db	80h,0
	ENDM
	ENDM
LABEL	esp32	BYTE
	REPT	4
	db	4 dup (0ffh)
	REPT	31
	db	80h,0,0,0
	ENDM
	ENDM

vsp8	db	8 dup(0ffh)
	REPT	7
	db	0ffh,7 dup(0)
	ENDM
vsp16	db	16 dup(0ffh)
	REPT	15
	db	0ffh,15 dup(0)
	ENDM
vsp32	db	32 dup(0ffh)
	REPT	31
	db	0ffh,31 dup(0)
	ENDM


	CODESEG
;====================================================
;
; Combine tiles together (background,foreground,icons)
;
;====================================================
PUBLIC	CombineTiles
PROC	CombineTiles
	ARG	tileb:WORD,tilef:WORD,tilei:WORD,tsize:WORD

	push	si
	push	di

	cld
	mov	bx,[tsize]
	shl	bx,1
	mov	dx,[hsize+bx-2]
	mov	[theight],dx
	mov	cx,[wsize+bx-2]
	mov	[twidth],cx
	mov	ax,[xmshandle]
	mov	[shandle],ax		;set XMS handle

	mov	si,[videomode]
	shl	si,1
	mov	ax,[CombTable+si]
	jmp	ax
;
; Combine CGA tiles
;
LABEL	CombineCGA	PROC

	shl	[twidth],1
	mov	cx,[cmsize+bx-2]
	mov	[bmlen],cx
	mov	ax,[csize+bx-2]		;get length of block to move
	mov	[blen],ax
	sub	cx,ax
	mov	[masklen],cx

;
; Handle the background tiles first!
;
	mov	ax,[tileb]
	cmp	ax,8000h		;solid color?
	jb	@@c1
	neg	ax

	mov	di,ds
	mov	es,di
	mov	di,OFFSET tdata
	mov	bx,ax
	and	bx,1
	mov	al,[cgacolors+bx]

	mov	cx,[blen]
	rep stosb
	jmp	SHORT @@c2

@@c1:
	shl	ax,2
	mov	bx,ax
	mov	es,[XMSlookup]		;(LONG SEG *)
	mov	ax,[es:bx]
	mov	[WORD PTR soff],ax
	mov	ax,[es:bx+2]
	mov	[WORD PTR soff+2],ax
	cmp	ax,-1			;SPARSE TILE?
	je	@@c1a

	mov	ax,OFFSET tdata		;(quickie modification since "doff"
	mov	[WORD doff],ax		; should always point to "tdata1")
	mov	si,OFFSET blen
	mov	ah,11
	call	[DWORD XMSdriver]	;move tile into tdata1
	mov	ax,OFFSET tdata1
	mov	[WORD doff],ax		;ahhh....back to normal

	jmp	@@c2
;
; Copy sparse tile to tdata
;
@@c1a:
	mov	bx,[tsize]
	shl	bx,1
	mov	si,[Csparse+bx-2]
	mov	di,OFFSET tdata
	mov	cx,[blen]
	shr	cx,1			;for WORD copy
	mov	ax,ds
	mov	es,ax
	rep movsw

;
; Handle foreground tiles
;
@@c2:
	mov	bx,[tilef]
	cmp	bx,0
	je	@@c3			;any masked tile?

	shl	bx,2
	mov	es,[XMSlookup]
	mov	ax,[es:bx]
	mov	[WORD PTR soff],ax
	mov	ax,[es:bx+2]
	mov	[WORD PTR soff+2],ax
	cmp	ax,-1			;SPARSE MASKED TILE?
	je	@@c3

	mov	ax,[bmlen]
	mov	[blen],ax		;set masked tile length for XMS copy
	mov	si,OFFSET blen
	mov	ah,11
	call	[DWORD XMSdriver]	;move tile into tdata1
;
; now we have the masked tile in "tdata1".
; time to mask it onto "tdata"
;
	mov	si,OFFSET tdata1
	add	si,[masklen]		;SI = masked tile's tiledata
	mov	di,OFFSET tdata		;DI = tile in "tdata"

	mov	dx,[theight]
	mov	bx,OFFSET tdata1	;BX = mask
@@c2a:
	mov	cx,[twidth]
@@c2b:
	mov	al,[bx]			;get mask byte
	and	[di],al			;and tdata with it
	lodsb				;get tilem byte
	or	[di],al			;OR it onto tdata
	inc	di
	inc	bx
	loop	@@c2b
	dec	dx
	jnz	@@c2a
;
; Handle icons (foreground tiles)
;
@@c3:
	mov	bx,[tilei]
	cmp	bx,[lasticon]
	ja	@@cexit			;any icons?
	cmp	bx,[firsticon]
	jbe	@@cexit

	shl	bx,2
	mov	es,[XMSlookup]
	mov	ax,[es:bx]
	mov	[WORD PTR soff],ax
	mov	ax,[es:bx+2]
	mov	[WORD PTR soff+2],ax

	mov	ax,[bmlen]
	mov	[blen],ax		;set masked tile length for XMS copy
	mov	si,OFFSET blen
	mov	ah,11
	call	[DWORD XMSdriver]	;move tile into tdata1
;
; now we have the masked tile in "tdata1".
; time to mask it onto "tdata"
;
	mov	si,OFFSET tdata1
	add	si,[masklen]		;SI = masked tile's tiledata
	mov	di,OFFSET tdata		;DI = tile in "tdata"

	mov	dx,[theight]
	mov	bx,OFFSET tdata1	;BX = mask
@@c3a:
	mov	cx,[twidth]
@@c3b:
	mov	al,[bx]			;get mask byte
	and	[di],al			;and tdata with it
	lodsb				;get tilem byte
	or	[di],al			;OR it onto tdata
	inc	di
	inc	bx
	loop	@@c3b
	dec	dx
	jnz	@@c3a
@@cexit:
	pop	di
	pop	si
	ret

;
; Combine EGA tiles
;
LABEL	CombineEGA	PROC

	mov	cx,[emsize+bx-2]
	mov	[bmlen],cx
	mov	ax,[esize+bx-2]		;get length of block to move
	mov	[blen],ax
	sub	cx,ax
	mov	[masklen],cx

;
; Handle the background tiles first!
;
	mov	ax,[tileb]
	cmp	ax,8000h		;solid color?
	jb	@@e1
	neg	ax

	mov	di,ds
	mov	es,di
	mov	di,OFFSET tdata
	mov	dx,ax

	mov	cx,[blen]		;plane 0
	shr	cx,3
	mov	bx,dx
	shr	dx,1
	and	bx,1
	shl	bx,1
	mov	ax,[egacolors+bx]
	rep stosw

	mov	cx,[blen]		;plane 1
	shr	cx,3
	mov	bx,dx
	shr	dx,1
	and	bx,1
	shl	bx,1
	mov	ax,[egacolors+bx]
	rep stosw

	mov	cx,[blen]		;plane 2
	shr	cx,3
	mov	bx,dx
	shr	dx,1
	and	bx,1
	shl	bx,1
	mov	ax,[egacolors+bx]
	rep stosw

	mov	cx,[blen]		;plane 3
	shr	cx,3
	mov	bx,dx
	shr	dx,1
	and	bx,1
	shl	bx,1
	mov	ax,[egacolors+bx]
	rep stosw
	jmp	SHORT @@e2

@@e1:
	shl	ax,2
	mov	bx,ax
	mov	es,[XMSlookup]		;(LONG SEG *)
	mov	ax,[es:bx]
	mov	[WORD PTR soff],ax
	mov	ax,[es:bx+2]
	mov	[WORD PTR soff+2],ax
	cmp	ax,-1			;SPARSE TILE?
	je	@@e1a

	mov	ax,OFFSET tdata		;(quickie modification since "doff"
	mov	[WORD doff],ax		; should always point to "tdata1")
	mov	si,OFFSET blen
	mov	ah,11
	call	[DWORD XMSdriver]	;move tile into tdata1
	mov	ax,OFFSET tdata1
	mov	[WORD doff],ax		;ahhh....back to normal

	jmp	@@e2
;
; Copy sparse tile to tdata
;
@@e1a:
	mov	bx,[tsize]
	shl	bx,1
	mov	si,[Esparse+bx-2]
	mov	di,OFFSET tdata
	mov	cx,[blen]
	shr	cx,1			;for WORD copy
	mov	ax,ds
	mov	es,ax
	rep movsw

;
; Handle foreground tiles
;
@@e2:
	mov	bx,[tilef]
	cmp	bx,0
	je	@@e3			;any masked tile?

	shl	bx,2
	mov	es,[XMSlookup]
	mov	ax,[es:bx]
	mov	[WORD PTR soff],ax
	mov	ax,[es:bx+2]
	mov	[WORD PTR soff+2],ax
	cmp	ax,-1			;SPARSE MASKED TILE?
	je	@@e3

	mov	ax,[bmlen]
	mov	[blen],ax		;set masked tile length for XMS copy
	mov	si,OFFSET blen
	mov	ah,11
	call	[DWORD XMSdriver]	;move tile into tdata1
;
; now we have the masked tile in "tdata1".
; time to mask it onto "tdata"
;
	mov	si,OFFSET tdata1
	add	si,[masklen]		;SI = masked tile's tiledata
	mov	di,OFFSET tdata		;DI = tile in "tdata"
	mov	ah,4
@@e2a0:
	mov	dx,[theight]
	mov	bx,OFFSET tdata1	;BX = mask
@@e2a:
	mov	cx,[twidth]
@@e2b:
	mov	al,[bx]			;get mask byte
	and	[di],al			;and tdata with it
	lodsb				;get tilem byte
	or	[di],al			;OR it onto tdata
	inc	di
	inc	bx
	loop	@@e2b
	dec	dx
	jnz	@@e2a
	dec	ah
	jnz	@@e2a0			;do all 4 planes!
;
; Handle icons (foreground tiles)
;
@@e3:
	mov	bx,[tilei]
	cmp	bx,[lasticon]
	ja	@@eexit			;any icons?
	cmp	bx,[firsticon]
	jbe	@@eexit

	shl	bx,2
	mov	es,[XMSlookup]
	mov	ax,[es:bx]
	mov	[WORD PTR soff],ax
	mov	ax,[es:bx+2]
	mov	[WORD PTR soff+2],ax

	mov	ax,[bmlen]
	mov	[blen],ax		;set masked tile length for XMS copy
	mov	si,OFFSET blen
	mov	ah,11
	call	[DWORD XMSdriver]	;move tile into tdata1
;
; now we have the masked tile in "tdata1".
; time to mask it onto "tdata"
;
	mov	si,OFFSET tdata1
	add	si,[masklen]		;SI = masked tile's tiledata
	mov	di,OFFSET tdata		;DI = tile in "tdata"
	mov	ah,4
@@e3a0:
	mov	dx,[theight]
	mov	bx,OFFSET tdata1	;BX = mask
@@e3a:
	mov	cx,[twidth]
@@e3b:
	mov	al,[bx]			;get mask byte
	and	[di],al			;and tdata with it
	lodsb				;get tilem byte
	or	[di],al			;OR it onto tdata
	inc	di
	inc	bx
	loop	@@e3b
	dec	dx
	jnz	@@e3a
	dec	ah
	jnz	@@e3a0			;do all 4 planes!
@@eexit:
	pop	di
	pop	si
	ret

;
; Combine VGA tiles
;
LABEL	CombineVGA	PROC

	shl	[twidth],3
	mov	cx,[vmsize+bx-2]
	mov	[bmlen],cx
	mov	ax,[vsize+bx-2]		;get length of block to move
	mov	[blen],ax
	sub	cx,ax
	mov	[masklen],cx

;
; Handle the background tiles first!
;
	mov	ax,[tileb]
	cmp	ax,8000h		;solid color?
	jb	@@v1
	neg	ax

	mov	di,ds
	mov	es,di
	mov	di,OFFSET tdata
	shl	ax,4
	mov	bx,[blen]
	shr	bx,3

	mov	cx,bx
	rep stosb
	inc	al
	mov	cx,bx
	rep stosb
	inc	al
	mov	cx,bx
	rep stosb
	inc	al
	mov	cx,bx
	rep stosb
	inc	al
	mov	cx,bx
	rep stosb
	inc	al
	mov	cx,bx
	rep stosb
	inc	al
	mov	cx,bx
	rep stosb
	inc	al
	mov	cx,bx
	rep stosb

	jmp	SHORT @@v2

@@v1:
	shl	ax,2
	mov	bx,ax
	mov	es,[XMSlookup]		;(LONG SEG *)
	mov	ax,[es:bx]
	mov	[WORD PTR soff],ax
	mov	ax,[es:bx+2]
	mov	[WORD PTR soff+2],ax
	cmp	ax,-1			;SPARSE TILE?
	je	@@v1a

	mov	ax,OFFSET tdata		;(quickie modification since "doff"
	mov	[WORD doff],ax		; should always point to "tdata1")
	mov	si,OFFSET blen
	mov	ah,11
	call	[DWORD XMSdriver]	;move tile into tdata1
	mov	ax,OFFSET tdata1
	mov	[WORD doff],ax		;ahhh....back to normal

	jmp	@@v2
;
; Copy sparse tile to tdata
;
@@v1a:
	mov	bx,[tsize]
	shl	bx,1
	mov	si,[Vsparse+bx-2]
	mov	di,OFFSET tdata
	mov	cx,[blen]
	shr	cx,1			;for WORD copy
	mov	ax,ds
	mov	es,ax
	rep movsw

;
; Handle foreground tiles
;
@@v2:
	mov	bx,[tilef]
	cmp	bx,0
	je	@@v3			;any masked tile?

	shl	bx,2
	mov	es,[XMSlookup]
	mov	ax,[es:bx]
	mov	[WORD PTR soff],ax
	mov	ax,[es:bx+2]
	mov	[WORD PTR soff+2],ax
	cmp	ax,-1			;SPARSE MASKED TILE?
	je	@@v3

	mov	ax,[bmlen]
	mov	[blen],ax		;set masked tile length for XMS copy
	mov	si,OFFSET blen
	mov	ah,11
	call	[DWORD XMSdriver]	;move tile into tdata1
;
; now we have the masked tile in "tdata1".
; time to mask it onto "tdata"
;
	mov	si,OFFSET tdata1
	add	si,[masklen]		;SI = masked tile's tiledata
	mov	di,OFFSET tdata		;DI = tile in "tdata"

	mov	dx,[theight]
	mov	bx,OFFSET tdata1	;BX = mask
@@v2a:
	mov	cx,[twidth]
@@v2b:
	mov	al,[bx]			;get mask byte
	and	[di],al			;and tdata with it
	lodsb				;get tilem byte
	or	[di],al			;OR it onto tdata
	inc	di
	inc	bx
	loop	@@v2b
	dec	dx
	jnz	@@v2a
;
; Handle icons (foreground tiles)
;
@@v3:
	mov	bx,[tilei]
	cmp	bx,[lasticon]
	ja	@@vexit			;any icons?
	cmp	bx,[firsticon]
	jbe	@@vexit

	shl	bx,2
	mov	es,[XMSlookup]
	mov	ax,[es:bx]
	mov	[WORD PTR soff],ax
	mov	ax,[es:bx+2]
	mov	[WORD PTR soff+2],ax

	mov	ax,[bmlen]
	mov	[blen],ax		;set masked tile length for XMS copy
	mov	si,OFFSET blen
	mov	ah,11
	call	[DWORD XMSdriver]	;move tile into tdata1
;
; now we have the masked tile in "tdata1".
; time to mask it onto "tdata"
;
	mov	si,OFFSET tdata1
	add	si,[masklen]		;SI = masked tile's tiledata
	mov	di,OFFSET tdata		;DI = tile in "tdata"

	mov	dx,[theight]
	mov	bx,OFFSET tdata1	;BX = mask
@@v3a:
	mov	cx,[twidth]
@@v3b:
	mov	al,[bx]			;get mask byte
	and	[di],al			;and tdata with it
	lodsb				;get tilem byte
	or	[di],al			;OR it onto tdata
	inc	di
	inc	bx
	loop	@@v3b
	dec	dx
	jnz	@@v3a
@@vexit:
	pop	di
	pop	si
	ret
ENDP


;====================================================
;
; Draw a tile in any of the video modes
; NOTE: ONLY WORKS WITH TSIZE=2!
;
;====================================================
PUBLIC	DrawTile
PROC	DrawTile
	ARG	x:WORD,y:WORD,tsize:WORD

	push	si
	push	di

	mov	bx,[tsize]
	shl	bx,1
	mov	ah,[BYTE PTR hsize+bx-2]
	mov	[oldh],ah
	mov	cx,[wsize+bx-2]

	mov	bx,[videomode]
	shl	bx,1
	mov	dx,[DrawTable+bx]

	mov	bx,[y]
	shl	bx,1
	jmp	dx

label	DrawTCGA proc

	mov	ax,0b800h
	mov	es,ax
	mov	si,OFFSET tdata
	mov	di,[x]
	shl	di,1
	add	di,[CGAlookup+bx]
	mov	bx,cx
	shl	bx,1
	mov	ah,[oldh]
@@c1:
	mov	cx,bx
	rep movsb
	sub	di,bx
	xor	di,2000h
	mov	cx,bx
	rep movsb
	sub	di,bx
	xor	di,2000h
	add	di,80

	sub	ah,2
	jnz	@@c1

	pop	di
	pop	si
	ret
;
; EGA1/2/3 TILE DRAW ROUTINES
;
label	DrawTEGA1 proc
	mov	[horzadd],38
	mov	di,[EGA1lookup+bx]
	jmp	SHORT DoDrawEGAT
label	DrawTEGA2 proc
	mov	[horzadd],78
	mov	di,[EGA2lookup+bx]
	jmp	SHORT DoDrawEGAT
label	DoDrawEGAT proc
	add	di,[x]
	mov	[oldy],di
	mov	si,OFFSET tdata
	mov	bx,0a000h
	mov	es,bx
	mov	bx,cx
;
; AH : height
; BX : width
; ES:DI : dest
; DS:SI : source
;
	cld
	mov	dx,GCindex
	mov	al,GCmode
	out	dx,al
	inc	dx
	mov	al,0
	out	dx,al
;
; Plane 0
;
	mov	dx,SCindex
	mov	al,SCmapmask
	out	dx,al
	inc	dx
	mov	al,1
	out	dx,al

	mov	dx,[horzadd]
@@1:
	mov	cx,bx
	rep movsb
	add	di,dx
	dec	ah
	jne	@@1
;
; Plane 1
;
	mov	dx,SCindex
	mov	al,SCmapmask
	out	dx,al
	inc	dx
	mov	al,2
	out	dx,al

	mov	dx,[horzadd]
	mov	di,[oldy]
	mov	ah,[oldh]
@@2:
	mov	cx,bx
	rep movsb
	add	di,dx
	dec	ah
	jne	@@2
;
; Plane 2
;
	mov	dx,SCindex
	mov	al,SCmapmask
	out	dx,al
	inc	dx
	mov	al,4
	out	dx,al

	mov	dx,[horzadd]
	mov	di,[oldy]
	mov	ah,[oldh]
@@3:
	mov	cx,bx
	rep movsb
	add	di,dx
	dec	ah
	jne	@@3
;
; Plane 3
;
	mov	dx,SCindex
	mov	al,SCmapmask
	out	dx,al
	inc	dx
	mov	al,8
	out	dx,al

	mov	dx,[horzadd]
	mov	di,[oldy]
	mov	ah,[oldh]
@@4:
	mov	cx,bx
	rep movsb
	add	di,dx
	dec	ah
	jne	@@4

	pop	di
	pop	si
	ret

label	DrawTVGA proc

	mov	ax,0a000h
	mov	es,ax
	mov	si,OFFSET tdata
	mov	di,[x]
	shl	di,3
	add	di,[VGAlookup+bx]
	mov	bx,cx
	shl	bx,2
	mov	ah,[oldh]
@@v1:
	mov	cx,bx
	rep movsw
	add	di,304

	dec	ah
	jnz	@@v1

	pop	di
	pop	si
	ret
ENDP

;////////////////////////////////////////////////////////////////////
;
; TED5 screen scroll routines!
;
;////////////////////////////////////////////////////////////////////
PROC	CopyCGA
PUBLIC	CopyCGA
	ARG	srcx:WORD,srcy:WORD,width:WORD,height:WORD,destx:WORD,desty:WORD
	USES	si,di
	push	ds

	mov	ax,0b800h
	mov	es,ax
	mov	ds,ax
	shl	[width],1
	shl	[destx],1
	shl	[srcx],1

	mov	ax,[srcy]
	cmp	ax,[desty]
	jb	@@1
	ja	@@2
	mov	ax,[srcx]
	cmp	ax,[destx]
	jb	@@0
;
; Copy from sourcex to destx, right
;
	mov	dx,[height]
	mov	bx,[srcy]	;source & dest are the same!
	shl	bx,1
	cld

@@aa:
	mov	si,[ss:CGAlookup+bx]
	mov	di,si
	add	si,[srcx]
	add	di,[destx]
	mov	cx,[width]
	rep movsb
	add	bx,2
	dec	dx
	jnz	@@aa
	jz	@@exit
;
; Copy from end of sourcex to end of destx, left
;
@@0:
	mov	dx,[height]
	mov	bx,[srcy]	;source & dest are the same!
	shl	bx,1

@@0a0:
	mov	si,[ss:CGAlookup+bx]
	mov	di,si
	add	si,[srcx]
	add	si,[width]
	dec	si
	add	di,[destx]
	add	di,[width]
	dec	di
	mov	cx,[width]
	std
	rep movsb
	cld
	add	bx,2
	dec	dx
	jnz	@@0a0
	jz	@@exit
;
; Copy from end of sourcey to end of desty, up
;
@@1:
	mov	ah,[BYTE height]
	mov	dl,[BYTE srcy]
	add	dl,ah
	dec	dl
	mov	dh,[BYTE desty]
	add	dh,ah
	dec	dh
	cld

@@1a0:
	mov	bx,[srcy]
	add	bx,[height]
	dec	bx
	shl	bx,1
	mov	si,[ss:CGAlookup+bx]
	mov	bx,[desty]
	add	bx,[height]
	dec	bx
	shl	bx,1
	mov	di,[ss:CGAlookup+bx]
	add	si,[srcx]
	add	di,[destx]
	mov	cx,[width]
	rep movsb
	dec	[height]
	jnz	@@1a0
	jz	@@exit
;
; Copy from sourcey to desty, down
;
@@2:
	mov	ah,[BYTE height]
	mov	dl,[BYTE srcy]
	mov	dh,[BYTE desty]
	cld

@@2a0:
	mov	bx,[srcy]
	shl	bx,1
	mov	si,[ss:CGAlookup+bx]
	mov	bx,[desty]
	shl	bx,1
	mov	di,[ss:CGAlookup+bx]
	add	si,[srcx]
	add	di,[destx]
	mov	cx,[width]
	rep movsb
	inc	[srcy]
	inc	[desty]
	dec	[height]
	jnz	@@2a0
	jz	@@exit
@@exit:
	pop	ds
	ret
ENDP


PROC	CopyEGA
PUBLIC	CopyEGA
	ARG	srcx:WORD,srcy:WORD,width:WORD,height:WORD,destx:WORD,desty:WORD
	USES	si,di
	push	ds

	mov	dx,GCindex
	mov	ax,GCmode OR 100h
	out	dx,ax

	mov	ax,0a000h
	mov	es,ax
	mov	ds,ax

	mov	ax,[srcy]
	cmp	ax,[desty]
	jb	@@1
	ja	@@2
	mov	ax,[srcx]
	cmp	ax,[destx]
	jb	@@0
;
; Copy from sourcex to destx, right
;
	mov	dx,[height]
	mov	bx,[srcy]	;source & dest are the same!
	shl	bx,1
	cld

	cmp	[ss:videomode],2
	jb	@@a
@@aa:
	mov	si,[ss:EGA2lookup+bx]
	mov	di,si
	add	si,[srcx]
	add	di,[destx]
	mov	cx,[width]
	rep movsb
	add	bx,2
	dec	dx
	jnz	@@aa
	jz	@@exit
@@a:
	mov	si,[ss:EGA1lookup+bx]
	mov	di,si
	add	si,[srcx]
	add	di,[destx]
	mov	cx,[width]
	rep movsb
	add	bx,2
	dec	dx
	jnz	@@a
	jz	@@exit
;
; Copy from end of sourcex to end of destx, left
;
@@0:
	mov	dx,[height]
	mov	bx,[srcy]	;source & dest are the same!
	shl	bx,1

	cmp	[ss:videomode],2
	jb	@@0a
@@0a0:
	mov	si,[ss:EGA2lookup+bx]
	mov	di,si
	add	si,[srcx]
	add	si,[width]
	dec	si
	add	di,[destx]
	add	di,[width]
	dec	di
	mov	cx,[width]
	std
	rep movsb
	cld
	add	bx,2
	dec	dx
	jnz	@@0a0
	jz	@@exit
@@0a:
	mov	si,[ss:EGA1lookup+bx]
	mov	di,si
	add	si,[srcx]
	add	si,[width]
	dec	si
	add	di,[destx]
	add	di,[width]
	dec	di
	mov	cx,[width]
	std
	rep movsb
	cld
	add	bx,2
	dec	dx
	jnz	@@0a
	jz	@@exit
;
; Copy from end of sourcey to end of desty, up
;
@@1:
	mov	ah,[BYTE height]
	mov	dl,[BYTE srcy]
	add	dl,ah
	dec	dl
	mov	dh,[BYTE desty]
	add	dh,ah
	dec	dh
	cld

	cmp	[ss:videomode],2
	jb	@@1a
@@1a0:
	mov	bx,[srcy]
	add	bx,[height]
	dec	bx
	shl	bx,1
	mov	si,[ss:EGA2lookup+bx]
	mov	bx,[desty]
	add	bx,[height]
	dec	bx
	shl	bx,1
	mov	di,[ss:EGA2lookup+bx]
	add	si,[srcx]
	add	di,[destx]
	mov	cx,[width]
	rep movsb
	dec	[height]
	jnz	@@1a0
	jz	@@exit
@@1a:
	mov	bl,dl
	xor	bh,bh
	shl	bx,1
	mov	si,[ss:EGA1lookup+bx]
	mov	bl,dh
	xor	bh,bh
	shl	bx,1
	mov	di,[ss:EGA1lookup+bx]
	add	si,[srcx]
	add	di,[destx]
	mov	cx,[width]
	rep movsb
	dec	dl
	dec	dh
	dec	ah
	jnz	@@1a
	jz	@@exit
;
; Copy from sourcey to desty, down
;
@@2:
	mov	ah,[BYTE height]
	mov	dl,[BYTE srcy]
	mov	dh,[BYTE desty]
	cld

	cmp	[ss:videomode],2
	jb	@@2a
@@2a0:
	mov	bx,[srcy]
	shl	bx,1
	mov	si,[ss:EGA2lookup+bx]
	mov	bx,[desty]
	shl	bx,1
	mov	di,[ss:EGA2lookup+bx]
	add	si,[srcx]
	add	di,[destx]
	mov	cx,[width]
	rep movsb
	inc	[srcy]
	inc	[desty]
	dec	[height]
	jnz	@@2a0
	jz	@@exit
@@2a:
	mov	bl,dl
	xor	bh,bh
	shl	bx,1
	mov	si,[ss:EGA1lookup+bx]
	mov	bl,dh
	xor	bh,bh
	shl	bx,1
	mov	di,[ss:EGA1lookup+bx]
	add	si,[srcx]
	add	di,[destx]
	mov	cx,[width]
	rep movsb
	inc	dl
	inc	dh
	dec	ah
	jnz	@@2a
@@exit:
	pop	ds
	ret
ENDP

PROC	CopyVGA
PUBLIC	CopyVGA
	ARG	srcx:WORD,srcy:WORD,width:WORD,height:WORD,destx:WORD,desty:WORD
	USES	si,di
	push	ds

	mov	ax,0a000h
	mov	es,ax
	mov	ds,ax
	shl	[width],3
	shl	[destx],3
	shl	[srcx],3

	mov	ax,[srcy]
	cmp	ax,[desty]
	jb	@@1
	ja	@@2
	mov	ax,[srcx]
	cmp	ax,[destx]
	jb	@@0
;
; Copy from sourcex to destx, right
;
	mov	dx,[height]
	mov	bx,[srcy]	;source & dest are the same!
	shl	bx,1
	cld

@@aa:
	mov	si,[ss:VGAlookup+bx]
	mov	di,si
	add	si,[srcx]
	add	di,[destx]
	mov	cx,[width]
	rep movsb
	add	bx,2
	dec	dx
	jnz	@@aa
	jz	@@exit
;
; Copy from end of sourcex to end of destx, left
;
@@0:
	mov	dx,[height]
	mov	bx,[srcy]	;source & dest are the same!
	shl	bx,1

@@0a0:
	mov	si,[ss:VGAlookup+bx]
	mov	di,si
	add	si,[srcx]
	add	si,[width]
	dec	si
	add	di,[destx]
	add	di,[width]
	dec	di
	mov	cx,[width]
	std
	rep movsb
	cld
	add	bx,2
	dec	dx
	jnz	@@0a0
	jz	@@exit
;
; Copy from end of sourcey to end of desty, up
;
@@1:
	mov	ah,[BYTE height]
	mov	dl,[BYTE srcy]
	add	dl,ah
	dec	dl
	mov	dh,[BYTE desty]
	add	dh,ah
	dec	dh
	cld

@@1a0:
	mov	bx,[srcy]
	add	bx,[height]
	dec	bx
	shl	bx,1
	mov	si,[ss:VGAlookup+bx]
	mov	bx,[desty]
	add	bx,[height]
	dec	bx
	shl	bx,1
	mov	di,[ss:VGAlookup+bx]
	add	si,[srcx]
	add	di,[destx]
	mov	cx,[width]
	rep movsb
	dec	[height]
	jnz	@@1a0
	jz	@@exit
;
; Copy from sourcey to desty, down
;
@@2:
	mov	ah,[BYTE height]
	mov	dl,[BYTE srcy]
	mov	dh,[BYTE desty]
	cld

@@2a0:
	mov	bx,[srcy]
	shl	bx,1
	mov	si,[ss:VGAlookup+bx]
	mov	bx,[desty]
	shl	bx,1
	mov	di,[ss:VGAlookup+bx]
	add	si,[srcx]
	add	di,[destx]
	mov	cx,[width]
	rep movsb
	inc	[srcy]
	inc	[desty]
	dec	[height]
	jnz	@@2a0
	jz	@@exit
@@exit:
	pop	ds
	ret
ENDP

;////////////////////////////////////////////////////////////////////
;
; OVERLAY routine
;
;////////////////////////////////////////////////////////////////////
PROC	Overlay
PUBLIC	Overlay
	ARG	tsize:WORD

	push	di
	push	si

	mov	di,OFFSET tdata
	mov	bx,[tsize]
	shl	bx,1
	mov	si,[videomode]
	shl	si,1
	mov	ax,[OverTable+si]
	jmp	ax

LABEL	OverlayCGA	PROC

	mov	cx,[csize+bx-2]
	shr	cx,1
	mov	si,[Csparse+bx-2]
@@c1:	lodsw
	or	[di],ax
	add	di,2
	loop	@@c1

	pop	si
	pop	di
	ret

LABEL	OverlayEGA	PROC

	mov	cx,[esize+bx-2]
	shr	cx,1
	mov	si,[Esparse+bx-2]
@@e1:	lodsw
	or	[di],ax
	add	di,2
	loop	@@e1

	pop	si
	pop	di
	ret

LABEL	OverlayVGA	PROC

	mov	cx,[vsize+bx-2]
	shr	cx,1
	mov	si,[Vsparse+bx-2]
@@v1:	lodsw
	or	[di],ax
	add	di,2
	loop	@@v1

	pop	si
	pop	di
	ret

ENDP

END