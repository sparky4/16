.387
		PUBLIC	MXPUTIMAGE
		EXTRN	SUBCLIPIMAGE:BYTE
		EXTRN	MX_BYTESPERLINE:BYTE
		EXTRN	MX_VIDEOSEGMENT:BYTE
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
L$1:
    DW	offset L$2
    DW	offset L$6
    DW	offset L$10
    DW	offset L$14
    DW	offset L$18
    DW	offset L$20
L$2:
	shr		cx,1
	jb		L$5
L$3:
	movsb
	add		si,3
	movsb
	add		si,3
	dec		cx
	jne		L$3
L$4:
	ret
L$5:
	movsb
	add		si,3
	jcxz		L$4
	jmp		L$3
L$6:
	shr		cx,1
	jb		L$9
L$7:
	mov		al,byte ptr [si]
	mov		ah,byte ptr 4[si]
	and		word ptr es:[di],ax
	inc		di
	inc		di
	add		si,8
	dec		cx
	jne		L$7
L$8:
	ret
L$9:
	lodsb
	and		byte ptr es:[di],al
	inc		di
	add		si,3
	jcxz		L$8
	jmp		L$7
L$10:
	shr		cx,1
	jb		L$13
L$11:
	mov		al,byte ptr [si]
	mov		ah,byte ptr 4[si]
	or		word ptr es:[di],ax
	inc		di
	inc		di
	add		si,8
	dec		cx
	jne		L$11
L$12:
	ret
L$13:
	lodsb
	or		byte ptr es:[di],al
	inc		di
	add		si,3
	jcxz		L$12
	jmp		L$11
L$14:
	shr		cx,1
	jb		L$17
L$15:
	mov		al,byte ptr [si]
	mov		ah,byte ptr 4[si]
	xor		word ptr es:[di],ax
	inc		di
	inc		di
	add		si,8
	dec		cx
	jne		L$15
L$16:
	ret
L$17:
	lodsb
	xor		byte ptr es:[di],al
	inc		di
	add		si,3
	jcxz		L$16
	jmp		L$15
L$18:
	mov		al,byte ptr [si]
	cmp		al,ah
	je		L$19
	mov		byte ptr es:[di],al
L$19:
	inc		di
	add		si,4
	dec		cx
	jne		L$18
	ret
L$20:
	mov		al,byte ptr [si]
	add		byte ptr es:[di],al
	inc		di
	add		si,4
	dec		cx
	jne		L$20
	ret
MXPUTIMAGE:
	push		bp
	mov		bp,sp
	sub		sp,14H
	push		ds
	push		si
	push		es
	push		di
	mov		bx,word ptr 0eH[bp]
	mov		ax,word ptr 0cH[bp]
	mov		cx,word ptr 0aH[bp]
	mov		dx,word ptr 8[bp]
	call		near ptr MX_TEXT:SUBCLIPIMAGE
	jae		L$21
	jmp		near ptr L$26
L$21:
	mov		word ptr 8[bp],dx
	add		word ptr 10H[bp],si
	mul		word ptr cs:MX_BYTESPERLINE
	mov		di,bx
	shr		di,1
	shr		di,1
	add		di,ax
	mov		word ptr -0aH[bp],di
	mov		es,word ptr cs:MX_VIDEOSEGMENT
	and		bl,3
	mov		byte ptr -10H[bp],bl
	mov		bx,cx
	shr		bx,1
	shr		bx,1
	and		cl,3
	mov		al,8
	shr		al,cl
	mov		si,6
L$22:
	mov		word ptr -8[bp+si],bx
	shr		al,1
	adc		bx,0
	dec		si
	dec		si
	jge		L$22
	mov		cl,byte ptr -10H[bp]
	mov		al,11H
	shl		al,cl
	mov		byte ptr -14H[bp],al
	mov		bx,word ptr 6[bp]
	mov		byte ptr -12H[bp],bh
	xor		bh,bh
	cmp		bl,5
	jbe		L$23
	xor		bl,bl
L$23:
	shl		bx,1
	mov		ax,word ptr cs:L$1[bx]
	mov		word ptr -0cH[bp],ax
	cld
	mov		byte ptr -0eH[bp],4
	lea		bx,-8[bp]
	mov		ds,word ptr 12H[bp]
L$24:
	cmp		word ptr ss:[bx],0
	je		L$26
	mov		si,word ptr 10H[bp]
	mov		ah,byte ptr -14H[bp]
	and		ah,0fH
	mov		al,2
	mov		dx,3c4H
	out		dx,ax
	mov		ah,byte ptr -10H[bp]
	and		ah,3
	mov		al,4
	mov		dx,3ceH
	out		dx,ax
	mov		dx,word ptr 8[bp]
	mov		di,word ptr -0aH[bp]
L$25:
	push		si
	push		di
	mov		cx,word ptr ss:[bx]
	mov		ah,byte ptr -12H[bp]
	call		word ptr -0cH[bp]
	pop		di
	pop		si
	add		si,word ptr 0aH[bp]
	add		di,word ptr cs:MX_BYTESPERLINE
	dec		dx
	jne		L$25
	inc		bx
	inc		bx
	inc		byte ptr -10H[bp]
	rol		byte ptr -14H[bp],1
	adc		word ptr -0aH[bp],0
	inc		word ptr 10H[bp]
	dec		byte ptr -0eH[bp]
	jne		L$24
L$26:
	xor		ax,ax
	pop		di
	pop		es
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	retf		0eH
MX_TEXT		ENDS
		END
