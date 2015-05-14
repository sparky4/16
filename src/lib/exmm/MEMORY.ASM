.387
		PUBLIC	_OpenEMM
		PUBLIC	_CloseEMM
		PUBLIC	_EMMInstalled
		PUBLIC	_EMMCoreLeft
		PUBLIC	_EMMalloc
		PUBLIC	_EMMRealloc
		PUBLIC	_EMMFree
		PUBLIC	_MapEMM
		PUBLIC	_UnmapEMM
		PUBLIC	_UseEMM
		PUBLIC	_SaveEMM
		EXTRN	`F_LXLSH@`:BYTE
		EXTRN	`F_SCOPY@`:BYTE
MEMORY_TEXT		SEGMENT	BYTE PUBLIC USE16 'CODE'
		ASSUME CS:MEMORY_TEXT, DS:DGROUP, SS:DGROUP
_OpenEMM:
    push        bp 
    mov         bp,sp 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    call        far ptr L$65 
    or          ax,ax 
    je          L$1 
    call        far ptr L$72 
    or          ax,ax 
    jne         L$3 
L$1:
    xor         ax,ax 
L$2:
    jmp         L$5 
L$3:
    call        far ptr L$76 
    mov         word ptr L$99,ax 
    or          ax,ax 
    jne         L$4 
    jmp         L$1 
L$4:
    mov         ax,1 
    jmp         L$2 
L$5:
    pop         ds 
    pop         bp 
    retf        
_CloseEMM:
    push        bp 
    mov         bp,sp 
    push        si 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    cmp         word ptr L$99,0 
    jne         L$6 
    jmp         L$10 
L$6:
    xor         si,si 
    jmp         L$9 
L$7:
    mov         bx,si 
    shl         bx,1 
    cmp         word ptr L$100[bx],0 
    je          L$8 
    mov         bx,si 
    shl         bx,1 
    push        word ptr L$100[bx] 
    call        far ptr L$87 
    pop         cx 
    mov         bx,si 
    shl         bx,1 
    mov         word ptr L$100[bx],0 
L$8:
    inc         si 
L$9:
    cmp         si,40H 
    jl          L$7 
    mov         word ptr L$99,0 
L$10:
    pop         ds 
    pop         si 
    pop         bp 
    retf        
_EMMInstalled:
    push        bp 
    mov         bp,sp 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    cmp         word ptr L$99,0 
    je          L$11 
    mov         ax,1 
    jmp         L$12 
L$11:
    xor         ax,ax 
L$12:
    jmp         L$13 
L$13:
    pop         ds 
    pop         bp 
    retf        
_EMMCoreLeft:
    push        bp 
    mov         bp,sp 
    sub         sp,6 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    mov         word ptr -4[bp],0 
    mov         word ptr -6[bp],0 
    mov         ah,42H 
    int         67H 
    or          ah,ah 
    js          L$14 
    mov         word ptr -2[bp],bx 
    mov         ax,word ptr -2[bp] 
    xor         dx,dx 
    mov         cl,0eH 
    call        far ptr `F_LXLSH@` 
    mov         word ptr -4[bp],dx 
    mov         word ptr -6[bp],ax 
L$14:
    mov         dx,word ptr -4[bp] 
    mov         ax,word ptr -6[bp] 
    jmp         L$15 
L$15:
    pop         ds 
    mov         sp,bp 
    pop         bp 
    retf        
_EMMalloc:
    push        bp 
    mov         bp,sp 
    sub         sp,4 
    push        si 
    push        di 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    mov         di,word ptr 0aH[bp] 
    mov         word ptr -2[bp],0 
    mov         word ptr -4[bp],0 
    cmp         word ptr L$99,0 
    jne         L$18 
    les         bx,dword ptr 6[bp] 
    mov         word ptr es:[bx],0 
L$16:
    xor         dx,dx 
    xor         ax,ax 
L$17:
    jmp         L$26 
L$18:
    cmp         di,1 
    jl          L$19 
    cmp         di,3fcH 
    jle         L$20 
L$19:
    les         bx,dword ptr 6[bp] 
    mov         word ptr es:[bx],0f440H 
    jmp         L$16 
L$20:
    xor         si,si 
    jmp         L$22 
L$21:
    inc         si 
L$22:
    cmp         si,40H 
    jge         L$23 
    mov         bx,si 
    shl         bx,1 
    cmp         word ptr L$100[bx],0 
    jne         L$21 
L$23:
    cmp         si,40H 
    jne         L$24 
    les         bx,dword ptr 6[bp] 
    mov         word ptr es:[bx],0f443H 
    jmp         L$16 
L$24:
    push        di 
    call        far ptr L$80 
    pop         cx 
    mov         bx,si 
    shl         bx,1 
    mov         word ptr L$100[bx],ax 
    or          ax,ax 
    jle         L$25 
    mov         ax,word ptr L$99 
    mov         word ptr -2[bp],ax 
    mov         word ptr -4[bp],0 
L$25:
    mov         bx,si 
    shl         bx,1 
    mov         ax,word ptr L$100[bx] 
    les         bx,dword ptr 6[bp] 
    mov         word ptr es:[bx],ax 
    mov         dx,word ptr -2[bp] 
    mov         ax,word ptr -4[bp] 
    jmp         L$17 
L$26:
    pop         ds 
    pop         di 
    pop         si 
    mov         sp,bp 
    pop         bp 
    retf        
_EMMRealloc:
    push        bp 
    mov         bp,sp 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    xor         cx,cx 
    cmp         word ptr L$99,0 
    je          L$27 
    cmp         word ptr 8[bp],0 
    jl          L$27 
    cmp         word ptr 8[bp],3fcH 
    jle         L$29 
L$27:
    xor         ax,ax 
L$28:
    jmp         L$31 
L$29:
    mov         ah,51H 
    mov         bx,word ptr 8[bp] 
    mov         dx,word ptr 6[bp] 
    int         67H 
    or          ah,ah 
    js          L$30 
    mov         cx,1 
L$30:
    mov         ax,cx 
    jmp         L$28 
L$31:
    pop         ds 
    pop         bp 
    retf        
_EMMFree:
    push        bp 
    mov         bp,sp 
    push        si 
    push        di 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    cmp         word ptr L$99,0 
    jne         L$32 
    jmp         L$40 
L$32:
    xor         si,si 
    jmp         L$34 
L$33:
    inc         si 
L$34:
    cmp         si,40H 
    jge         L$35 
    mov         bx,si 
    shl         bx,1 
    mov         ax,word ptr L$100[bx] 
    cmp         ax,word ptr 6[bp] 
    jne         L$33 
L$35:
    cmp         si,40H 
    jl          L$36 
    jmp         L$40 
L$36:
    mov         di,10H 
    jmp         L$38 
L$37:
    mov         bx,si 
    shl         bx,1 
    push        word ptr L$100[bx] 
    call        far ptr L$87 
    pop         cx 
    or          ax,ax 
    je          L$38 
    jmp         L$39 
L$38:
    mov         ax,di 
    dec         di 
    or          ax,ax 
    jne         L$37 
L$39:
    mov         bx,si 
    shl         bx,1 
    mov         word ptr L$100[bx],0 
L$40:
    pop         ds 
    pop         di 
    pop         si 
    pop         bp 
    retf        
_MapEMM:
    push        bp 
    mov         bp,sp 
    push        si 
    push        di 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    mov         di,word ptr 0aH[bp] 
    cmp         word ptr L$99,0 
    jne         L$43 
L$41:
    xor         ax,ax 
L$42:
    jmp         L$53 
L$43:
    xor         si,si 
    jmp         L$45 
L$44:
    inc         si 
L$45:
    cmp         si,40H 
    jge         L$46 
    mov         bx,si 
    shl         bx,1 
    mov         ax,word ptr L$100[bx] 
    cmp         ax,word ptr 6[bp] 
    jne         L$44 
L$46:
    cmp         si,40H 
    jne         L$47 
    jmp         L$41 
L$47:
    push        word ptr 6[bp] 
    call        far ptr L$91 
    pop         cx 
    cmp         ax,di 
    jl          L$48 
    cmp         di,1 
    jl          L$48 
    cmp         di,4 
    jle         L$49 
L$48:
    mov         ax,0f440H 
    jmp         L$42 
L$49:
    mov         si,word ptr 8[bp] 
    jmp         L$52 
L$50:
    mov         ax,si 
    sub         ax,word ptr 8[bp] 
    push        ax 
    push        si 
    push        word ptr 6[bp] 
    call        far ptr L$84 
    add         sp,6 
    or          ax,ax 
    jne         L$51 
    jmp         L$41 
L$51:
    inc         si 
L$52:
    mov         ax,word ptr 8[bp] 
    add         ax,di 
    cmp         ax,si 
    jg          L$50 
    mov         ax,1 
    jmp         L$42 
L$53:
    pop         ds 
    pop         di 
    pop         si 
    pop         bp 
    retf        
_UnmapEMM:
    push        bp 
    mov         bp,sp 
    sub         sp,2 
    push        si 
    push        di 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    mov         di,word ptr 6[bp] 
    cmp         word ptr L$99,0 
    jne         L$54 
    jmp         L$63 
L$54:
    xor         si,si 
    jmp         L$56 
L$55:
    inc         si 
L$56:
    cmp         si,40H 
    jge         L$57 
    mov         bx,si 
    shl         bx,1 
    cmp         word ptr L$100[bx],di 
    jne         L$55 
L$57:
    cmp         si,40H 
    jne         L$58 
    jmp         L$63 
L$58:
    mov         ax,word ptr 8[bp] 
    add         ax,word ptr 0aH[bp] 
    mov         word ptr -2[bp],ax 
    cmp         word ptr 0aH[bp],1 
    jl          L$59 
    cmp         word ptr -2[bp],4 
    jle         L$60 
L$59:
    jmp         L$63 
L$60:
    mov         si,word ptr 8[bp] 
    jmp         L$62 
L$61:
    push        si 
    mov         ax,0ffffH 
    push        ax 
    push        di 
    call        far ptr L$84 
    add         sp,6 
    inc         si 
L$62:
    cmp         si,word ptr -2[bp] 
    jl          L$61 
L$63:
    pop         ds 
    pop         di 
    pop         si 
    mov         sp,bp 
    pop         bp 
    retf        
_UseEMM:
    push        bp 
    mov         bp,sp 
    push        si 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    mov         si,word ptr 6[bp] 
    push        si 
    call        far ptr L$94 
    pop         cx 
    push        word ptr 0aH[bp] 
    push        word ptr 8[bp] 
    push        si 
    push        cs 
    call        near ptr _MapEMM 
    add         sp,6 
    jmp         L$64 
L$64:
    pop         ds 
    pop         si 
    pop         bp 
    retf        
_SaveEMM:
    push        bp 
    mov         bp,sp 
    push        si 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    mov         si,word ptr 6[bp] 
    push        word ptr 0aH[bp] 
    push        word ptr 8[bp] 
    push        si 
    push        cs 
    call        near ptr _UnmapEMM 
    add         sp,6 
    push        si 
    call        far ptr L$97 
    pop         cx 
    pop         ds 
    pop         si 
    pop         bp 
    retf        
L$65:
    push        bp 
    mov         bp,sp 
    sub         sp,14H 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    lea         ax,-0cH[bp] 
    push        ss 
    push        ax 
    mov         ax,offset L$98 
    push        ds 
    push        ax 
    mov         cx,9 
    call        far ptr `F_SCOPY@` 
    push        es 
    mov         ax,3567H 
    int         21H 
    mov         ax,es 
    mov         word ptr -2[bp],ax 
    pop         es 
    mov         ax,word ptr -2[bp] 
    mov         word ptr -12H[bp],ax 
    mov         word ptr -14H[bp],0aH 
    lea         ax,-0cH[bp] 
    mov         word ptr -0eH[bp],ss 
    mov         word ptr -10H[bp],ax 
    xor         dx,dx 
    jmp         L$67 
L$66:
    inc         dx 
L$67:
    cmp         dx,8 
    jge         L$68 
    les         bx,dword ptr -10H[bp] 
    inc         word ptr -10H[bp] 
    mov         al,byte ptr es:[bx] 
    les         bx,dword ptr -14H[bp] 
    inc         word ptr -14H[bp] 
    cmp         al,byte ptr es:[bx] 
    je          L$66 
L$68:
    cmp         dx,8 
    jne         L$70 
    mov         ax,1 
L$69:
    jmp         L$71 
L$70:
    xor         ax,ax 
    jmp         L$69 
L$71:
    pop         ds 
    mov         sp,bp 
    pop         bp 
    retf        
L$72:
    push        bp 
    mov         bp,sp 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    mov         ah,40H 
    int         67H 
    or          ah,ah 
    jns         L$74 
    xor         ax,ax 
L$73:
    jmp         L$75 
L$74:
    mov         ax,1 
    jmp         L$73 
L$75:
    pop         ds 
    pop         bp 
    retf        
L$76:
    push        bp 
    mov         bp,sp 
    sub         sp,2 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    mov         ah,41H 
    int         67H 
    or          ah,ah 
    js          L$78 
    mov         word ptr -2[bp],bx 
    mov         ax,word ptr -2[bp] 
L$77:
    jmp         L$79 
L$78:
    xor         ax,ax 
    jmp         L$77 
L$79:
    pop         ds 
    mov         sp,bp 
    pop         bp 
    retf        
L$80:
    push        bp 
    mov         bp,sp 
    sub         sp,2 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    mov         ah,43H 
    mov         bx,word ptr 6[bp] 
    int         67H 
    or          ah,ah 
    js          L$82 
    mov         word ptr -2[bp],dx 
    mov         ax,word ptr -2[bp] 
L$81:
    jmp         L$83 
L$82:
    xor         ax,ax 
    jmp         L$81 
L$83:
    pop         ds 
    mov         sp,bp 
    pop         bp 
    retf        
L$84:
    push        bp 
    mov         bp,sp 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    xor         cx,cx 
    mov         ax,word ptr 0aH[bp] 
    mov         ah,44H 
    mov         bx,word ptr 8[bp] 
    mov         dx,word ptr 6[bp] 
    int         67H 
    or          ah,ah 
    js          L$85 
    mov         cx,1 
L$85:
    mov         ax,cx 
    jmp         L$86 
L$86:
    pop         ds 
    pop         bp 
    retf        
L$87:
    push        bp 
    mov         bp,sp 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    mov         ah,45H 
    mov         dx,word ptr 6[bp] 
    int         67H 
    or          ah,ah 
    js          L$89 
    mov         ax,1 
L$88:
    jmp         L$90 
L$89:
    xor         ax,ax 
    jmp         L$88 
L$90:
    pop         ds 
    pop         bp 
    retf        
L$91:
    push        bp 
    mov         bp,sp 
    sub         sp,2 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    mov         word ptr -2[bp],0 
    mov         ah,4cH 
    mov         dx,word ptr 6[bp] 
    int         67H 
    or          ah,ah 
    js          L$92 
    mov         word ptr -2[bp],bx 
L$92:
    mov         ax,word ptr -2[bp] 
    jmp         L$93 
L$93:
    pop         ds 
    mov         sp,bp 
    pop         bp 
    retf        
L$94:
    push        bp 
    mov         bp,sp 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    mov         bx,0f442H 
    mov         ah,47H 
    mov         dx,word ptr 6[bp] 
    int         67H 
    or          ah,ah 
    js          L$95 
    mov         bx,1 
L$95:
    mov         ax,bx 
    jmp         L$96 
L$96:
    pop         ds 
    pop         bp 
    retf        
L$97:
    push        bp 
    mov         bp,sp 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    mov         ah,48H 
    mov         dx,word ptr 6[bp] 
    int         67H 
    pop         ds 
    pop         bp 
    retf        
MEMORY_TEXT		ENDS
MEMORY_DATA		SEGMENT	WORD PUBLIC USE16 'FAR_DATA'
L$98:
    DB	45H, 4dH, 4dH, 58H, 58H, 58H, 58H, 30H
    DB	0
L$99:
    DB	0, 0
L$100:
    DB	0FH DUP(0,0,0,0,0,0,0,0)
    DB	0, 0, 0, 0, 0, 0, 0, 0

MEMORY_DATA		ENDS
		END
