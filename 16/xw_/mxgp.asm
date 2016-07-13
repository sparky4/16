		PUBLIC	MXGETPALETTE
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
MXGETPALETTE:
	push		bp
	mov		bp,sp
	sub		sp,0
	push		es
	push		di
	les		di,dword ptr 0aH[bp]
	mov		cx,word ptr 6[bp]
	mov		ax,word ptr 8[bp]
	mov		dx,3c7H
	out		dx,al
	inc		dx
	inc		dx
	cld
L$1:
	in		al,dx
	stosb
	in		al,dx
	stosb
	in		al,dx
	stosb
	loop		L$1
	pop		di
	pop		es
	mov		sp,bp
	pop		bp
	retf		8
MX_TEXT		ENDS
		END
