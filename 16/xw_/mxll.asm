		PUBLIC	MXLOADLATCHES
		EXTRN	MX_VIDEOSEGMENT:BYTE
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
MXLOADLATCHES:
	push		ds
	push		si
	mov		dx,3ceH
	mov		ax,0ff08H
	out		dx,ax
	mov		ax,3
	out		dx,ax
	mov		ax,5
	out		dx,ax
	mov		ax,word ptr cs:MX_VIDEOSEGMENT
	mov		ds,ax
	mov		si,0ffffH
	mov		bh,8
	mov		cx,3
L$1:
	mov		dx,3ceH
	mov		al,4
	mov		ah,cl
	out		dx,ax
	mov		dx,3c4H
	mov		al,2
	mov		ah,bh
	out		dx,ax
	mov		al,byte ptr [si]
	push		ax
	mov		byte ptr [si],bl
	mov		al,byte ptr [di]
	shr		bh,1
	loop		L$1
	mov		cx,3
	mov		bh,8
	mov		dx,3c4H
L$2:
	mov		al,2
	mov		ah,bh
	out		dx,ax
	pop		ax
	mov		byte ptr [si],al
	shr		bh,1
	loop		L$2
	pop		si
	pop		ds
	ret
MX_TEXT		ENDS
		END
