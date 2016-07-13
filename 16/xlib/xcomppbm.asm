;-----------------------------------------------------------------------
; module XCOMPPBM
;
; This module contains only the compiler and sizeof routines --
; use the plotter from XCBITMAP.
;
;-----------------------------------------------------------------------

include xlib.inc
include xcomppbm.inc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; _x_compile_pbm
;
; I only changed five instructions, instead of rewriting this
; for PBMs.  So it is amazingly inefficient.  But, what the hell,
; It's only a game :).
;

; accessory macros to save typing (what else?)
Emitb macro arg
	mov byte ptr es:[di],&arg&
	inc di
	endm

Emitw macro arg
	mov word ptr es:[di],&arg&
	add di,2
	endm

; opcodes emitted by _x_compile_pbm
ROL_AL          equ 0c0d0h              ; rol al
SHORT_STORE_8   equ 044c6h              ; mov [si]+disp8,  imm8
STORE_8         equ 084c6h              ; mov [si]+disp16, imm8
SHORT_STORE_16  equ 044c7h              ; mov [si]+disp8,  imm16
STORE_16        equ 084c7h              ; mov [si]+disp16, imm16
ADC_SI_IMMED    equ 0d683h              ; adc si,imm8
OUT_AL          equ 0eeh                ; out dx,al
RETURN          equ 0cbh                ; ret


.data

align 2
ColumnMask      db      011h,022h,044h,088h


.code

	align   2
_x_compile_pbm proc
ARG   logical_width:word,bitmap:dword,output:dword
LOCAL bwidth,scanx,scany,outputx,outputy,column,set_column,input_size:word=LocalStk
	push bp
	mov  bp, sp         ; caller's stack frame
	sub  sp,LocalStk    ; local space
	push si
	push di
	push ds

	mov word ptr [scanx],0
	mov word ptr [scany],0
	mov word ptr [outputx],0
	mov word ptr [outputy],0
	mov word ptr [column],0
	mov word ptr [set_column],0

	lds si,[bitmap]     ; 32-bit pointer to source bitmap

	les di,[output]     ; 32-bit pointer to destination stream

	lodsb               ; load width byte
	xor ah, ah          ; convert to word
	mov [bwidth], ax    ; save for future reference
	mov bl, al          ; copy width byte to bl
	lodsb               ; load height byte -- already a word since ah=0
	mul bl              ; mult height word by width byte
	mov [input_size], ax;  to get pixel total

@@MainLoop:
	mov bx, [scanx]     ; position in original bitmap
	add bx, [scany]

	mov al, [si+bx]     ; get pixel
	or  al, al          ; skip empty pixels
	jnz @@NoAdvance
	jmp @@Advance
@@NoAdvance:

	mov dx, [set_column]
	cmp dx, [column]
	je @@SameColumn
@@ColumnLoop:
	Emitw ROL_AL        ; emit code to move to new column
	Emitw ADC_SI_IMMED
	Emitb 0

	inc dx
	cmp dx, [column]
	jl @@ColumnLoop

	Emitb OUT_AL        ; emit code to set VGA mask for new column
	mov [set_column], dx
@@SameColumn:
	mov dx, [outputy]   ; calculate output position
	add dx, [outputx]
	sub dx, 128

	inc word ptr [scanx]
	mov cx, [scanx]     ; within four pixels of right edge?
	cmp cx, [bwidth]
	jge @@OnePixel

	inc word ptr [outputx]
	mov ah, [si+bx+1]   ; get second pixel
	or ah, ah
	jnz @@TwoPixels
@@OnePixel:
	cmp dx, 127         ; can we use shorter form?
	jg @@OnePixLarge
	cmp dx, -128
	jl @@OnePixLarge
	Emitw SHORT_STORE_8
	Emitb dl            ; 8-bit position in output
	jmp @@EmitOnePixel
@@OnePixLarge:
	Emitw STORE_8
	Emitw dx            ; position in output
@@EmitOnePixel:
	Emitb al
	jmp short @@Advance
@@TwoPixels:
	cmp dx, 127
	jg @@TwoPixLarge
	cmp dx, -128
	jl @@TwoPixLarge
	Emitw SHORT_STORE_16
	Emitb dl            ; 8-bit position in output
	jmp @@EmitTwoPixels
@@TwoPixLarge:
	Emitw STORE_16
	Emitw dx            ; position in output
@@EmitTwoPixels:
	Emitw ax

@@Advance:
	inc word ptr [outputx]
	mov ax, [scanx]
	inc ax
	cmp ax, [bwidth]
	jl @@AdvanceDone
	mov dx, [outputy]
	add dx, [logical_width]
	mov cx, [scany]
	add cx, [bwidth]
	cmp cx, [input_size]
	jl @@NoNewColumn
	inc word ptr [column]
	mov cx, [column]
	cmp cx, 4
	je @@Exit           ; Column 4: there is no column 4.
	xor cx, cx          ; scany and outputy are 0 again for
	mov dx, cx          ; the new column
	add si, [input_size]
@@NoNewColumn:
	mov [outputy], dx
	mov [scany], cx
	xor ax, ax
	mov word ptr [outputx], 0
@@AdvanceDone:
	mov [scanx], ax
	jmp @@MainLoop

@@Exit:
	Emitb RETURN
	mov ax,di
	sub ax,word ptr [output] ; size of generated code

	pop ds
	pop di
	pop si
	mov sp, bp
	pop bp

	ret
_x_compile_pbm endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; _x_sizeof_cpbm
;


	align   2
_x_sizeof_cpbm proc
ARG   logical_width:word,bitmap:dword
LOCAL bwidth,scanx,scany,outputx,outputy,column,set_column,input_size:word=LocalStk
	push bp
	mov  bp, sp         ; caller's stack frame
	sub  sp,LocalStk    ; local space
	push si
	push di
	push ds

	mov word ptr [scanx], 0
	mov word ptr [scany], 0
	mov word ptr [outputx], 0
	mov word ptr [outputy], 0
	mov word ptr [column], 0
	mov word ptr [set_column], 0

	lds si,[bitmap]     ; 32-bit pointer to source bitmap

	mov di, 1           ; initial size is just the size of the far RET

	lodsb               ; load width byte
	xor ah, ah          ; convert to word
	mov [bwidth], ax    ; save for future reference
	mov bl, al          ; copy width byte to bl
	lodsb               ; load height byte -- already a word since ah=0
	mul bl              ; mult height word by width byte
	mov [input_size], ax;  to get pixel total

@@MainLoop:
	mov bx, [scanx]     ; position in original bitmap
	add bx, [scany]

	mov al, [si+bx]     ; get pixel
	or  al, al          ; skip empty pixels
	jnz @@NoAdvance
	jmp @@Advance
@@NoAdvance:

	mov dx, [set_column]
	cmp dx, [column]
	je @@SameColumn
@@ColumnLoop:
	add di, 5           ; size of code to move to new column
	inc dx
	cmp dx,[column]
	jl @@ColumnLoop

	inc di              ; size of code to set VGA mask
	mov [set_column], dx
@@SameColumn:
	mov dx, [outputy]   ; calculate output position
	add dx, [outputx]
	sub dx, 128

	inc word ptr [scanx]
	mov cx, [scanx]     ; within four pixels of right edge?
	cmp cx, [bwidth]
	jge @@OnePixel

	inc word ptr [outputx]
	mov ah,[si+bx+1]    ; get second pixel
	or ah, ah
	jnz @@TwoPixels
@@OnePixel:
	cmp dx, 127         ; can we use shorter form?
	jg @@OnePixLarge
	cmp dx, -128
	jl @@OnePixLarge
	add di, 4           ; size of 8-bit position in output plus one pixel
	jmp @@EmitOnePixel
@@OnePixLarge:
	add di, 5           ; size of position in output plus one pixels
@@EmitOnePixel:
	jmp short @@Advance
@@TwoPixels:
	cmp dx, 127
	jg @@TwoPixLarge
	cmp dx, -128
	jl @@TwoPixLarge
	add di, 5           ; size of 8-bit position in output plus two pixels
	jmp @@EmitTwoPixels
@@TwoPixLarge:
	add di, 6           ; size of 16-bit position in output plus two pixels
@@EmitTwoPixels:

@@Advance:
	inc word ptr [outputx]
	mov ax, [scanx]
	inc ax
	cmp ax, [bwidth]
	jl @@AdvanceDone
	mov dx, [outputy]
	add dx, [logical_width]
	mov cx, [scany]
	add cx, [bwidth]
	cmp cx, [input_size]
	jl @@NoNewColumn
	inc word ptr [column]
	mov cx, [column]
	cmp cx, 4
	je @@Exit           ; Column 4: there is no column 4.
	xor cx,cx           ; scany and outputy are 0 again for
	mov dx,cx           ; the new column
	add si, [input_size]
@@NoNewColumn:
	mov [outputy], dx
	mov [scany], cx
	xor ax, ax
	mov word ptr [outputx], ax
@@AdvanceDone:
	mov [scanx], ax
	jmp @@MainLoop

@@Exit:
	mov ax, di          ; size of generated code

	pop ds
	pop di
	pop si
	mov sp,bp
	pop bp

	ret
_x_sizeof_cpbm endp

end

