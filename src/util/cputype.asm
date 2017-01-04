; To assemble the code using MASM 5.10:
;  MASM cputype;
;  LINK cputype;
;  EXE2COM cputype;
;  Version:  1.00b

;  Author:  Ben Lunt (Forever Young Software)

;  Date:  20 Nov 1997

; If you need an EXE to COM converter, you can get one here
;
; The following code won't assemble with NBASM yet, since
;  I have not added all the FPU instructions.  I hope to get to them soon.
;  Until then, you will have to use a different assembler.  Sorry.
;

CodeSeg    segment
           assume cs:CodeSeg, ds:CodeSeg, es:CodeSeg
.186
           org  100h
start:     mov  ax,cs                   ; free unused part of Mem Block
           mov  es,ax                   ;   for .COM file format
           mov  bx,4096                 ;
           mov  ah,4Ah                  ;
           int  21h                     ;

           mov  cx,0121h                ; If CH can be shifted by 21h,
           shl  ch,cl                   ; then it's an 8086, because
           jz   short p1_8086           ; a 186+ limits shift counts.
           push sp                      ; If SP is pushed as its
           pop  ax                      ; original value, then
           cmp  ax,sp                   ; it's a 286+.
           jne  short p1_186
           mov  ax,7000h                ; if bits 12,13,14 are still set
           push ax                      ; after pushing/poping to/from
           popf                         ; the flags register then we have
           pushf                        ; a 386+
           pop  ax
           and  ax,7000h
           cmp  ax,7000h
           jne  short p1_286            ; it's a 386+
.386
           push bp                      ; Align stack to dword
           mov  bp,sp
           and  sp,0FFFCh
           pushfd                       ; Save eflags
           cli                          ; No interrupts
           pushfd                       ; EAX = eflags
           pop  eax
           mov  ebx,eax                 ; EBX = eflags
           xor  eax,40000h              ; Toggle AC bit
           push eax                     ; Eflags = EAX
           popfd
           pushfd                       ; EAX = eflags
           pop  eax
           popfd                        ; Restore eflags
           mov  sp,bp                   ; Restore stack
           pop  bp
           cmp  eax,ebx                 ; If the bit was not
           je   short p1_386            ; reset, it's a 486+
           pushfd                       ; Save eflags
           cli                          ; No interrupts
           pushfd                       ; EAX = eflags
           pop  eax
           xor  eax,200000h             ; Toggle ID bit
           push eax                     ; Eflags = EAX
           popfd
           pushfd                       ; EBX = eflags
           pop  ebx
           popfd                        ; Restore eflags
           cmp  eax,ebx                 ; If the bit was not
           jne  short p1_486            ; reset, it's a 586+
           mov  dx,offset CPUType5      ; 586+
           jmp  short cpudone
p1_486:    mov  dx,offset CPUType4      ; 486
           jmp  short cpudone
p1_386:    mov  dx,offset CPUType3      ; 386
           jmp  short cpudone
p1_286:    mov  dx,offset CPUType2      ; 286
           jmp  short cpudone
p1_186:    mov  dx,offset CPUType1      ; 186
           jmp  short cpudone
p1_8086:   mov  dx,offset CPUType0      ; 8086
cpudone:   mov  ah,09
           int  21h
.186                                    ; make sure we set proc back to 186
.8087      fninit                       ; Initialize FPU
           mov  _Junk,55AAh             ; Set junk value
           fnstsw _Junk                 ; Store status word
           cmp  _Junk,0                 ; If it's not 0, no FPU
           jne  short p2_nofpu
           fnstcw _Junk                 ; Store control word
           mov  ax,_Junk                ; If the bits are not the way
           and  ax,103Fh                ; they should be, no FPU
           cmp  ax,3Fh
           jne  short p2_nofpu

           and  _Junk,0FF7Fh            ; Clear interrupt bit
           fldcw _Junk                  ; Load control word
           fdisi                        ; Disable interrupts
           fstcw _Junk                  ; Store control word
           test _Junk,80h               ; If it changed, it's an 8087
           jnz  short p2_8087
.286                                    ; allowes .287 also
           finit                        ; Re-initialize
           fld1                         ; Divide 1 by 0 to get
           fldz                         ; a positive infinity
           fdiv
           fld  st                      ; Get a negative infinity
           fchs
           fcompp                       ; Compare them
           fstsw ax                     ; Store status word
           sahf                         ; If the FPU thought that they
           je   short p2_287            ; were equal, it's a 287
           mov  dx,offset FPUType3      ; 387
           finit                        ; Init processor
           jmp  short fputypeD
p2_287:    mov  dx,offset FPUType2      ; 287
           finit                        ; Init processor
           jmp  short fputypeD
p2_8087:   mov  dx,offset FPUType0      ; 8087
           finit                        ; Init processor
           jmp  short fputypeD
p2_nofpu:  mov  dx,offset FPUType
fputypeD:  mov  ah,09
           int  21h
.186
.8087
           ret


_Junk      dw  00h
CPUType0   db  13,10,'You have an 8086/88 processor.$'
CPUType1   db  13,10,'You have an 186/88 processor.$'
CPUType2   db  13,10,'You have an 286 processor.$'
CPUType3   db  13,10,'You have an 386 processor.$'
CPUType4   db  13,10,'You have an 486 processor.$'
CPUType5   db  13,10,'You have an 586 or better processor.$'

FPUType    db  13,10,"You don't have a math coprocessor.$"
FPUType0   db  13,10,'You have an 8087 math coprocessor.$'
FPUType2   db  13,10,'You have an 287 math coprocessor.$'
FPUType3   db  13,10,'You have an 387 or better math coprocessor.$'

CodeSeg    ends
           end  start
