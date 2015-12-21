.387
		PUBLIC	MXROTATEPALETTE
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
MXROTATEPALETTE:
	push		bp
	mov		bp,sp
	sub		sp,300H
	push		ds
	push		si
	push		es
	push		di
	mov		bx,word ptr 8[bp]
	add		bx,bx
	add		bx,word ptr 8[bp]
	lds		si,dword ptr 0aH[bp]
	push		ss
	pop		es
	lea		di,-300H[bp]
	cld
	mov		ax,word ptr 6[bp]
	mov		dx,ax
	test		ax,ax
	je		L$2
	jl		L$1
	add		ax,ax
	add		dx,ax
	sub		bx,dx
	add		si,bx
	push		si
	mov		cx,dx
	rep movsb
	mov		es,word ptr 0cH[bp]
	mov		di,si
	dec		di
	pop		si
	dec		si
	mov		cx,bx
	std
	rep movsb
	push		ss
	pop		ds
	lea		si,-300H[bp]
	les		di,dword ptr 0aH[bp]
	mov		cx,dx
	cld
	rep movsb
	jmp		L$2
L$1:
	add		ax,ax
	add		dx,ax
	neg		dx
	sub		bx,dx
	mov		cx,dx
	rep movsb
	les		di,dword ptr 0aH[bp]
	mov		cx,bx
	rep movsb
	push		ss
	pop		ds
	lea		si,-300H[bp]
	mov		cx,dx
	rep movsb
L$2:
	pop		di
	pop		es
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	retf		8
MX_TEXT		ENDS
		END
