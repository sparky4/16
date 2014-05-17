;-----------------------------------------------------------
;
; MXLN.ASM - Line function
; Copyright (c) 1993,1994 by Alessandro Scotti
;
;-----------------------------------------------------------
;WARN    PRO
;NOWARN  RES
INCLUDE MODEX.DEF

PUBLIC  mxLine

MX_TEXT SEGMENT USE16 PARA PUBLIC 'CODE'
        ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

EXTRN   mx_BytesPerLine : WORD
EXTRN   mx_VideoSegment : WORD

EXTRN   mx_ClipX1       : WORD
EXTRN   mx_ClipY1       : WORD
EXTRN   mx_ClipX2       : WORD
EXTRN   mx_ClipY2       : WORD

tblDrawFunc     LABEL   WORD
        DW      subWidthMove
        DW      subHeightMove
        DW      subWidthOp
        DW      subHeightOp

;-----------------------------------------------------------
;
; Draws a line from (X1,Y1) to (X2,Y2) using the Bresenham
; algorithm.
;
; Input:
;       X1, Y1  = start point
;       X2, Y2  = end point
;       Color   = line color
;       Op      = raster operator
; Output:
;       none
;
; Note: the end point (X2,Y2) *IS* drawed. I don't like this very much
; but clipping is much simpler.
;
mxLine  PROC FAR
        ARG     Op:WORD,        \
                Color:WORD,     \
                Y2:WORD,        \
                X2:WORD,        \
                Y1:WORD,        \
                X1:WORD         = ARG_SIZE
        LOCAL   Width:WORD,     \
                Height:WORD,    \
                ErrorAdd:WORD,  \
                ErrorSub:WORD,  \
                DeltaX:WORD,    \
                DeltaY:WORD,    \
                P1:BYTE,        \
                P2:BYTE,        \
                WritePlane:BYTE = AUTO_SIZE
        .enter  AUTO_SIZE
        .push   ds, si, di
        ASSUME  ds:NOTHING

        mov     ax, [X1]
        mov     bx, [Y1]
        mov     cx, [X2]
        mov     dx, [Y2]
        call    subClipLine
        jc      @@Exit                  ; Line is full clipped

; Get width
        mov     si, cx
        xchg    ax, si                  ; SI = X1, AX = X2
        sub     ax, si
        jge     @@1
; Swap points, we want X1 < X2
        xchg    si, cx                  ; Swap X1 and X2
        xchg    bx, dx                  ; Swap Y1 and Y2
        neg     ax
@@1:
        mov     [Width], ax

; Get height
        mov     cx, [mx_BytesPerLine]   ; We don't need X2 anymore
        mov     ax, dx
        sub     ax, bx
        jge     @@2
        neg     cx                      ; Move from bottom to top
        neg     ax                      ; Get absolute value of AX
@@2:
        mov     [Height], ax
        mov     [DeltaY], cx

; Get pixel address and write plane
        mov     ax, bx
        mul     [mx_BytesPerLine]
        mov     cx, si                  ; CX = X1
        shr     si, 1
        shr     si, 1
        add     si, ax                  ; SI = pixel offset
        and     cl, 03h
        mov     ax, 1102h
        shl     ah, cl
        mov     [WritePlane], ah
        mov     dx, TS
        out     dx, ax                  ; Set write plane
        mov     ax, [mx_VideoSegment]
        mov     ds, ax                  ; DS:SI points to (X1,Y1)

; Select the function to handle the drawing loop
        xor     bx, bx
        mov     al, BYTE PTR [Op]
        cmp     al, OP_MOVE
        je      @@3
        and     al, 03h
        shl     al, 1
        shl     al, 1
        shl     al, 1
        mov     ah, al
        mov     al, 03h
        mov     dx, GDC
        out     dx, ax                  ; Set logical function
        inc     bx
        inc     bx
@@3:
        mov     ax, [Width]
        mov     cx, [Height]
; Horizontal, vertical and diagonal lines are not optimized yet
        cmp     ax, cx
        jae     @@4
        inc     bx
@@4:
        shl     bx, 1
        call    tblDrawFunc[bx]

; Reset logical function if needed
        cmp     BYTE PTR [Op], OP_MOVE
        je      @@Exit
        mov     ax, 0003h
        mov     dx, GDC
        out     dx, ax

@@Exit:
        xor     ax, ax
        .pop    ds, si, di
        .leave  ARG_SIZE

;-----------------------------------------------------------
;
; Checks the coordinates of a line against the active
; clip region.
; Uses a variation of the Cohen-Sutherland algorithm developed
; by Victor Duvanenko.
;
; Input:
;       AX, BX  = X1, Y1
;       CX, DX  = X2, Y2
; Output:
;       CF      = set if line is full clipped
;       AX, BX  = clipped X1, Y1
;       CX, DX  = clipped X2, Y2
; Note:
;       destroys SI, DI
;
subClipLine     PROC    NEAR
        mov     di, ax                  ; Copy X1 to DI and free AX
        mov     si, dx                  ; Copy Y2 to SI and free DX
; Compute clip codes for point (X2,Y2)=(CX,SI)
        xor     al, al
@@P2X1: cmp     cx, [mx_ClipX1]
        jge     @@P2X2
        or      al, 1
@@P2X2: cmp     cx, [mx_ClipX2]
        jle     @@P2Y1
        or      al, 2
@@P2Y1: cmp     si, [mx_ClipY1]
        jge     @@P2Y2
        or      al, 4
@@P2Y2: cmp     si, [mx_ClipY2]
        jle     @@P2XY
        or      al, 8
@@P2XY: mov     [P2], al
; Compute clip codes for point (X1,Y1)=(DI,BX)
        xor     al, al
@@P1X1: cmp     di, [mx_ClipX1]
        jge     @@P1X2
        or      al, 1
@@P1X2: cmp     di, [mx_ClipX2]
        jle     @@P1Y1
        or      al, 2
@@P1Y1: cmp     bx, [mx_ClipY1]
        jge     @@P1Y2
        or      al, 4
@@P1Y2: cmp     bx, [mx_ClipY2]
        jle     @@P1XY
        or      al, 8
@@P1XY: mov     [P1], al
; Check codes for trivial cases
        mov     ah, [P2]
        test    al, ah                  ; Is line invisible?
        jnz     @@FullClip              ; Yes, exit
        or      ah, al                  ; Both points clipped?
        jz      @@Done                  ; Yes, exit
; Calculate deltas
        mov     ax, cx
        sub     ax, di
        mov     [DeltaX], ax
        mov     ax, si
        sub     ax, bx
        mov     [DeltaY], ax
        mov     al, [P1]                ; Init clipping code
; Clipping loop
@@ClipLoop:
        test    al, al                  ; Is first point clipped?
        jnz     @@ClipX1                ; No, continue
        xchg    cx, di                  ; Swap points...
        xchg    bx, si
        xchg    al, [P2]                ; ...and codes
; Clip left: Y1 = Y1 + DeltaY*(mx_ClipX1-X1)/DeltaX
@@ClipX1:
        test    al, 1
        jz      @@ClipX2
        mov     ax, [mx_ClipX1]
        sub     ax, di
        mov     di, [mx_ClipX1]
        jmp     @@ClipX1X2
; Clip right: Y1 = Y1 + DeltaY*(mx_ClipX2-X1)/DeltaX
@@ClipX2:
        test    al, 2
        jz      @@ClipY1
        mov     ax, [mx_ClipX2]
        sub     ax, di
        mov     di, [mx_ClipX2]
@@ClipX1X2:
        imul    [DeltaY]
        idiv    [DeltaX]
        add     bx, ax
        mov     al, 8
        cmp     bx, [mx_ClipY2]
        jg      @@CheckLoop
        mov     al, 4
        cmp     bx, [mx_ClipY1]
        jl      @@CheckLoop
        xor     al, al
        jmp     @@CheckLoop
; Clip top: X1 = X1 + DeltaX*(mx_ClipY1-Y1)/DeltaY
@@ClipY1:
        test    al, 4
        jz      @@ClipY2
        mov     ax, [mx_ClipY1]
        sub     ax, bx
        mov     bx, [mx_ClipY1]
        jmp     @@ClipY1Y2
; Clip bottom: X1 = X1 + DeltaX*(mx_ClipY2-Y1)/DeltaY
@@ClipY2:
        mov     ax, [mx_ClipY2]
        sub     ax, bx
        mov     bx, [mx_ClipY2]
@@ClipY1Y2:
        imul    [DeltaX]
        idiv    [DeltaY]
        add     di, ax
        mov     al, 1
        cmp     di, [mx_ClipX1]
        jl      @@CheckLoop
        mov     al, 2
        cmp     di, [mx_ClipX2]
        jg      @@CheckLoop
        xor     al, al
@@CheckLoop:
        mov     ah, [P2]
        test    al, ah
        jnz     @@FullClip
        or      ah, al
        jnz     @@ClipLoop

@@Done:
        mov     ax, di
        mov     dx, si
        clc
        ret
@@FullClip:
        stc
        ret
subClipLine     ENDP

; Called when Width >= Height and Op = OP_MOVE
subWidthMove    PROC NEAR
        mov     di, ax
        neg     di                      ; Initialize error term
        shl     cx, 1
        mov     [ErrorAdd], cx
        mov     cx, ax
        shl     ax, 1
        mov     [ErrorSub], ax
        mov     al, 02h
        mov     ah, [WritePlane]
        mov     bl, BYTE PTR [Color]
        mov     dx, TS
        inc     cx
@@Loop:
        mov     ds:[si], bl
        dec     cx
        jz      @@Exit
        rol     ah, 1
        adc     si, 0
        out     dx, ax
        add     di, [ErrorAdd]
        jl      @@Loop
        add     si, [DeltaY]
        sub     di, [ErrorSub]
        jmp     @@Loop
@@Exit:
        ret
subWidthMove    ENDP

; Called when Width < Height and Op = OP_MOVE
subHeightMove   PROC NEAR
        mov     di, cx
        neg     di                      ; Initialize error term
        shl     ax, 1
        mov     [ErrorAdd], ax
        mov     ax, cx
        shl     ax, 1
        mov     [ErrorSub], ax
        mov     bl, BYTE PTR [Color]
        mov     ah, [WritePlane]
        mov     al, 02h
        mov     dx, TS
        inc     cx
@@Loop:
        mov     ds:[si], bl
        dec     cx
        jz      @@Exit
        add     si, [DeltaY]
        add     di, [ErrorAdd]
        jl      @@Loop
        rol     ah, 1                   ; Next write plane
        adc     si, 0                   ; Bump video offset if plane overflows
        out     dx, ax
        sub     di, [ErrorSub]          ; Adjust error down
        jmp     @@Loop
@@Exit:
        ret
subHeightMove   ENDP

; Called when Width >= Height and Op <> OP_MOVE
subWidthOp      PROC NEAR
        mov     di, ax
        neg     di                      ; Initialize error term
        shl     cx, 1
        mov     [ErrorAdd], cx
        mov     cx, ax
        shl     ax, 1
        mov     [ErrorSub], ax
        mov     al, 02h
        mov     ah, [WritePlane]
        mov     bl, BYTE PTR [Color]
        mov     dx, TS
        inc     cx
@@Loop:
        mov     bh, ds:[si]             ; Latch data
        mov     ds:[si], bl
        dec     cx
        jz      @@Exit
        rol     ah, 1
        adc     si, 0
        out     dx, ax
        add     di, [ErrorAdd]
        jl      @@Loop
        add     si, [DeltaY]
        sub     di, [ErrorSub]
        jmp     @@Loop
@@Exit:
        ret
subWidthOp      ENDP

; Called when Width < Height and Op <> OP_MOVE
subHeightOp     PROC NEAR
        mov     di, cx
        neg     di                      ; Initialize error term
        shl     ax, 1
        mov     [ErrorAdd], ax
        mov     ax, cx
        shl     ax, 1
        mov     [ErrorSub], ax
        mov     bl, BYTE PTR [Color]
        mov     ah, [WritePlane]
        mov     al, 02h
        mov     dx, TS
        inc     cx
@@Loop:
        mov     bh, ds:[si]
        mov     ds:[si], bl
        dec     cx
        jz      @@Exit
        add     si, [DeltaY]
        add     di, [ErrorAdd]
        jl      @@Loop
        rol     ah, 1                   ; Next write plane
        adc     si, 0                   ; Bump video offset if plane overflows
        out     dx, ax
        sub     di, [ErrorSub]          ; Adjust error down
        jmp     @@Loop
@@Exit:
        ret
subHeightOp     ENDP

mxLine  ENDP

MX_TEXT         ENDS
END
