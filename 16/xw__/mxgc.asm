.387
		PUBLIC	MXGETCOLOR
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
MXGETCOLOR:
	push		bp
	mov		bp,sp
	sub		sp,0
	push		ds
	push		si
	mov		ax,word ptr 12H[bp]
	mov		dx,3c7H
	out		dx,al
	inc		dx
	inc		dx
	lds		si,dword ptr 0eH[bp]
	in		al,dx
	mov		byte ptr [si],al
	lds		si,dword ptr 0aH[bp]
	in		al,dx
	mov		byte ptr [si],al
	lds		si,dword ptr 6[bp]
	in		al,dx
	mov		byte ptr [si],al
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	retf		0eH
MX_TEXT		ENDS
		END
