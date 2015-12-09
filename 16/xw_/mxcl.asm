		PUBLIC	MXCIRCLE
		EXTRN	MX_VIDEOSEGMENT:BYTE
		EXTRN	MX_CLIPX1:BYTE
		EXTRN	MX_CLIPX2:BYTE
		EXTRN	MX_CLIPY1:BYTE
		EXTRN	MX_CLIPY2:BYTE
		EXTRN	MX_BYTESPERLINE:BYTE
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
MXCIRCLE:
	push		bp
	mov		bp,sp
	sub		sp,2
	push		ds
	push		si
	push		di
	xor		si,si
	mov		di,word ptr 8[bp]
	mov		ax,3
	sub		ax,di
	sub		ax,di
	mov		word ptr -2[bp],ax
	mov		ds,word ptr cs:MX_VIDEOSEGMENT
L$1:
	cmp		si,di
	jg		L$3
	mov		ax,si
	mov		bx,di
	call		near ptr L$4
	mov		ax,si
	neg		ax
	mov		bx,di
	call		near ptr L$4
	mov		ax,si
	mov		bx,di
	neg		bx
	call		near ptr L$4
	mov		ax,si
	neg		ax
	mov		bx,di
	neg		bx
	call		near ptr L$4
	mov		ax,di
	mov		bx,si
	call		near ptr L$4
	mov		ax,di
	neg		ax
	mov		bx,si
	call		near ptr L$4
	mov		ax,di
	mov		bx,si
	neg		bx
	call		near ptr L$4
	mov		ax,di
	neg		ax
	mov		bx,si
	neg		bx
	call		near ptr L$4
	mov		ax,word ptr -2[bp]
	test		ax,ax
	jl		L$2
	mov		ax,di
	shl		ax,1
	shl		ax,1
	sub		ax,4
	sub		word ptr -2[bp],ax
	dec		di
L$2:
	mov		ax,si
	shl		ax,1
	shl		ax,1
	add		ax,6
	add		word ptr -2[bp],ax
	inc		si
	jmp		L$1
L$3:
	xor		ax,ax
	pop		di
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	retf		8
L$4:
	add		bx,word ptr 0cH[bp]
	add		ax,word ptr 0aH[bp]
	cmp		bx,word ptr cs:MX_CLIPX1
	jl		L$5
	cmp		bx,word ptr cs:MX_CLIPX2
	jg		L$5
	cmp		ax,word ptr cs:MX_CLIPY1
	jl		L$5
	cmp		ax,word ptr cs:MX_CLIPY2
	jg		L$5
	mul		word ptr cs:MX_BYTESPERLINE
	mov		cx,bx
	shr		bx,1
	shr		bx,1
	add		bx,ax
	and		cl,3
	mov		ax,102H
	shl		ah,cl
	mov		dx,3c4H
	out		dx,ax
	mov		al,byte ptr 6[bp]
	mov		byte ptr [bx],al
L$5:
	ret
MX_TEXT		ENDS
		END
