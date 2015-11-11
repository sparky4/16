;-----------------------------------------------------------------------
; MODULE XPBITMAP
;
; Planar Bitmap functions - System Ram <-> Video Ram
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


COMMENT $

  This module implements a set of functions to operate on planar bitmaps.
  Planar bitmaps as used by these functions have the following structure:

  BYTE 0                 The bitmap width in bytes (4 pixel groups) range 1..255
  BYTE 1                 The bitmap height in rows range 1..255
  BYTE 2..n1             The plane 0 pixels width*height bytes
  BYTE n1..n2            The plane 1 pixels width*height bytes
  BYTE n2..n3            The plane 2 pixels width*height bytes
  BYTE n3..n4            The plane 3 pixels width*height bytes

  These functions provide the fastest possible bitmap blts from system ram to
  to video and further, the single bitmap is applicable to all pixel
  allignments. The masked functions do not need separate masks since all non
  zero pixels are considered to be masking pixels, hence if a pixel is 0 the
  corresponding screen destination pixel is left unchanged.


$

include xlib.inc
include xpbitmap.inc
LOCALS
	.code

;----------------------------------------------------------------------
; x_flip_masked_pbm - mask write a planar bitmap from system ram to video ram
; all zero source bitmap bytes indicate destination byte to be left unchanged
; If "Orientation" is set then the bitmap is flipped from left to right as
; it is drawn
;
; Source Bitmap structure:
;
;  Width:byte, Height:byte, Bitmap data (plane 0)...Bitmap data (plane 1)..,
;  Bitmap data (plane 2)..,Bitmap data (plane 3)..
;
;  note width is in bytes ie lots of 4 pixels
;
;  x_flip_masked_pbm(X,Y,ScrnOffs,char far * Bitmap, Orientation)
;
;
; LIMITATIONS: No clipping is supported
;              Only supports bitmaps with widths which are a multiple of
;              4 pixels
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
_x_flip_masked_pbm  proc
        ARG X:word,Y:word,ScrnOffs:word,Bitmap:dword,Orientation:word
	LOCAL Plane:byte,BMHeight:byte,LineInc:word=LocalStk
	push  bp
        mov   bp,sp
	sub   sp,LocalStk                 ; Create space for local variables
        push  si
        push  di
	push  ds
	cld
        mov   ax,SCREEN_SEG
        mov   es,ax
        mov   ax,[Y]                      ; Calculate dest screen row
	mov   bx,[_ScrnLogicalByteWidth]  ;  by mult. dest Y coord by Screen
	mul   bx                          ;  width then adding screen offset
        mov   di,[ScrnOffs]               ;  store result in DI
        add   di,ax
        mov   cx,[X]                      ; Load X coord into CX and make a
	mov   dx,cx                       ;  copy in DX
	shr   dx,2                        ; Find starting byte in dest row
	add   di,dx                       ;  add to DI giving screen offset of
                                          ;  first pixel's byte
        lds   si,[Bitmap]                 ; DS:SI -> Bitmap data
        lodsw                             ; Al = B.M. width (bytes) AH = B.M.
                                          ;  height
        cmp   Orientation,0
        jz    UnFlippedMasked

	mov   [BMHeight],ah               ; Save source bitmap dimensions
        xor   ah,ah                       ; LineInc = bytes to the begin.
	add   bx,ax                       ;  of bitmaps next row on screen
	mov   [LineInc],bx
	mov   bh,al                       ; Use bh as column loop count
        and   cx,0003h                    ; mask X coord giving plane of 1st
					  ; bitmap pixel(zero CH coincidentally)
	mov   ah,11h                      ; Init. mask for VGA plane selection
        shl   ah,cl                       ; Shift for starting pixel plane
        mov   dx,SC_INDEX                 ; Prepare VGA for cpu to video writes
        mov   al,MAP_MASK
        out   dx,al
        inc   dx
	mov   [Plane],4                   ; Set plane counter to 4
@@PlaneLoop:
        push  di                          ; Save bitmap's start dest. offset
        mov   bl,[BMHeight]               ; Reset row counter (BL)
        mov   al,ah
        out   dx,al                       ; set vga write plane
@@RowLoop:
	mov   cl,bh                       ; Reset Column counter cl
@@ColLoop:
	lodsb                             ; Get next source bitmap byte
	or    al,al                       ; If not zero then write to dest.
	jz    @@NoPixel                   ; otherwise skip to next byte
	mov   es:[di],al
@@NoPixel:
	dec   di
	loop  @@ColLoop                   ; loop if more columns left
	add   di,[LineInc]                ; Move to next row
        dec   bl                          ; decrement row counter
        jnz   @@RowLoop                   ; Jump if more rows left
        pop   di                          ; Restore bitmaps start dest byte
        ror   ah,1                        ; Shift mask for next plane
	sbb   di,0                        ; If wrapped increment dest address
	dec   [Plane]                     ; Decrement plane counter
        jnz   @@PlaneLoop                 ; Jump if more planes left

        pop   ds                          ; restore data segment
        pop   di                          ; restore registers
        pop   si
        mov   sp,bp                       ; dealloc local variables
        pop   bp
        ret
_x_flip_masked_pbm  endp


;----------------------------------------------------------------------
; x_put_masked_pbm - mask write a planar bitmap from system ram to video ram
; all zero source bitmap bytes indicate destination byte to be left unchanged
;
; Source Bitmap structure:
;
;  Width:byte, Height:byte, Bitmap data (plane 0)...Bitmap data (plane 1)..,
;  Bitmap data (plane 2)..,Bitmap data (plane 3)..
;
;  note width is in bytes ie lots of 4 pixels
;
;  x_put_masked_pbm(X,Y,ScrnOffs,char far * Bitmap)
;
;
; LIMITATIONS: No clipping is supported
;              Only supports bitmaps with widths which are a multiple of
;              4 pixels
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
_x_put_masked_pbm  proc
        ARG X:word,Y:word,ScrnOffs:word,Bitmap:dword
	LOCAL Plane:byte,BMHeight:byte,LineInc:word=LocalStk
	push  bp
        mov   bp,sp
	sub   sp,LocalStk                 ; Create space for local variables
        push  si
        push  di
	push  ds
	cld
        mov   ax,SCREEN_SEG
        mov   es,ax
        mov   ax,[Y]                      ; Calculate dest screen row
	mov   bx,[_ScrnLogicalByteWidth]  ;  by mult. dest Y coord by Screen
	mul   bx                          ;  width then adding screen offset
        mov   di,[ScrnOffs]               ;  store result in DI
        add   di,ax
        mov   cx,[X]                      ; Load X coord into CX and make a
	mov   dx,cx                       ;  copy in DX
	shr   dx,2                        ; Find starting byte in dest row
	add   di,dx                       ;  add to DI giving screen offset of
                                          ;  first pixel's byte
        lds   si,[Bitmap]                 ; DS:SI -> Bitmap data
        lodsw                             ; Al = B.M. width (bytes) AH = B.M.
                                          ;  height
UnFlippedMasked:
	mov   [BMHeight],ah               ; Save source bitmap dimensions
        xor   ah,ah                       ; LineInc = bytes to the begin.
	sub   bx,ax                       ;  of bitmaps next row on screen
	mov   [LineInc],bx
	mov   bh,al                       ; Use bh as column loop count
        and   cx,0003h                    ; mask X coord giving plane of 1st
					  ; bitmap pixel(zero CH coincidentally)
	mov   ah,11h                      ; Init. mask for VGA plane selection
        shl   ah,cl                       ; Shift for starting pixel plane
        mov   dx,SC_INDEX                 ; Prepare VGA for cpu to video writes
        mov   al,MAP_MASK
        out   dx,al
        inc   dx
	mov   [Plane],4                   ; Set plane counter to 4
@@PlaneLoop:
        push  di                          ; Save bitmap's start dest. offset
        mov   bl,[BMHeight]               ; Reset row counter (BL)
        mov   al,ah
        out   dx,al                       ; set vga write plane
@@RowLoop:
	mov   cl,bh                       ; Reset Column counter cl
@@ColLoop:
	lodsb                             ; Get next source bitmap byte
	or    al,al                       ; If not zero then write to dest.
	jz    @@NoPixel                   ; otherwise skip to next byte
	mov   es:[di],al
@@NoPixel:
	inc   di
	loop  @@ColLoop                   ; loop if more columns left
	add   di,[LineInc]                ; Move to next row
        dec   bl                          ; decrement row counter
        jnz   @@RowLoop                   ; Jump if more rows left
        pop   di                          ; Restore bitmaps start dest byte
        rol   ah,1                        ; Shift mask for next plane
	adc   di,0                        ; If wrapped increment dest address
	dec   [Plane]                     ; Decrement plane counter
        jnz   @@PlaneLoop                 ; Jump if more planes left

        pop   ds                          ; restore data segment
        pop   di                          ; restore registers
        pop   si
        mov   sp,bp                       ; dealloc local variables
        pop   bp
        ret
_x_put_masked_pbm  endp



;----------------------------------------------------------------------
; x_put_pbm - Write a planar bitmap from system ram to video ram
;
; Source Bitmap structure:
;
;  Width:byte, Height:byte, Bitmap data (plane 0)...Bitmap data (plane 1)..,
;  Bitmap data (plane 2)..,Bitmap data (plane 3)..
;
;  note width is in bytes ie lots of 4 pixels
;
;  x_put_pbm(X,Y,ScrnOffs,char far * Bitmap)
;
;
; LIMITATIONS: No clipping is supported
;              Only supports bitmaps with widths which are a multiple of
;              4 pixels
; FEATURES   : Automatically selects REP MOVSB or REP MOVSW  depending on
;              source bitmap width, by modifying opcode ;-).
;
; Written by Themie Gouthas
;----------------------------------------------------------------------


_x_put_pbm  proc
	ARG X:word,Y:word,ScrnOffs:word,Bitmap:dword
	LOCAL Plane:byte,BMHeight:byte,LineInc:word=LocalStk
	push  bp
        mov   bp,sp
	sub   sp,LocalStk                 ; Create space for local variables
	push  si
        push  di
	push  ds
	cld
        mov   ax,SCREEN_SEG
        mov   es,ax
        mov   ax,[Y]                      ; Calculate dest screen row
	mov   bx,[_ScrnLogicalByteWidth]  ;  by mult. dest Y coord by Screen
	mul   bx                          ;  width then adding screen offset
        mov   di,[ScrnOffs]               ;  store result in DI
        add   di,ax
        mov   cx,[X]                      ; Load X coord into CX and make a
	mov   dx,cx                       ;  copy in DX
	shr   dx,2                        ; Find starting byte in dest row
	add   di,dx                       ;  add to DI giving screen offset of
                                          ;  first pixel's byte
        lds   si,[Bitmap]                 ; DS:SI -> Bitmap data
        lodsw                             ; Al = B.M. width (bytes) AH = B.M.
                                          ;  height
UnFlipped:
        mov   [BMHeight],ah               ; Save source bitmap dimensions
        xor   ah,ah                       ; LineInc = bytes to the begin.
	sub   bx,ax                       ;  of bitmaps next row on screen
	mov   [LineInc],bx
	mov   bh,al
					  ; Self Modifying, Shame, shame shame..
        and   cx,0003h                    ; mask X coord giving plane of 1st
                                          ; bitmap pixel(zero CH coincidentally)
	mov   ah,11h                      ; Init. mask for VGA plane selection
        shl   ah,cl                       ; Shift for starting pixel plane
        mov   dx,SC_INDEX                 ; Prepare VGA for cpu to video writes
        mov   al,MAP_MASK
        out   dx,al
        inc   dx
	mov   [Plane],4                   ; Set plane counter to 4
@@PlaneLoop:
        push  di
	mov   bl,[BMHeight]
        mov   al,ah
        out   dx,al
@@RowLoop:
	mov   cl,bh
	shr   cl,1
	rep   movsw                       ; Copy a complete row for curr plane
	adc   cl,0
	rep   movsb
	add   di,[LineInc]                ; Move to next row
        dec   bl                          ; decrement row counter
        jnz   @@RowLoop                   ; Jump if more rows left
        pop   di                          ; Restore bitmaps start dest byte
        rol   ah,1                        ; Shift mask for next plane
	adc   di,0                        ; If wrapped increment dest address
	dec   [Plane]                     ; Decrement plane counter
        jnz   @@PlaneLoop                 ; Jump if more planes left

        pop   ds                          ; restore data segment
        pop   di                          ; restore registers
        pop   si
        mov   sp,bp                       ; dealloc local variables
        pop   bp
        ret
_x_put_pbm  endp

;----------------------------------------------------------------------
; x_flip_pbm - Write a planar bitmap from system ram to video ram
; If "Orientation" is set then the bitmap is flipped from left to right as
; it is drawn

;
; Source Bitmap structure:
;
;  Width:byte, Height:byte, Bitmap data (plane 0)...Bitmap data (plane 1)..,
;  Bitmap data (plane 2)..,Bitmap data (plane 3)..
;
;  note width is in bytes ie lots of 4 pixels
;
;  x_flip_pbm(X,Y,ScrnOffs,char far * Bitmap, WORD orientation)
;
;
; LIMITATIONS: No clipping is supported
;              Only supports bitmaps with widths which are a multiple of
;              4 pixels
;
; NOTES: The flipped put function is slower than the standard put function
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
_x_flip_pbm  proc
        ARG X:word,Y:word,ScrnOffs:word,Bitmap:dword,Orientation:word
	LOCAL Plane:byte,BMHeight:byte,LineInc:word=LocalStk
	push  bp
        mov   bp,sp
	sub   sp,LocalStk                 ; Create space for local variables
        push  si
        push  di
	push  ds
	cld
        mov   ax,SCREEN_SEG
        mov   es,ax
        mov   ax,[Y]                      ; Calculate dest screen row
	mov   bx,[_ScrnLogicalByteWidth]  ;  by mult. dest Y coord by Screen
	mul   bx                          ;  width then adding screen offset
        mov   di,[ScrnOffs]               ;  store result in DI
        add   di,ax
        mov   cx,[X]                      ; Load X coord into CX and make a
	mov   dx,cx                       ;  copy in DX
	shr   dx,2                        ; Find starting byte in dest row
	add   di,dx                       ;  add to DI giving screen offset of
                                          ;  first pixel's byte
        lds   si,[Bitmap]                 ; DS:SI -> Bitmap data
        lodsw                             ; Al = B.M. width (bytes) AH = B.M.
                                          ;  height
        cmp   Orientation,0
        jz    UnFlipped

	mov   [BMHeight],ah               ; Save source bitmap dimensions
        xor   ah,ah                       ; LineInc = bytes to the begin.
	add   bx,ax                       ;  of bitmaps next row on screen
	mov   [LineInc],bx
	mov   bh,al                       ; Use bh as column loop count
        and   cx,0003h                    ; mask X coord giving plane of 1st
					  ; bitmap pixel(zero CH coincidentally)
	mov   ah,11h                      ; Init. mask for VGA plane selection
        shl   ah,cl                       ; Shift for starting pixel plane
        mov   dx,SC_INDEX                 ; Prepare VGA for cpu to video writes
        mov   al,MAP_MASK
        out   dx,al
        inc   dx
	mov   [Plane],4                   ; Set plane counter to 4
@@PlaneLoop:
        push  di                          ; Save bitmap's start dest. offset
        mov   bl,[BMHeight]               ; Reset row counter (BL)
        mov   al,ah
        out   dx,al                       ; set vga write plane
@@RowLoop:
	mov   cl,bh                       ; Reset Column counter cl
@@ColLoop:
        lodsb
        mov   es:[di],al
        dec   di
        sub   di,2
	loop  @@ColLoop                   ; loop if more columns left
@@DoneCol:
	add   di,[LineInc]                ; Move to next row
        dec   bl                          ; decrement row counter
        jnz   @@RowLoop                   ; Jump if more rows left
        pop   di                          ; Restore bitmaps start dest byte
        ror   ah,1                        ; Shift mask for next plane
	sbb   di,0                        ; If wrapped increment dest address
	dec   [Plane]                     ; Decrement plane counter
        jnz   @@PlaneLoop                 ; Jump if more planes left

        pop   ds                          ; restore data segment
        pop   di                          ; restore registers
        pop   si
        mov   sp,bp                       ; dealloc local variables
        pop   bp
        ret
_x_flip_pbm  endp


;----------------------------------------------------------------------
; x_get_pbm - Read a planar bitmap to system ram from video ram
;
; Source Bitmap structure:
;
;  Width:byte, Height:byte, Bitmap data (plane 0)...Bitmap data (plane 1)..,
;  Bitmap data (plane 2)..,Bitmap data (plane 3)..
;
;  note width is in bytes ie lots of 4 pixels
;
;  x_get_pbm(X,Y,BMwidth,BMheight,ScrnOffs,char far * Bitmap)
;
;
; LIMITATIONS: No clipping is supported
;              Only supports bitmaps with widths which are a multiple of
;              4 pixels
; FEATURES   : Automatically selects REP MOVSB or REP MOVSW  depending on
;              source bitmap width, by modifying opcode ;-).
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
_x_get_pbm  proc
	ARG X:word,Y:word,SrcWidth:byte,SrcHeight:byte,ScrnOffs:word,Bitmap:dword
	LOCAL Plane:byte,LineInc:word=LocalStk
	push  bp
        mov   bp,sp
	sub   sp,LocalStk                 ; Create space for local variables
        push  si
        push  di
        push  ds
	cld

	mov   ax,[Y]                      ; Calculate screen row
	mov   bx,[_ScrnLogicalByteWidth]  ;  by mult. Y coord by Screen
	mul   bx                          ;  width then adding screen offset
	mov   si,[ScrnOffs]               ;  store result in SI
	add   si,ax
        mov   cx,[X]                      ; Load X coord into CX and make a
	mov   dx,cx                       ;  copy in DX
	shr   dx,2                        ; Find starting byte in screen row
	add   si,dx                       ;  add to SI giving screen offset of
					  ;  first pixel's byte
	mov   ax,SCREEN_SEG
	mov   ds,ax
	les   di,[Bitmap]                 ; ES:DI -> Bitmap data
	mov   al,[SrcWidth]
	mov   ah,[SrcHeight]
	stosw                             ; Al = B.M. width (bytes) AH = B.M.
                                          ;  height
        xor   ah,ah                       ; LineInc = bytes to the begin.
	sub   bx,ax                       ;  of bitmaps next row on screen
	mov   [LineInc],bx
	mov   bh,al
					  ; Self Modifying, Shame, shame shame..
        and   cx,0003h                    ; mask X coord giving plane of 1st
                                          ; bitmap pixel(zero CH coincidentally)
	mov   ah,11h                      ; Init. mask for VGA plane selection
	shl   ah,cl                       ; Shift for starting pixel plane
	mov   dx,GC_INDEX                 ; Prepare VGA for cpu to video reads
	mov   al,READ_MAP
        out   dx,al
        inc   dx
	mov   [Plane],4                   ; Set plane counter (BH) to 4
	mov   al,cl
@@PlaneLoop:
	push  si
	mov   bl,[SrcHeight]
        out   dx,al
@@RowLoop:
	mov   cl,bh
	shr   cl,1
	rep   movsw                       ; Copy a complete row for curr plane
	adc   cl,0
	rep   movsb
	add   si,[LineInc]                ; Move to next row
        dec   bl                          ; decrement row counter
        jnz   @@RowLoop                   ; Jump if more rows left
	pop   si                          ; Restore bitmaps start dest byte

	inc   al                          ; Select next plane to read from
	and   al,3                        ;

	rol   ah,1                        ; Shift mask for next plane
	adc   si,0                        ; If wrapped increment dest address
	dec   [Plane]                     ; Decrement plane counter
	jnz   @@PlaneLoop                 ; Jump if more planes left

        pop   ds                          ; restore data segment
        pop   di                          ; restore registers
        pop   si
        mov   sp,bp                       ; dealloc local variables
        pop   bp
        ret
_x_get_pbm  endp




	end



        ARG X:word,Y:word,ScrnOffs:word,Bitmap:dword,Orientation:word
	LOCAL Plane:byte,BMHeight:byte,LineInc:word,Columns:byte=LocalStk
	push  bp
        mov   bp,sp
	sub   sp,LocalStk                 ; Create space for local variables
        push  si
        push  di
	push  ds
	cld
        mov   ax,SCREEN_SEG
        mov   es,ax
        mov   ax,[Y]                      ; Calculate dest screen row
	mov   bx,[_ScrnLogicalByteWidth]  ;  by mult. dest Y coord by Screen
	mul   bx                          ;  width then adding screen offset
        mov   di,[ScrnOffs]               ;  store result in DI
        add   di,ax
        mov   cx,[X]                      ; Load X coord into CX and make a
	mov   dx,cx                       ;  copy in DX
	shr   dx,2                        ; Find starting byte in dest row
	add   di,dx                       ;  add to DI giving screen offset of
                                          ;  first pixel's byte
        lds   si,[Bitmap]                 ; DS:SI -> Bitmap data
        lodsw                             ; Al = B.M. width (bytes) AH = B.M.
                                          ;  height
        cmp   Orientation,0
        jz    UnFlipped

	mov   [BMHeight],ah               ; Save source bitmap dimensions
        xor   ah,ah                       ; LineInc = bytes to the begin.
	add   bx,ax                       ;  of bitmaps next row on screen
	mov   [LineInc],bx
	mov   [Columns],al                ; Use bh as column loop count
        and   cx,0003h                    ; mask X coord giving plane of 1st
					  ; bitmap pixel(zero CH coincidentally)
	mov   ah,11h                      ; Init. mask for VGA plane selection
        shl   ah,cl                       ; Shift for starting pixel plane
        mov   bh,ah
        mov   dx,SC_INDEX                 ; Prepare VGA for cpu to video writes
        mov   al,MAP_MASK
        out   dx,al
        inc   dx
	mov   [Plane],4                   ; Set plane counter to 4
@@PlaneLoop:
        push  di                          ; Save bitmap's start dest. offset
        mov   bl,[BMHeight]               ; Reset row counter (BL)
        mov   al,bh
        out   dx,al                       ; set vga write plane
@@RowLoop:
	mov   cl,[Columns]                ; Reset Column counter cl
        shr   cx,1
        jnc   @@ColLoop
        lodsb
        mov   es:[di],al
	dec   di
@@ColLoop:
	lodsw                             ; Get next source bitmap byte
        xchg  al,ah
	mov   es:[di],ax
	sub   di,2
	loop  @@ColLoop                   ; loop if more columns left

	add   di,[LineInc]                ; Move to next row
        dec   bl                          ; decrement row counter
        jnz   @@RowLoop                   ; Jump if more rows left
        pop   di                          ; Restore bitmaps start dest byte
        ror   bh,1                        ; Shift mask for next plane
	sbb   di,0                        ; If wrapped increment dest address
	dec   [Plane]                     ; Decrement plane counter
        jnz   @@PlaneLoop                 ; Jump if more planes left

        pop   ds                          ; restore data segment
        pop   di                          ; restore registers
        pop   si
        mov   sp,bp                       ; dealloc local variables
        pop   bp
        ret