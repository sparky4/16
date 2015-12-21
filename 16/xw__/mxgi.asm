.387
		PUBLIC	MXGETIMAGE
		EXTRN	SUBCLIPIMAGE:BYTE
		EXTRN	MX_BYTESPERLINE:BYTE
		EXTRN	MX_VIDEOSEGMENT:BYTE
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
MXGETIMAGE:
	push		bp
	mov		bp,sp
	sub		sp,10H
	push		ds
	push		si
	push		es
	push		di
	mov		bx,word ptr 0cH[bp]
	mov		ax,word ptr 0aH[bp]
	mov		cx,word ptr 8[bp]
	mov		dx,word ptr 6[bp]
	call		near ptr MX_TEXT:SUBCLIPIMAGE
	jae		L$1
	jmp		near ptr L$7
L$1:
	mov		word ptr 6[bp],dx
	add		word ptr 0eH[bp],si
	mul		word ptr cs:MX_BYTESPERLINE
	mov		si,bx
	shr		si,1
	shr		si,1
	add		si,ax
	mov		word ptr -0aH[bp],si
	mov		ds,word ptr cs:MX_VIDEOSEGMENT
	and		bl,3
	mov		byte ptr -0eH[bp],bl
	mov		bx,cx
	shr		bx,1
	shr		bx,1
	and		cl,3
	mov		al,8
	shr		al,cl
	mov		di,6
L$2:
	mov		word ptr -8[bp+di],bx
	shr		al,1
	adc		bx,0
	dec		di
	dec		di
	jge		L$2
	cld
	mov		byte ptr -10H[bp],4
	lea		bx,-8[bp]
	mov		es,word ptr 10H[bp]
	mov		ah,byte ptr -0eH[bp]
L$3:
	cmp		word ptr ss:[bx],0
	je		L$7
	mov		di,word ptr 0eH[bp]
	mov		al,4
	mov		dx,3ceH
	out		dx,ax
	mov		dx,word ptr 6[bp]
	mov		si,word ptr -0aH[bp]
L$4:
	push		si
	push		di
	mov		cx,word ptr ss:[bx]
L$5:
	movsb
	add		di,3
	dec		cx
	jne		L$5
	pop		di
	pop		si
	add		di,word ptr 8[bp]
	add		si,word ptr cs:MX_BYTESPERLINE
	dec		dx
	jne		L$4
	inc		bx
	inc		bx
	inc		ah
	test		ah,4
	je		L$6
	inc		word ptr -0aH[bp]
	and		ah,3
L$6:
	inc		word ptr 0eH[bp]
	dec		byte ptr -10H[bp]
	jne		L$3
L$7:
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
