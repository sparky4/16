;; Palette operations
;; Note that where needed in the macros, a "palette" refers to
;; the segment handle to a 768-byte piece of memory.  So palettes
;; can be loaded and freed, they're not permanent, but if you want
;; to use a fixed (not allocated) palette you'd better make sure
;; it's segment aligned or else you can't use these macros.  If it
;; is, you can just supply "seg myPalette" as the 'palette' argument
;; to any of these macros.

;; Fade from a palette to black
FADE_OFF        MACRO   fade,palette
                mov     si,0
                mov     ds,palette
                mov     bh,fade         ; positive -> Gets dimmer...
                mov     bl,0            ; Starts exact
                mov     cx,64/fade+1    ; Total number of loops required
                call    FadePalette
                ENDM

;; Fade from black to a palette
FADE_ON         MACRO   fade,palette
                mov     si,0
                mov     ds,palette
                mov     bh,-fade        ; negative -> Gets brighter...
                mov     bl,64           ; Starts totally dimmed
                mov     cx,64/fade+1    ; Total number of loops required
                call    FadePalette
                ENDM

;; Flash from a palette to white
FLASH_OFF       MACRO   fade,palette
                mov     si,0
                mov     ds,palette
                mov     bh,-fade        ; negative -> gets brighter
                mov     bl,0            ; Starts exact
                mov     cx,64/fade+1    ; Total number of loops required
                call    FadePalette
                ENDM

;; Flash from white to a palette
FLASH_ON        MACRO   fade,palette
                mov     si,0
                mov     ds,palette
                mov     bh,fade         ; positive -> Gets dimmer...
                mov     bl,-64          ; Starts totally bright
                mov     cx,64/fade+1    ; Total number of loops required
                call    FadePalette
                ENDM

;; Save a palette into a palette-sized piece of memory
PAL_SAVE        MACRO   palette
                mov     es,palette
                mov     di,0
                call    SavePalette
                ENDM

; Returns AX = a new segment for a palette
NEW_PAL         MACRO   palette
                mov     bx,(256 * 3) / 16
                mov     ah,48h
                int     21h
                mov     palette,ax
                ENDM

;; Black the entire palette temporarily.  Used to blank the screen while
;; drawing a frame before fading in.
PAL_BLACK       MACRO
                mov     ax,seg tmppal
                mov     ds,ax
                mov     si,OFFSET tmppal
                mov     bh,-1           ; Doesn't really matter...
                mov     bl,64           ; Starts totally dimmed
                mov     cx,1            ; Just one time -- to leave it black
                call    FadePalette
                ENDM

;; drawing a frame before fading in.
PAL_WHITE       MACRO
                mov     ax,seg tmppal
                mov     ds,ax
                mov     si,OFFSET tmppal
                mov     bh,-1           ; Doesn't really matter...
                mov     bl,-64          ; Starts totally dimmed
                mov     cx,1            ; Just one time -- to leave it black
                call    FadePalette
                ENDM

;; Black the entire palette temporarily.  Used to blank the screen while
;; drawing a frame before fading in.
PAL_UPDATE      MACRO
                mov     cx,0            ; 0 times = update
                call    FadePalette
                ENDM

WAITBORDER      MACRO
                LOCAL   wbr1,wbr2
                mov     dx,INPUT_STATUS_1
wbr1:           in      al,dx
                test    al,8
                jnz     wbr1
wbr2:           in      al,dx
                test    al,8
                jz      wbr2
                ENDM

;; Fade Palette:
;; The following code is modified greatly from the Future Crew's palette
;; fading code.  Works on blocks of 256 colors only, so far, but I might
;; change it later.  Also, it theoretically could "anti-fade" -- fade to
;; white -- which I call flashing, so I added that ability, which was
;; missing from FC's code.
EVEN
tmppal          DB      768 dup (?)      ; Stores old palette
FadePalette     PROC NEAR
                mov     ax,seg tmppal
                mov     es,ax

FadeLoop:       push    cx
                push    si

                cmp     cx,0
                je      JustUpdate

        ; Load in the colors in the palette
                mov     di,OFFSET tmppal ; ES:DI -> temp palette
                mov     cx,768          ; Reads 256*3 bytes at a time.
loadpal_loop:   mov     al,ds:[si]      ; Load one color byte
                inc     si
                sub     al,bl           ; Subtract the fade amount
                jge     pal_more        ; Limit the range by clipping
                xor     al,al           ;  to between 0 and 63
                jmp     pal_ok          ; (there's probably a faster
pal_more:       cmp     al,63           ; way to do it than this,
                jle     pal_ok          ; but I don't know it)
                mov     al,63
pal_ok:         mov     es:[di],al      ; Store that byte in the new
                inc     di
                dec     cx              ;  temp palette and loop.
                jnz     loadpal_loop

        ; Get ready to move this block of palette values
JustUpdate:     sti                     ; Let interrupts happen now,
                WAITBORDER              ;  while waiting for a retrace,
                cli                     ;  instead of more critical times

                mov     dx,PEL_WRITE_REG; Set up to write to color register,
                xor     al,al           ; starting at palette entry 0.
                out     dx,al
                mov     dx,PEL_DATA_REG ; Point at color port

        ; Quickly put out the first half of the color palette
                mov     di,OFFSET tmppal
                mov     cl,(768/6)/2    ; Does 2 loops of 128 colors each.
                cli                     ;  Waits a retrace inbetween...
FirstHalfLoop:  REPEAT 6                ; Steps of 6 -- reduces the
                mov     al,es:[di]      ; number of LOOP instructions
                inc     di
                out     dx,al
                ENDM
                dec     cl
                jnz     FirstHalfLoop
                sti

                WAITBORDER              ; Waits one retrace -- less flicker
                mov     dx,PEL_DATA_REG ; Reset DX

        ; Now, quickly put out the other half of the colors.
                mov     cl,(768/6)/2
                cli
SecondHalfLoop: REPEAT 6                ; Steps of 6 -- reduces the
                mov     al,es:[di]      ; number of LOOP instructions
                inc     di
                out     dx,al
                ENDM
                dec     cl
                jnz     SecondHalfLoop

        ; For the next iteration, restore everything and loop
                pop     si
                pop     cx

                cmp     cx,0
                je      JustUpdated

                add     bl,bh           ; Change brightness by BH

                dec     cx
                jnz     FadeLoop

        ; All done, re-enable interrupts and return
JustUpdated:    sti
                ret
FadePalette     ENDP

;; Saves the palette into the memory pointed at by DS:SI.  That memory
;; must be at least 768 bytes long...
SavePalette     PROC NEAR
                mov     dx,PEL_READ_REG ; Set up to read from color register,
                xor     al,al           ; starting at palette entry 0.
                out     dx,al
                mov     dx,PEL_DATA_REG

        ; Quickly read in the first half of the color palette
                mov     cl,(768/6)
                cli
ReadPalLoop:    REPEAT 6                ; Steps of 6 -- reduces the
                in      al,dx           ; number of LOOP instructions
                mov     es:[di],al
                inc     di
                ENDM
                dec     cl
                jnz     ReadPalLoop
        ; All done, re-enable interrupts and return
                sti
                ret
SavePalette     ENDP

;; Load a palette from a file.  Opens the file and reads it into
;; memory (standard LoadFile) and then points the palette at that
;; newly allocated memory.  Also, frees old memory before it does
;; any loading ...
LoadPaletteFile PROC    near
                mov     ax,segPalette
                cmp     ax,-1
                je      pal_not_loaded
                mov     es,ax
                mov     ah,49h
                int     21h
                mov     nError,ERR_MEM
                jc      lp_err
                mov     segPalette,-1

pal_not_loaded: call    LoadFile
                jc      lp_err

                mov     segPalette,dx
lp_err:         ret
LoadPaletteFile ENDP
