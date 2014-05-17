;-----------------------------------------------------------
;
; MXVS.ASM - Set/get virtual screen
; Copyright (c) 1993,1994 by Alessandro Scotti
;
;-----------------------------------------------------------
;WARN    PRO
;NOWARN  RES
INCLUDE MODEX.DEF

PUBLIC  mxSetVirtualScreen
PUBLIC  mxGetVirtualScreen

EXTRN   mxRowAddress            : FAR
EXTRN   mxSetSysClipRegion      : FAR

MX_TEXT         SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MX_TEXT, ds:NOTHING, es:NOTHING

EXTRN   mx_BytesPerLine : WORD
EXTRN   mx_CodeSegment  : WORD

mx_VirtualWidth         DW      ?       ; Virtual screen size
mx_VirtualHeight        DW      ?

;-----------------------------------------------------------
;
; Sets the virtual screen.
;
; Input:
;       Width   = virtual screen width
;       Height  = virtual screen height
; Output:
;       0 on success, else invalid parameters
;
mxSetVirtualScreen      PROC    FAR
        ARG     Height:WORD,            \
                Width:WORD              = ARG_SIZE
        ASSUME  ds:NOTHING
        .enter  0
        .push   ds

; Set DS to code segment
        mov     ds, [mx_CodeSegment]
        ASSUME  ds:MX_TEXT

        mov     ax, 1                   ; Assume an error
        cmp     [Width], 320            ; Check width
        jb      @@Exit
        push    ax                      ; Save return code
        mov     dx, 0004h
        xor     ax, ax                  ; DX:AX = 256K
        div     [Width]                 ; Max height in AX
        cmp     [Height], ax
        pop     ax                      ; Restore return code
        ja      @@Exit                  ; Exit if bad heigth

        mov     ax, [Width]
        and     ax, 0FFF8h              ; Align to byte
        mov     [mx_VirtualWidth], ax
        shr     ax, 1
        shr     ax, 1
        mov     [mx_BytesPerLine], ax
        shr     ax, 1
        push    ax
        call    mxRowAddress            ; Set row address
        mov     ax, [Height]
        mov     [mx_VirtualHeight], ax

        push    [Width]
        push    [Height]
        call    mxSetSysClipRegion
        xor     ax, ax

@@Exit:
        .pop    ds
        .leave  ARG_SIZE
mxSetVirtualScreen      ENDP

;-----------------------------------------------------------
;
; Returns the current virtual screen size.
;
; Input:
;       Width   = pointer to virtual screen width
;       Height  = pointer to virtual screen height
; Output:
;       none
;
mxGetVirtualScreen      PROC    FAR
        ARG     Height:DWORD,           \
                Width:DWORD             = ARG_SIZE
        ASSUME  ds:NOTHING
        .enter  0
        .push   ds, si

        mov     ax, [mx_VirtualWidth]
        lds     si, [Width]
        mov     ds:[si], ax
        mov     ax, [mx_VirtualHeight]
        lds     si, [Height]
        mov     ds:[si], ax

        xor     ax, ax
        .pop    ds, si
        .leave  ARG_SIZE
mxGetVirtualScreen      ENDP

MX_TEXT         ENDS
END
