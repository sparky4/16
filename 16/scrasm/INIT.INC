;; Error messages
ERR_OK          EQU     0
msgErr0         db      'Later!',13,10,'$'
ERR_MEM         EQU     1
msgErr1         db      'Error 001:  Out of memory?',13,10,'$'
ERR_CPU         EQU     2
msgErr2         db      'Error 002:  CPU must be at least an 80386.',13,10,'$'
ERR_FILE        EQU     3
msgErr3         db      'Error 003:  File error.',13,10,'$'
ERR_FILENOTFOUND EQU    4
msgErr4         db      'Error 004:  File not found.',13,10,'$'
msgtblError     dw      offset msgErr0, offset msgErr1, offset msgErr2,
                        offset msgErr3, offset msgErr4
nError          db      0

;; CPU name strings
CPUName86       DB      "8088/8086$"
CPUName286      DB      "80286DX/SX$"
CPUName386      DB      "80386DX/SX$"
CPUName486      DB      "80486DX/SX or better$"
CPUNameTable    DW      CPUName86,CPUName286,CPUName386,CPUName486

EVEN
msgCPUTypeIs    DB      "Your CPU type:  $"
EVEN
msgCPUTypeIsEnd DB      13,10,'$'
nCPU            DB      0

EVEN
msgPages        DB      'Pages displayed:  '
strNumPages     DB      6 dup (?),13,10,'$'

EVEN
bufText         DW      80*50 DUP (?)   ; Needs this much to hold
                                        ; a 50-line screen...
wCPos           DW      0
nDisplay        DB      0

EVEN
fnMap1          db      'DIAGONAL.MAP',0
fnTiles1        db      'DIAGONAL.TIL',0
fnPalette       db      'DIAGONAL.PAL',0        ; only one allowed, for now
fnMap2          db      'SCROLL.MAP',0
fnTiles2        db      'SCROLL.TIL',0

fntblMap        dw      offset fnMap1,offset fnMap2
fntblTiles      dw      offset fnTiles1,offset fnTiles2
nMap            dw      0

;; CPUType routine snatched from Ray Duncan's _Power Programming MASM_
;; chapter 14.  Reformatted to my style, but I left the code alone
;; except that it used to push a bunch of stuff, but doesn't any more
;; because I don't care what gets destroyed.
CPUType         PROC    near
                pushf                   ; now try to clear bits 12-15
                pop     ax              ; of CPU flags
                and     ax,0fffh
                push    ax              ; set modified CPU flags
                popf
                pushf
                pop     ax              ; get flags again
                and     ax,0f000h       ; if bits 12-15 are still
                cmp     ax,0f000h       ; set, this is 8086/88
                jne     cpu1            ; jump, not 8086/88
                mov     nCPU,CPU8086    ; set nCPU = 86/88 CPU type
                jmp     cpux            ; and exit

cpu1:           or      ax,0f000h       ; must be 286 or later,
                push    ax              ; now try to set bits 12-15
                popf                    ; of CPU flags
                pushf
                pop     ax              ; if bits 12-15 can't be
                and     ax,0f000h       ; set, this is a 286
                jnz     cpu2            ; jump, not 80286
                mov     nCPU,CPU80286   ; set nCPU = 286 CPU type
                jmp     cpux            ; and exit

cpu2:           mov     bx,sp           ; 386 or later, save SP
                and     sp,not 3        ; avoid stack alignment fault
                pushfd                  ; get value of EFLAGS
                pop     eax
                mov     ecx,eax         ; save copy of EFLAGS
                xor     eax,40000h      ; flip AC bit in EFLAGS
                push    eax             ; try and force EFLAGS
                popfd
                pushfd                  ; get back EFLAGS value
                pop     eax
                mov     sp,bx           ; restore old stack pointer
                xor     eax,ecx         ; can AC bit be changed?
                jnz     cpu3            ; no, jump, not a 386
                mov     nCPU,CPU80386   ; set nCPU = 386 CPU type
                jmp     cpux            ; and exit

cpu3:           mov     nCPU,CPU80486   ; set nCPU = 486 CPU type

cpux:           mov     bl,nCPU
                xor     bh,bh
                shl     bx,1
                DOSPRINT <offset msgCPUTypeIs>
                DOSPRINT CPUNameTable[bx]
                DOSPRINT <offset msgCPUTypeIsEnd>
                ret                     ; return with nCPU = CPU type
CPUType         ENDP

;; Initialize:  So far, all it does is make sure you have a 386 +
;; (because that's what I assembled the code for).
Initialize      PROC    near
        ; Set DS = CS in this program, since data is local
                mov     ax,cs
                mov     segCode,ax      ; Store the Code Segment
                mov     bx,ds
                mov     segPSP,bx       ; Store the PSP Segment
                mov     ds,ax           ; Set DS = CS

        ; Resize code to 64K
        CODE_SIZE EQU 64                ; <- this is arbitrary.
                ; ES already -> allocated segment
                mov     ah,4ah
                mov     bx,64*CODE_SIZE
                int     21h
                mov     nError,ERR_MEM
                jc      TerminateError

;; I've chosen not to implement sprites yet so that I can get this out
;; the door...
;; ; 320x200 buffer for sprite drawing.  To draw sprites, first draw them
;; ; into this buffer, adding rectangles to the current rectangle list.
;; ; Then, use BUFFER_COPY to put out the buffers with the current
;; ; rectangle list to the screen.  BUFFER_COPY will ensure minimal VGA
;; ; writing.
;;         ; Create a buffer segment
;;                 mov     bx,(320 * 200) / 16
;;                 mov     ah,48h
;;                 int     21h
;;                 mov     nError,ERR_MEM
;;                 jc      TerminateError
;;                 mov     segBuffer,ax

                call    CPUType
                mov     nError,ERR_CPU
                cmp     nCPU,2
                jl      TerminateError

                mov     ds,segCode
                mov     dx,offset fnPalette
                call    LoadPaletteFile
                jc      TerminateError

                call    LoadIndex
                jc      TerminateError

                KEYB_START

                call    Beginning       ; Can display an entry screen here

        ; This is linked in from Michael Abrash's zen timer code.
        ; (But I wrote the Click myself)
        call    Click
        call    ZTimerOn

                call    MainLoop

        call    ZTimerOff
        call    Click

                call    Ending          ; Can display an exit screen here

                KEYB_END

Terminate:      mov     nError,ERR_OK
TerminateError:
                mov     ax,cs   ;DOS functions require that DS point
                mov     ds,ax   ; to text to be displayed on the screen
                mov     bh,0
                mov     bl,nError
                shl     bx,1
                DOSPRINT msgtblError[bx]

                mov     ax,pages
                mov     ds,segCode
                mov     si,offset strNumPages
                call    Int2Ascii
                DOSPRINT <offset msgPages>

        call    ZTimerReport

                mov     al,nError
                mov     ah,4ch        ; DOS Terminate
                int     21h
                ; Don't need to RET!  We're outta here
Initialize      ENDP

;; Clicks the internal speaker.  I use this to indicate that page timing
;; has started.
Click           PROC
                in      al,61h
                mov     ah,al
                or      al,3
                out     61h,al

                mov     cx,5000         ; (this is an arbitrary delay!)
spkr_on:        loop    spkr_on
                mov     al,ah
                out     61h,al
                ret
Click           ENDP

;; Copied from an old 8088 "Learn Assembly" book and changed a bit
Int2Ascii       PROC
                mov     cx,6
                mov     byte ptr cs:[si],' '
                mov     byte ptr cs:[si+1],'0'
                mov     byte ptr cs:[si+2],'0'
                mov     byte ptr cs:[si+3],'0'
                mov     byte ptr cs:[si+4],'0'
                mov     byte ptr cs:[si+5],'0'
                add     si,6
                mov     cx,10
                or      ax,ax
                jns     clear_divide
                neg     ax
                mov     byte ptr cs:[si-6],'-'
clear_divide:   mov     dx,0
                div     cx
                add     dx,'0'
                dec     si
                mov     cs:[si],dl
                or      ax,ax
                jnz     clear_divide
                ret
Int2Ascii       ENDP

;; Given a filename at DS:DX, reads the file into memory and returns
;; a pointer to it as DX:0000.
;; Note that this routine obviously will only work correctly for
;; a file < 640k in size, but you can bring in files bigger than 64k.
;; This code comes from Future Crew's STMIK sampler "Mental Surgery"
;; and I commented it up to make it fit in with my stuff a little better.
;; Thank you, FC, for releasing that code!  Several of the routines
;; in this program were inspired or helped along by having that source...
;; Most recently, added in error codes.
EVEN
LoadFile        PROC    NEAR
                ;set: DX=offset to filename
                ;return: DX=segment of file

        ; Open the datafile at DS:DX.
                mov     ax,3D00h        ; 3D,00 -> Open file, read only
                                        ; DS:DX already points at filename
                int     21h             ;  returns AX=file handle
                mov     cl,ERR_FILENOTFOUND
                jc      ferror
                mov     bx,ax           ; Store file handle in BX
                mov     si,bx           ; and also in a variable

        ; Get the length of the file so we know how much to allocate
                mov     ax,4202h        ; 42,02 -> Seek, signed from end
                mov     cx,0            ; CX:DX is a long file offset,
                                        ; BX is already set as file handle
                mov     dx,0            ;  zero in this case = end of file
                int     21h             ; (returns long offset in DX:AX)
                mov     cl,ERR_FILE
                jc      ferror

;;;             shr     dx,1            ; This is original FC code,
;;;             rcr     ax,1            ; which I removed because the
;;;             shr     dx,1            ; 386 has a nice instruction
;;;             rcr     ax,1            ; to do this all!
;;;             shr     dx,1            ; But 286 users will want to
;;;             rcr     ax,1            ; return to this code, instead
;;;             shr     dx,1            ; of SHRD dx,ax,4
;;;             rcr     ax,1            ;

        ; Now turn that long DX:AX into a number of paragraphs to allocate
        ; for when we read the file.
                shrd    ax,dx,4         ; Divides long DX:AX by 4,
                mov     bx,ax           ;  and stores this in BX
                inc     bx      ; HHMMMM?  One more needed for small #'s
                mov     ah,48h          ; 48 -> Allocate memory
                                        ; BX already = # of paragraphs
                int     21h
                mov     cl,ERR_MEM
                jc      ferror
                mov     di,ax           ; store this in a variable

        ; Seek the file back to the beginning in order to read it into
        ; the memory we just allocated.
                mov     ax,4200h        ; 42,00 -> Seek, absolute offset
                mov     bx,si           ; BX is the file handle.
                mov     cx,0            ; CX:DX is a long offset
                mov     dx,0
                int     21h
                jc      ferror

        ; Now read the file into memory
                mov     ds,di           ; DS points at alloc'd memory
ReadBlock:      mov     ah,3fh          ; 3F -> Read file
                mov     cx,32768        ; read 32768 bytes at a time
                mov     dx,0            ; DS:DX points at beginning of
                int     21h             ;  this block of memory.
                mov     cl,ERR_FILE
                jc      ferror
                mov     dx,ds           ; Offset DS by (32768/16), which
                add     dx,800h         ;  is the number of paragraphs in
                mov     ds,dx           ;  each block of 32768 bytes.
                cmp     ax,32768        ; Did we actually read 32768 bytes?
                je      ReadBlock       ; If so, there's more to read...
                                        ; Otherwise, we've read all the
                                        ;  data in the file.

        ; So now, close the file handle.
                mov     ah,3Eh          ; 3E -> Close file
                                        ; BX still is the file handle
                int     21h

        ; Everything went ok.  Return the segment in DX.
                mov     dx,di
                mov     nError,ERR_OK
                ret
ferror:         mov     nError,cl
                ret
LoadFile        ENDP

;; Eventually, this should load in an index of all data files to
;; allow for filenames to be specified outside of the program.  The
;; goal is to make the program have no hardcoded filenames...
;; Of course, the structure of this index and its entries will be
;; hardcoded, as will the structures of all of the files it includes.
LoadIndex       PROC    near
                ret
LoadIndex       ENDP

;; Save the current video mode and cursor position with standard
;; BIOS calls
SaveVideo       PROC    near
                mov     ah,0Fh
                int     10h             ; Get current display Mode
                mov     nDisplay,al
                mov     ah,03h
                mov     bh,0
                int     10h
                mov     wCPos,dx

                mov     ds,segText
                mov     si,0
                mov     es,segCode
                mov     di,offset bufText
                mov     cx,80*50
            rep movsw
                ret
SaveVideo       ENDP

;; Restore the current video mode and cursor position with standard
;; BIOS calls
RestoreVideo    PROC    near
                mov     ah,00h
                mov     al,nDisplay
                int     10h             ; Get current display Mode
                mov     ah,02h
                mov     bh,0
                mov     dx,wCPos
                int     10h

                PAL_UPDATE      ; When flipping into text mode, re-do the
                                ; palette because the BIOS changes it.

                mov     es,segText
                mov     di,0
                mov     ds,segCode
                mov     si,offset bufText
                mov     cx,80*50
            rep movsw
                ret
RestoreVideo    ENDP
