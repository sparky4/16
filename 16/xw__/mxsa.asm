.387
		PUBLIC	MXSTARTADDRESS
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
MXSTARTADDRESS:
	push		bp
	mov		bp,sp
	sub		sp,0
	mov		bx,word ptr 6[bp]
	mov		dx,3d4H
	mov		al,0cH
	mov		ah,bh
	cli
	out		dx,ax
	mov		al,0dH
	mov		ah,bl
	out		dx,ax
	sti
	mov		sp,bp
	pop		bp
	retf		2
MX_TEXT		ENDS
		END
