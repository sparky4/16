		PUBLIC	MXPUTTILE
		PUBLIC	MXTRANSPUTTILE
		EXTRN	MX_BYTESPERLINE:BYTE
		EXTRN	MX_VIDEOSEGMENT:BYTE
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
MXPUTTILE:
	push		bp
	mov		bp,sp
	sub		sp,0
	push		ds
	push		si
	push		es
	push		di
	mov		ax,word ptr 0aH[bp]
	mul		word ptr cs:MX_BYTESPERLINE
	mov		di,word ptr 0cH[bp]
	shr		di,1
	shr		di,1
	add		di,ax
	mov		es,word ptr cs:MX_VIDEOSEGMENT
	lds		si,dword ptr 0eH[bp]
	shr		word ptr 8[bp],1
	shr		word ptr 8[bp],1
	mov		cl,byte ptr 0cH[bp]
	and		cl,3
	mov		ah,11H
	shl		ah,cl
	mov		word ptr 0aH[bp],4
	mov		bx,word ptr cs:MX_BYTESPERLINE
	sub		bx,word ptr 8[bp]
L$1:
	mov		al,2
	mov		dx,3c4H
	out		dx,ax
	mov		word ptr 0cH[bp],di
	mov		dx,word ptr 6[bp]
L$2:
	mov		cx,word ptr 8[bp]
	shr		cx,1
	rep movsw
	rcl		cx,1
	rep movsb
	add		di,bx
	dec		dx
	jne		L$2
	mov		di,word ptr 0cH[bp]
	rol		ah,1
	adc		di,0
	dec		word ptr 0aH[bp]
	jne		L$1
	xor		ax,ax
	pop		di
	pop		es
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	retf		0cH
MXTRANSPUTTILE:
	push		bp
	mov		bp,sp
	sub		sp,0
	push		ds
	push		si
	push		es
	push		di
	mov		ax,word ptr 0aH[bp]
	mul		word ptr cs:MX_BYTESPERLINE
	mov		di,word ptr 0cH[bp]
	shr		di,1
	shr		di,1
	add		di,ax
	mov		es,word ptr cs:MX_VIDEOSEGMENT
	lds		si,dword ptr 0eH[bp]
	shr		word ptr 8[bp],1
	shr		word ptr 8[bp],1
	mov		cl,byte ptr 0cH[bp]
	and		cl,3
	mov		ah,11H
	shl		ah,cl
	mov		word ptr 0aH[bp],4
	mov		bx,word ptr cs:MX_BYTESPERLINE
	sub		bx,word ptr 8[bp]
L$3:
	mov		al,2
	mov		dx,3c4H
	out		dx,ax
	mov		word ptr 0cH[bp],di
	mov		dx,word ptr 6[bp]
L$4:
	mov		cx,word ptr 8[bp]
	jcxz		L$7
L$5:
	mov		al,byte ptr [si]
	test		al,al
	je		L$6
	mov		byte ptr es:[di],al
L$6:
	inc		si
	inc		di
	dec		cx
	jne		L$5
L$7:
	add		di,bx
	dec		dx
	jne		L$4
	mov		di,word ptr 0cH[bp]
	rol		ah,1
	adc		di,0
	dec		word ptr 0aH[bp]
	jne		L$3
	xor		ax,ax
	pop		di
	pop		es
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	retf		0cH
MX_TEXT		ENDS
		END
