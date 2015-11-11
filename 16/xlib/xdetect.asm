;-----------------------------------------------------------------------
; MODULE XDETECT
;
; Hardware detection module
;
; Compile with Tasm.
; C callable.
;
;
; ****** XLIB - Mode X graphics library                ****************
; ******                                               ****************
; ****** Written By Themie Gouthas                     ****************
;
; egg@dstos3.dsto.gov.au
; teg@bart.dsto.gov.au
;-----------------------------------------------------------------------
LOCALS
.286

include model.inc
include xdetect.inc

	.data

_MouseButtonCount dw 0
_MouseVersion     dw 0
_MouseType        db 0
_MouseIRQ         db 0

	.code


i86       equ 0
i186      equ 1
i286      equ 2
i386sx    equ 3
i386dx    equ 4
i486      equ 5


NONE      equ 0
MDA       equ 1
CGA       equ 2
EGAMono   equ 3
EGAColor  equ 4
VGAMono   equ 5
VGAColor  equ 6
MCGAMono  equ 7
MCGAColor equ 8

PS2_CARDS db  0,1,2,2,4,3,2,5,6,2,8,7,8


;-----------------------------------------------------------------------
; PC Graphics detection routine. Returns graphics card type
;
; C callable as:
;    unsigned int x_graphics_card();
;
;

proc _x_graphics_card
	push bp                  ; Preserve caller's stack frame
	mov  bp,sp
	mov  ax,1A00h            ; Try calling VGA Identity Adapter function
	int  10h
	cmp  al,1Ah              ; Do we have PS/2 video bios ?
	jne  @@not_PS2           ; No!

	cmp  bl,0Ch              ; bl > 0Ch => CGA hardware
	jg   @@is_CGA            ; Jump if we have CGA
	xor  bh,bh
	xor  ah,ah
	mov  al,cs:PS2_CARDS[bx] ; Load ax from PS/2 hardware table
	jmp  short @@done        ; return ax
@@is_CGA:
	mov  ax,CGA              ; Have detected CGA, return id
	jmp  short @@done
@@not_PS2:                       ; OK We don't have PS/2 Video bios
	mov  ah,12h              ; Set alternate function service
	mov  bx,10h              ; Set to return EGA information
	int  10h                 ; call video service
	cmp  bx,10h              ; Is EGA there ?
	je   @@simple_adapter    ; Nop!
	mov  ah,12h              ; Since we have EGA bios, get details
	mov  bl,10h
	int  10h
	or   bh,bh               ; Do we have colour EGA ?
	jz   @@ega_color         ; Yes
	mov  ax,EGAMono          ; Otherwise we have Mono EGA
	jmp  short @@done
@@ega_color:
	mov  ax,EGAColor         ; Have detected EGA Color, return id
	jmp  short @@done
@@simple_adapter:
	int  11h                 ; Lets try equipment determination service
	and  al,30h
	shr  al,4
	xor  ah,ah
	or   al,al               ; Do we have any graphics card at all ?
	jz   @@done              ; No ? This is a stupid machine!
	cmp  al,3                ; Do We have a Mono adapter
	jne  @@is_CGA            ; No
	mov  ax,MDA              ; Have detected MDA, return id
@@done:
	pop  bp                  ;restore caller's stack frame
	ret
_x_graphics_card endp


;-----------------------------------------------------------------------
; PC Processor detection routine
;
; C callable as:
;    unsigned int x_processor();
;
;
proc _x_processor
	push bp
	mov  bp,sp
	pushf                    ; Save flags
	xor  ax,ax		 ; Clear AX
	push ax                  ; Push it on the stack
	popf                     ; Zero the flags
	pushf                    ; Try to zero bits 12-15
	pop  ax                  ; Recover flags
        and  ax,0F000h           ; If bits 12-15 are 1 => i86 or i286
        cmp  ax,0F000h
        jne  @@1

        push cx                  ; save CX
        mov  ax,0FFFFh           ; Set all AX bits
        mov  cl,33               ; Will shift once on 80186
        shl  ax,cl               ; or 33 x on 8086
        pop  cx
        mov  ax,i186
        jnz  @@done
        mov  ax,i86              ; 0 => 8086/8088
        jmp  short @@done

@@1:
        mov  ax,07000h           ; Try to set bits 12-14
        push ax
        popf
        pushf
        pop  ax
        and  ax,07000h           ; If bits 12-14 are 0 => i286
        mov  ax,i286
        jz   @@done

        ;; 386/486 resolution code taken from WHATCPU.ASM by
        ;; Dave M. Walker


        P386
        mov  eax,cr0
        mov  ebx,eax                 ;Original CR0 into EBX
        or   al,10h                  ;Set bit
        mov  cr0,eax                 ;Store it
        mov  eax,cr0                 ;Read it back
        mov  cr0,ebx                 ;Restore CR0
        test al,10h                  ;Did it set?
        mov  ax,i386sx
        jz   @@done                  ;Jump if 386SX

       ;*** Test AC bit in EFLAGS (386DX won't change)
        mov     ecx,esp                 ;Original ESP in ECX
        pushfd                          ;Original EFLAGS in EBX
        pop     ebx
        and     esp,not 3               ;Align stack to prevent 486
                                        ;  fault when AC is flipped
        mov     eax,ebx                 ;EFLAGS => EAX
        xor     eax,40000h              ;Flip AC flag
        push    eax                     ;Store it
        popfd
        pushfd                          ;Read it back
        pop     eax
        push    ebx                     ;Restore EFLAGS
        popfd
        mov     esp,ecx                 ;Restore ESP
        cmp     eax,ebx                 ;Compare old/new AC bits
        mov     ax,i386dx
        je      @@done
is_486:                                 ;Until the Pentium appears...
        mov   ax,i486
@@done:
	popf
        .286
	pop  bp
	ret
_x_processor endp

.8086
;-----------------------------------------------------------------------
; PC Numeric coprocessor detection routine
;
; C callable as:
;    unsigned int x_coprocessor();
;
;  Based on an article in PC Tech Journal, Aug 87 by Ted Forgeron
;
;  Returns 1 if coprocessor found, zero otherwise

_x_coprocessor   proc
ARG     control:word=StkSize
	push   bp
	mov    bp,sp
	sub    sp,StkSize

	fninit                          ; try to initialize the copro.
	mov    [control],0              ; clear control word variable
        fnstcw control                  ; put control word in memory
	mov    ax,[control]             ;
	cmp    ah,03h                   ; do we have a coprocessor ?
	je     @@HaveCopro              ; jump if yes!
	xor    ax,ax                    ; return 0 since nothing found
	jmp    short @@done
@@HaveCopro:
	mov    ax,1
@@done:
	mov    sp,bp
	pop    bp
	ret
_x_coprocessor   endp


;-----------------------------------------------------------------------
; PC Mouse Driver detection routine
;
; C callable as:
;    unsigned int x_mousedriver();
;
;
;  Returns 1 if mouse driver found, zero otherwise
_x_mousedriver proc
	       push bp
	       mov  bp,sp
	       mov  ax,3533h        ; Get int 33 interrupt vector
	       int  21h             ; Call dos
	       xor  cx,cx           ; Clear "found" flag
	       mov  ax,es           ; Is the vector null (ES==0 && BX==0) ?
	       or   bx,ax
	       jz   @@NoMouseDriver ; Yes! No mouse driver installed - Jump

	       ; Just make absolutely sure the vector points to the mouse
	       ; driver (just in case)

	       xor  ax,ax           ; FUNC 0: Mouse Initialization
	       int   33h
	       or    ax,ax          ; Do we have an installed mouse driver ?
	       jz    @@NoMouseDriver; No ?
	       mov   [_MouseButtonCount],bx

	       mov   ax,24h
	       int   33h
	       mov   [_MouseVersion],bx
	       mov   [_MouseType],ch
	       mov   [_MouseIRQ],cl

	       mov  cx,1            ; Yes! set flag

@@NoMouseDriver:
	       mov  ax,cx           ; Return "found" flag
	       pop  bp
	       ret
_x_mousedriver endp


end