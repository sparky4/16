		PUBLIC	MXGOURAUDPOLY
		EXTRN	MX_BYTESPERLINE:BYTE
		EXTRN	MX_CLIPX1:BYTE
		EXTRN	MX_CLIPX2:BYTE
		EXTRN	MX_CLIPY1:BYTE
		EXTRN	MX_CLIPY2:BYTE
		EXTRN	MX_CODESEGMENT:BYTE
		EXTRN	MX_SCANBUFFER:BYTE
		EXTRN	MX_VIDEOSEGMENT:BYTE
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
L$1:
	mov		cx,word ptr [si]
	sub		cx,word ptr [bx]
	jg		L$2
	ret
L$2:
	push		bp
	push		di
	push		cx
	push		ax
	push		dx
	mov		ax,word ptr 2[si]
	mov		bx,word ptr 2[bx]
	sub		ax,bx
	jg		L$4
	jl		L$6
	mov		ax,bx
L$3:
	mov		word ptr es:[di],ax
	add		di,8
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
	add		di,8
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
	add		di,8
	sub		dx,bx
	sbb		ax,bp
	dec		cx
	jne		L$7
L$8:
	pop		bx
	pop		ax
	pop		cx
	pop		di
	sub		ax,bx
	jg		L$10
	jl		L$12
	mov		ah,bl
	mov		al,80H
L$9:
	mov		word ptr es:2[di],ax
	add		di,8
	dec		cx
	jne		L$9
	jmp		L$14
L$10:
	cwd
	div		cx
	mov		bp,ax
	xor		ax,ax
	div		cx
	xchg		ax,bx
	mov		dx,8000H
L$11:
	mov		byte ptr es:3[di],al
	mov		byte ptr es:2[di],dh
	add		di,8
	add		dx,bx
	adc		ax,bp
	dec		cx
	jne		L$11
	jmp		L$14
L$12:
	neg		ax
	cwd
	div		cx
	mov		bp,ax
	xor		ax,ax
	div		cx
	xchg		ax,bx
	mov		dx,8000H
L$13:
	mov		byte ptr es:3[di],al
	mov		byte ptr es:2[di],dh
	add		di,8
	sub		dx,bx
	sbb		ax,bp
	dec		cx
	jne		L$13
L$14:
	pop		bp
	ret
L$15:
	mov		ax,word ptr 6[si]
	mov		bx,word ptr 2[si]
	cmp		ah,bh
	jg		L$17
	jl		L$19
	add		ax,dx
	mov		dx,word ptr cs:MX_BYTESPERLINE
L$16:
	mov		byte ptr es:[di],ah
	add		di,dx
	dec		cx
	jne		L$16
	ret
L$17:
	push		bp
	push		si
	mov		si,bx
	add		si,dx
	sub		ax,bx
	xor		dx,dx
	div		cx
	mov		bp,ax
	xor		ax,ax
	div		cx
	mov		bx,ax
	mov		dx,8000H
	mov		ax,word ptr cs:MX_BYTESPERLINE
	xchg		ax,si
L$18:
	mov		byte ptr es:[di],ah
	add		dx,bx
	adc		ax,bp
	add		di,si
	dec		cx
	jne		L$18
	pop		si
	pop		bp
	ret
L$19:
	push		bp
	push		si
	mov		si,bx
	add		si,dx
	sub		ax,bx
	neg		ax
	xor		dx,dx
	div		cx
	mov		bp,ax
	xor		ax,ax
	div		cx
	mov		bx,ax
	mov		dx,8000H
	mov		ax,word ptr cs:MX_BYTESPERLINE
	xchg		ax,si
L$20:
	mov		byte ptr es:[di],ah
	sub		dx,bx
	sbb		ax,bp
	add		di,si
	dec		cx
	jne		L$20
	pop		si
	pop		bp
	ret
MXGOURAUDPOLY:
	push		bp
	mov		bp,sp
	sub		sp,1eH
	push		ds
	push		si
	push		es
	push		di
	mov		cx,word ptr 14H[bp]
	cmp		cx,3
	jae		L$21
	jmp		near ptr L$47
L$21:
	les		di,dword ptr 10H[bp]
	lds		si,dword ptr 0cH[bp]
	mov		word ptr -16H[bp],7fffH
	mov		word ptr -1aH[bp],8000H
	mov		word ptr -18H[bp],7fffH
	mov		word ptr -1eH[bp],8000H
	xor		dx,dx
L$22:
	mov		bx,word ptr es:[di]
	shl		bx,1
	shl		bx,1
	add		bx,si
	mov		ax,word ptr [bx]
	cmp		ax,word ptr -16H[bp]
	jge		L$23
	mov		word ptr -16H[bp],ax
	mov		word ptr -0eH[bp],dx
	mov		word ptr -10H[bp],dx
L$23:
	cmp		ax,word ptr -1aH[bp]
	jle		L$24
	mov		word ptr -1aH[bp],ax
	mov		word ptr -12H[bp],dx
L$24:
	mov		ax,word ptr 2[bx]
	cmp		ax,word ptr -18H[bp]
	jge		L$25
	mov		word ptr -18H[bp],ax
L$25:
	cmp		ax,word ptr -1eH[bp]
	jle		L$26
	mov		word ptr -1eH[bp],ax
L$26:
	inc		di
	inc		di
	inc		dx
	inc		dx
	dec		cx
	jne		L$22
	mov		ax,word ptr -1aH[bp]
	cmp		ax,word ptr cs:MX_CLIPX1
	jge		L$27
	jmp		near ptr L$47
L$27:
	mov		bx,word ptr -16H[bp]
	cmp		bx,word ptr cs:MX_CLIPX2
	jle		L$28
	jmp		near ptr L$47
L$28:
	sub		ax,bx
	jg		L$29
	jmp		near ptr L$47
L$29:
	mov		ax,word ptr -1eH[bp]
	cmp		ax,word ptr cs:MX_CLIPY1
	jge		L$30
	jmp		near ptr L$47
L$30:
	mov		bx,word ptr -18H[bp]
	cmp		bx,word ptr cs:MX_CLIPY2
	jle		L$31
	jmp		near ptr L$47
L$31:
	sub		ax,bx
	jg		L$32
	jmp		near ptr L$47
L$32:
	dec		word ptr 14H[bp]
	shl		word ptr 14H[bp],1
	mov		es,word ptr cs:MX_CODESEGMENT
	mov		ax,offset MX_TEXT:MX_SCANBUFFER
	mov		word ptr -4[bp],ax
	mov		si,word ptr -0eH[bp]
L$33:
	lds		bx,dword ptr 10H[bp]
	mov		di,word ptr [bx+si]
	dec		si
	dec		si
	test		si,si
	jge		L$34
	mov		si,word ptr 14H[bp]
L$34:
	mov		word ptr -0eH[bp],si
	mov		si,word ptr [bx+si]
	lds		bx,dword ptr 8[bp]
	shl		di,1
	shl		si,1
	mov		ax,word ptr [bx+si]
	mov		dx,word ptr [bx+di]
	lds		bx,dword ptr 0cH[bp]
	shl		si,1
	shl		di,1
	add		si,bx
	add		bx,di
	mov		di,word ptr -4[bp]
	call		near ptr L$1
	mov		word ptr -4[bp],di
	mov		si,word ptr -0eH[bp]
	cmp		si,word ptr -12H[bp]
	jne		L$33
	mov		ax,offset MX_TEXT:MX_SCANBUFFER+4
	mov		word ptr -6[bp],ax
	mov		si,word ptr -10H[bp]
L$35:
	lds		bx,dword ptr 10H[bp]
	mov		di,word ptr [bx+si]
	inc		si
	inc		si
	cmp		si,word ptr 14H[bp]
	jbe		L$36
	xor		si,si
L$36:
	mov		word ptr -10H[bp],si
	mov		si,word ptr [bx+si]
	lds		bx,dword ptr 8[bp]
	shl		di,1
	shl		si,1
	mov		ax,word ptr [bx+si]
	mov		dx,word ptr [bx+di]
	lds		bx,dword ptr 0cH[bp]
	shl		si,1
	shl		di,1
	add		si,bx
	add		bx,di
	mov		di,word ptr -6[bp]
	call		near ptr L$1
	mov		word ptr -6[bp],di
	mov		si,word ptr -10H[bp]
	cmp		si,word ptr -12H[bp]
	jne		L$35
	mov		si,offset MX_TEXT:MX_SCANBUFFER
	mov		ax,word ptr -16H[bp]
	mov		cx,word ptr -1aH[bp]
	sub		cx,ax
	mov		bx,word ptr cs:MX_CLIPX1
	sub		bx,ax
	jle		L$37
	sub		cx,bx
	add		ax,bx
	mov		word ptr -16H[bp],ax
	shl		bx,1
	shl		bx,1
	shl		bx,1
	add		si,bx
L$37:
	mov		bx,ax
	add		bx,cx
	sub		bx,word ptr cs:MX_CLIPX2
	jle		L$38
	sub		cx,bx
L$38:
	test		cx,cx
	jg		L$39
	jmp		near ptr L$47
L$39:
	mov		word ptr -8[bp],cx
	mov		word ptr -4[bp],si
	mov		ds,word ptr cs:MX_CODESEGMENT
	mov		ax,word ptr -18H[bp]
	cmp		ax,word ptr cs:MX_CLIPY1
	jl		L$40
	mov		ax,word ptr -1eH[bp]
	cmp		ax,word ptr cs:MX_CLIPY2
	jg		L$40
	jmp		L$44
L$40:
	mov		di,cx
	inc		di
	sub		si,8
L$41:
	dec		di
	je		L$44
	add		si,8
	mov		ax,word ptr [si]
	mov		cx,word ptr 4[si]
	mov		dx,word ptr cs:MX_CLIPY2
	cmp		ax,dx
	jg		L$43
	cmp		cx,dx
	jle		L$42
	mov		word ptr 4[si],dx
	mov		bx,cx
	sub		bx,dx
	sub		cx,ax
	jle		L$43
	mov		ax,word ptr 2[si]
	sub		ax,word ptr 6[si]
	imul		bx
	idiv		cx
	add		word ptr 6[si],ax
	mov		ax,word ptr [si]
	mov		cx,word ptr 4[si]
L$42:
	mov		dx,word ptr cs:MX_CLIPY1
	cmp		cx,dx
	jl		L$43
	sub		cx,ax
	jle		L$43
	cmp		ax,dx
	jge		L$41
	mov		word ptr [si],dx
	sub		dx,ax
	cmp		cx,dx
	jbe		L$43
	mov		ax,word ptr 6[si]
	sub		ax,word ptr 2[si]
	imul		dx
	idiv		cx
	add		word ptr 2[si],ax
	jmp		L$41
L$43:
	mov		word ptr [si],0ffffH
	jmp		L$41
L$44:
	mov		es,word ptr cs:MX_VIDEOSEGMENT
	mov		si,word ptr -4[bp]
	mov		cl,byte ptr -16H[bp]
	and		cl,3
	mov		al,11H
	shl		al,cl
	mov		byte ptr -2[bp],al
	shr		word ptr -16H[bp],1
	shr		word ptr -16H[bp],1
	mov		ax,word ptr 6[bp]
	mov		ah,al
	xor		al,al
	mov		word ptr 6[bp],ax
L$45:
	mov		ax,word ptr [si]
	test		ax,ax
	js		L$46
	mov		cx,word ptr 4[si]
	sub		cx,ax
	jle		L$46
	mul		word ptr cs:MX_BYTESPERLINE
	add		ax,word ptr -16H[bp]
	mov		di,ax
	mov		ah,byte ptr -2[bp]
	mov		al,2
	mov		dx,3c4H
	out		dx,ax
	mov		dx,word ptr 6[bp]
	call		near ptr L$15
L$46:
	rol		byte ptr -2[bp],1
	adc		word ptr -16H[bp],0
	add		si,8
	dec		word ptr -8[bp]
	jne		L$45
L$47:
	xor		ax,ax
	pop		di
	pop		es
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	retf		10H
MX_TEXT		ENDS
		END
