COMMENT /
        Fixed-point math functions and 3D transforms
        Copyright (c) 1993,94 by Alessandro Scotti
/
WARN    PRO
P386
JUMPS
LOCALS

INCLUDE MATH.INC

PUBLIC  tdFixedMul
PUBLIC  tdGetNormal
PUBLIC  tdRotate
PUBLIC  tdGetSurfaceLight
PUBLIC  tdSetLight
PUBLIC  tdSetRotation
PUBLIC  tdSetTranslation
PUBLIC  tdTransform
PUBLIC  tdTransformToImage
PUBLIC  tdTransformLight
PUBLIC  tdBackPlaneCull
PUBLIC  tdSetPerspective

;-----------------------------------------------------------
;
; Data segment
;
MATH_DATA       SEGMENT USE16 PARA PUBLIC 'DATA'
                ASSUME ds:MATH_DATA

INCLUDE         SINCOS.INC              ; Fixed 8:24 sin/cos table

XRotation       TPOINT  <>              ; 3x3 rotation matrix
YRotation       TPOINT  <>
ZRotation       TPOINT  <>

Translation     TPOINT  <>              ; Translation vector

Light           TPOINT  <>              ; Light vector
AmbientLight    DW      00              ; Ambient light

XScale                  DD      10000h  ; Scaling factor for X coordinate
YScale                  DD      10000h  ; Scaling factor for Y coordinate
PerspectiveDistance     DD      20000000h

MATH_DATA       ENDS

;-----------------------------------------------------------
;
; Code segment
;
MATH_TEXT       SEGMENT USE16 PARA PUBLIC 'CODE'
                ASSUME cs:MATH_TEXT, es:NOTHING, fs:NOTHING

tdSetPerspective        PROC PASCAL FAR
        ARG     Perspective:DWORD,      \
                ScaleX:DWORD,           \
                ScaleY:DWORD
        USES    ds

        mov     ax, SEG MATH_DATA
        mov     ds, ax
        ASSUME  ds:MATH_DATA

        mov     eax, [Perspective]
        mov     [PerspectiveDistance], eax
        mov     eax, [ScaleX]
        mov     [XScale], eax
        mov     eax, [ScaleY]
        mov     [YScale], eax

        ret
tdSetPerspective        ENDP


;-----------------------------------------------------------
;
; Sets the rotation matrix.
;
; Input:
;       RX      = X-axis rotation angle
;       RY      = X-axis rotation angle
;       RZ      = X-axis rotation angle
; Output:
;       none
;
tdSetRotation   PROC PASCAL FAR
        ARG     RX:WORD,        \
                RY:WORD,        \
                RZ:WORD
        USES    ds, si, di

        mov     ax, SEG MATH_DATA
        mov     ds, ax
        ASSUME  ds:MATH_DATA

        mov     bx, [RZ]
        mov     si, [RY]
        mov     di, [RX]
        shl     bx, 2
        shl     si, 2
        shl     di, 2

        push    ebp                     ; We use EBP as a scratch register

; Set X rotation
        mov     eax, tblCos[bx]
        imul    tblCos[si]
        mov     [XRotation.X], edx

        mov     eax, tblSin[bx]
        imul    tblCos[si]
        mov     [XRotation.Y], edx

        mov     eax, tblSin[si]
        sar     eax, 8                  ; Convert fixed 8:24 to fixed 16:16
        mov     [XRotation.Z], eax

; Set Y rotation
        mov     eax, tblCos[bx]
        imul    tblSin[si]              ; EDX:EAX = fixed 16:48
        shrd    eax, edx, 24            ; EAX = fixed 8:24
        imul    tblSin[di]              ; EDX:EAX = fixed 16:48
        mov     ebp, eax
        mov     ecx, edx
        mov     eax, tblSin[bx]
        imul    tblCos[di]
        add     eax, ebp
        adc     edx, ecx                ; EDX:EAX = fixed 16:48
        neg     edx
        mov     [YRotation.X], edx

        mov     eax, tblSin[bx]
        imul    tblSin[si]
        shrd    eax, edx, 24
        imul    tblSin[di]
        mov     ebp, eax
        mov     ecx, edx
        mov     eax, tblCos[bx]
        imul    tblCos[di]
        sub     eax, ebp
        sbb     edx, ecx
        mov     [YRotation.Y], edx

        mov     eax, tblCos[si]
        imul    tblSin[di]
        mov     [YRotation.Z], edx

; Set Z rotation
        mov     eax, tblCos[bx]
        imul    tblSin[si]
        shrd    eax, edx, 24
        imul    tblCos[di]
        mov     ebp, eax
        mov     ecx, edx
        mov     eax, tblSin[bx]
        imul    tblSin[di]
        sub     eax, ebp
        sbb     edx, ecx
        mov     [ZRotation.X], edx

        mov     eax, tblSin[bx]
        imul    tblSin[si]
        shrd    eax, edx, 24
        imul    tblCos[di]
        mov     ebp, eax
        mov     ecx, edx
        mov     eax, tblCos[bx]
        imul    tblSin[di]
        add     eax, ebp
        add     edx, ecx
        neg     edx
        mov     [ZRotation.Y], edx

        mov     eax, tblCos[si]
        imul    tblCos[di]
        mov     [ZRotation.Z], edx

        pop     ebp                     ; Restore EBP

        ret
tdSetRotation   ENDP

;-----------------------------------------------------------
;
; Sets the translation vector.
;
; Input:
;       TV      = pointer to translation vector
; Output:
;       none
;
tdSetTranslation        PROC PASCAL FAR
        ARG     TV:DWORD
        USES    ds, es, di

        mov     ax, SEG MATH_DATA
        mov     ds, ax
        ASSUME  ds:MATH_DATA

        les     di, [TV]
        mov     eax, es:[di].X
        mov     [Translation.X], eax
        mov     eax, es:[di].Y
        mov     [Translation.Y], eax
        mov     eax, es:[di].Z
        mov     [Translation.Z], eax

        ret
tdSetTranslation        ENDP

;-----------------------------------------------------------
;
; Transforms an array of TPOINT.
;
; Input:
;       Source  = pointer to source array of TPOINT
;       Dest    = pointer to destination array of TPOINT
;       Count   = number of entries to transform
; Output:
;       none
;
tdTransform     PROC PASCAL FAR
        ARG     Source:DWORD,   \
                Dest:DWORD,     \
                Count:WORD
        LOCAL   Adjust:DWORD
        USES    ds, si, es, di, fs

        mov     ax, SEG MATH_DATA
        mov     ds, ax
        ASSUME  ds:MATH_DATA

        lfs     si, [Source]
        les     di, [Dest]

        ALIGN   DWORD
@@Loop:
; Transform Z coordinate
        mov     eax, fs:[si].X
        imul    [ZRotation.X]
        mov     ecx, eax
        mov     ebx, edx
        mov     eax, fs:[si].Y
        imul    [ZRotation.Y]
        add     ecx, eax
        adc     ebx, edx
        mov     eax, fs:[si].Z
        imul    [ZRotation.Z]
        add     eax, ecx
        adc     edx, ebx
        mov     ebx, eax
        shrd    eax, edx, 16
        add     eax, [Translation.Z]    ; EAX = new Z coord (fixed 16:16)
        mov     es:[di].Z, eax
; Get perspective factor
        mov     ebx, [PerspectiveDistance]
        sub     eax, ebx
        neg     eax                     ; EAX = PD - Z
        xor     edx, edx
        shld    edx, eax, 16
        shl     eax, 16
        idiv    ebx                     ; EAX = fixed 16:16 result
        mov     [Adjust], eax

; Transform X coordinate
        mov     eax, fs:[si].X
        imul    [XRotation.X]
        mov     ecx, eax
        mov     ebx, edx
        mov     eax, fs:[si].Y
        imul    [XRotation.Y]
        add     ecx, eax
        adc     ebx, edx
        mov     eax, fs:[si].Z
        imul    [XRotation.Z]
        add     eax, ecx
        adc     edx, ebx
        shrd    eax, edx, 16
        add     eax, [Translation.X]
        imul    [Adjust]
        shrd    eax, edx, 16
        mov     es:[di].X, eax

; Transform Y coordinate
        mov     eax, fs:[si].X
        imul    [YRotation.X]
        mov     ecx, eax
        mov     ebx, edx
        mov     eax, fs:[si].Y
        imul    [YRotation.Y]
        add     ecx, eax
        adc     ebx, edx
        mov     eax, fs:[si].Z
        imul    [YRotation.Z]
        add     eax, ecx
        adc     edx, ebx
        shrd    eax, edx, 16
        add     eax, [Translation.Y]
        imul    [Adjust]
        shrd    eax, edx, 16
        mov     es:[di].Y, eax

        add     si, SIZE TPOINT
        add     di, SIZE TPOINT
        dec     [Count]
        jnz     @@Loop

        ret
tdTransform     ENDP

;-----------------------------------------------------------
;
; Transforms an array of TPOINT into an array of TIMAGEPOINT.
;
; Input:
;       Source  = pointer to source array of TPOINT
;       Dest    = pointer to destination array of TIMAGEPOINT
;       Count   = number of entries to transform
;       DeltaX  = translation distance for the X coordinate
;       DeltaY  = translation distance for the Y coordinate
; Output:
;       the maximum Z value
;
tdTransformToImage      PROC PASCAL FAR
        ARG     Source:DWORD,   \
                Dest:DWORD,     \
                Count:WORD,     \
                DeltaX:WORD,    \
                DeltaY:WORD
        LOCAL   Adjust:DWORD,   \
                Max:DWORD
        USES    ds, si, es, di, fs

        mov     ax, SEG MATH_DATA
        mov     ds, ax
        ASSUME  ds:MATH_DATA

        lfs     si, [Source]
        les     di, [Dest]
        mov     [Max], 80000000h

@@Loop:
; Check max Z
        mov     eax, fs:[si].Z
        cmp     eax, [Max]
        jle     @@1
        mov     [Max], eax
@@1:

; Transform X coordinate
        mov     ax, WORD PTR fs:[si].X[2]
        add     ax, [DeltaX]
        mov     es:[di].IX, ax

; Transform Y coordinate
        mov     ax, WORD PTR fs:[si].Y[2]
        add     ax, [DeltaY]
        mov     es:[di].IY, ax

        add     si, SIZE TPOINT
        add     di, SIZE TIMAGEPOINT
        dec     [Count]
        jnz     @@Loop

        mov     eax, [Max]
        shld    edx, eax, 16
        ret
tdTransformToImage      ENDP

;-----------------------------------------------------------
;
; Sets the light source.
;
; Input:
;       Light   = pointer to light vector
; Output:
;       none
;
tdSetLight      PROC PASCAL FAR
        ARG     L:DWORD
        USES    ds, es, di

        mov     ax, SEG MATH_DATA
        mov     ds, ax
        ASSUME  ds:MATH_DATA

        les     di, [L]
        mov     eax, es:[di].X
        mov     [Light.X], eax
        mov     eax, es:[di].Y
        mov     [Light.Y], eax
        mov     eax, es:[di].Z
        mov     [Light.Z], eax

        ret
tdSetLight      ENDP

;-----------------------------------------------------------
;
; Computes light intensity for an array of surfaces.
;
; Input:
;       Normals = pointer to an array of surface normals
;       Lights  = pointer to an array of integer to be filled with
;                 light intensity
;       Count   = number of elements to transform
; Output:
;       none
;
tdTransformLight        PROC PASCAL FAR
        ARG     Normals:DWORD,  \
                Lights:DWORD,   \
                Count:WORD
        USES    ds, si, es, di, fs

        mov     ax, SEG MATH_DATA
        mov     fs, ax
        ASSUME  fs:MATH_DATA

        lds     si, [Normals]
        les     di, [Lights]
        ASSUME  ds:NOTHING

; Intensity is given by the dot product between the Light vector and
; the surface normal
@@Loop:
        mov     eax, ds:[si].Z
        imul    [Light.Z]
        mov     ebx, eax
        mov     ecx, edx
        mov     eax, ds:[si].Y
        imul    [Light.Y]
        add     ebx, eax
        adc     ecx, edx
        mov     eax, ds:[si].X
        imul    [Light.X]
        add     eax, ebx
        adc     edx, ecx                ; EDX:EAX = fixed 32:32 intensity
        add     dx, [AmbientLight]
        test    dx, dx
        jg      @@1
        xor     dx, dx                  ; Return 0 for no light
@@1:
        mov     es:[di], dx
        inc     di
        inc     di
        add     si, SIZE TPOINT
        dec     [Count]
        jnz     @@Loop

        ASSUME  fs:NOTHING
        ret
tdTransformLight        ENDP

;-----------------------------------------------------------
;
; Returns the light value given the normal to a surface.
;
; Input:
;       Normal  = pointer to TPOINT surface normal vector
; Output:
;       AX      = light intensity (>=0)
; Notes:
;       the normal is rotated according to the current setting.
;
tdGetSurfaceLight       PROC PASCAL FAR
        ARG     Normal:DWORD
        USES    ds, esi, es, di

        mov     ax, SEG MATH_DATA
        mov     ds, ax
        ASSUME  ds:MATH_DATA

        les     di, [Normal]

; Transform Z coordinate
        mov     eax, es:[di].X
        imul    [ZRotation.X]
        mov     ecx, eax
        mov     ebx, edx
        mov     eax, es:[di].Y
        imul    [ZRotation.Y]
        add     ecx, eax
        adc     ebx, edx
        mov     eax, es:[di].Z
        imul    [ZRotation.Z]
        add     eax, ecx
        adc     edx, ebx
        shrd    eax, edx, 16
        imul    [Light.Z]
        shrd    eax, edx, 16
        mov     esi, eax

; Transform X coordinate
        mov     eax, es:[di].X
        imul    [XRotation.X]
        mov     ecx, eax
        mov     ebx, edx
        mov     eax, es:[di].Y
        imul    [XRotation.Y]
        add     ecx, eax
        adc     ebx, edx
        mov     eax, es:[di].Z
        imul    [XRotation.Z]
        add     eax, ecx
        adc     edx, ebx
        shrd    eax, edx, 16
        imul    [Light.X]
        shrd    eax, edx, 16
        add     esi, eax

; Transform Y coordinate
        mov     eax, es:[di].X
        imul    [YRotation.X]
        mov     ecx, eax
        mov     ebx, edx
        mov     eax, es:[di].Y
        imul    [YRotation.Y]
        add     ecx, eax
        adc     ebx, edx
        mov     eax, es:[di].Z
        imul    [YRotation.Z]
        add     eax, ecx
        adc     edx, ebx
        shrd    eax, edx, 16
        imul    [Light.X]
        shrd    eax, edx, 16
        add     eax, esi
        shr     eax, 16

; Add ambient light
        add     ax, [AmbientLight]
        test    ax, ax
        jge     @@Exit
        xor     ax, ax

@@Exit:
        ret
tdGetSurfaceLight       ENDP

;-----------------------------------------------------------
;
; Rotates an array of TPOINT.
;
; Input:
;       Source  = pointer to source array of TPOINT
;       Dest    = pointer to destination array of TPOINT
;       Count   = number of entries to transform
; Output:
;       none
;
tdRotate        PROC PASCAL FAR
        ARG     Source:DWORD,   \
                Dest:DWORD,     \
                Count:WORD
        USES    ds, si, es, di, fs

        mov     ax, SEG MATH_DATA
        mov     ds, ax
        ASSUME  ds:MATH_DATA

        lfs     si, [Source]
        les     di, [Dest]

@@Loop:
; Transform Z coordinate
        mov     eax, fs:[si].X
        imul    [ZRotation.X]
        mov     ecx, eax
        mov     ebx, edx
        mov     eax, fs:[si].Y
        imul    [ZRotation.Y]
        add     ecx, eax
        adc     ebx, edx
        mov     eax, fs:[si].Z
        imul    [ZRotation.Z]
        add     eax, ecx
        adc     edx, ebx
        shrd    eax, edx, 16
        mov     es:[di].Z, eax

; Transform X coordinate
        mov     eax, fs:[si].X
        imul    [XRotation.X]
        mov     ecx, eax
        mov     ebx, edx
        mov     eax, fs:[si].Y
        imul    [XRotation.Y]
        add     ecx, eax
        adc     ebx, edx
        mov     eax, fs:[si].Z
        imul    [XRotation.Z]
        add     eax, ecx
        adc     edx, ebx
        shrd    eax, edx, 16
        mov     es:[di].X, eax

; Transform Y coordinate
        mov     eax, fs:[si].X
        imul    [YRotation.X]
        mov     ecx, eax
        mov     ebx, edx
        mov     eax, fs:[si].Y
        imul    [YRotation.Y]
        add     ecx, eax
        adc     ebx, edx
        mov     eax, fs:[si].Z
        imul    [YRotation.Z]
        add     eax, ecx
        adc     edx, ebx
        shrd    eax, edx, 16
        mov     es:[di].Y, eax

        add     si, SIZE TPOINT
        add     di, SIZE TPOINT
        dec     [Count]
        jnz     @@Loop

        ret
tdRotate        ENDP

tdFixedMul      PROC PASCAL FAR
        ARG     F1:DWORD,       \
                F2:DWORD

        mov     eax, [F1]
        imul    [F2]
        shr     eax, 16

        ret
tdFixedMul      ENDP

;-----------------------------------------------------------
;
; Returns in EAX the square root of EDX:EAX.
;
subSqrt PROC NEAR
        push    esi
        push    edi

        add     eax, eax
        adc     edx, 0
        mov     eax, edx                ; Just discard the low bits

        mov     esi, eax
        xor     edi, edi
        shld    edi, esi, 16
        shl     esi, 16
@@Loop:
        mov     ebx, eax
        mul     eax
        add     eax, esi
        adc     edx, edi
        shrd    eax, edx, 1
        shr     edx, 1
        div     ebx
        cmp     eax, ebx
        jne     @@Loop

; Adjust EAX
        shl     eax, 8

        pop     edi
        pop     esi
        ret
subSqrt ENDP

;-----------------------------------------------------------
;
; Finds the unitary normal to a given surface.
;
; Input:
;       Dest            = pointer to TPOINT (vector) result
;       P1, P2, P3      = pointer to TPOINT points on surface
; Output:
;       none
; Notes:
;       the normal is given by the cross-product between (P3-P1) and
;       (P2-P1), so its orientation depends on the parameters order.
;
tdGetNormal     PROC PASCAL FAR
        ARG     Dest:DWORD,     \
                P1:DWORD,       \
                P2:DWORD,       \
                P3:DWORD
        LOCAL   V1:TPOINT,      \
                V2:TPOINT,      \
                N:TPOINT
        USES    ds, si, es, di

; Get vector V1
        lds     si, [P1]
        les     di, [P3]
        mov     eax, es:[di].X
        sub     eax, ds:[si].X
        mov     [V1.X], eax
        mov     eax, es:[di].Y
        sub     eax, ds:[si].Y
        mov     [V1.Y], eax
        mov     eax, es:[di].Z
        sub     eax, ds:[si].Z
        mov     [V1.Z], eax

; Get vector V2
        les     di, [P2]
        mov     eax, es:[di].X
        sub     eax, ds:[si].X
        mov     [V2.X], eax
        mov     eax, es:[di].Y
        sub     eax, ds:[si].Y
        mov     [V2.Y], eax
        mov     eax, es:[di].Z
        sub     eax, ds:[si].Z
        mov     [V2.Z], eax

; Get normal vector (V1 x V2)
        mov     eax, [V1.Z]
        imul    [V2.Y]
        mov     ebx, eax
        mov     ecx, edx
        mov     eax, [V1.Y]
        imul    [V2.Z]
        sub     eax, ebx
        sbb     edx, ecx
        shrd    eax, edx, 16
        mov     [N.X], eax

        mov     eax, [V1.X]
        imul    [V2.Z]
        mov     ebx, eax
        mov     ecx, edx
        mov     eax, [V1.Z]
        imul    [V2.X]
        sub     eax, ebx
        sbb     edx, ecx
        shrd    eax, edx, 16
        mov     [N.Y], eax

        mov     eax, [V1.Y]
        imul    [V2.X]
        mov     ebx, eax
        mov     ecx, edx
        mov     eax, [V1.X]
        imul    [V2.Y]
        sub     eax, ebx
        sbb     edx, ecx
        shrd    eax, edx, 16
        mov     [N.Z], eax

; Get normal length
        mov     eax, [N.X]
        imul    eax
        mov     ebx, eax
        mov     ecx, edx
        mov     eax, [N.Y]
        imul    eax
        add     ebx, eax
        adc     ecx, edx
        mov     eax, [N.Z]
        imul    eax
        add     eax, ebx
        adc     edx, ecx                ; EDX:EAX = N.X*N.X + N.Y*N.Y + N.Z*N.Z
        call    subSqrt                 ; EAX = normal length
        mov     ebx, eax

; Adjust vector and save it
        les     di, [Dest]
        mov     eax, [N.X]
        cdq
        shld    edx, eax, 16
        shl     eax, 16
        idiv    ebx
        mov     es:[di].X, eax
        mov     eax, [N.Y]
        cdq
        shld    edx, eax, 16
        shl     eax, 16
        idiv    ebx
        mov     es:[di].Y, eax
        mov     eax, [N.Z]
        cdq
        shld    edx, eax, 16
        shl     eax, 16
        idiv    ebx
        mov     es:[di].Z, eax

        ret
tdGetNormal     ENDP

TPOLY   STRUC
        Vtx     DW      4 DUP(?)
TPOLY   ENDS

;-----------------------------------------------------------
;
; Performs surface removal on an array of polygons.
;
; Input:
;       Poly    = pointer to an array of TPOLY
;       Vertex  = pointer to an array of TPOINT
;       Dest    = pointer to an array of integer
;       Count   = number of polygons to check
;       Step    = size of TPOLY structure
; Output:
;       if the n-th polygon is invisible the n-th entry of the
;       Dest array is set to -1, other entries are not modified
;       (so it's possible to use the Light array for Dest, because
;       the light intensity is always >= 0)
;
tdBackPlaneCull PROC PASCAL FAR
        ARG     Step:WORD,      \
                Poly:DWORD,     \
                Vertex:DWORD,   \
                Dest:DWORD,     \
                Count:WORD
        USES    ds, si, es, di, fs
        ASSUME  ds:NOTHING

        mov     ds, WORD PTR Vertex[2]
        les     di, [Poly]
        mov     fs, WORD PTR Dest[2]

@@Loop:
        mov     ax, es:[di].Vtx[2]      ; Index of 2nd vertex
        shl     ax, 2
        mov     bx, ax
        shl     ax, 1
        add     bx, ax                  ; BX = index*SIZE TPOINT
        add     bx, WORD PTR [Vertex]   ; BX = offset of 2nd vertex
        mov     ax, es:[di].Vtx[4]      ; Index of 3rd vertex
        shl     ax, 2
        mov     si, ax
        shl     ax, 1
        add     si, ax
        add     si, WORD PTR [Vertex]   ; SI = offset of 3rd vertex
        mov     ecx, ds:[si].X
        sub     ecx, ds:[bx].X          ; ECX = V3.X-V2.X
        mov     edx, ds:[si].Y
        sub     edx, ds:[bx].Y          ; EDX = V3.Y-V2.Y
        mov     ax, es:[di].Vtx[0]      ; Index of 1st vertex
        shl     ax, 2
        mov     si, ax
        shl     ax, 1
        add     si, ax
        add     si, WORD PTR [Vertex]   ; SI = offset of 1st vertex
        mov     eax, ds:[si].X
        sub     eax, ds:[bx].X          ; EAX = V1.X-V2.X
        mov     esi, ds:[si].Y
        sub     esi, ds:[bx].Y          ; ESI = V1.Y-V2.Y
        imul    edx
        mov     ebx, eax
        xchg    ecx, edx                ; ECX:EBX = (V1.X-V2.X)*(V3.Y-V2.Y)
        mov     eax, esi
        imul    edx                     ; EDX:EAX = (V1.Y-V2.Y)*(V3.X-V2.X)
        sub     eax, ebx
        sbb     edx, ecx
        jl      @@Next                  ; Polygon is visible
        mov     bx, WORD PTR [Dest]     ; FS:BX -> current Dest entry
        mov     WORD PTR fs:[bx], -1    ; Remove polygon
@@Next:
        add     WORD PTR [Dest], 2      ; Next entry for dest
        add     di, [Step]              ; Next polygon
        dec     [Count]
        jnz     @@Loop

        ret
tdBackPlaneCull ENDP

MATH_TEXT       ENDS
END
