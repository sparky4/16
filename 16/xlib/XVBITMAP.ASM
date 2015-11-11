;-----------------------------------------------------------------------
; MODULE XVBITMAP
;
; Video Bitmap functions - Video Ram <-> Video Ram
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

The XVBITMAP module implements yet another type of bitmap to complement
planar and compiled bitmaps, VRAM based bitmaps. If a 4 cylinder car is
analagous to planar bitmaps, that is thrifty on memory consumption but low
performance and and a V8 is analagous to Compiled bitmaps, memory guzzlers
that really fly, then VRAM based bitmaps are the 6 cylinder modest performers
with acceptable memory consumption.

To summarise their selling points, VBM's are moderately fast with fair memory
consumption, and unlike compiled bitmaps, can be clipped. The disadvantages
are that they are limited by the amount of free video ram and have a complex
structure.

The VRAM bitmap format is rather complex consisting of components stored in
video ram and components in system ram working together. This complexity
necessitates the existence of a creation function "x_make_vbm" which takes
an input linear bitmap and generates the equivalent VBM (VRAM Bit Map).

VBM structure:

      WORD  0   Size          Total size of this VBM structure in bytes
      WORD  1   ImageWidth    Width in bytes of the image (for all alignments)
      WORD  2   ImageHeight   Height in scan lines of the image

      WORD  3 Alignment 0  ImagePtr   Offset in VidRAM of this aligned image
   +--WORD  4              MaskPtr    Offset (within this structure's DS) of
   |   .			       alignment masks
   |   .
   |   .
   |  WORD  9 Alignment 3  ImagePtr   Offset in VidRAM of this aligned image
  +|--WORD 10              MaskPtr    Offset (within this structure's DS) of
  ||   	        		       alignment masks
  ||
  |+->BYTE 21 (WORD 11)                -------+-- Image masks for alignment 0
  |   .                                       |
  |   .                                       |
  |   BYTE  21 + ImageWidth*ImageHeight  -----+
  |
  |   .
  |   . (similaly for alignments 1 - 2 )
  |   .
  |
  +-->BYTE  21 + 3*ImageWidth*ImageHeight + 1-+-- Image masks for alignment 3
      .                                       |
      .                                       |
      BYTE  21 + 4*(ImageWidth*ImageHeight) --+

      .
      .
      << Similarly for alignments 2 and 3 >>
      .
      .
      BYTE 21 + 4*(ImageWidth*ImageHeight)
  -------------

  (And dont forget the corresponding data in video ram)

$


include xlib.inc
include xvbitmap.inc

VBM_info_struc struc
  Size        dw ?
  ImageWidth  dw ?
  ImageHeight dw ?
;  AlignData   dw ?
VBM_info_struc ends

AlignData equ 6

VBM_alignment_struc struc
  ImagePtr    dw ?
  MaskPtr     dw ?
VBM_alignment_struc ends


	.code


;----------------------------------------------------------------------
; x_store_vbm_image
;
;  Store the linear bitmap in video RAM using the specified alignment and
;  start address. Returns number video ram bytes used.
;
;  THIS FUNCTION IS FOR USE BY x_make_masked_vbm
;
; Prototype:
;
;  x_store_vbm_image(unsigned int vramoffs, unsigned int Align,
;		 char far *lbm);
;
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
_x_store_vbm_image  proc
	ARG VramOffs:word,Align:word,LBitmap:dword
	LOCAL BMWidth:byte=LocalStk
	push  bp
        mov   bp,sp
	sub   sp,LocalStk                 ; Create space for local variables
        push  si
        push  di
	push  ds
	cld

	mov   ax,SCREEN_SEG               ; Point ES to screen segment
	mov   es,ax
	mov   di,[VramOffs]               ; Point ES:DI to VRAM dest start
	mov   bx,[Align]                  ; Set BL to first pixel plane align
        and   bl,03h

	lds   si,[LBitmap]                ; DS:SI -> source linear Bitmap
        lodsw                             ; Al = B.M. width (bytes) AH = B.M.
	mov   bh,ah                       ; Save source bitmap dimensions
	mov   [BMWidth],al                ;

	mov   dx,SC_INDEX                 ; Initialize Map Mask for plane
	mov   al,MAP_MASK                 ; selection
        out   dx,al
	inc   dx
	xor   ch,ch                       ; clear CH
@@RowLoop:
	mov   cl,bl                       ; Set initial plane for current
	mov   ah,11h                      ; allignment
	shl   ah,cl

	mov   cl,[BMWidth]                ; Initialize column counter
@@ColLoop:
	mov   al,ah
        out   dx,al                       ; set vga write plane
	lodsb                             ; load next LBM pixel
	mov   es:[di],al                  ; store it in Video Ram
	shl   ah,1                        ; rotate plane mask
	jnb   @@NoAddrIncr                ; Time to increment dest address ?
	inc   di                          ; Yes: increment addr and reset
	mov   ah,11h                      ;  plane mask to plane 0
@@NoAddrIncr:
	loop  @@ColLoop                   ; Loop to next pixel column
	cmp   ah,11h
;	je    @@skip
	inc   di  			  ; Increment dest addr
;@@skip:
	dec   bh                          ; Decrement row counter
	jnz   @@RowLoop                   ; Jump if more rows to go
	mov   ax,di                       ; calculate video RAM consumed and
	sub   ax,[VramOffs]               ;   return value

        pop   ds                          ; restore data segment
	pop   di                          ; restore registers
        pop   si
        mov   sp,bp                       ; dealloc local variables
	pop   bp
        ret
_x_store_vbm_image  endp


_x_put_masked_vbm  proc
	ARG X:word,Y:word,ScrnOffs:word,SrcVBM:dword
	LOCAL VBMWidth:word,VBMHeight:word,NextLineIncr:word=LocalStk
	push  bp
        mov   bp,sp
	sub   sp,LocalStk                 ; Create space for local variables
        push  si
        push  di
	push  ds
	cld

	mov   ax,SCREEN_SEG               ; Point es to VGA segment
        mov   es,ax
        mov   ax,[Y]                      ; Calculate dest screen row
	mov   cx,[_ScrnLogicalByteWidth]  ;  by mult. dest Y coord by Screen
	mul   cx                          ;  width then adding screen offset

	mov   di,[ScrnOffs]               ;  store result in DI
        add   di,ax
	mov   si,[X]                      ; Load X coord into CX and make a
	mov   bx,si                       ;  copy in DX
	shr   bx,2                        ; Find starting byte in dest row
	add   di,bx                       ;  add to DI giving screen offset of
                                          ;  first pixel's byte

	and   si,3                        ; get pixel alignment in si

	lds   bx,[SrcVBM]                 ; DS:BX -> VBM data structure
	shl   si,2                        ; si = offset of data  for curr
					  ; alignment

	mov ax,word ptr [bx+ImageHeight]  ; Get image height
	mov   [VBMHeight],ax
	mov ax,word ptr [bx+ImageWidth]   ; Get image width
	mov   [VBMWidth],ax

	sub   cx,ax                       ; NextLineIncr = bytes to the begin.
	mov   [NextLineIncr],cx           ;  of bitmaps next row on screen
	mov   dx,[bx+MaskPtr+AlignData+si]  ; DS:SI -> mask data
	mov   bx,[bx+ImagePtr+AlignData+si] ; ES:BX -> source video bitmap
	mov   si,dx

	mov   dx,GC_INDEX                 ; Set bit mask for all bits from
	mov   ax,BIT_MASK                 ; VGA latches and none from CPU
	out   dx,ax

	mov   dx,SC_INDEX                 ; Point SC register to map mask
	mov   al,MAP_MASK                 ; in preperation for masking data
	out   dx,al
	inc   dx                          ; Point dx to SC data register
	mov   ah,byte ptr [VBMHeight]     ; AH = Scanline loop counter

@@RowLoop:
	mov   cx,[VBMWidth]               ; Width in bytes across

@@ColumnLoop:
	lodsb
	out   dx,al
	mov   al,es:[bx]                  ; load latches from source bitmap
	stosb                             ; store latches to dest. bitmap
	inc   bx
	loop  @@ColumnLoop

	add   di,[NextLineIncr]           ; point to start of next dest row
	dec   ah                          ; decrement scan line counter
	jnz   @@RowLoop                   ; jump if more scanlines left

	mov   dx,GC_INDEX+1               ; Restore bitmask to the default -
	mov   al,0ffh                     ;  all data from cpu
	out   dx,al

        pop   ds                          ; restore data segment
        pop   di                          ; restore registers
        pop   si
        mov   sp,bp                       ; dealloc local variables
        pop   bp
        ret
_x_put_masked_vbm  endp


_x_put_masked_vbm_clipx  proc
ARG X:word,Y:word,ScrnOffs:word,SrcVBM:dword
LOCAL DataInc,LeftSkip,VBMWidth,VBMHeight,NextLineIncr:word=LocalStk
	push  bp
        mov   bp,sp
	sub   sp,LocalStk             ; Create space for local variables
        push  si
        push  di
	push  ds
	cld

	mov   di,[X]                  ; load X coord int DI and make a
	mov   si,di                   ;  copy in SI
	sar   di,2                    ; Find Byte offset of X coord

	and   si,3                    ; Calculate pixels plane alignment
	shl   si,2                    ; Prepare to lookup mask & data
	les   bx,[SrcVBM]             ; ES:BX -> begining of VBM data

	mov   cx,es:[bx+ImageWidth]   ; Get image width and save in CX



	;;;;; CLIP PROCESSING FOR LEFT CLIP BORDER ;;;;;;;;;;;;;;;;;;;

	mov   dx,[_LeftClip]
	sub   dx,di
	jle   @@NotLeftClip
	cmp   dx,cx
	jnl   @@NotVisible
	add   di,dx
	mov   [LeftSkip],dx
	mov   [DataInc],dx
	sub   cx,dx
	jmp   short @@HorizClipDone

        ;;;; EXIT FOR COMPLETELY OBSCURED V.B.M's ;;;;;;;;;;;;;;;;;;;;;;

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
	cmp   dx,cx
	jge   @@HorizClipDone
	inc   dx
	sub   cx,dx
	mov   [DataInc],cx
	mov   cx,dx

@@HorizClipDone:


	add   di,[ScrnOffs]           ; Add the current page offset
	mov   [VBMWidth],cx
	mov   ax,es:[bx+ImageHeight]  ; Get image height and save in AX
	mov   [VBMHeight],ax


	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	mov   ax,[Y]                      ; Calculate dest screen row
	mov   cx,[_ScrnLogicalByteWidth]  ;  by mult. dest Y coord by Screen
	mul   cx                          ;  width then adding screen offset
	add   di,ax                       ; Add Dest Screen Row to di
	sub   cx,[VBMWidth]
	mov   [NextLineIncr],cx

	mov   ax,es                       ; copy ES to DS
	mov   ds,ax
	mov   ax,SCREEN_SEG               ; Point es to VGA segment
	mov   es,ax

	mov   ax,[bx+MaskPtr+AlignData+si]  ; DS:SI -> mask data
	mov   bx,[bx+ImagePtr+AlignData+si] ; ES:BX -> source video bitmap
	mov   si,ax

	mov   ax,[LeftSkip]               ; Skip data/mask bytes in
	add   bx,ax                       ; each row that have been clipped
	add   si,ax                       ; by the L.H.S border


	mov   dx,GC_INDEX                 ; Set bit mask for all bits from
	mov   ax,BIT_MASK                 ; VGA latches and none from CPU
	out   dx,ax
	mov   dx,SC_INDEX                 ; Point SC register to map mask
	mov   al,MAP_MASK                 ; in preperation for masking data
	out   dx,al
	inc   dx                          ; Point dx to SC data register
	mov   ah,byte ptr [VBMHeight]     ; AH = Scanline loop counter

@@RowLoop:
	mov   cx,[VBMWidth]               ; Width in bytes across

@@ColumnLoop:
	lodsb
	out   dx,al
	mov   al,es:[bx]                  ; load latches from source bitmap
	stosb                             ; store latches to dest. bitmap
	inc   bx
	loop  @@ColumnLoop
	add   bx,[DataInc]
	add   si,[DataInc]
	add   di,[NextLineIncr]           ; point to start of next dest row
	dec   byte ptr ah                 ; decrement scan line counter
	jnz   @@RowLoop                   ; jump if more scanlines left

	mov   dx,GC_INDEX+1               ; Restore bitmask to the default -
	mov   al,0ffh                     ;  all data from cpu
	out   dx,al
	xor   ax,ax
	pop   ds                          ; restore data segment
	pop   di                          ; restore registers
	pop   si
	mov   sp,bp                       ; dealloc local variables
	pop   bp
	ret
_x_put_masked_vbm_clipx  endp


_x_put_masked_vbm_clipy  proc
ARG X:word,Y:word,ScrnOffs:word,SrcVBM:dword
LOCAL VBMWidth,VBMHeight,TopRow,NextLineIncr:word=LocalStk
	push  bp
	mov   bp,sp
	sub   sp,LocalStk                 ; Create space for local variables
	push  si
	push  di
	push  ds
	cld

	mov   di,[X]                  ; load X coord int DI and make a
	mov   si,di                   ;  copy in SI


	and   si,3                    ; Calculate pixels plane alignment
	shl   si,2                    ; Prepare to lookup mask & data
	les   bx,[SrcVBM]             ; ES:BX -> begining of VBM data


	mov   ax,es:[bx+ImageHeight]  ; Get image height and save in AX



	;;;;; CLIP PROCESSING FOR TOP CLIP BORDER ;;;;;;;;;;;;;;;;;;;;;

	mov   dx,[_TopClip]           ; Compare u.l. Y coord with Top
	sub   dx,[Y]                  ; clipping border
	jle   @@NotTopClip            ; jump if VBM not clipped from above
	cmp   dx,ax
	jnl   @@NotVisible            ; jump if VBM is completely obscured
	mov   [TopRow],dx
	sub   ax,dx
	add   [Y],dx
	jmp   short @@VertClipDone

	;;;; EXIT FOR COMPLETELY OBSCURED V.B.M's ;;;;;;;;;;;;;;;;;;;;;;

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
	cmp   dx,ax
	jg    @@VertClipDone
	inc   dx
	mov   ax,dx

@@VertClipDone:


	shr   di,2                    ; Find Byte offset of X coord
	add   di,[ScrnOffs]           ; Add the current page offset
	mov   cx,es:[bx+ImageWidth]   ; Get image width and save in CX
	mov   [VBMWidth],cx
	mov   [VBMHeight],ax

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	mov   ax,[Y]                      ; Calculate dest screen row
	mov   cx,[_ScrnLogicalByteWidth]  ;  by mult. dest Y coord by Screen
	mul   cx                          ;  width then adding screen offset
	add   di,ax                       ; Add Dest Screen Row to di
	sub   cx,[VBMWidth]
	mov   [NextLineIncr],cx

	mov   ax,es                       ; copy ES to DS
	mov   ds,ax
	mov   ax,SCREEN_SEG               ; Point es to VGA segment
	mov   es,ax

	mov   ax,[bx+MaskPtr+AlignData+si]  ; DS:SI -> mask data
	mov   bx,[bx+ImagePtr+AlignData+si] ; ES:BX -> source video bitmap
	mov   si,ax



	mov   ax,[VBMWidth]               ; Increment DS:BX and DS:SI to
	mul   [TopRow]                    ;  skip image/mask data that has
	add   bx,ax                       ;  been clipped by the top border
	add   si,ax


	mov   dx,GC_INDEX                 ; Set bit mask for all bits from
	mov   ax,BIT_MASK                 ; VGA latches and none from CPU
	out   dx,ax
	mov   dx,SC_INDEX                 ; Point SC register to map mask
	mov   al,MAP_MASK                 ; in preperation for masking data
	out   dx,al
	inc   dx                          ; Point dx to SC data register
	mov   ah,byte ptr [VBMHeight]     ; AH = Scanline loop counter

@@RowLoop:
	mov   cx,[VBMWidth]               ; Width in bytes across

@@ColumnLoop:
	lodsb
	out   dx,al
	mov   al,es:[bx]                  ; load latches from source bitmap
	stosb                             ; store latches to dest. bitmap
	inc   bx
	loop  @@ColumnLoop
	add   di,[NextLineIncr]           ; point to start of next dest row
	dec   byte ptr ah                 ; decrement scan line counter
	jnz   @@RowLoop                   ; jump if more scanlines left

	mov   dx,GC_INDEX+1               ; Restore bitmask to the default -
	mov   al,0ffh                     ;  all data from cpu
	out   dx,al

	xor   ax,ax
	pop   ds                          ; restore data segment
	pop   di                          ; restore registers
	pop   si
	mov   sp,bp                       ; dealloc local variables
	pop   bp
	ret
_x_put_masked_vbm_clipy  endp

_x_put_masked_vbm_clipxy  proc
ARG X:word,Y:word,ScrnOffs:word,SrcVBM:dword
LOCAL DataInc,LeftSkip,VBMWidth,VBMHeight,TopRow,NextLineIncr:word=LocalStk
	push  bp
	mov   bp,sp
	sub   sp,LocalStk                 ; Create space for local variables
	push  si
	push  di
	push  ds
	cld

	mov   di,[X]                  ; load X coord int DI and make a
	mov   si,di                   ;  copy in SI
	sar   di,2                    ; Find Byte offset of X coord
	and   si,3                    ; Calculate pixels plane alignment
	shl   si,2                    ; Prepare to lookup mask & data
	les   bx,[SrcVBM]             ; ES:BX -> begining of VBM data

	mov   cx,es:[bx+ImageWidth]   ; Get image width and save in CX
	mov   ax,es:[bx+ImageHeight]  ; Get image height and save in AX



	;;;;; CLIP PROCESSING FOR TOP CLIP BORDER ;;;;;;;;;;;;;;;;;;;;;

	mov   dx,[_TopClip]           ; Compare u.l. Y coord with Top
	sub   dx,[Y]                  ; clipping border
	jle   @@NotTopClip            ; jump if VBM not clipped from above
	cmp   dx,ax
	jnl   @@NotVisible            ; jump if VBM is completely obscured
	mov   [TopRow],dx
	sub   ax,dx
	add   [Y],dx
	jmp   short @@VertClipDone

	;;;; EXIT FOR COMPLETELY OBSCURED V.B.M's ;;;;;;;;;;;;;;;;;;;;;;

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
	cmp   dx,ax
	jg    @@VertClipDone
	inc   dx
	mov   ax,dx

@@VertClipDone:

	;;;;; CLIP PROCESSING FOR LEFT CLIP BORDER ;;;;;;;;;;;;;;;;;;;


	mov   dx,[_LeftClip]
	sub   dx,di
	jle   @@NotLeftClip
	cmp   dx,cx
	jnl   @@NotVisible
	add   di,dx
	mov   [LeftSkip],dx
	mov   [DataInc],dx
	sub   cx,dx
	jmp   short @@HorizClipDone

	;;;;; CLIP PROCESSING FOR RIGHT CLIP BORDER ;;;;;;;;;;;;;;;;;;;

@@NotLeftClip:
	mov   dx,[_RightClip]
	sub   dx,di
	js    @@NotVisible
	mov   [LeftSkip],0
	mov   [DataInc],0
	cmp   dx,cx
	jge    @@HorizClipDone
	inc   dx
	sub   cx,dx
	mov   [DataInc],cx
	mov   cx,dx

@@HorizClipDone:

	add   di,[ScrnOffs]           ; Add the current page offset
	mov   [VBMWidth],cx
	mov   [VBMHeight],ax
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	mov   ax,[Y]                      ; Calculate dest screen row
	mov   cx,[_ScrnLogicalByteWidth]  ;  by mult. dest Y coord by Screen
	mul   cx                          ;  width then adding screen offset
	add   di,ax                       ; Add Dest Screen Row to di
	sub   cx,[VBMWidth]
	mov   [NextLineIncr],cx

	mov   ax,es                       ; copy ES to DS
	mov   ds,ax
	mov   ax,SCREEN_SEG               ; Point es to VGA segment
	mov   es,ax

	mov   ax,[bx+MaskPtr+AlignData+si]  ; DS:SI -> mask data
	mov   bx,[bx+ImagePtr+AlignData+si] ; ES:BX -> source video bitmap
	mov   si,ax



	mov   ax,[VBMWidth]               ; Increment DS:BX and DS:SI to
	add   ax,[DataInc]                ;  skip image/mask data that has
	mul   [TopRow]                    ;  been clipped by the top border
	add   ax,[LeftSkip]               ; Skip also data/mask bytes in
	add   bx,ax                       ; each row that have been clipped
	add   si,ax                       ; by the L.H.S border


	mov   dx,GC_INDEX                 ; Set bit mask for all bits from
	mov   ax,BIT_MASK                 ; VGA latches and none from CPU
	out   dx,ax
	mov   dx,SC_INDEX                 ; Point SC register to map mask
	mov   al,MAP_MASK                 ; in preperation for masking data
	out   dx,al
	inc   dx                          ; Point dx to SC data register
	mov   ah,byte ptr [VBMHeight]     ; AH = Scanline loop counter

@@RowLoop:
	mov   cx,[VBMWidth]               ; Width in bytes across

@@ColumnLoop:
	lodsb
	out   dx,al
	mov   al,es:[bx]                  ; load latches from source bitmap
	stosb                             ; store latches to dest. bitmap
	inc   bx
	loop  @@ColumnLoop
	add   bx,[DataInc]
	add   si,[DataInc]
	add   di,[NextLineIncr]           ; point to start of next dest row
	dec   byte ptr ah                 ; decrement scan line counter
	jnz   @@RowLoop                   ; jump if more scanlines left

	mov   dx,GC_INDEX+1               ; Restore bitmask to the default -
	mov   al,0ffh                     ;  all data from cpu
	out   dx,al
	xor   ax,ax
	pop   ds                          ; restore data segment
	pop   di                          ; restore registers
	pop   si
	mov   sp,bp                       ; dealloc local variables
	pop   bp
	ret
_x_put_masked_vbm_clipxy  endp


	end

