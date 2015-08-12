; SPRITE routines
MAX_SPRITE      EQU     100

RECTANGLE STRUCT 2,NONUNIQUE
                X       WORD    0
                Y       WORD    0
                Wid4    BYTE    0
                Ht      BYTE    0
                Color   BYTE    0
                Next    WORD    0
        ; DrawMe is used to not bother with sprites that you know
        ; are contained totally within another, allowing animated
        ; eyes, etc to be stored in separate sprites.  These will be
        ; drawn to the local buffer but skipped when copying to the
        ; screen, so if they are not TOTALLY contained, they will
        ; just get clipped away.
                DrawMe  BYTE    1       ; default, yes draw me.
        ; (Storage from this point on ... NEVER provide anything but
        ; default for these values!)
                address_virt    WORD    0
                address_buf     WORD    0
                next_line_virt  WORD    0
                next_line_buf   WORD    0
RECTANGLE ENDS

SPRITE  STRUCT 2, NONUNIQUE
        RECTANGLE       <>      ; Contains rectangle info
SPRITE  ENDS

EVEN
rect5   SPRITE  <<40 ,60 , 2,8, C_TRANSPARENT, 0           , 0>>
rect4   SPRITE  <<80 ,30 , 2,8, C_TRANSPARENT, offset rect5, 0>>
rect3   SPRITE  <<120,60 , 2,8, C_TRANSPARENT, offset rect4, 0>>
rect2   SPRITE  <<55 ,100, 2,8, C_TRANSPARENT, offset rect3, 0>>
rect1   SPRITE  <<105,100, 2,8, C_TRANSPARENT, offset rect2, 0>>

rect6   SPRITE  <<36 ,56 , 4,16, C_BLUE, offset rect1, 1>>
rect7   SPRITE  <<76 ,26 , 4,16, C_BLUE, offset rect6, 1>>
rect8   SPRITE  <<116,56 , 4,16, C_BLUE, offset rect7, 1>>
rect9   SPRITE  <<51 ,96 , 4,16, C_BLUE, offset rect8, 1>>
rect10  SPRITE  <<101,96 , 4,16, C_BLUE, offset rect9, 1>>

;; Simply adding in these 5 rectangles (~20000 pixels for both
;; drawing and erasing) really slows things down!  That's why
;; it's important to optimize the sprite drawing routines!
rect11  SPRITE  <<35 ,55 ,14,36, C_GREEN, offset rect10, 1>>
rect12  SPRITE  <<75 ,25 ,14,36, C_GREEN, offset rect11, 1>>
rect13  SPRITE  <<115,55 ,14,36, C_GREEN, offset rect12, 1>>
rect14  SPRITE  <<50 ,95 ,14,36, C_GREEN, offset rect13, 1>>
rect15  SPRITE  <<100,95 ,14,36, C_GREEN, offset rect14, 1>>

FIRST_SPRITE    EQU     rect10

EVEN
AnimateSprites  PROC    near
                ret
        ; Blank out the draw page, by copying from the blank page
        ; to the draw page all rectangles which had changed.  The
        ; blank page must always be entirely blank if this is going
        ; to work!
                mov     di,cs:DrawPage.UpperLeftAddress
                add     di,cs:DrawPage.ScrollOffset
                mov     si,cs:BlankPage.UpperLeftAddress
                add     si,cs:BlankPage.ScrollOffset
                mov     bp,cs:BlankPage.Rectangles
                call    CopyRectangles

        ; Now draw the sprites.  Uses a temporary buffer to ensure
        ; minimal drawing to the screen, but that's not really necessary,
        ; if memory is at a minimum.  It's just faster...
                mov     bp,offset FIRST_SPRITE
                mov     cs:DrawPage.Rectangles,bp
                call    do_fill_buffer
                mov     di,cs:DrawPage.UpperLeftAddress
                add     di,cs:DrawPage.ScrollOffset
                mov     bh,cs:DrawPage.AlignmentMask
                mov     bp,offset FIRST_SPRITE
                jmp     smart_rects     ; "call"
AnimateSprites  ENDP

smart_dest      DW      0
out_di          DW      0
out_si          DW      0

EVEN
smart_rects     PROC    near
                add     di,cs:DrawPage.Address
                mov     ds,cs:segBuffer
                mov     es,cs:segVideo
                mov     dx,3c4h
                mov     al,02h
                out     dx,al
                inc     dx
                mov     cs:smart_dest,di

        ; === Beginning of loop through rectangles! ===
sp_nextrect:
                cmp     cs:[bp].RECTANGLE.DrawMe,1
                jne     sp_next
        ; Draw this rectangle from the buffer to screen memory.
        ; Calculate the output address.
                mov     si,cs:[bp].RECTANGLE.address_buf
                mov     di,cs:[bp].RECTANGLE.address_virt
                add     di,cs:smart_dest

        ; Loop over 4 planes
                mov     bl,4
sp_plane_loop:  mov     al,bh
                out     dx,al

                mov     cs:out_di,di
                mov     cs:out_si,si

        ; Loop over height
                mov     ch,cs:[bp].RECTANGLE.Ht
sp_row_loop:

        ; Loop over width of rectangle (Wid4 is actually width/4)
                mov     cl,cs:[bp].RECTANGLE.Wid4
sp_col_loop:

        ; Read a byte from the buffer
        ; Is it transparent (no-modify)?  If so, just jump over the draw
                mov     al,byte ptr ds:[si]
                cmp     al,C_TRANSPARENT
                je      sp_next_pixel
        ; Otherwise, draw it on the spreen, and mark it transparent
        ; so that it won't be drawn again.
                mov     byte ptr es:[di],al
                mov     byte ptr ds:[si],C_TRANSPARENT

        ; Skip to next 4-byte group (next column that can be drawn in
        ; Mode X)  Also increment spreen draw address, but only by 1
        ; because ModeX is 4 pixels per byte
sp_next_pixel:
                add     si,4
                inc     di

                dec     cl
                jnz     sp_col_loop

        ; End of row.  Skip space to get to left edge of next row down
        ;  Skip SI = (SCREEN_WIDTH - #bytesdrawn)
        ; Only draw up to height of rectangle
                add     si,cs:[bp].RECTANGLE.next_line_buf
                add     di,cs:[bp].RECTANGLE.next_line_virt
                dec     ch
                jnz     sp_row_loop

                mov     di,cs:out_di
                mov     si,cs:out_si
                inc     si
                rol     bh,1
                adc     di,0

                dec     bl
                jnz     sp_plane_loop

        ; Follow chain to next rectangle
sp_next:        mov     bp,cs:[bp].RECTANGLE.Next
                cmp     bp,0
                jne     sp_nextrect
        ; All done
sp_end:         ret
smart_rects     ENDP

; BP -> first rectangle.  Follows BP->next, stops when BP = 0
EVEN
do_fill_buffer  PROC    near
                mov     es,cs:segBuffer

                cmp     bp,0
                je      fill_end
fill_loop:

                mov     bx,cs:[bp].RECTANGLE.Y
                shl     bx,1                    ; BX = word index y
                mov     di,cs:MultBufWidth[bx]  ; DI = SW * y
                mov     cx,cs:[bp].RECTANGLE.X  ; CX = x
                add     di,cx                   ; DI = (SW * y) + x
                mov     cs:[bp].RECTANGLE.address_buf,di ; (DI used later)

                mov     ax,cs:MultVirtWidth[bx] ; AX = (VW/4) * y
                shr     cx,2                    ; CX = (x / 4)
                add     ax,cx                   ; AX = (VW * y + x)/4
                mov     cs:[bp].RECTANGLE.address_virt,ax

                mov     dx,(VIRTUAL_WIDTH / 4)
                sub     dl,cs:[bp].RECTANGLE.Wid4 ; DX = (VW - w) / 4
                mov     cs:[bp].RECTANGLE.next_line_virt,dx

                mov     dx,(SCREEN_WIDTH / 4)
                sub     dl,cs:[bp].RECTANGLE.Wid4 ; DX = (SW - w) / 4
                shl     dx,2                      ; DX = SW - w
                mov     cs:[bp].RECTANGLE.next_line_buf,dx

                mov     ah,cs:[bp].RECTANGLE.Color
                mov     al,cs:[bp].RECTANGLE.Color

                mov     ch,cs:[bp].RECTANGLE.Ht
fill_row_loop:  mov     cl,cs:[bp].RECTANGLE.Wid4
fill_col_loop:  mov     es:[di],ax
                mov     es:[di+2],ax
                add     di,4
                dec     cl
                jnz     fill_col_loop
                add     di,dx
                dec     ch
                jnz     fill_row_loop

                mov     bp,cs:[bp].RECTANGLE.Next
                cmp     bp,0
                jne     fill_loop
fill_end:       ret
do_fill_buffer  ENDP

EVEN
CopyRectangles  PROC    near
                mov     ax,cs:segVideo
                mov     ds,ax
                mov     es,ax

        ; Calculate the difference between the source and destination
        ; pages.  Since in a movsb loop the two would remain a constant
        ; distance apart, we can just calculate a displacement and then
        ; not have to worry about SI; instead use DI and DI+BX, thanks
        ; to the thoughtful x86 ALU!
                mov     bx,di
                sub     bx,si

                mov     dx,GC_INDEX
                mov     ax,ALL_COPY_BITS
                out     dx,ax

                mov     dx,SC_INDEX
                mov     ax,0F02h
                out     dx,ax
                mov     si,di   ;store destination

        ; === Beginning of loop through rectangles! ===
cr_nextrect:    cmp     cs:[bp].RECTANGLE.DrawMe,1
                jne     cr_next
        ; Draw this rectangle from the buffer to screen memory.
        ; Calculate the output address.
                mov     di,cs:[bp].RECTANGLE.address_virt
                mov     dx,cs:[bp].RECTANGLE.next_line_virt
                add     di,si

        ; Loop over height
                mov     ch,cs:[bp].RECTANGLE.Ht
cr_row_loop:

        ; Loop over width of rectangle (Wid4 is actually width/4)
                mov     cl,cs:[bp].RECTANGLE.Wid4
cr_col_loop:    mov     al,ds:[di + bx]
                stosb
                dec     cl
                jnz     cr_col_loop
                mov     al,ds:[di + bx]
                mov     es:[di],al

        ; End of row.  Skip space to get to left edge of next row down
        ; Only draw up to height of rectangle
                add     di,dx
                dec     ch
                jnz     cr_row_loop

        ; Follow chain to next rectangle
cr_next:        mov     bp,cs:[bp].RECTANGLE.Next
                cmp     bp,0
                jne     cr_nextrect
        ; All done
cr_end:
                mov     dx,GC_INDEX
                mov     ax,ALL_DRAW_BITS
                out     dx,ax
                ret
CopyRectangles  ENDP

