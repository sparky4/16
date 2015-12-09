.387
		PUBLIC	MXSTARTLINE
		EXTRN	MX_BYTESPERLINE:BYTE
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
MXSTARTLINE:
	push		bp
	mov		bp,sp
	sub		sp,0
	mov		ax,word ptr 6[bp]
	mul		word ptr cs:MX_BYTESPERLINE
	xchg		ax,bx
	mov		dx,3daH
L$1:
	in		al,dx
	test		al,8
	jne		L$1
	mov		dx,3d4H
	mov		al,0cH
	mov		ah,bh
	cli
	out		dx,ax
	mov		al,0dH
	mov		ah,bl
	out		dx,ax
	sti
	mov		dx,3daH
L$2:
	in		al,dx
	test		al,8
	je		L$2
	xor		ax,ax
	mov		sp,bp
	pop		bp
	retf		2
MX_TEXT		ENDS
		END
