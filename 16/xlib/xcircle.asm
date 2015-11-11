;-----------------------------------------------------------------------
;
; MODULE XCIRCLE
;
;   This module was written by Matthew MacKenzie
;   matm@eng.umd.edu
;
; Circles, full and empty.
;
; Compile with Tasm.
; C callable.
;
; ****** XLIB - Mode X graphics library                ****************
; ******                                               ****************
;
; egg@dstos3.dsto.gov.au
; teg@bart.dsto.gov.au
;-----------------------------------------------------------------------

include xlib.inc
include xcircle.inc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; _x_circle
;
; Draw a circle.
;
; C near-callable as:
; int x_circle (WORD Left, WORD Top, WORD Diameter,
;                WORD Color, WORD ScreenOffs);
;
; No clipping is performed.
;
; ax, bx, cx, and dx bite the dust, as Homer would say.

; we plot into eight arcs at once:
;      4 0
;    6 \|/ 2
;     --*--
;    7 /|\ 3
;      5 1
;
; 0, 1, 4, and 5 are considered x-major; the rest, y-major.
;
; The x-major plots grow out from the top and bottom of the circle,
; while the y-major plots start at the left and right edges.

	.data

	align 2

ColumnMask      db      011h,022h,044h,088h

	.code

	public _x_circle
	align   2
_x_circle proc
ARG Left:word, Top:word, Diameter:word, Color:word, ScreenOffs:word
LOCAL offset0,offset1,offset2,offset3,offset4,offset5,offset6,offset7,mask0n1,mask2n3,mask4n5,mask6n7,shrunk_radius,diameter_even,error:word=LocalStk
; Tasm 1.0 does not allow the \ line continuation
;LOCAL offset0:word, offset1:word, offset2:word, offset3:word, \
;	  offset4:word, offset5:word, offset6:word, offset7:word, \
;	  mask0n1:word, mask2n3:word, mask4n5:word, mask6n7:word, \
;	  shrunk_radius:word, diameter_even:word, error:word=LocalStk

	push bp
	mov  bp, sp
	sub  sp, LocalStk
	push si
	push di
	push ds

; find starting locations of plots 2, 3, 6, and 7
	mov di, _ScrnLogicalByteWidth
	xor dx, dx

	mov ax, Diameter    ; find vertical midpoint
	dec ax
	shr ax, 1
	adc dx, 0           ; remember if it's rounded
	mov shrunk_radius, ax ; radius, rounded down for adding
	mov diameter_even, dx ; (diameter - 1) & 1, effectively
	add ax, Top
	mul di              ; vertical midpoint in bytes
	add ax, ScreenOffs

	mov bx, Left
	mov cx, bx          ; save for later
	mov si, bx
	shr si, 2
	add si, ax
	mov offset6, si
	and bx, 3           ; column of left side
	mov bl, ColumnMask[bx]
	mov mask6n7, bx

	add cx, Diameter
	dec cx
	mov bx, cx
	shr cx, 2
	add cx, ax
	mov offset2, cx
	and bx, 3           ; column of right side
	mov bl, ColumnMask[bx]
	mov mask2n3, bx

	cmp diameter_even, 1
	jne @@MiddlePlotsOverlap
	add si, di
	add cx, di
@@MiddlePlotsOverlap:
	mov offset7, si
	mov offset3, cx

; starting locations of 0, 1, 4, and 5
	mov bx, Left
	add bx, shrunk_radius ; find horizontal midpoint

	mov ax, Top         ; top in bytes
	mul di
	add ax, ScreenOffs
	mov si, ax

	mov ax, Diameter    ; bottom in bytes
	dec ax
	mul di
	add ax, si

	mov di, bx          ; horizontal midpoint in bytes
	shr di, 2
	add si, di     		; top midpoint in bytes
	mov offset4, si
	add di, ax     		; bottom midpoint in bytes
	mov offset5, di
	and bx, 3           ; column of horizontal midpoint
	mov bl, ColumnMask[bx]
	mov mask4n5, bx

	cmp diameter_even, 1
	jne @@TopAndBottomPlotsOverlap
	rol bl, 1
	jnc @@TopAndBottomPlotsOverlap
	inc si
	inc di
@@TopAndBottomPlotsOverlap:
	mov offset0, si
	mov offset1, di
	mov mask0n1, bx

; we've got our eight plots in their starting positions, so
; it's time to sort out the registers
	mov bx, _ScrnLogicalByteWidth

	mov dx, SCREEN_SEG
	mov ds, dx

	mov dx, SC_INDEX    ; set VGA to accept column masks
	mov al, MAP_MASK
	out dx, al
	inc dx              ; gun the engine...

	mov si, Diameter    ; initial y is radius -- 2 #s per pixel
	inc si

	mov cx, si
	neg cx
	add cx, 2
	mov error, cx	    ; error = -y + one pixel since we're a step ahead

	xor cx, cx          ; initial x = 0
	mov ah, byte ptr Color
	jmp @@CircleCalc	; let's actually put something on the screen!

; move the x-major plots horizontally and the y-major plots vertically
@@NoAdvance:
	mov al, byte ptr mask0n1
	out dx, al
	mov di, offset0     ; plot 0
	mov [di], ah
	rol al, 1           ; advance 0 right
	mov byte ptr mask0n1, al
	adc di, 0
	mov offset0, di
	mov di, offset1
	mov [di], ah        ; plot 1
	ror al, 1           ; what was that bit again?
	adc di, 0           ; advance 1 right
	mov offset1, di

	mov al, byte ptr mask2n3
	out dx, al
	mov di, offset2
	mov [di], ah        ; plot 2
	sub di, bx          ; advance 2 up
	mov offset2, di
	mov di, offset3
	mov [di], ah        ; plot 3
	add di, bx          ; advance 3 down
	mov offset3, di

	mov al, byte ptr mask4n5
	out dx, al
	mov di, offset4
	mov [di], ah
	ror al, 1
	mov byte ptr mask4n5, al
	sbb di, 0
	mov offset4, di
	mov di, offset5
	mov [di], ah
	rol al, 1
	sbb di, 0
	mov offset5, di

	mov al, byte ptr mask6n7
	out dx, al
	mov di, offset6
	mov [di], ah
	sub di, bx
	mov offset6, di
	mov di, offset7
	mov [di], ah
	add di, bx
	mov offset7, di

	jmp @@CircleCalc

; move all plots diagonally
@@Advance:
	mov al, byte ptr mask0n1
	out dx, al
	mov di, offset0
	mov [di], ah        ; plot 0
	rol al, 1           ; advance 0 right and down
	mov byte ptr mask0n1, al
	adc di, bx
	mov offset0, di
	mov di, offset1
	mov [di], ah        ; plot 1
	ror al, 1           ; what was that bit again?
	adc di, 0           ; advance 1 right and up
	sub di, bx
	mov offset1, di

	mov al, byte ptr mask2n3
	out dx, al
	mov di, offset2
	mov [di], ah        ; plot 2
	ror al, 1           ; advance 2 up and left
	mov byte ptr mask2n3, al
	sbb di, bx
	mov offset2, di
	mov di, offset3
	mov [di], ah        ; plot 3
	rol al, 1
	sbb di, 0           ; advance 3 down and left
	add di, bx
	mov offset3, di

	mov al, byte ptr mask4n5
	out dx, al
	mov di, offset4
	mov [di], ah
	ror al, 1
	mov byte ptr mask4n5, al
	sbb di, 0
	add di, bx
	mov offset4, di
	mov di, offset5
	mov [di], ah
	rol al, 1
	sbb di, bx
	mov offset5, di

	mov al, byte ptr mask6n7
	out dx, al
	mov di, offset6
	mov [di], ah
	rol al, 1
	mov byte ptr mask6n7, al
	adc di, 0
	sub di, bx
	mov offset6, di
	mov di, offset7
	mov [di], ah
	ror al, 1
	adc di, bx
	mov offset7, di

; do you realize the entire function has been set up for this little jot?
; keep in mind that radii values are 2 per pixel
@@CircleCalc:
	add cx, 2           ; x += 1
	mov di, error
	add di, cx          ; error += (2 * x) + 1
	inc di
	jl @@CircleNoError
	cmp cx, si          ; x > y?
	ja @@FleeFlyFlowFum
	sub si, 2           ; y -= 1
	sub di, si          ; error -= (2 * y)
	mov error, di
	jmp @@Advance
@@CircleNoError:
	mov error, di
	jmp @@NoAdvance

@@FleeFlyFlowFum:
	pop ds
	pop di
	pop si
	mov sp,bp
	pop bp

	ret

_x_circle endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; _x_filled_circle
;
; Draw a disc.
;
; C near-callable as:
; int x_filled_circle (WORD Left, WORD Top, WORD Diameter,
;                      WORD Color, WORD ScreenOffs);
;
; No clipping is performed.
;
; ax, bx, cx, dx, and es bite the dust, as Homer would say.
; DF is set to 0 (strings go forward).

	.data

	align 2

; the only entries of these tables which are used are positions
; 1, 2, 4, and 8
LeftMaskTable   db      0, 0ffh, 0eeh, 0, 0cch, 0, 0, 0, 088h
RightMaskTable  db		0, 011h, 033h, 0, 077h, 0, 0, 0, 0ffh

	.code

	public _x_filled_circle
	align   2
_x_filled_circle proc
ARG Left:word, Top:word, Diameter:word, Color:word, ScreenOffs:word
; Tasm 1.0 does not allow the \ line continuation
;LOCAL offset0:word, offset1:word, offset2:word, offset3:word, \
;	  offset4:word, offset5:word, offset6:word, offset7:word, \
;	  mask0n1:word, mask2n3:word, mask4n5:word, mask6n7:word, \
;	  shrunk_radius:word, diameter_even:word, error:word, \
;	  jump_vector:word=LocalStk
LOCAL offset0,offset1,offset2,offset3,offset4,offset5,offset6,offset7,mask0n1,mask2n3,mask4n5,mask6n7,shrunk_radius,diameter_even,error,jump_vector:word=LocalStk

	push bp
	mov  bp, sp
	sub  sp, LocalStk
	push si
	push di

	cld					; strings march forward

; this first part is identical to the other function --
; the only differences, in fact, are in the drawing and moving around

; find starting locations of plots 2, 3, 6, and 7
	mov di, _ScrnLogicalByteWidth
	xor dx, dx

	mov ax, Diameter    ; find vertical midpoint
	dec ax
	shr ax, 1
	adc dx, 0           ; remember if it's rounded
	mov shrunk_radius, ax ; radius, rounded down for adding
	mov diameter_even, dx ; (diameter - 1) & 1, effectively
	add ax, Top
	mul di              ; vertical midpoint in bytes
	add ax, ScreenOffs

	mov bx, Left
	mov cx, bx          ; save for later
	mov si, bx
	shr si, 2
	add si, ax
	mov offset6, si
	and bx, 3           ; column of left side
	mov bl, ColumnMask[bx]
	mov mask6n7, bx

	add cx, Diameter
	dec cx
	mov bx, cx
	shr cx, 2
	add cx, ax
	mov offset2, cx
	and bx, 3           ; column of right side
	mov bl, ColumnMask[bx]
	mov mask2n3, bx

	cmp diameter_even, 1
	jne @@MiddlePlotsOverlap
	add si, di
	add cx, di
@@MiddlePlotsOverlap:
	mov offset7, si
	mov offset3, cx

; starting locations of 0, 1, 4, and 5
	mov bx, Left
	add bx, shrunk_radius ; find horizontal midpoint

	mov ax, Top         ; top in bytes
	mul di
	add ax, ScreenOffs
	mov si, ax

	mov ax, Diameter    ; bottom in bytes
	dec ax
	mul di
	add ax, si

	mov di, bx          ; horizontal midpoint in bytes
	shr di, 2
	add si, di     		; top midpoint in bytes
	mov offset4, si
	add di, ax     		; bottom midpoint in bytes
	mov offset5, di
	and bx, 3           ; column of horizontal midpoint
	mov bl, ColumnMask[bx]
	mov mask4n5, bx

	cmp diameter_even, 1
	jne @@TopAndBottomPlotsOverlap
	rol bl, 1
	jnc @@TopAndBottomPlotsOverlap
	inc si
	inc di
@@TopAndBottomPlotsOverlap:
	mov offset0, si
	mov offset1, di
	mov mask0n1, bx

; we've got our eight plots in their starting positions, so
; it's time to sort out the registers
	mov bx, _ScrnLogicalByteWidth

	mov dx, SCREEN_SEG
	mov es, dx

	mov dx, SC_INDEX    ; set VGA to accept column masks
	mov al, MAP_MASK
	out dx, al
	inc dx              ; gun the engine...

	mov si, Diameter    ; initial y is radius -- 2 #s per pixel
	inc si

	mov cx, si
	neg cx
	add cx, 2
	mov error, cx	    ; error = -y + one pixel since we're a step ahead

	xor cx, cx          ; initial x = 0
	mov ah, byte ptr Color
	jmp @@FilledCircleCalc ; let's actually put something on the screen!


; plotting is completely different from in the other function (naturally)
@@PlotLines:
	push cx				; we'll need cx for string stores

; draw x-major horz. lines, from plot 4 to plot 0 and from plot 5 to plot 1
	mov di, mask0n1
	and di, 0000fh		; we only want the lower nybble for the mask table
	mov al, RightMaskTable[di]
	mov di, offset0		; left and right offsets the same?
	cmp di, offset4
	jne @@PlotXMajorNontrivial ; try and say this one 10 times fast!
	mov di, mask4n5
	and di, 0000fh
	and al, LeftMaskTable[di] ; intersection of left & right masks
	out dx, al			; set mask
	mov di, offset4
	mov es:[di], ah
	mov di, offset5
	mov es:[di], ah
	jmp @@PlotYMajor
@@PlotXMajorNontrivial:
	out dx, al          ; draw right edge
	mov es:[di], ah
	mov di, offset1
	mov es:[di], ah

	mov di, mask4n5		; draw left edge
	and di, 0000fh
	mov al, LeftMaskTable[di]
	out dx, al
	mov di, offset4
	mov es:[di], ah
	mov di, offset5
	mov es:[di], ah

	mov al, 0ffh		; set mask for middle chunks
	out dx, al
	mov al, ah			; ready to store two pixels at a time
	inc di				; move string start past left edge
	mov cx, offset1		; store line from plot 5 to plot 1, exclusive
	sub cx, di			; width of section in bytes
	push cx
	shr cx, 1			; draw midsection eight pixels at a time
	rep stosw
	adc cx, 0			; draw last four pixels, if such there are
	rep stosb

	mov di, offset4		; draw line from plot 4 to plot 0
	inc di				; move past left edge
	pop cx
	shr cx, 1
	rep stosw
	adc cx, 0
	rep stosb

@@PlotYMajor:
; draw y-major horz. lines, from plot 6 to plot 2 and from plot 7 to plot 3
	mov di, mask2n3
	and di, 0000fh		; we only want the lower nybble for the mask table
	mov al, RightMaskTable[di]
	mov di, offset2		; left and right offsets the same?
	cmp di, offset6
	jne @@PlotYMajorNontrivial ; try and say this one 10 times fast!
	mov di, mask6n7
	and di, 0000fh
	and al, LeftMaskTable[di] ; intersection of left & right masks
	out dx, al			; set mask
	mov di, offset6
	mov es:[di], ah
	mov di, offset7
	mov es:[di], ah
	jmp @@ClimaxOfPlot
@@PlotYMajorNontrivial:
	out dx, al          ; draw right edge
	mov es:[di], ah
	mov di, offset3
	mov es:[di], ah

	mov di, mask6n7		; draw left edge
	and di, 0000fh
	mov al, LeftMaskTable[di]
	out dx, al
	mov di, offset6
	mov es:[di], ah
	mov di, offset7
	mov es:[di], ah

	mov al, 0ffh		; set mask for middle chunks
	out dx, al
	mov al, ah			; ready to store two pixels at a time

	inc di				; move string start past left edge
	mov cx, offset3		; draw line from plot 7 to plot 3, exclusive
	sub cx, di			; width of section in bytes
	push cx
	shr cx, 1			; store midsection
	rep stosw
	adc cx, 0
	rep stosb

	mov di, offset6		; draw line from plot 6 to plot 2
	inc di				; move past left edge
	pop cx
	shr cx, 1
	rep stosw
	adc cx, 0
	rep stosb

@@ClimaxOfPlot:
	pop cx
	jmp [jump_vector]	; either @@Advance or @@NoAdvance


; unlike their counterparts in the other function, these do not draw --
; they only move the eight pointers

; move the x-major plots horizontally and the y-major plots vertically
@@NoAdvance:
	mov al, byte ptr mask0n1 ; advance left x-major plots
	mov di, offset0
	rol al, 1           ; advance 0 right
	mov byte ptr mask0n1, al
	adc di, 0
	mov offset0, di
	mov di, offset1
	ror al, 1           ; what was that bit again?
	adc di, 0           ; advance 1 right
	mov offset1, di

	mov al, byte ptr mask4n5 ; advance left x-major plots
	mov di, offset4
	ror al, 1
	mov byte ptr mask4n5, al
	sbb di, 0
	mov offset4, di
	mov di, offset5
	rol al, 1
	sbb di, 0
	mov offset5, di

	mov al, byte ptr mask2n3
	mov di, offset2
	sub di, bx          ; advance 2 up
	mov offset2, di
	mov di, offset3
	add di, bx          ; advance 3 down
	mov offset3, di

	mov al, byte ptr mask6n7
	mov di, offset6
	sub di, bx
	mov offset6, di
	mov di, offset7
	add di, bx
	mov offset7, di

	jmp @@FilledCircleCalc

; move all plots diagonally
@@Advance:
	mov al, byte ptr mask0n1
	mov di, offset0
	rol al, 1           ; advance 0 right and down
	mov byte ptr mask0n1, al
	adc di, bx
	mov offset0, di
	mov di, offset1
	ror al, 1           ; what was that bit again?
	adc di, 0           ; advance 1 right and up
	sub di, bx
	mov offset1, di

	mov al, byte ptr mask2n3
	mov di, offset2
	ror al, 1           ; advance 2 up and left
	mov byte ptr mask2n3, al
	sbb di, bx
	mov offset2, di
	mov di, offset3
	rol al, 1
	sbb di, 0           ; advance 3 down and left
	add di, bx
	mov offset3, di

	mov al, byte ptr mask4n5
	mov di, offset4
	ror al, 1
	mov byte ptr mask4n5, al
	sbb di, 0
	add di, bx
	mov offset4, di
	mov di, offset5
	rol al, 1
	sbb di, bx
	mov offset5, di

	mov al, byte ptr mask6n7
	mov di, offset6
	rol al, 1
	mov byte ptr mask6n7, al
	adc di, 0
	sub di, bx
	mov offset6, di
	mov di, offset7
	ror al, 1
	adc di, bx
	mov offset7, di

; do you realize the entire function has been set up around this little jot?
; keep in mind that radii values are 2 per pixel
@@FilledCircleCalc:
	add cx, 2           ; x += 1
	mov di, error
	add di, cx          ; error += (2 * x) + 1
	inc di
	jl @@FilledCircleNoError
	cmp cx, si          ; x > y?
	ja @@FleeFlyFlowFum
	sub si, 2           ; y -= 1
	sub di, si          ; error -= (2 * y)
	mov error, di
	mov jump_vector, offset @@Advance
	jmp @@PlotLines
@@FilledCircleNoError:
	mov error, di
	mov jump_vector, offset @@NoAdvance
	jmp @@PlotLines

@@FleeFlyFlowFum:
	pop di
	pop si
	mov sp,bp
	pop bp

	ret

_x_filled_circle endp

	end

