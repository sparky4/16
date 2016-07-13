.387
		PUBLIC	MXPAN
		EXTRN	MX_BYTESPERLINE:BYTE
		EXTRN	MXWAITDISPLAY:BYTE
		EXTRN	MXSTARTADDRESS:BYTE
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
MXPAN:
	push		bp
	mov		bp,sp
	sub		sp,0
	mov		ax,word ptr 6[bp]
	mul		word ptr cs:MX_BYTESPERLINE
	mov		dx,word ptr 8[bp]
	shr		dx,1
	shr		dx,1
	add		ax,dx
	push		ax
	call		far ptr MXWAITDISPLAY
	call		far ptr MXSTARTADDRESS
	mov		dx,3daH
	in		al,dx
	mov		dx,3c0H
	mov		al,33H
	out		dx,al
	mov		al,byte ptr 8[bp]
	and		al,3
	shl		al,1
	out		dx,al
	xor		ax,ax
	mov		sp,bp
	pop		bp
	retf		4
MX_TEXT		ENDS
		END
