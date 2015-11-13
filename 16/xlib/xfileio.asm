;-----------------------------------------------------------------------
; MODULE XFILEIO
;
; Sequential binary file I/O functions
;
; Some functions based on a r.g.p post by Joshua Jensen
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


$

LOCALS
.8086

include model.inc
include xfileio.inc
	.data

	_file_err dw (?)  ; error value

	.code

PUSH_DS macro
    IFNDEF s
    push  ds
    ENDIF
    endm

POP_DS macro
    IFNDEF s
    pop   ds
    ENDIF
    endm

LDS_M macro arg1,arg2
    IFNDEF s
    lds &arg1&,&arg2&
    ELSE
    mov &arg1&,word ptr &arg2&
    ENDIF
    endm



;****************************************************************
;
; name: f_open
;
; C Prototype:
;
;     extern int f_open(char * filename, char access)
;
; Opens a file according to the access char:
;
;   0 = read only   - If doesnt exist return error
;   1 = write only  - If doesnt exist create it otherwise clear it
;   2 = read/write  - If doesnt exist create it
;
; Returns the file handle on success, -1 on failure
;
;
proc _f_open
IFNDEF s
  ARG   filename:dword,access:byte
ELSE
  ARG   filename:word,access:byte
ENDIF
	push  bp             ; Preserve caller's stack frame
	mov   bp,sp
	PUSH_DS
	LDS_M dx,[filename]  ; point DS:DX to file name string
	mov  [_file_err],0
	cmp   [access],1
	je    @@creat

	mov   ah,3dh         ; select "open file" DOS service
	mov   al,[access]    ; select access type code
	int   21h            ; call DOS service
	jnb   @@Done         ; If carry flag set we have failed


	cmp   [access],2
	jne   @@error
@@creat:
	mov   ah,3ch         ; select "creat file" DOS service
	mov   cx,0
	int   21h            ; call DOS service
	jnb   @@Done         ; If carry flag set we have failed
@@error:
	mov   [_file_err],ax
	mov   ax,-1          ;  indicate failure
@@Done:                      ; otherwise return file handle
	POP_DS
	pop  bp              ;restore caller's stack frame
	ret
_f_open endp


;****************************************************************
;
; name: f_close
;
; C Prototype:
;
;     extern int f_close(int handle)
;
; Closes the file associated with the specified handle
;
; Returns 0 on success, -1 on failure
;
proc _f_close
ARG   handle:word
	push bp             ; Preserve caller's stack frame
	mov  bp,sp

        mov  [_file_err],0  ; Clear error
	mov  ah,3eh         ; select  "close file handle" DOS service
	mov  bx,[handle]    ; select handle of file to close
	int  21h            ; call DOS service
	jnb  @@Fix          ; failed if carry flag set
	mov   [_file_err],ax;
	mov  ax,-1          ;  return error
	jmp  short @@Done
@@Fix:                      ; otherwise
	xor  ax,ax          ;  return 0
@@Done:
	pop  bp             ;restore caller's stack frame
	ret
_f_close endp


;****************************************************************
;
; name: f_read
;
; C Prototype:
;
;     extern int f_read(int handle, char  *buffer, int count)
;
; Reads a block of count bytes from the file specified by the handle
; into the buffer
;
; Returns count on success, failure is detectable via _file_err
;
proc _f_read
IFNDEF s
  ARG   handle:word,buffer:dword,count:word
ELSE
  ARG   handle:word,buffer:word,count:word
ENDIF
	push bp             ; Preserve caller's stack frame
	mov  bp,sp
	PUSH_DS

        mov   [_file_err],0  ; Clear error
	mov   ah,3fh         ; select "read from file or device" DOS service
	mov   bx,[handle]    ; select handle of file to close
	mov   cx,[count]
	LDS_M dx,[buffer]
	int   21h            ; call DOS service
	jnb   @@Done         ; failed if carry flag set
	mov   [_file_err],ax
	xor   ax,ax          ;  return error
	jmp   short @@Done
@@Done:
	POP_DS
	pop  bp             ;restore caller's stack frame
	ret
_f_read endp

;****************************************************************
;
; name: f_write
;
; C Prototype:
;
;     extern int f_write(int handle, char *buffer, int count)
;
; Writes a block of count bytes to the file specified by the handle
; from the buffer
;
; Returns count on success, error is indicated by _file_err iff count = 0
;
proc _f_write
IFNDEF s
  ARG   handle:word,buffer:dword,count:word
ELSE
  ARG   handle:word,buffer:word,count:word
ENDIF
	push bp             ; Preserve caller's stack frame
	mov  bp,sp
	PUSH_DS

        mov   [_file_err],0  ; Clear error
	mov   ah,40h         ; select "write to file or device" DOS service
	mov   bx,[handle]    ; select handle of file to write
	mov   cx,[count]
	LDS_M dx,[buffer]
	int   21h            ; call DOS service
	jnb   @@Done         ; has the function failed ?
	mov   [_file_err],ax
	xor   ax,ax          ;  yes, return error
	jmp   short @@Done
@@Done:                      ; otherwise return bytes written
	POP_DS
	pop  bp              ; restore caller's stack frame
	ret
_f_write endp

;****************************************************************
;
; name: f_readfar
;
; C Prototype:
;
;     extern int f_readfar(int handle, char far * buffer, int count)
;
; Reads a block of count bytes from the file specified by the handle
; into the buffer
;
; Returns count on success, failure is detectable via _file_err
;
proc _f_readfar
ARG   handle:word,buffer:dword,count:word
	push bp             ; Preserve caller's stack frame
	mov  bp,sp
	push ds

        mov  [_file_err],0  ; Clear error
	mov  ah,3fh         ; select "read from file or device" DOS service
	mov  bx,[handle]    ; select handle of file to close
	mov  cx,[count]
	lds  dx,[buffer]
	int  21h            ; call DOS service
	jnb  @@Done         ; failed if carry flag set
	mov  [_file_err],ax
	xor  ax,ax          ;  return error
	jmp  short @@Done
@@Done:
	pop  ds
	pop  bp             ;restore caller's stack frame
	ret
_f_readfar endp

;****************************************************************
;
; name: f_writefar
;
; C Prototype:
;
;     extern int f_writefar(int handle, char far * buffer, int count)
;
; Writes a block of count bytes to the file specified by the handle
; from the buffer
;
; Returns count on success, error is indicated by _file_err iff count = 0
;
proc _f_writefar
ARG   handle:word,buffer:dword,count:word
	push bp             ; Preserve caller's stack frame
	mov  bp,sp
	push ds

        mov  [_file_err],0  ; Clear error
	mov  ah,40h         ; select "write to file or device" DOS service
	mov  bx,[handle]    ; select handle of file to write
	mov  cx,[count]
	lds  dx,[buffer]
	int  21h            ; call DOS service
	jnb  @@Done         ; has the function failed ?
	mov  [_file_err],ax
	xor  ax,ax          ;  yes, return error
	jmp  short @@Done
@@Done:                     ; otherwise return bytes written
	pop  ds
	pop  bp             ; restore caller's stack frame
	ret
_f_writefar endp



;****************************************************************
;
; name: f_seek
;
; C Prototype:
;
;   extern long int f_seek(int handle, long int position, char method_code)
;
; Moves the file pointer according to the position and method code
;
; Returns file pointer position on success, -1 on failure
;
proc _f_seek
ARG   handle:word,position:dword,method_code:byte
	push bp             ; Preserve caller's stack frame
	mov  bp,sp

        mov  [_file_err],0  ; Clear error
	mov  ah,42h         ; select "move file pointer" DOS service
	mov  bx,[handle]    ; select handle of file to close
	mov  al,[method_code]
	mov  cx,word ptr [position+2]
	mov  dx,word ptr [position]
	int  21h            ; call DOS service
	jnb  @@Done         ; has the function failed ?
	mov  [_file_err],ax
	mov  ax,-1          ;  yes, return error
	mov  dx,-1          ;
	jmp  short @@Done
@@Done:                     ; otherwise return bytes written
	pop  bp             ; restore caller's stack frame
	ret
_f_seek endp

;****************************************************************
;
; name: f_tell
;
; C Prototype:
;
;   extern long int f_tell(int handle)
;
; Returns file pointer position on success, -1 on failure
;
proc _f_tell
ARG   handle:word,position:dword,method_code:byte
	push bp             ; Preserve caller's stack frame
	mov  bp,sp

	mov  [_file_err],0  ; Clear error
	mov  ah,42h         ; select "move file pointer" DOS service
	mov  bx,[handle]    ; select handle of file to close
	xor  dx,dx
	mov  cx,dx
	int  21h
	jnb  @@Done
	mov  [_file_err],ax
	mov  ax,-1          ;  yes, return error
	mov  dx,-1          ;
	jmp  short @@Done
@@Done:                     ; otherwise return bytes written
	pop  bp             ; restore caller's stack frame
	ret
_f_tell endp


;****************************************************************
;
; name: f_filelength
;
; C Prototype:
;
;   extern long int f_filelength(int handle)
;
; Returns the length of the file associated with the specified handle
;
; Returns file length on success, -1 on failure
;
proc _f_filelength
ARG     handle:word
LOCAL   low:word,high:word=LocalStk
	push bp             ; Preserve caller's stack frame
	mov  bp,sp
	sub  sp,LocalStk

	mov  [_file_err],0  ; Clear error

	; Get ptr's current location in file and save it

	mov  ah,42h         ; select "move file pointer" DOS service
	mov  al,1           ; select "from current location" method
	mov  bx,[handle]    ; select handle of file to close
	xor  cx,cx
	xor  dx,dx
	int  21h
	jb   @@Error
	mov  [low],ax
	mov  [high],dx

	; Get ptr's value at end of file

	mov  ah,42h         ; select "move file pointer" DOS service
	mov  al,2           ; select "from end of file" method
	mov  bx,[handle]    ; select handle of file to close
	xor  cx,cx
	xor  dx,dx
	int  21h
	jb   @@Error

	; Save the results while returning pointer to its previous location

	push ax
	push dx

	mov  ah,42h         ; select "move file pointer" DOS service
	mov  al,0           ; select "from start of file" method
	mov  bx,[handle]    ; select handle of file to close
	mov  cx,[high]
	mov  dx,[low]
	int  21h

	; restore resultant length

	pop  dx
	pop  ax

	jnb   @@Done        ; Was the operation a success ?
@@Error:
	mov  [_file_err],ax
	mov  ax,-1          ;  no, return error
	mov  dx,-1          ;
@@Done:                     ; otherwise return bytes written
	mov  sp,bp
	pop  bp             ; restore caller's stack frame
	ret
_f_filelength endp


	end
