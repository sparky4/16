;-----------------------------------------------------------
;
; MXGI.ASM - Get image
; Copyright (c) 1994 by Alessandro Scotti
;
;-----------------------------------------------------------
;WARN    PRO
;NOWARN  RES
INCLUDE MODEX.DEF

PUBLIC  mxGetImage

EXTRN   subClipImage            : NEAR

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

EXTRN   mx_VideoSegment : WORD
EXTRN   mx_BytesPerLine : WORD

;-----------------------------------------------------------
;
; Copies an image from screen to memory.
;
; Input:
;       Image   = pointer to buffer for image
;       X, Y    = coordinates of image on screen
;       Width   = width of image in pixels
;       Height  = height of image in pixels
; Output:
;       none
;
mxGetImage      PROC    FAR
        ARG     Height:WORD,            \
                Width:WORD,             \
                Y:WORD,                 \
                X:WORD,                 \
                Image:DWORD             = ARG_SIZE
        LOCAL   PlaneWidth:WORD:4,      \
                PixelOffset:WORD,       \
                MoveFunction:WORD,      \
                ReadPlane:BYTE,         \
                Count:BYTE              = AUTO_SIZE
        ASSUME  ds:NOTHING
        .enter  AUTO_SIZE
        .push   ds, si, es, di

; Clip image
        mov     bx, [X]
        mov     ax, [Y]
        mov     cx, [Width]
        mov     dx, [Height]
        call    subClipImage
        jc      @@Exit                  ; Full clipped
        mov     [Height], dx
        add     WORD PTR Image[0], si   ; Skip clipped pixels

; Get pixel address
        mul     [mx_BytesPerLine]
        mov     si, bx
        shr     si, 1
        shr     si, 1
        add     si, ax
        mov     [PixelOffset], si
        mov     ds, [mx_VideoSegment]   ; DS:SI points to pixel
        and     bl, 03h
        mov     [ReadPlane], bl

; Compute extra bytes and width count for each plane
        mov     bx, cx
        shr     bx, 1
        shr     bx, 1                   ; Width for each plane
        and     cl, 03h
        mov     al, 00001000b
        shr     al, cl
        mov     di, 3 SHL 1
@@PatchLoop:
        mov     PlaneWidth[di], bx
        shr     al, 1
        adc     bx, 0
        dec     di
        dec     di
        jge     @@PatchLoop

; Get image
        cld
        mov     [Count], 4              ; Four planes
        lea     bx, PlaneWidth          ; SS:[BX] = width in bytes for plane
        mov     es, WORD PTR Image[2]   ; ES:DI will point to image
        mov     ah, [ReadPlane]
@@PlaneLoop:
        cmp     WORD PTR ss:[bx], 0     ; Exit if nothing more to do
        je      @@Exit                  ; (also, never try to move zero bytes!)
        mov     di, WORD PTR Image[0]
        mov     al, 04h
        mov     dx, GDC
        out     dx, ax                  ; Select read plane
        mov     dx, [Height]
        mov     si, [PixelOffset]       ; DS:SI points to video memory
@@Loop:
        .push   si, di
        mov     cx, WORD PTR ss:[bx]    ; Number of bytes to move
@@MoveLoop:
        movsb
        add     di, 3
        dec     cx
        jnz     @@MoveLoop
        .pop    si, di
        add     di, [Width]             ; Go to next image line
        add     si, [mx_BytesPerLine]   ; Go to next screen row
        dec     dx
        jnz     @@Loop                  ; Repeat for all lines
        inc     bx
        inc     bx                      ; Select width for next plane
        inc     ah
        test    ah, 04h                 ; Plane wraparound?
        jz      @@PlaneOk               ; No
        inc     [PixelOffset]           ; Yes, bump video pointer
        and     ah, 03h
@@PlaneOk:
        inc     WORD PTR Image[0]
        dec     [Count]
        jnz     @@PlaneLoop             ; Repeat for all planes

@@Exit:
        xor     ax, ax
        .pop    ds, si, es, di
        .leave  ARG_SIZE
mxGetImage      ENDP

MX_TEXT         ENDS
END
