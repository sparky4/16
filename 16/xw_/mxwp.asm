		PUBLIC	MXWRITEPLANE
		PUBLIC	MXREADPLANE
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
MXWRITEPLANE:
	push		bp
	mov		bp,sp
	sub		sp,0
	mov		ah,byte ptr 6[bp]
	and		ah,0fH
	mov		al,2
	mov		dx,3c4H
	out		dx,ax
	mov		sp,bp
	pop		bp
	retf		2
MXREADPLANE:
	push		bp
	mov		bp,sp
	sub		sp,0
	mov		al,4
	mov		ah,byte ptr 6[bp]
	and		ah,3
	mov		dx,3ceH
	out		dx,ax
	mov		sp,bp
	pop		bp
	retf		2
MX_TEXT		ENDS
		END
