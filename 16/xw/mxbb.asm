;-----------------------------------------------------------
;
; MXBB.ASM - Bit block transfer
; Copyright (c) 1993,1994 by Alessandro Scotti
;
;-----------------------------------------------------------
;WARN    PRO
;NOWARN  RES
INCLUDE modex.def

PUBLIC  mxBitBlt

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

EXTRN   subHorizontalLineInfo   : NEAR

EXTRN   mx_VideoSegment : WORD
EXTRN   mx_BytesPerLine : WORD

;-----------------------------------------------------------
;
; Moves a block of video memory.
;
; Input:
;       SX, SY  = source coordinates
;       Width   = source width
;       Height  = source height
;       DX, DY  = destination coordinates
; Output:
;       none
;
; Note: overlapping regions are not allowed.
;
mxBitBlt        PROC    FAR
        ARG     DestY:WORD,             \
                DestX:WORD,             \
                Height:WORD,            \
                Width:WORD,             \
                SY:WORD,                \
                SX:WORD                 = ARG_SIZE
        LOCAL   PlaneWidth:WORD:4,      \
                SourceOffset:WORD,      \
                DestOffset:WORD,        \
                Count:BYTE,             \
                ReadPlane:BYTE,         \
                WritePlane:BYTE,        \
                LeftMask:BYTE,          \
                RightMask:BYTE          = AUTO_SIZE
	.enter  AUTO_SIZE
        .push   ds, si, es, di

; Exit now if null width
        cmp     [Width], 0
        je      @@Exit

; Calls the proper procedure to handle transfer
        mov     ax, [SX]
        and     al, 03h                 ; AL = source plane
        mov     dx, [DestX]
        and     dl, 03h                 ; DL = destination plane
        mov     bx, OFFSET subPlaneBlt
        cmp     al, dl                  ; Same source and destination plane?
        jne     @@BitBlt                ; No, use slow procedure
        mov     bx, OFFSET subMoveBlt
@@BitBlt:
        call    bx

@@Exit:
        xor     ax, ax
        .pop    ds, si, es, di
        .leave  ARG_SIZE

;-----------------------------------------------------------
; Uses write mode 1 for maximum speed. Only works if source
; and destination are plane-aligned.
;
subMoveBlt      PROC    NEAR
; Get horizontal line info and address of destination
        mov     bx, [DestX]
        mov     ax, [DestY]
        mov     cx, [Width]
        call    subHorizontalLineInfo
        mov     [LeftMask], al
        mov     [RightMask], ah
        mov     [Width], cx

; Setup segments
        mov     ax, [mx_VideoSegment]
        mov     ds, ax
        mov     es, ax

; Get address of source pixel
        mov     ax, [SY]
        mul     [mx_BytesPerLine]
        mov     si, [SX]
        .shr    si, 2
        add     si, ax

; Set write mode 1
        mov     dx, GDC
        mov     ax, 4105h
        out     dx, ax
        cld

; Move left block
@@L0:
        mov     ah, [LeftMask]
        or      ah, ah
        jz      @@C0
        mov     dx, TS
        mov     al, 02h
        out     dx, ax                  ; Set write plane mask
        mov     ax, [mx_BytesPerLine]
        dec     ax
        mov     cx, [Height]
        .push   si, di
@@L1:
        movsb
        add     si, ax
        add     di, ax
        dec     cx
        jnz     @@L1
        .pop    si, di
        inc     si
        inc     di

; Move center block
@@C0:
        mov     bx, [Width]
        test    bx, bx
        jz      @@R0
        mov     dx, TS
        mov     ax, 0F02h
        out     dx, ax                  ; Enable all planes
        mov     ax, [mx_BytesPerLine]
        sub     ax, bx
        mov     dx, [Height]
        .push   si, di
@@C1:
        mov     cx, bx                  ; CX = image width
        rep     movsb                   ; Cannot use "movsw" here!
        add     si, ax                  ; Next scan line
        add     di, ax                  ; Next scan line
        dec     dx                      ; All lines processed?
        jnz     @@C1                    ; No, continue
        .pop    si, di
        add     si, bx
        add     di, bx

; Move right block
@@R0:
        mov     ah, [RightMask]
        or      ah, ah
        jz      @@Done
        mov     dx, TS
        mov     al, 02h
        out     dx, ax                  ; Set write plane mask
        mov     ax, [mx_BytesPerLine]
        dec     ax
        mov     cx, [Height]
@@R1:
        movsb
        add     si, ax
        add     di, ax
        dec     cx
        jnz     @@R1

@@Done:
        mov     dx, GDC
        mov     ax, 4005h
        out     dx, ax                  ; Restore write mode 0

@@Exit:
        ret
subMoveBlt      ENDP

;-----------------------------------------------------------
; Moves one plane at a time.
;
subPlaneBlt     PROC    NEAR
; Compute extra bytes and width count for each plane
        mov     cx, [Width]
        mov     bx, cx
        shr     bx, 1
        shr     bx, 1                   ; Width for each plane
        and     cl, 03h
        mov     al, 00001000b
        shr     al, cl
        mov     si, 3 SHL 1
@@PatchLoop:
        mov     PlaneWidth[si], bx
        shr     al, 1
        adc     bx, 0
        dec     si
        dec     si
        jge     @@PatchLoop

; Get pixel addresses
        mov     ax, [mx_VideoSegment]
        mov     ds, ax
        mov     es, ax
        mov     ax, [SY]
        mul     [mx_BytesPerLine]
        mov     si, [SX]
        shr     si, 1
        shr     si, 1
        add     si, ax                  ; DS:SI points to source
        mov     [SourceOffset], si
        mov     ax, [DestY]
        mul     [mx_BytesPerLine]
        mov     di, [DestX]
        shr     di, 1
        shr     di, 1
        add     di, ax                  ; ES:DI points to destination
        mov     [DestOffset], di

; Adjust plane for output to VGA registers
        mov     ax, [SX]
        and     al, 03h
        mov     [ReadPlane], al
        mov     cx, [DestX]
        and     cl, 03h
        mov     al, 00010001b
        shl     al, cl
        mov     [WritePlane], al

; Ready to move now
        cld
        mov     [Count], 4              ; Four planes
        lea     bx, PlaneWidth          ; SS:[BX] = width in bytes for plane
@@PlaneLoop:
        cmp     WORD PTR ss:[bx], 0
        je      @@Done
        mov     ah, [WritePlane]
        and     ah, 0Fh
        mov     al, 02h
        mov     dx, TS
        out     dx, ax                  ; Select write plane
        mov     ah, [ReadPlane]
        mov     al, 04h
        mov     dx, GDC
        out     dx, ax                  ; Select read plane
        mov     dx, [Height]
        mov     ax, [mx_BytesPerLine]
        sub     ax, ss:[bx]             ; AX = extra bytes per line
@@Loop:
        mov     cx, ss:[bx]
        shr     cx, 1
        rep     movsw
        rcl     cx, 1
        rep     movsb
        add     si, ax
        add     di, ax
        dec     dx
        jnz     @@Loop                  ; Repeat for all lines
        inc     bx
        inc     bx                      ; Select width for next plane
        inc     [ReadPlane]
        and     [ReadPlane], 03h        ; Should be faster on 386 using
        jnz     @@ReadPlaneOk           ; BTR and ADC...
        inc     [SourceOffset]
@@ReadPlaneOk:
        rol     [WritePlane], 1
        adc     [DestOffset], 0
        mov     si, [SourceOffset]
        mov     di, [DestOffset]
        dec     [Count]
        jnz     @@PlaneLoop             ; Repeat for all planes

@@Done:
        ret
subPlaneBlt     ENDP

mxBitBlt        ENDP

MX_TEXT         ENDS
END
