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
    call        far ptr L$59 
    or          ax,ax 
    je          L$1 
    call        far ptr L$65 
    or          ax,ax 
    jne         L$2 
L$1:
    xor         ax,ax 
    jmp         L$4 
L$2:
    call        far ptr L$68 
    mov         word ptr L$89,ax 
    or          ax,ax 
    jne         L$3 
    xor         ax,ax 
    jmp         L$4 
L$3:
    mov         ax,1 
    jmp         L$4 
L$4:
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
    cmp         word ptr L$89,0 
    jne         L$5 
    jmp         L$9 
L$5:
    xor         si,si 
    jmp         L$8 
L$6:
    mov         bx,si 
    shl         bx,1 
    cmp         word ptr L$88[bx],0 
    je          L$7 
    mov         bx,si 
    shl         bx,1 
    push        word ptr L$88[bx] 
    call        far ptr L$77 
    inc         sp 
    inc         sp 
    mov         bx,si 
    shl         bx,1 
    mov         word ptr L$88[bx],0 
L$7:
    inc         si 
L$8:
    cmp         si,40H 
    jl          L$6 
    mov         word ptr L$89,0 
L$9:
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
    cmp         word ptr L$89,0 
    je          L$10 
    mov         ax,1 
    jmp         L$11 
L$10:
    xor         ax,ax 
L$11:
    jmp         L$12 
L$12:
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
    js          L$13 
    mov         word ptr -2[bp],bx 
    mov         ax,word ptr -2[bp] 
    xor         dx,dx 
    mov         cl,0eH 
    call        far ptr `F_LXLSH@` 
    mov         word ptr -4[bp],dx 
    mov         word ptr -6[bp],ax 
L$13:
    mov         dx,word ptr -4[bp] 
    mov         ax,word ptr -6[bp] 
    jmp         L$14 
L$14:
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
    cmp         word ptr L$89,0 
    jne         L$15 
    les         bx,dword ptr 6[bp] 
    mov         word ptr es:[bx],0 
    xor         dx,dx 
    xor         ax,ax 
    jmp         L$23 
L$15:
    cmp         di,1 
    jl          L$16 
    cmp         di,3fcH 
    jle         L$17 
L$16:
    les         bx,dword ptr 6[bp] 
    mov         word ptr es:[bx],0f440H 
    xor         dx,dx 
    xor         ax,ax 
    jmp         L$23 
L$17:
    xor         si,si 
    jmp         L$19 
L$18:
    inc         si 
L$19:
    cmp         si,40H 
    jge         L$20 
    mov         bx,si 
    shl         bx,1 
    cmp         word ptr L$88[bx],0 
    jne         L$18 
L$20:
    cmp         si,40H 
    jne         L$21 
    les         bx,dword ptr 6[bp] 
    mov         word ptr es:[bx],0f443H 
    xor         dx,dx 
    xor         ax,ax 
    jmp         L$23 
L$21:
    push        di 
    call        far ptr L$71 
    inc         sp 
    inc         sp 
    mov         bx,si 
    shl         bx,1 
    mov         word ptr L$88[bx],ax 
    or          ax,ax 
    jle         L$22 
    mov         ax,word ptr L$89 
    mov         word ptr -2[bp],ax 
    mov         word ptr -4[bp],0 
L$22:
    mov         bx,si 
    shl         bx,1 
    mov         ax,word ptr L$88[bx] 
    les         bx,dword ptr 6[bp] 
    mov         word ptr es:[bx],ax 
    mov         dx,word ptr -2[bp] 
    mov         ax,word ptr -4[bp] 
    jmp         L$23 
L$23:
    pop         ds 
    pop         di 
    pop         si 
    mov         sp,bp 
    pop         bp 
    retf        
_EMMRealloc:
    push        bp 
    mov         bp,sp 
    push        si 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    xor         si,si 
    cmp         word ptr L$89,0 
    je          L$24 
    cmp         word ptr 8[bp],0 
    jl          L$24 
    cmp         word ptr 8[bp],3fcH 
    jle         L$25 
L$24:
    xor         ax,ax 
    jmp         L$27 
L$25:
    mov         ah,51H 
    mov         bx,word ptr 8[bp] 
    mov         dx,word ptr 6[bp] 
    int         67H 
    or          ah,ah 
    js          L$26 
    mov         si,1 
L$26:
    mov         ax,si 
    jmp         L$27 
L$27:
    pop         ds 
    pop         si 
    pop         bp 
    retf        
_EMMFree:
    push        bp 
    mov         bp,sp 
    dec         sp 
    dec         sp 
    push        si 
    push        di 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    mov         di,word ptr 6[bp] 
    cmp         word ptr L$89,0 
    jne         L$28 
    jmp         L$36 
L$28:
    xor         si,si 
    jmp         L$30 
L$29:
    inc         si 
L$30:
    cmp         si,40H 
    jge         L$31 
    mov         bx,si 
    shl         bx,1 
    cmp         word ptr L$88[bx],di 
    jne         L$29 
L$31:
    cmp         si,40H 
    jl          L$32 
    jmp         L$36 
L$32:
    mov         word ptr -2[bp],10H 
    jmp         L$34 
L$33:
    mov         bx,si 
    shl         bx,1 
    push        word ptr L$88[bx] 
    call        far ptr L$77 
    inc         sp 
    inc         sp 
    or          ax,ax 
    je          L$34 
    jmp         L$35 
L$34:
    mov         ax,word ptr -2[bp] 
    dec         word ptr -2[bp] 
    or          ax,ax 
    jne         L$33 
L$35:
    mov         bx,si 
    shl         bx,1 
    mov         word ptr L$88[bx],0 
L$36:
    pop         ds 
    pop         di 
    pop         si 
    mov         sp,bp 
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
    mov         di,word ptr 6[bp] 
    cmp         word ptr L$89,0 
    jne         L$37 
    xor         ax,ax 
    jmp         L$47 
L$37:
    xor         si,si 
    jmp         L$39 
L$38:
    inc         si 
L$39:
    cmp         si,40H 
    jge         L$40 
    mov         bx,si 
    shl         bx,1 
    cmp         word ptr L$88[bx],di 
    jne         L$38 
L$40:
    cmp         si,40H 
    jne         L$41 
    xor         ax,ax 
    jmp         L$47 
L$41:
    push        di 
    call        far ptr L$80 
    inc         sp 
    inc         sp 
    cmp         ax,word ptr 0aH[bp] 
    jl          L$42 
    cmp         word ptr 0aH[bp],1 
    jl          L$42 
    cmp         word ptr 0aH[bp],4 
    jle         L$43 
L$42:
    mov         ax,0f440H 
    jmp         L$47 
L$43:
    mov         si,word ptr 8[bp] 
    jmp         L$46 
L$44:
    mov         ax,si 
    sub         ax,word ptr 8[bp] 
    push        ax 
    push        si 
    push        di 
    call        far ptr L$74 
    add         sp,6 
    or          ax,ax 
    jne         L$45 
    xor         ax,ax 
    jmp         L$47 
L$45:
    inc         si 
L$46:
    mov         ax,word ptr 8[bp] 
    add         ax,word ptr 0aH[bp] 
    cmp         ax,si 
    jg          L$44 
    mov         ax,1 
    jmp         L$47 
L$47:
    pop         ds 
    pop         di 
    pop         si 
    pop         bp 
    retf        
_UnmapEMM:
    push        bp 
    mov         bp,sp 
    dec         sp 
    dec         sp 
    push        si 
    push        di 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    mov         di,word ptr 6[bp] 
    cmp         word ptr L$89,0 
    jne         L$48 
    jmp         L$57 
L$48:
    xor         si,si 
    jmp         L$50 
L$49:
    inc         si 
L$50:
    cmp         si,40H 
    jge         L$51 
    mov         bx,si 
    shl         bx,1 
    cmp         word ptr L$88[bx],di 
    jne         L$49 
L$51:
    cmp         si,40H 
    jne         L$52 
    jmp         L$57 
L$52:
    mov         ax,word ptr 8[bp] 
    add         ax,word ptr 0aH[bp] 
    mov         word ptr -2[bp],ax 
    cmp         word ptr 0aH[bp],1 
    jl          L$53 
    cmp         word ptr -2[bp],4 
    jle         L$54 
L$53:
    jmp         L$57 
L$54:
    mov         si,word ptr 8[bp] 
    jmp         L$56 
L$55:
    push        si 
    mov         ax,0ffffH 
    push        ax 
    push        di 
    call        far ptr L$74 
    add         sp,6 
    inc         si 
L$56:
    cmp         si,word ptr -2[bp] 
    jl          L$55 
L$57:
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
    call        far ptr L$83 
    inc         sp 
    inc         sp 
    push        word ptr 0aH[bp] 
    push        word ptr 8[bp] 
    push        si 
    push        cs 
    call        near ptr _MapEMM 
    add         sp,6 
    jmp         L$58 
L$58:
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
    call        far ptr L$86 
    inc         sp 
    inc         sp 
    pop         ds 
    pop         si 
    pop         bp 
    retf        
L$59:
    push        bp 
    mov         bp,sp 
    sub         sp,14H 
    push        si 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    push        ss 
    lea         ax,-0cH[bp] 
    push        ax 
    push        ds 
    mov         ax,offset L$87 
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
    xor         si,si 
    jmp         L$61 
L$60:
    inc         si 
L$61:
    cmp         si,8 
    jge         L$62 
    les         bx,dword ptr -10H[bp] 
    inc         word ptr -10H[bp] 
    mov         al,byte ptr es:[bx] 
    les         bx,dword ptr -14H[bp] 
    inc         word ptr -14H[bp] 
    cmp         al,byte ptr es:[bx] 
    je          L$60 
L$62:
    cmp         si,8 
    jne         L$63 
    mov         ax,1 
    jmp         L$64 
L$63:
    xor         ax,ax 
    jmp         L$64 
L$64:
    pop         ds 
    pop         si 
    mov         sp,bp 
    pop         bp 
    retf        
L$65:
    push        bp 
    mov         bp,sp 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    mov         ah,40H 
    int         67H 
    or          ah,ah 
    jns         L$66 
    xor         ax,ax 
    jmp         L$67 
L$66:
    mov         ax,1 
    jmp         L$67 
L$67:
    pop         ds 
    pop         bp 
    retf        
L$68:
    push        bp 
    mov         bp,sp 
    dec         sp 
    dec         sp 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    mov         ah,41H 
    int         67H 
    or          ah,ah 
    js          L$69 
    mov         word ptr -2[bp],bx 
    mov         ax,word ptr -2[bp] 
    jmp         L$70 
L$69:
    xor         ax,ax 
    jmp         L$70 
L$70:
    pop         ds 
    mov         sp,bp 
    pop         bp 
    retf        
L$71:
    push        bp 
    mov         bp,sp 
    dec         sp 
    dec         sp 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    mov         ah,43H 
    mov         bx,word ptr 6[bp] 
    int         67H 
    or          ah,ah 
    js          L$72 
    mov         word ptr -2[bp],dx 
    mov         ax,word ptr -2[bp] 
    jmp         L$73 
L$72:
    xor         ax,ax 
    jmp         L$73 
L$73:
    pop         ds 
    mov         sp,bp 
    pop         bp 
    retf        
L$74:
    push        bp 
    mov         bp,sp 
    push        si 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    xor         si,si 
    mov         ax,word ptr 0aH[bp] 
    mov         ah,44H 
    mov         bx,word ptr 8[bp] 
    mov         dx,word ptr 6[bp] 
    int         67H 
    or          ah,ah 
    js          L$75 
    mov         si,1 
L$75:
    mov         ax,si 
    jmp         L$76 
L$76:
    pop         ds 
    pop         si 
    pop         bp 
    retf        
L$77:
    push        bp 
    mov         bp,sp 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    mov         ah,45H 
    mov         dx,word ptr 6[bp] 
    int         67H 
    or          ah,ah 
    js          L$78 
    mov         ax,1 
    jmp         L$79 
L$78:
    xor         ax,ax 
    jmp         L$79 
L$79:
    pop         ds 
    pop         bp 
    retf        
L$80:
    push        bp 
    mov         bp,sp 
    dec         sp 
    dec         sp 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    mov         word ptr -2[bp],0 
    mov         ah,4cH 
    mov         dx,word ptr 6[bp] 
    int         67H 
    or          ah,ah 
    js          L$81 
    mov         word ptr -2[bp],bx 
L$81:
    mov         ax,word ptr -2[bp] 
    jmp         L$82 
L$82:
    pop         ds 
    mov         sp,bp 
    pop         bp 
    retf        
L$83:
    push        bp 
    mov         bp,sp 
    push        si 
    push        ds 
    mov         ax,MEMORY_DATA 
    mov         ds,ax 
    mov         si,0f442H 
    mov         ah,47H 
    mov         dx,word ptr 6[bp] 
    int         67H 
    or          ah,ah 
    js          L$84 
    mov         si,1 
L$84:
    mov         ax,si 
    jmp         L$85 
L$85:
    pop         ds 
    pop         si 
    pop         bp 
    retf        
L$86:
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
L$87:
    DB	45H, 4dH, 4dH, 58H, 58H, 58H, 58H, 30H
    DB	0, 0
L$88:
    DB	0FH DUP(0,0,0,0,0,0,0,0)
    DB	0, 0, 0, 0, 0, 0, 0, 0
L$89:
    DB	0, 0

MEMORY_DATA		ENDS
		END
