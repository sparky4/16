		PUBLIC	MXSETPALETTE
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
MXSETPALETTE:
	push		bp
	mov		bp,sp
	sub		sp,0
	push		ds
	push		si
	lds		si,dword ptr 0aH[bp]
	mov		cx,word ptr 6[bp]
	mov		ax,word ptr 8[bp]
	mov		dx,3c8H
	out		dx,al
	inc		dx
	cld
	cli
L$1:
	lodsb
	out		dx,al
	lodsb
	out		dx,al
	lodsb
	out		dx,al
	loop		L$1
	sti
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	retf		8
MX_TEXT		ENDS
		END
