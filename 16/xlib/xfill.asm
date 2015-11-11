;-----------------------------------------------------------------------
; MODULE XFILL
;
; Point functions all MODE X 256 Color resolutions
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

  This code is my interpretation of a simple "C" flood filling algorithm
  titled:

  * A Seed Fill Algorithm
  * by Paul Heckbert
  * from "Graphics Gems", Academic Press, 1990

  The original C source is readily available at numerous internet archive
  sites.

  Its been modified and optimized for tweaked 13h modes (Mode X derrivatives).
  The most fundamental change is that it fills a column at a time rather
  than a row at a time to minimize the number of plane setting "out"s.
  And of course the border fill variant was a logical and useful further
  modification.

  Both functions return the number of pixels filled..

  WARNING: These fill functions make heavy use of the stack and no stack
    checking is performed, so caution is advised.

$


include xlib.inc
include xfill.inc

	.code

_x_flood_fill  proc
	ARG X:word,Y:word,PgOfs:word,Color:word
	LOCAL len:word,y1:word,y2:word,deltax:word,floodval:word,\
	      stackptr:word,FillCount:word=STK
	push bp
	mov  bp,sp
	sub  sp,STK
	mov  [FillCount],0
	push di si
	mov  si,[Y]
	mov  ax,[_ScrnLogicalByteWidth]
	mul  si                   ;offset of pixel's scan line in page
	mov  di,[X]
	mov  bx,di
	shr  di,2                 ;X/4 = offset of pixel in scan line
	add  di,ax                ;offset of pixel in page
	add  di,[PgOfs]           ;offset of pixel in display memory
	mov  ax,SCREEN_SEG
	mov  es,ax                ;point ES:DI to the pixel's address

	;---- Select read plane ------

	mov  ah,bl
	and  ah,011b              ;AH = pixel's plane
	mov  al,READ_MAP          ;AL = index in GC of the Read Map reg
	mov  dx,GC_INDEX          ;set the Read Map to read the pixel's
	out  dx,ax                ; plane

	mov  al,es:[di]           ;read the pixel's color
	cmp  al,byte ptr Color    ;Is dest pixel the same color as the flood?
	je   @@Done2              ; if it is abort.

	mov  cx,_LeftClip         ; Is the dest. pixel out of the clipping window?
	sal  cx,2                 ;  if not abort.
	cmp  bx,cx
	jl   @@Done2

	mov  cx,_RightClip
	sal  cx,2
	cmp  bx,cx
	jg   @@Done2

	mov  floodval,ax           ; store the color to flood

	;-- Push fill segment ---

	push bx         ; X
	push si         ; Y
	push si         ; Y
	mov  cx,1       ; deltaX  (either 1 or -1 indicating direction)
	push cx
	mov  stackptr,1

	mov  deltax,-1  ; Initialize first column scan
	mov  y1,si      ;   then bypass some of the preliminary crap in
	mov  y2,si      ;   the main fill loop
	jmp  short @@entry

@@Done2:mov  ax,[FillCount]
	pop  si di
	mov  sp,bp
	pop  bp
	ret

@@NextScanCol:
	dec  stackptr
	js   @@Done2

@@WhileLoop:
	pop  cx           ; get fill segment from stack
	mov  deltax,cx    ; ie deltaX, Y1, Y2, X
	pop  ax
	mov  y2,ax
	pop  si
	mov  y1,si
	pop  bx

	sub  ax,si           ; Acculmulate number of filled pixels
	jns  @@PositiveY
	neg  ax
@@PositiveY:
	add  FillCount,ax


	add  bx,cx          ; move to new column according to deltaX

	mov  ax,bx          ; Make sure the column is within the clipping
	sar  ax,2           ; rectangle
	cmp  ax,_LeftClip
	jl   @@NextScanCol

	cmp  ax,_RightClip
	jg   @@NextScanCol


	;---- Select read plane ------

	mov  ah,bl
	and  ah,011b              ;AH = pixel's plane
	mov  al,READ_MAP          ;AL = index in GC of the Read Map reg
	mov  dx,GC_INDEX          ;set the Read Map to read the pixel's
	out  dx,ax                ; plane

@@entry:

	;---- Select write plane ------

	mov  cl,bl
	and  cl,011b              ;CL = pixel's plane
	mov  ax,0100h + MAP_MASK  ;AL = index in SC of Map Mask reg
	shl  ah,cl                ;set only the bit for the pixel's
				  ; plane to 1
	mov  dx,SC_INDEX          ;set the Map Mask to enable only the
	out  dx,ax                ; pixel's plane

	mov  ax,_ScrnLogicalByteWidth     ; store logical width in CX
	mov  cx,ax                        ; get offset of scan row
	mul  si                           ; set ES:DI ->
	mov  di,bx                        ; address of pixel at x,y1
	shr  di,2
	add  di,ax
	add  di,PgOfs          ;ES:DI->first pixel of column segment to fill
	mov  dx,di             ; save y1 offset for after upward fill

	mov  al,byte ptr Color
	mov  ah,byte ptr floodval

@@FillColUpward:
	cmp  si,_TopClip           ; Dont fill beyond clip boundaries
	jl   @@UpwardFillDone

	cmp  es:[di],ah            ; if flood pixel color then replace
	jne  @@UpwardFillDone      ; with new color otherwise column is done

	mov  es:[di],al
	sub  di,cx
	dec  si
	jmp  short @@FillColUpward

@@UpwardFillDone:
	cmp  si,y1
	jge  @@Skip

	inc  si
	mov  len,si

	cmp  si,y1
	jge  @@AtColumnTop

	push bx     ;  queue an upward leak check
	push si
	mov  ax,y1
	dec  ax
	push ax
	mov  ax,deltax
	neg  ax
	push ax
	inc  stackptr

@@AtColumnTop:
	mov  si,y1
	mov  di,dx
	add  di,cx
	inc  si


@@ColumnLoop:
	mov  ah,byte ptr floodval
	mov  al,byte ptr Color

@@DownwardFill:
	cmp  si,_BottomClip
	jg   @@DownwardFillDone
	cmp  es:[di],ah
	jne  @@DownwardFillDone
	mov  es:[di],al
	add  di,cx
	inc  si
	jmp  short @@DownwardFill

@@DownwardFillDone:

	push bx      ; queue an upward leak check
	mov  ax,len
	push ax
	mov  ax,si
	dec  ax
	push ax
	mov  ax,deltax
	push ax
	inc  stackptr

	mov  ax,y2
	inc  ax
	cmp  si,ax
	jle  @@Skip

	push bx       ;  queue a downward leak check
	push ax
	mov  ax,si
	dec  ax
	push ax
	mov  ax,deltax
	neg  ax
	push ax
	inc  stackptr

@@Skip:
	mov  ah,byte ptr floodval
	mov  dx,y2

@@Backtrack:
	add  di,cx
	inc  si
	cmp  si,dx
	jg   @@BacktrackDone

	cmp  byte ptr es:[di],ah
	jne  @@Backtrack

@@BacktrackDone:
	mov  len,si
	cmp  si,dx
	jle  @@ColumnLoop

	dec  stackptr
	js   @@Done
	jmp  @@WhileLoop
@@Done:
	mov  ax,[FillCount]
	pop  si di
	mov  sp,bp
	pop  bp
	ret
_x_flood_fill   endp


_x_boundary_fill  proc
	ARG X:word,Y:word,PgOfs:word,BoundaryColor:word,Color:word
	LOCAL len:word,y1:word,y2:word,deltax:word,y1_offs:word,\
	      stackptr:word,FillCount:word=STK
	push bp
	mov  bp,sp
	sub  sp,STK
	mov  [FillCount],0
	push di si
	mov  si,[Y]
	mov  ax,[_ScrnLogicalByteWidth]
	mul  si                  ;offset of pixel's scan line in page
	mov  di,[X]
	mov  bx,di
	shr  di,2                 ;X/4 = offset of pixel in scan line
	add  di,ax                ;offset of pixel in page
	add  di,[PgOfs]           ;offset of pixel in display memory
	mov  ax,SCREEN_SEG
	mov  es,ax                ;point ES:DI to the pixel's address

	;---- Select read plane ------

	mov  ah,bl
	and  ah,011b              ;AH = pixel's plane
	mov  al,READ_MAP          ;AL = index in GC of the Read Map reg
	mov  dx,GC_INDEX          ;set the Read Map to read the pixel's
	out  dx,ax                ; plane

	mov  al,es:[di]           ;read the pixel's color
	cmp  al,byte ptr Color    ;Is dest pixel the same color as the flood?
	je   @@Done2

	cmp  al,byte ptr BoundaryColor ;Is dest pixel the same color
	je   @@Done2                   ; as the boundary color?


	mov  cx,_LeftClip         ; Is the dest. pixel out of the clipping window?
	sal  cx,2
	cmp  bx,cx
	jl   @@Done2

	mov  cx,_RightClip
	sal  cx,2
	cmp  bx,cx
	jg   @@Done2

	push bx      ; X
	push si      ; Y
	push si      ; Y
	mov  cx,1    ; DX
	push cx
	mov  stackptr,1
	mov  al,byte ptr BoundaryColor
	mov  byte ptr [Color+1],al

	mov  deltax,-1
	mov  y1,si
	mov  y2,si
	jmp  short @@entry

@@Done2:mov  ax,[FillCount]
	pop  si di
	mov  sp,bp
	pop  bp
	ret

@@NextScanCol:
	dec  stackptr
	js   @@Done2

@@WhileLoop:
	pop  cx
	mov  deltax,cx
	pop  ax
	mov  y2,ax
	pop  si
	mov  y1,si
	pop  bx
	add  bx,cx   ; bx = X

	sub  ax,si           ; Acculmulate number of filled pixels
	jns  @@PositiveY
	neg  ax
@@PositiveY:
	add  FillCount,ax



	mov  ax,bx          ; Make sure the column is within the clipping
	sar  ax,2           ; rectangle
	cmp  ax,_LeftClip
	jl   @@NextScanCol
	cmp  ax,_RightClip
	jg   @@NextScanCol


	;---- Select read plane ------

	mov  ah,bl
	and  ah,011b              ;AH = pixel's plane
	mov  al,READ_MAP          ;AL = index in GC of the Read Map reg
	mov  dx,GC_INDEX          ;set the Read Map to read the pixel's
	out  dx,ax                ; plane

@@entry:

	;---- Select write plane ------

	mov  cl,bl
	and  cl,011b              ;CL = pixel's plane
	mov  ax,0100h + MAP_MASK  ;AL = index in SC of Map Mask reg
	shl  ah,cl                ;set only the bit for the pixel's
				  ; plane to 1
	mov  dx,SC_INDEX          ;set the Map Mask to enable only the
	out  dx,ax                ; pixel's plane

	mov  ax,_ScrnLogicalByteWidth     ; store logical width in CX
	mov  cx,ax                        ; get offset of scan row
	mul  si                           ; set ES:DI ->
	mov  di,bx                        ; address of pixel at x,y1
	shr  di,2
	add  di,ax
	add  di,PgOfs
	mov  y1_offs,di                   ; save y1 offset for after upward fill

	mov   ax,Color                    ; al = Color ah = BoundaryColor

@@FillColUpward:
	cmp  si,_TopClip                  ; Dont fill beyond clip boundaries
	jl   @@UpwardFillDone

	mov  dl,es:[di]
	cmp  dl,ah
	je   @@UpwardFillDone

	cmp  dl,al
	je   @@UpwardFillDone

	mov  es:[di],al
	sub  di,cx
	dec  si
	jmp  short @@FillColUpward

@@UpwardFillDone:
	cmp  si,y1
	jge  @@Skip

	inc  si
	mov  len,si

	cmp  si,y1
	jge  @@AtColumnTop

	push bx     ;  queue an upward leak check
	push si
	mov  ax,y1
	dec  ax
	push ax
	mov  ax,deltax
	neg  ax
	push ax
	inc  stackptr

@@AtColumnTop:
	mov  si,y1
	mov  di,y1_offs
	add  di,cx
	inc  si


@@ColumnLoop:
	mov   ax,Color           ; al = Color ah = BoundaryColor

@@DownwardFill:
	cmp  si,_BottomClip
	jg   @@DownwardFillDone

	cmp  es:[di],ah
	je   @@DownwardFillDone

	cmp  es:[di],al
	je   @@DownwardFillDone

	mov  es:[di],al
	add  di,cx
	inc  si
	jmp  short @@DownwardFill

@@DownwardFillDone:

	push bx      ; queue an upward leak check
	mov  ax,len
	push ax
	mov  ax,si
	dec  ax
	push ax
	mov  ax,deltax
	push ax
	inc  stackptr

	mov  ax,y2
	inc  ax
	cmp  si,ax
	jle  @@Skip

	push bx       ;  queue a downward leak check
	push ax
	mov  ax,si
	dec  ax
	push ax
	mov  ax,deltax
	neg  ax
	push ax
	inc  stackptr

@@Skip:
	mov  ax,Color                ; al = Color ah = BoundaryColor

@@Backtrack:
	add  di,cx
	inc  si
	cmp  si,y2
	jg   @@BacktrackDone

	mov  dl,byte ptr es:[di]
	cmp  dl,al
	je   @@Backtrack

	cmp  dl,ah
	je   @@Backtrack

@@BacktrackDone:
	mov  len,si
	cmp  si,y2
	jle  @@ColumnLoop

	dec  stackptr
	js   @@Done
	jmp  @@WhileLoop
@@Done:
	mov  ax,[FillCount]
	pop  si di
	mov  sp,bp
	pop  bp
	ret
_x_boundary_fill   endp

end
