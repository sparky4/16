		PUBLIC	MXROWADDRESS
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
MXROWADDRESS:
	push		bp
	mov		bp,sp
	sub		sp,0
	mov		dx,3d4H
	mov		al,13H
	mov		ah,byte ptr 6[bp]
	out		dx,ax
	xor		ax,ax
	mov		sp,bp
	pop		bp
	retf		2
MX_TEXT		ENDS
		END
