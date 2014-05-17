;-----------------------------------------------------------
;
; MXTL.ASM - Put tile
; Copyright (c) 1994 by Alessandro Scotti
;
;-----------------------------------------------------------
;WARN    PRO
;NOWARN  RES
INCLUDE MODEX.DEF

PUBLIC  mxPutTile
PUBLIC  mxTransPutTile

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

EXTRN   mx_VideoSegment : WORD
EXTRN   mx_BytesPerLine : WORD

;-----------------------------------------------------------
;
; Copies a "mode-x" tile from memory to screen.
;
; Input:
;       Image   = pointer to tile
;       X, Y    = coordinates of destination
;       Width   = width of image in pixels (Width and 3 = 0)
;       Height  = height of image in pixels
; Output:
;       none
; Note:
;       no clipping is performed on tiles!
;
mxPutTile       PROC    FAR
        ARG     Height:WORD,            \
                Width:WORD,             \
                Y:WORD,                 \
                X:WORD,                 \
                Image:DWORD             = ARG_SIZE
        ASSUME  ds:NOTHING
        .enter  0
        .push   ds, si, es, di

        mov     ax, [Y]                 ; Get pixel address
        mul     [mx_BytesPerLine]
        mov     di, [X]
        .shr    di, 2
        add     di, ax
        mov     es, [mx_VideoSegment]

        lds     si, [Image]             ; Get tile address
        .shr    [Width], 2              ; Number of bytes per plane
        mov     cl, BYTE PTR [X]
        and     cl, 3
        mov     ah, 11h                 ; AH = plane mask
        shl     ah, cl                  ; Align mask to first plane

        mov     [Y], 4                  ; Number of planes
        mov     bx, [mx_BytesPerLine]
        sub     bx, [Width]             ; Extra bytes per line
@@Loop:
        mov     al, 02h
        mov     dx, TS
        out     dx, ax                  ; Set write plane
        mov     [X], di                 ; Save video offset
        mov     dx, [Height]
@@Loop2:
        mov     cx, [Width]             ; Number of bytes to move

        shr     cx, 1                   ; Move line
        rep     movsw
        rcl     cx, 1
        rep     movsb

        add     di, bx                  ; Move video offset to next line
        dec     dx                      ; Done all lines?
        jnz     @@Loop2                 ; No, continue
        mov     di, [X]                 ; Restore video offset
        rol     ah, 1                   ; Next plane
        adc     di, 0                   ; Bump video offset if needed
        dec     [Y]                     ; Any plane left?
        jnz     @@Loop                  ; Yes, keep looping

        xor     ax, ax
        .pop    ds, si, es, di
        .leave  ARG_SIZE
mxPutTile       ENDP

;-----------------------------------------------------------
;
; Copies a "mode-x" tile from memory to screen.
; Skips over color 0.
;
; Input:
;       Image   = pointer to tile
;       X, Y    = coordinates of destination
;       Width   = width of image in pixels (Width and 3 = 0)
;       Height  = height of image in pixels
; Output:
;       none
; Note:
;       no clipping is performed on tiles!
;
mxTransPutTile  PROC    FAR
        ARG     Height:WORD,            \
                Width:WORD,             \
                Y:WORD,                 \
                X:WORD,                 \
                Image:DWORD             = ARG_SIZE
        ASSUME  ds:NOTHING
        .enter  0
        .push   ds, si, es, di

        mov     ax, [Y]                 ; Get pixel address
        mul     [mx_BytesPerLine]
        mov     di, [X]
        .shr    di, 2
        add     di, ax
        mov     es, [mx_VideoSegment]

        lds     si, [Image]             ; Get tile address
        .shr    [Width], 2              ; Number of bytes per plane
        mov     cl, BYTE PTR [X]
        and     cl, 3
        mov     ah, 11h                 ; AH = plane mask
        shl     ah, cl                  ; Align mask to first plane

        mov     [Y], 4                  ; Number of planes
        mov     bx, [mx_BytesPerLine]
        sub     bx, [Width]             ; Extra bytes per line
@@Loop:
        mov     al, 02h
        mov     dx, TS
        out     dx, ax                  ; Set write plane
        mov     [X], di                 ; Save video offset
        mov     dx, [Height]
@@Loop2:
        mov     cx, [Width]             ; Number of bytes to move

; Move one line
        jcxz    @@MoveLineDone
@@MoveLineLoop:
        mov     al, ds:[si]
        test    al, al
        jz      @@MoveLineNext
        mov     es:[di], al
@@MoveLineNext:
        inc     si
        inc     di
        dec     cx
        jnz     @@MoveLineLoop
@@MoveLineDone:

        add     di, bx                  ; Move video offset to next line
        dec     dx                      ; Done all lines?
        jnz     @@Loop2                 ; No, continue
        mov     di, [X]                 ; Restore video offset
        rol     ah, 1                   ; Next plane
        adc     di, 0                   ; Bump video offset if needed
        dec     [Y]                     ; Any plane left?
        jnz     @@Loop                  ; Yes, keep looping

        xor     ax, ax
        .pop    ds, si, es, di
        .leave  ARG_SIZE
mxTransPutTile  ENDP

MX_TEXT         ENDS
END
