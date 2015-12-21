.387
		PUBLIC	MXSPLITSCREEN
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
MXSPLITSCREEN:
	push		bp
	mov		bp,sp
	sub		sp,0
	mov		ax,word ptr 6[bp]
	shl		ax,1
	mov		bh,ah
	mov		bl,ah
	and		bx,201H
	mov		cl,4
	shl		bx,cl
	shl		bh,1
	mov		dx,3d4H
	mov		ah,al
	mov		al,18H
	out		dx,ax
	mov		al,7
	out		dx,al
	inc		dx
	in		al,dx
	dec		dx
	mov		ah,al
	and		ah,0efH
	or		ah,bl
	mov		al,7
	out		dx,ax
	mov		al,9
	out		dx,al
	inc		dx
	in		al,dx
	dec		dx
	mov		ah,al
	and		ah,0bfH
	or		ah,bh
	mov		al,9
	out		dx,ax
	mov		sp,bp
	pop		bp
	retf		2
MX_TEXT		ENDS
		END
