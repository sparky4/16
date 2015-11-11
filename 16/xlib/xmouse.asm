;-----------------------------------------------------------------------
; MODULE XMOUSE
;
; Mouse functions functions for all MODE X 256 Color resolutions
;
; Compile with Tasm.
; C callable.
;
; ****** XLIB - Mode X graphics library                ****************
; ******                                               ****************
; ****** Written By Themie Gouthas                     ****************
;
; This module is based on Shane Hyde's module of the same name,
; posted to Rec.Games.Programmer, October 92.
;
;
; egg@dstos3.dsto.gov.au
; teg@bart.dsto.gov.au
;
; mouse cursor shape by Tiaan A Geldenhuys
;
;-----------------------------------------------------------------------

COMMENT  $


This is a module implementing very basic mouse functions.

It does not support the full functionality of:

  SPLIT SCREENS
  SCROLLED WINDOWS
  VIRTUAL WINDOWS

	      --------------------------------------

	      MS Mouse Driver Functions

	      Mouse Initialization                 0
	      Show Cursor                          1
	      Hide Cursor                          2
	      Get Mouse Position & Button Status   3
	      Set Mouse Cursor Position            4
	      Get Button Press Information         5
	      Get Button Release Information       6
	      Set Min/Max Horizontal Position      7
	      Set Min/Max Vertical Position        8
	      Define Graphics Cursor Block         9
	      Define Text Cursor                  10
	      Read Mouse Motion Counters          11
	      Define Event Handler                12
	      Light Pen Emulation Mode ON         13
	      Light Pen Emulation Mode OFF        14
	      Set Mouse Mickey/Pixel Ratio        15
	      Conditional Hide Cursor             16
	      Set Double-Speed Threshold          19
	      --------------------------------------
$

include xlib.inc
include xdetect.inc

.data

global   _MouseInstalled    :word
global   _MouseHidden       :word
global   _MouseButtonStatus :word
global   _MouseX            :word
global   _MouseY            :word
global   _MouseFrozen       :byte
global   _MouseColor        :byte

global   _x_define_mouse_cursor :proc
global   _x_show_mouse          :proc
global   _x_hide_mouse          :proc
global   _x_mouse_remove        :proc
global   _x_position_mouse      :proc
global   _x_put_cursor          :proc
global   _x_update_mouse        :proc
global   _x_mouse_init          :proc
global   _x_mouse_window        :proc


ALIGN 2



      InitMouseDef db 00000001b  ; Default mouse mask, note the reverse order
		db 00000011b
		db 00000111b
		db 00001111b
		db 00011111b
		db 00111111b
		db 01111111b
		db 11111111b
		db 00011111b
		db 00011011b
		db 00110001b
		db 00110000b
		db 01100000b
		db 01100000b



COMMENT $

   Old mouse definition

   InitMouseDef db 00000001b  ; Default mouse mask, note the reverse order
		db 00000011b
		db 00000111b
		db 00001111b
		db 00011111b
		db 00111111b
		db 01111111b
		db 11111111b
		db 00011100b
		db 00111100b
		db 01111100b
		db 00000000b
		db 00000000b
		db 00000000b

$

    MouseMask          db 168 dup(?)
    OldHandlerSeg      dw  ?
    OldHandlerOffs     dw  ?
    OldHandlerMask     dw  ?
    OldX               dw  ?
    OldY               dw  ?
    OldScrnOffs        dw  ?

    BGSaveOffs         dw  0

   _MouseInstalled     dw 0     ; Flag indicating whether mouse is installed
   _MouseHidden        dw 0     ; Flag indicating whether mouse is hidden
   _MouseButtonStatus  dw 0     ; Holds current button press information
   _MouseX             dw 0     ; Coords of cursor hot spot
   _MouseY             dw 0
   _MouseFrozen        db 0     ; Mouse motion enable/disable control
   _MouseColor         db 0     ; Mouse cursor colour

   inhandler           db 0
.code

;----------------------------------------------------------------------
; Local function that updates the cursor position
;
; Destroys SI,DI,AX,BX
;
;----------------------------------------------------------------------
proc update_cursor near
   WaitVsyncStart

   mov di,[OldScrnOffs]             ; Delete cursor (restore old background)
   mov ax,[OldY]
   mov bx,[OldX]

   call restorebg

   mov si,[_VisiblePageOffs]        ; Save cursor background
   mov ax,[_MouseY]
   mov bx,[_MouseX]
   mov [OldScrnOffs],si
   mov [OldY],ax
   mov [OldX],bx
   call getbg

   push [_VisiblePageOffs]          ; Draw the cursor
   mov  ax,[_ScrnPhysicalHeight]
   push ax
   mov  ax,0
   push ax
   push [OldY]
   push [OldX]
   call _x_put_cursor
   add  sp,10
   ret
update_cursor endp


;----------------------------------------------------------------------
; x_mouse_init - Initialise Mode X mouse handler
;
; C Prototype
;
;  int x_mouse_init()
;
; This is the first function you must call before using any of the mouse
; functions
;
; WARNING: This function uses and updates "NonVisual_Offset" to allocate
;          video ram for the saved mouse background.
;
; This mouse code uses the fastest possible techniques to save and restore
; mouse backgrounds and to draw the mouse cursor.
;
; LIMITATIONS: No clipping is supported horizontally for the mouse cursor
;              No validity checking is performed for NonVisual_Offs
;
;
; **WARNING** Hide or freeze mouse while drawing using any of the other
;             Modules. VGA register settings are not preserved which will
;             result in unpredictable drawing behavior.
;             If you know the drawing will occur away from the mouse cursor
;             set MouseFrozen to TRUE (1), do your drawing then set it to
;             FALSE (0). Alternatively call "x_hide_mouse", perform your
;             drawing and then call "x_show_mouse"
;
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
_x_mouse_init proc
     push  bp
     mov   bp,sp

     cmp   [_MouseButtonCount],0  ; Dont initialize if mouse detection
     jne   @@DontInitialize       ; or initialization function previously
				  ;   called
     xor   ax,ax                  ; FUNC 0: Mouse Initialization
     int   33h                    ;
     or    ax,ax                  ; Is there a mouse installed ?
     jz    @@Done
     mov   [_MouseButtonCount],bx ; Set the button count

@@DontInitialize:

     mov   [_MouseInstalled],ax
     or    ax,ax                  ; Do we have an installed mouse driver ?
     jz    @@Done                 ; Nop!

     mov   ax,[_NonVisual_Offs];  ; Allocate VRAM for saved background
     mov   BGSaveOffs,ax

     add   ax,14*3
     mov   [_NonVisual_Offs],ax   ; Update NonVisualOffset

     mov   ax,02                  ; FUNC 2: Hide Cursor
     int   33h                    ;  (hide the mouse driver's default cursor)
     mov   _MouseInstalled,TRUE   ; Indicate user mouse driver present

     mov   ax,07h                 ; FUNC 7:Set min/max horizontal position
     mov   cx,0
     mov   dx,[_ScrnPhysicalPixelWidth]
     shl   dx,1                   ; Mult X by 2 as cursor steps by 2 pixels
     int   33h                    ; 0 < X < _ScrnPhysicalPixelWidth

     mov   ax,08h                 ; FUNC 8:Set min/max vertical position
     mov   cx,0
     mov   dx,_ScrnPhysicalHeight
     int   33h                    ; 0 < Y < _ScrnPhysicalHeight

     mov   ax,0fh                 ; FUNC 15: Mouse Hor/Vert resolution
     mov   cx,4                   ; Horiz speed  >> Value => << Speed
     mov   dx,8                   ; Vert Speed
     int   33h

     mov   ax,3                   ; FUNC 3: Get mouse pos & button status
     int   33h
     mov   [_MouseY],dx
     shr   cx,1
     mov   [_MouseX],cx

     mov   ax,12                  ; FUNC 12: Define Event Handler
     mov   bx,seg mouse_handler   ;  ES:DX -> Event handler
     mov   es,bx
     mov   dx,offset mouse_handler
     mov   cx,1fh                 ;  Set handler for all events
     int   33h



     mov   [_MouseHidden],TRUE    ; Mouse initially hidden

     push  ds                     ; Set the default cursor shape
     mov   ax,offset InitMouseDef
     push  ax
     call  _x_define_mouse_cursor
     add   sp,04h

     mov   ax,[_MouseInstalled]   ; Return MouseInstalled flag
@@Done:
     pop   bp
     ret
_x_mouse_init endp

;----------------------------------------------------------------------
; x_mouse_window - Define a mouse window
;
; C Prototype
;
;  void x_mouse_window(int x0, int y0, int x1, int y1);
;
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
_x_mouse_window proc
ARG x0:word,y0:word,x1:word,y1:word
     push  bp
     mov   bp,sp

     mov   ax,7       ; FUNC 7: Set X range
     mov   cx,x0
     shl   cx,1
     mov   dx,x1
     shl   dx,1
     int   33h

     mov   ax,8       ; FUNC 8: Set Y range
     mov   cx,y0
     mov   dx,y1
     int   33h
     pop   bp
     ret
_x_mouse_window endp


;----------------------------------------------------------------------
; x_define_mouse_cursor - Define a mouse cursor from an input bitmask
;
; C Prototype
;
;  void x_define_mouse_cursor(char far *MouseDef, unsigned char MouseColor)
;
; WARNING: This function assumes MouseDef points to 14 bytes.
;
; Note: Bit order is in reverse. ie bit 7 represents pixel 0 ..
;       bit 0 represents pixel 7 in each "MouseDef" byte.
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
_x_define_mouse_cursor proc
ARG MouseDef:dword,MouseColor:byte
     push  bp
     mov   bp,sp

     cmp  [_MouseInstalled],FALSE   ; Check whether we have installed
     je   @@Done                    ;  our mouse handler and leave if not

     mov  al,[MouseColor]           ; Set the mouse pointers color
     mov  [_MouseColor],al

     push  si
     push  di
     push  ds
     mov   ax,ds                ; ES:DI -> Stored plane mask for all
     mov   es,ax                ;   pixel alignments of mouse cursor
     mov   di,offset MouseMask
     lds   si,MouseDef
     xor   cl,cl                ; CL = current alignment (initially zero)
@@AlignmentLoop:
     push  si                   ; save MouseDef ptr for next alignment
     mov   dh,14                ; Init Row counter to Cursor Height
@@RowLoop:
     lodsb                      ; Load first cursor def byte each bit
				;  representing pixel in the row
     xor   ah,ah                ; AH is the shift overflow byte
     shl   ax,cl                ; Shift mask for current alignment

     mov   bl,al                ; store first three nibbles of ax into
     and   bl,0fh               ;  consecutive bytes in the destination
     mov   es:[di],bl           ;  buffer
     inc   di
     shr   al,4
     stosw

     dec   dh                   ; Next row for this alignment if there
     jnz   @@RowLoop            ;  are more to do

     pop   si                   ; point to the start of the cursor def.
     inc   cl                   ; Select next pixel alignment
     cmp   cl,4                 ; If there are more alignments to do
     jne   @@AlignmentLoop      ;   then jump

     pop   ds
     pop   di
     pop   si
@@Done:
     pop   bp
     ret
_x_define_mouse_cursor endp


;----------------------------------------------------------------------
; x_show_mouse - Shows a previously hidden mouse cursor
;
; C Prototype
;
;  void x_show_mouse()
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
_x_show_mouse proc
     push  bp
     mov   bp,sp
     cmp   [_MouseInstalled],FALSE  ; Make sure our handler is installed
     je    @@Done
     cmp   [_MouseHidden],FALSE     ; If not hidden then exit
     je    @@Done
     push  si
     push  di


@@WaitEndOfHandler:               ; Make sure handler not currently active
     mov   cl,[inhandler]
     or    cl,cl
     jnz   @@WaitEndOfHandler


     mov   si,[_VisiblePageOffs]  ; Save mouse background and pos details
     mov   ax,[_MouseY]
     mov   bx,[_MouseX]
     mov   [OldScrnOffs],si
     mov   [OldY],ax
     mov   [OldX],bx
     call  getbg

     push [_VisiblePageOffs]      ; Draw cursor
     push [_ScrnLogicalHeight]
     xor  ax,ax
     push ax
     push [OldY]
     push [OldX]
     call _x_put_cursor
     add  sp,10

     mov   [_MouseHidden],FALSE   ; Indicate mouse cursor no longer hidden

     pop  di
     pop  si
@@Done:
     pop   bp
     ret
_x_show_mouse endp


;----------------------------------------------------------------------
; x_hide_mouse - Hides a previously visible mouse cursor
;
; C Prototype
;
;  void x_hide_mouse()
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
_x_hide_mouse proc
     push  bp
     mov   bp,sp

     cmp   [_MouseInstalled],FALSE   ; Make sure our handler is installed
     je    @@Done
     cmp   [_MouseHidden],FALSE      ; If cursor is already hidden exit
     jne   @@Done
     push  si
     push  di

@@WaitEndOfHandler:            ; Make sure handler not currently active
     mov   cl,[inhandler]
     or    cl,cl
     jnz   @@WaitEndOfHandler

     mov   [_MouseHidden],TRUE       ; Delete mouse cursor
     mov   di,[OldScrnOffs]
     mov   ax,[OldY]
     mov   bx,[OldX]
     call  restorebg
     pop   di
     pop   si
@@Done:
     pop   bp
     ret
_x_hide_mouse endp


;----------------------------------------------------------------------
; x_remove_mouse - removes mouse handler
;
; C Prototype
;
;  void x_remove_mouse()
;
; NOTE: This function MUST be called before quitting the program if
;       a mouse handler has been installed
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
_x_mouse_remove proc
    push  bp
    mov   bp,sp
    cmp   [_MouseInstalled],FALSE  ; Check whether we have installed
    je    @@Done                   ;  our mouse handler
    call  _x_hide_mouse
    mov   ax,12                    ; FUNC 12: Install event handler
    xor   cx,cx                    ; Disable all events
    int   33h
    mov   [_MouseInstalled],FALSE
@@Done:
    pop   bp
    ret
_x_mouse_remove endp


;----------------------------------------------------------------------
; x_position_mouse - Positions the mouse cursor at the specified location
;
; C Prototype
;
;  void x_position_mouse(int x, int y)
;
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
_x_position_mouse proc
ARG  X:word,Y:word
     push  bp
     mov   bp,sp

@@WaitEndOfHandler:               ; Make sure handler not currently active
     mov   bl,[inhandler]

     or    bl,bl
     jnz   @@WaitEndOfHandler

     mov   ax,4
     mov   cx,X
     mov   dx,Y
     mov   [_MouseX],cx
     mov   [_MouseY],dx
     shl   cx,1

     mov   [inhandler],1
     int   33h

     ; The handler doesnt get called so need
     ; to update manually;

     cmp   [_MouseHidden],FALSE
     jne   @@NotVisible
     push  di si
     call  update_cursor
     pop   si di

@@NotVisible:
     mov   [inhandler],0
     pop   bp
     ret
_x_position_mouse endp

;----------------------------------------------------------------------
; x_update_mouse - Forces the mouse position to be updated and cursor
;                  to be redrawn.
;
; C Prototype
;
;  void x_update_mouse()
;
; Note this function is useful when you have set "MouseFrozen" to true.
; Allows the cursor position to be updated manually rather than
; automatically by the installed handler.
;
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
_x_update_mouse proc
     push  bp
     mov   bp,sp
     cmp   [_MouseInstalled],FALSE   ; Make sure our handler is installed
     je    @@Done
     cmp   [_MouseHidden],FALSE      ; If cursor is already hidden exit
     jne   @@Done
     push  si
     push  di
     mov   ax,03h                 ; FUNC 3: get cursor pos / button status
     int   33h                    ; Update position variables first
     shr   cx,1
     mov   [_MouseX],cx
     mov   [_MouseY],dx
     mov   [_MouseButtonStatus],bx    ; Update button status
     call  update_cursor
     pop   di
     pop   si
@@Done:
     pop   bp
     ret
_x_update_mouse endp


;----------------------------------------------------------------------
; x_put_cursor - Draws the mouse cursor
;
; C Prototype
;
; void x_put_cursor(int X, int Y, int TopClip, int BottomClip, WORD ScrnOffs)
;
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
ALIGN 2
_x_put_cursor  proc
ARG X:word,Y:word,TopClip,BottomClip,ScrnOffs
LOCAL Height,TopRow,NextLineIncr:word=LocalStk
	push  bp
	mov   bp,sp
	sub   sp,LocalStk                 ; Create space for local variables
	push  si
	push  di
	push  ds
	mov   ax,@data
	mov   ds,ax
	cld

	mov   ax,14                   ; Get image height and save in AX
	mov   bx,Y
	; cx = top Row

	;;;;; CLIP PROCESSING FOR TOP CLIP BORDER ;;;;;;;;;;;;;;;;;;;;;

	mov   dx,[TopClip]            ; Compare u.l. Y coord with Top
	sub   dx,bx                   ; clipping border
	jle   @@NotTopClip            ; jump if  not clipped from above
	cmp   dx,ax
	jnl   @@NotVisible            ; jump if  is completely obscured
	mov   cx,dx
	sub   ax,dx
	add   bx,dx
	jmp   short @@VertClipDone

	;;;; EXIT FOR COMPLETELY OBSCURED  ;;;;;;;;;;;;;;;;;;;;;;

@@NotVisible:
	pop   ds
	pop   di                          ; restore registers
	pop   si
	mov   sp,bp                       ; dealloc local variables
	pop   bp
	ret

	;;;;; CLIP PROCESSING FOR BOTTOM CLIP BORDER ;;;;;;;;;;;;;;;;;;;

@@NotTopClip:
	mov   dx,[BottomClip]
	sub   dx,bx
	js    @@NotVisible
	mov   cx,0
	cmp   dx,ax
	jg    @@VertClipDone
	inc   dx
	mov   ax,dx

@@VertClipDone:

	mov   [Height],ax
	mov   [TopRow],cx

	mov   ax,SCREEN_SEG               ; Point es to VGA segment
	mov   es,ax

	mov   ax,bx
	mov   cx,[_ScrnLogicalByteWidth]  ; Find required screen address
	mul   cx
	mov   di,ax

	sub   cx,3                        ; Distance to next screen row
	mov   [NextLineIncr],cx


	mov   cx,[X]
	mov   bx,cx
	shr   cx,2
	add   di,cx
	and   bx,3
	add   di,[ScrnOffs]

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


	mov   ax,42
	mul   bx
	mov   si,offset MouseMask
	add   si,ax

	mov   ax,3                        ; Increment DS:BX and DS:SI to
	mul   [TopRow]                    ;  been clipped by the top border
	add   si,ax                       ; by the L.H.S border

	mov   dx,SC_INDEX                 ; Point SC register to map mask
	mov   al,MAP_MASK                 ; in preperation for masking data
	out   dx,al
	inc   dx                          ; Point dx to SC data register
	mov   ah,byte ptr [Height]        ; AH = Scanline loop counter
	mov   bl,[_MouseColor]
@@RowLoop:
	mov   cx,3                        ; Width in bytes across
@@ColLoop:
	lodsb                             ; load plane mask and set MAP MASK
	out   dx,al
	mov   es:[di],bl                  ; store color to specified planes
	inc   di
	loop  @@ColLoop

	add   di,[NextLineIncr]
	dec   ah
	jnz   @@RowLoop

	pop   ds
	pop   di                          ; restore registers
	pop   si
	mov   sp,bp                       ; dealloc local variables
	pop   bp
	ret
_x_put_cursor  endp


;----------------------------------------------------------------------
; getbg - saves cursor background
;
; C Prototype
;
; local function using register parameters
;
; si = screen offset
; ax  = y
; bx  = x
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
ALIGN 2
getbg  proc near

	push  bp
	mov   bp,sp
	push  ds
	cld

	mov   cx,[_ScrnLogicalByteWidth]  ;  by mult. dest Y coord by Screen
	mul   cx                          ;  width then adding screen offset
	add   si,ax                       ; Add Dest Screen Row to di
	sub   cx,3
	shr   bx,2
	add   si,bx
	mov   bx,cx

	mov   di,BGSaveOffs
	mov   ax,SCREEN_SEG               ; Point es to VGA segment
	mov   es,ax
	mov   ds,ax

	mov   dx,GC_INDEX                 ; Set bit mask for all bits from
	mov   ax,BIT_MASK                 ; VGA latches and none from CPU
	out   dx,ax

	mov   dx,SC_INDEX                 ; Point SC register to map mask
	mov   al,MAP_MASK           ; in preperation for masking data
	out   dx,al
	inc   dx
	mov   al,0fh
	out   dx,al

	mov   cx,14
@@Loop:
	movsb
	movsb
	movsb
	add si,bx
	loop @@Loop

mov     dx,GC_INDEX+1 ;restore the bit mask to its default,
	mov     al,0ffh       ; which selects all bits from the CPU
	out     dx,al         ; and none from the latches (the GC
			      ; Index still points to Bit Mask)

	pop   ds
	mov   sp,bp                       ; dealloc local variables
	pop   bp
	ret
getbg  endp

;----------------------------------------------------------------------
; restorebg - restores cursor background
;
; C Prototype
;
; local function using register parameters
;
; di = screen offset
; ax  = y
; bx  = x
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
ALIGN 2
restorebg  proc near
;di = scrn offs
;ax  = y
;bx  = x
	push  bp
	mov   bp,sp
	push  ds
	cld

	mov   cx,[_ScrnLogicalByteWidth]  ;  by mult. dest Y coord by Screen
	mul   cx                          ;  width then adding screen offset
	add   di,ax                       ; Add Dest Screen Row to di
	sub   cx,3
	shr   bx,2
	add   di,bx
	mov   si,BGSaveOffs
	mov   ax,SCREEN_SEG               ; Point es to VGA segment
	mov   es,ax
	mov   ds,ax

	mov   dx,GC_INDEX                 ; Set bit mask for all bits from
	mov   ax,BIT_MASK                 ; VGA latches and none from CPU
	out   dx,ax

	mov   dx,SC_INDEX                 ; Point SC register to map mask
	mov   al,MAP_MASK                 ; in preperation for masking data
	out   dx,al
	inc   dx
	mov   al,0fh
	out   dx,al

	mov   bx,cx
	mov   cx,14
@@Loop:
	movsb
	movsb
	movsb
	add di,bx
	loop @@Loop
	mov  dx,GC_INDEX+1 ;restore the bit mask to its default,
	mov  al,0ffh       ; which selects all bits from the CPU
	out  dx,al         ; and none from the latches (the GC
			   ; Index still points to Bit Mask)
	pop   ds
	pop   bp
	ret
restorebg  endp



;********************** The Mouse event handler *************************

ALIGN 2
mouse_handler proc far
   push  bp
   mov   bp,sp
   push  ds

   mov   di,@data                   ; Make sure DS points to data segment
   mov   ds,di
   cmp   [inhandler],1
   jne   @@NotActive
   pop   ds
   pop   bp
   ret

@@NotActive:
   mov   [inhandler],1
   mov   [_MouseButtonStatus],bx    ; Update button status
   test  ax,1                       ; Is this a motion event ?
   jz    @@Done                     ; No Jump

   shr  cx,1                        ; convert X coord to pixel coords
   mov  [_MouseX],cx                ; save mouse position
   mov  [_MouseY],dx

   cmp  [_MouseHidden],TRUE         ; If mouse hidden dont bother drawing
   je   @@Done

   cmp  [_MouseFrozen],TRUE         ; If mouse hidden dont bother drawing
   je   @@Done

   call update_cursor
@@Done:
   mov  [inhandler],0
   pop  ds
   pop  bp
   ret
mouse_handler endp


end
