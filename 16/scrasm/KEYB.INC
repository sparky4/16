;; ====================================================================
;; Macros
;; ====================================================================
;; Jump if key pressed
JKEYP           MACRO   key,label
                cmp     byte ptr cs:_keyFlags[key+1],1
                je      label
                ENDM
;; Jump if key not pressed
JKEYNP          MACRO   key,label
                cmp     byte ptr cs:_keyFlags[key+1],1
                jne     label
                ENDM

;; Note that JNKEY and JKEY both modify _flKeyChanged, so you cannot
;; use one after the other!  In other words,
;;  JKEYNP no_key
;;  JKEYP  yes_key      ;<-- this will fail
;; will not work like you'd think it would.  The second call (JKEYP)
;; will not know that a key has been pressed!
;; Jump if no key pressed:
JNKEY           MACRO   label
                cmp     cs:_flKeyChanged,0
                je      label
                mov     cs:_flKeyChanged,0      ; <--- important!
                ENDM
;; Jump if key pressed:
JKEY            MACRO   label
                cmp     cs:_flKeyChanged,0
                mov     cs:_flKeyChanged,0
                jne     label
                ENDM

;; Start keyboard interrupts
KEYB_START      MACRO
                call    SwapInt9
                mov     cs:_flKeyChanged,0
                ENDM

;; Clear keyboard interrupts
KEYB_END        MACRO
                call    SwapInt9
                ENDM

;; Credit for these routines:  Steve Dollins, Brown Computer Group.
;; I didn't write any of the code below -- just heisted it from some
;; stuff that he wrote and released!  Very useful keyboard routines.
;; Any comments prefixed SDE were added by me.
_keyFlags       dw      256 dup (0)     ; SDE: since they only use 2 bits
                                        ; per word, this is a tradeoff,
                                        ; space for time

oldint9_offset  dw      offset newint9
oldint9_segment dw      seg newint9

_flKeyChanged   dw      0

;-----------------------------------------------------------------------
; void SwapInt9( void )
;
;       SwapInt9() exchanges the vector in oldint9_segment:oldint9_offset
;       with the vector in the interrupt table for INT 9h.
;-----------------------------------------------------------------------

SwapInt9        PROC    far
                mov     ax,cs
                mov     ds,ax

                mov     ax,03509h       ; Get interrupt 09h
                int     21h             ;   return in ES:BX

                mov     ax,oldint9_segment
                mov     dx,oldint9_offset
                push    ds
                mov     ds,ax
                mov     ax,02509h       ; Set new interrupt
                int     21h             ;  to address in DS:DX
                pop     ds

                mov     oldint9_segment,es    ; Save the old interrupt
                mov     oldint9_offset,bx
                ret
SwapInt9        ENDP


;-----------------------------------------------------------------------
; newint9 is the new keyboard interrupt (INT 9h).
;
;       Reads the scan code from the keyboard and modifies the key
;       flags table.  The high byte is set to the position of the key,
;       pressed=1, release=0.  The low byte is set to 1 when the key
;       is pressed and left unmodified when the key is released.
;-----------------------------------------------------------------------
newint9         PROC    far
                push    ax
                push    bx
                push    ds

                mov     ax,cs
                mov     ds,ax

                JKEYNP  kCTRL,not_ctrlaltdel    ; SDE code
                JKEYNP  kALT,not_ctrlaltdel     ; To still allow ctrl-
                JKEYNP  kDELETE,not_ctrlaltdel  ; alt-delete.  Nothing
                jmp     ctrlaltdel      ; worse than a total lockup!
not_ctrlaltdel:

                in      ax,60h          ; get scan code in AL, control byte in AH
                mov     bx,ax           ; save a copy in BX
                xchg    ah,al           ; swap to get control byte in AL
                or      al,80h          ; clear keyboard
                out     61h,al          ;   of interrupt
                and     al,7Fh
                out     61h,al
                mov     al,20h          ; send generic EOI to
                out     20h,al          ;   PIC

                and     bx,0007fh       ; strip all but the scan code
                shl     bx,1            ; multiply by two to get our offset

                ; if the key was released, the high bit is set in the scan code
                bt      ax,15           ; move this high bit into the carry flag
                setnc   byte ptr [_keyFlags+bx+1] ; set "Is being pressed" flag
                jc      short int09done ; if the key was released, we're done
                mov     byte ptr [_keyFlags+bx],1 ; set "Has been pressed" flag
                mov     _flKeyChanged,1         ; state of keyboard has changed
int09done:
                mov     _flKeyChanged,1         ; state of keyboard has changed
                pop     ds
                pop     bx
                pop     ax
                iret
ctrlaltdel:     int     19h                     ; SDE -- added this.
                                                ;  Allows a reboot.
newint9         ENDP

;; Defines the current key procedure (used as a jump-through)
kprocCur        dw      KprocDirect

;; This is a keyboard procedure.  Normally, this would control some
;; sprite, or something, and the screen would follow the sprite.  For
;; the purposes of this code, though (namely, sprite-less scrolling)
;; it just directly affects ScrollDX and ScrollDY.
;; This keyproc is inertialess, use + and - to increase speed and
;; the up/down/left/right keys to move directions.
;; Pressing K will switch to the other keyprocedure on the fly.
;; P pauses the screen -- note that this is just for completely
;; freezing the screen... it doesn't return until you let go!

EVEN
scroll_speed_x  dw      SCROLL_SPEED                    ; (defaults)
scroll_speed_y  dw      SCROLL_SPEED * VIRTUAL_WIDTH    ; (defaults)
KprocDirect     PROC    near
chk_leftright:  mov     ax,0
                JKEYNP  kRIGHT,not_right
                mov     ax,scroll_speed_x
                mov     ScrollDX,ax
                jmp     chk_updown
not_right:      JKEYNP  kLEFT,not_left
                sub     ax,scroll_speed_x
                mov     ScrollDX,ax
                jmp     chk_updown
not_left:       mov     ScrollDX,ax

chk_updown:     mov     ax,0
                JKEYNP  kUP,not_up
                sub     ax,scroll_speed_y
                mov     ScrollDY,ax
                jmp     chk_other
not_up:         JKEYNP  kDOWN,not_down
                mov     ax,scroll_speed_y
                mov     ScrollDY,ax
                jmp     chk_other
not_down:       mov     ScrollDY,ax

chk_other:      JKEYNP  kK,not_k
                mov     kprocCur,KprocInertia
not_k:          JKEYNP  kM,not_m
                mov     bDoTransition,1
not_m:          JKEYNP  kGREY_MINUS,not_minus
                cmp     scroll_speed_x,1
                jle     not_minus
                dec     scroll_speed_x
                sub     scroll_speed_y,VIRTUAL_WIDTH
not_minus:      JKEYNP  kGREY_PLUS,not_plus
                cmp     scroll_speed_x,16
                jge     not_plus
                inc     scroll_speed_x
                add     scroll_speed_y,VIRTUAL_WIDTH
not_plus:

pause_key:      JKEYP   kP,pause_key

                ret
KprocDirect     ENDP

;; This keyproc has inertia, so + and - don't work.
;; Use up/down/left/right keys to increase speed in those directions.
;; Pressing K will switch to the other keyprocedure on the fly.
;; P pauses the screen -- note that this is just for completely
;; freezing the screen... it doesn't return until you let go!
KprocInertia    PROC    near
chk2_leftright: JKEYNP  kRIGHT,not2_right
                cmp     ScrollDX,16
                je      not2_right
                inc     ScrollDX
                jmp     chk2_updown
not2_right:     JKEYNP  kLEFT,not2_left
                cmp     ScrollDX,-16
                je      not2_left
                dec     ScrollDX
                jmp     chk2_updown
not2_left:

chk2_updown:    JKEYNP  kUP,not2_up
                cmp     ScrollDY,-VIRTUAL_WIDTH * 16
                je      not2_up
                add     ScrollDY,-VIRTUAL_WIDTH
                jmp     chk2_other
not2_up:        JKEYNP  kDOWN,not2_down
                cmp     ScrollDY,VIRTUAL_WIDTH * 16
                je      not2_down
                add     ScrollDY,VIRTUAL_WIDTH
                jmp     chk2_other
not2_down:

chk2_other:     JKEYNP  kK,not2_k
                mov     kprocCur,KprocDirect
not2_k:         JKEYNP  kM,not2_m
                mov     bDoTransition,1
not2_m:

pause2_key:     JKEYP   kP,pause2_key

                ret
KprocInertia    ENDP
