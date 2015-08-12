;; MAP in own segment allows map of tiles to be up to 65536 tiles in area
;; which translates to about 16.8 million pixels of virtual screen.  This
;; can be represented in almost any rectangle -- just set MAP_WIDTH.

;; Sorry this code isn't commented -- I was working on it right up until
;; the point that I released this.  You have any questions?  Ask away
;; (my internet address is in the DOC file).

MAPHEADER       STRUCT, NONUNIQUE
                MapName BYTE    ""
                Wid     WORD    2
                Ht      WORD    3
                Extent  WORD    4
                OffX1   WORD    5
                OffY1   WORD    6
                OffX2   WORD    7
                OffY2   WORD    8
                WrapX   WORD    9
                WrapY   WORD    10
                Magic   WORD    11
MAPHEADER       ENDS
MapInfo         MAPHEADER <>


; In: DS:DX = offset of filename
LoadMapFile     PROC    near
                mov     ax,segMap
                cmp     ax,-1
                je      map_not_loaded
                sub     ax,(SIZEOF MAPHEADER) / 16
                mov     es,ax
                mov     ah,49h
                int     21h
                mov     nError,ERR_MEM
                jc      lm_err
                mov     segMap,-1

map_not_loaded: call    LoadFile
                jc      lm_err

                mov     ds,dx
                mov     si,0
                mov     ax,cs
                mov     es,ax
                lea     di,MapInfo
                mov     cx,(SIZEOF MAPHEADER) / 4
            rep movsd

                add     dx,(SIZEOF MAPHEADER) / 16
                mov     cs:segMap,dx

                mov     BlankPage.Valid,0
                mov     ShowPage.Valid,0
                mov     DrawPage.Valid,0

                mov     upper_left,0
                mov     ScrollPosX,0
                mov     ScrollPosY,0
                mov     ScrollDX,0
                mov     ScrollDY,0

lm_err:         ret
LoadMapFile     ENDP

LoadTilesFile   PROC    near
                mov     ax,segTiles
                cmp     ax,-1
                je      tiles_not_loaded
                mov     es,ax
                mov     ah,49h
                int     21h
                mov     nError,ERR_MEM
                jc      lt_err
                mov     segMap,-1

tiles_not_loaded: call    LoadFile
                jc      lm_err
                mov     segTiles,dx

                mov     BlankPage.Valid,0
                mov     ShowPage.Valid,0
                mov     DrawPage.Valid,0

lt_err:         ret
LoadTilesFile   ENDP

EVEN
LoadData        PROC    near
        ; Load squares from data file
                mov     bx,nMap
                shl     bx,1
                mov     dx,fntblTiles[bx]
                mov     ds,segCode
                call    LoadTilesFile
                ; returns Carry if error
                jc      load_error

        ; Load map from data file
                mov     ds,segCode
                mov     bx,nMap
                shl     bx,1
                mov     dx,fntblMap[bx]
                call    LoadMapFile
                ; returns Carry if error

load_error:     ret
LoadData        ENDP

EVEN
update_full     PROC
                mov     ds,segTiles
                mov     es,segVideo
                mov     fs,segMap

                mov     dx,SC_INDEX
                mov     al,MAP_MASK
                out     dx,al

                mov     di,DrawPage.Address
                add     di,upper_left
                mov     bp,MapInfo.OffX1
                add     bp,MapInfo.OffY1

                mov     dx,MapInfo.WrapX

                mov     ch,(VIRTUAL_WIDTH/SQUARE_WIDTH)
draw_full_loop: push    cx
                push    si
                push    dx

                mov     al,11h
                mov     si,0

update_f_loop:  mov     dx,SC_INDEX + 1
                out     dx,al
                push    bp
                call    draw_col
                pop     bp
                sub     di,(VIRTUAL_WIDTH * VIRTUAL_HEIGHT) / 4
                add     si,(SQUARE_WIDTH * SQUARE_HEIGHT) / 4
                shl     al,1
                jnc     update_f_loop

                pop     dx
                dec     dx
                jnz     update_f_go_on
                mov     dx,MapInfo.Wid
                sub     bp,dx
update_f_go_on: inc     bp
                pop     si
                add     di,(SQUARE_WIDTH/ 4)
                pop     cx
                dec     ch
                jnz     draw_full_loop











                mov     dx,GC_INDEX
                mov     ax,ALL_COPY_BITS
                out     dx,ax

                mov     dx,SC_INDEX
                mov     ax,0F02h
                out     dx,ax

                mov     ds,segVideo
                mov     si,DrawPage.Address
                add     si,upper_left
                mov     es,segVideo
                mov     di,BlankPage.Address
                add     di,upper_left
                mov     cx,(VIRTUAL_WIDTH * VIRTUAL_HEIGHT) / 4
            rep movsb
                mov     si,DrawPage.Address
                add     si,upper_left
                mov     di,ShowPage.Address
                add     di,upper_left
                mov     cx,(VIRTUAL_WIDTH * VIRTUAL_HEIGHT) / 4
            rep movsb

                mov     dx,GC_INDEX
                mov     ax,ALL_DRAW_BITS
                out     dx,ax

                ret
update_full     ENDP

EVEN
update_left     PROC
                mov     ds,cs:segTiles
                mov     es,cs:segVideo
                mov     fs,cs:segMap

                mov     dx,SC_INDEX
                mov     al,MAP_MASK
                out     dx,al

                mov     al,011h
                mov     si,0
                mov     di,cs:DrawPage.Address
                add     di,cs:upper_left        ; becomes DI later
                mov     bp,MapInfo.OffX1
                add     bp,MapInfo.OffY1

update_l_loop:  mov     dx,SC_INDEX + 1
                out     dx,al
                push    bp
                call    draw_col
                pop     bp
                sub     di,(VIRTUAL_WIDTH * VIRTUAL_HEIGHT) / 4
                add     si,(SQUARE_WIDTH * SQUARE_HEIGHT) / 4
                shl     al,1
                jnc     update_l_loop

                ret
update_left     ENDP

EVEN
update_right    PROC    near
                mov     ds,cs:segTiles
                mov     es,cs:segVideo
                mov     fs,cs:segMap

                mov     dx,SC_INDEX
                mov     al,MAP_MASK
                out     dx,al

                mov     bp,MapInfo.OffX2
                add     bp,MapInfo.OffY1

                mov     al,011h
                mov     si,0

                mov     di,cs:DrawPage.Address  ; becomes DI
                add     di,cs:upper_left
                add     di,(VIRTUAL_WIDTH - SQUARE_WIDTH) / 4

update_r_loop:  mov     dx,SC_INDEX + 1
                out     dx,al

                push    bp
                call    draw_col
                pop     bp
                sub     di,(VIRTUAL_WIDTH * VIRTUAL_HEIGHT) / 4
                add     si,(SQUARE_WIDTH * SQUARE_HEIGHT) / 4
                shl     al,1
                jnc     update_r_loop

                ret
update_right    ENDP

EVEN
update_top      PROC
                mov     ds,cs:segTiles
                mov     es,cs:segVideo
                mov     fs,cs:segMap

                mov     dx,SC_INDEX
                mov     al,MAP_MASK
                out     dx,al

                mov     di,cs:DrawPage.Address
                add     di,cs:upper_left
                mov     bp,MapInfo.OffX1
                add     bp,MapInfo.OffY1

                mov     al,011h
                mov     si,0

update_top_loop:
                mov     dx,SC_INDEX + 1
                out     dx,al
                push    bp
                call    draw_row
                pop     bp
                sub     di,VIRTUAL_WIDTH / 4
                add     si,(SQUARE_WIDTH * SQUARE_HEIGHT) / 4
                shl     al,1
                jnc     update_top_loop

                ret
update_top      ENDP

EVEN
update_bottom   PROC
                mov     ds,cs:segTiles
                mov     es,cs:segVideo
                mov     fs,cs:segMap

                mov     dx,SC_INDEX
                mov     al,MAP_MASK
                out     dx,al

                mov     di,cs:DrawPage.Address
                add     di,cs:upper_left
                add     di,(VIRTUAL_WIDTH * (VIRTUAL_HEIGHT - SQUARE_HEIGHT)) / 4
                mov     bp,MapInfo.OffX1
                add     bp,MapInfo.OffY2

                mov     al,011h
                mov     si,0

update_bottom_loop:
                mov     dx,SC_INDEX + 1
                out     dx,al
                push    bp
                call    draw_row
                pop     bp
                sub     di,VIRTUAL_WIDTH / 4
                add     si,(SQUARE_WIDTH * SQUARE_HEIGHT) / 4
                shl     al,1
                jnc     update_bottom_loop

                ret
update_bottom   ENDP

; Draws ONE plane of a single col
EVEN
draw_col        PROC    near
        ; DI->upper left corner of col to draw
        ; BP->col of map to draw
        ; SI used to point at tiles
        ; AX,CX used
        ; BX used to push SI
        ; DX unused
                shl     eax,16  ; save it
                mov     ax,MapInfo.WrapY

                mov     cl,(VIRTUAL_HEIGHT / SQUARE_HEIGHT)
do_col_loop:    mov     bx,si
                mov     bh,byte ptr fs:[bp]     ; change tile #

                mov     ch,SQUARE_HEIGHT
do_col_sq_loop: mov     dl,byte ptr ds:[bx+2]
                mov     dh,byte ptr ds:[bx+3]
                shl     edx,16
                mov     dl,byte ptr ds:[bx+0]
                mov     dh,byte ptr ds:[bx+1]
                mov     es:[di],edx     ; 32-bit write
                add     di,VIRTUAL_WIDTH / 4
                add     bx,4
                dec     ch
                jnz     do_col_sq_loop

                add     bp,MapInfo.Wid
                dec     ax
                jnz     yayaya
                mov     ax,MapInfo.Ht
                sub     bp,MapInfo.Extent
yayaya:

                dec     cl
                jnz     do_col_loop

                shr     eax,16  ; restore it

                ret
draw_col        ENDP

; Draws ONE plane of a single row
EVEN
draw_row        PROC    near
                push    ax
;               shl     eax,16  ; save ax

                mov     ax,MapInfo.WrapX

        ; DI->upper left corner of row to draw
        ; BP->row of map to draw
        ; SI used to point at tiles
        ; AX,CX used
        ; BX used to push SI
        ; DX unused

                mov     cl,(VIRTUAL_WIDTH / SQUARE_WIDTH)
do_row_loop:    mov     bx,si
                mov     bh,byte ptr fs:[bp]     ; change tile #

                mov     ch,SQUARE_HEIGHT
do_row_sq_loop: mov     dl,byte ptr ds:[bx+2]
                mov     dh,byte ptr ds:[bx+3]
                shl     edx,16
                mov     dl,byte ptr ds:[bx+0]
                mov     dh,byte ptr ds:[bx+1]
                mov     es:[di],edx
                add     di,(VIRTUAL_WIDTH / 4)
                add     bx,4
                dec     ch
                jnz     do_row_sq_loop

                add     di,(-VIRTUAL_WIDTH*SQUARE_HEIGHT + SQUARE_WIDTH) / 4
                inc     bp
                dec     ax
                jnz     yayaya2
                mov     ax,MapInfo.Wid
                sub     bp,ax
yayaya2:
                dec     cl
                jnz     do_row_loop

;               shr     eax,16  ; restore it
                pop     ax
                ret
draw_row        ENDP
