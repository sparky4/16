.387
		PUBLIC	MXBITBLT
		EXTRN	SUBHORIZONTALLINEINFO:BYTE
		EXTRN	MX_VIDEOSEGMENT:BYTE
		EXTRN	MX_BYTESPERLINE:BYTE
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
MXBITBLT:
	push		bp
	mov		bp,sp
	sub		sp,16H
	push		ds
	push		si
	push		es
	push		di
	cmp		word ptr 0cH[bp],0
	je		L$2
	mov		ax,word ptr 10H[bp]
	and		al,3
	mov		dx,word ptr 8[bp]
	and		dl,3
	mov		bx,offset L$10
	cmp		al,dl
	jne		L$1
	mov		bx,offset L$3
L$1:
	call		bx
L$2:
	xor		ax,ax
	pop		di
	pop		es
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	retf		0cH
L$3:
	mov		bx,word ptr 8[bp]
	mov		ax,word ptr 6[bp]
	mov		cx,word ptr 0cH[bp]
	call		near ptr MX_TEXT:SUBHORIZONTALLINEINFO
	mov		byte ptr -14H[bp],al
	mov		byte ptr -16H[bp],ah
	mov		word ptr 0cH[bp],cx
	mov		ax,word ptr cs:MX_VIDEOSEGMENT
	mov		ds,ax
	mov		es,ax
	mov		ax,word ptr 0eH[bp]
	mul		word ptr cs:MX_BYTESPERLINE
	mov		si,word ptr 10H[bp]
	shr		si,1
	shr		si,1
	add		si,ax
	mov		dx,3ceH
	mov		ax,4105H
	out		dx,ax
	cld
	mov		ah,byte ptr -14H[bp]
	or		ah,ah
	je		L$5
	mov		dx,3c4H
	mov		al,2
	out		dx,ax
	mov		ax,word ptr cs:MX_BYTESPERLINE
	dec		ax
	mov		cx,word ptr 0aH[bp]
	push		si
	push		di
L$4:
	movsb
	add		si,ax
	add		di,ax
	dec		cx
	jne		L$4
	pop		di
	pop		si
	inc		si
	inc		di
L$5:
	mov		bx,word ptr 0cH[bp]
	test		bx,bx
	je		L$7
	mov		dx,3c4H
	mov		ax,0f02H
	out		dx,ax
	mov		ax,word ptr cs:MX_BYTESPERLINE
	sub		ax,bx
	mov		dx,word ptr 0aH[bp]
	push		si
	push		di
L$6:
	mov		cx,bx
	rep movsb
	add		si,ax
	add		di,ax
	dec		dx
	jne		L$6
	pop		di
	pop		si
	add		si,bx
	add		di,bx
L$7:
	mov		ah,byte ptr -16H[bp]
	or		ah,ah
	je		L$9
	mov		dx,3c4H
	mov		al,2
	out		dx,ax
	mov		ax,word ptr cs:MX_BYTESPERLINE
	dec		ax
	mov		cx,word ptr 0aH[bp]
L$8:
	movsb
	add		si,ax
	add		di,ax
	dec		cx
	jne		L$8
L$9:
	mov		dx,3ceH
	mov		ax,4005H
	out		dx,ax
	ret
L$10:
	mov		cx,word ptr 0cH[bp]
	mov		bx,cx
	shr		bx,1
	shr		bx,1
	and		cl,3
	mov		al,8
	shr		al,cl
	mov		si,6
L$11:
	mov		word ptr -8[bp+si],bx
	shr		al,1
	adc		bx,0
	dec		si
	dec		si
	jge		L$11
	mov		ax,word ptr cs:MX_VIDEOSEGMENT
	mov		ds,ax
	mov		es,ax
	mov		ax,word ptr 0eH[bp]
	mul		word ptr cs:MX_BYTESPERLINE
	mov		si,word ptr 10H[bp]
	shr		si,1
	shr		si,1
	add		si,ax
	mov		word ptr -0aH[bp],si
	mov		ax,word ptr 6[bp]
	mul		word ptr cs:MX_BYTESPERLINE
	mov		di,word ptr 8[bp]
	shr		di,1
	shr		di,1
	add		di,ax
	mov		word ptr -0cH[bp],di
	mov		ax,word ptr 10H[bp]
	and		al,3
	mov		byte ptr -10H[bp],al
	mov		cx,word ptr 8[bp]
	and		cl,3
	mov		al,11H
	shl		al,cl
	mov		byte ptr -12H[bp],al
	cld
	mov		byte ptr -0eH[bp],4
	lea		bx,-8[bp]
L$12:
	cmp		word ptr ss:[bx],0
	je		L$15
	mov		ah,byte ptr -12H[bp]
	and		ah,0fH
	mov		al,2
	mov		dx,3c4H
	out		dx,ax
	mov		ah,byte ptr -10H[bp]
	mov		al,4
	mov		dx,3ceH
	out		dx,ax
	mov		dx,word ptr 0aH[bp]
	mov		ax,word ptr cs:MX_BYTESPERLINE
	sub		ax,word ptr ss:[bx]
L$13:
	mov		cx,word ptr ss:[bx]
	shr		cx,1
	rep movsw
	rcl		cx,1
	rep movsb
	add		si,ax
	add		di,ax
	dec		dx
	jne		L$13
	inc		bx
	inc		bx
	inc		byte ptr -10H[bp]
	and		byte ptr -10H[bp],3
	jne		L$14
	inc		word ptr -0aH[bp]
L$14:
	rol		byte ptr -12H[bp],1
	adc		word ptr -0cH[bp],0
	mov		si,word ptr -0aH[bp]
	mov		di,word ptr -0cH[bp]
	dec		byte ptr -0eH[bp]
	jne		L$12
L$15:
	ret
MX_TEXT		ENDS
		END
