;-----------------------------------------------------------
;
; MXOT.ASM - Text functions
; Copyright (c) 1994 by Alessandro Scotti
;
;-----------------------------------------------------------
;WARN    PRO
;NOWARN  RES                             ; We use the reserved name 'WIDTH'
INCLUDE MODEX.DEF

PUBLIC  mxOutChar
PUBLIC  mxOutText
PUBLIC  mxSetFont
PUBLIC  mxSetTextColor
PUBLIC  mxGetTextStep
PUBLIC  mxSetTextStep

MAX_WIDTH       EQU     16              ; Must be <= 16
MAX_HEIGHT      EQU     32

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

EXTRN   mx_CodeSegment  : WORD
EXTRN   mxPutImage      : FAR

; Default 8x8 font
fnt_Default     LABEL
        INCLUDE DEFAULT.FNT

; Table of system fonts
tbl_SystemFont   LABEL   WORD
        DW      fnt_Default, 8, 8

MX_MAXSYSFONT   EQU     ($-OFFSET tbl_SystemFont) SHR 2

mx_FontPtr      DW      OFFSET fnt_Default, SEG MX_TEXT
mx_FontWidth    DW      8               ; Font width in pixels
mx_FontHeight   DW      8               ; Font height in pixels
mx_FontCharSize DW      8               ; Size in bytes of a font character
mx_FontColor    DW      00FFh           ; Color: foreground + background*256
mx_FontOp       DW      OP_MOVE         ; Raster op
mx_DeltaX       DW      8               ; Horizontal step
mx_DeltaY       DW      0               ; Vertical step

;-----------------------------------------------------------
;
; Sets the current font.
;
; Input:
;       Font    = pointer to font data
;       Width   = width of font character in pixels
;       Height  = height of font character in pixels
; Output:
;       AX = 0 on success, else invalid parameters
;
; Note: when the high word of Font (i.e. the segment) is zero, the low
;       word is used to select one of the system fonts.
;
mxSetFont       PROC FAR
        ARG     Height:WORD,    \
                Width:WORD,     \
                Font:DWORD      = ARG_SIZE
        .enter  0
        .push   ds

        mov     ds, [mx_CodeSegment]
        ASSUME  ds:MX_TEXT

        mov     ax, WORD PTR Font[2]    ; Get font segment
        test    ax, ax                  ; Null segment?
        jnz     @@UserFont              ; No, install user font

; Install system font
        mov     ax, WORD PTR Font[0]    ; Get font number
        cmp     ax, MX_MAXSYSFONT       ; Check range
        jb      @@SystemFont
        xor     ax, ax                  ; Out of range, use default font
@@SystemFont:
        shl     ax, 1
        shl     ax, 1
        mov     bx, ax
        mov     ax, tbl_SystemFont[bx]  ; Get font offset
        mov     WORD PTR mx_FontPtr[0], ax
        mov     WORD PTR mx_FontPtr[2], cs
        mov     al, BYTE PTR tbl_SystemFont[bx+2]
        xor     ah, ah
        mov     [mx_FontWidth], ax
        mov     [mx_DeltaX], ax
        mov     dl, BYTE PTR tbl_SystemFont[bx+3]
        xor     dh, dh
        mov     [mx_FontHeight], dx
        mul     dx
        mov     [mx_FontCharSize], ax
        mov     [mx_DeltaX], ax
        xor     ax, ax
        mov     [mx_DeltaY], ax
        jmp     @@Exit

; Install user font
@@UserFont:
        mov     ax, -1                  ; Assume an error
        mov     bx, [Width]
        cmp     bx, MAX_WIDTH
        ja      @@Exit                  ; Invalid character width
        mov     dx, [Height]
        cmp     dx, MAX_HEIGHT
        ja      @@Exit                  ; Invalid character height
        mov     [mx_FontWidth], bx
        mov     [mx_FontHeight], dx
        mov     ax, bx
        add     ax, 7
        .shr    ax, 3
        mul     dx
        mov     [mx_FontCharSize], ax
        mov     ax, WORD PTR Font[0]
        mov     WORD PTR mx_FontPtr[0], ax
        mov     ax, WORD PTR Font[2]
        mov     WORD PTR mx_FontPtr[2], ax
        xor     ax, ax

@@Exit:
        .pop    ds
        ASSUME  ds:NOTHING
        .leave  ARG_SIZE
mxSetFont       ENDP

;-----------------------------------------------------------
;
; Sets the text color and raster op.
;
; Input:
;       Color   = text color (foreground + background*256)
;       Op      = raster op
; Output:
;       none
;
mxSetTextColor  PROC FAR
        ARG     Op:WORD,        \
                Color:WORD      = ARG_SIZE
        .enter  0
        .push   ds

        mov     ds, [mx_CodeSegment]
        ASSUME  ds:MX_TEXT

        mov     ax, [Color]
        mov     [mx_FontColor], ax
        mov     ax, [Op]
        mov     [mx_FontOp], ax

        xor     ax, ax
        .pop    ds
        ASSUME  ds:NOTHING
        .leave  ARG_SIZE
mxSetTextColor  ENDP

;-----------------------------------------------------------
;
; Writes a character using the current font and attributes.
;
; Input:
;       X, Y    = video coordinates
;       C       = character to print
; Output:
;       none
;
mxOutChar       PROC FAR
        ARG     C:BYTE:2,       \
                Y:WORD,         \
                X:WORD          = ARG_SIZE
        LOCAL   Image:BYTE:MAX_WIDTH*MAX_HEIGHT,        \
                Count:WORD                              = AUTO_SIZE
        .enter  AUTO_SIZE
        .push   ds, si, es, di
        ASSUME  ds:NOTHING

; Gets the pointer to font data for the selected character
        lds     si, DWORD PTR [mx_FontPtr]
        mov     al, [C]
        xor     ah, ah
        mul     [mx_FontCharSize]       ; Offset into font
        add     si, ax                  ; DS:SI -> font data for character

; Converts font data into a 256-color linear image
        mov     ax, ss
        mov     es, ax
        lea     di, [Image]
        mov     dx, [mx_FontColor]
        mov     ax, [mx_FontHeight]
        mov     [Count], ax
@@HeightLoop:
        mov     cx, [mx_FontWidth]
        mov     bh, ds:[si]
        inc     si                      ; Get a byte from font data
        cmp     cx, 8
        jbe     @@WidthLoop             ; Ok for width <= 8
        mov     bl, ds:[si]             ; Get another byte
        inc     si
@@WidthLoop:
        mov     al, dl                  ; Assume foreground color
        shl     bx, 1                   ; Is font bit set?
        jc      @@1                     ; Yes, foreground is just great
        mov     al, dh                  ; Get background color
@@1:
        mov     es:[di], al             ; Put pixel into image
        inc     di
        dec     cx
        jnz     @@WidthLoop
        dec     [Count]
        jnz     @@HeightLoop

; Now pass image to mx_PutImage
        lea     ax, [Image]
        push    es
        push    ax                      ; Pointer to image
        push    [X]
        push    [Y]                     ; Image coordinates
        push    [mx_FontWidth]
        push    [mx_FontHeight]         ; Image size
        push    [mx_FontOp]             ; Raster op
        call    mxPutImage              ; Write character

        xor     ax, ax
        .pop    ds, si, es, di
        .leave  ARG_SIZE
mxOutChar       ENDP

;-----------------------------------------------------------
;
; Writes a string at the coordinates specified.
;
; Input:
;       X, Y    = text coordinates
;       S       = pointer to ASCIIZ string
; Output:
;       none
;
mxOutText       PROC FAR
        ARG     S:DWORD,        \
                Y:WORD,         \
                X:WORD          = ARG_SIZE
        .enter  0
        .push   ds, si
        ASSUME  ds:NOTHING

        lds     si, [S]
@@Loop:
        mov     al, ds:[si]
        test    al, al                  ; End of string?
        jz      @@Exit                  ; Yes, exit
        inc     si
        push    [X]                     ; Display character
        push    [Y]
        push    ax
        call    mxOutChar
        mov     ax, [mx_DeltaX]
        add     [X], ax                 ; Bump X coordinate
        mov     ax, [mx_DeltaY]
        add     [Y], ax                 ; Bump Y coordinate
        dec     [Count]
        jnz     @@Loop

@@Exit:
        xor     ax, ax
        .pop    ds, si
        .leave  ARG_SIZE
        ret
mxOutText       ENDP

;-----------------------------------------------------------
;
; Sets the distance between characters.
;
; Input:
;       DeltaX  = horizontal distance in pixels
;       DeltaY  = vertical distance in pixels
; Output:
;       none
;
; Note: this function may be used to set the text direction.
;
mxSetTextStep   PROC FAR
        ARG     DeltaY:WORD,    \
                DeltaX:WORD     = ARG_SIZE
        .enter  0
        .push   ds

        mov     ds, [mx_CodeSegment]
        ASSUME  ds:MX_TEXT

        mov     ax, [DeltaX]
        mov     [mx_DeltaX], ax
        mov     ax, [DeltaY]
        mov     [mx_DeltaY], ax

        .pop    ds
        .leave  ARG_SIZE
mxSetTextStep   ENDP

;-----------------------------------------------------------
;
; Gets the current distance between characters.
;
; Input:
;       DeltaX  = pointer to horizontal distance in pixels (integer)
;       DeltaY  = pointer to vertical distance in pixels (integer)
; Output:
;       none
;
mxGetTextStep   PROC FAR
        ARG     DeltaY:DWORD,   \
                DeltaX:DWORD    = ARG_SIZE
        .enter  0
        .push   ds, si
        ASSUME  ds:NOTHING

        mov     ax, [mx_DeltaX]
        lds     si, [DeltaX]
        mov     ds:[si], ax
        mov     ax, [mx_DeltaY]
        lds     si, [DeltaY]
        mov     ds:[si], ax

        .pop    ds, si
        .leave  ARG_SIZE
mxGetTextStep   ENDP

MX_TEXT         ENDS
END
