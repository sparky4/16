		PUBLIC	MXWRITEMODE
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
MXWRITEMODE:
	push		bp
	mov		bp,sp
	sub		sp,0
	mov		dx,3ceH
	mov		ah,byte ptr 6[bp]
	and		ah,3
	or		ah,40H
	mov		al,5
	out		dx,ax
	mov		sp,bp
	pop		bp
	retf		2
MX_TEXT		ENDS
		END
