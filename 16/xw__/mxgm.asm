.387
		PUBLIC	MXGAMMACORRECT
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
L$1:
	add		byte ptr [bp+si],cl
	push		cs
	adc		word ptr [bp+di],dx
	adc		ax,1817H
	sbb		bl,byte ptr [bp+di]
	sbb		al,1dH
	pop		ds
	and		byte ptr [bx+di],ah
	and		ah,byte ptr [bp+di]
	and		al,25H
	and		ax,2726H
	sub		byte ptr [bx+di],ch
	sub		word ptr [bp+si],bp
	sub		bp,word ptr [si]
	sub		al,2dH
	das
	xor		byte ptr [bx+si],dh
	xor		word ptr [bx+di],si
	xor		dh,byte ptr [bp+di]
	xor		si,word ptr [si]
	xor		al,35H
	xor		ax,3636H
	aaa
	aaa
	cmp		byte ptr [bx+si],bh
	cmp		word ptr [bx+di],di
	cmp		bh,byte ptr [bp+si]
	cmp		di,word ptr [bp+di]
	cmp		al,3cH
	cmp		ax,3e3dH
	aas
	aas
MXGAMMACORRECT:
	push		bp
	mov		bp,sp
	sub		sp,0
	push		ds
	push		si
	push		es
	push		di
	mov		cx,word ptr 6[bp]
	jcxz		L$3
	lds		si,dword ptr 0cH[bp]
	les		di,dword ptr 8[bp]
	mov		bx,offset L$1
	cld
	mov		ax,cx
	add		cx,cx
	add		cx,ax
L$2:
	lodsb
	xlat		word ptr cs:[bx]
	stosb
	loop		L$2
L$3:
	pop		di
	pop		es
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	retf		0aH
MX_TEXT		ENDS
		END
