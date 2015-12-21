;-----------------------------------------------------------
;
; MXEL.ASM - Mid-point ellipse
; Copyright (c) 1994 by Alessandro Scotti
;
;-----------------------------------------------------------
WARN    PRO
NOWARN  RES
INCLUDE MODEX.DEF

PUBLIC  mxEllipse

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
; Draws an ellipse using the mid-point algorithm.
;
; Input:
;       XC, YC  = center coordinates
;       A       = horizontal radius
;       B       = vertical radius
;       Color   = ellipse color
; Output:
;       none
; Note:
;       computes only points in the first quadrant, all other
;       points are obtained by symmetry.
;
mxEllipse       PROC FAR
        ARG     Color:BYTE:2,   \
                B:WORD,         \
                A:WORD,         \
                YC:WORD,        \
                XC:WORD         = ARG_SIZE
        LOCAL   A2:DWORD,       \
                B2:DWORD,       \
                CC:DWORD,       \
                DD:DWORD,       \
                X:DWORD,        \
                Y:DWORD         = AUTO_SIZE
        .enter  AUTO_SIZE
        .push   ds, si, di

        .chk386 mxEllipse, @@Exit

; Initialize variables
        xor     eax, eax
        mov     [X], ax         ; X = 0
        mov     ax, [A]
        mul     eax             ; EAX = A*A
        mov     [DD], eax       ; DD = A*A
        shl     eax, 1
        mov     [CC], eax       ; CC = 2*A*A
        shl     eax, 1
        mov     [A2], eax       ; A2 = 4*A*A
        movzx   edx, [B]
        mov     [Y], edx
        mul     edx             ; EAX = 4*A*A*B
        sub     [DD], eax       ; DD = A*A - 4*A*A*B
        movzx   eax, [B]
        mul     eax             ; EAX = B*B
        shl     eax, 2          ; EAX = 4*B*B
        mov     [B2], eax       ; B2 = 4*B*B
        add     [CC], eax       ; CC = 2*A*A + 4*B*B
        add     [D1], eax       ; DD = A*A - 4*A*A*B + 4*B*B

; Draw initial points
        call    subPlot4

; First loop
@@Loop1:
        mov     eax, [X]        ; Check slope
        mul     [B2]
        mov     ecx, eax
        mov     eax, [Y]
        mul     [A2]
        sub     eax, ecx
        sub     eax, [CC]       ; EAX = Y*A2 - X*B2 - CC
        jle     @@Done1         ; Crossed critical point, jump to next loop

        mov     ecx, [DD]       ; Get error
        test    ecx, ecx        ; Positive?
        jl      @@Draw1         ; No, use default step

        mov     eax, 1
        sub     eax, [Y]
        mul     [A2]
        shl     eax, 1
        add     ecx, eax        ; Bump error
        dec     WORD PTR [Y]    ; Decrement Y coordinate

@@Draw1:
        mov     eax, [X]
        shl     eax, 1
        add     eax, 3
        mul     [B2]
        add     ecx, eax        ; Bump error
        mov     [DD], ecx       ; Save error
        inc     WORD PTR [X]    ; Increment X coordinate

        call    subPlot4        ; Draw points
        jmp     @@Loop1
@@Done1:

; Initialize variables
        shr     [B2], 2
        mov     eax, [X]
        mul     eax
        add     [X]
        shl     eax, 2
        inc     eax
        mul     [B2]
        mov     [DD], eax
        mov     eax, [Y]
        mul     eax
        add     [Y]
        dec     eax
        add     [Y]
        sub     eax, [B2]
        add     [DD], eax
        shl     [B2], 3
        inc     WORD PTR [X]

; Second loop
@@Loop2:
        cmp     WORD PTR [Y], 0
        ja      @@Done2

        mov     ecx, [DD]
        test    ecx, ecx
        jge     @@Draw2

        mov     eax, [X]
        inc     eax
        mul     [B2]
        add     ecx, eax
        inc     WORD PTR [X]

@@Draw2:
        mov     eax, [Y]
        shl     eax, 1
        sub     eax, 3
        neg     eax
        imul    [A2]
        add     ecx, eax
        dec     WORD PTR [Y]

        call    subPlot4
        jmp     @@Loop2
@@Done2:

@@Exit:
        xor     ax, ax
        .leave  ARG_SIZE
mxEllipse       ENDP

MX_TEXT         ENDS
END
