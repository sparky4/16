;-----------------------------------------------------------------------
; MODULE XTEXT
;
; Point functions all MODE X 256 Color resolutions
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


include xlib.inc
include xtext.inc

.data

_FontDriverActive db 0


_CharHeight   db         0
_CharWidth    db         0
_FontPtr      dw  2 dup (0)
_FirstChar    db         0

_UserFontPtr   dw  2 dup (0)
_UserChHeight  db         0
_UserChWidth   db         0
_UserFirstCh   db         0


F8x8Ptr       dw  2 dup (0)
F8x14Ptr      dw  2 dup (0)

; This is a look up table for the mirror image of a byte eg
; a byte with the value 11001010 has a corresponding byte in the table
; 01010011. This is necessary as the VGA rom font bits are the reverse
; order of what we need for the Mode X. If you know a better-faster way
; TELL ME!

MirrorTable  label byte
	db   0,128, 64,192, 32,160, 96,224, 16,144, 80,208, 48,176,112,240
	db   8,136, 72,200, 40,168,104,232, 24,152, 88,216, 56,184,120,248
	db   4,132, 68,196, 36,164,100,228, 20,148, 84,212, 52,180,116,244
	db  12,140, 76,204, 44,172,108,236, 28,156, 92,220, 60,188,124,252
	db   2,130, 66,194, 34,162, 98,226, 18,146, 82,210, 50,178,114,242
	db  10,138, 74,202, 42,170,106,234, 26,154, 90,218, 58,186,122,250
	db   6,134, 70,198, 38,166,102,230, 22,150, 86,214, 54,182,118,246
	db  14,142, 78,206, 46,174,110,238, 30,158, 94,222, 62,190,126,254
	db   1,129, 65,193, 33,161, 97,225, 17,145, 81,209, 49,177,113,241
	db   9,137, 73,201, 41,169,105,233, 25,153, 89,217, 57,185,121,249
	db   5,133, 69,197, 37,165,101,229, 21,149, 85,213, 53,181,117,245
	db  13,141, 77,205, 45,173,109,237, 29,157, 93,221, 61,189,125,253
	db   3,131, 67,195, 35,163, 99,227, 19,147, 83,211, 51,179,115,243
	db  11,139, 75,203, 43,171,107,235, 27,155, 91,219, 59,187,123,251
	db   7,135, 71,199, 39,167,103,231, 23,151, 87,215, 55,183,119,247
	db  15,143, 79,207, 47,175,111,239, 31,159, 95,223, 63,191,127,255

MirrorTableOffs dw         ?
.code

;----------------------------------------------------------------------
; x_text_init    - Initializes the Mode X text driver and sets the
;                  default font (VGA ROM 8x8)
;
; C caller:
;
;  x_text_init()
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
_x_text_init proc
  push bp

  mov  [_FontDriverActive],TRUE
  mov  ax,1130h                   ; AH = BIOS generator function
				  ; AL = BIOS get font pointer subfunction
  push ax                         ; Save Video interrupt function parameters
  mov  bh,3                       ; Select 8x8 VGA ROM font
  int  10h                        ; Call BIOS video interrupt
  mov  word ptr [F8x8Ptr],bp      ; Save 8x8 Font address in FontPtr table
  mov  word ptr [F8x8Ptr+2],es

  mov  word ptr [_FontPtr],bp     ; Default font = 8x8 ROM font
  mov  word ptr [_FontPtr+2],es

  pop  ax                         ; Recall Video interrupt function parameters
  mov  bh,2                       ; Select 8x14 VGA ROM font
  int  10h                        ; Call BIOS video interrupt
  mov  word ptr [F8x14Ptr],bp     ; Save 8x14 Font address in FontPtr table
  mov  word ptr [F8x14Ptr+2],es


  mov  al,8
  mov  [_CharHeight],al            ; Set the font character heights
  mov  [_CharWidth] ,al            ; Set the font character widths

  mov  dx,offset MirrorTable       ; Initialize mirror table offset
  mov  [MirrorTableOffs],dx

  pop  bp
  ret
_x_text_init endp


;----------------------------------------------------------------------
; x_set_font - Mode X Set current font for text drawing
;
; C caller:
;
;  x_set_font(int FontID)
;
; PARAMETERS  FontID    0 = VGA ROM 8x8
;                       1 = VGA ROM 8x14
;                       2 = User defined bitmapped font
;
;
; WARNING: A user font must be registered before setting FontID 2
;
; Written by Themie Gouthas
;----------------------------------------------------------------------

_x_set_font proc
  ARG FontID:word
  push bp
  mov  bp,sp

  xor  dx,dx             ; Clear DX - Mirror table offset (0 for non ROM fonts)
  mov  cx,FontID
  cmp  cx,2

  jne  @@not_userfont     ; Do we have a user font
  mov  ax,[_UserFontPtr]   ; Yes - Activate it
  mov  [_FontPtr],ax

  mov  ax,[_UserFontPtr+2]
  mov  [_FontPtr+2],ax

  mov  al,[_UserChHeight]
  mov  [_CharHeight],al   ; Set the font character heights

  mov  al,[_UserChWidth]
  mov  [_CharWidth],al    ; Set the font character heights

  mov  al,[_UserFirstCh]
  mov  [_FirstChar],al
  jmp  short @@done

@@not_userfont:              ; We have a ROM font

  mov  dx,offset MirrorTable
  mov  [_CharWidth],8        ; Set the font character widths
  mov  [_FirstChar],0        ; Character sets start at ascii 0
  cmp  cx,1                  ; Do we have an 8x14 ROM font
  jne  @@not_8x14font        ; No, we have 8x8 - jump

  mov  ax,[F8x14Ptr]         ; Yes Activate it
  mov  [_FontPtr],ax

  mov  ax,[F8x14Ptr+2]
  mov  [_FontPtr+2],ax

  mov  [_CharHeight],14    ; Set the font character heights
  jmp  short @@done

@@not_8x14font:
  mov  ax,[F8x8Ptr]        ; Activate the 8x8 ROM Font
  mov  [_FontPtr],ax

  mov  ax,[F8x8Ptr+2]
  mov  [_FontPtr+2],ax

  mov  [_CharHeight],8     ; Set the font character heights

@@done:
  mov  [MirrorTableOffs],dx

  pop  bp
  ret
_x_set_font endp


;----------------------------------------------------------------------
; x_register_userfont - Mode X register user font
;
; C caller:
;
;  x_register_userfont(void far *user_font)
;
;
; NOTES  registering a user font deregisters the previous user font
;        User fonts may be at most 8 pixels wide
;
;
; USER FONT STRUCTURE
;
;  Word:  ascii code of first char in font
;  Byte:  Height of chars in font
;  Byte:  Width of chars in font
;  n*h*Byte: the font data where n = number of chars and h = height
;	  of chars
;
; WARNING: The onus is on the program to ensure that all characters
;          drawn whilst this font is active, are within the range of
;          characters defined.
;
;
; UPDATE: Variable width fonts are now available (up to 8 pixels max)
;  If the Width byte in the font header is 0 then it is assumed that
;  the font is variable width. For variable width fonts each characters
;  data is followed by one byte representing the characters pixel width.
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
_x_register_userfont proc
  ARG  FontToRegister:dword
  push bp
  mov  bp,sp
  push si

  mov  ax,word ptr [FontToRegister]
  mov  bx,word ptr [FontToRegister+2]
  add  ax,4
  mov  [_UserFontPtr],ax
  mov  [_UserFontPtr+2],bx

  push ds
  lds  si,[FontToRegister]
  lodsw
  mov  bx,ax
  lodsw
  pop  ds

  mov  [_UserChHeight],al
  mov  [_UserChWidth],ah
  mov  [_UserFirstCh],bl
  pop  si
  pop  bp
  ret
_x_register_userfont endp


_x_get_char_width  proc
  ARG  Chr:byte
  push bp
  mov  bp,sp

  xor  ah,ah
  mov  al,[_CharWidth]
  or   al,al
  jz   @@NotFixed
  pop  bp
  ret

@@NotFixed:
  push si
  mov  al,[_CharHeight]
  mov  bx,ax
  inc  al
  mov  dl,[Chr]                   ; User fonts may have incomplete charsets
  sub  dl,[_FirstChar]            ;  this compensates for fonts not starting at
				  ;  ascii value 0
  mul  dl                         ; Mult AX by character to draw giving offset
				  ; of first character byte in font table
  add  ax,bx
  les  si,dword ptr [_FontPtr]
  add  si,ax
  xor  ah,ah
  mov  al,es:[si]
  pop  si
  pop  bp
  ret
_x_get_char_width  endp


;----------------------------------------------------------------------
; x_char_put - Mode X Draw a text character at the specified location
;
;
; C caller:
;
;  x_char_put(char ch, int x, int y, unsigned ScrnOffs, unsigned Color)
;
; PARAMETERS  ch        char to draw
;             x,y       screen coords at which to draw ch
;             ScrnOffs  Starting offset of page on whih to draw
;	      Color     Color of the text
;
; NOTES:  Uses the current font settings. See SetFont, InitTextDriver,
;         RegisterUserFont
;
; WARNING: InitTextDriver must be called before using this function
;
;
; Written by Themie Gouthas
;----------------------------------------------------------------------
_x_char_put  proc
  ARG  Chr:byte,X:word,Y:word,ScrnOffs:word,Color:word
  LOCAL ScreenInc:word,Hold:word=LocalStk
  push bp
  mov  bp,sp
  sub  sp,LocalStk
  push si
  push di
  push ds

  cld
  mov  ax,[_ScrnLogicalByteWidth] ; AX = Virtual screen width
  mov  bx,ax                      ; copy Virt screen width and decrement
  sub  bx,3                       ; by the max number of bytes (whole or part)
				  ; that a character row may occupy on the screen
  mov  [ScreenInc],bx             ; Save it to the local stack var. SceenInc
  mul  [Y]                        ; Find the starting dest. screen address of
  mov  di,[X]                     ;  the character to draw
  mov  cx,di
  shr  di,2
  add  di,ax
  add  di,[ScrnOffs]              ; Dont forget to compensate for page

  mov  ax,SCREEN_SEG              ; ES:DI -> first screen dest. byte of char
  mov  es,ax

  and  cx,3                       ; CH = 0, CL = Plane of first pixel

  mov  bx,[MirrorTableOffs]       ; set BX to offset of mirror table for XLAT
  mov  al,[_CharHeight]           ; AL = Character height, AH = 0
  xor  ah,ah
  mov  ch,al                      ; CH = Character height

  cmp  [_CharWidth],0
  jne  @@NoWidthByte
  inc  al
@@NoWidthByte:

  mov  dl,[Chr]                   ; User fonts may have incomplete charsets
  sub  dl,[_FirstChar]            ;  this compensates for fonts not starting at
				  ;  ascii value 0
  mul  dl                         ; Mult AX by character to draw giving offset
				  ; of first character byte in font table

  lds  si,dword ptr [_FontPtr]    ; DS:SI -> beggining of required font
  add  si,ax                      ; DS:SI -> first byte of req. char

  mov  dx,SC_INDEX                ; Prepare for VGA out's

@@MainLoop:

  lodsb               ; load character byte into AL
  or   al,al
  jz   @@NoCharPixels ; Dont bother if no pixels to draw

  or   bx,bx          ; if BX=0 -> User font, so no need to mirror data
  jz   @@DontMirror
  push ds
  mov  dx,@data       ; Set DS to the Mirror lookup table's segment
  mov  ds,dx          ; - BX should already contain the offset addr of table
  xlatb               ; AL is now replaced by the corresponding table entry
  pop  ds             ; Restore previous data segment
  mov  dx,SC_INDEX    ; Restore DX

@@DontMirror:
  xor  ah,ah          ; shift the byte for the dest plane and save it
  shl  ax,cl
  mov  [Hold],ax

  mov  ah,al                 ; output high nibble of first byte of shifted char
  and  ah,0fh                ; 4 pixels at a time !
  jnz  @@p1                  ; if nibble has pixels, draw them
  inc  di                    ;  otherwise go to next nibble
  jmp  @@SecondNibble

@@p1:
  mov  al,MAP_MASK
  out  dx,ax
  mov  al,byte ptr [Color]
  stosb

@@SecondNibble:
			     ; output low nibble of first byte of shifted char
  mov  ax,[Hold]
  shl  ax,4
  and  ah,0fh
  jnz  @@p2
  inc  di
  jmp  @@ThirdNibble

@@p2:
  mov  al,MAP_MASK
  out  dx,ax
  mov  al,byte ptr [Color]
  stosb

@@ThirdNibble:
  mov  ax,[Hold]             ; output high nibble of last byte of shifted char
  and  ah,0fh
  jnz  @@p3
  inc  di
  jmp  short  @@NextCharRow

@@p3:
  mov  al,MAP_MASK           ; completing the drawing of one character row
  out  dx,ax
  mov  al,byte ptr [Color]
  stosb

@@NextCharRow:
  add  di,[ScreenInc]        ; Now move to the next screen row and do the same
  dec  ch                    ; any remaining character bytes
  jnz  @@MainLoop

@@done:
  pop  es
  mov  ah,0
  mov  al,es:[_CharWidth]     ; return the character width (for string fuctions
  or   al,al
  jnz  @@FixedSpacing         ;  using this character drawing function).
  lodsb
@@FixedSpacing:

  mov  bx,es
  mov  ds,bx

  pop  di
  pop  si
  mov  sp,bp
  pop  bp
  ret

@@NoCharPixels:
  add  di,3
  add  di,[ScreenInc]        ; Now move to the next screen row and do the same
  dec  ch                    ; any remaining character bytes
  jnz  @@MainLoop
  jmp  short @@done

_x_char_put endp


end

