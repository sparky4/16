;-----------------------------------------------------------------------
; MODULE XPAL
;
; Palette functions all MODE X 256 Color resolutions
;
; Compile with Tasm.
; C callable.
;
;
; ****** XLIB - Mode X graphics library                ****************
; ******                                               ****************
; ****** Written By Themie Gouthas                     ****************
;
; egg@dstos3.dsto.gov.au
; teg@bart.dsto.gov.au
;-----------------------------------------------------------------------


COMMENT	$



    All the functions in this module operate on two variations of the
    pallete buffer, the raw and annotated buffers.

    All those functions ending in buff operate on the following palette
    structure:

       BYTE:r0,g0,b0,r1,g1,b1,...rn,gn,bn

    No reference to the starting colour index or number of colours stored
    is contained in the structure.

    All those functions ending in struc operate on the following palette
    structure:

       BYTE:c,BYTE:n,BYTE:r0,g0,b0,r1,g1,b1,...rn,gn,bn

    where c is the starting colour and n is the number of colours stored


    NOTE: previously interrupts were disabled for DAC reads/writes but
	  they have been left enabled in this version to allow the mouse
	  interrupt to be invoked.

$




include xlib.inc
include xpal.inc

.code


;----------------------------------------------------------------------
; Read DAC palette into annotated type buffer with interrupts disabled
; ie BYTE colours to skip, BYTE colours to set, r1,g1,b1,r1,g2,b2...rn,gn,bn
;
; x_get_pal_struc(char far * pal, int num_colrs, int start_color)
;
; WARNING: memory for the palette buffers must all be pre-allocated
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
_x_get_pal_struc  proc
ARG  PalBuff:dword,NumColors:word,StartColor:word
     push  bp		; Set up stack frame
     mov   bp,sp
     push  di
     push  si
     cld

     les   di,dword ptr [PalBuff]  ; Point es:di to palette buffer
     mov   si,[StartColor]         ; Store the Start Colour
     mov   ax,si
     stosb
     mov   dx,[NumColors]          ; Store the Number of Colours
     mov   al,dl
     stosb

     mov   cx,dx                   ; setup regs and jump
     jmp   short ReadPalEntry

_x_get_pal_struc endp





;----------------------------------------------------------------------
; Read DAC palette into raw buffer with interrupts disabled
; ie BYTE r1,g1,b1,r1,g2,b2...rn,gn,bn
;
; x_get_pal_raw(char far * pal, int num_colrs, int start_index)
;
; WARNING: memory for the palette buffers must all be pre-allocated
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
_x_get_pal_raw	proc
ARG  PalBuff:dword,NumColors:word,StartColor:word
     push  bp		; Set up stack frame
     mov   bp,sp
     push  di
     push  si

     les   di,dword ptr [PalBuff]  ; Point es:di to palette buffer

     mov  si,[StartColor]
     mov  cx,[NumColors]

ReadPalEntry:
     cld
     WaitVsyncStart
     mov  ax,si
     mov  dx,DAC_READ_INDEX
     ;cli
     out  dx,al                    ; Tell DAC what colour to start reading
     mov  dx,DAC_DATA

     mov  bx,cx                    ; set cx to Num Colors * 3 ( size of
     shl  bx,1                     ; palette buffer)
     add  cx,bx

     rep  insb                     ; read the palette enntries

     ;sti
     pop  si
     pop  di
     pop  bp
     ret
_x_get_pal_raw endp



;----------------------------------------------------------------------
; Write DAC palette from annotated type buffer with interrupts disabled
; ie BYTE colours to skip, BYTE colours to set, r1,g1,b1,r1,g2,b2...rn,gn,bn
;
; x_put_pal_struc(char far * pal)
;
; Written by Themie Gouthas
;----------------------------------------------------------------------

_x_put_pal_struc proc
ARG     CompPalBuff:dword
        push    bp      ;preserve caller's stack frame
	mov     bp,sp   ;point to local stack frame
	push    ds
	push    si
	cld
	lds     si,[CompPalBuff]   ; load the source compressed colour data
	lodsb			   ; get the colours to skip
	mov     ah,0
	mov     bx,ax              ; skip colours

	lodsb			   ; get the count of colours to set
	mov     ah,0
	mov	cx,ax              ; use it as a loop counter
	jmp     short WritePalEntry

_x_put_pal_struc  endp


;----------------------------------------------------------------------
; Write DAC palette from annotated type buffer with interrupts disabled
; starting at a new palette index
;
; ie BYTE colours to skip, BYTE colours to set, r1,g1,b1,r1,g2,b2...rn,gn,bn
;
; x_transpose_pal_struc(char far * pal, int StartColor)
;
; WARNING: memory for the palette buffers must all be pre-allocated
;
; Written by Themie Gouthas
;----------------------------------------------------------------------

_x_transpose_pal_struc proc
ARG     CompPalBuff:dword,StartColor:word
        push    bp      ;preserve caller's stack frame
	mov     bp,sp   ;point to local stack frame
	push    ds
	push    si
	cld
	lds     si,[CompPalBuff]   ; load the source compressed colour data
	mov     bx,[StartColor]
	mov     [si],bl
	inc     si
	lodsb			   ; get the count of colours to set
	mov     ah,0
	mov	cx,ax              ; use it as a loop counter
	jmp     short WritePalEntry
_x_transpose_pal_struc endp


;----------------------------------------------------------------------
; Write DAC palette from raw buffer with interrupts disabled
; ie BYTE r1,g1,b1,r1,g2,b2...rn,gn,bn
;
; _x_put_pal_raw(char far * pal, int num_colrs, int start_index)
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
_x_put_pal_raw  proc
ARG  PalBuff:dword,NumColors:word,StartColor:word
		push bp		; Set up stack frame
		mov  bp,sp
		push ds
		push si

		mov  cx,[NumColors]      ; Number of colours to set
		mov  bx,[StartColor]
		lds  si,[PalBuff]        ; ds:si -> palette buffer


WritePalEntry:
                mov  ax,@data
		mov  es,ax
		cmp  es:[_VsyncHandlerActive],TRUE
		jne  @@NoVsyncHandler
@@WaitForLast:
		cmp  es:[_VsyncPaletteCount],0
		jne  @@WaitForLast
		push cx
		push es
		mov  di, offset _VsyncPaletteBuffer
		mov  ax,3
		mul  cx
		mov  cx,ax
		rep  movsb
		pop  ds
		pop  cx
		mov  [_VsyncPaletteStart],bx
		mov  [_VsyncPaletteCount],cx
		jmp  short @@Done
@@NoVsyncHandler:


		or   cx,cx
		jz   @@Done
		;cli
		cld                      ; Make sure we're going the right way
		WaitVsyncStart           ; Wait for vert sync to start
                mov  ax,bx
		mov  bx,60               ; set the vsync check timer (Vsync
                			 ; is tested for at each bx'th entry to
					 ; prevent snow 60 is otimum for 10
					 ; MHz 286 or greater

@@SetLoop:
		mov  dx,DAC_WRITE_INDEX  ; Tell DAC what colour index to start
		out  dx,al               ; writing from
		mov  dx,DAC_DATA

		outsb                    ; Set the red component
		outsb                    ; Set the green component
		outsb                    ; Set the blue component
		inc  al                  ; increment the colour index
		dec  bx                  ; decrement vsync test counter
		js   @@test_vsync        ; ready to test for vsync again ?
		loop @@SetLoop           ; No! - continue loop
		jmp  short @@Done        ; All colours done

@@test_vsync:
		mov     dx,INPUT_STATUS_0
		push    ax               ; save current colour index
@@Wait:
		in      al,dx            ; wait for vsync leading edge pulse
		test    al,08h
		jz      @@Wait

		pop     ax               ; restore current colour index
		mov     bx,60            ; reset vsync test counter
		loop @@SetLoop           ; loop for next colour index

@@Done:
		;sti
		pop  si
		pop  ds
		pop  bp
		ret
_x_put_pal_raw endp



;----------------------------------------------------------------------
; Set the RGB setting of a vga color
;
; _x_set_rgb(unsigned char color, unsigned char R,unsigned char G,
;            unsigned char B)
;
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
_x_set_rgb  proc
ARG  ColorIndex:byte,R:byte,G:byte,B:byte
		push bp		; Set up stack frame
		mov  bp,sp

		mov  al,[ColorIndex]
		mov  dx,DAC_WRITE_INDEX  ; Tell DAC what colour index to
		out  dx,al               ; write to
		mov  dx,DAC_DATA

		mov  al,[R]              ; Set the red component
		out  dx,al
		mov  al,[G]              ; Set the green component
		out  dx,al
		mov  al,[B]              ; Set the blue component
		out  dx,al
		pop  bp
		ret
_x_set_rgb endp


;----------------------------------------------------------------------
; Rotate annotated palette buffer entries
;
; x_rot_pal_struc(char far * pal, int direction)
;
; Direction : 0 = backward 1 = forward
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
_x_rot_pal_struc  proc
ARG  PalBuff:dword,Direction:word
    push bp		; Set up stack frame
    mov	 bp,sp
    push ds
    push si
    push di

    cld
    lds	 si,dword ptr [PalBuff]  ; point ds:si to Palette buffer
    lodsw         ; al = colorst ot skip, ah = num colors

    xor  ch,ch    ; Set the number of palette entries to cycle in cx
    mov  cl,ah    ;

    jmp  short RotatePalEntry

_x_rot_pal_struc  endp



;----------------------------------------------------------------------
; Rotate raw palette buffer
;
; x_rot_pal_raw(char far * pal, int direction, int num_colrs)
;
; Direcction : 0 = backward 1 = forward
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
_x_rot_pal_raw	proc
ARG  PalBuff:dword,Direction:word,NumColors:word
    push bp		; Set up stack frame
    mov	 bp,sp
    push ds
    push si
    push di

    cld
    mov	 cx,[NumColors]          ; Set the number of palette entries to cycle
    lds	 si,dword ptr [PalBuff]  ; point ds:si to Palette buffer

RotatePalEntry:


    mov	 ax,ds                ; copy ds to es
    mov	 es,ax

    dec  cx
    mov	 bx,cx                ; Multiply cx by 3
    shl	 bx,1
    add  cx,bx

    cmp  [Direction],0        ; are we going forward ?
    jne  @@forward            ; yes - jump (colors move one position back)

    std                       ; no - set reverse direction
    add  si,cx                ; set si to last byte in palette
    add  si,2

@@forward:
    mov	 ax,si                ; copy si to di
    mov	 di,ax

    lodsb                     ; load first color triplet into regs
    mov  dl,al
    lodsb
    mov  dh,al
    lodsb
    mov  bl,al

    rep	 movsb                ; move remaining triplets direction indicated
			      ; by direction flag

    mov  al,dl                ; write color triplet from regs to last position
    stosb
    mov  al,dh
    stosb
    mov  al,bl
    stosb

    pop	 di
    pop	 si
    pop	 ds
    pop	 bp
    ret
_x_rot_pal_raw  endp

;----------------------------------------------------------------------
; Copy palette making intensity adjustment
; x_cpcontrast_pal_struc(char far *src_pal, char far *dest_pal, unsigned char Intensity)
;
; WARNING: memory for the palette buffers must all be pre-allocated
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
_x_cpcontrast_pal_struc	proc
ARG  PalSrcBuff:dword,PalDestBuff:dword,Intensity:byte
    push bp		; Set up stack frame
    mov	 bp,sp
    push ds
    push si
    push di

    cld
    mov  bh,0ffh
    sub  bh,[Intensity]
    and  bh,07fh            ; Palettes are 7 bit
    lds	 si,dword ptr [PalSrcBuff]  ; point ds:si to Source Palette buffer
    les	 di,dword ptr [PalDestBuff] ; point ds:si to Source Palette buffer
    lodsw			    ; al = colorst ot skip, ah = num color
    stosw

    xor  ch,ch    ; Set the number of palette entries to adjust
    mov  cl,ah    ;

    mov  dx,0     ; flag set to 0 if all output palette entries zero
@@MainLoop:
    lodsw
    sub  al,bh               ; adjust intensity and copy RED
    jns  @@DecrementOK_R
    xor  al,al
@@DecrementOK_R:
    sub  ah,bh               ; adjust intensity and copy GREEN
    jns  @@DecrementOK_G
    xor  ah,ah
@@DecrementOK_G:
    or   dx,ax
    or   dl,ah
    stosw
    lodsb
    sub  al,bh               ; adjust intensity and copy BLUE
    jns  @@DecrementOK_B
    xor  al,al
@@DecrementOK_B:
    or   dl,al
    stosb
    loop @@MainLoop

    mov  ax,dx
    pop  di
    pop	 si
    pop	 ds
    pop	 bp
    ret
_x_cpcontrast_pal_struc  endp



;----------------------------------------------------------------------
; Write DAC palette from annotated type buffer with specified intensity
; ie BYTE colours to skip, BYTE colours to set, r1,g1,b1,r1,g2,b2...rn,gn,bn
;
; x_put_contrast_pal_struc(char far * pal, unsigned char  intensity)
;
; Designed for fading in or out a palette without using an intermediate
; working palette buffer ! (Slow but memory efficient ... OK for small
; pal strucs}
;
; Written by Themie Gouthas
;----------------------------------------------------------------------

_x_put_contrast_pal_struc proc
ARG     CompPalBuff:dword,Intensity:byte
        push    bp      ;preserve caller's stack frame
	mov     bp,sp   ;point to local stack frame
	push    ds
	push    si
	push    di
	cld

	mov     bh,0ffh
	sub     bh,[Intensity]
	and     bh,07fh            ; Palettes are 7 bit
	mov     di,40              ; set the vsync check timer (Vsync
				   ; is tested for at each di'th entry to
				   ; prevent snow 40 is otimum for 10
				   ; MHz 286 or greater)
	lds     si,[CompPalBuff]   ; load the source compressed colour data
	lodsb			   ; get the colours to skip
	mov     bl,al

	lodsb			   ; get the count of colours to set
	mov     ah,0
	mov	cx,ax              ; use it as a loop counter
	or      cx,cx
	jz      @@Done

	WaitVsyncStart           ; Wait for vert sync to start

@@MainLoop:
        mov  al,bl
	mov  dx,DAC_WRITE_INDEX  ; Tell DAC what colour index to start
	out  dx,al               ; writing from
	inc  dx                  ; == mov  dx,DAC_DATA

	lodsb                    ; Load each colour component, modify for
	sub  al,bh               ; intensity and write to DAC H/Ware
	jns  @@DecrementOK_R
	xor  al,al
@@DecrementOK_R:
	out  dx,al

	lodsb
	sub  al,bh
	jns  @@DecrementOK_G
	xor  al,al
@@DecrementOK_G:
	out  dx,al

	lodsb
	sub  al,bh
	jns  @@DecrementOK_B
	xor  al,al
@@DecrementOK_B:
	out  dx,al

	inc  bl                  ; increment color index
	dec  di                  ; decrement vsync test flag
	js   @@test_vsync
	loop @@MainLoop
	jmp  short @@Done


@@test_vsync:
	mov     dx,INPUT_STATUS_0
	push    ax               ; save current colour index
@@Wait:
	in      al,dx            ; wait for vsync leading edge pulse
	test    al,08h
	jz      @@Wait

	pop     ax               ; restore current colour index
	mov     di,40            ; reset vsync test counter
	loop    @@MainLoop       ; loop for next colour index

@@Done:
	;sti
	pop  di
	pop  si
	pop  ds
	pop  bp
	ret

_x_put_contrast_pal_struc   endp


    end


