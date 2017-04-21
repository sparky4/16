;=======================================================
;===  C_UTILS.ASM  - Asm Utilities for C/C++         ===
;=======================================================

	PAGE	255, 132

	.MODEL Huge
;	.286

	; ==== MACROS ====

	; macros to PUSH and POP multiple registers

; PUSHx MACRO R1, R2, R3, R4;, R5, R6, R7, R8
; 	IFNB <R1>
; 		push	R1				; Save R1
; 		PUSHx	R2, R3, R4;, R5, R6, R7, R8
; 	ENDIF
; ENDM
;
; POPx MACRO R1, R2, R3, R4;, R5, R6, R7, R8
; 	IFNB <R1>
; 		pop		R1				; Restore R1
; 		POPx	R2, R3, R4;, R5, R6, R7, R8
; 	ENDIF
; ENDM

	; Macro to Clear a Register to 0

CLR MACRO Register
	xor		Register, Register		; Set Register = 0
ENDM

	; Macros to Decrement Counter & Jump on Condition

LOOPx MACRO Register, Destination
	dec		Register				; Counter--
	jnz		Destination				; Jump if not 0
ENDM

LOOPjz MACRO Register, Destination
	dec		Register				; Counter--
	jz		Destination				; Jump if 0
ENDM


	; ==== General Constants ====

	False	EQU	0
	True	EQU	-1
	nil		EQU 0

	b		EQU	BYTE PTR
	w		EQU	WORD PTR
	d		EQU	DWORD PTR
	o		EQU	OFFSET
	f		EQU FAR PTR
	s		EQU	SHORT
	?x4		EQU <?,?,?,?>
	?x3		EQU <?,?,?>
	?x2		EQU <?,?>
	?x1		EQU <?>


	.Data

	EVEN

RND_Seed	DW	7397, 29447, 802
RND_Mult	DW	179, 183, 182
RND_ModV	DW	32771, 32779, 32783

CR_LF		DB	13, 10			; the CRLF data

	.Code

;===========================================
;void far pascal dos_print  (far char *Text)
;===========================================
;
; - Print Text Directly to DOS console w/ CR/LF
;

	PUBLIC	DOS_PRINT

DP_Stack	STRUC
				DW	?x1	; DI, SI, DS, BP
				DW	?x1	; DI, SI, DS, BP
				DW	?x1	; DI, SI, DS, BP
				DW	?x1	; DI, SI, DS, BP
				DD	?	; Caller
	DP_Text		DD	?	; Far Address of Text to print
DP_Stack	ENDS


DOS_PRINT	 PROC	 FAR

	;PUSHx	BP, DS, SI, DI		; Preserve Important Registers
	push bp
	push ds
	push si
	push di
	mov		BP, SP				; Set up Stack Frame

	lds     DX, [BP].DP_Text	; Get Addr of Text$ descriptor

	; Compute Length of string

	CLR		CX					; Length = 0
	mov		SI, DX				; DS:SI = String data

@@DP_Scan_it:

	cmp		b [SI], 0			; Null Byte found?
	je		@@DP_Got_Len		; exit loop if so

	inc		CX					; Len++
	inc		SI					; Point to next char
	jmp		s @@DP_Scan_it		; check again...

@@DP_Got_len:

	jcxz	@No_Print			; Don't Print if empty

	mov		BX, 1				; 1= DOS Handle for Display
	mov		AH, 40h				; Write Text Function
	int		21h					; Call DOS to do it

@No_Print:
	mov		AX, SEG DGROUP		; Restore DGroup
	mov		DS, AX

	mov		DX, o CR_LF			; Get Addr of CR/LF pair
	mov		CX, 2				; 2 Characters to Write
	mov		BX, 1				; 1= DOS Handle for Display

	mov		AH, 40h				; Write Text Function
	int		21h					; Call DOS to do it

	cld							; Reset Direction Flag
	;POPx	DI, SI, DS, BP		; Restore Saved Registers
	pop di
	pop si
	pop ds
	pop bp
	ret		4					; Exit & Clean Up Stack

DOS_PRINT	 ENDP


;===========================================
;void far pascal dos_prints (char far *Text)
;===========================================
;
; Print Text Directly to DOS console
; without a trailing CR/LF
;

	PUBLIC	DOS_PRINTS

DOS_PRINTS	 PROC	 FAR

	;PUSHx	BP, DS, SI, DI		; Preserve Important Registers
	push bp
	push ds
	push si
	push di
	mov		BP, SP				; Set up Stack Frame

	lds     DX, [BP].DP_Text	; Get Addr of Text$ descriptor

	; Compute Length of string

	CLR		CX					; Length = 0
	mov		SI, DX				; DS:SI = String data

@@DPS_Scan_it:

	cmp		b [SI], 0			; Null Byte found?
	je		@@DPS_Got_Len		; exit loop if so

	inc		CX					; Len++
	inc		SI					; Point to next char
	jmp		s @@DPS_Scan_it		; check again...

@@DPS_Got_len:

	jcxz	@DPS_Exit			; Don't Print if empty

	mov		BX, 1				; 1= DOS Handle for Display
	mov		AH, 40h				; Write Text Function
	int		21h					; Call DOS to do it

@DPS_Exit:
	cld	  						; Reset Direction Flag
	;POPx	DI, SI, DS, BP		; Restore Saved Registers
	pop di
	pop si
	pop ds
	pop bp
	ret		2					; Exit & Clean Up Stack

DOS_PRINTS	 ENDP


;=========================================
;void far pascal set_video_mode (int Mode)
;=========================================
;
; Sets the Video Mode through the BIOS
;

	PUBLIC	SET_VIDEO_MODE

SVM_Stack	STRUC
				DW	?x1	; DI, SI, DS, BP
				DW	?x1	; DI, SI, DS, BP
				DW	?x1	; DI, SI, DS, BP
				DW	?x1	; DI, SI, DS, BP
				DD	?	; Caller
	SVM_Mode	DB	?,? ; Desired Video Mode
SVM_Stack	ENDS


SET_VIDEO_MODE	PROC	FAR

	;PUSHx	BP, DS, SI, DI		; Preserve Important Registers
	push bp
	push ds
	push si
	push di
	mov		BP, SP				; Set up Stack Frame

	CLR		AH					; Function 0
	mov		AL, [BP].SVM_Mode	; Get Mode #

	int		10H					; Change Video Modes

@SVM_Exit:
	;POPx	DI, SI, DS, BP		; Restore Saved Registers
	pop di
	pop si
	pop ds
	pop bp
	ret		2					; Exit & Clean Up Stack

SET_VIDEO_MODE	ENDP


;===================================
;int far pascal scan_keyboard (void)
;===================================
;
; Function to scan keyboard for a pressed key
;

	PUBLIC	SCAN_KEYBOARD

SCAN_KEYBOARD	PROC	FAR

	;PUSHx	BP, DS, SI, DI		; Preserve Important Registers
	push bp
	push ds
	push si
	push di

	mov		AH, 01H				; Function #1
	INT		16H					; Call Keyboard Driver
	JZ		@SK_NO_KEY			; Exit if Zero flag set

	mov		AH,	00H				; Remove Key from Buffer
	INT		16H					; Get Keycode in AX

	OR		AL, AL				; Low Byte Set (Ascii?)
	JZ		@SK_Exit			; if not, it's a F-Key

	CLR		AH					; Clear ScanCode if Ascii
	JMP		s @SK_Exit			; Return Key in AX

@SK_NO_KEY:
	CLR		AX					; Return Nil (no Keypress)

@SK_Exit:
	cld							; Reset Direction Flag
	;POPx	DI, SI, DS, BP		; Restore Saved Registers
	pop di
	pop si
	pop ds
	pop bp
	ret							; Exit & Clean Up Stack

SCAN_KEYBOARD	ENDP


;========================================
;int far pascal random_int (int MaxValue)
;========================================
;
; Returns a pseudo-random number in the range of (0.. MaxInt-1)
;


	PUBLIC	RANDOM_INT

RI_Stack	STRUC
				DW	?	; BP
				DD	?	; Caller
	RI_MaxVal	DW	?	; Maximum Value to Return + 1
RI_Stack	ENDS


RANDOM_INT	PROC	FAR

	push	BP					; Preserve Important Registers
	mov		BP, SP				; Set up Stack Frame

   	CLR		BX					; BX is the data index
	CLR		CX		          	; CX is the accumulator

REPT 3
  	mov		AX, RND_Seed[BX]	; load the initial seed
	mul		RND_Mult[BX]		; multiply it
	div		RND_ModV[BX]		; and obtain the Mod value
	mov		RND_Seed[BX], DX	; save that for the next time

	add		CX, DX				; add it into the accumulator
	inc		BX
	inc		BX              	; point to the next set of values
ENDM

	mov		AX, CX				; AX = Random #
	CLR		DX					; DX = 0
	div		[BP].RI_MaxVal		; DX = DX:AX / MAxVal Remainder

	mov		AX, DX

	pop		BP					; Restore BP
	ret		2        			; back to BASIC with AX holding the result

RANDOM_INT	ENDP


;==================================
;void far pascal init_random (void)
;==================================
;
; Scrambles the psuedo-random number sequence
; (XOR's the seed value with the timer)
;

	PUBLIC	INIT_RANDOM

INIT_RANDOM	PROC	FAR

	CLR		AX					; Segment = 0000
	mov		ES, AX
	mov		AX, ES:[046Ch]      ; Get Timer Lo Word

	xor		RND_Seed, AX		; Scramble 1st Seed

	ret							; Exit & Clean Up Stack

INIT_RANDOM	ENDP

;=========================================
;int far pascal int_sqr (int X, int Round)
;=========================================
;
; Returns the Integer Square Root of (X)
; Round allows the return value to be rounded to the
; nearest integer value by passing 0x80.  Passing 0
; return the Integer Portion only.  The rounding amound is
; a number from 0 to 1 multiplied by 256, thus
; 0.5 * 0x100 = 0x80!
;

ISQ_Stack	STRUC
					DW	?,?	; BP, DI
					DD	?	; Caller
	ISQ_Round		DW	?	; Amount to Round Result * 256
	ISQ_X			DW	?	; "X"
ISQ_Stack	ENDS

	PUBLIC	INT_SQR

INT_SQR		PROC	FAR

    ;PUSHx   BP, DI				; Save BP
	push bp
	push di
    mov     BP, SP				; Set up Stack Frame

 	xor 	AX, AX				; {xor eax,eax}
	xor 	DX, DX				; {xor edx,edx}
	mov 	DI, [BP].ISQ_X		; {mov edi,x}

	mov 	CX, 16				; {mov cx, 32}

@ISQ_L:

	shl 	DI, 1				; {shl edi,1}
	rcl 	DX, 1				; {rcl edx,1}
	shl 	DI, 1				; {shl edi,1}
	rcl 	DX, 1				; {rcl edx,1}
	shl 	AX, 1				; {shl eax,1}
	mov 	BX, AX				; {mov ebx,eax}
	shl 	BX, 1				; {shl ebx,1}
	inc 	BX 					; {inc ebx}
	cmp 	DX, BX				; {cmp edx,ebx}
	jl 		@ISQ_S

  	sub 	DX, BX				; {sub edx,ebx}
	inc 	AX 					; {inc eax}

@ISQ_S:
	loop 	@ISQ_L

  	add 	ax, [BP].ISQ_Round	; {add eax,$00008000}
								; {*round* result in hi word: ie. +0.5}
	shr 	ax, 1				; {shr eax,16}  {to ax (result)}
	shr 	ax, 1				; {shr eax,16}  {to ax (result)}
	shr 	ax, 1				; {shr eax,16}  {to ax (result)}
	shr 	ax, 1				; {shr eax,16}  {to ax (result)}
	shr 	ax, 1				; {shr eax,16}  {to ax (result)}
	shr 	ax, 1				; {shr eax,16}  {to ax (result)}
	shr 	ax, 1				; {shr eax,16}  {to ax (result)}
	shr 	ax, 1				; {shr eax,16}  {to ax (result)}

	;POPx	DI, BP				; Restore Registers
	pop di
	pop bp
	ret		4					; Exit

INT_SQR		ENDP

;=================================
;int far pascal timer_count (void)
;=================================
;
; Returns the current timer value as an integer/long integer
;

	PUBLIC  TIMER_COUNT

TIMER_COUNT      PROC    FAR

	CLR		AX					; Segment = 0000
	mov		ES, AX
	mov		AX, ES:[046Ch]      ; Get Timer Lo Word
	mov		DX, ES:[046Eh]		; Get Timer Hi Word
	ret							; Exit & Clean Up Stack

TIMER_COUNT      ENDP


	END
