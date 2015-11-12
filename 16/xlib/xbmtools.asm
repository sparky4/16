;-----------------------------------------------------------------------
; MODULE XBMTOOLS
;
; Bitmap conversion / manipulation tools
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

  This module implements a set of functions to manipulate both planar
  bitmaps and linear bitmaps (as used by x_compile_bitmap):

  PLANAR BITMAPS

  Planar bitmaps as used by these functions have the following structure:

  BYTE 0                 The bitmap width in bytes (4 pixel groups) range 1..255
  BYTE 1                 The bitmap height in rows range 1..255
  BYTE 2..n1             The plane 0 pixels width*height bytes
  BYTE n1..n2            The plane 1 pixels width*height bytes
  BYTE n2..n3            The plane 2 pixels width*height bytes
  BYTE n3..n4            The plane 3 pixels width*height bytes

  LINEAR BITMAPS

  Linear bitmaps have the following structure:

  BYTE 0                 The bitmap width in pixels  range 1..255
  BYTE 1                 The bitmap height in rows   range 1..255
  BYTE 2..n              The width*height bytes of the bitmap



$

LOCALS
.8086

include model.inc
include xbmtools.inc

	.code



;-----------------------------------------------------------------------
; x_pbm_to_bm
;
; This function converts a bitmap in the planar format to the linear format
; as used by x_compile_bitmap.
;
; WARNING: the source and destination bitmaps must be pre - allocated
;
; NOTE: This function can only convert planar bitmaps that are suitable.
;       If the source planar bitmap's width (per plane) is >= 256/4
;       it cannot be converted. In this situation an error code
;       BM_WIDTH_ERROR. On successful conversion 0 is returned.
;
; C callable as:
;    int x_pbm_to_bm(char far * source_pbm, char far * dest_bm);
;
; Written By Themie Gouthas

proc _x_pbm_to_bm
ARG   src_pbm:dword,dest_bm:dword
	push bp                  ; Preserve caller's stack frame
	mov  bp,sp
	push ds
	push di
	push si

	les  di,[dest_bm]     ; es:di -> destination bitmap
	lds  si,[src_pbm]     ; ds:si -> source planar bitmap
	lodsb                 ; load AL with source pbm pixel width per plane
	mov  bl,al            ; save in CL
	xor  ah,ah            ; convert to word
	shl  ax,2             ; mult by 4 giving source image width
	cmp  ax,255           ; if the result > 255 then we have exceeded
	ja   @@WidthError     ; the max width of linear bm.

	stosb                 ; write do dest_bm

	lodsb                 ; tranfer source pbm height in pixels to
	stosb             ;  dest_bm

	xor  ah,ah            ; convert to word
	mul  bl               ; AX = AX * BL ie. total no. pixels per plane
	mov  dx,di            ; save DI, the pointer to the destination bm
	mov  bl,3             ; set plane loop counter (BL)

@@PlaneLoop:
	mov  cx,ax            ; set CX to total number of pixels per plane

@@PixelLoop:
	movsb                 ; transfer pixel
	add  di,3             ; increment destination to compensate for plane
	loop @@PixelLoop

	inc  dx               ; increment original di for next pixel plane
	mov  di,dx            ; and restore di from incremented original
	dec  bl               ; decrement plane counter
	jns  @@PlaneLoop      ; loop if more planes left
	xor  ax,ax
	jmp  short @@Done
@@WidthError:
	mov  ax,1

@@Done:
	pop  si
	pop  di
	pop  ds
	pop  bp                  ;restore caller's stack frame
	ret
_x_pbm_to_bm endp


;-----------------------------------------------------------------------
; x_bm_to_pbm
;
; This function converts a bitmap in the linear format as used by
; x_compile_bitmap to the planar formap.
;
; WARNING: the source and destination bitmaps must be pre - allocated
;
; NOTE: This function can only convert linear bitmaps that are suitable.
;       If the source linear bitmap's width is not a multiple of 4
;       it cannot be converted. In this situation an error code
;       BM_WIDTH_ERROR. On successful conversion 0 is returned.
;
;
; C callable as:
;    int x_bm_to_pbm(char far * source_pbm, char far * dest_bm);
;
; Written By Themie Gouthas

proc _x_bm_to_pbm
ARG src_bm:dword,dest_pbm:dword
	push bp                  ; Preserve caller's stack frame
	mov  bp,sp
	push ds
	push di
	push si

	les  di,[dest_pbm]       ; es:di -> destination planar bitmap
	lds  si,[src_bm]         ; ds:si -> source bitmap
	lodsb                    ; load AX with source bitmap width
	test al,03h              ; Check that width is a multiple of 4
	jnz  @@WidthIncompatible
	shr  al,2                ; divide by 4 giving width of plane
	stosb                    ; store destination planar bitmap width
	mov  bl,al               ;  and copy to bl
	lodsb
	stosb            ; Transfer source bitmap height to dest pbm
	xor  ah,ah               ; Conver height to word
	mul  bl                  ; calculate the total no. of pixels / plane
	mov  dx,si               ; save source offset
	mov  bl,3

@@PlaneLoop:
	mov  cx,ax            ; set CX to total number of pixels per plane

@@PixelLoop:
	movsb                 ; transfer pixel
	add  si,3             ; increment src offset to compensate for plane
	loop @@PixelLoop

	inc  dx               ; increment original si for next pixel plane
	mov  si,dx            ; and restore si from incremented original
	dec  bl               ; decrement plane counter
	jns  @@PlaneLoop      ; loop if more planes left
	xor  ax,ax
	jmp  short @@Done
@@WidthIncompatible:
	mov  ax,1

@@Done:
	pop  si
	pop  di
	pop  ds
	pop  bp                  ;restore caller's stack frame
	ret
_x_bm_to_pbm endp


	end

