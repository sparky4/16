.8086
code		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:code, DS:DGROUP, SS:DGROUP
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	cld
	cld
	jmp		near ptr main
L$1:
	call		near ptr L$2
	add		ah,3
L$2:
	lodsb
a_outaw:
	push		ax
	push		cx
	xchg		al,ah
	mov		dx,388H
	out		dx,al
	mov		cx,7
	call		near ptr L$3
	mov		dx,389H
	mov		al,ah
	out		dx,al
	mov		cx,1eH
	call		near ptr L$3
	pop		cx
	pop		ax
	ret
L$3:
	in		al,dx
	loop		L$3
	ret
a_loadinstrument:
	mov		ah,byte ptr L$59[bx]
	mov		cx,4
L$4:
	call		near ptr L$1
	add		ah,1dH
	loop		L$4
	add		ah,40H
	call		near ptr L$1
	mov		ah,bl
	add		ah,0c0H
	jmp		L$2
a_playnote:
	push		bx
	xchg		ah,bl
	add		ah,0a0H
	call		near ptr a_outaw
	mov		al,bl
	add		ah,10H
	pop		bx
	jmp		a_outaw
a_init:
	mov		ax,120H
	call		near ptr a_outaw
	mov		ax,800H
	call		near ptr a_outaw
	mov		ah,0bdH
	call		near ptr a_outaw
	mov		bp,9
	xor		bx,bx
	mov		di,offset L$66
L$5:
	mov		si,word ptr [di]
	add		di,2
	call		near ptr a_loadinstrument
	xor		ax,ax
	call		near ptr a_playnote
	inc		bx
	dec		bp
	jne		L$5
	ret
a_dorow:
	sub		byte ptr L$108,1
	jae		L$10
	mov		byte ptr L$108,8
	mov		cx,8
	mov		di,offset L$72
	xor		bx,bx
L$6:
	sub		byte ptr L$109[bx],1
	jns		L$9
	mov		si,word ptr [di]
	xor		ax,ax
	call		near ptr a_playnote
L$7:
	lodsb
	or		al,al
	je		L$11
	jns		L$8
	sub		al,81H
	mov		byte ptr L$110[bx],al
	lodsb
L$8:
	mov		dl,al
	and		ax,0fH
	mov		bp,ax
	add		bp,bp
	mov		ax,word ptr ds:L$60[bp]
	shr		dl,2
	and		dl,0fcH
	add		ah,dl
	call		near ptr a_playnote
	mov		al,byte ptr L$110[bx]
	mov		byte ptr L$109[bx],al
	mov		word ptr [di],si
L$9:
	add		di,4
	inc		bx
	loop		L$6
L$10:
	ret
L$11:
	mov		si,word ptr 2[di]
	jmp		L$7
L$12:
	add		ax,word ptr [bx]
	mov		word ptr [bx],ax
L$13:
	add		ax,4000H
L$14:
	mov		bx,ax
	mov		cx,bx
	and		cx,3ffH
	neg		cx
	add		cx,3ffH
	shr		bx,0aH
	mov		ah,byte ptr L$57[bx]
	xor		al,al
	imul		cx
	push		ax
	push		dx
	mov		ah,byte ptr L$58[bx]
	xor		al,al
	neg		cx
	add		cx,3ffH
	imul		cx
	pop		bx
	pop		cx
	add		ax,cx
	adc		dx,bx
	shrd		ax,dx,0bH
	ret
rand:
	mov		eax,41fbf0e7H
	mul		dword ptr L$115
	add		eax,17b99H
	mov		dword ptr L$115,eax
	shr		eax,0fH
	and		ax,1fffH
	sub		ax,1000H
	add		bx,2
	mov		word ptr [bx],ax
	ret
inittimer:
	mov		eax,dword ptr fs:[20H]
	mov		dword ptr L$114,eax
	mov		ax,cs
	shl		eax,10H
	mov		ax,offset intti8
	mov		dx,4268H
	jmp		L$16
L$15:
	mov		eax,dword ptr L$114
	xor		dx,dx
L$16:
	cli
	mov		dword ptr fs:[20H],eax
	mov		al,36H
	out		43H,al
	mov		al,dl
	out		40H,al
	mov		al,dh
	out		40H,al
	sti
	ret
intti8:
	push		ax
	mov		al,20H
	out		20H,al
	inc		word ptr cs:L$113
	pop		ax
	iret
setpal:
	mov		dx,3c8H
	xor		al,al
	out		dx,al
	inc		dx
	mov		cx,8
L$17:
	xor		bh,bh
	mov		bl,byte ptr [si]
	shr		bl,2
	call		near ptr L$18
	mov		bl,byte ptr [si]
	shl		bx,2
	call		near ptr L$18
	inc		si
	loop		L$17
	ret
L$18:
	and		bx,1eH
	mov		ax,word ptr L$61[bx]
	out		dx,al
	mov		al,ah
	out		dx,al
	mov		al,byte ptr L$62[bx]
	out		dx,al
	ret
clearcopy:
	xor		edx,edx
	mov		si,offset L$93
	mov		bx,4
	mov		cx,0c8H
	mov		di,0fffcH
L$19:
	mov		bp,5
L$20:
	mov		eax,dword ptr [si]
	add		di,bx
	mov		dword ptr [si],edx
	add		si,bx
	mov		dword ptr es:[di],eax
	mov		eax,dword ptr [si]
	add		di,bx
	mov		dword ptr [si],edx
	add		si,bx
	mov		dword ptr es:[di],eax
	dec		bp
	jne		L$20
	add		si,bx
	dec		cx
	jne		L$19
	ret
pset1:
	mov		al,byte ptr L$88[si]
	or		byte ptr [di],al
	ret
pset2:
	mov		ax,word ptr L$91[si]
	or		word ptr [di],ax
	or		word ptr 2cH[di],ax
	cmp		bp,206cH
	jl		L$21
	mov		ax,word ptr L$90[si]
	or		word ptr -2cH[di],ax
	or		word ptr 58H[di],ax
	mov		ax,word ptr L$89[si]
	or		word ptr -58H[di],ax
	or		word ptr 84H[di],ax
	ret
L$21:
	or		word ptr -2cH[di],ax
	or		word ptr 58H[di],ax
	mov		ax,word ptr L$90[si]
	or		word ptr -58H[di],ax
	or		word ptr 84H[di],ax
	ret
letter3d:
	sub		bx,41H
	jb		L$25
	shl		bx,3
	mov		di,word ptr L$111
	mov		cx,8
L$22:
	push		cx
	push		si
	mov		cx,8
L$23:
	cmp		byte ptr L$94[bx],0
	je		L$24
	mov		word ptr L$96[di],si
	mov		word ptr L$97[di],bp
	push		si
	add		si,word ptr L$106
	sar		si,6
	and		si,3fH
	mov		al,byte ptr L$57[si]
	cbw
	pop		si
	shl		ax,2
	mov		word ptr L$98[di],ax
	mov		word ptr L$99[di],offset pset2
	add		di,8
	and		di,7ffH
L$24:
	inc		bx
	add		si,80H
	loop		L$23
	pop		si
	add		bx,138H
	add		bp,80H
	pop		cx
	loop		L$22
	mov		word ptr L$111,di
L$25:
	ret
set3drot:
	push		ax
	call		near ptr L$14
	mov		word ptr 4[di],ax
	neg		ax
	mov		word ptr 2[di],ax
	pop		ax
	call		near ptr L$13
	mov		word ptr [di],ax
	mov		word ptr 6[di],ax
	ret
rotate2x2i:
	push		bx
	mov		ax,di
	imul		word ptr [si]
	mov		cx,ax
	mov		bx,dx
	mov		ax,bp
	imul		word ptr 2[si]
	add		cx,ax
	adc		bx,dx
	shrd		cx,bx,0eH
	pop		bx
	add		si,4
	ret
doit:
	mov		dx,3daH
L$26:
	in		al,dx
	test		al,8
	jne		L$26
L$27:
	in		al,dx
	test		al,8
	je		L$27
	mov		si,word ptr L$65
	push		si
	call		near ptr setpal
	pop		si
	add		si,9
	cmp		si,offset L$64
	jbe		L$28
	mov		si,offset L$63
L$28:
	mov		word ptr L$65,si
	mov		al,2
	mov		ah,byte ptr 8[si]
	mov		dx,3c4H
	out		dx,ax
	call		near ptr clearcopy
	xor		cx,cx
	mov		byte ptr L$112,0
	xchg		word ptr L$113,cx
	jcxz		L$30
L$29:
	push		cx
	add		byte ptr L$112,5aH
	call		near ptr doit70
	pop		cx
	loop		L$29
L$30:
	mov		cx,1bcH
	mov		bx,offset L$96
L$31:
	push		cx
	push		bx
	mov		bp,word ptr 2[bx]
	mov		di,word ptr 4[bx]
	cmp		word ptr 6[bx],offset pset2
	jne		L$32
	mov		cx,word ptr [bx]
	mov		si,word ptr L$107
	add		si,cx
	sar		si,7
	and		si,3fH
	mov		al,byte ptr L$57[si]
	cbw
	shl		ax,2
	add		bp,ax
	sub		cx,word ptr L$112
	mov		word ptr [bx],cx
	cmp		cx,0f0c4H
	jl		L$33
	cmp		cx,0f3cH
	jg		L$33
L$32:
	mov		si,offset L$100
	call		near ptr rotate2x2i
	push		cx
	call		near ptr rotate2x2i
	pop		di
	mov		bp,word ptr [bx]
	mov		si,offset L$101
	push		cx
	call		near ptr rotate2x2i
	push		cx
	call		near ptr rotate2x2i
	pop		bp
	pop		di
	add		bp,word ptr L$102
	cmp		bp,400H
	jl		L$33
	mov		ax,100H
	imul		di
	idiv		bp
	add		ax,64H
	mov		di,ax
	mov		ax,133H
	imul		cx
	idiv		bp
	add		ax,0a0H
	mov		si,ax
	mov		ax,word ptr 6[bx]
	cmp		si,13fH
	ja		L$33
	cmp		di,0c7H
	ja		L$33
	add		di,di
	mov		di,word ptr L$86[di]
	add		si,si
	add		di,word ptr L$87[si]
	call		ax
L$33:
	pop		bx
	pop		cx
	add		bx,8
	dec		cx
	jne		L$31
	ret
doit70:
	add		byte ptr L$106,46H
	add		byte ptr L$107,0b1H
	add		word ptr L$53,0bb8H
	mov		bx,offset L$105
	mov		ax,4dH
	call		near ptr L$12
	sar		ax,5
	mov		word ptr L$103,ax
	mov		bx,offset L$56
	mov		ax,172H
	call		near ptr L$12
	sar		ax,3
	add		ax,22b8H
	mov		word ptr L$102,ax
	mov		bx,offset L$104
	mov		ax,word ptr L$103
	call		near ptr L$12
	imul		word ptr L$54
	shrd		ax,dx,8
	mov		di,offset L$100
	call		near ptr set3drot
	mov		bx,offset L$55
	mov		ax,0c8H
	call		near ptr L$12
	sar		ax,1
	mov		di,offset L$101
	call		near ptr set3drot
	sub		word ptr L$52,5aH
	jae		L$36
	mov		word ptr L$52,3ffH
	mov		si,word ptr L$51
	mov		bl,byte ptr [si]
	and		bx,0ffH
	je		L$35
	inc		si
	mov		word ptr L$51,si
	cmp		bl,20H
	jge		L$34
	shl		bx,9
	mov		word ptr L$52,bx
	jmp		L$36
L$34:
	mov		bp,0
	mov		si,1004H
	call		near ptr letter3d
	jmp		L$36
L$35:
	mov		si,offset L$84
	mov		word ptr L$51,si
L$36:
	jmp		near ptr a_dorow
main:
	push		cs
	push		cs
	pop		ds
	pop		es
	mov		cx,2627H
	mov		di,offset L$86
	xor		ax,ax
	rep stosw
	mov		dx,0a000H
	mov		es,dx
	push		sp
	pop		dx
	cmp		dx,sp
	je		L$38
L$37:
	jmp		near ptr L$47
L$38:
	mov		bx,offset L$86
	sgdt		fword ptr [bx]
	cmp		byte ptr 5[bx],0
	js		L$37
	mov		fs,ax
	mov		ax,1a00H
	int		10H
	cmp		al,1aH
	jne		L$47
	cmp		bl,7
	jb		L$47
	mov		ax,13H
	int		10H
	mov		cx,1aH
	mov		bx,10H
	mov		ax,0e41H
L$39:
	int		10H
	inc		al
	loop		L$39
	mov		cx,500H
	mov		bx,offset L$94
	xor		di,di
L$40:
	mov		ax,word ptr es:[di]
	mov		word ptr [bx+di],ax
	add		di,2
	loop		L$40
	mov		ax,0dH
	int		10H
	mov		si,0fffeH
	mov		di,offset L$92
	mov		bl,80H
	xor		bp,bp
	jmp		L$42
L$41:
	mov		word ptr L$86[si],di
	mov		byte ptr L$88[si],bl
	mov		word ptr L$91[si],cx
	shr		cl,1
	rcr		ch,1
	mov		word ptr L$90[si],dx
	shr		dl,1
	rcr		dh,1
	mov		word ptr L$89[si],ax
	shr		al,1
	rcr		ah,1
	mov		word ptr L$87[si],bp
	ror		bl,1
	jae		L$43
	inc		bp
L$42:
	mov		cx,0feH
	mov		dx,7cH
	mov		ax,38H
L$43:
	add		di,2cH
	add		si,2
	cmp		si,280H
	jle		L$41
	mov		dx,3daH
	in		al,dx
	mov		dl,0c0H
	xor		ax,ax
	mov		cx,10H
L$44:
	out		dx,al
	out		dx,al
	inc		al
	loop		L$44
	mov		al,20H
	out		dx,al
	mov		cx,1bcH
	mov		bx,offset L$95
L$45:
	push		cx
	call		near ptr rand
	call		near ptr rand
	call		near ptr rand
	sar		ax,2
	mov		word ptr [bx],ax
	add		bx,2
	mov		word ptr [bx],offset pset1
	pop		cx
	loop		L$45
	call		near ptr a_init
	call		near ptr inittimer
L$46:
	call		near ptr doit
	mov		ah,1
	int		16H
	je		L$46
	mov		ah,0
	int		16H
	call		near ptr L$15
	call		near ptr a_init
L$47:
	mov		ax,3
	int		10H
	mov		si,offset L$85
	push		0b800H
	pop		es
	xor		di,di
	mov		ah,0eH
L$48:
	lodsb
	cmp		al,1fH
	jae		L$49
	mov		ah,al
	jmp		L$48
L$49:
	je		L$50
	stosw
	jmp		L$48
L$50:
	mov		ax,4c00H
	int		21H
L$51:
    DW	offset L$84
L$52:
    DB	1, 0
L$53:
    DB	0, 0
L$54:
    DB	40H, 0
L$55:
    DB	0e0H, 0b1H
L$56:
    DB	80H, 3eH
L$57:
    DB	0
L$58:
    DB	0cH, 18H, 24H, 30H, 3bH, 46H, 50H, 59H
    DB	62H, 69H, 70H, 75H, 79H, 7cH, 7eH, 7fH
    DB	7eH, 7cH, 79H, 75H, 70H, 69H, 62H, 59H
    DB	50H, 46H, 3bH, 30H, 24H, 18H, 0cH, 0
    DB	0f4H, 0e8H, 0dcH, 0d0H, 0c5H, 0baH, 0b0H, 0a7H
    DB	9eH, 97H, 90H, 8bH, 87H, 84H, 82H, 81H
    DB	82H, 84H, 87H, 8bH, 90H, 97H, 9eH, 0a7H
    DB	0b0H, 0baH, 0c5H, 0d0H, 0dcH, 0e8H, 0f4H, 0
    DB	3, 6, 9, 0cH, 0fH, 12H, 15H, 18H
    DB	1bH, 1eH, 21H, 24H, 27H, 2aH, 2dH, 30H
    DB	33H, 36H, 39H, 3bH, 3eH, 41H, 43H, 46H
L$59:
    DB	20H, 21H, 22H, 28H, 29H, 2aH, 30H, 31H
    DB	32H
L$60:
    DB	6bH, 21H, 81H, 21H, 98H, 21H, 0b0H, 21H
    DB	0caH, 21H, 0e5H, 21H, 2, 22H, 20H, 22H
    DB	41H, 22H, 63H, 22H, 87H, 22H, 64H, 23H
L$61:
    DB	0, 0
L$62:
    DB	0, 0, 0, 0fH, 23H, 0, 10H, 1eH
    DB	30H, 0, 20H, 2dH, 37H, 0, 3cH, 3dH
    DB	3eH
L$63:
    DB	4, 34H, 24H, 34H, 14H, 34H, 24H, 34H
    DB	1, 3, 23H, 13H, 23H, 44H, 44H, 44H
    DB	44H, 8, 2, 12H, 44H, 44H, 33H, 33H
    DB	44H, 44H, 4
L$64:
    DB	1, 44H, 33H, 44H, 22H, 44H, 33H, 44H
    DB	2
L$65:
    DW	offset L$63
L$66:
    DW	offset L$71
    DW	offset L$68
    DW	offset L$70
    DW	offset L$69
    DW	offset L$69
    DW	offset L$67
    DW	offset L$67
    DW	offset L$70
L$67:
    DB	41H, 0c2H, 6, 0, 23H, 0f2H, 0f0H, 0f0H
    DB	1, 0, 4
L$68:
    DB	91H, 40H, 87H, 80H, 0f3H, 6fH, 23H, 3
    DB	1, 1, 2
L$69:
    DB	0e1H, 21H, 11H, 80H, 11H, 13H, 22H, 22H
    DB	0, 0, 0cH
L$70:
    DB	61H, 21H, 1bH, 0, 62H, 84H, 56H, 55H
    DB	0, 0, 0eH
L$71:
    DB	91H, 40H, 87H, 88H, 0f3H, 6fH, 23H, 3
    DB	1, 1, 2
L$72:
    DW	offset L$73
    DW	offset L$74
    DW	offset L$75
    DW	offset L$76
    DW	offset L$77
    DW	offset L$78
    DW	offset L$79
    DW	offset L$79
    DW	offset L$80
    DW	offset L$80
    DW	offset L$81
    DW	offset L$81
    DW	offset L$82
    DW	offset L$82
    DW	offset L$83
    DW	offset L$83
L$73:
    DB	81H
L$74:
    DB	57H, 50H, 50H, 55H, 57H, 50H, 55H, 57H
    DB	50H, 55H, 57H, 50H, 55H, 57H, 50H, 55H
    DB	0
L$75:
    DB	81H
L$76:
    DB	50H, 55H, 57H, 50H, 55H, 57H, 50H, 55H
    DB	57H, 50H, 55H, 57H, 50H, 55H, 57H, 50H
    DB	0
L$77:
    DB	0c0H, 50H, 84H, 30H, 20H, 30H, 20H, 2aH
    DB	1aH, 2aH, 1aH, 30H, 20H, 30H, 20H, 2aH
    DB	1aH, 2aH, 1aH
L$78:
    DB	30H, 20H, 30H, 20H, 2aH, 1aH, 2aH, 1aH
    DB	25H, 15H, 25H, 15H, 28H, 18H, 2aH, 1aH
    DB	0
L$79:
    DB	0a0H, 50H, 40H, 0c0H, 40H, 88H, 40H, 40H
    DB	3aH, 42H, 90H, 45H, 88H, 40H, 42H, 40H
    DB	47H, 90H, 4aH, 88H, 45H, 98H, 40H, 0
L$80:
    DB	0a0H, 50H, 30H, 0c0H, 47H, 88H, 47H, 43H
    DB	42H, 45H, 47H, 45H, 48H, 47H, 47H, 50H
    DB	52H, 84H, 50H, 4aH, 88H, 50H, 98H, 45H
    DB	0
L$81:
    DB	0c0H, 20H, 0a0H, 10H, 10H, 90H, 10H, 2aH
    DB	25H, 88H, 28H, 2aH, 90H, 10H, 2aH, 25H
    DB	88H, 28H, 2aH, 0
L$82:
    DB	0c0H, 20H, 0a0H, 20H, 20H, 90H, 20H, 1aH
    DB	15H, 88H, 18H, 1aH, 90H, 20H, 1aH, 15H
    DB	88H, 18H, 1aH, 0
L$83:
    DB	0c0H, 0cH, 0feH, 50H, 90H, 0cH, 81H, 4aH
    DB	50H, 84H, 52H, 55H, 86H, 4aH, 81H, 50H
    DB	4aH, 86H, 50H, 82H, 55H, 98H, 45H, 0
    DB	0fcH
L$84:
    DB	1fH, 19H, 43H, 41H, 4cH, 4cH, 20H, 53H
    DB	54H, 41H, 52H, 50H, 4fH, 52H, 54H, 9
    DB	46H, 55H, 54H, 55H, 52H, 45H, 20H, 43H
    DB	52H, 45H, 57H, 20H, 57H, 4fH, 52H, 4cH
    DB	44H, 20H, 48H, 51H, 9, 43H, 44H, 4eH
    DB	9, 47H, 52H, 41H, 56H, 49H, 53H, 20H
    DB	45H, 55H, 52H, 4fH, 9, 41H, 4eH, 44H
    DB	20H, 4dH, 4fH, 52H, 45H, 0
L$85:
    DB	0fH, 53H, 74H, 61H, 72H, 50H, 6fH, 72H
    DB	74H, 3, 20H, 0c4H, 0c4H, 20H, 0bH, 56H
    DB	33H, 32H, 62H, 69H, 73H, 20H, 2bH, 33H
    DB	35H, 38H, 2dH, 30H, 2dH, 38H, 30H, 34H
    DB	34H, 36H, 32H, 36H, 20H, 2bH, 33H, 35H
    DB	38H, 2dH, 30H, 2dH, 38H, 30H, 34H, 31H
    DB	31H, 33H, 33H, 3, 20H, 0c4H, 0c4H, 20H
    DB	0fH, 46H, 43H, 2dH, 57H, 48H, 51H, 1fH
    DB	0fcH
L$86:
    DB	04FH DUP(0,0,0,0,0,0,0,0)
    DB	0, 0, 0, 0, 0, 0, 0, 0
L$87:
    DB	04FH DUP(0,0,0,0,0,0,0,0)
    DB	0, 0, 0, 0, 0, 0, 0, 0
L$88:
    DB	04FH DUP(0,0,0,0,0,0,0,0)
    DB	0, 0, 0, 0, 0, 0, 0, 0
L$89:
    DB	04FH DUP(0,0,0,0,0,0,0,0)
    DB	0, 0, 0, 0, 0, 0, 0, 0
L$90:
    DB	04FH DUP(0,0,0,0,0,0,0,0)
    DB	0, 0, 0, 0, 0, 0, 0, 0
L$91:
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0fcH, 0, 0, 0, 0, 0, 0, 0
    DB	025H DUP(0,0,0,0,0,0,0,0)
    DB	0, 0, 0, 0, 0
L$92:
    DB	05H DUP(0,0,0,0,0,0,0,0)
    DB	0, 0, 0, 0
L$93:
    DB	0477H DUP(0,0,0,0,0,0,0,0)
    DB	0, 0, 0, 0, 0, 0, 0, 0
L$94:
    DB	013FH DUP(0,0,0,0,0,0,0,0)
    DB	0, 0, 0, 0, 0, 0
L$95:
    DB	0, 0
L$96:
    DB	0, 0
L$97:
    DB	0, 0
L$98:
    DB	0, 0
L$99:
    DB	01BBH DUP(0,0,0,0,0,0,0,0)
    DB	0, 0
L$100:
    DB	0, 0, 0, 0, 0, 0, 0, 0
L$101:
    DB	0, 0, 0, 0, 0, 0, 0, 0
L$102:
    DB	0, 0
L$103:
    DB	0, 0
L$104:
    DB	0, 0
L$105:
    DB	0, 0
L$106:
    DB	0, 0
L$107:
    DB	0, 0
L$108:
    DB	0, 0
L$109:
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0
L$110:
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0
L$111:
    DB	0, 0
L$112:
    DB	0, 0
L$113:
    DB	0, 0
L$114:
    DB	0, 0, 0, 0
L$115:
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0, 0, 0, 0, 0
    DB	0, 0, 0, 0
code		ENDS
DGROUP	segment
DGROUP	ENDS
		END
