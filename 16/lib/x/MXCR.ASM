;-----------------------------------------------------------
;
; MXCR.ASM - Clip functions
; Copyright (c) 1993,1994 by Alessandro Scotti
;
;-----------------------------------------------------------
;WARN    PRO
;NOWARN  RES
INCLUDE MODEX.DEF

PUBLIC  mxSetSysClipRegion
PUBLIC  mxGetClipRegion
PUBLIC  mxSetClipRegion
PUBLIC  mxSetClip
PUBLIC  mxGetClip

PUBLIC  subClipBox
PUBLIC  subClipImage

PUBLIC  mx_ClipX1
PUBLIC  mx_ClipY1
PUBLIC  mx_ClipX2
PUBLIC  mx_ClipY2

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

EXTRN   mx_CodeSegment  : WORD

mx_ClipX1       DW      ?               ; Clip coordinates
mx_ClipY1       DW      ?
mx_ClipX2       DW      ?
mx_ClipY2       DW      ?

mx_SysClipX1    DW      ?               ; System clip coordinates
mx_SysClipY1    DW      ?               ; (active when mx_ClipStatus is FALSE)
mx_SysClipX2    DW      ?
mx_SysClipY2    DW      ?

mx_UserClipX1   DW      ?               ; User clip coordinates
mx_UserClipY1   DW      ?               ; (active when mx_ClipStatus is TRUE)
mx_UserClipX2   DW      ?
mx_UserClipY2   DW      ?

mx_ClipStatus   DB      ?

;-----------------------------------------------------------
;
; Toggles clipping between user and system regions.
;
; Input:
;       ClipStatus      = TRUE (FALSE) to enable (disable) clipping
; Output:
;       AX      = old clip status
;
mxSetClip       PROC    FAR
        ARG     ClipStatus:BYTE:2       = ARG_SIZE
        ASSUME  ds:NOTHING
        .enter  0
        .push   ds
        mov     ds, [mx_CodeSegment]
        ASSUME  ds:MX_TEXT

        mov     ax, [mx_UserClipX1]
        mov     bx, [mx_UserClipY1]
        mov     cx, [mx_UserClipX2]
        mov     dx, [mx_UserClipY2]
        cmp     [ClipStatus], TRUE
        je      @@Done
        mov     ax, [mx_SysClipX1]
        mov     bx, [mx_SysClipY1]
        mov     cx, [mx_SysClipX2]
        mov     dx, [mx_SysClipY2]
@@Done:
        mov     [mx_ClipX1], ax
        mov     [mx_ClipY1], bx
        mov     [mx_ClipX2], cx
        mov     [mx_ClipY2], dx

        mov     al, [ClipStatus]
        xchg    al, [mx_ClipStatus]
        xor     ah, ah

        .pop    ds
        .leave  ARG_SIZE
mxSetClip       ENDP

;-----------------------------------------------------------
;
; Returns the current clipping status.
;
; Input:
;       none
; Output:
;       TRUE (FALSE) if clipping enabled (disabled)
;
mxGetClip       PROC    FAR
        ASSUME  ds:NOTHING
        mov     al, [mx_ClipStatus]
        xor     ah, ah
        ret
mxGetClip       ENDP

;-----------------------------------------------------------
;
; Sets the system clip region and disables user clipping.
;
; Input:
;       Width   = width in pixels of clip region
;       Height  = height in pixels of clip region
; Output:
;       old clip status.
;
mxSetSysClipRegion      PROC    FAR
        ARG     Height:WORD,            \
                Width:WORD              = ARG_SIZE
        ASSUME  ds:NOTHING
        .enter  0
        .push   ds
        mov     ds, [mx_CodeSegment]
        ASSUME  ds:MX_TEXT

        xor     ax, ax                  ; Sys clip region always starts at (0,0)
        mov     [mx_SysClipX1], ax
        mov     [mx_SysClipY1], ax
        mov     ax, [Width]
        dec     ax
        mov     [mx_SysClipX2], ax
        mov     ax, [Height]
        dec     ax
        mov     [mx_SysClipY2], ax

    IF USE286 EQ TRUE
        push    FALSE
    ELSE
        mov     ax, FALSE
        push    ax
    ENDIF
        call    mxSetClip

        .pop    ds
        .leave  ARG_SIZE
mxSetSysClipRegion      ENDP

;-----------------------------------------------------------
;
; Sets the clip region.
;
; Input:
;       X, Y    = coordinates of top left corner of clip region
;       Width   = width in pixels of clip region
;       Height  = height in pixels of clip region
; Output:
;       none (no checking on parameters)
;
mxSetClipRegion PROC    FAR
        ARG     Height:WORD,            \
                Width:WORD,             \
                Y:WORD,                 \
                X:WORD                  = ARG_SIZE
        ASSUME  ds:NOTHING
        .enter  0
        .push   ds
        mov     ds, [mx_CodeSegment]
        ASSUME  ds:MX_TEXT

        mov     ax, [X]
        mov     [mx_UserClipX1], ax
        mov     ax, [Y]
        mov     [mx_UserClipY1], ax
        mov     ax, [Width]
        add     ax, [X]
        dec     ax
        mov     [mx_UserClipX2], ax
        mov     ax, [Height]
        add     ax, [Y]
        dec     ax
        mov     [mx_UserClipY2], ax

        mov     al, [mx_ClipStatus]
        cmp     al, TRUE
        jne     @@Exit
        push    ax
        call    mxSetClip

@@Exit:
        xor     ax, ax
        .pop    ds
        .leave  ARG_SIZE
mxSetClipRegion ENDP

;-----------------------------------------------------------
;
; Returns the current user clip region.
;
; Input:
;       X, Y    = pointers to integer coordinates of top left corner
;       Width   = pointer to word width of clip region
;       Height  = pointer to word height of clip region
; Output:
;       AX      = current clip status
;
mxGetClipRegion PROC    FAR
        ARG     Height:DWORD,           \
                Width:DWORD,            \
                Y:DWORD,                \
                X:DWORD                 = ARG_SIZE
        ASSUME  ds:NOTHING
        .enter  0
        .push   es, di

        mov     ax, [mx_UserClipX1]
        les     di, [X]
        mov     es:[di], ax
        mov     ax, [mx_UserClipY1]
        les     di, [Y]
        mov     es:[di], ax

        mov     ax, [mx_UserClipX2]
        sub     ax, [mx_UserClipX1]
        inc     ax
        les     di, [Width]
        mov     es:[di], ax
        mov     ax, [mx_UserClipY2]
        sub     ax, [mx_UserClipY1]
        inc     ax
        les     di, [Height]
        mov     es:[di], ax

        mov     al, [mx_ClipStatus]
        xor     ah, ah
        .pop    es, di
        .leave  ARG_SIZE
mxGetClipRegion ENDP

;-----------------------------------------------------------
;
; Internal use: checks the coordinates of a rectangle against
; the active clip region.
; This function assumes that a "raw" image has to be clipped,
; so it returns in SI the number of "raw" bytes to skip if
; X, Y were clipped.
;
; Input:
;       BX, AX  = X, Y coordinates of rectangle (signed)
;       CX      = box width
;       DX      = box height
; Output:
;       CF      = set if rectangle is full clipped
;       BX, AX  = new X, Y coordinates of rectangle
;       CX, DX  = clipped width and height
;       SI      = number of bytes to skip before copying a buffer
;       DI destroyed
;
subClipImage    PROC    NEAR
        ASSUME  ds:NOTHING
        xor     si, si

; Check clip height
        mov     di, [mx_ClipY1]
        cmp     ax, di
        jge     @@CheckBottom
        sub     di, ax                  ; Number of lines to clip
        sub     dx, di                  ; New box height
        jle     @@Exit
        mov     ax, di
        mov     di, dx                  ; Save box height into DI
        mul     cx                      ; DX:AX = number of bytes to skip
        mov     si, ax
        mov     dx, di                  ; Restore box height
        mov     ax, [mx_ClipY1]
@@CheckBottom:
        mov     di, [mx_ClipY2]
        cmp     ax, di
        jg      @@Exit
        inc     di
        sub     di, dx
        sub     di, ax
        jge     @@DoneHeight            ; None, continue
        add     dx, di                  ; Clip lines
@@DoneHeight:

; Check clip width
@@CheckLeft:
        mov     di, [mx_ClipX1]
        cmp     bx, di
        jge     @@CheckRight
        sub     di, bx                  ; Number of columns to clip left
        sub     cx, di
        jle     @@Exit
        add     si, di                  ; Update skip count
        mov     bx, [mx_ClipX1]
@@CheckRight:
        mov     di, [mx_ClipX2]
        cmp     bx, di
        jg      @@Exit
        inc     di
        sub     di, bx
        sub     di, cx
        jge     @@DoneWidth             ; None, exit
        add     cx, di                  ; New box width
@@DoneWidth:

; Set return flag and exit
@@Done:
        clc
        ret
@@Exit:
        stc
        ret
subClipImage    ENDP

;-----------------------------------------------------------
;
; Internal use: checks the coordinates of a rectangle against
; the active clip region.
;
; Input:
;       BX, AX  = X, Y coordinates of rectangle (signed)
;       CX      = box width
;       DX      = box height
; Output:
;       CF      = set if rectangle is full clipped
;       BX, AX  = new X, Y coordinates of rectangle
;       CX, DX  = clipped width and height
;       DI destroyed
;
subClipBox      PROC    NEAR
        ASSUME  ds:NOTHING

; Check clip height
        mov     di, [mx_ClipY1]
        cmp     ax, di
        jge     @@CheckBottom
        sub     di, ax                  ; Number of lines to clip
        sub     dx, di                  ; New box height
        jle     @@Exit
        mov     ax, [mx_ClipY1]
@@CheckBottom:
        mov     di, [mx_ClipY2]
        cmp     ax, di
        jg      @@Exit
        inc     di
        sub     di, dx
        sub     di, ax                  ; Clipped some point?
        jge     @@DoneHeight            ; No, continue
        add     dx, di                  ; Clip lines (DI is negative)
@@DoneHeight:

; Check clip width
@@CheckLeft:
        mov     di, [mx_ClipX1]
        cmp     bx, di
        jge     @@CheckRight
        sub     di, bx                  ; Number of columns to clip left
        sub     cx, di
        jle     @@Exit
        mov     bx, [mx_ClipX1]
@@CheckRight:
        mov     di, [mx_ClipX2]
        cmp     bx, di
        jg      @@Exit
        inc     di
        sub     di, bx
        sub     di, cx                  ; Clipped some point?
        jge     @@DoneWidth             ; No, exit
        add     cx, di                  ; New box width (DI is negative)
@@DoneWidth:

; Set return flag and exit
@@Done:
        clc
        ret
@@Exit:
        stc
        ret
subClipBox      ENDP

MX_TEXT         ENDS
END
