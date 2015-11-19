;=======================================================
;===  UTILS.ASM  - Asm Utilities for QuickBasic/BC7  ===
;=======================================================

	PAGE	255, 132

	.MODEL Medium
	.286

	; ==== MACROS ====

	; macros to PUSH and POP multiple registers

PUSHx MACRO R1, R2, R3, R4, R5, R6, R7, R8
	IFNB <R1>
		push	R1				; Save R1
		PUSHx	R2, R3, R4, R5, R6, R7, R8
	ENDIF
ENDM

POPx MACRO R1, R2, R3, R4, R5, R6, R7, R8
	IFNB <R1>
		pop		R1				; Restore R1
		POPx	R2, R3, R4, R5, R6, R7, R8
	ENDIF
ENDM

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


IFDEF FARSTRINGS

	EXTRN	stringaddress:far
	EXTRN	stringlength:far

ENDIF


	.Data

	EVEN

RND_Seed	DW	7397, 29447, 802
RND_Mult	DW	179, 183, 182
RND_ModV	DW	32771, 32779, 32783

CR_LF		DB	13, 10			; the CRLF data

	.Code

;=================
;DOS_PRINT (Text$)
;=================
;
; Prints Text Directly to DOS console w/ CR/LF
;

	PUBLIC	DOS_PRINT

DP_Stack	STRUC
				DW	?x4	; DI, SI, DS, BP
				DD	?	; Caller
	DP_Text		DW	?	; Address of Text$ Descriptor
DP_Stack	ENDS


DOS_PRINT	 PROC	 FAR

	PUSHx	BP, DS, SI, DI		; Preserve Important Registers
	mov		BP, SP				; Set up Stack Frame

	mov     SI, [BP].DP_Text		; Get Addr of Text$ descriptor

IFDEF FARSTRINGS
	push	SI					; Push Addr of BC7 Decriptor Ptr
	call	stringaddress		; Get Address + Len of string!!!
								; DX:AX = Addr  CX = Len
	mov		DS, DX				; DS = DX = Segment of string
	mov		DX, AX				; DX = AX = Offset of String
ELSE
	mov     CX, [SI]         	; put its length into CX
    mov     DX, [SI+02]      	; now DS:DX points to the String
ENDIF

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
	POPx	DI, SI, DS, BP		; Restore Saved Registers
	ret		2					; Exit & Clean Up Stack

DOS_PRINT	 ENDP


;==================
;DOS_PRINTS (Text$)
;==================
; 
; Print Text$ Directly to DOS console 
; without a trailing CR/LF
;

	PUBLIC	DOS_PRINTS

DOS_PRINTS	 PROC	 FAR

	PUSHx	BP, DS, SI, DI		; Preserve Important Registers
	mov		BP, SP				; Set up Stack Frame

    mov     SI, [BP].DP_Text	; Get Addr of Text$ descriptor

IFDEF FARSTRINGS
	push	SI					; Push Addr of BC7 Decriptor Ptr
	call	stringaddress		; Get Address + Len of string!!!
								; DX:AX = Addr  CX = Len
	mov		DS, DX				; DS = DX = Segment of string
	mov		DX, AX				; DX = AX = Offset of String
ELSE
	mov     CX, [SI]         	; put its length into CX
    mov     DX, [SI+02]      	; now DS:DX points to the String
ENDIF

	jcxz	@DPS_Exit			; Don't Print if empty

	mov		BX, 1				; 1= DOS Handle for Display
	mov		AH, 40h				; Write Text Function
	int		21h					; Call DOS to do it

@DPS_Exit:
	cld							; Reset Direction Flag		
	POPx	DI, SI, DS, BP		; Restore Saved Registers
	ret		2					; Exit & Clean Up Stack

DOS_PRINTS	 ENDP


;======================
;SET_VIDEO_MODE (Mode%) 
;======================
;
; Sets the Video Mode through the BIOS
;

	PUBLIC	SET_VIDEO_MODE

SVM_Stack	STRUC
				DW	?x4	; DI, SI, DS, BP
				DD	?	; Caller
	SVM_Mode	DB	?,? ; Desired Video Mode
SVM_Stack	ENDS


SET_VIDEO_MODE	PROC	FAR

	PUSHx	BP, DS, SI, DI		; Preserve Important Registers
	mov		BP, SP				; Set up Stack Frame

	CLR		AH					; Function 0
	mov		AL, [BP].SVM_Mode	; Get Mode #

	int		10H					; Change Video Modes

@SVM_Exit:
	POPx	DI, SI, DS, BP		; Restore Saved Registers
	ret		2					; Exit & Clean Up Stack

SET_VIDEO_MODE	ENDP


;==============
;SCAN_KEYBOARD%
;==============
;
; Function to scan keyboard for a pressed key
;

	PUBLIC	SCAN_KEYBOARD

SCAN_KEYBOARD	PROC	FAR

	PUSHx	BP, DS, SI, DI		; Preserve Important Registers

	mov		AH, 01H				; Function #1
	int		16H					; Call Keyboard Driver
	jz		@SK_NO_KEY			; Exit if Zero flag set

	mov		AH,	00H				; Remove Key from Buffer
	int		16H					; Get Keycode in AX

	or		AL, AL				; Low Byte Set (Ascii?)
	jz		@SK_Exit			; if not, it's a F-Key

	CLR		AH					; Clear ScanCode if Ascii
	jmp		s @SK_Exit			; Return Key in AX

@SK_NO_KEY:
	CLR		AX					; Return Nil (no Keypress)

@SK_Exit:
	cld							; Reset Direction Flag		
	POPx	DI, SI, DS, BP		; Restore Saved Registers
	ret							; Exit & Clean Up Stack

SCAN_KEYBOARD	ENDP


;====================
;RANDOM_INT (MaxInt%)
;====================
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


;===========
;INIT_RANDOM
;===========
;
; Scrambles the psuedo-random number sequence
; (XOR's the seed value with the timer)
;

	PUBLIC	INIT_RANDOM

INIT_RANDOM	PROC	FAR

	clr		AX					; Segment = 0000
	mov		ES, AX
	mov		AX, ES:[046Ch]      ; Get Timer Lo Word

	xor		RND_Seed, AX		; Scramble 1st Seed

	ret							; Exit & Clean Up Stack

INIT_RANDOM	ENDP


;====================
;INT_SQR (X%, Round%)
;====================
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

    PUSHx   BP, DI				; Save BP
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
	shr 	ax, 8				; {shr eax,16}  {to ax (result)}

	POPx	DI, BP				; Restore Registers	
	ret		4					; Exit

INT_SQR		ENDP


;============
;TIMER_COUNT&
;============
;
; Returns the current timer value as an integer/long integer
;


	PUBLIC  TIMER_COUNT

TIMER_COUNT      PROC    FAR

	clr		AX					; Segment = 0000
	mov		ES, AX				; use ES to get at data
	mov		AX, ES:[046Ch]      ; Get Timer Lo Word
	mov		DX, ES:[046Eh]		; Get Timer Hi Word
	ret							; Exit & Return value in DX:AX

TIMER_COUNT      ENDP


	END
