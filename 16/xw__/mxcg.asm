.387
		PUBLIC	MXCOLORTOGRAY
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
MXCOLORTOGRAY:
	push		bp
	mov		bp,sp
	sub		sp,0
	push		ds
	push		si
	push		es
	push		di
	mov		cx,word ptr 6[bp]
	jcxz		L$2
	lds		si,dword ptr 0cH[bp]
	les		di,dword ptr 8[bp]
	cld
	mov		bx,4d97H
L$1:
	lodsb
	mul		bh
	mov		dx,ax
	lodsb
	mul		bl
	add		dx,ax
	lodsb
	mov		ah,1cH
	mul		ah
	add		ax,dx
	mov		al,ah
	stosw
	stosb
	loop		L$1
L$2:
	pop		di
	pop		es
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	retf		0aH
MX_TEXT		ENDS
		END
