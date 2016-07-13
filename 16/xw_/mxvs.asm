		PUBLIC	MXSETVIRTUALSCREEN
		PUBLIC	MXGETVIRTUALSCREEN
		EXTRN	MX_CODESEGMENT:BYTE
		EXTRN	MX_BYTESPERLINE:BYTE
		EXTRN	MXROWADDRESS:BYTE
		EXTRN	MXSETSYSCLIPREGION:BYTE
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
L$1:
	add		byte ptr [bx+si],al
L$2:
	add		byte ptr [bx+si],al
MXSETVIRTUALSCREEN:
	push		bp
	mov		bp,sp
	sub		sp,0
	push		ds
	mov		ds,word ptr cs:MX_CODESEGMENT
	mov		ax,1
	cmp		word ptr 8[bp],140H
	jb		L$3
	push		ax
	mov		dx,4
	xor		ax,ax
	div		word ptr 8[bp]
	cmp		word ptr 6[bp],ax
	pop		ax
	ja		L$3
	mov		ax,word ptr 8[bp]
	and		ax,0fff8H
	mov		word ptr L$1,ax
	shr		ax,1
	shr		ax,1
	mov		word ptr MX_TEXT:MX_BYTESPERLINE,ax
	shr		ax,1
	push		ax
	call		far ptr MXROWADDRESS
	mov		ax,word ptr 6[bp]
	mov		word ptr L$2,ax
	push		word ptr 8[bp]
	push		word ptr 6[bp]
	call		far ptr MXSETSYSCLIPREGION
	xor		ax,ax
L$3:
	pop		ds
	mov		sp,bp
	pop		bp
	retf		4
MXGETVIRTUALSCREEN:
	push		bp
	mov		bp,sp
	sub		sp,0
	push		ds
	push		si
	mov		ax,word ptr cs:L$1
	lds		si,dword ptr 0aH[bp]
	mov		word ptr [si],ax
	mov		ax,word ptr cs:L$2
	lds		si,dword ptr 6[bp]
	mov		word ptr [si],ax
	xor		ax,ax
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	retf		8
MX_TEXT		ENDS
		END
