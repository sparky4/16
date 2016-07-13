.387
		PUBLIC	MX_VIDEOSEGMENT
		PUBLIC	MX_CODESEGMENT
		PUBLIC	MXINIT
		PUBLIC	MXTERM
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
	dec		bp
	dec		di
	inc		sp
	inc		bp
	pop		ax
	and		byte ptr 69H[si],ch
	bound		si,word ptr 61H[bp+si]
	jb		L$3
	and		byte ptr [di],ch
	and		byte ptr 6fH[bp+di],al
	jo		L$5
	jb		L$2
	push		2074H
	sub		byte ptr 29H[bp+di],ah
	and		byte ptr [bx+di],dh
	cmp		word ptr [bx+di],di
	xor		ch,byte ptr [di]
	xor		word ptr [bx+di],di
	cmp		word ptr [si],si
	and		byte ptr 6cH[bx+di],al
	; The label referred to here is an undefined location
	jae		0a0H
	popa
	outsb
	; The label referred to here is an undefined location
	jb		0a1H
	and		byte ptr 63H[bp+di],dl
	outsw
	; The label referred to here is an undefined location
	je		0acH
    DB	69H
MX_VIDEOSEGMENT:
    DB	0, 0a0H
MX_CODESEGMENT:
    DW	MX_TEXT
MXINIT:
	push		bp
	mov		bp,sp
	sub		sp,6
	push		ds
	push		si
	push		es
	push		di
	mov		word ptr -2[bp],0ffffH
	mov		word ptr -4[bp],0a000H
	mov		word ptr -6[bp],cs
	mov		ax,1686H
	int		2fH
	or		ax,ax
	jne		L$1
	mov		ax,0aH
	mov		bx,cs
	int		31H
	jb		L$4
	mov		word ptr -6[bp],ax
	mov		ax,2
	mov		bx,0a000H
	int		31H
	jb		L$4
	mov		word ptr -4[bp],ax
L$1:
	mov		ds,word ptr -6[bp]
	mov		word ptr MX_CODESEGMENT,ds
    DB	8bH, 46H
L$2:
	cld
	mov		word ptr MX_VIDEOSEGMENT,ax
    DB	0c7H, 46H, 0feH
L$3:
	add		byte ptr [bx+si],al
L$4:
	mov		ax,word ptr -2[bp]
	pop		di
	pop		es
L$5:
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	retf
MXTERM:
	xor		ax,ax
	retf
MX_TEXT		ENDS
		END
