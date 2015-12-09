.387
		PUBLIC	XSUBCLIPLINE
		EXTRN	MX_CLIPY1:BYTE
		EXTRN	MX_CLIPY2:BYTE
		EXTRN	MX_CLIPX2:BYTE
		EXTRN	MX_CLIPX1:BYTE
		EXTRN	MX_VIDEOSEGMENT:BYTE
		EXTRN	MX_BYTESPERLINE:BYTE
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
L$1:
	or		al,byte ptr [bx+si]
L$2:
    DW	offset L$3
	or		al,byte ptr [bx+si]
    DW	offset L$4
	or		word ptr [bx+si],ax
    DW	offset L$5
	????
	inc		word ptr [bx+si]
	add		byte ptr [bp+si],cl
	add		byte ptr 0a00H[bx+si],al
	add		byte ptr 900H[bp],dl
	add		byte ptr -100H[si],ch
	inc		word ptr [bx+si]
	add		byte ptr ds:[0c000H],al
	add		byte ptr ds:[0ce00H],al
	add		byte ptr [di],al
	add		ah,bl
	add		bh,bh
	inc		word ptr [bx+si]
	add		bh,bh
	inc		word ptr [bx+si]
	add		bh,bh
	inc		word ptr [bx+si]
	add		bh,bh
	inc		word ptr [bx+si]
	add		bh,bh
	inc		word ptr [bx+si]
    DB	0
L$3:
    DW	offset MX_TEXT+0ecH
    DW	offset MX_TEXT+0eeH
    DW	offset MX_TEXT+0f1H
    DW	offset MX_TEXT+0eaH
    DW	offset MX_TEXT+0f4H
    DW	offset MX_TEXT+0f7H
    DW	offset MX_TEXT+104H
    DW	offset MX_TEXT+0eaH
    DW	offset MX_TEXT+111H
    DW	offset MX_TEXT+114H
    DW	offset MX_TEXT+121H
L$4:
    DW	offset MX_TEXT+132H
    DW	offset MX_TEXT+0eaH
    DW	offset MX_TEXT+135H
    DW	offset MX_TEXT+0eaH
    DW	offset MX_TEXT+13bH
    DW	offset MX_TEXT+0eaH
    DW	offset MX_TEXT+148H
    DW	offset L$7
    DW	offset L$10
    DW	offset L$7
    DW	offset L$11
L$5:
    DW	offset L$14
    DW	offset L$15
    DW	offset L$7
    DW	offset L$7
    DW	offset L$16
    DW	offset L$17
    DW	offset L$7
    DW	offset L$7
    DW	offset L$18
    DW	offset L$19
    DW	offset L$22
    DW	offset L$23
    DW	offset L$24
    DW	offset L$7
    DW	offset L$7
    DW	offset L$7
    DW	offset L$7
    DW	offset L$7
    DW	offset L$25
    DW	offset L$26
    DW	offset L$27
    DW	offset L$30
    DW	offset L$7
    DW	offset L$31
    DW	offset L$7
    DW	offset L$7
    DW	offset L$7
    DW	offset L$7
    DW	offset L$7
    DW	offset L$32
    DW	offset L$7
    DW	offset L$33
    DW	offset L$36
    DW	offset L$37
    DW	offset L$7
    DW	offset L$7
    DW	offset L$7
    DW	offset L$7
    DW	offset L$7
    DW	offset L$7
    DW	offset L$38
    DW	offset L$39
    DW	offset L$43
    DW	offset L$44
    DW	offset L$45
    DW	offset L$7
    DW	offset L$46
    DW	offset L$47
    DW	offset L$48
    DW	offset L$51
    DW	offset L$7
    DW	offset L$52
    DW	offset L$7
    DW	offset L$53
    DW	offset L$7
    DW	offset L$54
    DW	offset L$58
    DW	offset L$59
    DW	offset L$7
    DW	offset L$7
    DW	offset L$60
    DW	offset L$61
L$6:
	clc
	ret
L$7:
	stc
	ret
	clc
	ret
	jmp		near ptr L$67
	jmp		near ptr L$68
	jmp		near ptr L$70
	call		near ptr L$67
	cmp		si,word ptr cs:MX_CLIPY1
	jge		L$6
	jmp		near ptr L$70
	call		near ptr L$68
	cmp		si,word ptr cs:MX_CLIPY1
	jge		L$6
	jmp		near ptr L$70
	jmp		near ptr L$69
	call		near ptr L$67
	cmp		si,word ptr cs:MX_CLIPY2
	jle		L$6
	jmp		near ptr L$69
	call		near ptr L$68
	cmp		si,word ptr cs:MX_CLIPY2
	jle		L$6
	jmp		near ptr L$69
L$8:
	stc
	ret
L$9:
	clc
	ret
	jmp		near ptr L$63
	call		near ptr L$63
	jmp		near ptr L$68
	call		near ptr L$63
	cmp		bx,word ptr cs:MX_CLIPY1
	jl		L$8
	jmp		near ptr L$70
	call		near ptr L$63
	cmp		bx,word ptr cs:MX_CLIPY1
	jl		L$8
	call		near ptr L$70
	cmp		cx,word ptr cs:MX_CLIPX2
	jle		L$9
	jmp		near ptr L$68
L$10:
	call		near ptr L$63
	cmp		bx,word ptr cs:MX_CLIPY2
	jg		L$8
	jmp		near ptr L$69
L$11:
	call		near ptr L$63
	cmp		bx,word ptr cs:MX_CLIPY2
	jg		L$8
	call		near ptr L$69
	cmp		cx,word ptr cs:MX_CLIPX2
	jle		L$9
	jmp		near ptr L$68
L$12:
	clc
	ret
L$13:
	stc
	ret
L$14:
	jmp		near ptr L$64
L$15:
	call		near ptr L$64
	jmp		near ptr L$67
L$16:
	call		near ptr L$64
	cmp		bx,word ptr cs:MX_CLIPY1
	jl		L$13
	jmp		near ptr L$70
L$17:
	call		near ptr L$64
	cmp		bx,word ptr cs:MX_CLIPY1
	jl		L$13
	call		near ptr L$70
	cmp		cx,word ptr cs:MX_CLIPX1
	jge		L$12
	jmp		near ptr L$67
L$18:
	call		near ptr L$64
	cmp		bx,word ptr cs:MX_CLIPY2
	jg		L$13
	jmp		near ptr L$69
L$19:
	call		near ptr L$64
	cmp		bx,word ptr cs:MX_CLIPY2
	jg		L$13
	call		near ptr L$69
	cmp		cx,word ptr cs:MX_CLIPX1
	jge		L$12
	jmp		near ptr L$67
L$20:
	clc
	ret
L$21:
	stc
	ret
L$22:
	jmp		near ptr L$66
L$23:
	call		near ptr L$66
	cmp		di,word ptr cs:MX_CLIPX1
	jl		L$21
	call		near ptr L$67
	cmp		si,word ptr cs:MX_CLIPY1
	jge		L$20
	jmp		near ptr L$70
L$24:
	call		near ptr L$66
	cmp		di,word ptr cs:MX_CLIPX2
	jg		L$21
	jmp		near ptr L$68
L$25:
	call		near ptr L$66
	jmp		near ptr L$69
L$26:
	call		near ptr L$66
	cmp		di,word ptr cs:MX_CLIPX1
	jl		L$21
	call		near ptr L$67
	cmp		si,word ptr cs:MX_CLIPY2
	jle		L$20
	jmp		near ptr L$69
L$27:
	call		near ptr L$66
	cmp		di,word ptr cs:MX_CLIPX2
	jg		L$21
	call		near ptr L$68
	cmp		si,word ptr cs:MX_CLIPY2
	jle		L$20
	jmp		near ptr L$69
L$28:
	clc
	ret
L$29:
	stc
	ret
L$30:
	call		near ptr L$63
	cmp		bx,word ptr cs:MX_CLIPY1
	jge		L$28
	jmp		near ptr L$66
L$31:
	call		near ptr L$68
	cmp		si,word ptr cs:MX_CLIPY1
	jl		L$29
	call		near ptr L$66
	cmp		di,word ptr cs:MX_CLIPX1
	jge		L$28
	jmp		near ptr L$63
L$32:
	call		near ptr L$69
	cmp		cx,word ptr cs:MX_CLIPX1
	jl		L$29
	call		near ptr L$66
	cmp		di,word ptr cs:MX_CLIPX1
	jge		L$28
	jmp		near ptr L$63
L$33:
	call		near ptr L$63
	cmp		bx,word ptr cs:MX_CLIPY2
	jg		L$29
	call		near ptr L$68
	cmp		bx,word ptr cs:MX_CLIPY1
	jl		L$29
	cmp		si,word ptr cs:MX_CLIPY2
	jle		L$28
	jmp		near ptr L$69
L$34:
	clc
	ret
L$35:
	stc
	ret
L$36:
	call		near ptr L$64
	cmp		bx,word ptr cs:MX_CLIPY1
	jge		L$34
	jmp		near ptr L$66
L$37:
	call		near ptr L$67
	cmp		si,word ptr cs:MX_CLIPY2
	jl		L$35
	call		near ptr L$66
	cmp		di,word ptr cs:MX_CLIPX2
	jle		L$34
	jmp		near ptr L$64
L$38:
	call		near ptr L$69
	cmp		cx,word ptr cs:MX_CLIPX2
	jg		L$35
	call		near ptr L$64
	cmp		bx,word ptr cs:MX_CLIPY1
	jge		L$34
	jmp		near ptr L$66
L$39:
	call		near ptr L$67
	cmp		si,word ptr cs:MX_CLIPY1
	jl		L$35
	call		near ptr L$64
	cmp		bx,word ptr cs:MX_CLIPY2
	jg		L$35
	cmp		si,word ptr cs:MX_CLIPY2
	jle		L$40
	call		near ptr L$69
L$40:
	cmp		bx,word ptr cs:MX_CLIPY1
	jge		L$34
	jmp		near ptr L$66
L$41:
	clc
	ret
L$42:
	stc
	ret
L$43:
	jmp		near ptr L$65
L$44:
	call		near ptr L$65
	cmp		di,word ptr cs:MX_CLIPX1
	jl		L$42
	jmp		near ptr L$67
L$45:
	call		near ptr L$65
	cmp		di,word ptr cs:MX_CLIPX2
	jg		L$42
	jmp		near ptr L$68
L$46:
	call		near ptr L$65
	jmp		near ptr L$70
L$47:
	call		near ptr L$65
	cmp		di,word ptr cs:MX_CLIPX1
	jl		L$42
	call		near ptr L$67
	cmp		si,word ptr cs:MX_CLIPY1
	jge		L$42
	jmp		near ptr L$70
L$48:
	call		near ptr L$65
	cmp		di,word ptr cs:MX_CLIPX2
	jg		L$42
	call		near ptr L$68
	cmp		si,word ptr cs:MX_CLIPY1
	jge		L$41
	jmp		near ptr L$70
L$49:
	clc
	ret
L$50:
	stc
	ret
L$51:
	call		near ptr L$63
	cmp		bx,word ptr cs:MX_CLIPY2
	jle		L$49
	jmp		near ptr L$65
L$52:
	call		near ptr L$68
	cmp		si,word ptr cs:MX_CLIPY2
	jg		L$50
	call		near ptr L$65
	cmp		di,word ptr cs:MX_CLIPX1
	jge		L$49
	jmp		near ptr L$63
L$53:
	call		near ptr L$70
	cmp		cx,word ptr cs:MX_CLIPX1
	jl		L$50
	call		near ptr L$63
	cmp		bx,word ptr cs:MX_CLIPY2
	jle		L$49
	jmp		near ptr L$65
L$54:
	call		near ptr L$63
	cmp		bx,word ptr cs:MX_CLIPY1
	jl		L$50
	call		near ptr L$68
	cmp		si,word ptr cs:MX_CLIPY2
	jg		L$50
	cmp		bx,word ptr cs:MX_CLIPY2
	jle		L$55
	call		near ptr L$65
L$55:
	cmp		si,word ptr cs:MX_CLIPY1
	jge		L$49
	jmp		near ptr L$70
L$56:
	clc
	ret
L$57:
	stc
	ret
L$58:
	call		near ptr L$64
	cmp		bx,word ptr cs:MX_CLIPY2
	jle		L$56
	jmp		near ptr L$65
L$59:
	call		near ptr L$67
	cmp		si,word ptr cs:MX_CLIPY2
	jg		L$57
	call		near ptr L$65
	cmp		di,word ptr cs:MX_CLIPX2
	jle		L$56
	jmp		L$64
L$60:
	call		near ptr L$70
	cmp		cx,word ptr cs:MX_CLIPX2
	jg		L$57
	call		near ptr L$64
	cmp		bx,word ptr cs:MX_CLIPY2
	jle		L$56
	jmp		L$65
L$61:
	call		near ptr L$67
	cmp		si,word ptr cs:MX_CLIPY2
	jg		L$57
	call		near ptr L$64
	cmp		bx,word ptr cs:MX_CLIPY1
	jl		L$57
	cmp		si,word ptr cs:MX_CLIPY1
	jge		L$62
	call		near ptr L$70
L$62:
	cmp		bx,word ptr cs:MX_CLIPY2
	jle		L$56
	jmp		L$65
L$63:
	mov		ax,si
	sub		ax,bx
	mov		dx,word ptr cs:MX_CLIPX1
	sub		dx,di
	imul		dx
	mov		bp,cx
	sub		bp,di
	idiv		bp
	add		bx,ax
	mov		di,word ptr cs:MX_CLIPX1
	clc
	ret
L$64:
	mov		ax,si
	sub		ax,bx
	mov		dx,word ptr cs:MX_CLIPX2
	sub		dx,di
	imul		dx
	mov		bp,cx
	sub		bp,di
	idiv		bp
	add		bx,ax
	mov		di,word ptr cs:MX_CLIPX2
	clc
	ret
L$65:
	mov		ax,cx
	sub		ax,di
	mov		dx,word ptr cs:MX_CLIPY2
	sub		dx,bx
	imul		dx
	mov		bp,si
	sub		bp,bx
	idiv		bp
	add		di,ax
	mov		bx,word ptr cs:MX_CLIPY2
	clc
	ret
L$66:
	mov		ax,cx
	sub		ax,di
	mov		dx,word ptr cs:MX_CLIPY1
	sub		dx,bx
	imul		dx
	mov		bp,si
	sub		bp,bx
	idiv		bp
	add		di,ax
	mov		bx,word ptr cs:MX_CLIPY1
	clc
	ret
L$67:
	mov		ax,bx
	sub		ax,si
	mov		dx,word ptr cs:MX_CLIPX1
	sub		dx,cx
	imul		dx
	mov		bp,di
	sub		bp,cx
	idiv		bp
	add		si,ax
	mov		cx,word ptr cs:MX_CLIPX1
	clc
	ret
L$68:
	mov		ax,bx
	sub		ax,si
	mov		dx,word ptr cs:MX_CLIPX2
	sub		dx,cx
	imul		dx
	mov		bp,di
	sub		bp,cx
	idiv		bp
	add		si,ax
	mov		cx,word ptr cs:MX_CLIPX2
	clc
	ret
L$69:
	mov		ax,di
	sub		ax,cx
	mov		dx,word ptr cs:MX_CLIPY2
	sub		dx,si
	imul		dx
	mov		bp,bx
	sub		bp,si
	idiv		bp
	add		cx,ax
	mov		si,word ptr cs:MX_CLIPY2
	clc
	ret
L$70:
	mov		ax,di
	sub		ax,cx
	mov		dx,word ptr cs:MX_CLIPY1
	sub		dx,si
	imul		dx
	mov		bp,bx
	sub		bp,si
	idiv		bp
	add		cx,ax
	mov		si,word ptr cs:MX_CLIPY1
	clc
	ret
XSUBCLIPLINE:
	push		bp
	xor		si,si
	cmp		dx,word ptr cs:MX_CLIPY2
	jle		L$71
	or		si,8
	jmp		L$72
L$71:
	cmp		dx,word ptr cs:MX_CLIPY1
	jge		L$72
	or		si,4
L$72:
	cmp		cx,word ptr cs:MX_CLIPX2
	jle		L$73
	or		si,2
	jmp		L$74
L$73:
	cmp		cx,word ptr cs:MX_CLIPX1
	jge		L$74
	or		si,1
L$74:
	cmp		bx,word ptr cs:MX_CLIPY2
	jle		L$75
	or		si,80H
	jmp		L$76
L$75:
	cmp		bx,word ptr cs:MX_CLIPY1
	jge		L$76
	or		si,40H
L$76:
	cmp		ax,word ptr cs:MX_CLIPX2
	jle		L$77
	or		si,20H
	jmp		L$78
L$77:
	cmp		ax,word ptr cs:MX_CLIPX1
	jge		L$78
	or		si,10H
L$78:
	mov		di,si
	and		di,0fH
	and		si,0f0H
	shr		si,1
	shr		si,1
	cmp		di,word ptr cs:L$1[si]
	jg		L$79
	mov		si,word ptr cs:L$2[si]
	shl		di,1
	add		si,di
	mov		di,ax
	mov		si,word ptr cs:[si]
	xchg		si,dx
	call		dx
	mov		ax,di
	mov		dx,si
	pop		bp
	ret
L$79:
	pop		bp
	stc
	ret
MX_TEXT		ENDS
		END
