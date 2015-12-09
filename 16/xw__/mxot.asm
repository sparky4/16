.387
		PUBLIC	MXSETFONT
		PUBLIC	MXSETTEXTCOLOR
		PUBLIC	MXOUTCHAR
		PUBLIC	MXOUTTEXT
		PUBLIC	MXSETTEXTSTEP
		PUBLIC	MXGETTEXTSTEP
		EXTRN	MX_CODESEGMENT:BYTE
		EXTRN	MXPUTIMAGE:BYTE
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
L$1:
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	; The label referred to here is an undefined location
	jle		0ffffff8bH
	movsw
	and		word ptr -7e67H[di],7e7eH
	call		bx
	call		bx
	out		0ffH,ax
	jle		L$9
	????
	????
	????
	jl		L$7
	adc		byte ptr [bx+si],al
	adc		byte ptr [bx+si],bh
L$2:
	jl		L$2
	jl		L$8
	adc		byte ptr [bx+si],al
	adc		byte ptr [bx+si],bh
	adc		byte ptr -2[si],dl
	push		sp
	adc		dh,bh
	adc		byte ptr [bx+si],bh
L$3:
	jl		L$3
L$4:
	????
	jl		L$5
	inc		byte ptr [bx+si]
	sbb		byte ptr [si],bh
	jle		L$11
	cmp		al,18H
	add		bh,bh
	out		0c3H,ax
    DB	81H, 81H, 0c3H, 0e7H
L$5:
	inc		word ptr [bx+si]
	cmp		al,66H
	inc		dx
	inc		dx
    DB	66H, 3cH
L$6:
	add		bh,bh
	ret
	cwd
	mov		bp,99bdH
L$7:
	ret
	inc		word ptr [bx]
	add		ax,word ptr [di]
	; The label referred to here is an undefined location
	js		0ffffffe1H
    DB	84H
L$8:
	test		byte ptr 7cH[bx+si],bh
	add		byte ptr 7c82H[bp+si],10H
	cmp		byte ptr [bx+si],dl
	sbb		al,10H
	sbb		al,10H
	adc		byte ptr [bx+si],dl
	xor		byte ptr [bx+si],dh
	and		bh,byte ptr ds:[2222H]
	pushad
	cwd
	pop		dx
	cmp		al,0e7H
	out		3cH,ax
	pop		dx
	cwd
	add		byte ptr [bx+si],dl
	xor		byte ptr -10H[bx+si],dh
L$9:
	jo		L$10
	adc		byte ptr [bx+si],al
	add		al,0e0H
	lock loopnz	L$6
	adc		byte ptr [bx+si],38H
	push		sp
	adc		byte ptr [bx+si],dl
	push		sp
	cmp		byte ptr [bx+si],dl
	dec		ax
	dec		ax
	dec		ax
	dec		ax
	dec		ax
	add		byte ptr [bx+si],cl
	jle		L$4
	xchg		ax,dx
	jb		L$10
	adc		dl,byte ptr [bp+si]
	add		byte ptr [si],bh
	and		bl,byte ptr [bx+si]
	and		al,24H
	sbb		byte ptr 3cH[si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
    DB	0, 3eH
    DD	ds:[3eH]
L$10:
	add		byte ptr [bx+si],bh
	push		sp
    DB	10H
L$11:
	adc		byte ptr [bx+si],dl
	push		sp
	cmp		dh,bh
	add		byte ptr [bx+si],dl
	cmp		byte ptr 10H[si],dl
	adc		byte ptr [bx+si],dl
	add		byte ptr [bx+si],al
	adc		byte ptr [bx+si],dl
	adc		byte ptr 38H[si],dl
	adc		byte ptr [bx+si],al
	add		byte ptr [bx+si],cl
	add		al,0feH
	add		al,8
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],ah
	inc		ax
	inc		byte ptr 20H[bx+si]
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr -380H[bx+si],0
	add		byte ptr [bx+si],al
	and		al,42H
	inc		word ptr 24H[bp+si]
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	adc		byte ptr [bx+si],bh
L$12:
	jl		L$12
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	????
	jl		L$15
	adc		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],dl
	add		byte ptr [bx+si],dl
	add		byte ptr [bx+si],al
	and		al,24H
	add		byte ptr [bx+si],al
    DB	0
L$13:
	add		byte ptr [bx+si],al
	and		al,24H
	jle		140H
	jle		142H
L$14:
	and		al,0
	cmp		byte ptr 50H[si],dl
	cmp		byte ptr [si],dl
	push		sp
	cmp		byte ptr [bx+si],dl
	add		byte ptr [bp+si],al
	inc		sp
	or		byte ptr [bx+si],dl
	and		byte ptr [bp+si],al
	cmp		byte ptr 38H[si],al
	pusha
	xchg		ax,sp
L$15:
    DB	88H
L$16:
	je		138H
	and		byte ptr [bx+si],ah
	inc		ax
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],dl
	and		byte ptr 40H[bx+si],al
	inc		ax
	and		byte ptr [bx+si],dl
	add		byte ptr 20H[bx+si],al
	adc		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],ah
L$17:
	inc		ax
	add		byte ptr [bx+si],al
	and		al,18H
	jle		L$18
	and		al,0
	add		byte ptr [bx+si],al
	adc		byte ptr [bx+si],dl
	jl		L$18
	adc		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	adc		byte ptr [bx+si],dl
	and		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	cld
    DB	0
L$18:
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],dl
	add		byte ptr [bx+si],al
	add		al,8
	adc		byte ptr [bx+si],ah
	inc		ax
L$19:
	add		byte ptr [bx+si],7cH
	????
    DB	8aH, 92H
L$20:
	mov		byte ptr 0c6H,al
    DD	L$21
L$21:
	adc		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],bh
	add		byte ptr L$13[bx+si],bh
	add		al,18H
	pusha
	cmp		ah,0
	js		L$14
	add		al,38H
	add		al,84H
	js		L$22
L$22:
	sbb		al,24H
	inc		sp
	test		dh,bh
	add		al,0eH
	add		ah,bh
	cmp		al,4
	add		al,84H
	js		L$23
L$23:
	js		L$16
	cmp		al,84H
	test		byte ptr [bx+si],bh
	cld
	add		al,4
	or		byte ptr [bx+si],dl
	and		byte ptr [bx+si],ah
	add		byte ptr -7cH[bx+si],bh
	test		byte ptr -7cH[bx+si],bh
	test		byte ptr [bx+si],bh
	js		L$17
	test		byte ptr 4[si],bh
	test		byte ptr [bx+si],bh
	add		byte ptr [bx+si],al
	adc		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	adc		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	adc		byte ptr [bx+si],al
	add		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],ah
	or		byte ptr [bx+si],dl
	and		byte ptr 20H[bx+si],al
	adc		byte ptr [bx+si],cl
	add		byte ptr [bx+si],al
	add		ah,bh
	add		byte ptr [bx+si],al
	cld
	add		byte ptr [bx+si],al
	inc		ax
	and		byte ptr [bx+si],dl
	or		byte ptr [bx+si],dl
	and		byte ptr [bx+si],al
	js		L$19
	add		al,8
	adc		byte ptr [bx+si],al
L$24:
	adc		byte ptr [bx+si],al
	jl		L$20
	mov		dx,0bea6H
	cmp		byte ptr [si],78H
	test		byte ptr -7b04H[si],al
	test		byte ptr -800H[si],al
	test		byte ptr -7b08H[si],al
	test		al,bh
	add		byte ptr -7cH[bx+si],bh
	add		byte ptr -7b80H[bx+si],78H
	add		al,dh
	mov		byte ptr -7b7cH[si],al
	mov		al,dh
	add		ah,bh
	add		byte ptr -7f10H[bx+si],80H
	cld
	add		ah,bh
	add		byte ptr -7f10H[bx+si],80H
	add		byte ptr [bx+si],78H
	test		byte ptr -7b64H[bx+si],al
	test		byte ptr [bx+si],bh
	test		byte ptr -37cH[si],al
	test		byte ptr 84H[si],al
	cmp		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],dl
	cmp		byte ptr [bx+si],al
	sbb		al,8
	or		byte ptr [bx+si],cl
	mov		byte ptr 70H[bx+si],cl
	test		byte ptr -1f70H[bx+si],cl
	nop
	mov		byte ptr -8000H[si],al
	add		byte ptr -7f80H[bx+si],80H
	cld
	add		dh,al
	stosb
	xchg		ax,dx
	add		byte ptr -7d7eH[bp+si],0
	add		dl,0a2H
	xchg		ax,dx
	mov		al,byte ptr 82H[bp]
	js		L$24
	test		byte ptr -7b7cH[si],al
	js		L$25
L$25:
	clc
	test		byte ptr -7f08H[si],al
	add		byte ptr 7800H[bx+si],84H
	test		byte ptr -776cH[si],al
	jbe		L$26
L$26:
	clc
	test		byte ptr -6f08H[si],al
	mov		byte ptr 7800H[si],al
	test		byte ptr 478H[bx+si],al
	test		byte ptr [bx+si],bh
	????
	adc		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],dl
	add		byte ptr -7b7cH[si],al
	test		byte ptr 7884H[si],al
	add		byte ptr -7b7cH[si],al
	test		byte ptr 3048H[si],al
	add		byte ptr -7d7eH[bp+si],al
	adc		byte ptr -3956H[bp+si],0
	add		byte ptr 28H[si],10H
	sub		byte ptr -7eH[si],al
	add		byte ptr 44H[si],al
	inc		sp
	cmp		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],dl
	add		dh,bh
	add		al,8
	adc		byte ptr [bx+si],ah
	inc		ax
	inc		byte ptr [bx+si]
	js		L$32
	inc		ax
	inc		ax
	inc		ax
	inc		ax
	js		L$27
L$27:
	add		byte ptr 2040H[bx+si],al
	adc		byte ptr [bx+si],cl
	add		al,0
	js		L$29
	or		byte ptr [bx+si],cl
	or		byte ptr [bx+si],cl
	js		L$28
L$28:
	adc		byte ptr [bx+si],ch
L$29:
	inc		sp
	add		byte ptr [bx+si],0
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		bh,bh
	and		byte ptr [bx+si],ah
	adc		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	cmp		byte ptr [si],al
	cmp		al,44H
	jl		L$30
L$30:
	add		byte ptr 40H[bx+si],al
	js		L$33
	inc		sp
	js		L$31
L$31:
	add		byte ptr [bx+si],al
L$32:
	cmp		al,40H
	inc		ax
	inc		ax
	cmp		al,0
	add		byte ptr [si],al
	add		al,3cH
	inc		sp
	inc		sp
	cmp		al,0
	add		byte ptr [bx+si],al
	cmp		byte ptr 7cH[si],al
	inc		ax
	cmp		al,0
	add		byte ptr [si],cl
	adc		byte ptr [si],bh
	adc		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	cmp		al,44H
	inc		sp
	cmp		al,4
	cmp		byte ptr [bx+si],al
	inc		ax
	inc		ax
	js		L$35
	inc		sp
	inc		sp
	add		byte ptr [bx+si],al
	adc		byte ptr [bx+si],al
	adc		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],dl
	add		byte ptr [bx+si],al
	add		al,0
	add		al,4
	add		al,44H
	cmp		byte ptr [bx+si],al
L$33:
	inc		ax
	inc		ax
	push		ax
	pusha
	push		ax
	dec		ax
	add		byte ptr [bx+si],al
	xor		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],dl
	add		byte ptr [bx+si],al
	add		byte ptr 54H[bx+si],ch
	push		sp
	inc		sp
	inc		sp
	add		byte ptr [bx+si],al
	add		byte ptr 44H[bx+si],bh
	inc		sp
	inc		sp
	inc		sp
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],bh
	inc		sp
	inc		sp
	inc		sp
	cmp		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
L$34:
	js		L$42
	inc		sp
	js		L$41
	inc		ax
    DB	0
L$35:
	add		byte ptr [si],bh
	inc		sp
	inc		sp
	cmp		al,4
	add		al,0
	add		byte ptr 60H[si],bl
	inc		ax
	inc		ax
	inc		ax
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],bh
L$36:
	inc		ax
	jl		L$38
	jl		L$37
L$37:
	add		byte ptr [bx+si],dl
L$38:
	cmp		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],dl
	sbb		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	inc		sp
	inc		sp
	inc		sp
	inc		sp
	cmp		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	inc		sp
	inc		sp
	inc		sp
	sub		byte ptr [bx+si],dl
	add		byte ptr [bx+si],al
	add		byte ptr 44H[si],al
L$39:
	push		sp
	push		sp
	insb
	add		byte ptr [bx+si],al
	add		byte ptr 28H[si],al
L$40:
	adc		byte ptr [bx+si],ch
	inc		sp
L$41:
    DB	0
L$42:
	add		byte ptr [bx+si],al
	inc		sp
	inc		sp
	inc		sp
	cmp		al,4
	jl		L$43
L$43:
	add		byte ptr 4[si],bh
	cmp		byte ptr 7cH[bx+si],al
	add		byte ptr [bx+si],al
	or		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],dh
	adc		byte ptr [bx+si],dl
	or		byte ptr [bx+si],al
	adc		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],al
	adc		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],al
	and		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],bl
	adc		byte ptr [bx+si],dl
	and		byte ptr -68H[si],ah
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],dl
	sub		byte ptr -7eH[si],al
	cmp		dh,0
	jl		L$34
	add		byte ptr 7c80H[bx+si],4
	jl		L$44
L$44:
	sub		byte ptr [bx+si],al
	inc		sp
	inc		sp
	inc		sp
	cmp		byte ptr [bx+si],al
	cmp		al,0
	jl		L$54
	jl		L$53
	jl		L$45
L$45:
	jle		L$36
	cmp		byte ptr [si],al
	cmp		al,44H
	jl		L$46
L$46:
	and		al,0
L$47:
	cmp		byte ptr [si],al
	cmp		al,44H
	jl		L$48
L$48:
	js		L$49
L$49:
	cmp		byte ptr [si],al
	cmp		al,44H
	jl		L$50
L$50:
	sbb		byte ptr [bx+si],bl
	cmp		byte ptr [si],al
	cmp		al,44H
	jl		L$51
L$51:
	add		byte ptr [bx+si],al
	js		L$39
	cmp		byte ptr 8[bx+si],38H
	jl		L$40
	cmp		byte ptr 7cH[si],al
	inc		ax
	cmp		al,0
	dec		ax
	add		byte ptr [bx+si],bh
	inc		sp
	jl		L$55
	cmp		al,0
	js		L$52
L$52:
	cmp		byte ptr 7cH[si],al
	inc		ax
L$53:
	cmp		al,0
L$54:
	add		byte ptr [bx+si],ch
	add		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],al
	adc		byte ptr [bx+si],ch
	add		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],al
	add		byte ptr [bx+si],dh
	add		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],al
	dec		ax
	add		byte ptr -7cH[bx+si],bh
	cld
	test		byte ptr 3000H[si],al
	xor		byte ptr -7cH[bx+si],bh
	cld
	test		byte ptr 3800H[si],al
	add		ah,bh
	xor		al,80H
	cld
	add		byte ptr [bx+si],al
	add		byte ptr 8[bp],bh
	jle		L$59
L$55:
	jle		L$56
L$56:
	jle		L$47
	nop
	cld
	nop
	nop
	sahf
	add		byte ptr -7eH[si],bh
	cmp		byte ptr 44H[si],al
	inc		sp
	cmp		byte ptr [bx+si],al
	sub		byte ptr [bx+si],al
	cmp		byte ptr 44H[si],al
	inc		sp
	cmp		byte ptr [bx+si],al
	jo		L$57
L$57:
	cmp		byte ptr 44H[si],al
	inc		sp
	cmp		byte ptr [bx+si],al
	cmp		byte ptr [si],al
	inc		sp
	inc		sp
	inc		sp
	cmp		byte ptr [bx+si],al
	jo		L$58
L$58:
	inc		sp
	inc		sp
	inc		sp
	inc		sp
	cmp		byte ptr [bx+si],al
	sub		byte ptr [bx+si],al
	inc		sp
	inc		sp
	inc		sp
	cmp		al,4
	jl		L$64
	add		byte ptr -7cH[bx+si],bh
	test		byte ptr 78H[si],al
	dec		ax
	add		byte ptr -7b7cH[si],al
    DB	84H
L$59:
	js		4d8H
	add		byte ptr [bx+si],dl
	cmp		byte ptr 40H[bx+si],al
	inc		ax
	cmp		byte ptr [bx+si],dl
	cmp		byte ptr 40H[si],al
	loopnz		L$66
	inc		ax
	cmp		ah,44H
	jl		L$60
	jl		L$61
	jl		L$62
	add		al,dh
	mov		byte ptr -7d09H[bp+si],cl
	add		byte ptr 0c00H[bp+si],12H
    DB	10H
L$60:
	sbb		byte ptr [bx+si],dh
L$61:
    DB	10H, 90H
L$62:
	pusha
	cmp		al,0
	cmp		byte ptr [si],al
	cmp		al,44H
	jl		L$63
L$63:
	add		byte ptr [bx+si],bl
	add		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],al
    DB	1cH
L$64:
	add		byte ptr [bx+si],bh
	inc		sp
	inc		sp
	inc		sp
	cmp		byte ptr [bx+si],al
	sbb		al,0
	inc		sp
	inc		sp
	inc		sp
	inc		sp
	cmp		byte ptr [bx+si],al
	jl		L$65
L$65:
	js		L$67
	inc		sp
L$66:
	inc		sp
	inc		sp
	add		byte ptr [si],bh
	inc		sp
	push		sp
	dec		sp
	inc		sp
	add		byte ptr [bx+si],bl
	and		al,24H
	push		ds
	add		byte ptr ds:[0],bh
	sbb		al,22H
	and		bl,byte ptr [si]
	add		byte ptr ds:[0],bh
	adc		byte ptr [bx+si],al
	adc		byte ptr [bx+si],ah
	inc		ax
	inc		dx
	cmp		al,0
	add		byte ptr [bx+si],al
	add		ah,bh
	add		byte ptr [bx+si],0
	add		byte ptr [bx+si],al
	cld
	add		al,4
	add		byte ptr [bx+si],al
	inc		ax
	inc		sp
	dec		ax
	push		di
	and		word ptr 4[bx],ax
	pop		es
	inc		ax
	inc		sp
	dec		ax
	push		dx
	dec		dx
	pop		ds
    DB	2
L$67:
	adc		byte ptr [bx+si],al
	adc		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],dl
L$68:
	adc		byte ptr [bx+si],al
	add		byte ptr [si],ah
L$69:
	dec		ax
	nop
	dec		ax
	and		al,0
	add		byte ptr [bx+si],al
	dec		ax
	and		al,12H
	and		al,48H
	add		byte ptr [bx+si],al
	and		cl,byte ptr -77deH[bx+si]
	and		cl,byte ptr -77deH[bx+si]
	push		bp
	stosb
	push		bp
	stosb
	push		bp
	stosb
	push		bp
	stosb
	????
	ja		L$68
	out		dx,al
	????
	ja		L$69
	out		dx,al
	sbb		byte ptr [bx+si],bl
	sbb		byte ptr [bx+si],bl
	sbb		byte ptr [bx+si],bl
	sbb		byte ptr [bx+si],bl
	sbb		byte ptr [bx+si],bl
	sbb		byte ptr [bx+si],bl
	clc
	sbb		byte ptr [bx+si],bl
	sbb		byte ptr [bx+si],bl
	sbb		al,bh
	sbb		al,bh
	sbb		byte ptr [bx+si],bl
	sbb		byte ptr ds:[3636H],dh
	div		byte ptr ss:[3636H]
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	????
	add		byte ptr ss:[bx+si],al
	clc
	sbb		al,bh
	sbb		byte ptr [bx+si],bl
	sbb		byte ptr ds:[0f636H],dh
	push		es
	div		byte ptr ds:[3636H]
	add		byte ptr ss:[bx+si],al
	inc		byte ptr ds:[36f6H]
	test		byte ptr ss:[0feH],0
	add		byte ptr ds:[3636H],dh
	inc		byte ptr ss:[bx+si]
	add		byte ptr [bx+si],al
	sbb		byte ptr [bx+si],bl
	clc
	sbb		al,bh
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		al,bh
	sbb		byte ptr [bx+si],bl
	sbb		byte ptr [bx+si],bl
	sbb		byte ptr [bx+si],bl
	sbb		byte ptr [bx],bl
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],bl
	sbb		byte ptr [bx+si],bl
	sbb		bh,bh
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		bh,bh
	sbb		byte ptr [bx+si],bl
	sbb		byte ptr [bx+si],bl
	sbb		byte ptr [bx+si],bl
	sbb		byte ptr [bx],bl
	sbb		byte ptr [bx+si],bl
	sbb		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		bh,bh
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],bl
	sbb		byte ptr [bx+si],bl
	sbb		bh,bh
	sbb		byte ptr [bx+si],bl
	sbb		byte ptr [bx+si],bl
	sbb		byte ptr [bx],bl
	sbb		byte ptr [bx],bl
	sbb		byte ptr [bx+si],bl
	sbb		byte ptr ds:[3636H],dh
	aaa
	aaa
	xor		byte ptr [bx],bh
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx],bh
	xor		byte ptr [bx],dh
	test		word ptr ss:[bx+si],0ffH
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	inc		word ptr [bx+si]
	div		word ptr ds:[3636H]
	aaa
	xor		byte ptr [bx],dh
	add		byte ptr ss:[bx+si],al
	inc		word ptr [bx+si]
	inc		word ptr [bx+si]
	add		byte ptr [bx+si],al
	test		word ptr ss:[bx+si],36f7H
	sbb		byte ptr ss:[bx+si],bl
	inc		word ptr [bx+si]
	inc		word ptr [bx+si]
	add		byte ptr [bx+si],al
	inc		word ptr ss:[bx+si]
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	inc		word ptr [bx+si]
	call		dword ptr [bx+si]
	sbb		byte ptr [bx+si],bl
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	push		word ptr ds:[3636H]
	aas
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],bl
	sbb		byte ptr [bx],bl
	sbb		byte ptr [bx],bl
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx],bl
	sbb		byte ptr [bx],bl
	sbb		byte ptr [bx+si],bl
	sbb		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx],bh
	push		word ptr ss:[3636H]
	sbb		byte ptr [bx+si],bl
	call		dword ptr [bx+si]
	call		dword ptr [bx+si]
	sbb		byte ptr [bx+si],bl
	sbb		byte ptr [bx+si],bl
	sbb		byte ptr [bx+si],bl
	clc
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx],bl
    DB	18H
L$70:
	sbb		byte ptr [bx+si],bl
	????
	????
	????
	????
	????
	????
	????
	inc		word ptr [bx+si]
	add		byte ptr [bx+si],al
	add		bh,bh
	????
	????
	push		ax
	????
	????
	????
	????
	????
	????
	????
	????
	????
	????
	????
	????
	????
	????
	????
	????
	????
	????
	inc		word ptr [bx+si]
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr -6cH[bp+si],ah
	mov		byte ptr 62H[si],dl
	add		al,dh
	mov		al,dh
	mov		byte ptr -7f10H[bx+si],cl
	add		al,bh
	mov		byte ptr -7f80H[bx+si],al
	add		byte ptr [bx+si],0
	cld
	dec		ax
	dec		ax
	dec		ax
	dec		ax
	dec		ax
	add		ah,bh
	test		byte ptr 20H[bx+si],al
	inc		ax
	test		ah,bh
	add		byte ptr [si],bh
	inc		ax
	cmp		byte ptr 44H[si],al
	inc		sp
	cmp		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	inc		sp
	inc		sp
	inc		sp
	js		L$71
	inc		ax
	add		byte ptr ds:[848H],dh
	or		byte ptr [bx+si],cl
	or		byte ptr [bx+si],al
	cmp		byte ptr [bx+si],dl
	cmp		byte ptr 44H[si],al
	cmp		byte ptr [bx+si],dl
	cmp		byte ptr -7cH[bx+si],bh
	test		ah,bh
	test		byte ptr 78H[si],al
	js		L$70
	test		byte ptr 4848H[si],al
	int		3
	add		byte ptr 4[bx+si],bh
	cmp		byte ptr 44H[si],al
	inc		sp
	cmp		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	insb
	xchg		ax,dx
	xchg		ax,dx
	insb
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	cmp		al,byte ptr 5aH[si]
	and		bl,byte ptr [si]
	sbb		byte ptr [bx+si],ah
	inc		ax
	js		L$73
	and		byte ptr [bx+si],bl
L$71:
	add		byte ptr -7cH[bx+si],bh
	test		byte ptr -7b7cH[si],al
	test		byte ptr [bx+si],al
	add		ah,bh
	add		ah,bh
	add		ah,bh
	add		byte ptr [bx+si],al
	and		byte ptr [bx+si],ah
	clc
	and		byte ptr [bx+si],ah
	add		al,bh
	add		byte ptr [bx+si],ah
	adc		byte ptr [bx+si],cl
	adc		byte ptr [bx+si],ah
	add		byte ptr [si],bh
	or		byte ptr [bx+si],dl
	and		byte ptr [bx+si],dl
	or		byte ptr [bx+si],al
	jl		L$72
L$72:
	or		al,12H
	adc		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],dl
	adc		byte ptr [bx+si],dl
	nop
	pusha
	add		byte ptr [bx+si],dl
	add		byte ptr [si],bh
L$73:
	adc		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	xor		cl,byte ptr [si]
	xor		cl,byte ptr [si]
	add		byte ptr [bx+si],bh
	inc		sp
	inc		sp
	cmp		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	sbb		byte ptr [bx+si],bl
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],bl
	add		byte ptr [bx+si],al
	add		byte ptr ds:[808H],cl
	or		byte ptr 28H[bx+si],cl
	sbb		byte ptr [bx+si],cl
	cmp		byte ptr [si],ah
	and		al,24H
	and		al,0
	add		byte ptr [bx+si],al
	cmp		al,4
	cmp		al,20H
	cmp		al,0
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	cmp		al,3cH
	cmp		al,3cH
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
	add		byte ptr [bx+si],al
L$74:
    DW	offset L$1
L$75:
    DB	8
L$76:
	add		byte ptr [bx+si],cl
    DB	0
L$77:
    DW	offset L$1
L$78:
    DW	seg L$1
L$79:
	or		byte ptr [bx+si],al
L$80:
	or		byte ptr [bx+si],al
L$81:
	or		byte ptr [bx+si],al
L$82:
	inc		word ptr [bx+si]
L$83:
	add		byte ptr [bx+si],al
L$84:
	or		byte ptr [bx+si],al
L$85:
	add		byte ptr [bx+si],al
MXSETFONT:
	push		bp
	mov		bp,sp
	sub		sp,0
	push		ds
	mov		ds,word ptr cs:MX_CODESEGMENT
	mov		ax,word ptr 0cH[bp]
	test		ax,ax
	jne		L$87
	mov		ax,word ptr 0aH[bp]
	cmp		ax,1
	jb		L$86
	xor		ax,ax
L$86:
	shl		ax,1
	shl		ax,1
	mov		bx,ax
	mov		ax,word ptr L$74[bx]
	mov		word ptr L$77,ax
	mov		word ptr L$78,cs
	mov		al,byte ptr L$75[bx]
	xor		ah,ah
	mov		word ptr L$79,ax
	mov		word ptr L$84,ax
	mov		dl,byte ptr L$76[bx]
	xor		dh,dh
	mov		word ptr L$80,dx
	mul		dx
	mov		word ptr L$81,ax
	mov		word ptr L$84,ax
	xor		ax,ax
	mov		word ptr L$85,ax
	jmp		L$88
L$87:
	mov		ax,0ffffH
	mov		bx,word ptr 8[bp]
	cmp		bx,10H
	ja		L$88
	mov		dx,word ptr 6[bp]
	cmp		dx,20H
	ja		L$88
	mov		word ptr L$79,bx
	mov		word ptr L$80,dx
	mov		ax,bx
	add		ax,7
	shr		ax,1
	shr		ax,1
	shr		ax,1
	mul		dx
	mov		word ptr L$81,ax
	mov		ax,word ptr 0aH[bp]
	mov		word ptr L$77,ax
	mov		ax,word ptr 0cH[bp]
	mov		word ptr L$78,ax
	xor		ax,ax
L$88:
	pop		ds
	mov		sp,bp
	pop		bp
	retf		8
MXSETTEXTCOLOR:
	push		bp
	mov		bp,sp
	sub		sp,0
	push		ds
	mov		ds,word ptr cs:MX_CODESEGMENT
	mov		ax,word ptr 8[bp]
	mov		word ptr L$82,ax
	mov		ax,word ptr 6[bp]
	mov		word ptr L$83,ax
	xor		ax,ax
	pop		ds
	mov		sp,bp
	pop		bp
	retf		4
MXOUTCHAR:
	push		bp
	mov		bp,sp
	sub		sp,202H
	push		ds
	push		si
	push		es
	push		di
	lds		si,dword ptr cs:L$77
	mov		al,byte ptr 6[bp]
	xor		ah,ah
	mul		word ptr cs:L$81
	add		si,ax
	mov		ax,ss
	mov		es,ax
	lea		di,-200H[bp]
	mov		dx,word ptr cs:L$82
	mov		ax,word ptr cs:L$80
	mov		word ptr -202H[bp],ax
L$89:
	mov		cx,word ptr cs:L$79
	mov		bh,byte ptr [si]
	inc		si
	cmp		cx,8
	jbe		L$90
	mov		bl,byte ptr [si]
	inc		si
L$90:
	mov		al,dl
	shl		bx,1
	jb		L$91
	mov		al,dh
L$91:
	mov		byte ptr es:[di],al
	inc		di
	dec		cx
	jne		L$90
	dec		word ptr -202H[bp]
	jne		L$89
	lea		ax,-200H[bp]
	push		es
	push		ax
	push		word ptr 0aH[bp]
	push		word ptr 8[bp]
	push		word ptr cs:L$79
	push		word ptr cs:L$80
	push		word ptr cs:L$83
	push		cs
	call		near ptr MX_TEXT:MXPUTIMAGE
	xor		ax,ax
	pop		di
	pop		es
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	retf		6
MXOUTTEXT:
	push		bp
	mov		bp,sp
	sub		sp,0
	push		ds
	push		si
	lds		si,dword ptr 6[bp]
L$92:
	mov		al,byte ptr [si]
	test		al,al
	je		L$93
	inc		si
	push		word ptr 0cH[bp]
	push		word ptr 0aH[bp]
	push		ax
	push		cs
	call		near ptr MXOUTCHAR
	mov		ax,word ptr cs:L$84
	add		word ptr 0cH[bp],ax
	mov		ax,word ptr cs:L$85
	add		word ptr 0aH[bp],ax
	dec		word ptr -202H[bp]
	jne		L$92
L$93:
	xor		ax,ax
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	retf		8
	retf
MXSETTEXTSTEP:
	push		bp
	mov		bp,sp
	sub		sp,0
	push		ds
	mov		ds,word ptr cs:MX_CODESEGMENT
	mov		ax,word ptr 8[bp]
	mov		word ptr L$84,ax
	mov		ax,word ptr 6[bp]
	mov		word ptr L$85,ax
	pop		ds
	mov		sp,bp
	pop		bp
	retf		4
MXGETTEXTSTEP:
	push		bp
	mov		bp,sp
	sub		sp,0
	push		ds
	push		si
	mov		ax,word ptr cs:L$84
	lds		si,dword ptr 0aH[bp]
	mov		word ptr [si],ax
	mov		ax,word ptr cs:L$85
	lds		si,dword ptr 6[bp]
	mov		word ptr [si],ax
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	retf		8
MX_TEXT		ENDS
		END
