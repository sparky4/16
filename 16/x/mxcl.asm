;-----------------------------------------------------------
;
; MXCL.ASM - Bresenham circle
; Copyright (c) 1993,1994 by Alessandro Scotti
;
;-----------------------------------------------------------
WARN    PRO
NOWARN  RES
INCLUDE MODEX.DEF

PUBLIC  mxCircle

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

EXTRN   mx_BytesPerLine : WORD
EXTRN   mx_VideoSegment : WORD
EXTRN   mx_ClipX1       : WORD
EXTRN   mx_ClipY1       : WORD
EXTRN   mx_ClipX2       : WORD
EXTRN   mx_ClipY2       : WORD

;-----------------------------------------------------------
;
; Draws a circle using the Bresenham algorithm.
;
; Input:
;       XC, YC  = center coordinates
;       Radius  = circle radius
;       Color   = circle color
; Output:
;       none
; Note:
;       computes only points in the first octant, all other
;       points are obtained by symmetry.
;
mxCircle        PROC FAR
        ARG     Color:BYTE:2,           \
                Radius:WORD,            \
                YC:WORD,                \
                XC:WORD                 = ARG_SIZE
        LOCAL   Delta:WORD              = AUTO_SIZE
        .enter  AUTO_SIZE
        .push   ds, si, di

        xor     si, si                  ; X
        mov     di, [Radius]            ; Y
        mov     ax, 3
        sub     ax, di
        sub     ax, di
        mov     [Delta], ax             ; Delta = 3-R*2

        mov     ds, [mx_VideoSegment]

@@Loop:
        cmp     si, di                  ;
        jg      @@Done                  ; Exit when X > Y
; Draw points
        mov     ax, si
        mov     bx, di
        call    @@subPutPixel
        mov     ax, si
        neg     ax
        mov     bx, di
        call    @@subPutPixel
        mov     ax, si
        mov     bx, di
        neg     bx
        call    @@subPutPixel
        mov     ax, si
        neg     ax
        mov     bx, di
        neg     bx
        call    @@subPutPixel
        mov     ax, di
        mov     bx, si
        call    @@subPutPixel
        mov     ax, di
        neg     ax
        mov     bx, si
        call    @@subPutPixel
        mov     ax, di
        mov     bx, si
        neg     bx
        call    @@subPutPixel
        mov     ax, di
        neg     ax
        mov     bx, si
        neg     bx
        call    @@subPutPixel
; Moves coordinates to next point
        mov     ax, [Delta]
        test    ax, ax
        jl      @@Next
        mov     ax, di
        .shl    ax, 2
        sub     ax, 4
        sub     [Delta], ax
        dec     di
@@Next:
        mov     ax, si
        .shl    ax, 2
        add     ax, 6
        add     [Delta], ax
        inc     si
        jmp     @@Loop

@@Done:
        xor     ax, ax
        .pop    ds, si, di
        .leave  ARG_SIZE

;---------------------------------------
; Put pixel function.
; Input:
;       BX      = X coordinate (relative to center)
;       AX      = Y coordinate (relative to center)
;       DS      = video segment
@@subPutPixel:
        add     bx, [XC]                ; Get absolute coordinates
        add     ax, [YC]

        cmp     bx, [mx_ClipX1]         ; Clip pixel
        jl      @@subExit
        cmp     bx, [mx_ClipX2]
        jg      @@subExit
        cmp     ax, [mx_ClipY1]
        jl      @@subExit
        cmp     ax, [mx_ClipY2]
        jg      @@subExit

        mul     [mx_BytesPerLine]       ; Get pixel offset
        mov     cx, bx                  ; Save X coordinate
        .shr    bx, 2
        add     bx, ax                  ; DS:BX = pixel offset

        and     cl, 3                   ; Set write plane
        mov     ax, 0102h
        shl     ah, cl
        mov     dx, TS
        out     dx, ax

        mov     al, [Color]             ; Write pixel
        mov     ds:[bx], al

@@subExit:
        retn
mxCircle        ENDP

MX_TEXT         ENDS
END
