;-----------------------------------------------------------
;
; MXPG.ASM - Convex polygon fill
; Copyright (c) 1994 by Alessandro Scotti
;
;-----------------------------------------------------------
WARN    PRO
NOWARN  RES
INCLUDE MODEX.DEF

PUBLIC  mxFillPoly

;-----------------------------------------------------------
;
; "Local" definitions
;
TPOINT  STRUC
        X       DW      ?
        Y       DW      ?
TPOINT  ENDS

; Do NOT change order!
TSCAN   STRUC
        Y1      DW      ?
        Y2      DW      ?
TSCAN   ENDS

MAXSCANCOLUMNS  EQU     POLYSCANBUFSIZE / SIZE TSCAN

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

EXTRN   mx_VideoSegment : WORD
EXTRN   mx_CodeSegment  : WORD
EXTRN   mx_BytesPerLine : WORD
EXTRN   mx_ClipX1       : WORD
EXTRN   mx_ClipY1       : WORD
EXTRN   mx_ClipX2       : WORD
EXTRN   mx_ClipY2       : WORD
EXTRN   mx_ScanBuffer   : NEAR

;-----------------------------------------------------------
;
; Scans an edge using the DDA (digital differential analyzer) algorithm.
;
; Input:
;       DS:BX   = pointer to start point (X1, Y1)
;       DS:SI   = pointer to end point (X2, Y2)
;       ES:DI   = pointer to edge buffer
; Output:
;       ES:DI   = updated pointer to edge buffer
; Notes:
;       must preserve DS:SI.
;
subScan         PROC NEAR
        mov     cx, ds:[si].X
        sub     cx, ds:[bx].X           ; Get width
        jg      @@1
        ret
@@1:
        push    bp                      ; Save BP

        mov     ax, ds:[si].Y
        mov     bx, ds:[bx].Y
        sub     ax, bx                  ; Get height
        jg      @@T2B                   ; Scan top to bottom
        jl      @@B2T                   ; Scan bottom to top

; Special case: vertical line
        mov     ax, bx
@@V:
        mov     es:[di].Y1, ax
        add     di, SIZE TSCAN
        dec     cx
	jnz	@@V
        jmp     @@Exit

; Scan top to bottom
@@T2B:
        cwd
        div     cx
        mov     bp, ax
        xor     ax, ax
        div     cx
        xchg    ax, bx                  ; BP:BX = fixed 16:16 step
        mov     dx, 8000h
@@T2BLoop:
        mov     es:[di].Y1, ax
        add     di, SIZE TSCAN
        add     dx, bx
        adc     ax, bp
        dec     cx
        jnz     @@T2BLoop
        jmp     @@Exit

; Scan bottom to top
@@B2T:
        neg     ax
        cwd
        div     cx
        mov     bp, ax
        xor     ax, ax
        div     cx
        xchg    ax, bx
        mov     dx, 8000h
@@B2TLoop:
        mov     es:[di].Y1, ax
        add     di, SIZE TSCAN
        sub     dx, bx
        sbb     ax, bp
        dec     cx
        jnz     @@B2TLoop

@@Exit:
        pop     bp                      ; Restore BP
        ret
subScan         ENDP

;-----------------------------------------------------------
;
; Fills a convex polygon with the specified color.
;
; Input:
;       Count   = number of vertexes
;       Map     = indexes of points and colors (integer)
;       Points  = array of points (integer X, Y coordinates)
;       Color   = base color
; Output:
;       none
; Notes:
;       vertexes must be in counterclockwise order, arrays are 0-based.
;
mxFillPoly      PROC FAR
        ARG     Color:WORD,     \
                Points:DWORD,   \
                Map:DWORD,      \
                Count:WORD      = ARG_SIZE
        LOCAL   WritePlane:BYTE:2,      \
                ScanOffsetT:WORD,       \
                ScanOffsetB:WORD,       \
                ScanCount:WORD,         \
                Holder:WORD,            \
                Height:WORD,            \
                MinIdxT:WORD,           \
                MinIdxB:WORD,           \
                MaxIdx:WORD,            \
                Width:WORD,             \
                BoxX1:WORD,             \
                BoxY1:WORD,             \
                BoxX2:WORD,             \
                BoxY2::WORD             = AUTO_SIZE
        .enter  AUTO_SIZE
        .push   ds, si, es, di
        ASSUME  ds:NOTHING

; Check that at least three vertexes are specified
        mov     cx, [Count]
        cmp     cx, 3
        jb      @@Exit

;------------------------------
; Find bounding box for polygon
        les     di, [Map]
        lds     si, [Points]            ; Pointer to vertex array
        mov     [BoxX1], 32767
        mov     [BoxX2], -32768
        mov     [BoxY1], 32767
        mov     [BoxY2], -32768

        xor     dx, dx
@@MinMaxLoop:
        mov     bx, es:[di]             ; Get index of vertex
        .shl    bx, 2                   ; Get offset in point array
        add     bx, si

; Check X range
@@CheckMinX:
        mov     ax, ds:[bx].X           ; Get X coordinate
        cmp     ax, [BoxX1]
        jge     @@CheckMaxX
        mov     [BoxX1], ax
        mov     [MinIdxT], dx
        mov     [MinIdxB], dx
@@CheckMaxX:
        cmp     ax, [BoxX2]
        jle     @@CheckMinY
        mov     [BoxX2], ax
        mov     [MaxIdx], dx

; Check Y range
@@CheckMinY:
        mov     ax, ds:[bx].Y
        cmp     ax, [BoxY1]
        jge     @@CheckMaxY
        mov     [BoxY1], ax
@@CheckMaxY:
        cmp     ax, [BoxY2]
        jle     @@CheckDone
        mov     [BoxY2], ax

; Repeat thru all points
@@CheckDone:
        inc     di                      ; Next map entry
        inc     dx
        inc     di
        inc     dx
        dec     cx
        jnz     @@MinMaxLoop

;---------------------------------
; Check if polygon is full clipped
        mov     ax, [BoxX2]
        cmp     ax, [mx_ClipX1]         ; Is poly full clipped?
        jl      @@Exit
        mov     bx, [BoxX1]
        cmp     bx, [mx_ClipX2]         ; Is poly full clipped?
        jg      @@Exit
        sub     ax, bx                  ; Get width
        jle     @@Exit                  ; Exit if not positive
        mov     ax, [BoxY2]
        cmp     ax, [mx_ClipY1]         ; Is poly full clipped?
        jl      @@Exit
        mov     bx, [BoxY1]
        cmp     bx, [mx_ClipY2]         ; Is poly full clipped?
        jg      @@Exit
        sub     ax, bx                  ; Get height
        jle     @@Exit                  ; Exit if not positive

        dec     [Count]
        shl     [Count], 1              ; We'll work with word offsets
        mov     es, [mx_CodeSegment]

;--------------
; Scan top edge
        mov     ax, OFFSET mx_ScanBuffer
        mov     [ScanOffsetT], ax
        mov     si, [MinIdxT]           ; Offset of bottom point index
@@STLoop:
        lds     bx, [Map]               ; DS:BX -> map table
        mov     di, ds:[bx+si]          ; Index of top point #1
        dec     si                      ; Next point
        dec     si
        test    si, si
        jnl     @@ST1
        mov     si, [Count]
@@ST1:
        mov     [MinIdxT], si           ; Save new index of top point
        mov     si, ds:[bx+si]          ; Get index of top point #2
        .shl    di, 2                   ; Convert indexes to offsets
        .shl    si, 2
        lds     bx, [Points]            ; DS:BX -> point array
        add     si, bx                  ; DS:SI -> top point #2
        add     bx, di                  ; DS:BX -> top point #1
        mov     di, [ScanOffsetT]
        call    subScan                 ; Scan edge
        mov     [ScanOffsetT], di
        mov     si, [MinIdxT]
        cmp     si, [MaxIdx]            ; End of edge?
        jne     @@STLoop                ; No, continue

;-----------------
; Scan bottom edge
        mov     ax, OFFSET mx_ScanBuffer + OFFSET Y2
        mov     [ScanOffsetB], ax
        mov     si, [MinIdxB]           ; Offset of bottom point index
@@SBLoop:
        lds     bx, [Map]               ; DS:BX -> map table
        mov     di, ds:[bx+si]          ; Index of bottom point #1
        inc     si                      ; Next bottom point
        inc     si
        cmp     si, [Count]
        jbe     @@SB1
        xor     si, si
@@SB1:
        mov     [MinIdxB], si           ; Save new index of bottom point
        mov     si, ds:[bx+si]          ; Get index of bottom point #2
        .shl    di, 2                   ; Convert indexes to offsets
        .shl    si, 2
        lds     bx, [Points]            ; DS:BX -> point array
        add     si, bx                  ; DS:SI -> top point #2
        add     bx, di                  ; DS:BX -> top point #1
        mov     di, [ScanOffsetB]
        call    subScan                 ; Scan edge
        mov     [ScanOffsetB], di
        mov     si, [MinIdxB]
        cmp     si, [MaxIdx]            ; End of edge?
        jne     @@SBLoop                ; No, continue

;--------------------
; Clip left and right
        mov     si, OFFSET mx_ScanBuffer
        mov     ax, [BoxX1]
        mov     cx, [BoxX2]
        sub     cx, ax                  ; CX = bounding box width
        mov     bx, [mx_ClipX1]
        sub     bx, ax
        jle     @@ClipL1                ; No need to clip left
        sub     cx, bx                  ; Update width
        add     ax, bx                  ; BoxX1 = mx_ClipX1
        mov     [BoxX1], ax
        .shl    bx, 2                   ; Warning!!! This is an hand-coded
        add     si, bx                  ; multiply by the size of TSCAN
@@ClipL1:
        mov     bx, ax
        add     bx, cx                  ; Last scan column
        sub     bx, [mx_ClipX2]
        jle     @@ClipL2                ; No need to clip right
        sub     cx, bx                  ; Clip right
@@ClipL2:
        test    cx, cx                  ; Is clipped width positive?
        jle     @@Exit                  ; No, exit
        mov     [ScanCount], cx         ; Save number of columns to draw
        mov     [ScanOffsetT], si       ; Remember offset of (clipped) buffer
        mov     ds, [mx_CodeSegment]    ; DS:SI -> scan buffer

;------------------------------
; Check if Y clipping is needed
        mov     ax, [BoxY1]
        cmp     ax, [mx_ClipY1]
        jl      @@ClipTB                ; Need to clip top
        mov     ax, [BoxY2]
        cmp     ax, [mx_ClipY2]
        jg      @@ClipTB                ; Need to clip bottom
        jmp     @@ClipYExit             ; Skip Y clipping

;--------------------
; Clip top and bottom
@@ClipTB:
        mov     di, cx                  ; DI = scan count
        inc     di                      ; Increment count for pre-loop test
        sub     si, SIZE TSCAN
@@ClipYLoop:
        dec     di                      ; Any column left?
        jz      @@ClipYExit             ; No, exit
        add     si, SIZE TSCAN
        mov     ax, ds:[si].Y1          ; Y1
        mov     cx, ds:[si].Y2          ; Y2
        mov     dx, [mx_ClipY2]
        cmp     ax, dx                  ; Full clipped?
        jg      @@ClipYClip             ; Yes, skip this column
        cmp     cx, dx                  ; Need to clip bottom?
        jle     @@ClipY1                ; No, continue
; Clip bottom
        mov     ds:[si].Y2, dx
        mov     bx, cx
        sub     bx, dx                  ; Clip distance
        sub     cx, ax                  ; Height
        jle     @@ClipYClip
        mov     cx, ds:[si].Y2
@@ClipY1:
        mov     dx, [mx_ClipY1]
        cmp     cx, dx                  ; Full top clipped?
        jl      @@ClipYClip             ; Yes, skip
        sub     cx, ax                  ; Get height
        jle     @@ClipYClip             ; Skip if not positive
        cmp     ax, dx                  ; Need to clip top?
        jge     @@ClipYLoop             ; No, continue
; Clip top
        mov     ds:[si].Y1, dx          ; Y1 = mx_ClipY1
        sub     dx, ax                  ; DX = number of pixels clipped
        cmp     cx, dx
        ja      @@ClipYLoop             ; Not clipped, continue
@@ClipYClip:
        mov     ds:[si].Y1, -1          ; Mark column as clipped
        jmp     @@ClipYLoop
@@ClipYExit:

;-------------
; Draw columns
        mov     es, [mx_VideoSegment]
        mov     si, [ScanOffsetT]
        mov     cl, BYTE PTR [BoxX1]    ; Init write plane
        and     cl, 03h
        mov     al, 11h
        shl     al, cl
        mov     [WritePlane], al
        .shr    [BoxX1], 2
@@DrawLoop:
        mov     ax, ds:[si].Y1
        test    ax, ax                  ; Was column clipped?
        js      @@DrawNext              ; Yes, skip
        mov     cx, ds:[si].Y2
        sub     cx, ax                  ; CX = height
        jle     @@DrawNext
        mul     [mx_BytesPerLine]       ; Get pixel address
        add     ax, [BoxX1]
        mov     di, ax
        mov     ah, [WritePlane]
        mov     dx, TS
        mov     al, 02h
        out     dx, ax
        mov     ax, [Color]
        mov     dx, [mx_BytesPerLine]
        shr     cx, 1
        jnc     @@FillScan
        mov     es:[di], al
        add     di, dx
        jcxz    @@DrawNext
@@FillScan:
        mov     es:[di], al
        add     di, dx
        mov     es:[di], al
        add     di, dx
        dec     cx
        jnz     @@FillScan
@@DrawNext:
        rol     [WritePlane], 1
        adc     [BoxX1], 0              ; Bump pointer to video memory if needed
        add     si, SIZE TSCAN
        dec     [ScanCount]
        jnz     @@DrawLoop

@@Exit:
        xor     ax, ax
        .pop    ds, si, es, di
        .leave  ARG_SIZE
mxFillPoly      ENDP

MX_TEXT         ENDS
END
