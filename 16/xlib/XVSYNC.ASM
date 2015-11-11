; MODULE XVSYNC
; Xlib comptible vsync handler
; Written by Tore Bastiansen
; based on REND386 by Dave Stampe and Bernie Roehl

include xlib.inc
include xmain.inc
include xvsync.inc


TIMER_VECT                      equ 08h

PIC_CMD                         equ 20h
NONSPEC_EOI                     equ 20h
TIMER_MODE                      equ 34h
TIMER_CONTROL                   equ 43h
TIMER_0                         equ 40h

LATCH_COUNT                     equ 00h

INT_IN_ADVANCE                  equ 100

DOS_GETVECT                     equ 3500h
DOS_SETVECT                     equ 2500h


.data
_TicksPerSecond         dw 0
_VsyncIntTicks          label dword
VsyncIntTicksLo         dw 0
VsyncIntTicksHi         dw 0
_VsyncPeriod            dw 0             ;Time (in clicks) between each vsync
									 ;1 click = 1.193 microseconds

ClockRate               dw 0             ;Clock rate (in clicks) for timer 0
ClockCounter            dw 0             ;Counts total clicks modulo 65536
UserVsyncHandler        label dword      ;Pointer to user routine called
UserVsyncOffs           dw 0             ;called once each vsync period.
UserVsyncSeg            dw 0
InUserHandler           dw 0
			db 100h dup(?)
LocalStack              label byte       ;Local stack for user handler
StackSeg                dw 0
StackPtr                dw 0


ElapsedVrts            dw 0
VrtsToSkip             dw 1

.code
get_vsync_period proc near
	mov    al,TIMER_MODE            ;Start timer
	out    TIMER_CONTROL,al
	mov    al,0
	out    TIMER_0,al
	out    TIMER_0,al

	WaitVsyncStart

	mov    al,LATCH_COUNT
	out    TIMER_CONTROL,al
	in     al,TIMER_0
	mov    cl,al
	in     al,TIMER_0
	mov    ch,al                    ;cx=65536-clicks

	WaitVsyncStart

	mov    al,LATCH_COUNT
	out    TIMER_CONTROL,al
	in     al,TIMER_0
	mov    dl,al
	in     al,TIMER_0
	mov    dh,al                    ;dx=65536-clicks

	sub    cx,dx                    ;cx=clicks between two vsyncs
	mov    ax,cx                    ;return in ax
	ret
get_vsync_period endp

vsync_int proc far
	pusha                            ;Save regs
	push   ds
	push   es

	mov    ax,@data                 ;Set the right datasegment
	mov    ds,ax
	add    [VsyncIntTicksLo],1      ;Increment _VsyncIntTicks
	adc    [VsyncIntTicksHi],0

        inc    [ElapsedVrts]
        mov    cx,[ElapsedVrts]
        cmp    cx,[VrtsToSkip]
        jl     @@StopClock

	cmp    [_StartAddressFlag],1    ;Change in start address
	jne    @@StopClock

	mov    dx,CRTC_INDEX            ;Yes, set start address
	mov    ax,[_WaitingStartLow]
	mov    bx,[_WaitingStartHigh]
	out    dx,ax
	mov    ax,bx
	out    dx,ax

@@StopClock:
	cli
	mov    al,TIMER_MODE            ;Stop the timer
	out    TIMER_CONTROL,al         ;Dont want any interrupts
	mov    al,255
	out    TIMER_0,al
	out    TIMER_0,al
	sti

	cli
	mov    dx,INPUT_STATUS_0                   ;Wait for vsync
@@WaitVS:
	in     al,dx
	test   al,08h
	jz     @@WaitVS

	mov    al,TIMER_MODE            ;Start timer again
	out    TIMER_CONTROL,al
	mov    ax,[ClockRate]
	out    TIMER_0,al
	mov    al,ah
	out    TIMER_0,al

        cmp    cx,[VrtsToSkip]
        jl     @@PaletteInt

	cmp    [_StartAddressFlag],1    ;Any change in start address ?
	jne    @@PaletteInt

        xor    cx,cx
        mov    [ElapsedVrts],cx

	mov    ax,[_WaitingPelPan]      ;Yes, set pel pan register
	mov    dx,AC_INDEX
	out    dx,al
	mov    al,ah
	out    dx,al
	mov    [_StartAddressFlag],0

@@PaletteInt:
	cmp    [_VsyncPaletteCount],0   ;Any changes in the palette
	je     @@MouseInt
	mov    si, offset _VsyncPaletteBuffer  ;Yes
	mov    cx, [_VsyncPaletteCount]
	mov    ax, [_VsyncPaletteStart]
	mov    dx, DAC_WRITE_INDEX
	out    dx, al
	mov    dx, DAC_DATA

@@DacOutLoop:
	outsb
	outsb
	outsb
	loop    @@DacOutLoop
	mov     [_VsyncPaletteCount],0

@@MouseInt:
	cmp    [_MouseRefreshFlag],1             ; Does the mouse need refresh
	jne    @@UserInt
	call   dword ptr [_MouseVsyncHandler]    ; Yes
											  ;(this is not yet implemented)

@@UserInt:
	cmp    [UserVsyncSeg], 0       ;Is the a user interrupt routine?
	je     short @@Sim182
	cmp    [InUserHandler],0       ;Yes, but is it already active?
	jne    short @@Sim182
	mov    [InUserHandler],1       ;No, mark it as active
	mov    [StackSeg],ss           ;make a local stack
	mov    [StackPtr],sp
	push    ds
	pop     ss
	mov     sp, offset LocalStack
	sti
	call    dword ptr [UserVsyncHandler]
	cli
	mov     sp, [StackPtr]          ;Restore old stack
	mov     ss, [StackSeg]
	mov     [InUserHandler],0       ;Mark as not active

;       SIM 18.2 Hz
@@Sim182:
	mov     ax,[_VsyncPeriod]       ;Count number of clicks
	add     [ClockCounter],ax       ;If it is bigger than 65536
	jnc     short @@DontChainOld
	pop     es                      ;more than 1/18.2 secs has gone
	pop     ds
	popa
	sti
	db     0eah                    ; jmp instruction
	OldTimerInt    dd 0            ; Pointer to old int8 routine
				       ;  Selfmodyfiing code
	;jmp    dword ptr [OldTimerInt] Chain to old
@@DontChainOld:

; CLEAN UP AND RETURN
	mov    al,NONSPEC_EOI
	out    PIC_CMD,al


	pop    es
	pop    ds
	popa
	sti
	iret
vsync_int endp


_x_install_vsync_handler proc
ARG VrtSkipCount:word
        push    bp
        mov     bp,sp
        mov     ax,[VrtSkipCount]
        or      ax,ax
        jnz     @@NonZeroCount
        mov     ax,1
@@NonZeroCount:
        mov     [VrtsToSkip],ax
        mov     [ElapsedVrts],0
	cmp     [_VsyncHandlerActive],TRUE      ;Is it already active
	je      short @@Return
	call    get_vsync_period                ;no, get the vsync period

	mov     [_VsyncPeriod],ax
	sub     ax,INT_IN_ADVANCE               ;We need a little extra
	mov     [ClockRate],ax                  ;time

	mov     dx,18                           ;dx:ax=1193000
	mov     ax,13352
	idiv    [_VsyncPeriod]
	mov     [_TicksPerSecond],ax            ;1193/_VsyncPeriod

	mov     word ptr [_VsyncIntTicks],0
	mov     word ptr [_VsyncIntTicks+2],0

	cli
	mov     ax, DOS_GETVECT+TIMER_VECT      ;Get address of old timer int
	int     21h
	mov     ax,es
	mov     word ptr cs:[OldTimerInt],bx       ;Store in OldTimerInt
	mov     word ptr cs:[OldTimerInt+2],ax

	mov     [_VsyncHandlerActive],TRUE      ;Mark handler as active
	mov     ax,DOS_SETVECT+TIMER_VECT       ;Set the new timer int
	push    ds
	mov     dx,seg vsync_int
	mov     ds,dx
	mov     dx,offset vsync_int
	int     21h
	pop     ds

	mov     al,TIMER_MODE                   ;Reprogram timer 0
	out     TIMER_CONTROL,al
	mov     ax,ClockRate
	out     TIMER_0,al
	mov     al,ah
	out     TIMER_0,al
	sti
@@Return:
        pop     bp
	ret
_x_install_vsync_handler endp

_x_remove_vsync_handler proc
	cmp     [_VsyncHandlerActive],FALSE
	je      short @@Return
	mov     dx, word ptr cs:[OldTimerInt]
	mov     ax, word ptr cs:[OldTimerInt+2]
	push    ds
	mov     ds,ax
	mov     ax,DOS_SETVECT+TIMER_VECT       ;Restore the old timer int
	cli
	int     21h
	pop     ds
	mov     al,TIMER_MODE                   ;Restore timer 0
	out     TIMER_CONTROL,al
	mov     al,0
	out     TIMER_0,al
	out     TIMER_0,al
	sti
@@Return:
	ret
_x_remove_vsync_handler endp


; WARNING:  The user vsync handler cannot use the 386 specific registers
;           (EAX,EBX,ECX,EDX,ESI,EDI,ESP,EBP,FS,GS)
;                       whithout saving them first.
;                       It must not do any drawing.
;                       Only 256 butes of stack is provided.

_x_set_user_vsync_handler proc
ARG handler_proc:dword
	push    bp
	mov     bp,sp
	mov     ax, word ptr [handler_proc]
	mov     dx, word ptr [handler_proc+2]
	cli
	mov     word ptr [UserVsyncHandler],ax
	mov     word ptr [UserVsyncHandler+2],dx
	sti
	pop     bp
	ret
_x_set_user_vsync_handler endp

end
