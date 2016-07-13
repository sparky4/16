		PUBLIC	MX_CLIPX1
		PUBLIC	MX_CLIPY1
		PUBLIC	MX_CLIPX2
		PUBLIC	MX_CLIPY2
		PUBLIC	MXSETCLIP
		PUBLIC	MXGETCLIP
		PUBLIC	MXSETSYSCLIPREGION
		PUBLIC	MXSETCLIPREGION
		PUBLIC	MXGETCLIPREGION
		PUBLIC	SUBCLIPIMAGE
		PUBLIC	SUBCLIPBOX
		EXTRN	MX_CODESEGMENT:BYTE
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
MX_CLIPX1:
	add		byte ptr [bx+si],al
MX_CLIPY1:
	add		byte ptr [bx+si],al
MX_CLIPX2:
	add		byte ptr [bx+si],al
MX_CLIPY2:
	add		byte ptr [bx+si],al
L$1:
	add		byte ptr [bx+si],al
L$2:
	add		byte ptr [bx+si],al
L$3:
	add		byte ptr [bx+si],al
L$4:
	add		byte ptr [bx+si],al
L$5:
	add		byte ptr [bx+si],al
L$6:
	add		byte ptr [bx+si],al
L$7:
	add		byte ptr [bx+si],al
L$8:
	add		byte ptr [bx+si],al
L$9:
    DB	0
MXSETCLIP:
	push		bp
	mov		bp,sp
	sub		sp,0
	push		ds
	mov		ds,word ptr cs:MX_CODESEGMENT
	mov		ax,word ptr L$5
	mov		bx,word ptr L$6
	mov		cx,word ptr L$7
	mov		dx,word ptr L$8
	cmp		byte ptr 6[bp],1
	je		L$10
	mov		ax,word ptr L$1
	mov		bx,word ptr L$2
	mov		cx,word ptr L$3
	mov		dx,word ptr L$4
L$10:
	mov		word ptr MX_CLIPX1,ax
	mov		word ptr MX_CLIPY1,bx
	mov		word ptr MX_CLIPX2,cx
	mov		word ptr MX_CLIPY2,dx
	mov		al,byte ptr 6[bp]
	xchg		byte ptr L$9,al
	xor		ah,ah
	pop		ds
	mov		sp,bp
	pop		bp
	retf		2
MXGETCLIP:
	mov		al,byte ptr cs:L$9
	xor		ah,ah
	retf
MXSETSYSCLIPREGION:
	push		bp
	mov		bp,sp
	sub		sp,0
	push		ds
	mov		ds,word ptr cs:MX_CODESEGMENT
	xor		ax,ax
	mov		word ptr L$1,ax
	mov		word ptr L$2,ax
	mov		ax,word ptr 8[bp]
	dec		ax
	mov		word ptr L$3,ax
	mov		ax,word ptr 6[bp]
	dec		ax
	mov		word ptr L$4,ax
	mov		ax,0
	push		ax
	push		cs
	call		near ptr MXSETCLIP
	pop		ds
	mov		sp,bp
	pop		bp
	retf		4
MXSETCLIPREGION:
	push		bp
	mov		bp,sp
	sub		sp,0
	push		ds
	mov		ds,word ptr cs:MX_CODESEGMENT
	mov		ax,word ptr 0cH[bp]
	mov		word ptr L$5,ax
	mov		ax,word ptr 0aH[bp]
	mov		word ptr L$6,ax
	mov		ax,word ptr 8[bp]
	add		ax,word ptr 0cH[bp]
	dec		ax
	mov		word ptr L$7,ax
	mov		ax,word ptr 6[bp]
	add		ax,word ptr 0aH[bp]
	dec		ax
	mov		word ptr L$8,ax
	mov		al,byte ptr L$9
	cmp		al,1
	jne		L$11
	push		ax
	push		cs
	call		near ptr MXSETCLIP
L$11:
	xor		ax,ax
	pop		ds
	mov		sp,bp
	pop		bp
	retf		8
MXGETCLIPREGION:
	push		bp
	mov		bp,sp
	sub		sp,0
	push		es
	push		di
	mov		ax,word ptr cs:L$5
	les		di,dword ptr 12H[bp]
	mov		word ptr es:[di],ax
	mov		ax,word ptr cs:L$6
	les		di,dword ptr 0eH[bp]
	mov		word ptr es:[di],ax
	mov		ax,word ptr cs:L$7
	sub		ax,word ptr cs:L$5
	inc		ax
	les		di,dword ptr 0aH[bp]
	mov		word ptr es:[di],ax
	mov		ax,word ptr cs:L$8
	sub		ax,word ptr cs:L$6
	inc		ax
	les		di,dword ptr 6[bp]
	mov		word ptr es:[di],ax
	mov		al,byte ptr cs:L$9
	xor		ah,ah
	pop		di
	pop		es
	mov		sp,bp
	pop		bp
	retf		10H
SUBCLIPIMAGE:
	xor		si,si
	mov		di,word ptr cs:MX_CLIPY1
	cmp		ax,di
	jge		L$12
	sub		di,ax
	sub		dx,di
	jle		L$16
	mov		ax,di
	mov		di,dx
	mul		cx
	mov		si,ax
	mov		dx,di
	mov		ax,word ptr cs:MX_CLIPY1
L$12:
	mov		di,word ptr cs:MX_CLIPY2
	cmp		ax,di
	jg		L$16
	inc		di
	sub		di,dx
	sub		di,ax
	jge		L$13
	add		dx,di
L$13:
	mov		di,word ptr cs:MX_CLIPX1
	cmp		bx,di
	jge		L$14
	sub		di,bx
	sub		cx,di
	jle		L$16
	add		si,di
	mov		bx,word ptr cs:MX_CLIPX1
L$14:
	mov		di,word ptr cs:MX_CLIPX2
	cmp		bx,di
	jg		L$16
	inc		di
	sub		di,bx
	sub		di,cx
	jge		L$15
	add		cx,di
L$15:
	clc
	ret
L$16:
	stc
	ret
SUBCLIPBOX:
	mov		di,word ptr cs:MX_CLIPY1
	cmp		ax,di
	jge		L$17
	sub		di,ax
	sub		dx,di
	jle		L$21
	mov		ax,word ptr cs:MX_CLIPY1
L$17:
	mov		di,word ptr cs:MX_CLIPY2
	cmp		ax,di
	jg		L$21
	inc		di
	sub		di,dx
	sub		di,ax
	jge		L$18
	add		dx,di
L$18:
	mov		di,word ptr cs:MX_CLIPX1
	cmp		bx,di
	jge		L$19
	sub		di,bx
	sub		cx,di
	jle		L$21
	mov		bx,word ptr cs:MX_CLIPX1
L$19:
	mov		di,word ptr cs:MX_CLIPX2
	cmp		bx,di
	jg		L$21
	inc		di
	sub		di,bx
	sub		di,cx
	jge		L$20
	add		cx,di
L$20:
	clc
	ret
L$21:
	stc
	ret
MX_TEXT		ENDS
		END
