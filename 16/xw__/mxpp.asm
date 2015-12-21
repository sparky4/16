.387
		PUBLIC	MXGETPIXEL
		PUBLIC	MXPUTPIXEL
		EXTRN	MX_CLIPX1:BYTE
		EXTRN	MX_CLIPX2:BYTE
		EXTRN	MX_CLIPY1:BYTE
		EXTRN	MX_CLIPY2:BYTE
		EXTRN	MX_VIDEOSEGMENT:BYTE
		EXTRN	MX_BYTESPERLINE:BYTE
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
MXGETPIXEL:
	push		bp
	mov		bp,sp
	sub		sp,0
	push		ds
	push		si
	xor		ax,ax
	mov		si,word ptr 8[bp]
	cmp		si,word ptr cs:MX_CLIPX1
	jl		L$1
	cmp		si,word ptr cs:MX_CLIPX2
	jg		L$1
	mov		bx,word ptr 6[bp]
	cmp		bx,word ptr cs:MX_CLIPY1
	jl		L$1
	cmp		bx,word ptr cs:MX_CLIPY2
	jg		L$1
	mov		al,4
	mov		ah,byte ptr 8[bp]
	and		ah,3
	mov		dx,3ceH
	out		dx,ax
	mov		ds,word ptr cs:MX_VIDEOSEGMENT
	mov		ax,bx
	mul		word ptr cs:MX_BYTESPERLINE
	shr		si,1
	shr		si,1
	add		si,ax
	mov		al,byte ptr [si]
	xor		ah,ah
L$1:
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	retf		4
MXPUTPIXEL:
	push		bp
	mov		bp,sp
	sub		sp,0
	push		ds
	push		si
	mov		si,word ptr 0aH[bp]
	cmp		si,word ptr cs:MX_CLIPX1
	jl		L$2
	cmp		si,word ptr cs:MX_CLIPX2
	jg		L$2
	mov		ax,word ptr 8[bp]
	cmp		ax,word ptr cs:MX_CLIPY1
	jl		L$2
	cmp		ax,word ptr cs:MX_CLIPY2
	jg		L$2
	mov		ds,word ptr cs:MX_VIDEOSEGMENT
	mul		word ptr cs:MX_BYTESPERLINE
	shr		si,1
	shr		si,1
	add		si,ax
	mov		cl,byte ptr 0aH[bp]
	and		cl,3
	mov		ax,102H
	shl		ah,cl
	mov		dx,3c4H
	out		dx,ax
	mov		al,byte ptr 6[bp]
	mov		byte ptr [si],al
L$2:
	xor		ax,ax
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	retf		6
MX_TEXT		ENDS
		END
