.387
		PUBLIC	MXLINE
		EXTRN	MX_BYTESPERLINE:BYTE
		EXTRN	MX_VIDEOSEGMENT:BYTE
		EXTRN	MX_CLIPX1:BYTE
		EXTRN	MX_CLIPX2:BYTE
		EXTRN	MX_CLIPY1:BYTE
		EXTRN	MX_CLIPY2:BYTE
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
L$1:
    DW	offset L$29
    DW	offset L$32
    DW	offset L$35
    DW	offset L$38
MXLINE:
	push		bp
	mov		bp,sp
	sub		sp,12H
	push		ds
	push		si
	push		di
	mov		ax,word ptr 10H[bp]
	mov		bx,word ptr 0eH[bp]
	mov		cx,word ptr 0cH[bp]
	mov		dx,word ptr 0aH[bp]
	call		near ptr L$8
	jae		L$2
	jmp		near ptr L$7
L$2:
	mov		si,cx
	xchg		ax,si
	sub		ax,si
	jge		L$3
	xchg		cx,si
	xchg		dx,bx
	neg		ax
L$3:
	mov		word ptr -2[bp],ax
	mov		cx,word ptr cs:MX_BYTESPERLINE
	mov		ax,dx
	sub		ax,bx
	jge		L$4
	neg		cx
	neg		ax
L$4:
	mov		word ptr -4[bp],ax
	mov		word ptr -0cH[bp],cx
	mov		ax,bx
	mul		word ptr cs:MX_BYTESPERLINE
	mov		cx,si
	shr		si,1
	shr		si,1
	add		si,ax
	and		cl,3
	mov		ax,1102H
	shl		ah,cl
	mov		byte ptr -12H[bp],ah
	mov		dx,3c4H
	out		dx,ax
	mov		ax,word ptr cs:MX_VIDEOSEGMENT
	mov		ds,ax
	xor		bx,bx
	mov		al,byte ptr 6[bp]
	cmp		al,0
	je		L$5
	and		al,3
	shl		al,1
	shl		al,1
	shl		al,1
	mov		ah,al
	mov		al,3
	mov		dx,3ceH
	out		dx,ax
	inc		bx
	inc		bx
L$5:
	mov		ax,word ptr -2[bp]
	mov		cx,word ptr -4[bp]
	cmp		ax,cx
	jae		L$6
	inc		bx
L$6:
	shl		bx,1
	call		word ptr cs:L$1[bx]
	cmp		byte ptr 6[bp],0
	je		L$7
	mov		ax,3
	mov		dx,3ceH
	out		dx,ax
L$7:
	xor		ax,ax
	pop		di
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	retf		0cH
L$8:
	mov		di,ax
	mov		si,dx
	xor		al,al
	cmp		cx,word ptr cs:MX_CLIPX1
	jge		L$9
	or		al,1
L$9:
	cmp		cx,word ptr cs:MX_CLIPX2
	jle		L$10
	or		al,2
L$10:
	cmp		si,word ptr cs:MX_CLIPY1
	jge		L$11
	or		al,4
L$11:
	cmp		si,word ptr cs:MX_CLIPY2
	jle		L$12
	or		al,8
L$12:
	mov		byte ptr -10H[bp],al
	xor		al,al
	cmp		di,word ptr cs:MX_CLIPX1
	jge		L$13
	or		al,1
L$13:
	cmp		di,word ptr cs:MX_CLIPX2
	jle		L$14
	or		al,2
L$14:
	cmp		bx,word ptr cs:MX_CLIPY1
	jge		L$15
	or		al,4
L$15:
	cmp		bx,word ptr cs:MX_CLIPY2
	jle		L$16
	or		al,8
L$16:
	mov		byte ptr -0eH[bp],al
	mov		ah,byte ptr -10H[bp]
	test		ah,al
	je		L$17
	jmp		near ptr L$28
L$17:
	or		ah,al
	jne		L$18
	jmp		near ptr L$27
L$18:
	mov		ax,cx
	sub		ax,di
	mov		word ptr -0aH[bp],ax
	mov		ax,si
	sub		ax,bx
	mov		word ptr -0cH[bp],ax
	mov		al,byte ptr -0eH[bp]
L$19:
	test		al,al
	jne		L$20
	xchg		di,cx
	xchg		si,bx
	xchg		byte ptr -10H[bp],al
L$20:
	test		al,1
	je		L$21
	mov		ax,word ptr cs:MX_CLIPX1
	sub		ax,di
	mov		di,word ptr cs:MX_CLIPX1
	jmp		L$22
L$21:
	test		al,2
	je		L$23
	mov		ax,word ptr cs:MX_CLIPX2
	sub		ax,di
	mov		di,word ptr cs:MX_CLIPX2
L$22:
	imul		word ptr -0cH[bp]
	idiv		word ptr -0aH[bp]
	add		bx,ax
	mov		al,8
	cmp		bx,word ptr cs:MX_CLIPY2
	jg		L$26
	mov		al,4
	cmp		bx,word ptr cs:MX_CLIPY1
	jl		L$26
	xor		al,al
	jmp		L$26
L$23:
	test		al,4
	je		L$24
	mov		ax,word ptr cs:MX_CLIPY1
	sub		ax,bx
	mov		bx,word ptr cs:MX_CLIPY1
	jmp		L$25
L$24:
	mov		ax,word ptr cs:MX_CLIPY2
	sub		ax,bx
	mov		bx,word ptr cs:MX_CLIPY2
L$25:
	imul		word ptr -0aH[bp]
	idiv		word ptr -0cH[bp]
	add		di,ax
	mov		al,1
	cmp		di,word ptr cs:MX_CLIPX1
	jl		L$26
	mov		al,2
	cmp		di,word ptr cs:MX_CLIPX2
	jg		L$26
	xor		al,al
L$26:
	mov		ah,byte ptr -10H[bp]
	test		ah,al
	jne		L$28
	or		ah,al
	je		L$27
	jmp		near ptr L$19
L$27:
	mov		ax,di
	mov		dx,si
	clc
	ret
L$28:
	stc
	ret
L$29:
	mov		di,ax
	neg		di
	shl		cx,1
	mov		word ptr -6[bp],cx
	mov		cx,ax
	shl		ax,1
	mov		word ptr -8[bp],ax
	mov		al,2
	mov		ah,byte ptr -12H[bp]
	mov		bl,byte ptr 8[bp]
	mov		dx,3c4H
	inc		cx
L$30:
	mov		byte ptr [si],bl
	dec		cx
	je		L$31
	rol		ah,1
	adc		si,0
	out		dx,ax
	add		di,word ptr -6[bp]
	jl		L$30
	add		si,word ptr -0cH[bp]
	sub		di,word ptr -8[bp]
	jmp		L$30
L$31:
	ret
L$32:
	mov		di,cx
	neg		di
	shl		ax,1
	mov		word ptr -6[bp],ax
	mov		ax,cx
	shl		ax,1
	mov		word ptr -8[bp],ax
	mov		bl,byte ptr 8[bp]
	mov		ah,byte ptr -12H[bp]
	mov		al,2
	mov		dx,3c4H
	inc		cx
L$33:
	mov		byte ptr [si],bl
	dec		cx
	je		L$34
	add		si,word ptr -0cH[bp]
	add		di,word ptr -6[bp]
	jl		L$33
	rol		ah,1
	adc		si,0
	out		dx,ax
	sub		di,word ptr -8[bp]
	jmp		L$33
L$34:
	ret
L$35:
	mov		di,ax
	neg		di
	shl		cx,1
	mov		word ptr -6[bp],cx
	mov		cx,ax
	shl		ax,1
	mov		word ptr -8[bp],ax
	mov		al,2
	mov		ah,byte ptr -12H[bp]
	mov		bl,byte ptr 8[bp]
	mov		dx,3c4H
	inc		cx
L$36:
	mov		bh,byte ptr [si]
	mov		byte ptr [si],bl
	dec		cx
	je		L$37
	rol		ah,1
	adc		si,0
	out		dx,ax
	add		di,word ptr -6[bp]
	jl		L$36
	add		si,word ptr -0cH[bp]
	sub		di,word ptr -8[bp]
	jmp		L$36
L$37:
	ret
L$38:
	mov		di,cx
	neg		di
	shl		ax,1
	mov		word ptr -6[bp],ax
	mov		ax,cx
	shl		ax,1
	mov		word ptr -8[bp],ax
	mov		bl,byte ptr 8[bp]
	mov		ah,byte ptr -12H[bp]
	mov		al,2
	mov		dx,3c4H
	inc		cx
L$39:
	mov		bh,byte ptr [si]
	mov		byte ptr [si],bl
	dec		cx
	je		L$40
	add		si,word ptr -0cH[bp]
	add		di,word ptr -6[bp]
	jl		L$39
	rol		ah,1
	adc		si,0
	out		dx,ax
	sub		di,word ptr -8[bp]
	jmp		L$39
L$40:
	ret
MX_TEXT		ENDS
		END
