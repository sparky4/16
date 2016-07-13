;-----------------------------------------------------------------------
; MODULE XPOLYGON
;
; Filled Triangle function for all MODE X 256 Color resolutions
;
; Compile with Tasm.
; C callable.
;
; ****** XLIB - Mode X graphics library                ****************
; ******                                               ****************
; ****** Written By Themie Gouthas                     ****************
;
; This module is based on code developed by Steve Dollind for his
; XSPACE game.
; Copyright (C) 1992 Steven Dollins  --  sdollins@uiuc.edu
;
;
; egg@dstos3.dsto.gov.au
; teg@bart.dsto.gov.au
;-----------------------------------------------------------------------
include xlib.inc
include xpolygon.inc

.data
; Plane masks for clipping left and right edges of rectangle.
	LeftClipPlaneMask       db      00fh,00eh,00ch,008h
	RightClipPlaneMask      db      00fh,001h,003h,007h

.code


;-----------------------------------------------------------------------
; void HLineClipR
;
;       Draws a horizontal line from (X1, Y) to (X2, Y).
;       Uses Watcom Parameter passing convention in registers
;
;   X1 in AX
;   X2 in DX
;   Y in CX
;   Color in BX
;   PageOffset in DI
;
; By Themie Gouthas - Adapted from x_fill_rect.
;-----------------------------------------------------------------------
proc _HLineClipR near
	push    di
	cmp     dx,ax                ; if (X2 < X1) then assume no line
	jl      @@Invisible          ;   is visible

	cmp     cx,[_TopClip]        ; if (Y < TopClip) then no line
	jl      @@Invisible

	cmp     cx,[_BottomClip]     ;if (Y > BottomClip) then no line
	jg      @@Invisible

	mov     di,[_RightClip]      ;convert RightClip to pixel coords
	sal     di,2
	cmp     ax,di                ; if (X1 > RightClip) then no line
	jg      @@Invisible

	cmp     dx,di                ; if (X2 > RightClip) then
	jle     @@ClipLeft           ;  X2:=RightClip
	mov     dx,di

@@ClipLeft:
	mov     di,[_LeftClip]       ;convert LeftClip to pixel coords
	sal     di,2
        cmp     dx,di                ; if (X2 < LeftClip) then no line
        jl      @@Invisible

	cmp     ax,di                ;if (X1 > LeftClip) then were ready to plot
	jge     @@DoLine

	mov     ax,di                ;  X1:=LeftClip
        jmp     short @@DoLine

@@Invisible:
	pop     di
	ret

@@DoLine:
        pop     di                      ; di = PageOffset
	xchg    cx,ax                   ; AX = Y,  CX = X1
        mov     si,dx                   ; SI = DX = X2
	mul     [_ScrnLogicalByteWidth]
        mov     dx,si                   ; Reset DX to X1 since mul erases DX
	add     ax,di
	mov     di,cx
	sar     di,2                    ; Convert to bytes
	add     di,ax                   ; DI->First byte

        and     si,03h                  ; look up right edge plane mask
	mov     ah,RightClipPlaneMask[si]
	mov     si,cx                   ; look up left edge plane mask
	and     si,03h
	mov     al,LeftClipPlaneMask[si]

	cmp     dx,cx                   ; No harm in being paranoid..
	jle     @@Invisible2

	xchg    cx,dx                   ;CX=X2, DX=X1
	dec     cx
	and     dx,not 03h
	sub     cx,dx
        js      @@Invisible2
	shr     cx,2
	jnz     @@MasksSet
	and     al,ah
@@MasksSet:
	mov     dl,bl                 ; set BX=Plane Masks, AH=Color
	mov     bx,ax
	mov     ah,dl
	mov     dx,SC_INDEX+1         ;set the Sequence Controller Index to
	mov     al,bl
	out     dx,al
        mov     al,ah
	stosb                         ; Plot left byte
	dec     cx
	js      @@Invisible2
	jz      @@RightEnd


	mov     al,0fh                ; plot middle bytes
	out     dx,al
        mov     al,ah
        shr     cx,1
	rep     stosw
        adc     cx,cx
        rep     stosb



@@RightEnd:
	mov     al,bh              ; Plot right  byte
	out     dx,al
        mov     al,ah
	stosb
@@Invisible2:
	ret
_HLineClipR endp


;-----------------------------------------------------------------------
; void x_triangle( int X0, int Y0, int X1, int Y1,
;                int X2, int Y2, unsigned Color, unsigned PageOffset );
;
;
; Written by S. Dollins

_x_triangle  proc
ARG   X0:word,Y0:word,X1:word,Y1:word,X2:word,Y2:word,Color:word,PageOffset:word
LOCAL DX01:word, DY01:word, DX02:word, DY02:word, DX12:word, DY12:word, \
      DP01:word, DP02:word, DP12:word, XA01:word, XA02:word, XA12:word=STK

	push    bp
	mov     bp,sp
	sub     sp,STK
	push    ds es si di  ; Save es for polygon routine

	mov     ax,X0
	mov     bx,Y0
	mov     cx,X1
	mov     dx,Y1

	cmp     bx,dx   ; Y0,Y1
	jl      tri_Y0lY1
	je      tri_Y0eY1
	xchg    ax,cx   ; X0,X1
	xchg    bx,dx   ; Y0,Y1
tri_Y0lY1:
	cmp     dx,Y2   ; Y1,Y2
	jg      tri_a
	jmp     tri_sorted
tri_a:  xchg    cx,X2   ; X1,X2
	xchg    dx,Y2   ; Y1,Y2
	cmp     bx,dx   ; Y0,Y1
	jge     tri_b
	jmp     tri_sorted
tri_b:  je      tri_bot
	xchg    ax,cx   ; X0,X1
	xchg    bx,dx   ; Y0,Y1
	jmp     tri_sorted
tri_Y0eY1:
	cmp     bx,Y2   ; Y0,Y2
	jl      tri_bot
	jg      tri_c
	jmp     tri_done
tri_c:  xchg    ax,X2   ; X0,X2
	xchg    bx,Y2   ; Y0,Y2
	jmp     tri_sorted

tri_bot:
	cmp     ax,cx   ; X0,X1
	jl      tri_bot_sorted
	jg      tri_bot_a
	jmp     tri_done
tri_bot_a:
	xchg    ax,cx   ; X0,X1
tri_bot_sorted:
	cmp     bx,[_BottomClip]
	jle     tri_bot_y0ok
	jmp     tri_done
tri_bot_y0ok:
	mov     si,Y2
	cmp     si,[_TopClip]
	jge     tri_bot_y2ok
	jmp     tri_done
tri_bot_y2ok:
	mov     X0,ax
	mov     Y0,bx
	mov     X1,cx
	mov     Y1,dx

	mov     bx,Y2   ;    bx <- Y2
	sub     bx,Y0   ;    bx <- Y2 - Y0
	mov     DY02,bx ;  DY02 <- Y2 - Y0
	mov     ax,X2   ;    ax <- X2
	sub     ax,X0   ;    ax <- X2 - X0
	mov     DX02,ax ;  DX02 <- X2 - X0
	mov     cx,ax   ;    cx <- DX02
	cwd             ; dx:ax <- DX02
	idiv    bx      ;    ax <- DX02 / DY02
	cmp     cx,0
	jge     short tri_bot02
	dec     ax      ;    ax <- DX02 / DY02 - 1
tri_bot02:
	mov     XA02,ax ;  XA02 <- DX02 / DY02
	imul    bx      ;    ax <- XA02 * DY02
	sub     cx,ax   ;    cx <- DX02 - XA02 * DY02
	mov     DP02,cx ;  DP02 <- DX02 - XA02 * DY02

	mov     bx,Y2   ;    bx <- Y2
	sub     bx,Y1   ;    bx <- Y2 - Y1
	mov     DY12,bx ;  DY02 <- Y2 - Y1
	mov     ax,X2   ;    ax <- X2
	sub     ax,X1   ;    ax <- X2 - X1
	mov     DX12,ax ;  DX12 <- X2 - X1
	mov     cx,ax   ;    cx <- DX12
	cwd             ; dx:ax <- DX12
	idiv    bx      ;    ax <- DX12 / DY12
	cmp     cx,0
	jge     short tri_bot12
	dec     ax      ;    ax <- DX12 / DY12 - 1
tri_bot12:
	mov     XA12,ax ;  XA12 <- DX12 / DY12
	imul    bx      ;    ax <- XA12 * DY12
	sub     cx,ax   ;    cx <- DX12 - XA12 * DY12
	mov     DP12,cx ;  DP12 <- DX12 - XA12 * DY12

	mov     ax,0    ; PL <- 0
	mov     bx,0    ; PS <- 0
	mov     cx,Y0   ;  Y <- Y0
	mov     si,X0
	mov     di,X1
	dec     di
tri_bot_loop:
	inc     cx      ; Y

	add     ax,DP02 ; PL,DP02
	jle     short tri_bot_shortl
	sub     ax,DY02 ; PL,DY02
	inc     si      ; XL
tri_bot_shortl:
	add     si,XA02 ; XL,XA02

	add     bx,DP12 ; PS,DP12
	jle     short tri_bot_shortr
	sub     bx,DY12 ; PS,DY12
	inc     di      ; XS
tri_bot_shortr:
	add     di,XA12 ; XS,XA12

	push    di      ; XS
	push    si      ; XL
	cmp     cx,Y2   ; Y,Y2
	jl      short tri_bot_loop

	jmp     tri_draw_lines


tri_sorted:
	cmp     bx,[_BottomClip]
	jle     tri_y0ok
	jmp     tri_done
tri_y0ok:
	mov     si,Y2
	cmp     si,[_TopClip]
	jge     tri_y2ok
	jmp     tri_done
tri_y2ok:
	mov     X0,ax
	mov     Y0,bx
	mov     X1,cx
	mov     Y1,dx

	mov     bx,dx   ;    bx <- Y1
	sub     bx,Y0   ;    bx <- Y1 - Y0
	mov     DY01,bx ;  DY01 <- Y1 - Y0
	mov     ax,X1   ;    ax <- X1
	sub     ax,X0   ;    ax <- X1 - X0
	mov     DX01,ax ;  DX01 <- X1 - X0
	mov     cx,ax   ;    cx <- DX01
	cwd             ; dx:ax <- DX01
	idiv    bx      ;    ax <- DX01 / DY01
	cmp     cx,0    ;  DX01 ? 0
	jge     short tri_psl01
	dec     ax      ;    ax <- DX01 / DY01 - 1
tri_psl01:
	mov     XA01,ax ;  XA01 <- DX01 / DY01
	imul    bx      ;    ax <- XA01 * DY01
	sub     cx,ax   ;    cx <- DX01 - XA01 * DY01
	mov     DP01,cx ;  DP01 <- DX01 - XA01 * DY01

	mov     bx,Y2   ;    bx <- Y2
	sub     bx,Y0   ;    bx <- Y2 - Y0
	mov     DY02,bx ;  DY02 <- Y2 - Y0
	mov     ax,X2   ;    ax <- X2
	sub     ax,X0   ;    ax <- X2 - X0
	mov     DX02,ax ;  DX02 <- X2 - X0
	mov     cx,ax   ;    cx <- DX02
	cwd             ; dx:ax <- DX02
	idiv    bx      ;    ax <- DX02 / DY02
	cmp     cx,0
	jge     short tri_psl02
	dec     ax      ;    ax <- DX02 / DY02 - 1
tri_psl02:
	mov     XA02,ax ;  XA02 <- DX02 / DY02
	imul    bx      ;    ax <- XA02 * DY02
	sub     cx,ax   ;    cx <- DX02 - XA02 * DY02
	mov     DP02,cx ;  DP02 <- DX02 - XA02 * DY02

	mov     bx,Y2   ;    bx <- Y2
	sub     bx,Y1   ;    bx <- Y2 - Y1
	jle     short tri_const_computed
	mov     DY12,bx ;  DY12 <- Y2 - Y1
	mov     ax,X2   ;    ax <- X2
	sub     ax,X1   ;    ax <- X2 - X1
	mov     DX12,ax ;  DX12 <- X2 - X1
	mov     cx,ax   ;    cx <- DX12
	cwd             ; dx:ax <- DX12
	idiv    bx      ;    ax <- DX12 / DY12
	cmp     cx,0
	jge     short tri_psl12
	dec     ax      ;    ax <- DX12 / DY12 - 1
tri_psl12:
	mov     XA12,ax ;  XA12 <- DX12 / DY12
	imul    bx      ;    ax <- XA12 * DY12
	sub     cx,ax   ;    cx <- DX12 - XA12 * DY12
	mov     DP12,cx ;  DP12 <- DX12 - XA12 * DY12

tri_const_computed:
	mov     ax,DX01
	imul    word ptr DY02
	mov     bx,ax
	mov     cx,dx   ; DX01 * DY02 in cx:bx

	mov     ax,DX02
	imul    word ptr DY01 ; DX02 * DY01 in dx:ax
	cmp     cx,dx
	jg      tri_pt1rt
	jl      tri_pt1lt
	cmp     bx,ax
	ja      tri_pt1rt
	jb      tri_pt1lt
	jmp     tri_done

;------------------------------------
; Short sides are on the left
;
tri_pt1lt:
	mov     ax,0    ; PL <- 0
	mov     bx,0    ; PS <- 0
	mov     cx,Y0   ;  Y <- Y0
	mov     si,X0
	mov     di,si
	dec     si
tri_lt_loop:
	inc     cx      ; Y

	add     ax,DP02 ; PL,DP02
	jle     short tri_lt_shortl
	sub     ax,DY02 ; PL,DY02
	inc     si      ; XL
tri_lt_shortl:
	add     si,XA02 ; XL,XA02

	add     bx,DP01 ; PS,DP01
	jle     short tri_lt_shortr
	sub     bx,DY01 ; PS,DY01
	inc     di      ; XS
tri_lt_shortr:
	add     di,XA01 ; XS,XA01

	push    si      ; XL
	push    di      ; XS
	cmp     cx,Y1   ; Y,Y1
	jl      short tri_lt_loop

	jmp     short tri_lb_start
tri_lb_loop:
	inc     cx      ; Y

	add     ax,DP02 ; PL,DP02
	jle     short tri_lb_shortl
	sub     ax,DY02 ; PL,DY02
	inc     si      ; XL
tri_lb_shortl:
	add     si,XA02 ; XL,XA02

	add     bx,DP12 ; PS,DP12
	jle     short tri_lb_shortr
	sub     bx,DY12 ; PS,DY12
	inc     di      ; XS
tri_lb_shortr:
	add     di,XA12 ; XS,XA12

	push    si      ; XL
	push    di      ; XS
tri_lb_start:
	cmp     cx,Y2   ; Y,Y2
	jl      tri_lb_loop
	jmp     short tri_draw_lines

;------------------------------------
; short sides are on the right
;
tri_pt1rt:
	mov     ax,0    ; PL <- 0
	mov     bx,0    ; PS <- 0
	mov     cx,Y0   ;  Y <- Y0
	mov     si,X0
	mov     di,si
	dec     di
tri_rt_loop:
	inc     cx      ; Y

	add     ax,DP02 ; PL,DP02
	jle     short tri_rt_shortl
	sub     ax,DY02 ; PL,DY02
	inc     si      ; XL
tri_rt_shortl:
	add     si,XA02 ; XL,XA02

	add     bx,DP01 ; PS,DP01
	jle     short tri_rt_shortr
	sub     bx,DY01 ; PS,DY01
	inc     di      ; XS
tri_rt_shortr:
	add     di,XA01 ; XS,XA01

	push    di      ; XS
	push    si      ; XL
	cmp     cx,Y1   ; Y,Y1
	jl      short tri_rt_loop

	jmp     short tri_rb_start
tri_rb_loop:
	inc     cx      ; Y

	add     ax,DP02 ; PL,DP02
	jle     short tri_rb_shortl
	sub     ax,DY02 ; PL,DY02
	inc     si      ; XL
tri_rb_shortl:
	add     si,XA02 ; XL,XA02

	add     bx,DP12 ; PS,DP12
	jle     short tri_rb_shorts
	sub     bx,DY12 ; PS,DY12
	inc     di      ; XS
tri_rb_shorts:
	add     di,XA12 ; XS,XA12

	push    di      ; XS
	push    si      ; XL
tri_rb_start:
	cmp     cx,Y2   ; Y,Y2
	jl      short tri_rb_loop

;------------------------------------
; Draw the horizontal lines
;


tri_draw_lines:

	mov     cx,SCREEN_SEG       ;point ES to video segment
	mov     es,cx
	mov     dx,SC_INDEX         ;set the Sequence Controller Index to
	mov     al,MAP_MASK         ; point to the Map Mask register
	out     dx,al


line_loop:
	pop     ax
	pop     dx
	cmp     ax,dx
	jg      tri_draw_next
	mov     bx,Color
	mov     cx,Y2
	add     dx,2
	mov     di,PageOffset
	call    _HLineClipR
tri_draw_next:
	dec     word ptr Y2
	dec     word ptr DY02
	jnz     line_loop

tri_done:
	pop     di si es ds
	mov     sp,bp
	pop     bp
	ret
_x_triangle  endp

;-----------------------------------------------------------------------
; void x_polygon( VERTEX vertices[], int num_vertices,unsigned Color,
;                unsigned PageOffset );
;
; Where VERTEX is defined as:
;
; typedef struct {
;    int X;
;    int Y;
; } far VERTEX;
;
;
; Written by T. Gouthas
;
;
;
; Note: This is just a quick hack of a generalized polygon routine.
;   The way it works is by splitting up polygons into triangles and
;   drawing each individual triangle.
;
; Obviously this is not as fast as it could be, but for polygons of
; 4 vertices it should perform quite respectably.
;
; Warning: Only works for convex polygons (convex polygons are such
;  that if you draw a line from any two vertices, every point on that
;  line will be within the polygon)
;
;

_x_polygon proc
       ARG   vertices:dword,numvertices:word,Color:word,PageOffset:word
       LOCAL x0:word,y0:word,tri_count:word=STK
       push  bp
       mov   bp,sp
       sub   sp,STK
       push  di si

       mov   cx,numvertices
       cmp   cx,3
       jl    @@Done

       sub   cx,3
       mov   tri_count,cx      ; Number of triangles to draw
       les   di,vertices       ; ES:DI -> Vertices

       mov   ax,es:[di]        ; Save first vertex
       mov   x0,ax
       mov   ax,es:[di+2]
       mov   y0,ax

       ; Set up permanent parameter stack frame for
       ; triangle parameters

       mov   ax,PageOffset
       push  ax
       mov   ax,Color
       push  ax

       sub   sp,12
       mov   si,sp

@@NextTriangle:

       add   di,4
       mov   ax,es:[di]      ; Vertex 2
       mov   ss:[si],ax
       mov   ax,es:[di+2]
       mov   ss:[si+2],ax

       mov   ax,es:[di+4]    ; Vertex 1
       mov   ss:[si+4],ax
       mov   ax,es:[di+6]
       mov   ss:[si+6],ax

       mov   ax,x0           ; Vertex 0: The first vertex is
       mov   ss:[si+8],ax    ; part of every triangle
       mov   ax,y0
       mov   ss:[si+10],ax

       call  _x_triangle
       dec   tri_count
       jns   @@NextTriangle

       add   sp,16    ; Remove triangle stack frame

@@Done:
       pop   si di
       mov   sp,bp
       pop   bp
       ret
_x_polygon endp
	end