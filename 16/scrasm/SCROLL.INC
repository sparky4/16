;; Global variables used here ...
EVEN
ScrollPosX      dw      0       ; Scroll origin, upper-left X
ScrollPosY      dw      0       ; Scroll origin, upper-left Y
ScrollDX        dw      0       ; Amount to change scroll origin, X
ScrollDY        dw      0       ; Amount to change scroll origin, Y

;; SCROLL:
;; This routine takes care of all of the scrolling, however it calls
;; outside drawing routines to update the screen.  Scrollx and
;; Scrolly determine the amount to scroll by.
;; Note that this does only RELATIVE scrolling, not absolute scrolling.
;; Scroll saves time by updating only up to the one row or column of
;; tiles which have come into view due to a change in scroll offset.
;; In other words, it's not good for "jumping" to a particular point,
;; although this effect can be accomplished in other ways -- the draw_full
;; routine is available to draw a full screen again.
;; Sometimes this means that you will have to calculate values ahead of
;; time, for instance if you wish the scrolling to keep a certain sprite
;; in the center of the screen.  In this case, just set ScrollDX and
;; ScrollDY to the delta-x and delta-y of the sprite.
;; * Newly added:
;; Since there are three pages, it is necessary to keep each one of them
;; up to date with each scroll.  Recently, I was doing some fast (8+
;; pixels per frame) scrolling and noticed that there was a significant
;; pause when the screen snapped to a new origin.  (The origin is always
;; at a square's corner, even though it may not look like it because it
;; disguises things by smooth-panning the hardware.)  Every time it
;; scrolled, it was drawing the new information and copying it to the
;; two other planes.  I've now distributed the load over successive
;; pages, in other words it doesn't copy the new info all at once, but
;; over several frames.  This really smoothed out the scrolling so that
;; while there are still some jumps, they only occur very infrequently
;; and then only at 15 or 16 pixel/frame scroll rates...)  That's the
;; "catchup" code at the bottom, and that's why it's more complex than
;; it maybe could be...
EVEN
Scroll          PROC    near
        ; Using the ScrollDX variable as delta-x, move the scroll-origin
        ; in the x direction.  Then, if the visible screen is now
        ; viewing invalid data, snap the origin to a new point and
        ; draw any new columns that are necessary.
do_x_scroll:    mov     ax,cs:ScrollPosX
                add     ax,cs:ScrollDX           ; ScrollDX is a delta-x
                jl      wrap_l                  ; wrap left if negative
                cmp     ax,VIRTUAL_WIDTH - SCREEN_WIDTH ; too far right?
                jge     wrap_r                  ; wrap right if too big
                mov     cs:ScrollPosX,ax        ; Stores new scroll-x
        ; (just like above, for y:)
        ; Using the ScrollDY variable as delta-y, move the scroll-origin
        ; in the y direction.  Then, if the visible screen is now
        ; viewing invalid data, snap the origin to a new point and
        ; draw any new rows that are necessary.
do_y_scroll:    mov     ax,cs:ScrollPosY
                add     ax,cs:ScrollDY          ; ScrollDY is a delta-y
                jl      wrap_t                  ; wrap top if negative
                cmp     ax,(VIRTUAL_HEIGHT - SCREEN_HEIGHT) * VIRTUAL_WIDTH
                jge     wrap_b                  ; wrap bottom if too big
                mov     cs:ScrollPosY,ax        ; Store the new scroll-y
                jmp     calculate

        ; To wrap to the right:
        ; Add a square's width to the origin's upper left corner, and
        ; subtract the same amount from the scroll origin's upper left
        ; corner.  This makes no difference on the screen but allows
        ; us to forget about the leftmost column on the screen (it's
        ; offscreen now...) so we can take over the right column.
        ; See any documentation I included for an explanation of the
EVEN    ; scrolling...
wrap_r:         add     cs:upper_left,SQUARE_WIDTH / 4
                sub     ax,SQUARE_WIDTH
                mov     cs:ScrollPosX,ax

                mov     dx,MapInfo.Wid
                mov     bp,MapInfo.OffX1
                inc     bp
                cmp     bp,dx
                jb      wrap_r1_ok
                sub     bp,dx
wrap_r1_ok:     mov     MapInfo.OffX1,bp

                mov     bp,MapInfo.OffX2
                inc     bp
                cmp     bp,dx
                jb      wrap_r2_ok
                sub     bp,dx
wrap_r2_ok:     mov     MapInfo.OffX2,bp

                mov     bp,MapInfo.WrapX
                dec     bp
                jnz     wrap_r3_ok
                add     bp,dx
wrap_r3_ok:     mov     MapInfo.WrapX,bp

                call    update_right
                jmp     do_y_scroll     ; Jump back to do Y

EVEN    ; Same for left side
wrap_l:         sub     cs:upper_left,SQUARE_WIDTH / 4
                add     ax,SQUARE_WIDTH
                mov     cs:ScrollPosX,ax

                mov     dx,MapInfo.Wid
                mov     bp,MapInfo.OffX1
                dec     bp
                cmp     bp,dx
                jb      wrap_l1_ok
                add     bp,dx
wrap_l1_ok:     mov     MapInfo.OffX1,bp

                mov     bp,MapInfo.OffX2
                dec     bp
                cmp     bp,dx
                jb      wrap_l2_ok
                add     bp,dx
wrap_l2_ok:     mov     MapInfo.OffX2,bp

                mov     bp,MapInfo.WrapX
                inc     bp
                cmp     bp,dx
                jbe     wrap_l3_ok
                sub     bp,dx
wrap_l3_ok:     mov     MapInfo.WrapX,bp

                call    update_left
                jmp     do_y_scroll     ; Jump back to do Y

EVEN    ; Same for bottom
wrap_b:         add     cs:upper_left,(SQUARE_HEIGHT * VIRTUAL_WIDTH) / 4
                sub     ax,SQUARE_HEIGHT * VIRTUAL_WIDTH
                mov     cs:ScrollPosY,ax

                mov     bp,MapInfo.OffY1
                mov     dx,MapInfo.Extent
                add     bp,MapInfo.Wid
                cmp     bp,dx
                jb      wrap_b1_ok
                sub     bp,dx
wrap_b1_ok:     mov     MapInfo.OffY1,bp

                mov     bp,MapInfo.OffY2
                add     bp,MapInfo.Wid
                cmp     bp,dx
                jb      wrap_b2_ok
                sub     bp,dx
wrap_b2_ok:     mov     MapInfo.OffY2,bp

                mov     dx,MapInfo.Ht
                mov     bp,MapInfo.WrapY
                dec     bp
                jg      wrap_b3_ok
                add     bp,dx
wrap_b3_ok:     mov     MapInfo.WrapY,bp

                call    update_bottom
                mov     ax,cs:ScrollPosY
                jmp     calculate       ; Jump down to calc new offsets

EVEN    ; Same for top
wrap_t:         sub     cs:upper_left,(SQUARE_HEIGHT * VIRTUAL_WIDTH) / 4
                add     ax,SQUARE_HEIGHT * VIRTUAL_WIDTH
                mov     cs:ScrollPosY,ax

                mov     bp,MapInfo.OffY1
                mov     dx,MapInfo.Extent
                sub     bp,MapInfo.Wid
                cmp     bp,dx
                jb      wrap_t1_ok
                add     bp,dx
wrap_t1_ok:     mov     MapInfo.OffY1,bp

                mov     bp,MapInfo.OffY2
                sub     bp,MapInfo.Wid
                cmp     bp,dx
                jb      wrap_t2_ok
                add     bp,dx
wrap_t2_ok:     mov     MapInfo.OffY2,bp

                mov     bp,MapInfo.WrapY
                mov     dx,MapInfo.Ht
                inc     bp
                cmp     bp,dx
                jbe     wrap_t3_ok
                sub     bp,dx
wrap_t3_ok:     mov     MapInfo.WrapY,bp

                call    update_top
                mov     ax,cs:ScrollPosY
                jmp     calculate       ; Jump down to calc new offsets

EVEN
align_mask_table DB     11h,22h,44h,88h
calculate:
        ; Calculate the scroll offset
        ; AX already = ScrollPosY
                add     ax,cs:ScrollPosX        ;Now AX = scroll offset

        ; Calculate the plane alignment
                mov     bl,al
                and     bx,0003h
                mov     cs:DrawPage.Alignment,bl
;               mov     bl,cs:align_mask_table[bx]
;               mov     cs:DrawPage.AlignmentMask,bl

        ; Now we don't need Scroll Offset on a pixel level any more,
        ; so shift it to a byte level (/4) and store it away.
                shr     ax,2
                mov     cs:DrawPage.ScrollOffset,ax

        ; Calculate the actual upper left corner address
                mov     si,cs:DrawPage.Address
                add     si,cs:upper_left
                mov     cs:DrawPage.UpperLeftAddress,si

        ; And the map offset:
                mov     bx,MapInfo.WrapX
                mov     cs:DrawPage.MapPosX,bx
                mov     di,MapInfo.WrapY
                mov     cs:DrawPage.MapPosY,di

                mov     cs:DrawPage.Valid,1
                cmp     cs:BlankPage.Valid,0
                je      no_catch_up

        ; Lastly, update dirty area (if any) on blank page.
        ; BP still contains the draw page's mapoffset.
                sub     bx,cs:BlankPage.MapPosX
                sub     di,cs:BlankPage.MapPosY
                jnz     yes_catch_up
                cmp     bx,0
                jnz     yes_catch_up
        ; No catchup necessary -- return.
no_catch_up:    ret

;; Okay, this stuff is a mess.  I've registerized everything except
;; for the video data itself.  I'll try to comment it best I can.
EVEN
yes_catch_up:
        ; First, switch into full-copy mode.  This means latching the
        ; bit mask as coming entirely from the local 32-bit registers
        ; and then setting the map mask to write to all 4 planes.  This
        ; is Mode X's greatest advantage, when you can do it!  It
        ; provides a 2x speedup or so...
                mov     dx,SC_INDEX     ; Select Sequencer input
                mov     ax,0F02h
                out     dx,ax           ; set map mask = all bits

                mov     dx,GC_INDEX
                mov     ax,ALL_COPY_BITS
                out     dx,ax

                JKEYNP  kB,isntbp
isbp:           nop
isntbp:
        ; Next, calculate the amount to catch up the top/bottom rows
        ; If we just wrapped over the edge, it is possible that the
        ; distance traveled will be as high as MapInfo.Ht - 1.  So,
        ; in the fashion of signed numbers, if the number is greater
        ; than MapInfo.Ht / 2, we take it to mean negative.  To convert
        ; it to signed, we have to shift it into the proper range.  But
        ; if it's less than MapInfo.Ht / 2, then it's okay as it is.
                mov     ax,di
                cmp     ax,0
                je      y_mod

                mov     cx,MapInfo.Ht
                cwd             ; DX = -1 or 0 based on AX's sign.
                and     dx,cx   ; DX = Ht or 0
                add     ax,dx   ; AX = 0 ... Ht (unsigned)

                mov     di,ax
                shl     di,1
                cmp     di,cx
                jb      y_signed
                sub     ax,cx
y_signed:       neg     ax

        ; Find DI MOD MapInfo.Wid, and then convert to it into virtual
        ; coordinates from map offset coordinates.
        ; This routine also calculates BP, which will be used as a loop
        ; counter to determine how many rows to draw on the left/right
        ; column copy.
y_mod:          mov     bp,ax
                cwd
                add     bp,dx
                xor     bp,dx
                shl     bp,3            ; BP = (SQUARE_HEIGHT / 2) * dX
                mov     di,cs:MultVirtWidth[bp] ; Use multiplication table
                add     di,dx                   ; to calculate new DI, then
                xor     di,dx                   ; restore the sign.
                sub     bp,VIRTUAL_HEIGHT / 2
        ; Out:  DI = # of pixels traveled,
        ;       BP = (VIRTUAL_HEIGHT - # of rows) / 2

        ; Change BX (delta-x) to signed from unsigned, store in AX
                mov     ax,bx
                mov     cx,MapInfo.Wid
                cwd
                and     dx,cx   ; DX = Wid or 0
                add     ax,dx   ; AX = 0 ... Wid

                mov     bx,ax
                shl     bx,1
                cmp     bx,cx
                jb      x_signed
                sub     ax,cx
x_signed:

        ; The following is an optimization which would slow down on
        ; normal memory, but I believe it will be okay on VGA memory,
        ; which is so incredibly slow.  Basically, I've replaced all
        ; "rep movsb"'s with a loop that first calculates "bx = di - si",
        ; and then loops performing "mov ds:[si],es:[si+bx]".  Why?
        ; Because of several reasons, none of which I'm sure actually
        ; help out, but they do make for smaller code.  1)  It means that
        ; I only have to maintain SI, and "DI" is maintained automatically
        ; (because DI - SI should remain constant).  2)  Don't have to
        ; calculate DS.  Not much gain here.  3)  Because I'd already
        ; unrolled the loops, and the "rep movsb"'s had become instead
        ; "mov al, ds:[si] / mov es:[di], al / mov al, ds:[si + 1] /
        ; mov es:[di + 1],al ... etc ... add si, 4 / add di, 4".  In
        ; other words, I wasn't using MOVSB anyway.  The only advantage
        ; I can see in MOVSB is that it doesn't have to store the answer
        ; in AL so it could be slightly faster.  By unrolling the loops,
        ; I'd already made up for that, I think.  4)  Normally, using
        ; [SI + BX + 1] would incur a penalty of an additional clock
        ; cycle (because it has to add two indexs + an offset).  But
        ; the VGA memory and the '86 CPU can multi-task, and the VGA
        ; is very slow.  So by the time the VGA is ready to write the
        ; next byte, the one extra clock cycle has already passed.
        ;
        ; Am I right?  Does this make things faster?  I have no idea.
        ; I haven't bothered to check both ways.  Please let me know
        ; if I've missed something important...
        ;
        ; Here's the calculation of BX.  SI is already set.
                ; si already = DrawPage.UpperLeftAddress
                mov     bx,cs:BlankPage.Address
                sub     bx,cs:DrawPage.Address

        ; Now, converts SI into "1/4" units.  I do all the calculations
        ; in "1/4" scale and then scale back up, mostly because it saved
        ; me some instructions elsewhere.
                shr     si,2
        ; Stores this value of SI.  This will be restored after doing
        ; the top/bottom copying.
                mov     dx,si

        ; Check if it's necessary to catch up the top or bottom.
catchup_tb:     cmp     di,0
                je      catchup_tb_end
                jl      catchup_t
catchup_b:      ; COPY BOTTOM
        ; Move SI to point at the bottom of the screen - # of rows
        ; to update.
                add     si,((VIRTUAL_WIDTH * VIRTUAL_HEIGHT) / 4) / 4
                sub     si,di
                jmp     copy_tb
catchup_t:      ; COPY_TOP
        ; Leave SI, but add to the "pushed" value of SI the number of
        ; rows that will be drawn.  This prevents overlap between top
        ; and right/left when moving diagonally.  Also, DI = |DI|
                neg     di
                add     dx,di

        ; Now do the actual copying.  Shifts SI back into scale "1",
        ; then performs an unrolled loop to copy the entire virtual
        ; width * # of pixel rows.  Since DI is already in "1/4" scale,
        ; it is only decremented once for each four pixels drawn.
copy_tb:        shl     si,2
copy_tb_loop:   mov     cl,es:[si]
                mov     es:[si+bx],cl
                mov     cl,es:[si+1]
                mov     es:[si+bx+1],cl
                mov     cl,es:[si+2]
                mov     es:[si+bx+2],cl
                mov     cl,es:[si+3]
                mov     es:[si+bx+3],cl
                add     si,4
                dec     di
                jnz     copy_tb_loop
catchup_tb_end:

        ; Next, check to see if it's necessary to draw the right or
        ; the left side.
catchup_rl:     cmp     ax,0
                je      catchup_rl_end
                jg      catchup_l
catchup_r:      ; COPY RIGHT
        ; Adds to the "pushed" SI the width of the screen, minus
        ; the number of rows to be drawn.
                neg     ax
                add     dx,(VIRTUAL_WIDTH / 4) / 4
                sub     dx,ax
catchup_l:      ; COPY LEFT (or nothing)

        ; Does the actual copying.  First pops SI from its stored value
        ; and shifts it back into scale "1"
copy_rl:        mov     si,dx
                shl     si,2

        ; This is a loop over BP -- which has already been set as
        ; VIRTUAL_HEIGHT - (# of bytes drawn in vertical update)
        ; Again, this loop is unrolled such that it does two rows @
        ; 4 bytes each with every iteration.
        ; This LEA instruction is just a quick MOV DI, SI + 2 *y
        ; DI is used to push the next value of SI for each iteration
        ; of the loop.
copy_rl_loop:   lea     di,[si + 2*(VIRTUAL_WIDTH/4)]
                mov     cx,ax
copy_rl_col:    mov     dl,es:[si]
                mov     es:[si+bx],dl
                mov     dl,es:[si+1]
                mov     es:[si+bx+1],dl
                mov     dl,es:[si+2]
                mov     es:[si+bx+2],dl
                mov     dl,es:[si+3]
                mov     es:[si+bx+3],dl
                mov     dl,es:[si+VIRTUAL_WIDTH/4]
                mov     es:[si+bx+VIRTUAL_WIDTH/4],dl
                mov     dl,es:[si+VIRTUAL_WIDTH/4+1]
                mov     es:[si+bx+VIRTUAL_WIDTH/4+1],dl
                mov     dl,es:[si+VIRTUAL_WIDTH/4+2]
                mov     es:[si+bx+VIRTUAL_WIDTH/4+2],dl
                mov     dl,es:[si+VIRTUAL_WIDTH/4+3]
                mov     es:[si+bx+VIRTUAL_WIDTH/4+3],dl
                add     si,4
                dec     cx
                jnz     copy_rl_col
                mov     si,di           ; SI = pop (SI + VIRTUAL_WIDTH/4)
                inc     bp              ; (BP is negative, so INC it)
                jnz     copy_rl_loop
catchup_rl_end:

        ; Switch back to all-draw mode.
                mov     dx,GC_INDEX
                mov     ax,ALL_DRAW_BITS
                out     dx,ax
                ret
Scroll          ENDP
