;-----------------------------------------------------------
;
; MXLL.ASM - Load latches
; Copyright (c) 1993,1994 by Alessandro Scotti
;
;-----------------------------------------------------------
WARN    PRO
INCLUDE MODEX.DEF

PUBLIC  mxLoadLatches

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

EXTRN   mx_VideoSegment : WORD

;-----------------------------------------------------------
;
; Loads the specified value into the VGA latches.
;
; Input:
;       BL      = value to load into latches
; Output:
;       none
; Changes:
;       bit mask register to FFh;
;       function select register to "move";
;       write mode to 00h.
; Note:
;       this is for internal use only.
;
mxLoadLatches   PROC    NEAR
        ASSUME  ds:NOTHING

        .push   ds, si
        mov     dx, GDC
        mov     ax, 0FF08h
        out     dx, ax                  ; Set bit mask to FFh
        mov     ax, 0003h
        out     dx, ax                  ; Set function to "move"
        mov     ax, 0005h
        out     dx, ax                  ; Set write mode
        mov     ax, [mx_VideoSegment]
        mov     ds, ax
        mov     si, 0FFFFh
        mov     bh, 8                   ; BH = write plane mask
        mov     cx, 3                   ; CX = count = read plane
; Saves old values and force BL into latches
@@SetLoop:
        mov     dx, GDC
        mov     al, 04h
        mov     ah, cl
        out     dx, ax                  ; Select read plane
        mov     dx, TS
        mov     al, 02h
        mov     ah, bh
        out     dx, ax                  ; Select write plane
        mov     al, ds:[si]
        push    ax
        mov     ds:[si], bl
        mov     al, ds:[di]             ; Force value into latch
        shr     bh, 1                   ; Next write plane
        loop    @@SetLoop
; Restore previous values
        mov     cx, 3
        mov     bh, 8
        mov     dx, TS
@@ResetLoop:
        mov     al, 02h
        mov     ah, bh
        out     dx, ax                  ; Select write plane
        pop     ax
        mov     ds:[si], al
        shr     bh, 1                   ; Next write plane
        loop    @@ResetLoop
; Exit
        .pop    ds, si
        ret
mxLoadLatches   ENDP

MX_TEXT         ENDS
END
