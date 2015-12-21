;-----------------------------------------------------------
;
; MXFB.ASM - Fill box function
; Copyright (c) 1993,1994 by Alessandro Scotti
;
;-----------------------------------------------------------
WARN    PRO
NOWARN  RES
INCLUDE MODEX.DEF

PUBLIC  mxFillBox

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

EXTRN   mx_BytesPerLine         : WORD
EXTRN   mx_VideoSegment         : WORD
EXTRN   subClipBox              : NEAR
EXTRN   subHorizontalLineInfo   : NEAR

;-----------------------------------------------------------
;
; Raster op functions. Raster op is limited to OP_MOVE,
; OP_AND, OP_OR and OP_XOR. The VGA hardware is used to
; perform the selected logical functions on up to four
; pixel at a time.
;
subRepFill      PROC    NEAR
        mov     ah, al
        shr     cx, 1
        rep     stosw
        rcl     cx, 1
        rep     stosb
        ret
subRepFill      ENDP
;
subFill         PROC    NEAR
@@Loop:
        mov     ds:[bx], al
        add     bx, dx
        loop    @@Loop
        ret
subFill         ENDP
;
subRepMove      PROC    NEAR
        mov     si, di
@@Loop:
        mov     ah, ds:[si]             ; Dummy read to latch the data
        mov     ds:[si], al
        inc     si
        loop    @@Loop
        ret
subRepMove      ENDP
;
subMove         PROC    NEAR
@@Loop:
        mov     ah, ds:[bx]             ; Dummy read to latch the data
        mov     ds:[bx], al
        add     bx, dx
        loop    @@Loop
        ret
subMove         ENDP

;-----------------------------------------------------------
;
; Fills a rectangle with a specified color.
;
; Input:
;       X, Y            = X, Y coordinates of rectangle
;       Width           = width of rectangle
;       Height          = height of rectangle
;       Color           = fill color
;       Op              = raster operator
; Output:
;       none
;
; Note: raster op is limited to OP_MOVE, OP_AND, OP_OR and OP_XOR.
;
mxFillBox       PROC    FAR
        ARG     Op:WORD,                \
                Color:BYTE:2,           \
                Height:WORD,            \
                Width:WORD,             \
                Y:WORD,                 \
                X:WORD                  = ARG_SIZE
        LOCAL   LeftMask:BYTE,          \
                RightMask:BYTE,         \
                FillFunction:WORD,      \
                RepFillFunction:WORD    = AUTO_SIZE
        .enter  AUTO_SIZE
        .push   ds, si, es, di
        ASSUME  ds:NOTHING

; Clip rectangle
        mov     bx, [X]
        mov     ax, [Y]
        mov     cx, [Width]
        mov     dx, [Height]
        call    subClipBox
        jc      @@Exit                  ; Full clipped

        mov     [Height], dx            ; Save clipped height
        call    subHorizontalLineInfo   ; Get additional info, init DI
        mov     [Width], cx
        mov     [LeftMask], al
        mov     [RightMask], ah

; Initialize segments
        mov     ax, [mx_VideoSegment]
        mov     es, ax                  ; ES:DI points to pixel
        mov     ds, ax
        cld                             ; Clear direction flag

; Select fill functions
        mov     [FillFunction], OFFSET subFill
        mov     [RepFillFunction], OFFSET subRepFill
        mov     ax, [Op]                ; Get raster op
        cmp     al, OP_XOR
        ja      @@1                     ; Assume it's a fill
        cmp     al, OP_MOVE
        je      @@1
        .shl    al, 3
        mov     ah, al
        mov     al, 03h
        mov     dx, GDC
        out     dx, ax                  ; Set GDC logical function
        mov     [FillFunction], OFFSET subMove
        mov     [RepFillFunction], OFFSET subRepMove
@@1:

; Fill left block
@@L0:
        mov     ah, [LeftMask]
        or      ah, ah
        jz      @@C0                    ; Nothing to do, go to center block
        mov     dx, TS
        mov     al, 02h
        out     dx, ax                  ; Set write plane mask
        mov     dx, [mx_BytesPerLine]
        mov     cx, [Height]
        mov     bx, di
        mov     al, [Color]
        call    [FillFunction]          ; Fill this column
        inc     di                      ; Update starting video offset

; Fill center block
@@C0:
        mov     cx, [Width]
        jcxz    @@R0                    ; Nothing to do, go to right block
        mov     dx, TS
        mov     ax, 0F02h
        out     dx, ax                  ; Write to all planes
        mov     al, [Color]
        mov     bx, di
        mov     dx, [Height]
        push    di                      ; Save pixel address
@@C1:
        mov     di, bx                  ; Update video offset
        call    [RepFillFunction]       ; Fill current scan line
        mov     cx, [Width]             ; Restore byte count
        add     bx, [mx_BytesPerLine]   ; Bump to next scan line
        dec     dx                      ; Done all lines?
        jnz     @@C1                    ; No, continue
        pop     di                      ; Restore pixel address
        add     di, [Width]             ; Go to right block

; Fill right block
@@R0:
        mov     ah, [RightMask]
        or      ah, ah
        jz      @@Done                  ; Nothing to do, exit
        mov     dx, TS
        mov     al, 02h
        out     dx, ax                  ; Set write plane mask
        mov     dx, [mx_BytesPerLine]
        mov     cx, [Height]
        mov     bx, di
        mov     al, [Color]
        call    [FillFunction]          ; Fill this column

; Restore VGA registers
@@Done:
        mov     dx, GDC
        mov     ax, 0003h
        out     dx, ax                  ; Set logical function to "move"

@@Exit:
        xor     ax, ax
        .pop    ds, si, es, di
        .leave  ARG_SIZE
mxFillBox       ENDP

MX_TEXT         ENDS
END
