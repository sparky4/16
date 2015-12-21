.387
		PUBLIC	SUBHORIZONTALLINEINFO
		EXTRN	MX_BYTESPERLINE:BYTE
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
L$1:
	add		byte ptr [bx+si],al
	add		ax,word ptr [bx+si]
	add		al,byte ptr [bx+si]
	add		word ptr [bx+si],ax
L$2:
	add		byte ptr ds:[80cH],cl
L$3:
	add		byte ptr [bx+di],al
	add		ax,word ptr [bx]
L$4:
    DB	2, 6
SUBHORIZONTALLINEINFO:
    DD	ds:[0f72eH]
	add		byte ptr -2e05H[bp+di],cl
	out		dx,ax
	shr		di,1
	add		di,ax
	and		bx,3
	mov		al,byte ptr cs:L$2[bx]
	shl		bx,1
	sub		cx,word ptr cs:L$1[bx]
	jge		L$5
	mov		bx,cx
	inc		bx
	inc		bx
	and		al,byte ptr cs:L$4[bx]
	xor		ah,ah
	xor		cx,cx
	jmp		L$6
L$5:
	mov		bx,cx
	and		bx,3
	mov		ah,byte ptr cs:L$3[bx]
	shr		cx,1
	shr		cx,1
L$6:
	ret
MX_TEXT		ENDS
		END
