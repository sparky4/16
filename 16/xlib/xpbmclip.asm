;-----------------------------------------------------------------------
; MODULE XPBMCLIP
;
; Clipped Planar Bitmap functions - System Ram <-> Video Ram
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
LOCALS
include xlib.inc
include xpbmclip.inc

	.code

;----------------------------------------------------------------------
; x_put_masked_pbm_clipx - mask write a planar bitmap from system ram to video
;			ram all zero source bitmap bytes indicate destination
;			byte to be left unchanged.
;                       Performs clipping in x directions. similar to
;		        "x_put_masked_pbm".
;
; See Also:  x_put_masked_pbm, x_put_masked_pbm_clipxy
;
; Clipping region variables: LeftClip,RightClip
;
; Written by Themie Gouthas
;
; This code is a SLOW hack, any better ideas are welcome
;----------------------------------------------------------------------
_x_put_masked_pbm_clipx  proc
ARG X:word,Y:word,ScrnOffs:word,Bitmap:dword
LOCAL   Plane:byte,CType,LeftSkip,DataInc,Width,Height,TopRow,LineInc:word=LocalStk
	push  bp
        mov   bp,sp
	sub   sp,LocalStk                 ; Create space for local variables
        push  si
        push  di
        push  ds
	cld

	les   si,[Bitmap]                 ; Point ES:SI to start of bitmap

	xor   ax,ax                       ; Clear AX
	mov   [CType],ax                  ; Clear Clip type descision var
	mov   al,byte ptr es:[si]         ; AX=width (byte coverted to word)


	mov   di,[X]                      ; DI = X coordinate of dest
	mov   cx,di                       ; copy to CX
	sar   di,2                        ; convert to offset in row


	;;;;; CLIP PROCESSING FOR LEFT CLIP BORDER ;;;;;;;;;;;;;;;;;;;

	mov   dx,[_LeftClip]              ; Is X Coord to the right of
	sub   dx,di                       ; LeftClip ?
	jle   @@NotLeftClip               ; Yes! => no left clipping
	cmp   dx,ax                       ; Is dist of X Coord from
	jnl   @@NotVisible                ; ClipLeft > Width ? yes => the
					  ; bitmap is not visible
	add   di,dx
	mov   [LeftSkip],dx
	mov   [DataInc],dx
	sub   ax,dx
	mov   [CType],1
	jmp   short @@HorizClipDone

	;;;; EXIT FOR COMPLETELY OBSCURED P.B.M's ;;;;;;;;;;;;;;;;;;;;;;

@@NotVisible:
	mov   ax,1
	pop   ds                          ; restore data segment
	pop   di                          ; restore registers
	pop   si
	mov   sp,bp                       ; dealloc local variables
	pop   bp
	ret

	;;;;; CLIP PROCESSING FOR RIGHT CLIP BORDER ;;;;;;;;;;;;;;;;;;;

@@NotLeftClip:
	mov   dx,[_RightClip]
	sub   dx,di
	js    @@NotVisible
	mov   [LeftSkip],0
	mov   [DataInc],0
	cmp   dx,ax
	jge   @@HorizClipDone       ; was jg
	inc   dx
	sub   ax,dx
	mov   [DataInc],ax
	mov   ax,dx
	mov   [CType],-1

@@HorizClipDone:

	xor   bh,bh
	mov   bl,byte ptr es:[si+1]       ; BX = height

	mov   [Width],ax                  ; Save width and height of clipped
	mov   [Height],bx                 ;  image


	add   si,2                        ; Skip dimension bytes in source
	add   si,[LeftSkip]               ; Skip pixels in front of row that
					  ;  are clipped


	mov   bx,[_ScrnLogicalByteWidth]  ; Set BX to Logical Screen Width
	mov   dx,bx                       ; BX - Width of image = No. bytes
	sub   dx,ax                       ;  to first byte of next screen
	mov   [LineInc],dx                ;  row.

	mov   ax,[Y]                      ; Calculate screen start row
	mul   bx                          ;  then adding screen offset
	add   di,ax
	add   di,[ScrnOffs]
	mov   ax,es                       ; copy ES to DS
	mov   ds,ax
	mov   ax,SCREEN_SEG               ; Point ES to VGA segment
	mov   es,ax

	and   cx,3
	mov   ah,11h                      ; Set up initial plane mask
	shl   ah,cl

	mov   dx,SC_INDEX                 ; Prepare VGA for cpu to video writes
	mov   al,MAP_MASK
	out   dx,al
	inc   dx
	mov   [Plane],4                   ; Set plane counter to 4
	mov   bh,byte ptr [Width]         ; set bh to width for fast looping
@@PlaneLoop:
	push  di 			  ; Save bitmap's start dest. offset
	mov   bl,byte ptr [Height]        ; Reset row counter (BL)
	mov   al,ah
	out   dx,al                       ; set vga write plane
@@RowLoop:
	mov   cl,bh                       ; Reset Column counter cl
	jcxz  @@NoWidth
@@ColLoop:
	lodsb        		          ; Get next source bitmap byte
	or    al,al                       ; If not zero then write to dest.
	jz    @@NoPixel                   ; otherwise skip to next byte
	mov   es:[di],al
@@NoPixel:
	inc   di
	loop @@ColLoop
@@NoWidth:
	add   si,[DataInc]                ; Move to next source row
	add   di,[LineInc]                ; Move to next screen row
	dec   bl                          ; decrement row counter
	jnz   @@RowLoop                   ; Jump if more rows left
	pop   di                          ; Restore bitmaps start dest byte
	rol   ah,1		          ; Shift mask for next plane

	; Plane Transition (A HACK but it works!)

	jnb   @@Nocarry                   ; Jump if not plane transition
	mov   bl,ah                       ; Save Plane Mask
	mov   ax,[CType]                  ; set AX to clip type inc variable
	add   bh,al                       ; Update advancing variables
	sub   [DataInc],ax                ;
	sub   [LineInc],ax                ;
	cmp   al,0                        ; What type of clip do we have
	mov   ah,bl                       ;   restore Plane mask
	jg    @@RightAdvance              ; jump on a right clip!
	inc   di                          ; otherwise increment DI
	jmp   @@Nocarry
@@RightAdvance:
	dec si
@@Nocarry:
	dec   [Plane]                     ; Decrement plane counter
	jnz   @@PlaneLoop                 ; Jump if more planes left

	xor   ax,ax
	pop   ds                          ; restore data segment
	pop   di                          ; restore registers
	pop   si
	mov   sp,bp                       ; dealloc local variables
	pop   bp
	ret
_x_put_masked_pbm_clipx  endp


;----------------------------------------------------------------------
; x_put_masked_pbm_clipy - mask write a planar bitmap from system ram to video
;			ram all zero source bitmap bytes indicate destination
;			byte to be left unchanged.
;                       Performs clipping in y direction. similar to
;		        "x_put_masked_pbm".
;
; See Also:  x_put_masked_pbm, x_put_masked_pbm_clipx, x_put_masked_pbm_clipy
;
; Clipping region variables: TopClip,BottomClip
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
_x_put_masked_pbm_clipy  proc
ARG X:word,Y:word,ScrnOffs:word,Bitmap:dword
LOCAL   Width,Height,TopRow,LineInc,PlaneInc:word=LocalStk
	push  bp
	mov   bp,sp
	sub   sp,LocalStk                 ; Create space for local variables
	push  si
	push  di
	push  ds
	cld

	les   si,[Bitmap]

	xor   bh,bh
	mov   bl,byte ptr es:[si+1]       ; BX = height

	xor   ah,ah
	mov   al,byte ptr es:[si]         ; AX = width

	mov   cx,ax                       ; Save AX
	mul   bx                          ; AX = AX*BX = bytes/plane
	mov   [PlaneInc],ax               ;  save as PlaneInc
	mov   ax,cx                       ; Restore AX

	mov   di,[X]
	mov   cx,di
	shr   di,2

	;;;;; CLIP PROCESSING FOR TOP CLIP BORDER ;;;;;;;;;;;;;;;;;;;;;

	mov   dx,[_TopClip]           ; Compare u.l. Y coord with Top
	sub   dx,[Y]                  ; clipping border
	jle   @@NotTopClip            ; jump if VBM not clipped from above
	cmp   dx,bx
	jnl   @@NotVisible            ; jump if VBM is completely obscured
	mov   [TopRow],dx
	sub   bx,dx
	add   [Y],dx
	jmp   short @@VertClipDone

	;;;; EXIT FOR COMPLETELY OBSCURED P.B.M's ;;;;;;;;;;;;;;;;;;;;;;

@@NotVisible:
	mov   ax,1
	pop   ds                          ; restore data segment
	pop   di                          ; restore registers
	pop   si
	mov   sp,bp                       ; dealloc local variables
	pop   bp
	ret

	;;;;; CLIP PROCESSING FOR BOTTOM CLIP BORDER ;;;;;;;;;;;;;;;;;;;

@@NotTopClip:
	mov   dx,[_BottomClip]
	sub   dx,[Y]
	js    @@NotVisible
	mov   [TopRow],0
	cmp   dx,bx
	jg    @@VertClipDone
	inc   dx
	mov   bx,dx

@@VertClipDone:

	mov   [Width],ax
	mov   [Height],bx                 ; Calculate relative offset in data
	mul   [TopRow]                    ;  of first visible scanline
	add   ax,2                        ; Skip dimension bytes in source
	add   si,ax                       ; Skip top rows that arent visible


	mov   ax,[Y]                      ; Calculate screen row
	mov   bx,[_ScrnLogicalByteWidth]  ;  by mult. Y coord by Screen
	mul   bx                          ;  width then adding screen offset
	add   di,ax
	add   di,[ScrnOffs]
	sub   bx,[Width]                  ; calculate difference from end of
	mov   [LineInc],bx                ; b.m. in curr line to beginning of
					  ; b.m. on next scan line
	mov   ax,es                       ; copy ES to DS
	mov   ds,ax
	mov   ax,SCREEN_SEG               ; Point ES to VGA segment
	mov   es,ax

	mov   ah,11h                      ; Set up initial plane mask
	and   cx,3
	shl   ah,cl

	mov   dx,SC_INDEX                 ; Prepare VGA for cpu to video writes
	mov   al,MAP_MASK
	out   dx,al
	inc   dx
	mov   bh,4                        ; Set plane counter to 4
@@PlaneLoop:
	push  di 			  ; Save bitmap's start dest. offset
	push  si                          ; Save Bitmaps data offset
	mov   bl,byte ptr [Height]        ; Reset row counter (BL)
	mov   al,ah
	out   dx,al                       ; set vga write plane
@@RowLoop:
	mov   cl,byte ptr [Width]         ; Reset Column counter cl
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
	pop   si                          ; Restore SI and set to offset of
	add   si,[PlaneInc]               ; first vis pixel in next plane data
	pop   di                          ; Restore bitmaps start dest byte
	rol   ah,1                        ; Shift mask for next plane
	adc   di,0                        ; if carry increment screen offset
	dec   bh                          ; Decrement plane counter
	jnz   @@PlaneLoop                 ; Jump if more planes left

	xor   ax,ax
	pop   ds                          ; restore data segment
	pop   di                          ; restore registers
	pop   si
	mov   sp,bp                       ; dealloc local variables
	pop   bp
	ret
_x_put_masked_pbm_clipy   endp

;----------------------------------------------------------------------
; x_put_masked_pbm_clipxy - Write a planar bitmap from system ram to video
;                    RAM with clipping in x and y directions. similar to
;		     "x_put_masked_pbm".
;
; See Also: x_put_masked_pbm, x_put_masked_pbm_clipx, x_put_masked_pbm_clipxy
;
; Clipping region variables: LeftClip,RightClip,TopClip,BottomClip
;
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
_x_put_masked_pbm_clipxy  proc
ARG X:word,Y:word,ScrnOffs:word,Bitmap:dword
LOCAL   Plane:byte,CType,LeftSkip,DataInc,Width,Height,TopRow,LineInc,PlaneInc:word=LocalStk
	push  bp
	mov   bp,sp
	sub   sp,LocalStk                 ; Create space for local variables
	push  si
	push  di
	push  ds
	cld

	les   si,[Bitmap]

	xor   ax,ax
	mov   [CType],ax
	mov   al,byte ptr es:[si]         ; AX = width
	xor   bh,bh
	mov   bl,byte ptr es:[si+1]       ; BX = height

	mov   cx,ax                       ; Save AX
	mul   bx                          ; AX = AX*BX = bytes/plane
	mov   [PlaneInc],ax               ;  save as PlaneInc
	mov   ax,cx                       ; Restore AX


	mov   di,[X]                      ; DI = X coordinate of dest.
	mov   cx,di                       ; save in CX
	sar   di,2                        ; convert to address byte


		;;;;; CLIP PROCESSING FOR TOP CLIP BORDER ;;;;;;;;;;;;;;;;;;;;;

	mov   dx,[_TopClip]           ; Compare u.l. Y coord with Top
	sub   dx,[Y]                  ; clipping border
	jle   @@NotTopClip            ; jump if VBM not clipped from above
	cmp   dx,bx
	jnl   @@NotVisible            ; jump if VBM is completely obscured
	mov   [TopRow],dx
	sub   bx,dx
	add   [Y],dx
	jmp   short @@VertClipDone

	;;;; EXIT FOR COMPLETELY OBSCURED P.B.M's ;;;;;;;;;;;;;;;;;;;;;;

@@NotVisible:
	mov   ax,1
	pop   ds                          ; restore data segment
	pop   di                          ; restore registers
	pop   si
	mov   sp,bp                       ; dealloc local variables
	pop   bp
	ret

	;;;;; CLIP PROCESSING FOR BOTTOM CLIP BORDER ;;;;;;;;;;;;;;;;;;;

@@NotTopClip:
	mov   dx,[_BottomClip]
	sub   dx,[Y]
	js    @@NotVisible
	mov   [TopRow],0
	cmp   dx,bx
	jg    @@VertClipDone
	inc   dx
	mov   bx,dx

@@VertClipDone:

	;;;;; CLIP PROCESSING FOR LEFT CLIP BORDER ;;;;;;;;;;;;;;;;;;;

	mov   dx,[_LeftClip]
	sub   dx,di
	jle   @@NotLeftClip
	cmp   dx,ax
	jnl   @@NotVisible

	add   di,dx
	mov   [LeftSkip],dx
	mov   [DataInc],dx
	sub   ax,dx
	mov   [CType],1
	jmp   short @@HorizClipDone

	;;;;; CLIP PROCESSING FOR RIGHT CLIP BORDER ;;;;;;;;;;;;;;;;;;;

@@NotLeftClip:
	mov   dx,[_RightClip]
	sub   dx,di
	js    @@NotVisible
	mov   [LeftSkip],0
	mov   [DataInc],0
	cmp   dx,ax
        jge   @@HorizClipDone       ; was jg
	inc   dx
	sub   ax,dx
	mov   [DataInc],ax
	mov   ax,dx


	mov   [CType],-1

@@HorizClipDone:



	mov   [Width],ax                  ; Save width and height of clipped
	mov   [Height],bx                 ;  image

	add   ax,[DataInc]                ; AX = original width of image
	mul   [TopRow]                    ; Calculate bytes in clipped top
	add   si,ax		          ;  rows
	add   si,2                        ; Skip dimension bytes in source
	add   si,[LeftSkip]               ; Skip pixels in front of row that
					  ;  are clipped

	mov   bx,[_ScrnLogicalByteWidth]  ; Set BX to Logical Screen Width
	mov   dx,bx                       ; BX - Width of image = No. bytes
	sub   dx,[Width]                  ;  to first byte of next screen
	mov   [LineInc],dx                ;  row.

	mov   ax,[Y]                      ; Calculate screen start row
	mul   bx                          ;  then adding screen offset
	add   di,ax
	add   di,[ScrnOffs]
	mov   ax,es                       ; copy ES to DS
	mov   ds,ax
	mov   ax,SCREEN_SEG               ; Point ES to VGA segment
	mov   es,ax



	and   cx,3
	mov   ah,11h                      ; Set up initial plane mask
	shl   ah,cl

	mov   dx,SC_INDEX                 ; Prepare VGA for cpu to video writes
	mov   al,MAP_MASK
	out   dx,al
	inc   dx
	mov   [Plane],4                   ; Set plane counter to 4
	mov   bh,byte ptr [Width]         ; set bh to width for fast looping
@@PlaneLoop:
	push  di 			  ; Save bitmap's start dest. offset
	push  si
	mov   bl,byte ptr [Height]        ; Reset row counter (BL)
	mov   al,ah
	out   dx,al                       ; set vga write plane
@@RowLoop:
	mov   cl,bh                       ; Reset Column counter cl
	jcxz   @@NoWidth
@@ColLoop:
	lodsb        		          ; Get next source bitmap byte
	or    al,al                       ; If not zero then write to dest.
	jz    @@NoPixel                   ; otherwise skip to next byte
	mov   es:[di],al
@@NoPixel:
	inc   di
	loop @@ColLoop
@@NoWidth:
	add   si,[DataInc]                ; Move to next source row
	add   di,[LineInc]                ; Move to next screen row
	dec   bl                          ; decrement row counter
	jnz   @@RowLoop                   ; Jump if more rows left
	pop   si                          ; Restore SI and set to offset of
	add   si,[PlaneInc]               ; first vis pixel in next plane data
	pop   di                          ; Restore bitmaps start dest byte
	rol   ah,1		          ; Shift mask for next plane

	; Plane Transition (A HACK but it works!)

	jnb   @@Nocarry                   ; Jump if not plane transition
	mov   bl,ah                       ; Save Plane Mask
	mov   ax,[CType]                  ; set AX to clip type inc variable
	add   bh,al                       ; Update advancing variables
	sub   [DataInc],ax                ;
	sub   [LineInc],ax                ;
	cmp   al,0                        ; What type of clip do we have
	mov   ah,bl                       ;   restore Plane mask
	jg    @@RightAdvance              ; jump on a right clip!
	inc   di                          ; otherwise increment DI
	jmp   @@Nocarry
@@RightAdvance:
	dec   si
@@Nocarry:
	dec   [Plane]                     ; Decrement plane counter
	jnz   @@PlaneLoop                 ; Jump if more planes left

	xor   ax,ax
	pop   ds                          ; restore data segment
	pop   di                          ; restore registers
	pop   si
	mov   sp,bp                       ; dealloc local variables
	pop   bp
	ret
_x_put_masked_pbm_clipxy  endp




;----------------------------------------------------------------------
; x_put_pbm_clipx - Write a planar bitmap from system ram to video ram
;                    with clipping in x and y directions. similar to
;		     "x_put_pbm".
;
; See Also:  x_put_pbm_clip
;
;
; See Also:  x_put_pbm,x_put_pbm_clipy,x_put_pbm_clipxy
;
; Clipping region variables: LeftClip,RightClip
;
; Written by Themie Gouthas
;
; This code is a SLOW hack, any better ideas are welcome
;----------------------------------------------------------------------
_x_put_pbm_clipx  proc
ARG X:word,Y:word,ScrnOffs:word,Bitmap:dword
LOCAL   Plane:byte,CType,LeftSkip,DataInc,Width,Height,TopRow,LineInc:word=LocalStk
	push  bp
	mov   bp,sp
	sub   sp,LocalStk                 ; Create space for local variables
	push  si
	push  di
	push  ds
	cld

	les   si,[Bitmap]

	xor   ax,ax
	mov   [CType],ax
	mov   al,byte ptr es:[si]         ; AX = width


	mov   di,[X]                      ; DI = X coordinate of dest.
	mov   cx,di                       ; save in CX
	sar   di,2                        ; convert to address byte



	;;;;; CLIP PROCESSING FOR LEFT CLIP BORDER ;;;;;;;;;;;;;;;;;;;

	mov   dx,[_LeftClip]
	sub   dx,di
	jle   @@NotLeftClip
	cmp   dx,ax
	jnl   @@NotVisible

	add   di,dx
	mov   [LeftSkip],dx
	mov   [DataInc],dx
	sub   ax,dx
	mov   [CType],1
	jmp   short @@HorizClipDone

	;;;; EXIT FOR COMPLETELY OBSCURED P.B.M's ;;;;;;;;;;;;;;;;;;;;;;

@@NotVisible:
	mov   ax,1
	pop   ds                          ; restore data segment
	pop   di                          ; restore registers
	pop   si
	mov   sp,bp                       ; dealloc local variables
	pop   bp
	ret

	;;;;; CLIP PROCESSING FOR RIGHT CLIP BORDER ;;;;;;;;;;;;;;;;;;;

@@NotLeftClip:
	mov   dx,[_RightClip]
	sub   dx,di
	js    @@NotVisible
	mov   [LeftSkip],0
	mov   [DataInc],0
	cmp   dx,ax
        jge   @@HorizClipDone       ; was jg
	inc   dx
	sub   ax,dx
	mov   [DataInc],ax
	mov   ax,dx
	mov   [CType],-1

@@HorizClipDone:

	xor   bh,bh
	mov   bl,byte ptr es:[si+1]       ; BX = height

	mov   [Width],ax                  ; Save width and height of clipped
	mov   [Height],bx                 ;  image


	add   si,2                        ; Skip dimension bytes in source
	add   si,[LeftSkip]               ; Skip pixels in front of row that
					  ;  are clipped


	mov   bx,[_ScrnLogicalByteWidth]  ; Set BX to Logical Screen Width
	mov   dx,bx                       ; BX - Width of image = No. bytes
	sub   dx,ax                       ;  to first byte of next screen
	mov   [LineInc],dx                ;  row.

	mov   ax,[Y]                      ; Calculate screen start row
	mul   bx                          ;  then adding screen offset
	add   di,ax
	add   di,[ScrnOffs]
	mov   ax,es                       ; copy ES to DS
	mov   ds,ax
	mov   ax,SCREEN_SEG               ; Point ES to VGA segment
	mov   es,ax

	and   cx,3
	mov   ah,11h                      ; Set up initial plane mask
	shl   ah,cl

	mov   dx,SC_INDEX                 ; Prepare VGA for cpu to video writes
	mov   al,MAP_MASK
	out   dx,al
	inc   dx
	mov   [Plane],4                   ; Set plane counter to 4
	mov   bh,byte ptr [Width]         ; set bh to width for fast looping
@@PlaneLoop:
	push  di 			  ; Save bitmap's start dest. offset
	mov   bl,byte ptr [Height]        ; Reset row counter (BL)
	mov   al,ah
	out   dx,al                       ; set vga write plane
@@RowLoop:
	mov   cl,bh                       ; Reset Column counter cl
	shr   cl,1
	rep   movsw                       ; Copy a complete row
	adc   cl,0
	rep   movsb
	add   si,[DataInc]                ; Move to next source row
	add   di,[LineInc]                ; Move to next screen row
	dec   bl                          ; decrement row counter
	jnz   @@RowLoop                   ; Jump if more rows left
	pop   di                          ; Restore bitmaps start dest byte
	rol   ah,1		          ; Shift mask for next plane

	; Plane Transition (A HACK but it works!)

	jnb   @@Nocarry                   ; Jump if not plane transition
	mov   bl,ah                       ; Save Plane Mask
	mov   ax,[CType]                  ; set AX to clip type inc variable
	add   bh,al                       ; Update advancing variables
	sub   [DataInc],ax                ;
	sub   [LineInc],ax                ;
	cmp   al,0                        ; What type of clip do we have
	mov   ah,bl                       ;   restore Plane mask
	jg    @@RightAdvance              ; jump on a right clip!
	inc   di                          ; otherwise increment DI
	jmp   @@Nocarry
@@RightAdvance:
	dec si
@@Nocarry:
	dec   [Plane]                     ; Decrement plane counter
	jnz   @@PlaneLoop                 ; Jump if more planes left

	xor   ax,ax
	pop   ds                          ; restore data segment
	pop   di                          ; restore registers
	pop   si
	mov   sp,bp                       ; dealloc local variables
	pop   bp
	ret
_x_put_pbm_clipx  endp



;----------------------------------------------------------------------
; x_put_pbm_clipy - Write a planar bitmap from system ram to video ram
;                    with clipping in y direction only. similar to
;		     "x_put_pbm".
;
; See Also:  x_put_pbm,x_put_pbm_clipx,x_put_pbm_clipxy
;
; Clipping region variables: TopClip,BottomClip
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
_x_put_pbm_clipy  proc
ARG X:word,Y:word,ScrnOffs:word,Bitmap:dword
LOCAL   Width,Height,TopRow,LineInc,PlaneInc:word=LocalStk
	push  bp
	mov   bp,sp
	sub   sp,LocalStk                 ; Create space for local variables
	push  si
	push  di
	push  ds
	cld

	les   si,[Bitmap]

	xor   bh,bh
	mov   bl,byte ptr es:[si+1]   ; BX = height
	;mov   [Height],bx

	xor   ah,ah
	mov   al,byte ptr es:[si]     ; AX = width
	mov   [Width],ax

	mov   cx,ax                       ; Save AX
	mul   bx                          ; AX = AX*BX = bytes/plane
	mov   [PlaneInc],ax               ;  save as PlaneInc
	mov   ax,cx                       ; Restore AX

	mov   di,[X]
	mov   cx,di
	and   cx,3
	shr   di,2

	;;;;; CLIP PROCESSING FOR TOP CLIP BORDER ;;;;;;;;;;;;;;;;;;;;;

	mov   dx,[_TopClip]           ; Compare u.l. Y coord with Top
	sub   dx,[Y]                  ; clipping border
	jle   @@NotTopClip            ; jump if VBM not clipped from above
	cmp   dx,bx
	jnl   @@NotVisible            ; jump if VBM is completely obscured
	mov   [TopRow],dx
	sub   bx,dx
	add   [Y],dx
	jmp   short @@VertClipDone

	;;;; EXIT FOR COMPLETELY OBSCURED P.B.M's ;;;;;;;;;;;;;;;;;;;;;;

@@NotVisible:
	mov   ax,1
	pop   ds                          ; restore data segment
	pop   di                          ; restore registers
	pop   si
	mov   sp,bp                       ; dealloc local variables
	pop   bp
	ret

	;;;;; CLIP PROCESSING FOR BOTTOM CLIP BORDER ;;;;;;;;;;;;;;;;;;;

@@NotTopClip:
	mov   dx,[_BottomClip]
	sub   dx,[Y]
	js    @@NotVisible
	mov   [TopRow],0
	cmp   dx,bx
	jg    @@VertClipDone
	inc   dx
	mov   bx,dx

@@VertClipDone:

	mov   [Height],bx                 ; Calculate relative offset in data
	mul   [TopRow]                    ;  of first visible scanline
	add   ax,2                        ; Skip dimension bytes in source
	add   si,ax                       ; Skip top rows that arent visible


	mov   ax,[Y]                      ; Calculate screen row
	mov   bx,[_ScrnLogicalByteWidth]  ;  by mult. Y coord by Screen
	mul   bx                          ;  width then adding screen offset
	add   di,ax
	add   di,[ScrnOffs]
	sub   bx,[Width]                  ; calculate difference from end of
	mov   [LineInc],bx                ; b.m. in curr line to beginning of
					  ; b.m. on next scan line
	mov   ax,es                       ; copy ES to DS
	mov   ds,ax
	mov   ax,SCREEN_SEG               ; Point ES to VGA segment
	mov   es,ax

	mov   ah,11h                      ; Set up initial plane mask
	shl   ah,cl

	mov   dx,SC_INDEX                 ; Prepare VGA for cpu to video writes
	mov   al,MAP_MASK
	out   dx,al
	inc   dx
	mov   bh,4                        ; Set plane counter to 4
@@PlaneLoop:
	push  di 			  ; Save bitmap's start dest. offset
	push  si                          ; Save Bitmaps data offset
	mov   bl,byte ptr [Height]        ; Reset row counter (BL)
	mov   al,ah
	out   dx,al                       ; set vga write plane
@@RowLoop:
	mov   cl,byte ptr [Width]         ; Reset Column counter cl
	shr   cl,1
	rep   movsw                       ; Copy a complete row
	adc   cl,0
	rep   movsb

	add   di,[LineInc]                ; Move to next row
	dec   bl                          ; decrement row counter
	jnz   @@RowLoop                   ; Jump if more rows left
	pop   si                          ; Restore SI and set to offset of
	add   si,[PlaneInc]               ; first vis pixel in next plane data
	pop   di                          ; Restore bitmaps start dest byte
	rol   ah,1                        ; Shift mask for next plane
	adc   di,0                        ; if carry increment screen offset
	dec   bh                          ; Decrement plane counter
	jnz   @@PlaneLoop                 ; Jump if more planes left

	xor   ax,ax
	pop   ds                          ; restore data segment
	pop   di                          ; restore registers
	pop   si
	mov   sp,bp                       ; dealloc local variables
	pop   bp
	ret
_x_put_pbm_clipy   endp


;----------------------------------------------------------------------
; x_put_pbm_clipxy - Write a planar bitmap from system ram to video ram
;                    with clipping in x and y directions. similar to
;		     "x_put_pbm".
;
; See Also:  x_put_pbm,x_put_pbm_clipy,x_put_pbm_clipx
;
; Clipping region variables: LeftClip,RightClip,TopClip,BottomClip
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
_x_put_pbm_clipxy  proc
ARG X:word,Y:word,ScrnOffs:word,Bitmap:dword
LOCAL   Plane:byte,CType,LeftSkip,DataInc,Width,Height,TopRow,LineInc,PlaneInc:word=LocalStk
	push  bp
	mov   bp,sp
	sub   sp,LocalStk                 ; Create space for local variables
	push  si
	push  di
	push  ds
	cld

	les   si,[Bitmap]

	xor   ax,ax
	mov   [CType],ax
	mov   al,byte ptr es:[si]         ; AX = width
	xor   bh,bh
	mov   bl,byte ptr es:[si+1]       ; BX = height

	mov   cx,ax                       ; Save AX
	mul   bx                          ; AX = AX*BX = bytes/plane
	mov   [PlaneInc],ax               ;  save as PlaneInc
	mov   ax,cx                       ; Restore AX


	mov   di,[X]                      ; DI = X coordinate of dest.
	mov   cx,di                       ; save in CX
	sar   di,2                        ; convert to address byte


		;;;;; CLIP PROCESSING FOR TOP CLIP BORDER ;;;;;;;;;;;;;;;;;;;;;

	mov   dx,[_TopClip]           ; Compare u.l. Y coord with Top
	sub   dx,[Y]                  ; clipping border
	jle   @@NotTopClip            ; jump if VBM not clipped from above
	cmp   dx,bx
	jnl   @@NotVisible            ; jump if VBM is completely obscured
	mov   [TopRow],dx
	sub   bx,dx
	add   [Y],dx
	jmp   short @@VertClipDone

	;;;; EXIT FOR COMPLETELY OBSCURED P.B.M's ;;;;;;;;;;;;;;;;;;;;;;

@@NotVisible:
	mov   ax,1
	pop   ds                          ; restore data segment
	pop   di                          ; restore registers
	pop   si
	mov   sp,bp                       ; dealloc local variables
	pop   bp
	ret

	;;;;; CLIP PROCESSING FOR BOTTOM CLIP BORDER ;;;;;;;;;;;;;;;;;;;

@@NotTopClip:
	mov   dx,[_BottomClip]
	sub   dx,[Y]
	js    @@NotVisible
	mov   [TopRow],0
	cmp   dx,bx
	jg    @@VertClipDone
	inc   dx
	mov   bx,dx

@@VertClipDone:

	;;;;; CLIP PROCESSING FOR LEFT CLIP BORDER ;;;;;;;;;;;;;;;;;;;

	mov   dx,[_LeftClip]
	sub   dx,di
	jle   @@NotLeftClip
	cmp   dx,ax
	jnl   @@NotVisible

	add   di,dx
	mov   [LeftSkip],dx
	mov   [DataInc],dx
	sub   ax,dx
	mov   [CType],1
	jmp   short @@HorizClipDone

	;;;;; CLIP PROCESSING FOR RIGHT CLIP BORDER ;;;;;;;;;;;;;;;;;;;

@@NotLeftClip:
	mov   dx,[_RightClip]
	sub   dx,di
	js    @@NotVisible
	mov   [LeftSkip],0
	mov   [DataInc],0
	cmp   dx,ax
        jge   @@HorizClipDone       ; was jg
	inc   dx
	sub   ax,dx
	mov   [DataInc],ax
	mov   ax,dx
	mov   [CType],-1

@@HorizClipDone:



	mov   [Width],ax                  ; Save width and height of clipped
	mov   [Height],bx                 ;  image

	add   ax,[DataInc]                ; AX = original width of image
	mul   [TopRow]                    ; Calculate bytes in clipped top
	add   si,ax		          ;  rows
	add   si,2                        ; Skip dimension bytes in source
	add   si,[LeftSkip]               ; Skip pixels in front of row that
					  ;  are clipped

	mov   bx,[_ScrnLogicalByteWidth]  ; Set BX to Logical Screen Width
	mov   dx,bx                       ; BX - Width of image = No. bytes
	sub   dx,[Width]                  ;  to first byte of next screen
	mov   [LineInc],dx                ;  row.

	mov   ax,[Y]                      ; Calculate screen start row
	mul   bx                          ;  then adding screen offset
	add   di,ax
	add   di,[ScrnOffs]
	mov   ax,es                       ; copy ES to DS
	mov   ds,ax
	mov   ax,SCREEN_SEG               ; Point ES to VGA segment
	mov   es,ax



	and   cx,3
	mov   ah,11h                      ; Set up initial plane mask
	shl   ah,cl

	mov   dx,SC_INDEX                 ; Prepare VGA for cpu to video writes
	mov   al,MAP_MASK
	out   dx,al
	inc   dx
	mov   [Plane],4                   ; Set plane counter to 4
	mov   bh,byte ptr [Width]         ; set bh to width for fast looping
@@PlaneLoop:
	push  di 			  ; Save bitmap's start dest. offset
	push  si
	mov   bl,byte ptr [Height]        ; Reset row counter (BL)
	mov   al,ah
	out   dx,al                       ; set vga write plane
@@RowLoop:
	mov   cl,bh                       ; Reset Column counter cl
	shr   cl,1
	rep   movsw                       ; Copy a complete row
	adc   cl,0
	rep   movsb
	add   si,[DataInc]                ; Move to next source row
	add   di,[LineInc]                ; Move to next screen row
	dec   bl                          ; decrement row counter
	jnz   @@RowLoop                   ; Jump if more rows left
	pop   si                          ; Restore SI and set to offset of
	add   si,[PlaneInc]               ; first vis pixel in next plane data
	pop   di                          ; Restore bitmaps start dest byte
	rol   ah,1		          ; Shift mask for next plane

	; Plane Transition (A HACK but it works!)

	jnb   @@Nocarry                   ; Jump if not plane transition
	mov   bl,ah                       ; Save Plane Mask
	mov   ax,[CType]                  ; set AX to clip type inc variable
	add   bh,al                       ; Update advancing variables
	sub   [DataInc],ax                ;
	sub   [LineInc],ax                ;
	cmp   al,0                        ; What type of clip do we have
	mov   ah,bl                       ;   restore Plane mask
	jg    @@RightAdvance              ; jump on a right clip!
	inc   di                          ; otherwise increment DI
	jmp   @@Nocarry
@@RightAdvance:
	dec si
@@Nocarry:
	dec   [Plane]                     ; Decrement plane counter
	jnz   @@PlaneLoop                 ; Jump if more planes left

	xor   ax,ax
	pop   ds                          ; restore data segment
	pop   di                          ; restore registers
	pop   si
	mov   sp,bp                       ; dealloc local variables
	pop   bp
	ret
_x_put_pbm_clipxy  endp

	end



