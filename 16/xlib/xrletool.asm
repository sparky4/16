;-----------------------------------------------------------------------
; MODULE XRLETOOL
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

COMMENT $

Firstly, please note that this module has been built from the ground up
in a rush so although I'm confident all the functions work, I have'nt
extensively checked them. If any should surface please let me know.


This module implements a number of functions comprising an RLE encoding
decoding system.

RLE stands for RUN LENGTH ENCODING. It is a quick simple data compression
scheme which is commonly used for image data compression or compression
of any data. Although not the most efficient system, it is fast, which is
why it is used in image storage systems like PCX. This implementation is
more efficient than the one used in PCX files because it uses 1 bit to
identify a Run Length byte as opposed to two in PCX files, but more on this
later.

This set of functions can be used to implement your own compressed image
file format or for example compress game mapse for various levels etc.
The uses are limited by your imagination.

I opted for trading off PCX RLE compatibility for the improved compression
efficiency.

Here is how the data is un-compressed to give an idea of its structure.


STEP 1 read a byte from the RLE compressed source buffer.

STEP 2 if has its high bit then the lower 7 bits represent the number of
       times the next byte is to be repeated in the destination buffer.
       if the count (lower 7 bits) is zero then
	  we have finished decoding goto STEP 5
       else goto STEP 4

STEP 3 Read a data from the source buffer and copy it directly to the
       destination buffer.
       goto STEP 1

STEP 4 Read a data from the source buffer and copy it to the destination
       buffer the number of times specified by step 2.
       goto STEP 1

STEP 5 Stop, decoding done.

If the byte does not have the high bit set then the byte itself is transfered
 to the destination buffer.

Data bytes that have the high bit already set and are unique in the input
 stream are represented as a Run Length of 1 (ie 81 which includes high bit)
 followed by the data byte.

If your original uncompressed data contains few consecutive bytes and most
have high bit set (ie have values > 127) then your so called
compressed data would require up to 2x the space of the uncompressed data,
so be aware that the compression ratio is extremely variable depending on the
type of data being compressed.

Apologies for this poor attempt at a description, but you can look up
RLE in any good text. Alternatively, any text that describes the PCX file
structure in any depth should have a section on RLE compression.



$

LOCALS
.286

include model.inc
include xrletool.inc

	.data

_RLE_last_buff_offs dw (0)
RLEbuff db 2 dup (?)

	.code

;****************************************************************
;*
;* NAME: x_buff_RLEncode
;*
;*
;* RLE Compresses a source buffer to a destination buffer and returns
;* the size of the resultant compressed data.
;*
;* C PROTOTYPE:
;*
;*  extern unsigned int x_buff_RLEncode(char far * source_buff,
;*      	 char far * dest_buff,unsigned int count);
;*
;* source_buff   - The buffer to compress
;* dest_buff     - The destination buffer
;* count         - The size of the source data in bytes
;*
;* WARNING: buffers must be pre allocated.
;*
proc _x_buff_RLEncode
ARG   src:dword,dest:dword,count:word
	push bp
	mov  bp,sp
	push ds
	push si
	push di

	lds  si,[src]
	les  di,[dest]
	mov  dx,[count]

	push di

	lodsb              ; Load first byte into BL
	mov  bl,al
	xor  cx,cx         ; Set number characters packed to zero
	cld                ; All moves are forward

@@RepeatByte:
	lodsb		   ; Get byte into AL
	inc  cx            ; Increment compressed byte count
	sub  dx,1          ; Decrement bytes left
	je   @@LastByte    ; Finished when dx = 1
	cmp  cx,7fh        ; Filled block yet
	jne  @@NotFilled   ; Nope!

	or   cl,80h        ; Set bit to indicate value is repeat count
	mov  es:[di],cl    ; store it
	inc  di
	xor  cx,cx         ; clear compressed byte count
	mov  es:[di],bl    ; store byte to be repeated
	inc  di

@@NotFilled:
	cmp  al,bl         ; hase there been a byte transition ?
	je   @@RepeatByte  ; No!

	cmp  cl,1          ; do we have a unique byte ?
	jne  @@NotUnique   ; No

	test bl,80h        ; Can this byte be mistaken for repeat count
	jz   @@Unambiguous ; No ! Dont bother with repeat count

@@NotUnique:
	or   cl,80h        ; Set bit to indicate value is repeat count
	mov  es:[di],cl    ; store it
	inc  di
@@Unambiguous:
	xor  cx,cx         ; clear compressed byte count
	mov  es:[di],bl    ; store byte to be repeated
	inc  di
	mov  bl,al         ; move latest byte into bl
	jmp  short @@RepeatByte

@@LastByte:
	cmp  cl,1          ; Is this a unique byte
	jne  @@FinalCount  ; No

	test bl,80h        ; Can this byte be mistaken for repeat count
	jz   @@FinalByte   ; No, so dont bother with the repeat count

@@FinalCount:              ; Output the repeat count
	or   cl,80h
	mov  al,cl
	stosb

@@FinalByte:
	mov  al,bl
	stosb

	mov  al,80h       ; store terminating null length
	stosb

        ; Calculate encoded length of buffer

	mov  ax,di
	pop  di
	sub  ax,di

	pop  di
	pop  si
	pop  ds
	pop  bp
	ret
_x_buff_RLEncode endp



;****************************************************************
;*
;* NAME: x_buff_RLE_size
;*
;*
;* Returns the size the input data would compress to.
;*
;* C PROTOTYPE:
;*
;*  extern unsigned int x_buff_RLE_size(char far * source_buff,
;*      	 unsigned int count);
;*
;* source_buff   - The uncompressed data buffer
;* count         - The size of the source data in bytes
;*
;*
proc _x_buff_RLE_size
ARG   src:dword,count:word
	push bp
	mov  bp,sp
	push ds
	push si
	push di

	lds  si,[src]
	mov  dx,[count]

	xor  di,di

	lodsb              ; Load first byte into BL
	mov  bl,al
	xor  cx,cx         ; Set number characters packed to zero
	cld                ; All moves are forward

@@RepeatByte:
	lodsb		   ; Get byte into AL
	inc  cx            ; Increment compressed byte count
	sub  dx,1          ; Decrement bytes left
	je   @@LastByte    ; Finished when dx = 1
	cmp  cx,7fh        ; Filled block yet
	jne  @@NotFilled   ; Nope!

	add  di,2          ; RL/BYTE pair stub

@@NotFilled:
	cmp  al,bl         ; hase there been a byte transition ?
	je   @@RepeatByte  ; No!

	cmp  cl,1          ; do we have a unique byte ?
	jne  @@NotUnique   ; No

	test bl,80h        ; Can this byte be mistaken for repeat count
	jz   @@Unambiguous ; No ! Dont bother with repeat count

@@NotUnique:
	inc  di            ; RL stub

@@Unambiguous:
	xor  cx,cx         ; clear compressed byte count
	inc  di            ; BYTE stub
	mov  bl,al         ; move latest byte into bl
	jmp  short @@RepeatByte

@@LastByte:
	cmp  cl,1          ; Is this a unique byte
	jne  @@FinalCount  ; No

	test bl,80h        ; Can this byte be mistaken for repeat count
	jz   @@FinalByte   ; No, so dont bother with the repeat count

@@FinalCount:              ; Output the repeat count
	inc  di            ; RL stub

@@FinalByte:
	inc  di            ; BYTE stub
	inc  di            ; RL stub - Account for termiating null
	mov  ax,di

	pop  di
	pop  si
	pop  ds
	pop  bp
	ret
_x_buff_RLE_size endp

;****************************************************************
;*
;* NAME: x_buff_RLDecode
;*
;*
;* Expands an RLE compresses source buffer to a destination buffer.
;* returns the size of the resultant uncompressed data.
;*
;* C PROTOTYPE:
;*
;*  extern unsigned int x_buff_RLDecode(char far * source_buff,
;*      	 char far * dest_buff);
;*
;* source_buff   - The buffer to compress
;* dest_buff     - The destination buffer
;*
;* WARNING: buffers must be pre allocated.
;*
proc _x_buff_RLDecode
ARG   src:dword,dest:dword
LOCAL si_ini:word=LocalStk
	push bp
	mov  bp,sp
        sub  sp,LocalStk
	push ds
	push si
	push di

	mov  dx,-1        ; zero output data buffer size - 1 (compensate for
			  ; terminating null RL)
	xor  cx,cx        ; clear CX
	cld               ; Move forward

	lds  si,[src]     ; point ds:si -> RLE source
	les  di,[dest]    ; point es:di -> uncompressed buffer
        mov  [si_ini],si

@@UnpackLoop:
	lodsb             ; load a byte into AL
	cmp  al,80h       ; is it terminating null RL code
	je   @@done       ; if so jump

	test al,80h       ; is AL a RL code (is high bit set ?)
	jz   @@NoRepeats  ; if not the no RL encoding for this byte, jump

	mov  cl,al        ; set CL to RL (run length) taking care
	xor  cl,80h       ; to remove the bit identifying it as a RL
        add  dx,cx        ; increment buffer size

	lodsb             ; get the next byte which should be a data byte

	shr  cx,1         ; divide RL by 2 to use word stos
	jcxz @@NoRepeats  ; result is zero, jump

	mov  ah,al        ; copy data byte to AH since going to use stosw
	rep  stosw        ; copy AX to outbut buffer RL times
	jnb  @@UnpackLoop ; when we shifted the RL if we had a carry =>
			  ; we had an odd number of repeats so store the
			  ; last BYTE if carry was set otherwise jump
        stosb             ; store AL in destination buffer
        jmp  short @@UnpackLoop

@@NoRepeats:
	inc  dx           ; increment buffer size
	stosb             ; store AL in destination buffer
        jmp  short @@UnpackLoop

@@done:

        mov  bx,si
        sub  bx,[si_ini]
	mov  ax,dx
	pop  di
	pop  si
	pop  ds
        mov  [_RLE_last_buff_offs],bx
        mov  sp,bp
	pop  bp
	ret
_x_buff_RLDecode endp

;==========================================================================
;==========================================================================
; RLEncode to file / RLDecode from file
; WARNING the following functions are *MUCH* slower than the above
; Its best to use the above functions with intermediate buffers where
; disk i/o is concearned... See demo 4
;==========================================================================
;==========================================================================

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; put_cx
;
;   Local utility proc for x_file_RLEncode - write cx to file
;
; Entry:
;       es:dx -> output buffer
;       cx = word to write
;
;
put_cx proc near
	push ds             ; preserve critical registers
	push ax
	push bx
	mov  ax,ds          ; set up DS to output buffers segment
	mov  ds,ax
	mov  word ptr [RLEbuff],cx ; copy CX to output buffer
	mov  ah,40h         ; select "write to file or device" DOS service
	mov  bx,[handle]    ; select handle of file to write
	mov  cx,2           ; sending 2 bytes
	int  21h	    ; call DOS service
	pop  bx             ; recover registers
	pop  ax
	pop  ds
	ret
put_cx endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; put_cx
;
;   Local utility proc for x_file_RLEncode - write cx to file
;
; Entry:
;       es:dx -> output buffer
;       cx = word to write
;
;
put_cl proc near
	push ds             ; preserve critical registers
	push ax
	push bx
	mov  ax,ds          ; set up DS to output buffers segment
	mov  ds,ax
	mov  byte ptr [RLEbuff],cl
	mov  ah,40h         ; select "write to file or device" DOS service
	mov  bx,[handle]    ; select handle of file to write
	mov  cx,1           ; sending 1 byte
	int  21h	    ; call DOS service
	pop  bx             ; recover registers
	pop  ax
	pop  ds
	ret
put_cl endp


;****************************************************************
;*
;* NAME: x_file_RLEncode
;*
;*
;* RLE Compresses a source buffer to an output file returning
;* the size of the resultant compressed data or 0 if it fails.
;*
;* C PROTOTYPE:
;*
;*  extern unsigned int x_file_RLEncode(int handle,
;*      	 char far * source_buff,unsigned int count);
;*
;* source_buff   - The buffer to compress
;* handle        - The file handler
;* count         - The size of the source data in bytes
;*
;*
proc _x_file_RLEncode
ARG   handle:word,src:dword,count:word
LOCAL filesize:word=LocalStk
	push bp
	mov  bp,sp
	sub  sp,LocalStk
	push ds
	push si
	push di

	mov  [filesize],0
	mov  dx,offset [RLEbuff]
	mov  ax,ds
	mov  es,ax
	lds  si,[src]
	mov  di,[count]

	lodsb              ; Load first byte into BL
	mov  bl,al
	xor  cx,cx         ; Set number characters packed to zero
	cld                ; All moves are forward

@@RepeatByte:
	lodsb		   ; Get byte into AL
	inc  cx            ; Increment compressed byte count
	sub  di,1          ; Decrement bytes left
	je   @@LastByte    ; Finished when di = 1
	cmp  cx,7fh        ; Filled block yet
	jne  @@NotFilled   ; Nope!

	or   cl,80h        ; Set bit to indicate value is repeat count
	mov  ch,bl
	add  [filesize],2
	call put_cx
	jb   @@FileError   ; if carry set then file I/O error
	xor  cx,cx         ; clear compressed byte count

@@NotFilled:
	cmp  al,bl         ; hase there been a byte transition ?
	je   @@RepeatByte  ; No!

	cmp  cl,1          ; do we have a unique byte ?
	jne  @@NotUnique   ; No

	test bl,80h        ; Can this byte be mistaken for repeat count
	jz   @@Unambiguous ; No ! Dont bother with repeat count

@@NotUnique:
	or   cl,80h        ; Set bit to indicate value is repeat count
	inc  [filesize]
	call put_cl        ; store it
        jb   @@FileError   ; if carry set then file I/O error
@@Unambiguous:

	mov  cl,bl         ; store byte to be repeated
        inc  [filesize]
	call put_cl
        jb   @@FileError   ; if carry set then file I/O error
	mov  bl,al         ; move latest byte into bl
        xor  cx,cx         ; clear compressed byte count
	jmp  short @@RepeatByte

@@FileError:
	mov  ax,0
	jmp  short @@exit

@@LastByte:
	cmp  cl,1          ; Is this a unique byte
	jne  @@FinalCount  ; No

	test bl,80h        ; Can this byte be mistaken for repeat count
	jz   @@FinalByte   ; No, so dont bother with the repeat count

@@FinalCount:              ; Output the repeat count
	or   cl,80h
        inc  [filesize]
	call put_cl
	jb   @@FileError   ; if carry set then file I/O error

@@FinalByte:
	mov  cl,bl
	mov  ch,80h
	add  [filesize],2
	call put_cx        ; store terminating null length
	jb   @@FileError   ; if carry set then file I/O error

	mov  ax,[filesize]
	jmp  short @@exit

@@exit:
	pop  di
	pop  si
	pop  ds
	mov  sp,bp
	pop  bp
	ret
_x_file_RLEncode endp




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; GET_BYTE
;
;   macro to read a byte from the input file into al
;
GET_BYTE macro
	push bx
	mov  ah,3fh         ; select "read from file or device" DOS service
	mov  bx,[handle]    ; Select handle of file to close
	mov  cx,1           ; Want to read 1 byte
	int  21h            ; call DOS service
	pop  bx
	jb   @@FileError    ; failed if carry flag set
	mov  al,[RLEbuff]
	endm


;****************************************************************
;*
;* NAME: x_file_RLDecode
;*
;*
;* Expands an RLE compresses file to a destination RAM buffer.
;* returns the size of the resultant uncompressed data.
;*
;* C PROTOTYPE:
;*
;*  extern unsigned int x_buff_RLDecode(int handle,
;*      	 char far * dest_buff);
;*
;* handle        - Input file handle
;* dest_buff     - The destination buffer
;*
;*
proc _x_file_RLDecode
ARG   handle:word,dest:dword
	push bp
	mov  bp,sp
	push si
	push di


	mov  bx,-1        ; zero output data buffer size - 1 (compensate for
			  ; terminating null RL)
	mov  dx,offset [RLEbuff] ; setup DS:DX -> RLEBuffer
        xor  cx,cx        ; clear CX
	cld               ; Move forward

	les  di,[dest]    ; point es:di -> uncompressed buffer

@@UnpackLoop:

	GET_BYTE	  ; Load a byte from file into AL

	cmp  al,80h       ; is it terminating null RL code
	je   @@done       ; if so jump

	test al,80h       ; is AL a RL code (is high bit set ?)
	jz   @@NoRepeats  ; if not the no RL encoding for this byte, jump

	mov  cl,al        ; set CL to RL (run length) taking care
	xor  cl,80h       ; to remove the bit identifying it as a RL
	add  bx,cx        ; increment buffer size
	mov  si,cx        ; save the CX value
	GET_BYTE          ; Load a byte from file into AL
	mov  cx,si        ; restore CX value
        shr  cx,1         ; divide RL by 2 to use word stos
	jcxz @@NoRepeats  ; result is zero, jump

	mov  ah,al        ; copy data byte to AH since going to use stosw
	rep  stosw        ; copy AX to outbut buffer RL times
	jnb  @@UnpackLoop ; when we shifted the RL if we had a carry =>
			  ; we had an odd number of repeats so store the
			  ; last BYTE if carry was set otherwise jump
        stosb             ; store AL in destination buffer
        jmp  short @@UnpackLoop

@@NoRepeats:
	inc  bx
	stosb             ; store AL in destination buffer
        jmp  short @@UnpackLoop

@@FileError:
	mov  ax,0
	jmp  short @@exit

@@done:
	mov  ax,bx
@@exit:
	pop  di
	pop  si
	pop  bp
	ret
_x_file_RLDecode endp

	end

