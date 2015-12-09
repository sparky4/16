.387
		PUBLIC	MXFADEPALETTE
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
	inc		bx
	outsw
	jo		L$5
	jb		L$4
	push		2074H
	sub		byte ptr 29H[bp+di],ah
	and		byte ptr [bx+di],dh
	cmp		word ptr [bx+di],di
	xor		ch,byte ptr [di]
	xor		word ptr [bx+di],di
	cmp		word ptr [si],si
	and		byte ptr 52H[bx+di],al
	push		sp
	dec		cx
	push		bx
	and		byte ptr 2eH[bp+di],dh
	jb		L$1
	insb
	and		byte ptr cs:6cH[bx+di],al
	insb
	and		byte ptr 69H[bp+si],dh
	push		7374H
	and		byte ptr 65H[bp+si],dh
	jae		L$7
	jb		L$9
    DB	65H, 64H, 2eH
MXFADEPALETTE:
	push		bp
	mov		bp,sp
	sub		sp,60aH
	push		si
	push		di
	push		ds
	push		es
	mov		word ptr -608H[bp],1
	mov		ax,word ptr 10H[bp]
	and		ax,0ff00H
L$1:
	je		L$2
	mov		cl,8
	shr		ax,cl
	mov		word ptr -608H[bp],ax
L$2:
	mov		ax,word ptr 10H[bp]
	and		ax,0feH
	mov		cl,1
	shr		ax,cl
	or		ax,ax
	jne		L$3
	mov		ax,30H
L$3:
	mov		word ptr -60aH[bp],ax
L$4:
	inc		ax
	mov		byte ptr -606H[bp],al
	mov		byte ptr -604H[bp],1
    DB	0c6H, 86H, 0feH, 0f9H
L$5:
	add		byte ptr 0e46H[bp+di],cl
	cmp		ax,100H
	jb		L$6
	jmp		near ptr L$12
L$6:
	add		ax,word ptr 0cH[bp]
	cmp		ax,100H
	jbe		L$8
	mov		ax,100H
	sub		ax,word ptr 0eH[bp]
    DB	89H, 46H
L$7:
	or		al,0bH
	????
	jne		L$8
	jmp		near ptr L$12
L$8:
	mov		cx,word ptr 0cH[bp]
	mov		ax,cx
	shl		ax,1
	add		cx,ax
	mov		ax,ss
L$9:
	mov		es,ax
	lea		di,-300H[bp]
	mov		ax,word ptr 0eH[bp]
	mov		si,ax
	shl		ax,1
	add		ax,si
	lds		si,dword ptr 12H[bp]
	add		si,ax
	cld
	shr		cx,1
	rep movsw
	rcl		cx,1
	rep movsb
	test		word ptr 10H[bp],1
	je		L$10
	mov		ax,word ptr -60aH[bp]
	mov		byte ptr -602H[bp],al
	mov		byte ptr -604H[bp],0ffH
	mov		ax,ss
	mov		ds,ax
	mov		es,ax
	lea		di,-300H[bp]
	mov		ax,word ptr 0eH[bp]
	mov		cx,word ptr 0cH[bp]
	call		near ptr L$19
L$10:
	mov		bh,byte ptr 0aH[bp]
	and		bh,3fH
	mov		bl,byte ptr 8[bp]
	and		bl,3fH
	mov		dh,byte ptr 6[bp]
	and		dh,3fH
	mov		dl,byte ptr -602H[bp]
	mov		ax,ss
	mov		ds,ax
	mov		es,ax
L$11:
	mov		ax,word ptr 0cH[bp]
	mov		cx,word ptr -60aH[bp]
	lea		si,-300H[bp]
	lea		di,-600H[bp]
	call		near ptr L$13
	push		bx
	push		dx
	lea		si,-600H[bp]
	mov		ax,word ptr 0eH[bp]
	mov		bx,word ptr 0cH[bp]
	mov		cx,word ptr -608H[bp]
	call		near ptr L$15
	pop		dx
	pop		bx
	add		dl,byte ptr -604H[bp]
	dec		byte ptr -606H[bp]
	jne		L$11
L$12:
	pop		es
	pop		ds
	pop		di
	pop		si
	mov		sp,bp
	pop		bp
	retf		10H
L$13:
	cld
	push		bp
	mov		bp,ax
L$14:
	lodsb
	sub		al,bh
	imul		dl
	idiv		cl
	add		al,bh
	stosb
	lodsb
	sub		al,bl
	imul		dl
	idiv		cl
	add		al,bl
	stosb
	lodsb
	sub		al,dh
	imul		dl
	idiv		cl
	add		al,dh
	stosb
	dec		bp
	jne		L$14
	pop		bp
	ret
L$15:
	mov		ah,al
	mov		dx,3daH
L$16:
	in		al,dx
	test		al,8
	jne		L$16
L$17:
	in		al,dx
	test		al,8
	je		L$17
	loop		L$16
	mov		cx,bx
	mov		dx,3c8H
	mov		al,ah
	out		dx,al
	inc		dx
	cld
	cli
L$18:
	lodsb
	out		dx,al
	lodsb
	out		dx,al
	lodsb
	out		dx,al
	loop		L$18
	sti
	ret
L$19:
	mov		dx,3c7H
	out		dx,al
	inc		dx
	inc		dx
	cld
L$20:
	in		al,dx
	stosb
	in		al,dx
	stosb
	in		al,dx
	stosb
	loop		L$20
	ret
MX_TEXT		ENDS
		END
