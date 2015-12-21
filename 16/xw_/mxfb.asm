		PUBLIC	MXFILLBOX
		EXTRN	SUBCLIPBOX:BYTE
		EXTRN	SUBHORIZONTALLINEINFO:BYTE
		EXTRN	MX_VIDEOSEGMENT:BYTE
		EXTRN	MX_BYTESPERLINE:BYTE
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
L$1:
	mov		ah,al
	shr		cx,1
	rep stosw
	rcl		cx,1
	rep stosb
	ret
L$2:
	mov		byte ptr [bx],al
	add		bx,dx
	loop		L$2
	ret
L$3:
	mov		si,di
L$4:
	mov		ah,byte ptr [si]
	mov		byte ptr [si],al
	inc		si
	loop		L$4
	ret
L$5:
	mov		ah,byte ptr [bx]
	mov		byte ptr [bx],al
	add		bx,dx
	loop		L$5
	ret
MXFILLBOX:
	push		bp
	mov		bp,sp
	sub		sp,8
	push		ds
	push		si
	push		es
	push		di
	mov		bx,word ptr 10H[bp]
	mov		ax,word ptr 0eH[bp]
	mov		cx,word ptr 0cH[bp]
	mov		dx,word ptr 0aH[bp]
	call		near ptr MX_TEXT:SUBCLIPBOX
	jae		L$6
	jmp		near ptr L$12
L$6:
	mov		word ptr 0aH[bp],dx
	call		near ptr MX_TEXT:SUBHORIZONTALLINEINFO
	mov		word ptr 0cH[bp],cx
	mov		byte ptr -2[bp],al
	mov		byte ptr -4[bp],ah
	mov		ax,word ptr cs:MX_VIDEOSEGMENT
	mov		es,ax
	mov		ds,ax
	cld
	mov		word ptr -6[bp],offset L$2
	mov		word ptr -8[bp],offset L$1
	mov		ax,word ptr 6[bp]
	cmp		al,3
	ja		L$7
	cmp		al,0
	je		L$7
	shl		al,1
	shl		al,1
	shl		al,1
	mov		ah,al
	mov		al,3
	mov		dx,3ceH
	out		dx,ax
	mov		word ptr -6[bp],offset L$5
	mov		word ptr -8[bp],offset L$3
L$7:
	mov		ah,byte ptr -2[bp]
	or		ah,ah
	je		L$8
	mov		dx,3c4H
	mov		al,2
	out		dx,ax
	mov		dx,word ptr cs:MX_BYTESPERLINE
	mov		cx,word ptr 0aH[bp]
	mov		bx,di
	mov		al,byte ptr 8[bp]
	call		word ptr -6[bp]
	inc		di
L$8:
	mov		cx,word ptr 0cH[bp]
	jcxz		L$10
	mov		dx,3c4H
	mov		ax,0f02H
	out		dx,ax
	mov		al,byte ptr 8[bp]
	mov		bx,di
	mov		dx,word ptr 0aH[bp]
	push		di
L$9:
	mov		di,bx
	call		word ptr -8[bp]
	mov		cx,word ptr 0cH[bp]
	add		bx,word ptr cs:MX_BYTESPERLINE
	dec		dx
	jne		L$9
	pop		di
	add		di,word ptr 0cH[bp]
L$10:
	mov		ah,byte ptr -4[bp]
	or		ah,ah
	je		L$11
	mov		dx,3c4H
	mov		al,2
	out		dx,ax
	mov		dx,word ptr cs:MX_BYTESPERLINE
	mov		cx,word ptr 0aH[bp]
	mov		bx,di
	mov		al,byte ptr 8[bp]
	call		word ptr -6[bp]
L$11:
	mov		dx,3ceH
	mov		ax,3
	out		dx,ax
L$12:
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
