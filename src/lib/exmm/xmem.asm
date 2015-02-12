;-----------------------------------------------------------------------
;Can't take credit for the ASM code here, found it on a local BBS.
;The author has beem lost in the mists of time.
; 
;
.MODEL MEDIUM
 
 
        EXTmemError     EQU     7
        XMSmemError     EQU     8
 
 
        ShortAdr        EQU     0
        LongAdr         EQU     1
 
 
procname        MACRO  Pnam
        PUBLIC  _&Pnam&
_&Pnam&  PROC    FAR
ENDM
 
endproc         MACRO  Pnam
 
_&Pnam&  ENDP
 
ENDM
 
pwrlolvl_TEXT   SEGMENT WORD PUBLIC 'CODE'
 
        ASSUME  CS:pwrlolvl_TEXT, DS:pwrlolvl_TEXT, ES:pwrlolvl_TEXT
 
SUBTTL  (Local Procedure) XMS_setup - find a XMS driver.
PAGE+
 
                EVEN
XMSwordByte     LABEL BYTE
XMSword         DW      0
 
XMSmoveSTRUC    STRUC
 
Length          DW      0
LengthX         DW      0
SrcHandle       DW      0
SrcOffset       DW      0
SrcOffsetX      DW      0
DestHandle      DW      0
DestOffset      DW      0
DestOffsetX     DW      0
 
XMSmoveSTRUC    ENDS
 
XMSmainGET      XMSmoveSTRUC  <>
XMSmainPUT      XMSmoveSTRUC  <>
XMSwordGET      XMSmoveSTRUC  <2,,,,,,OFFSET XMSword>
XMSwordPUT      XMSmoveSTRUC  <2,,,OFFSET XMSword>
 
XMSfunctAdr     DW      0, 0
 
 
; Don't try to call this from your programs
 
XMS_setup               PROC NEAR
 
        PUSH    DS
        PUSH    ES
        PUSH    BX
 
        MOV     AX,CS                   ; Set Data segment to the code
segment.
        MOV     DS,AX                   ;
        MOV     [XMSwordGET.DestOffsetX],AX  ; Set up the move data
structures.
        MOV     [XMSwordPUT.SrcOffsetX],AX   ;
 
        MOV     AX,4300H                ; See if a XMS Driver Exists.
        INT     2FH                     ;
        CMP     AL,80H                  ;
        MOV     AX,0                    ;
        JNE     XMS_setup01             ; Return 0 if not.
 
        MOV     AX,4310H                ; If so, set the driver's function
        INT     2FH                     ;  address.
        MOV     [XMSfunctAdr],BX        ;
        MOV     [XMSfunctAdr+2],ES      ;
 
        MOV     AX,1                    ; Return 1.
 
  XMS_setup01:
        POP     BX
        POP     ES
        POP     DS
 
        RET
 
XMS_setup               ENDP
 
 
SUBTTL  LSHL - Shift an unsigned long left
PAGE+
 
 ;****************************************************************************
 ;* 
 ;* Shift an unsigned long integer left n number of bits.
 ;*
 ;****************************************************************************
 
 ;
 ; Stack frame definition for void LSHL( unsigned long *SHLnumber, unsigned n
);
 ;
 
LSHLparms STRUC
 
        DW      0, 0
        DW      0
SHLadr   DD      ?
SHLn     DW      ?
 
LSHLparms ENDS
 
procname  LSHL
 
        PUSH    BP
        MOV     BP,SP
 
        PUSH    BX
        PUSH    CX
        PUSH    DX
 
        PUSH    DS
        LDS     BX,SHLadr[BP]
        MOV     CX,SHLn[BP]
 
        MOV     AX,[BX]                 ; Get the long integer.
        MOV     DX,[BX+2]               ; 
 
 LSHL_01:
        SHL     AX,1                    ; Do the long shift.
        RCL     DX,1                    ; 
        LOOP    LSHL_01                 ; 
 
        MOV     [BX],AX                 ; Replace the addressed number.
        MOV     [BX+2],DX               ; 
 
        POP     DS
        POP     DX
        POP     CX
        POP     BX
 
        POP     BP
        RET                             ; Exit
 
endproc   LSHL
 
 
SUBTTL  Extended Memory - Stack template for EXTget, EXTput
PAGE+
 
EXTgpparms STRUC
 
        DW      0, 0
        DW      0
extgpBase    DW      ?
extgpblk     DW      ?
extgpblkAdr  DW      ?
extgpBytes   DW      ?
extgpmemAdr  DW      ?
             DW      ?
 
EXTgpparms ENDS
 
 
 
SUBTTL  Extended Memory - XMS - Return total XMS memory.
PAGE+
 
 ; Use this function to detect wether or not XMS driver installed
 ;
 ; Stack frame definition for unsigned XMS_available( void );
 ;
 ;  The total XMS memory available (in 16k blocks) is returned.
 ;
procname  XMS_available
 
        PUSH    BX
        PUSH    CX
        PUSH    DX
 
        CALL    XMS_setup               ; Ensure XMS memory is set.
        TEST    AX,AX                   ;
        JZ      XMS_available01         ; Return zero if not.
 
        MOV     AH,08H                  ; Set the size function code.
        CALL    DWORD PTR CS:[XMSfunctAdr] ; Get the size.
        TEST    AX,AX                   ;
        JZ      XMS_available01         ;
 
        MOV     AX,DX                   ; Set available Kbytes.
        SUB     AX,64                   ; Subtract out the HMA (HIMEM.SYS
bug).
        JNC     XMS_available01         ;
        XOR     AX,AX                   ; Set zero if underflow.
 
  XMS_available01:
        MOV     CL,4                    ; Divide Kbytes by 16 for blocks.
        SHR     AX,CL                   ;
 
        POP     DX
        POP     CX
        POP     BX
 
        RET                             ; Exit
 
endproc   XMS_available
 
SUBTTL  Extended Memory - XMS - Return largest block XMS mem.
PAGE+
 
 ;
 ; Stack frame definition for unsigned XMSblk_available( void );
 ;
 ;  The size of the largest block of XMS memory available,
 ;  (in 16Kbyte blocks) is returned.
 ;
procname  XMSblk_available
 
        PUSH    BX
        PUSH    CX
        PUSH    DX
 
        CALL    XMS_setup               ; Ensure XMS memory is set.
        TEST    AX,AX                   ;
        JZ      XMSblk_available01      ; Return zero if not.
 
        MOV     AH,08H                  ; Set the size function code.
        CALL    DWORD PTR CS:[XMSfunctAdr] ; Get the size.
        TEST    AX,AX                   ;
        JZ      XMSblk_available01      ;
 
        SUB     DX,64                   ; Subtract out the HMA (HIMEM.SYS
bug).
        JNC     XMSblk_available0X      ;
        XOR     DX,DX                   ; Set zero if underflow.
 
 XMSblk_available0X:
        CMP     AX,DX                   ;
        JBE     XMSblk_available01      ;
        MOV     AX,DX                   ; Set available Kbytes.
 
  XMSblk_available01:
        MOV     CL,4                    ; Divide Kbytes by 16 for blocks.
        SHR     AX,CL                   ;
 
        POP     DX
        POP     CX
        POP     BX
 
        RET                             ; Exit
 
endproc   XMSblk_available
 
SUBTTL  Extended Memory - XMS De-allocate a memory block.
PAGE+
 
 ;
 ; Stack frame definition for int XMS_dealloc( int Hdl );
 ;
 ; Zero is returned if the operation fails, non-zero if success.
 ;
 ; its really important to do this, only other way to recover
 ; XMS blocks is to re-boot
 
XMSdealparms STRUC
 
        DW      0, 0
        DW      0
xmsdealHdl  DW      ?
 
XMSdealparms ENDS
 
 
procname  XMS_dealloc
 
        PUSH    BP
        MOV     BP,SP
 
        PUSH    BX
        PUSH    DX
 
;        CALL    XMS_setup               ; Ensure XMS memory is set.
;        TEST    AX,AX                   ;
;        JZ      XMS_dealloc01           ; Return zero if not.
 
        MOV     DX,xmsdealHdl[BP]       ; Get the handle to de-allocate.
        MOV     AH,0AH                  ;
 
        CALL    DWORD PTR CS:[XMSfunctAdr] ; De-allocate it.
 
  XMS_dealloc01:
        POP     DX
        POP     BX
 
        POP     BP
        RET                             ; Exit
 
endproc   XMS_dealloc
 
SUBTTL  Extended Memory - XMS Allocate a memory block.
PAGE+
 
 ;
 ; Stack frame definition for int XMS_alloc( unsigned rsrvd, *size );
 ;
 ;     rsrved and size are in 16K byte blocks.
 ;     rsrved is mem set aside for EMS, generaly zero
 ;
 ;  Zero is returned if the operation fails.
 ;  Block (XMS) handle is returned if success.
 ;
 ;  size - is reduced by the amount of XMS memory actually allocated.
 ;
 
XMSalparms STRUC
 
        DW      0, 0
        DW      0
xmsalrsrvd DW      ?
xmsalsize  DD      ?
 
XMSalparms ENDS
 
procname  XMS_alloc
 
        PUSH    BP
        MOV     BP,SP
 
        PUSH    BX
        PUSH    CX
        PUSH    DX
        PUSH    DI
        PUSH    ES
        PUSH    DS
 
        MOV     AX,CS                   ; Set the data segment to the code
        MOV     DS,AX                   ;  segment.
 
        MOV     CX,4                    ;
        ADD     xmsalrsrvd[BP],CX       ; Subtract out the HMA (HIMEM.SYS
bug).
        SHL     xmsalrsrvd[BP],CL       ; Convert reserved blocks to K-bytes.
 
        LES     DI,xmsalsize[BP]        ; Load size address.
        XOR     AX,AX                   ;
        MOV     BX,ES:[DI]              ; Get the requested size in blocks.
 
        TEST    BX,0F000H               ; Check for more than 64 Megabytes.
        JZ      XMS_alloc01             ;
        MOV     BX,00FFFH               ;
 
  XMS_alloc01:
        MOV     CL,4                    ;
        SHL     BX,CL                   ; Convert to K-Bytes.
        MOV     CX,BX                   ; In CX.
        JZ      XMS_alloc05             ; Return zero if no size requested.
 
;        CALL    XMS_setup               ; Ensure XMS memory is set.
;        TEST    AX,AX                   ;
;        JZ      XMS_alloc05             ; Return zero if not.
 
        XOR     BX,BX                   ;
        MOV     AH,08H                  ; Set to Query Free XMS Memory.
        CALL    DWORD PTR [XMSfunctAdr] ;
 
        SUB     DX,xmsalrsrvd[BP]       ; Subtract out reserved blocks.
        JB      XMS_alloc03             ; Ensure no borrow.
        CMP     AX,DX                   ;
        JBE     XMS_alloc02             ;
        MOV     AX,DX                   ;
 
  XMS_alloc02:
        MOV     DX,AX                   ;
        CMP     AX,68                   ; Ensure enough memory to allocate.
 
  XMS_alloc03:
        MOV     AX,0                    ;
        JB      XMS_alloc05             ; Exit if not.
 
        CMP     BL,80H                  ; Check for errors.
        JE      XMS_alloc05             ;
        CMP     BL,81H                  ;
        JE      XMS_alloc05             ;
 
        CMP     CX,DX                   ; Check actual against requested size.
        JBE     XMS_alloc04             ;
        MOV     CX,DX                   ; Set if actual < requested.
 
  XMS_alloc04:
        MOV     DX,CX                   ; Set requested size.
        MOV     AH,09H                  ;
        CALL    DWORD PTR [XMSfunctAdr] ; Allocate it.
        DEC     AX                      ; Check for errors.
        MOV     AX,0                    ;
        JNZ     XMS_alloc05             ;
 
 
        MOV     AX,CX                   ; Convert allocated size in KBytes
        MOV     CL,4                    ; to allocated blocks.
        SHR     AX,CL                   ;
 
        SUB     ES:[DI],AX              ; Subtract the blocks allocated.
        MOV     AX,DX                   ; Set to return the handle.
 
  XMS_alloc05:
        POP     DS
        POP     ES
        POP     DI
        POP     DX
        POP     CX
        POP     BX
 
        POP     BP
        RET                             ; Exit
 
endproc  XMS_alloc
 
SUBTTL  Extended Memory - XMS get, put Stack Frame definition
PAGE+
 
 
XMSgpparms STRUC
 
        DW      0, 0
        DW      0
xmsgpHdl     DW      ?
xmsgpblk     DW      ?
xmsgpblkAdr  DW      ?
xmsgpBytes   DW      ?
xmsgpmemAdr  DD      ?
 
XMSgpparms ENDS
 
SUBTTL  Extended Memory - XMStoMem
PAGE+
 
 
 ;
 ; Stack frame definition for int XMStoMem( unsigned Handle,
 ;                                          unsigned blk,
 ;                                          unsigned blkAdr,
 ;                                          unsigned Bytes,
 ;                                          char     *memAdr
 ;                                        );
 ;
 ;  XMSmemError is returned if the operation fails, Zero if success.
 ;
 
procname  XMStoMem
 
        PUSH    BP
        MOV     BP,SP
 
        PUSH    BX
        PUSH    CX
        PUSH    DX
        PUSH    SI
        PUSH    DI
        PUSH    ES
        PUSH    DS
 
        MOV     AX,CS                   ; Set Data Segment to Code Segment.
        MOV     DS,AX                   ;
 
        MOV     CX,xmsgpBytes[BP]       ; Get the number of bytes to transfer.
        LES     BX,xmsgpmemAdr[BP]      ; Get the memory address.
        MOV     DX,xmsgpHdl[BP]         ; Get the XMS handle.
        MOV     [XMSmainGET.SrcHandle],DX ; Set it in the move structures.
        MOV     [XMSwordGET.SrcHandle],DX ;
 
        XOR     DX,DX                   ;
        MOV     DI,xmsgpblk[BP]         ; Get the block number.
        SHR     DI,1                    ; Form the 32 bit XMS address in
        RCR     DX,1                    ;  DI:DX.
        SHR     DI,1                    ;
        RCR     DX,1                    ;
        ADD     DX,xmsgpblkAdr[BP]      ;
 
        TEST    CX,1                    ; Check for an odd number of bytes
        JZ      XMStoMem02              ;  to transfer.
 
        DEC     CX                      ; Decrement to an even number of
bytes.
 
        TEST    DX,1                    ; Check for an odd XMS address.
        JZ      XMStoMem01              ;
 
                                        ; XMS address is odd.
                                        ; -------------------
        DEC     DX                      ;
        MOV     [XMSwordGET.SrcOffset],DX   ; Set the XMS address.
        MOV     [XMSwordGET.SrcOffsetX],DI  ;
 
        MOV     AH,0BH                  ; Set the XMS move, function code.
        MOV     SI,OFFSET XMSwordGET    ; Set address of the move structure.
 
        PUSH    BX                      ;
        CALL    DWORD PTR [XMSfunctAdr] ; Call the XMS handler.
        POP     BX                      ;
        DEC     AX                      ; Check for errors.
        JNZ     XMStoMem03              ; Error out if error.
 
        MOV     AX,[XMSword]            ; Get the moved word.
 
        MOV     ES:[BX],AH              ; Move the odd byte to memory.
 
        INC     BX                      ; Reset the memory address.
        ADD     DX,2                    ; And the XMS address.
 
        JMP     XMStoMem02              ; Move the block.
 
 
  XMStoMem01:
                                        ; XMS address is even.
                                        ; --------------------
        ADD     DX,CX                   ;
        MOV     [XMSwordGET.SrcOffset],DX   ; Set the XMS address.
        SUB     DX,CX                       ;
        MOV     [XMSwordGET.SrcOffsetX],DI  ;
 
        MOV     AH,0BH                  ; Set the XMS move, function code.
        MOV     SI,OFFSET XMSwordGET    ; Set address of the move structure.
 
        PUSH    BX                      ;
        CALL    DWORD PTR [XMSfunctAdr] ; Call the XMS handler.
        POP     BX                      ;
        DEC     AX                      ; Check for errors.
        JNZ     XMStoMem03              ; Error out if error.
 
        MOV     AX,[XMSword]            ; Get the moved word.
 
        XCHG    DI,CX                   ;
        MOV     ES:[BX+DI],AL           ; Move the odd byte to memory.
        XCHG    DI,CX                   ;
 
  XMStoMem02:
        JCXZ    XMStoMem04              ; Avoid a zero byte move.
 
        MOV     XMSmainGET.Length,CX    ; Set length for the move.
 
        MOV     XMSmainGET.DestOffset,BX   ; Set Memory address.
        MOV     XMSmainGET.DestOffsetX,ES  ;
 
        MOV     XMSmainGET.SrcOffset,DX    ; Set XMS address.
        MOV     XMSmainGET.SrcOffsetX,DI   ;
 
        MOV     AH,0BH                  ; Set the XMS move, function code.
        MOV     SI,OFFSET XMSmainGET    ; Set address of the move structure.
 
        CALL    DWORD PTR [XMSfunctAdr] ; Call the XMS handler.
        DEC     AX                      ; Check for errors.
        JZ      XMStoMem05
 
  XMStoMem03:
        MOV     AX,XMSmemError          ; Set error code if error.
        JMP     XMStoMem05              ;
 
  XMStoMem04:
        XOR     AX,AX                   ;
 
  XMStoMem05:
        POP     DS
        POP     ES
        POP     DI
        POP     SI
        POP     DX
        POP     CX
        POP     BX
 
        POP     BP
        RET                             ; Exit
 
endproc  XMStoMem
 
SUBTTL  Extended Memory - MemToXMS
PAGE+
 
 
 ;
 ; Stack frame definition for int MemToXMS( unsigned Handle,
 ;                                        unsigned blk,
 ;                                        unsigned blkAdr,
 ;                                        unsigned Bytes,
 ;                                        char     *memAdr
 ;                                       );
 ;
 ;  XMSmemError is returned if the operation fails, Zero if success.
 ;
 
procname  MemToXMS
 
        PUSH    BP
        MOV     BP,SP
 
        PUSH    BX
        PUSH    CX
        PUSH    DX
        PUSH    SI
        PUSH    DI
        PUSH    ES
        PUSH    DS
 
        MOV     AX,CS                   ;
        MOV     DS,AX                   ;
 
        MOV     CX,xmsgpBytes[BP]       ; Get the number of bytes to transfer.
        LES     BX,xmsgpmemAdr[BP]      ; Get the memory address.
        MOV     DX,xmsgpHdl[BP]         ; Get the XMS handle.
        MOV     [XMSmainPUT.DestHandle],DX ; Set it in the move structures.
        MOV     [XMSwordPUT.DestHandle],DX ;
        MOV     [XMSwordGET.SrcHandle],DX  ;
 
        XOR     DX,DX                   ;
        MOV     DI,xmsgpblk[BP]         ; Get the block number.
        SHR     DI,1                    ; Form the 32 bit XMS address in
        RCR     DX,1                    ;  DI:DX.
        SHR     DI,1                    ;
        RCR     DX,1                    ;
        ADD     DX,xmsgpblkAdr[BP]      ;
 
        TEST    CX,1                    ; Check for an odd number of bytes
        JZ      MemToXMS02              ;  to transfer.
 
        DEC     CX                      ; Decrement to an even number of
bytes.
 
        TEST    DX,1                    ; Check for an odd XMS address.
        JZ      MemToXMS01              ;
 
                                        ; XMS address is odd.
                                        ; -------------------
        DEC     DX                      ;
        MOV     [XMSwordGET.SrcOffset],DX   ; Set the XMS address.
        MOV     [XMSwordGET.SrcOffsetX],DI  ;
        MOV     [XMSwordPUT.DestOffset],DX  ;
        MOV     [XMSwordPUT.DestOffsetX],DI ;
 
        MOV     AH,0BH                  ; Set the XMS move, function code.
        MOV     SI,OFFSET XMSwordGET    ; Set address of the move structure.
 
        PUSH    BX                      ;
        CALL    DWORD PTR [XMSfunctAdr] ; Call the XMS handler.
        POP     BX                      ;
        DEC     AX                      ; Check for errors.
        JNZ     MemToXMS03              ; Error out if error.
 
        MOV     AH,ES:[BX]              ; Get the odd memory byte.
 
        MOV     [XMSwordByte+1],AH      ; Put it in the moved word.
 
        MOV     AH,0BH                  ; Set the XMS move, function code.
        MOV     SI,OFFSET XMSwordPUT    ; Set address of the move structure.
 
        PUSH    BX                      ;
        CALL    DWORD PTR [XMSfunctAdr] ; Call the XMS handler.
        POP     BX                      ;
        DEC     AX                      ; Check for errors.
        JNZ     MemToXMS03              ; Error out if error.
 
        INC     BX                      ; Reset the memory address.
        ADD     DX,2                    ; And the XMS address.
 
        JMP     MemToXMS02              ; Move the block.
 
  MemToXMS01:
                                        ; XMS address is even.
                                        ; --------------------
        ADD     DX,CX                   ;
        MOV     [XMSwordGET.SrcOffset],DX   ; Set the XMS address.
        MOV     [XMSwordPUT.DestOffset],DX  ;
        SUB     DX,CX                       ;
        MOV     [XMSwordGET.SrcOffsetX],DI  ;
        MOV     [XMSwordPUT.DestOffsetX],DI ;
 
        MOV     AH,0BH                  ; Set the XMS move, function code.
        MOV     SI,OFFSET XMSwordGET    ; Set address of the move structure.
 
        PUSH    BX                      ;
        CALL    DWORD PTR [XMSfunctAdr] ; Call the XMS handler.
        POP     BX                      ;
        DEC     AX                      ; Check for errors.
        JNZ     MemToXMS03              ; Error out if error.
 
        XCHG    DI,CX                   ;
        MOV     AL,ES:[BX+DI]           ; Get the odd memory byte.
        XCHG    DI,CX                   ;
 
        MOV     [XMSwordByte],AL        ; Set the moved word.
 
        MOV     AH,0BH                  ; Set the XMS move, function code.
        MOV     SI,OFFSET XMSwordPUT    ; Set address of the move structure.
 
        PUSH    BX                      ;
        CALL    DWORD PTR [XMSfunctAdr] ; Call the XMS handler.
        POP     BX                      ;
        DEC     AX                      ; Check for errors.
        JNZ     MemToXMS03              ; Error out if error.
 
  MemToXMS02:
        JCXZ    MemToXMS04              ; Avoid a zero byte move.
 
        MOV     XMSmainPUT.Length,CX    ; Set length for the move.
 
        MOV     XMSmainPUT.SrcOffset,BX    ; Set Memory address.
        MOV     XMSmainPUT.SrcOffsetX,ES   ;
 
        MOV     XMSmainPUT.DestOffset,DX   ; Set XMS address.
        MOV     XMSmainPUT.DestOffsetX,DI  ;
 
        MOV     AH,0BH                  ; Set the XMS move, function code.
        MOV     SI,OFFSET XMSmainPUT    ; Set address of the move structure.
 
        CALL    DWORD PTR [XMSfunctAdr] ; Call the XMS handler.
        DEC     AX                      ; Check for errors.
        JZ      MemToXMS05
 
  MemToXMS03:
        MOV     AX,XMSmemError          ; Set error code if error.
        JMP     MemToXMS05              ;
 
  MemToXMS04:
        XOR     AX,AX                   ;
 
  MemToXMS05:
        POP     DS
        POP     ES
        POP     DI
        POP     SI
        POP     DX
        POP     CX
        POP     BX
 
        POP     BP
        RET                             ; Exit
 
endproc  MemToXMS
 
 
SUBTTL  Last Page
PAGE+
 
pwrlolvl_TEXT   ENDS
 
        END
 
