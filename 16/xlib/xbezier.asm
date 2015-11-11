
;-----------------------------------------------------------------------
; MODULE XBEZIER
;
;
; Compile with TASM.
; C near-callable.
;
; This module was written by Matthew MacKenzie
; matm@eng.umd.edu
;-----------------------------------------------------------------------
include xlib.inc
include xbezier.inc


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; _x_bezier
;
; Plot a Bezier curve, which is described by a box of two endpoints
; and two control points:
;                           C1--------C2
;                           /           \
;                          /             \
;                        E1..............E2
;
; All coordinates must be in the range -1024 to 3071.
; No clipping is performed.
;
; C near-callable as:
; x_bezier (int E1x, int E1y, int C1x, int C1y, int C2x, int C2y,
;           int E2x, int E2y, int levels, char color,
;           unsigned int PageOffset);
;
; All four main registers are totaled.
; This function may use as many as 162 bytes of stack space.

; Bezier curves need 32-bit precision, so we'll define macros and
; constants for operations on 32-bit virtual registers V0, V1, and V2.
; V0 is made up of DI and AX, V1 of SI and BX, and V2 of CX and DX.

LowWord         equ     0
HighWord        equ     2

; to load data --

LoadV0 macro loc, field
    mov ax, word ptr [bp + loc + field + LowWord]
    mov di, word ptr [bp + loc + field + HighWord]
    endm

LoadV1 macro loc, field
    mov bx, word ptr [bp + loc + field + LowWord]
    mov si, word ptr [bp + loc + field + HighWord]
    endm

LoadV2 macro loc, field
    mov dx, word ptr [bp + loc + field + LowWord]
    mov cx, word ptr [bp + loc + field + HighWord]
    endm

; to store data --

StoreV0 macro loc, field
    mov word ptr [bp + loc + field + LowWord], ax
    mov word ptr [bp + loc + field + HighWord], di
    endm

StoreV1 macro loc, field
    mov word ptr [bp + loc + field + LowWord], bx
    mov word ptr [bp + loc + field + HighWord], si
    endm


; to take the average of two registers (result is in first register) --

AverageV0nV1 macro
	add ax, bx
	adc di, si
	shr di, 1
	rcr ax, 1
	endm

AverageV0nV2 macro
	add ax, dx
	adc di, cx
	shr di, 1
	rcr ax, 1
	endm

AverageV1nV2 macro
	add bx, dx
	adc si, cx
	shr si, 1
	rcr bx, 1
	endm


; to take the average of a register and data --

AverageV1nData macro loc, field
    add bx, word ptr [bp + loc + field + LowWord]
    adc si, word ptr [bp + loc + field + HighWord]
    shr si, 1
    rcr bx, 1
    endm

AverageV2nData macro loc, field
    add dx, word ptr [bp + loc + field + LowWord]
    adc cx, word ptr [bp + loc + field + HighWord]
    shr cx, 1
    rcr dx, 1
    endm


; to turn a 32-bit fixed point data into a regular integer --

Extract macro reg, source, field
	mov reg, word ptr [bp + source + field + HighWord]
	shr reg, 3
	adc reg, 0          ; round
	endm


; to turn an integer argument into a 32-bit fixed point number
; and store it as local data --

Convert macro source, dest, field
	mov ax, source
	add ax, 1024
	shl ax, 3
	push ax
	push 0
	endm


; useful numbers for dealing with Bezier boxes (sets of four points) --

XCoord      equ     4
YCoord      equ     0

; stack offsets for splitting boxes
E1Src       equ     48
C1Src       equ     40
C2Src       equ     32
E2Src       equ     24

E1Dest      equ     48
P1Dest      equ     40
P4Dest      equ     32
P6Dest      equ     24
P5Dest      equ     16
P2Dest      equ     8
E2Dest      equ     0

; stack offsets for drawing boxes
E1Draw      equ     24
C1Draw      equ     16
C2Draw      equ     8
E2Draw      equ     0

    .data

    align 2

; depth of recursion at which to plot
WhenToDraw label byte
   db 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4
   db 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5


; since we'll be moving bp and sp in our recursion,
; we need to put local storage in the data segment
level           dw  (?)
gencount        dw  (?)
AdjustedOffs    dw  (?)
p1xt            dw  (?)
p1yt            dw  (?)
p2xt            dw  (?)
p2yt            dw  (?)
p4xt            dw  (?)
p4yt            dw  (?)
p5xt            dw  (?)
p5yt            dw  (?)
c1xt            dw  (?)
c1yt            dw  (?)
c2xt            dw  (?)
c2yt            dw  (?)
xdiff           dw  (?)
ydiff           dw  (?)
moveline        dw  (?)
stepper         dw  (?)
bptemp          dw  (?)

; by the same token we need a copy of this argument
Colort          dw  (?)


ColumnMasks label byte
    db 011h, 022h, 044h, 088h

    .code

    align 2
_x_bezier proc
ARG     E1x, E1y, C1x, C1y, C2x, C2y, E2x, E2y, Levels, Color, PageOffs:word

    push bp
    mov bp, sp          ; caller's stack frame
    push si
    push di
    push es

	; set local variables
    mov ax, -1024       ; 1024 rows
    imul [_ScrnLogicalByteWidth]
    add ax, PageOffs
    sub ax, 256         ; 1024 columns
    mov AdjustedOffs, ax ; subtract 1024 rows and 1024 columns

; copy color into data segment so we can change bp & sp
    mov ax, Color
	mov Colort, ax

	mov cx, Levels
	dec cx              ; gencount (number of boxes we will actually plot) =
	mov ax,1            ; 2^(Levels - 1)
	shl ax,cl
	dec ax
	mov gencount, ax

	mov [level], 0      ; variable to tell us where we are in the stack
	mov bptemp, bp		; when the dust settles at the end

; translate coordinates for adjusted offset, convert 'em to fixed-point
; with 13 bits for the integer part and 19 for the fractional part,
; and push them onto the stack in the right order to for a Bezier box

	Convert E2x
	Convert E2y

	Convert C2x
	Convert C2y

	Convert C1x
	Convert C1y

	Convert E1x
	Convert E1y

	mov bp, sp          ; we are using them as basically one pointer

	mov ax, 0a000h      ; point extra segment to VGA memory
    mov es, ax

    mov dx, SC_INDEX
    mov al, MAP_MASK
    out dx, al

MainLoop:
    mov si, gencount
    mov ax, 0
    mov al, WhenToDraw[si]
    cmp ax, level       ; are we at a terminal curve?
    jne Recurse
    jmp PlotCurve

Recurse:
; not at a terminal -- so expand this curve into two more and recurse

; start with:
;    C1___C2
;     |    |
;    E1...E2
;
; stack looks like:  E1 C1 C2 E2

; expand like this:
;    C1.....P3.....C2
;     .   .     .   .
;     . _P4_P6__P5_ .
;    P1-   .. ..   P2
;     |  ..     ..  |
;     |..         ..|
;    E1............E2
;
; stack looks like:  E1 P1 P4 P6 P5 P2 E2
; Since P6 is part of both new boxes, we use it twice.

    sub sp, 24
    sub bp, 24

; new points for X --
    LoadV0      E1Src, XCoord
    LoadV1      E2Src, XCoord
    StoreV1     E2Dest, XCoord
    LoadV2      C1Src, XCoord
    AverageV0nV2
    StoreV0     P1Dest, XCoord
    AverageV1nData C2Src, XCoord
    StoreV1     P2Dest, XCoord
    AverageV2nData C2Src, XCoord
    AverageV0nV2
    StoreV0     P4Dest, XCoord
    AverageV1nV2
    StoreV1     P5Dest, XCoord
    AverageV0nV1
	StoreV0     P6Dest, XCoord

; same thing for Y --
    LoadV0      E1Src, YCoord
    LoadV1      E2Src, YCoord
    StoreV1     E2Dest, YCoord
    LoadV2      C1Src, YCoord
    AverageV0nV2
    StoreV0     P1Dest, YCoord
    AverageV1nData C2Src, YCoord
    StoreV1     P2Dest, YCoord
    AverageV2nData C2Src, YCoord
    AverageV0nV2
    StoreV0     P4Dest, YCoord
    AverageV1nV2
    StoreV1     P5Dest, YCoord
    AverageV0nV1
    StoreV0     P6Dest, YCoord

    inc level           ; one level further into stack
    jmp MainLoop

PlotCurve:
; pull 16-bit coordinates back out of 32-bit fixed-point coordinates;
; integer part is highest 13 bits

	Extract cx, C1Draw, XCoord
	Extract di, E1Draw, XCoord
    mov c1xt, cx
    add cx, di
    shr cx, 1
    mov p1xt, cx

    Extract ax, C1Draw, YCoord
    Extract si, E1Draw, YCoord
    mov c1yt, ax
    add ax, si
    shr ax, 1
	mov p1yt, ax
	call ShortLine      ; line from P1 to E1

	Extract cx, E2Draw, XCoord
	Extract di, C2Draw, XCoord
	mov c2xt, di
	add di, cx
	shr di, 1
	mov p2xt, di

	Extract ax, E2Draw, YCoord
	Extract si, C2Draw, YCoord
	mov c2yt, si
	add si, ax
	shr si, 1
	mov p2yt, si
	call ShortLine      ; line from E2 to P2

; P3 is not in any line we draw, so we'll use it now to find both P5
; for the line after this on, and P4 for this line, then discard it --
	mov bx, c1xt
	add bx, c2xt
	shr bx, 1
	mov dx, c1yt
	add dx, c2yt
	shr dx, 1

; find P5 x and save for later lines
	mov cx, p2xt
	add cx, bx
	shr cx, 1
	mov p5xt, cx
; find P4 x for this line
	mov cx, p1xt
	add cx, bx
	shr cx, 1
	mov p4xt, cx
	mov di, p1xt

; find P5 y and save for later lines
	mov ax, p2yt
	add ax, dx
	shr ax, 1
	mov p5yt, ax
; find P4 y for this line
	mov ax, p1yt
	add ax, dx
	shr ax, 1
	mov p4yt, ax
	mov si, p1yt
	call ShortLine      ; line from P4 to P1 -- finally!

; we've already done all the work for these last two --
	mov cx, p2xt
	mov di, p5xt
	mov ax, p2yt
	mov si, p5yt
	call ShortLine      ; line from P2 to P5

	mov cx, p5xt
	mov di, p4xt
	mov ax, p5yt
	mov si, p4yt
	call ShortLine      ; line from P5 to P4

; we've drawn our five lines; this bezier box
; can be dropped off the stack
    add bp, 24
    add sp, 24

    dec gencount
    mov cx, gencount
    cmp cx, -1

    je WrapUp           ; if we've generated all the terminal nodes we
                        ; are supposed to, we pack our bags and go.
	dec level
    jmp MainLoop

WrapUp:
; plot the final point, which is simply the original E1

    mov bp, bptemp      ; back where we started
    mov ax, E1y
    add ax, 1024
    mul [_ScrnLogicalByteWidth]
    mov di, E1x
    add di, 1024
    mov si, di
    shr di, 2
    add di, ax
    add di, AdjustedOffs
    and si, 3
    mov al, ColumnMasks[si]
    mov ah, byte ptr Color
    mov dx, SC_INDEX + 1
    out dx, al
    mov es:[di], ah

    pop es
    pop di
    pop si
    mov sp, bp
    pop bp
    ret                 ; git

; ShortLine subfunction --
;
; This is designed for short line segments.  For longer lines,
; especially horizontal ones, the x_line routine in the XLINE module
; would be faster.  But calling that from here it would be a lot of
; extra complication.  This is part of the x_bezier routine because
; it has no particular outside use, and we can call it much faster
; through registers than through the stack.
;
; Since our line segments overlap, the second endpoint is not drawn.
; These routines are all out of order for the sake of conditional jumps.

LRHorz:
	dec di
LRHorzLoop:
	rol al, 1
	adc bx, 0
	out dx, al
	mov es:[bx], ah
	sub di, 1
	jg LRHorzLoop
	retn

; You are in a maze of little subroutines, all alike...


LR45Deg:
    dec si
LR45DegLoop:
    add bx, cx
    rol al, 1
    adc bx, 0
    out dx, al
    mov es:[bx], ah
    sub si, 1
    jg LR45DegLoop
	retn


LRXMajor:
    mov cx, di
    dec cx
    shr di, 1
LRXMajorLoop:
    sub di, si
    jge LRXMajorIterate
    add di, xdiff
    add bx, moveline
LRXMajorIterate:
    rol al, 1
    adc bx, 0
    out dx, al
    mov es:[bx], ah
    sub cx, 1
    jg LRXMajorLoop
	retn


LeftToRight:
    mov xdiff, di       ; store distance across line

    mov cx, [_ScrnLogicalByteWidth]
    cmp si, 0           ; check if height is positive, negative, or zero
    je LRHorz
    jg LRTopToBottom
    neg si
    neg cx
LRTopToBottom:
    mov ydiff, si
    mov moveline, cx
    cmp di, si
    jg LRXMajor
    je LR45Deg


LRYMajor:
    mov cx, si
    dec cx
    shr si, 1
LRYMajorLoop:
    add bx, moveline
    sub si, di
    jge LRYMajorIterate
    add si, ydiff
    rol al, 1
    adc bx, 0
LRYMajorIterate:
    out dx, al
    mov es:[bx], ah
    sub cx, 1
    jg LRYMajorLoop
	retn


; This is the actual starting point.
; On entry, registers look like this:
; X1: cx
; Y1: ax
; X2: di
; Y2: si
ShortLine:
    sub si, ax          ; height goes out in si
    sub di, cx          ; width goes out in di

    mul [_ScrnLogicalByteWidth]
    mov dx, cx
    shr dx, 2
    add ax, dx
    add ax, AdjustedOffs
    mov bx, ax          ; starting byte of X1, Y1 goes out in bx
    and cx, 3
    mov ax, 011h
    shl al, cl          ; column mask goes out in al
    mov dx, SC_INDEX + 1
    mov ah, byte ptr Colort; color goes out in ah
    out dx, al
    mov es:[bx], ah     ; plot first point

    cmp di, 0

    jg LeftToRight
    je VerticalLine


RightToLeft:
    neg di              ; much more useful this way
    mov xdiff, di

    mov cx, [_ScrnLogicalByteWidth]
    cmp si, 0           ; check if height is positive, negative, or zero
    je RLHorz
    jg RLTopToBottom
    neg si
    neg cx
RLTopToBottom:
    mov ydiff, si
    mov moveline, cx
    cmp di, si
    jg RLXMajor
    je RL45Deg


RLYMajor:
    mov cx, si
    dec cx
    shr si, 1
RLYMajorLoop:
    add bx, moveline
    sub si, di
    jge RLYMajorIterate
    add si, ydiff
    ror al, 1
    sbb bx, 0
RLYMajorIterate:
    out dx, al
    mov es:[bx], ah
    sub cx, 1
    jg RLYMajorLoop
	retn


VerticalLine:
    mov cx, [_ScrnLogicalByteWidth]
    cmp si, 0           ; check if height is positive
    jg VTopToBottom
    neg si
    neg cx
VTopToBottom:
    dec si
VLoop:
    add bx, cx
    mov es:[bx], ah
    sub si, 1
    jg VLoop
	retn


RLHorz:
    dec di
RLHorzLoop:
    ror al, 1
    sbb bx, 0
    out dx, al
    mov es:[bx], ah
    sub di, 1
    jg RLHorzLoop
	retn


RL45Deg:
    dec si
RL45DegLoop:
    add bx, cx
    ror al, 1
    sbb bx, 0
    out dx, al
    mov es:[bx], ah
    sub si, 1
    jg RL45DegLoop
	retn


RLXMajor:
    mov cx, di
    dec cx
    shr di, 1
RLXMajorLoop:
    sub di, si
    jge RLXMajorIterate
    add di, xdiff
    add bx, moveline
RLXMajorIterate:
    ror al, 1
    sbb bx, 0
    out dx, al
    mov es:[bx], ah
    sub cx,1
    jg RLXMajorLoop
	retn


_x_bezier endp

    end

