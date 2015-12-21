.387
		PUBLIC	MXFILLPOLY
		EXTRN	MX_CLIPX1:BYTE
		EXTRN	MX_CLIPX2:BYTE
		EXTRN	MX_CLIPY1:BYTE
		EXTRN	MX_CLIPY2:BYTE
		EXTRN	MX_CODESEGMENT:BYTE
		EXTRN	MX_SCANBUFFER:BYTE
		EXTRN	MX_VIDEOSEGMENT:BYTE
		EXTRN	MX_BYTESPERLINE:BYTE
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
L$1:
	mov		cx,word ptr [si]
	sub		cx,word ptr [bx]
	jg		L$2
	ret
L$2:
	push		bp
	mov		ax,word ptr 2[si]
	mov		bx,word ptr 2[bx]
	sub		ax,bx
	jg		L$4
	jl		L$6
	mov		ax,bx
L$3:
	mov		word ptr es:[di],ax
	add		di,4
	dec		cx
	jne		L$3
	jmp		L$8
L$4:
	cwd
	div		cx
	mov		bp,ax
	xor		ax,ax
	div		cx
	xchg		ax,bx
	mov		dx,8000H
L$5:
	mov		word ptr es:[di],ax
	add		di,4
	add		dx,bx
	adc		ax,bp
	dec		cx
	jne		L$5
	jmp		L$8
L$6:
	neg		ax
	cwd
	div		cx
	mov		bp,ax
	xor		ax,ax
	div		cx
	xchg		ax,bx
	mov		dx,8000H
L$7:
	mov		word ptr es:[di],ax
	add		di,4
	sub		dx,bx
	sbb		ax,bp
	dec		cx
	jne		L$7
L$8:
	pop		bp
	ret
MXFILLPOLY:
	push		bp
	mov		bp,sp
	sub		sp,1eH
	push		ds
	push		si
	push		es
	push		di
	mov		cx,word ptr 10H[bp]
	cmp		cx,3
	jae		L$9
	jmp		near ptr L$36
L$9:
	les		di,dword ptr 0cH[bp]
	lds		si,dword ptr 8[bp]
	mov		word ptr -16H[bp],7fffH
	mov		word ptr -1aH[bp],8000H
	mov		word ptr -18H[bp],7fffH
	mov		word ptr -1eH[bp],8000H
	xor		dx,dx
L$10:
	mov		bx,word ptr es:[di]
	shl		bx,1
	shl		bx,1
	add		bx,si
	mov		ax,word ptr [bx]
	cmp		ax,word ptr -16H[bp]
	jge		L$11
	mov		word ptr -16H[bp],ax
	mov		word ptr -0eH[bp],dx
	mov		word ptr -10H[bp],dx
L$11:
	cmp		ax,word ptr -1aH[bp]
	jle		L$12
	mov		word ptr -1aH[bp],ax
	mov		word ptr -12H[bp],dx
L$12:
	mov		ax,word ptr 2[bx]
	cmp		ax,word ptr -18H[bp]
	jge		L$13
	mov		word ptr -18H[bp],ax
L$13:
	cmp		ax,word ptr -1eH[bp]
	jle		L$14
	mov		word ptr -1eH[bp],ax
L$14:
	inc		di
	inc		dx
	inc		di
	inc		dx
	dec		cx
	jne		L$10
	mov		ax,word ptr -1aH[bp]
	cmp		ax,word ptr cs:MX_CLIPX1
	jge		L$15
	jmp		near ptr L$36
L$15:
	mov		bx,word ptr -16H[bp]
	cmp		bx,word ptr cs:MX_CLIPX2
	jle		L$16
	jmp		near ptr L$36
L$16:
	sub		ax,bx
	jg		L$17
	jmp		near ptr L$36
L$17:
	mov		ax,word ptr -1eH[bp]
	cmp		ax,word ptr cs:MX_CLIPY1
	jge		L$18
	jmp		near ptr L$36
L$18:
	mov		bx,word ptr -18H[bp]
	cmp		bx,word ptr cs:MX_CLIPY2
	jle		L$19
	jmp		near ptr L$36
L$19:
	sub		ax,bx
	jg		L$20
	jmp		near ptr L$36
L$20:
	dec		word ptr 10H[bp]
	shl		word ptr 10H[bp],1
	mov		es,word ptr cs:MX_CODESEGMENT
	mov		ax,offset MX_TEXT:MX_SCANBUFFER
	mov		word ptr -4[bp],ax
	mov		si,word ptr -0eH[bp]
L$21:
	lds		bx,dword ptr 0cH[bp]
	mov		di,word ptr [bx+si]
	dec		si
	dec		si
	test		si,si
	jge		L$22
	mov		si,word ptr 10H[bp]
L$22:
	mov		word ptr -0eH[bp],si
	mov		si,word ptr [bx+si]
	shl		di,1
	shl		di,1
	shl		si,1
	shl		si,1
	lds		bx,dword ptr 8[bp]
	add		si,bx
	add		bx,di
	mov		di,word ptr -4[bp]
	call		near ptr L$1
	mov		word ptr -4[bp],di
	mov		si,word ptr -0eH[bp]
	cmp		si,word ptr -12H[bp]
	jne		L$21
	mov		ax,offset MX_TEXT:MX_SCANBUFFER+2
	mov		word ptr -6[bp],ax
	mov		si,word ptr -10H[bp]
L$23:
	lds		bx,dword ptr 0cH[bp]
	mov		di,word ptr [bx+si]
	inc		si
	inc		si
	cmp		si,word ptr 10H[bp]
	jbe		L$24
	xor		si,si
L$24:
	mov		word ptr -10H[bp],si
	mov		si,word ptr [bx+si]
	shl		di,1
	shl		di,1
	shl		si,1
	shl		si,1
	lds		bx,dword ptr 8[bp]
	add		si,bx
	add		bx,di
	mov		di,word ptr -6[bp]
	call		near ptr L$1
	mov		word ptr -6[bp],di
	mov		si,word ptr -10H[bp]
	cmp		si,word ptr -12H[bp]
	jne		L$23
	mov		si,offset MX_TEXT:MX_SCANBUFFER
	mov		ax,word ptr -16H[bp]
	mov		cx,word ptr -1aH[bp]
	sub		cx,ax
	mov		bx,word ptr cs:MX_CLIPX1
	sub		bx,ax
	jle		L$25
	sub		cx,bx
	add		ax,bx
	mov		word ptr -16H[bp],ax
	shl		bx,1
	shl		bx,1
	add		si,bx
L$25:
	mov		bx,ax
	add		bx,cx
	sub		bx,word ptr cs:MX_CLIPX2
	jle		L$26
	sub		cx,bx
L$26:
	test		cx,cx
	jg		L$27
	jmp		near ptr L$36
L$27:
	mov		word ptr -8[bp],cx
	mov		word ptr -4[bp],si
	mov		ds,word ptr cs:MX_CODESEGMENT
	mov		ax,word ptr -18H[bp]
	cmp		ax,word ptr cs:MX_CLIPY1
	jl		L$28
	mov		ax,word ptr -1eH[bp]
	cmp		ax,word ptr cs:MX_CLIPY2
	jg		L$28
	jmp		L$32
L$28:
	mov		di,cx
	inc		di
	sub		si,4
L$29:
	dec		di
	je		L$32
	add		si,4
	mov		ax,word ptr [si]
	mov		cx,word ptr 2[si]
	mov		dx,word ptr cs:MX_CLIPY2
	cmp		ax,dx
	jg		L$31
	cmp		cx,dx
	jle		L$30
	mov		word ptr 2[si],dx
	mov		bx,cx
	sub		bx,dx
	sub		cx,ax
	jle		L$31
	mov		cx,word ptr 2[si]
L$30:
	mov		dx,word ptr cs:MX_CLIPY1
	cmp		cx,dx
	jl		L$31
	sub		cx,ax
	jle		L$31
	cmp		ax,dx
	jge		L$29
	mov		word ptr [si],dx
	sub		dx,ax
	cmp		cx,dx
	ja		L$29
L$31:
	mov		word ptr [si],0ffffH
	jmp		L$29
L$32:
	mov		es,word ptr cs:MX_VIDEOSEGMENT
	mov		si,word ptr -4[bp]
	mov		cl,byte ptr -16H[bp]
	and		cl,3
	mov		al,11H
	shl		al,cl
	mov		byte ptr -2[bp],al
	shr		word ptr -16H[bp],1
	shr		word ptr -16H[bp],1
L$33:
	mov		ax,word ptr [si]
	test		ax,ax
	js		L$35
	mov		cx,word ptr 2[si]
	sub		cx,ax
	jle		L$35
	mul		word ptr cs:MX_BYTESPERLINE
	add		ax,word ptr -16H[bp]
	mov		di,ax
	mov		ah,byte ptr -2[bp]
	mov		dx,3c4H
	mov		al,2
	out		dx,ax
	mov		ax,word ptr 6[bp]
	mov		dx,word ptr cs:MX_BYTESPERLINE
	shr		cx,1
	jae		L$34
	mov		byte ptr es:[di],al
	add		di,dx
	jcxz		L$35
L$34:
	mov		byte ptr es:[di],al
	add		di,dx
	mov		byte ptr es:[di],al
	add		di,dx
	dec		cx
	jne		L$34
L$35:
	rol		byte ptr -2[bp],1
	adc		word ptr -16H[bp],0
	add		si,4
	dec		word ptr -8[bp]
	jne		L$33
L$36:
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
