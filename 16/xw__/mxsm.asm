.387
		PUBLIC	MX_SCREENWIDTH
		PUBLIC	MX_SCREENHEIGHT
		PUBLIC	MX_BYTESPERLINE
		PUBLIC	MXSETMODE
		PUBLIC	MXCHANGEMODE
		PUBLIC	MXGETASPECT
		PUBLIC	MXGETSCREENSIZE
		EXTRN	MX_CODESEGMENT:BYTE
		EXTRN	MXSETSYSCLIPREGION:BYTE
		EXTRN	MX_VIDEOSEGMENT:BYTE
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
MX_SCREENWIDTH:
	add		byte ptr [bx+si],al
MX_SCREENHEIGHT:
	add		byte ptr [bx+si],al
L$1:
	add		byte ptr [bx+si],al
L$2:
	add		byte ptr [bx+si],al
MX_BYTESPERLINE:
	add		byte ptr [bx+si],al
L$3:
	or		word ptr 14H[bx+si],ax
	add		byte ptr [bx],dl
	jcxz		L$4
L$4:
    DB	0
L$5:
	or		word ptr 14H[bx+di],ax
	add		byte ptr [bx],dl
	jcxz		L$6
L$6:
    DB	0
L$7:
	add		byte ptr 1[bx],bl
	dec		di
	add		dl,byte ptr 3[bx+si]
	add		byte ptr [si],54H
	add		ax,1380H
	sub		byte ptr [bx+si],al
    DB	0
L$8:
	add		byte ptr 1[bp+di],ch
	pop		cx
	add		bl,byte ptr 3[bp+si]
	mov		es,word ptr [si]
	pop		si
	add		ax,138aH
	sub		ax,0
	push		es
	mov		di,1f07H
	adc		byte ptr -7aefH[bp+di],al
	adc		bl,byte ptr 15H[di]
	arpl		word ptr ds:[0baH],dx
    DB	0
L$9:
	push		es
    DD	ds:[7bfH]
	pushf
	adc		word ptr -70eeH[bp],cx
	adc		ax,1696H
	mov		cx,0
L$10:
	push		es
	or		ax,3e07H
	adc		dl,ch
	adc		word ptr -20eeH[si],cx
	adc		ax,16e7H
	push		es
	add		byte ptr [bx+si],al
L$11:
	add		byte ptr 1[si],dh
	arpl		word ptr [bp+si],ax
	add		dx,word ptr fs:6804H[bx]
	add		ax,695H
	xchg		byte ptr [bx],al
	lock or		word ptr 0fH[bx+si],sp
	xor		word ptr [bx+si],dx
	pop		bx
	adc		word ptr 5712H[di],cx
	adc		si,word ptr [bp+si]
	adc		al,0
	adc		ax,1660H
	adc		byte ptr [bx],0e3H
	add		byte ptr [bx+si],al
L$12:
	arpl		word ptr ds:[500H],ax
	add		byte ptr 1[bx+si],al
	enter		1a00H,0
    DW	offset L$9
    DW	offset L$5
	add		byte ptr [bx+si],al
	xor		ax,word ptr [bp+di]
L$13:
	jcxz		L$14
    DB	0
L$14:
	add		word ptr [bx+si],ax
	inc		ax
	add		ax,si
	add		byte ptr [bp+si],bl
	add		byte ptr [bp+si],bl
    DW	offset L$5
	add		byte ptr [bx+si],al
	xor		ax,word ptr [bp+di]
L$15:
	arpl		word ptr ds:[0a00H],ax
	add		byte ptr 1[bx+si],al
	nop
	add		word ptr [bp+si],bx
	add		byte ptr [bp+si],cl
    DW	offset L$3
	add		byte ptr [bx+si],al
	xor		ax,word ptr [bp+di]
L$16:
	jcxz		L$17
    DB	0
L$17:
	add		al,byte ptr [bx+si]
	inc		ax
	add		ax,sp
    DD	L$18
L$18:
	add		byte ptr [bp+si],cl
	add		byte ptr [bx+si],al
	add		byte ptr [bp+di],dh
    DB	3
L$19:
	sbb		ax,word ptr [eax]
	adc		al,0
	push		0c801H
	add		byte ptr [bp+si],ch
	add		byte ptr [bp+si],cl
    DW	offset L$5
	add		byte ptr [bx+si],al
	fadd		dword ptr [bp+si]
L$20:
	out		9,ax
	add		byte ptr [bx+si],cl
	add		byte ptr 1[bx+si],ch
	lock add	byte ptr [bp+si],ch
	add		byte ptr [bp+si],bl
    DW	offset L$5
	add		byte ptr [bx+si],al
	fadd		dword ptr [bp+si]
L$21:
	sbb		ax,word ptr [eax]
	sub		byte ptr [bx+si],al
	push		9001H
	add		word ptr [bp+si],bp
	add		byte ptr [bp+si],cl
    DW	offset L$3
	add		byte ptr [bx+si],al
	fadd		dword ptr [bp+si]
L$22:
	out		9,ax
	add		byte ptr [bx+si],dl
	add		byte ptr 1[bx+si],ch
	loopnz		L$23
    DW	offset L$8
L$23 equ $-1
    DW	offset L$10
    DW	offset L$3
	add		byte ptr [bx+si],al
	fadd		dword ptr [bp+si]
L$24:
	mov		word ptr ds:[0],ax
	add		byte ptr [bx+si],al
	inc		ax
	add		word ptr 1a00H[bx],bp
	add		byte ptr [bp+si],bh
	add		byte ptr [bp+si],dl
	add		byte ptr [bx+si],al
	add		byte ptr [bp+di],dh
    DB	3
L$25:
	mov		word ptr 0,ax
	add		byte ptr [bx+si],al
	inc		ax
	add		word ptr 1a00H[bx],bp
	add		byte ptr [bp+si],bh
	add		byte ptr [bp+si],cl
	add		byte ptr [bx+si],al
	add		byte ptr [bp+di],dh
    DB	3
L$26:
	cmpsw
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	push		0e001H
	add		word ptr [bp+si],bp
	add		byte ptr [bp+si],bh
	add		byte ptr [bp+si],dl
	add		byte ptr [bx+si],al
	add		al,bl
    DB	2
L$27:
	cmpsw
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	push		0e001H
	add		word ptr [bp+si],bp
	add		byte ptr [bp+si],bh
	add		byte ptr [bp+si],cl
	add		byte ptr [bx+si],al
	add		al,bl
    DB	2
L$28:
	out		1,ax
	add		byte ptr [bp+si],al
	add		byte ptr 5801H[bx+si],dl
	add		ch,byte ptr [bp+si]
	add		byte ptr [bx+si],al
	pop		word ptr [bp+si]
L$29:
    DW	offset L$24
    DW	offset L$12
    DW	offset L$13
    DW	offset L$25
    DW	offset L$15
    DW	offset L$16
    DW	offset L$26
    DW	offset L$19
    DW	offset L$20
    DW	offset L$27
    DW	offset L$21
    DW	offset L$22
    DW	offset L$28
L$30:
	mov		ax,3
	int		10H
	mov		word ptr MX_SCREENHEIGHT,0
	mov		word ptr MX_BYTESPERLINE,0
	ret
MXSETMODE:
	push		bp
	mov		bp,sp
	sub		sp,0
	push		ds
	push		si
	push		es
	push		di
	mov		ds,word ptr cs:MX_CODESEGMENT
	mov		si,word ptr 6[bp]
	cmp		si,0dH
	jbe		L$31
	jmp		near ptr L$36
L$31:
	test		si,si
	jne		L$32
	call		near ptr L$30
	jmp		near ptr L$36
L$32:
	dec		si
	shl		si,1
	mov		si,word ptr L$29[si]
	cld
	push		si
	mov		ax,13H
	int		10H
	pop		si
	mov		dx,3c4H
	mov		ax,604H
	out		dx,ax
	mov		ax,100H
	out		dx,ax
	mov		dx,3c2H
	lodsb
	out		dx,al
	mov		dx,3c4H
	mov		ax,300H
	out		dx,ax
	mov		dx,3d4H
	mov		al,11H
	out		dx,al
	inc		dx
	in		al,dx
	and		al,7fH
	out		dx,al
	lodsw
	mov		word ptr L$1,ax
	lodsw
	mov		word ptr L$2,ax
	lodsw
	mov		word ptr MX_SCREENWIDTH,ax
	shr		ax,1
	shr		ax,1
	mov		word ptr MX_BYTESPERLINE,ax
	lodsw
	mov		word ptr MX_SCREENHEIGHT,ax
	mov		bx,si
	mov		dx,3d4H
L$33:
	mov		si,word ptr [bx]
	inc		bx
	inc		bx
	test		si,si
	je		L$35
L$34:
	lodsw
	test		ax,ax
	je		L$33
	out		dx,ax
	jmp		L$34
L$35:
	push		word ptr MX_SCREENWIDTH
	push		word ptr [bx]
	push		cs
	call		near ptr MX_TEXT:MXSETSYSCLIPREGION
	mov		dx,3c4H
	mov		ax,0f02H
	out		dx,ax
	mov		es,word ptr MX_TEXT:MX_VIDEOSEGMENT
	xor		di,di
	mov		cx,8000H
	xor		ax,ax
	rep stosw
	mov		dx,3d4H
	mov		al,11H
	out		dx,al
	inc		dx
	in		al,dx
	or		al,80H
	out		dx,al
L$36:
	xor		ax,ax
	mov		ax,word ptr MX_SCREENWIDTH
	pop		di
	pop		es
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	retf		2
MXCHANGEMODE:
	push		bp
	mov		bp,sp
	sub		sp,0
	push		ds
	push		si
	push		es
	push		di
	mov		ds,word ptr cs:MX_CODESEGMENT
	mov		si,word ptr 6[bp]
	cmp		si,0dH
	ja		L$40
	test		si,si
	je		L$40
	dec		si
	shl		si,1
	mov		si,word ptr L$29[si]
	cld
	mov		dx,3c4H
	mov		ax,604H
	out		dx,ax
	mov		ax,100H
	out		dx,ax
	mov		dx,3c2H
	lodsb
	out		dx,al
	mov		dx,3c4H
	mov		ax,300H
	out		dx,ax
	mov		dx,3d4H
	mov		al,11H
	out		dx,al
	inc		dx
	in		al,dx
	and		al,7fH
	out		dx,al
	lodsw
	mov		word ptr L$1,ax
	lodsw
	mov		word ptr L$2,ax
	lodsw
	mov		word ptr MX_SCREENWIDTH,ax
	lodsw
	mov		word ptr MX_SCREENHEIGHT,ax
	mov		bx,si
	mov		dx,3d4H
L$37:
	mov		si,word ptr [bx]
	inc		bx
	inc		bx
	test		si,si
	je		L$39
L$38:
	lodsw
	test		ax,ax
	je		L$37
	cmp		al,13H
	je		L$38
	out		dx,ax
	jmp		L$38
L$39:
	mov		dx,3d4H
	mov		al,11H
	out		dx,al
	inc		dx
	in		al,dx
	or		al,80H
	out		dx,al
L$40:
	xor		ax,ax
	mov		ax,word ptr MX_SCREENWIDTH
	pop		di
	pop		es
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	retf		2
MXGETASPECT:
	push		bp
	mov		bp,sp
	sub		sp,0
	push		ds
	push		si
	lds		si,dword ptr 0aH[bp]
	mov		ax,word ptr cs:L$1
	mov		word ptr [si],ax
	lds		si,dword ptr 6[bp]
	mov		ax,word ptr cs:L$2
	mov		word ptr [si],ax
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	retf		8
MXGETSCREENSIZE:
	push		bp
	mov		bp,sp
	sub		sp,0
	push		ds
	push		si
	lds		si,dword ptr 0aH[bp]
	mov		ax,word ptr cs:MX_SCREENWIDTH
	mov		word ptr [si],ax
	lds		si,dword ptr 6[bp]
	mov		ax,word ptr cs:MX_SCREENHEIGHT
	mov		word ptr [si],ax
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	retf		8
MX_TEXT		ENDS
		END
