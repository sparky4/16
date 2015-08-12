;; ====================================================================
;; (Code follows)
;; ====================================================================

EVEN
upper_left      dw      0               ; Stores upper left corner offset
                                        ; relative to page offset.
pages           dw      0               ; for counting frame-per-sec

PAGE_INFO       STRUCT 2,NONUNIQUE
                Address         dw      0
                UpperLeftAddress dw     0
                MapPosX         dw      0
                MapPosY         dw      0
                Alignment       db      0
                AlignmentMask   db      0
                ScrollOffset    dw      0
                Rectangles      dw      0
                Valid           db      0
PAGE_INFO       ENDS

DrawPage        PAGE_INFO <PAGE_0,PAGE_0>
ShowPage        PAGE_INFO <PAGE_1,PAGE_1>
BlankPage       PAGE_INFO <PAGE_2,PAGE_2>

ROTATE3         MACRO   reg,item
                mov     reg,cs:ShowPage.item
                xchg    reg,cs:BlankPage.item
                xchg    reg,cs:DrawPage.item
                mov     cs:ShowPage.item,reg
                ENDM    ; Leaves ShowPage.item in reg!

;; This procedure is used to flip between the three available pages.
;; Originally from Dr. Dobb's Journal's Graphics Programming column by
;; Michael Abrash, I've reworked the code to be more specific to my
;; own purposes, and commented it more.
EVEN
FlipPage        PROC    near
        ; This series of instructions circles the show_page, blank_page,
        ; and draw page appropriately and leaves the current page to show
        ; in AX.  Note that it's a lot more instructions than it looks like,
        ; but I unrolled the copy loop for speed.  So-so good idea, because
        ; if you add a field and forget to rotate it, it could mean trouble!
                ROTATE3 ax,Rectangles
                ROTATE3 ax,ScrollOffset
                ROTATE3 ax,MapPosX
                ROTATE3 ax,MapPosY
;               ROTATE3 al,AlignmentMask        SPRITES ...
                ROTATE3 al,Alignment
                mov     di,ax           ; DI = scroll offset low, and
                                        ; garbage in the high bits...
                and     di,3            ; DI = pixel pan, 0 to 3.
                shl     di,1            ; Mode X requires 0 2 4 or 6.
                ROTATE3 ax,Address
                ROTATE3 al,Valid
                ROTATE3 ax,UpperLeftAddress ; Leaves AX=ShowPage.ULAddr

                add     ax,cs:ShowPage.ScrollOffset

        ; AX is set up to be the current show page already.
        ; By pre-loading BX with the low-address set code, and CX with
        ; the high-address set code, we can more quickly flip the page
        ; after the vertical retrace period.
                mov     bl,START_ADDRESS_LOW    ;preload for fastest
                mov     bh,al                   ; flipping once display
                mov     cl,START_ADDRESS_HIGH   ; enable is detected
                mov     ch,ah

        ; Wait for display enable to be active (status is active low), to be
        ; sure both halves of the start address will take in the same frame.
                mov     dx,INPUT_STATUS_1
WaitDE:         in      al,dx
                test    al,01h
                jnz     WaitDE  ;display enable is active low (0 = active)

        ; Set the start offset in display memory of the page to display.
                mov     dx,CRTC_INDEX
                mov     ax,bx
                out     dx,ax   ;start address low
                mov     ax,cx
                out     dx,ax   ;start address high

        ; Now wait for vertical sync, so the other page will be invisible when
        ; we start drawing to it.
                mov     dx,INPUT_STATUS_1
WaitVS:         in      al,dx
                test    al,08h
                jz      WaitVS  ;vertical sync is active high (1 = active)

        ; Finally, have to adjust the pixel panning register in order
        ; to fine-tune the starting address on a pixel level.
        ; This pixel pan value is the scroll offset mod 4 -- but since
        ; Mode X's pixel pan works by values of 2 (0, 2, 4 or 6) we
        ; have to shift appropriately.
                mov     dx,ATC_INDEX
                mov     al,13h  ; 13h = set pixel pan
                out     dx,al
                mov     ax,di   ; DI = pixel pan calculated above
                out     dx,al
                mov     dx,ATC_INDEX
                mov     al,32   ; Allows the computer to use this register
                out     dx,al   ; again.  Without this OUT, the screen will
                                ; remain blank!

        ; Increment the page counter now!
                inc     cs:pages
                ret
FlipPage        ENDP
