.387
		PUBLIC	MXSTRETCHIMAGE
		EXTRN	SUBCLIPBOX:BYTE
		EXTRN	MX_BYTESPERLINE:BYTE
		EXTRN	MX_VIDEOSEGMENT:BYTE
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
L$1:
    DW	offset L$8
    DW	offset L$10
    DW	offset L$12
    DW	offset L$14
    DW	offset L$16
    DW	offset L$19
MXSTRETCHIMAGE:
	push		bp
	mov		bp,sp
	sub		sp,14H
	push		ds
	push		si
	push		es
	push		di
	xor		dx,dx
	mov		ax,word ptr 0eH[bp]
	mov		bx,word ptr 0aH[bp]
	div		bx
	mov		word ptr -0aH[bp],ax
	xor		ax,ax
	div		bx
	mov		word ptr -0cH[bp],ax
	xor		dx,dx
	mov		ax,word ptr 0cH[bp]
	mov		bx,word ptr 8[bp]
	div		bx
	mov		word ptr -0eH[bp],ax
	xor		ax,ax
	div		bx
	mov		word ptr -10H[bp],ax
	mov		bx,word ptr 12H[bp]
	mov		ax,word ptr 10H[bp]
	mov		cx,word ptr 0aH[bp]
	mov		dx,word ptr 8[bp]
	call		near ptr MX_TEXT:SUBCLIPBOX
	jae		L$2
	jmp		near ptr L$7
L$2:
	mov		word ptr 0aH[bp],cx
	mov		word ptr 8[bp],dx
	sub		word ptr 12H[bp],bx
	sub		word ptr 10H[bp],ax
	mul		word ptr cs:MX_BYTESPERLINE
	mov		di,bx
	shr		di,1
	shr		di,1
	add		di,ax
	mov		word ptr -2[bp],di
	mov		es,word ptr cs:MX_VIDEOSEGMENT
	and		bl,3
	mov		byte ptr -6[bp],bl
	mov		cl,bl
	mov		al,11H
	shl		al,cl
	mov		byte ptr -14H[bp],al
	mov		ax,word ptr 10H[bp]
	test		ax,ax
	je		L$3
	mov		bx,ax
	mul		word ptr -10H[bp]
	mov		cx,dx
	mov		ax,bx
	mul		word ptr -0eH[bp]
	add		ax,cx
	mul		word ptr 0eH[bp]
	add		word ptr 14H[bp],ax
L$3:
	mov		ax,word ptr 12H[bp]
	test		ax,ax
	je		L$4
	mov		bx,ax
	mul		word ptr -0cH[bp]
	mov		cx,dx
	mov		ax,bx
	mul		word ptr -0aH[bp]
	add		ax,cx
	add		word ptr 14H[bp],ax
L$4:
	mov		ax,word ptr -0eH[bp]
	mul		word ptr 0eH[bp]
	mov		word ptr -0eH[bp],ax
	mov		bx,word ptr 6[bp]
	mov		byte ptr -8[bp],bh
	xor		bh,bh
	cmp		bl,5
	jbe		L$5
	xor		bl,bl
L$5:
	shl		bx,1
	mov		ax,word ptr cs:L$1[bx]
	mov		word ptr -4[bp],ax
	mov		ds,word ptr 16H[bp]
	xor		ax,ax
	mov		word ptr -12H[bp],ax
L$6:
	mov		si,word ptr 14H[bp]
	mov		ah,byte ptr -14H[bp]
	and		ah,0fH
	mov		al,2
	mov		dx,3c4H
	out		dx,ax
	mov		ah,byte ptr -6[bp]
	and		ah,3
	mov		al,4
	mov		dx,3ceH
	out		dx,ax
	mov		cx,word ptr 8[bp]
	mov		di,word ptr -2[bp]
	mov		ah,byte ptr -8[bp]
	xor		bx,bx
	mov		dx,word ptr cs:MX_BYTESPERLINE
	call		word ptr -4[bp]
	inc		byte ptr -6[bp]
	rol		byte ptr -14H[bp],1
	adc		word ptr -2[bp],0
	mov		dx,word ptr -0cH[bp]
	mov		ax,word ptr -0aH[bp]
	add		word ptr -12H[bp],dx
	adc		word ptr 14H[bp],ax
	dec		word ptr 0aH[bp]
	jne		L$6
L$7:
	xor		ax,ax
	pop		di
	pop		es
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	retf		12H
L$8:
	mov		al,byte ptr [si]
	mov		byte ptr es:[di],al
	add		di,dx
	dec		cx
	je		L$9
	add		si,word ptr -0eH[bp]
	add		bx,word ptr -10H[bp]
	jae		L$8
	add		si,word ptr 0eH[bp]
	jmp		L$8
L$9:
	ret
L$10:
	mov		al,byte ptr [si]
	and		byte ptr es:[di],al
	add		di,dx
	dec		cx
	je		L$11
	add		si,word ptr -0eH[bp]
	add		bx,word ptr -10H[bp]
	jae		L$10
	add		si,word ptr 0eH[bp]
	jmp		L$10
L$11:
	ret
L$12:
	mov		al,byte ptr [si]
	or		byte ptr es:[di],al
	add		di,dx
	dec		cx
	je		L$13
	add		si,word ptr -0eH[bp]
	add		bx,word ptr -10H[bp]
	jae		L$12
	add		si,word ptr 0eH[bp]
	jmp		L$12
L$13:
	ret
L$14:
	mov		al,byte ptr [si]
	xor		byte ptr es:[di],al
	add		di,dx
	dec		cx
	je		L$15
	add		si,word ptr -0eH[bp]
	add		bx,word ptr -10H[bp]
	jae		L$14
	add		si,word ptr 0eH[bp]
	jmp		L$14
L$15:
	ret
L$16:
	mov		al,byte ptr [si]
	cmp		al,ah
	je		L$17
	mov		byte ptr es:[di],al
L$17:
	add		di,dx
	dec		cx
	je		L$18
	add		si,word ptr -0eH[bp]
	add		bx,word ptr -10H[bp]
	jae		L$16
	add		si,word ptr 0eH[bp]
	jmp		L$16
L$18:
	ret
L$19:
	mov		al,byte ptr [si]
	add		byte ptr es:[di],al
	add		di,dx
	dec		cx
	je		L$20
	add		si,word ptr -0eH[bp]
	add		bx,word ptr -10H[bp]
	jae		L$19
	add		si,word ptr 0eH[bp]
	jmp		L$19
L$20:
	ret
MX_TEXT		ENDS
		END
