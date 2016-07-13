.387
		PUBLIC	MXSETCOLOR
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
MXSETCOLOR:
	push		bp
	mov		bp,sp
	sub		sp,0
	push		ds
	push		si
	mov		ax,word ptr 0cH[bp]
	mov		dx,3c8H
	out		dx,al
	inc		dx
	mov		al,byte ptr 0aH[bp]
	out		dx,al
	mov		al,byte ptr 8[bp]
	out		dx,al
	mov		al,byte ptr 6[bp]
	out		dx,al
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	retf		8
MX_TEXT		ENDS
		END
