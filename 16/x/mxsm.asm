;-----------------------------------------------------------
;
; MXSM.ASM - Set/change mode functions
; Copyright (c) 1993,1994 by Alessandro Scotti
;
;-----------------------------------------------------------
WARN    PRO
INCLUDE MODEX.DEF

PUBLIC  mxChangeMode
PUBLIC  mxGetAspect
PUBLIC  mxGetScreenSize
PUBLIC  mxSetMode

PUBLIC  mx_ScreenWidth
PUBLIC  mx_ScreenHeight
PUBLIC  mx_BytesPerLine

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

EXTRN   mxSetSysClipRegion      : FAR

EXTRN   mx_VideoSegment : WORD
EXTRN   mx_CodeSegment  : WORD

mx_ScreenWidth  DW      ?               ; Current screen width
mx_ScreenHeight DW      ?
mx_AspectX      DW      ?               ; Aspect ratio for current mode
mx_AspectY      DW      ?
mx_BytesPerLine DW      0               ; Bytes per line

;
; Tables for setting video modes, sources:
; - MODEX.ASM, Matt Pritchard
; - Dr. Dobb's Journal, Michael Abrash
; - Fractint VIDEO.ASM module
;
TBL_SingleLine  LABEL   WORD            ; CRTC
        DW      04009h                  ; Cell height: 1 scan line
        DW      00014h                  ; Double word mode off
        DW      0E317h                  ; Byte mode on
        DW      0
TBL_DoubleLine  LABEL   WORD            ; CRTC
        DW      04109h                  ; Cell height: 2 scan lines
        DW      00014h
        DW      0E317h
        DW      0
TBL_Width320    LABEL   WORD            ; CRTC
        DW      05F00h                  ; Horizontal total
        DW      04F01h                  ; Horizontal displayed
        DW      05002h                  ; Start horizontal blanking
        DW      08203h                  ; End horizontal blanking
        DW      05404h                  ; Start horizontal sync
        DW      08005h                  ; End horizontal sync
        DW      02813h                  ; Row address
        DW      0
TBL_Width360    LABEL   WORD            ; CRTC
        DW      06B00h                  ; Horizontal total
        DW      05901h                  ; Horizontal displayed
        DW      05A02h                  ; Start horizontal blanking
        DW      08E03h                  ; End horizontal blanking
        DW      05E04h                  ; Start horizontal sync
        DW      08A05h                  ; End horizontal sync
        DW      02D13h                  ; Row address
        DW      0
TBL_Height175   LABEL   WORD            ; CRTC
        DW      0BF06h                  ; Vertical total
        DW      01F07h                  ; Overflow
        DW      08310h                  ; Start vertical sync
        DW      08511h                  ; End vertical sync
        DW      05D12h                  ; Vertical displayed
        DW      06315h                  ; Start vertical blanking
        DW      0BA16h                  ; End vertical blanking
        DW      0
TBL_Height200   LABEL   WORD            ; CRTC
        DW      0BF06h                  ; Vertical total
        DW      01F07h                  ; Overflow
        DW      09C10h                  ; Start vertical sync
        DW      08E11h                  ; End vertical sync
        DW      08F12h                  ; Vertical displayed
        DW      09615h                  ; Start vertical blanking
        DW      0B916h                  ; End vertical blanking
        DW      0
TBL_Height240   LABEL   WORD            ; CRTC
        DW      00D06h                  ; Vertical total
        DW      03E07h                  ; Overflow
        DW      0EA10h                  ; Start vertical sync
        DW      08C11h                  ; End vertical sync
        DW      0DF12h                  ; Vertical displayed
        DW      0E715h                  ; Start vertical blanking
        DW      00616h                  ; End vertical blanking
        DW      0
TBL_Tweak400x600:
        DW      07400h
        DW      06301h
        DW      06402h
        DW      09703h
        DW      06804h
        DW      09505h
        DW      08606h
        DW      0F007h
        DW      06009h
        DW      0310Fh
        DW      05B10h
        DW      08D11h
        DW      05712h
        DW      03213h
        DW      00014h
        DW      06015h
        DW      08016h
        DW      0E317h
        DW      0

TBL_320x200:
        DB      63h                     ; 400 scan lines, 25 MHz clock
        DW      6, 5                    ; Aspect: 6/5 = 1.2:1
        DW      320, 200                ; Size
        DW      TBL_Width320, TBL_Height200, TBL_DoubleLine, 0
        DW      819                     ; Max height
TBL_320x240:
        DB      0E3h                    ; 400 scan lines, 25 MHz clock
        DW      1, 1                    ; Aspect: 1/1 = 1:1
        DW      320, 240                ; Size
        DW      TBL_Width320, TBL_Height240, TBL_DoubleLine, 0
        DW      819                     ; Max height
TBL_320x400:
        DB      63h                     ; 480 scan lines, 25 MHz clock
        DW      6, 10                   ; Aspect: 6/10 = 0.6:1
        DW      320, 400                ; Size
        DW      TBL_Width320, TBL_Height200, TBL_SingleLine, 0
        DW      819                     ; Max height
TBL_320x480:
        DB      0E3h                    ; 480 scan lines, 25 MHz clock
        DW      1, 2                    ; Aspect: 1/2 = 0.5:1
        DW      320, 480                ; Size
        DW      TBL_Width320, TBL_Height240, TBL_SingleLine, 0
        DW      819                     ; Max height
TBL_360x200:
        DB      067h                    ; 400 scan lines, 28 MHz clock
        DW      27, 20                  ; Aspect: 27/20 = 1.35:1
        DW      360, 200                ; Size
        DW      TBL_Width360, TBL_Height200, TBL_DoubleLine, 0
        DW      728                     ; Max height
TBL_360x240:
        DB      0E7h                    ; 480 scan lines, 28 MHz clock
        DW      9, 8                    ; Aspect: 9/8 = 1.125:1
        DW      360, 240                ; Size
        DW      TBL_Width360, TBL_Height240, TBL_DoubleLine, 0
        DW      728                     ; Max height
TBL_360x400:
        DB      067h                    ; 400 scan lines, 28 MHz clock
        DW      27, 40                  ; Aspect: 27/40 = 0.675:1
        DW      360, 400                ; Size
        DW      TBL_Width360, TBL_Height200, TBL_SingleLine, 0
        DW      728                     ; Max height
TBL_360x480:
        DB      0E7h                    ; 480 scan lines, 28 MHz clock
        DW      9, 16                   ; Aspect: 9/16 = 0.5625:1
        DW      360, 480                ; Size
        DW      TBL_Width360, TBL_Height240, TBL_SingleLine, 0
        DW      728                     ; Max height
TBL_320x175:
        DB      0A3h
        DW      0, 0                    ; Aspect:
        DW      320, 175
        DW      TBL_Width320, TBL_Height175, TBL_DoubleLine, 0
        DW      819
TBL_320x350:
        DB      0A3h
        DW      0, 0                    ; Aspect:
        DW      320, 175
        DW      TBL_Width320, TBL_Height175, TBL_SingleLine, 0
        DW      819
TBL_360x175:
        DB      0A7h
        DW      0, 0                    ; Aspect:
        DW      360, 480                ; Size
        DW      TBL_Width360, TBL_Height175, TBL_DoubleLine, 0
        DW      728                     ; Max height
TBL_360x350:
        DB      0A7h
        DW      0, 0                    ; Aspect:
        DW      360, 480                ; Size
        DW      TBL_Width360, TBL_Height175, TBL_SingleLine, 0
        DW      728                     ; Max height
TBL_400x600:
        DB      0E7h                    ; 28 MHz clock
        DW      1, 2                    ; Aspect: 1/2 = 0.5:1
        DW      400, 600                ; Size
        DW      TBL_Tweak400x600, 0
        DW      655                     ; Max height

TBL_Mode        LABEL   WORD
        DW      TBL_320x175
        DW      TBL_320x200
        DW      TBL_320x240
        DW      TBL_320x350
        DW      TBL_320x400
        DW      TBL_320x480
        DW      TBL_360x175
        DW      TBL_360x200
        DW      TBL_360x240
        DW      TBL_360x350
        DW      TBL_360x400
        DW      TBL_360x480
        DW      TBL_400x600

MAXVMODE        EQU     ($-OFFSET TBL_Mode) / 2

;-----------------------------------------------------------
;
; Enables 80x25 color text mode
;
subText         PROC    NEAR
        ASSUME  ds:MX_TEXT
        mov     ax, 0003h
        int     10h                     ; Call BIOS set mode

        mov     [mx_ScreenHeight], 0
        mov     [mx_BytesPerLine], 0
        ret
subText         ENDP

;-----------------------------------------------------------
;
; Enables the selected graphics mode.
;
; Input:
;       Mode    = mode to select (MX_???x???)
; Output:
;       none
;
mxSetMode       PROC    FAR
        ARG     Mode:WORD       = ARG_SIZE
        ASSUME  ds:NOTHING
        .enter  0
        .push   ds, si, es, di

; Set DS to code segment alias
        mov     ds, [mx_CodeSegment]
        ASSUME  ds:MX_TEXT

        mov     si, [Mode]
        cmp     si, MAXVMODE            ; Is it a valid mode?
        ja      @@Exit                  ; No, exit
        test    si, si                  ; Text mode?
        jnz     @@Set                   ; No, handle it

        call    subText                 ; Back to text mode
        jmp     @@Exit                  ; Exit now

; Set video mode
@@Set:
        dec     si                      ; Skip text mode
        shl     si, 1
        mov     si, TBL_Mode[si]
        cld

; Use BIOS to set 320x200x256 linear mode
        push    si                      ; Save SI
        mov     ax, 0013h
        int     10h                     ; Use BIOS to set 320x200 linear mode
        pop     si                      ; Restore SI

        mov     dx, TS
        mov     ax, 0604h
        out     dx, ax                  ; Disable chain-4 mode
        mov     ax, 0100h
        out     dx, ax                  ; Reset
        mov     dx, MISC
        lodsb
        out     dx, al                  ; New timing/size
        mov     dx, TS
        mov     ax, 0300h
        out     dx, ax                  ; Restart sequencer

; Unlock CRTC registers 0-7
        mov     dx, CRTC
        mov     al, 11h
        out     dx, al                  ; Vertical sync end register
        inc     dx
        in      al, dx
        and     al, 7Fh                 ; Clear write protect bit
        out     dx, al

        lodsw                           ; Get X aspect
        mov     [mx_AspectX], ax
        lodsw                           ; Get Y aspect
        mov     [mx_AspectY], ax
        lodsw                           ; Get screen width
        mov     [mx_ScreenWidth], ax
        shr     ax, 1
        shr     ax, 1                   ; Divide by four to get bytes per line
        mov     [mx_BytesPerLine], ax
        lodsw                           ; Get screen height
        mov     [mx_ScreenHeight], ax

; Set CRTC registers
        mov     bx, si
        mov     dx, CRTC
@@TableLoop:
        mov     si, ds:[bx]             ; DS:SI -> table of CRTC registers
        inc     bx
        inc     bx                      ; DS:BX -> offset of next table
        test    si, si                  ; Last table?
        jz      @@EndLoop               ; Yes, exit loop
@@Loop:
        lodsw                           ; Get CRTC register index and value
        test    ax, ax                  ; End of table?
        jz      @@TableLoop             ; Yes, go to next table
        out     dx, ax                  ; Set register AL to value AH
        jmp     @@Loop                  ; Get next register/value
@@EndLoop:

; Set virtual screen and system clip region
        push    [mx_ScreenWidth]
        push    WORD PTR ds:[bx]
        call    mxSetSysClipRegion

; Clear video memory
        mov     dx, TS
        mov     ax, 0F02h
        out     dx, ax                  ; Enable all planes
        mov     es, [mx_VideoSegment]
        xor     di, di
        mov     cx, 8000h
        xor     ax, ax
        rep     stosw

@@Done:
; Lock CRTC registers 0-7 (some cards need this)
        mov     dx, CRTC
        mov     al, 11h
        out     dx, al                  ; Vertical sync end register
        inc     dx
        in      al, dx
        or      al, 80h                 ; Set write protect bit
        out     dx, al

@@Exit:
        xor     ax, ax
        mov     ax, [mx_ScreenWidth]
        .pop    ds, si, es, di
        .leave  ARG_SIZE
mxSetMode       ENDP

;-----------------------------------------------------------
;
; Changes from the current mode the selected graphics mode.
;
; Input:
;       Mode    = mode to select (MX_???x???)
; Output:
;       none
; Notes:
;       this function assumes that mxSetMode and mxSetVirtualScreen
;       have been called first. View size is rearranged to match the
;       specified mode, but video memory is not cleared.
;       Differences from mxSetMode:
;       - video BIOS is not called to initialize graphics;
;       - row address register is not modified;
;       - video memory is not cleared;
;       - mx_BytesPerLine is not modified;
;       - system clip region is not modified.
;
mxChangeMode    PROC    FAR
        ARG     Mode:WORD       = ARG_SIZE
        ASSUME  ds:NOTHING
        .enter  0
        .push   ds, si, es, di

; Set DS to code segment alias
        mov     ds, [mx_CodeSegment]
        ASSUME  ds:MX_TEXT

        mov     si, [Mode]
        cmp     si, MAXVMODE            ; Is it a valid mode?
        ja      @@Exit                  ; No, exit
        test    si, si                  ; Text mode?
        jz      @@Exit                  ; Yes, exit

        dec     si                      ; Skip text mode
        shl     si, 1
        mov     si, TBL_Mode[si]
        cld

        mov     dx, TS
        mov     ax, 0604h
        out     dx, ax                  ; Disable chain-4 mode
        mov     ax, 0100h
        out     dx, ax                  ; Reset
        mov     dx, MISC
        lodsb
        out     dx, al                  ; New timing/size
        mov     dx, TS
        mov     ax, 0300h
        out     dx, ax                  ; Restart sequencer

; Unlock CRTC registers 0-7
        mov     dx, CRTC
        mov     al, 11h
        out     dx, al                  ; Vertical sync end register
        inc     dx
        in      al, dx
        and     al, 7Fh                 ; Clear write protect bit
        out     dx, al

        lodsw                           ; Get X aspect
        mov     [mx_AspectX], ax
        lodsw                           ; Get Y aspect
        mov     [mx_AspectY], ax
        lodsw                           ; Get screen width
        mov     [mx_ScreenWidth], ax
        lodsw                           ; Get screen height
        mov     [mx_ScreenHeight], ax

; Set CRTC registers
        mov     bx, si
        mov     dx, CRTC
@@TableLoop:
        mov     si, ds:[bx]             ; DS:SI -> table of CRTC registers
        inc     bx
        inc     bx                      ; DS:BX -> offset of next table
        test    si, si                  ; Last table?
        jz      @@EndLoop               ; Yes, exit loop
@@Loop:
        lodsw                           ; Get CRTC register index and value
        test    ax, ax                  ; End of table?
        jz      @@TableLoop             ; Yes, go to next table
        cmp     al, 13h                 ; Row address register?
        je      @@Loop                  ; Yes, ignore it
        out     dx, ax                  ; Set register AL to value AH
        jmp     @@Loop                  ; Get next register/value
@@EndLoop:

; Lock CRTC registers 0-7 (some cards need this)
        mov     dx, CRTC
        mov     al, 11h
        out     dx, al                  ; Vertical sync end register
        inc     dx
        in      al, dx
        or      al, 80h                 ; Set write protect bit
        out     dx, al

@@Exit:
        xor     ax, ax
        mov     ax, [mx_ScreenWidth]
        .pop    ds, si, es, di
        .leave  ARG_SIZE
mxChangeMode    ENDP

;-----------------------------------------------------------
;
; Returns the aspect ratio for the current mode.
;
; Input:
;       AspectX = pointer to aspect X
;       AspectY = pointer to aspect Y
;
; A rectangle of width AspectX and height AspectY looks like a square.
;
mxGetAspect     PROC FAR
        ARG     AspectY:DWORD,  \
                AspectX:DWORD   = ARG_SIZE
        .enter  0
        .push   ds, si
        ASSUME  ds:NOTHING

        lds     si, [AspectX]
        mov     ax, [mx_AspectX]
        mov     ds:[si], ax
        lds     si, [AspectY]
        mov     ax, [mx_AspectY]
        mov     ds:[si], ax

        .pop    ds, si
        .leave  ARG_SIZE
mxGetAspect     ENDP

;-----------------------------------------------------------
;
; Returns the current screen size.
;
; Input:
;       Width   = pointer to screen width
;       Height  = pointer to screen height
;
mxGetScreenSize PROC FAR
        ARG     SizeY:DWORD,    \
                SizeX:DWORD     = ARG_SIZE
        .enter  0
        .push   ds, si
        ASSUME  ds:NOTHING

        lds     si, [SizeX]
        mov     ax, [mx_ScreenWidth]
        mov     ds:[si], ax
        lds     si, [SizeY]
        mov     ax, [mx_ScreenHeight]
        mov     ds:[si], ax

        .pop    ds, si
        .leave  ARG_SIZE
mxGetScreenSize ENDP

MX_TEXT         ENDS
END
