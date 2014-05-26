;-----------------------------------------------------------
;
; MXFP.ASM - Fade palette function
; Copyright (c) 1992-1994 ARTIS s.r.l.
; Author: Alessandro Scotti
;
;-----------------------------------------------------------
WARN    PRO
INCLUDE MODEX.DEF

PUBLIC  mxFadePalette

MAXCOLOR        EQU     256
FADE_IN         EQU     0
FADE_OUT        EQU     1

; The actual speed of fading depends on the number of passes (FADE_SPEED) and
; the delay between two consecutive passes (FADE_DELAY). Below are the
; default values, used when no parameters are specified.
;
FADE_DELAY      EQU     1               ; Vert. retraces between video updates
FADE_SPEED      EQU     48              ; Speed of effect (max 127)

; Bit field record for fade commands
;
FADE_COMMAND    RECORD  fpDELAY:8,fpSPEED:7,fpDIRECTION:1

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

DB      'Copyright (c) 1992-1994 ARTIS s.r.l. All rights reserved.'

;-----------------------------------------------------------
;
; Fades a VGA palette.
;
; Input:
;       Buffer          = pointer to source/destination palette
;       Command         = fading direction and optional parameters
;       Start           = index of first color to fade
;       Count           = number of color to fade
;       Red             = destination red
;       Green           = destination green
;       Blue            = destination blue
; Output:
;       none
; Notes:
;    -  about 1.5 Kbytes of stack space are needed for internal buffers;
;    -  the Command argument usually is 0 to fade in and 1 to fade out,
;       however additional parameters may be specified. To set the effect
;       speed, i.e. the number of iterations needed to completely fade a
;       palette, shift the value one bit left and "or" it with the
;       direction bit (range is 0..127). To set the delay between two
;       consecutive passes, shift it eight bits left (range is 0..255).
;
mxFadePalette   PROC FAR
        ARG     bBlue:WORD,                     \
                bGreen:WORD,                    \
                bRed:WORD,                      \
                wCount:WORD,                    \
                wStartIdx:WORD,                 \
                wCommand:WORD,                  \
                vfpPalette:DWORD = ARG_SIZE
        LOCAL   bSPalette:BYTE:MAXCOLOR*3,      \
                bDPalette:BYTE:MAXCOLOR*3,      \
                bLoopIndex:BYTE,                \
                bLoopStep:BYTE,                 \
                bLoopCount:BYTE,                \
                wDelay:WORD,                    \
                wSpeed:WORD     = AUTO_SIZE
        ASSUME  ds:NOTHING
        .enter  AUTO_SIZE
        .push   si, di, ds, es                  ; Save registers
;
; Check parameters and setup variables
;
@@GetDelay:
        mov     [wDelay], FADE_DELAY            ; Set default delay
        mov     ax, [wCommand]                  ; Get command word
        and     ax, MASK fpDELAY                ; Mask delay command
        jz      @@GetSpeed                      ; No command, get speed
    IF USE286 EQ TRUE
        shr     ax, fpDELAY
    ELSE
        mov     cl, fpDELAY                     ; Get size of delay field
        shr     ax, cl                          ; Right justify the field
    ENDIF
        mov     [wDelay], ax                    ; Set new delay

@@GetSpeed:
        mov     ax, [wCommand]                  ; Get command
        and     ax, MASK fpSPEED                ; Mask speed
    IF USE286 EQ TRUE
        shr     ax, fpSPEED
    ELSE
        mov     cl, fpSPEED                     ; Get size of speed field
        shr     ax, cl                          ; Right justify the field
    ENDIF
        or      ax, ax                          ; Any speed specified?
        jnz     @@SetVariables                  ; Yes, set variables
        mov     ax, FADE_SPEED                  ; Set default speed

@@SetVariables:
        mov     [wSpeed], ax                    ; Set speed
        inc     ax                              ; Number of iterations
        mov     [bLoopCount], al                ; Set loop count
        mov     [bLoopStep], 1                  ; Assume we're fading in
        mov     [bLoopIndex], 0
;
; Check bounds for bad values
;
        mov     ax, [wStartIdx]                 ; Get first index
        cmp     ax, MAXCOLOR                    ; Is in the valid range?
        jae     @@Exit                          ; No, exit
        add     ax, [wCount]                    ; Get last index
        cmp     ax, MAXCOLOR                    ; Is in the valid range?
        jbe     @@BoundsOk                      ; Yes, continue
        mov     ax, MAXCOLOR
        sub     ax, [wStartIdx]
        mov     [wCount], ax                    ; Set count to maximum value
        or      ax, ax
        jz      @@Exit                          ; Nothing to do, exit
@@BoundsOk:
;
; Copy the source palette in a local array: if we fade in it's ready to
; use, otherwise we'll overwrite it later
;
        mov     cx, [wCount]
        mov     ax, cx
        shl     ax, 1
        add     cx, ax                          ; CX = wCount * 3
        mov     ax, ss
        mov     es, ax
        lea     di, [bSPalette]                 ; ES:DI points to local palette
        mov     ax, [wStartIdx]
        mov     si, ax
        shl     ax, 1
        add     ax, si
        lds     si, [vfpPalette]                ; DS:SI points to user palette
        add     si, ax                          ; Skip unused entries
        cld
        shr     cx, 1
        rep     movsw
        rcl     cx, 1
        rep     movsb
;
; Check direction
;
        test    [wCommand], MASK fpDIRECTION    ; Are we fading in?
        jz      @@Main                          ; Yes, ok to continue
        mov     ax, [wSpeed]                    ; Get speed
        mov     [bLoopIndex], al                ; Exchange first and last index
        mov     [bLoopStep], -1                 ; Move backward
        mov     ax, ss                          ; Overwrite our copy of
        mov     ds, ax                          ; user palette with the
        mov     es, ax                          ; current active palette
        lea     di, [bSPalette]
        mov     ax, [wStartIdx]
        mov     cx, [wCount]
        call    ReadPalette                     ; Read current palette
;
; Prepare variables and registers for fading
;
@@Main:
        mov     bh, BYTE PTR [bRed]             ; Destination red
        and     bh, 00111111b                   ; Be sure it's a valid VGA value
        mov     bl, BYTE PTR [bGreen]           ; Destination green
        and     bl, 00111111b                   ; Be sure it's a valid VGA value
        mov     dh, BYTE PTR [bBlue]            ; Destination blue
        and     dh, 00111111b                   ; Be sure it's a valid VGA value
        mov     dl, [bLoopIndex]                ; Loop index
        mov     ax, ss                          ; All tables are stored
        mov     ds, ax                          ; in the stack segment,
        mov     es, ax                          ; set DS and ES
;
; Main loop
;
@@Loop:
        mov     ax, [wCount]                    ; Store count in AX
        mov     cx, [wSpeed]                    ; Set maximum speed in CX
        lea     si, [bSPalette]                 ; DS:SI points to source palette
        lea     di, [bDPalette]                 ; ES:DI points to dest. palette
        call    RecalcPalette                   ; Build a faded palette

        .push   bx, dx                          ; Save registers we need
        lea     si, [bDPalette]                 ; DS:SI points to palette
        mov     ax, [wStartIdx]                 ; First index to write
        mov     bx, [wCount]                    ; Total entries to write
        mov     cx, [wDelay]                    ; Fade delay between updates
        call    WritePalette                    ; Write palette
        .pop    bx, dx                          ; Restore BX and DX

        add     dl, [bLoopStep]                 ; Change fade intensity
        dec     [bLoopCount]                    ; Done?
        jnz     @@Loop                          ; No, loop again

@@Exit:
        .pop    si, di, ds, es                  ; Restore registers
        .leave  ARG_SIZE
mxFadePalette   ENDP

;------- INTERNAL USE ONLY ------------------------------------------------
;
; Calculates a partially faded palette.
;
; Input:
;       AX      = number of entries in palette
;       CX      = maximum fade intensity (same as fade speed)
;       DS:SI   = pointer to source palette
;       ES:DI   = pointer to destination palette
;       BH      = destination red
;       BL      = destination green
;       DH      = destination blue
;       DL      = relative intensity of destination palette
; Note:
;       it's important that a new palette can be calculated in less
;       than 1/70th of second. Fading to any RGB value requires use
;       of "imul" instructions: "idiv" may be replaced with faster
;       "sar", but only when the number of passes is a power of two,
;       thus reducing the range of selectable speeds.
;       In both cases an extimate of CPU cycles required by this
;       procedure shows that it's too slow to run on a 4.77 Mhz 8086
;       CPU and a 256 color palette, so we keep "idiv" and hope
;       the target machine to be at least a 6 Mhz 80286 (that's not
;       asking too much...).
;
RecalcPalette   PROC NEAR
        cld
        push    bp                              ; Save BP
        mov     bp, ax                          ; Copy counter in BP
@@Loop:
        lodsb                                   ; Red: read value
        sub     al, bh                          ; Subtract destination value
        imul    dl                              ; Scale to desired weight
        idiv    cl                              ; Put value in AL
        add     al, bh                          ; Add destination value...
        stosb                                   ; ...and store it
        lodsb                                   ; Green: read value
        sub     al, bl                          ; Subtract destination value
        imul    dl                              ; Scale to desired weight
        idiv    cl                              ; Put value in AL
        add     al, bl                          ; Add destination value...
        stosb                                   ; ...and store it
        lodsb                                   ; Blue: read value
        sub     al, dh                          ; Subtract destination value
        imul    dl                              ; Scale to desired weight
        idiv    cl                              ; Put value in AL
        add     al, dh                          ; Add destination value...
        stosb                                   ; ...and store it
        dec     bp
        jnz     @@Loop
        pop     bp                              ; Restore BP
        ret
RecalcPalette   ENDP

;------- INTERNAL USE ONLY ------------------------------------------------
;
; Writes a 256 color palette.
;
; Input:
;       AX      = index of first color to write
;       BX      = number of colors to write (each color is an RGB triplet)
;       CX      = number of vertical retraces to wait before writing to DACs
;       DS:SI   = pointer to first entry of palette
;
WritePalette    PROC NEAR
        ASSUME  ds:NOTHING
        mov     ah, al                          ; Save index
        mov     dx, 03DAh                       ; Input status register
@@Delay1:
        in      al, dx
        test    al, 08h
        jnz     @@Delay1                        ; Wait for display mode
@@Delay2:
        in      al, dx
        test    al, 08h
        jz      @@Delay2                        ; Wait for vertical retrace mode
        loop    @@Delay1                        ; Repeat CX times
@@Write:
        mov     cx, bx                          ; Get number of colors
        mov     dx, 03C8h                       ; PEL write address register
        mov     al, ah                          ; Restore index
        out     dx, al                          ; Select index of first color
        inc     dx                              ; PEL data register
        cld                                     ; Move forward
        cli                                     ; Disable interrupts
@@Loop:
        lodsb
        out     dx, al                          ; Red
        lodsb
        out     dx, al                          ; Green
        lodsb
        out     dx, al                          ; Blue
        loop    @@Loop                          ; Loop until done
        sti                                     ; Enable interrupts
        ret
WritePalette    ENDP

;------- INTERNAL USE ONLY ------------------------------------------------
;
; Reads the current palette.
;
; Input:
;       AX      = index of first color to read
;       CX      = number of colors
;       ES:DI   = pointer to destination buffer
;
ReadPalette     PROC    NEAR
        mov     dx, 03C7h
        out     dx, al
        inc     dx
        inc     dx
        cld
@@Loop:
        in      al, dx
        stosb
        in      al, dx
        stosb
        in      al, dx
        stosb
        loop    @@Loop
        ret
ReadPalette     ENDP

MX_TEXT         ENDS
END
