;-----------------------------------------------------------------------
; MODULE XPOINT
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
include xpoint.inc

	.code


;-----------------------------------------------------------------------
; Mode X (256 color mode) write pixel routine.
; No clipping is performed.
;
; Based on code originally published in DDJ Mag by M. Abrash
;
; C near-callable as:
;    void x_put_pix(int X, int Y, int PageOffset, int Color);
;
;

_x_put_pix  proc    
	ARG X:word,Y:word,PgOfs:word,Color:word
	push bp                   ;preserve caller's stack frame
	mov  bp,sp                ;point to local stack frame

	mov  ax,[_ScrnLogicalByteWidth]
	mul  [Y]                  ;offset of pixel's scan line in page
	mov  bx,[X]
	shr  bx,2                 ;X/4 = offset of pixel in scan line
	add  bx,ax                ;offset of pixel in page
	add  bx,[PgOfs]           ;offset of pixel in display memory
	mov  ax,SCREEN_SEG
	mov  es,ax                ;point ES:BX to the pixel's address

	mov  cl,byte ptr [X]
	and  cl,011b              ;CL = pixel's plane
	mov  ax,0100h + MAP_MASK  ;AL = index in SC of Map Mask reg
	shl  ah,cl                ;set only the bit for the pixel's
				  ; plane to 1
	mov  dx,SC_INDEX          ;set the Map Mask to enable only the
	out  dx,ax		  ; pixel's plane

	mov  al,byte ptr [Color]
	mov  es:[bx],al           ;draw the pixel in the desired color

	pop   bp                  ;restore caller's stack frame
	ret
_x_put_pix   endp

;-------------------------------------------------------------------------
; Mode X (320x240, 256 colors) read pixel routine. Works on all VGAs.
; No clipping is performed.
;
; Based on code originally published in DDJ Mag by M. Abrash
;
; C near-callable as:
;    unsigned int x_get_pix(int X, int Y, unsigned int PageBase);


_x_get_pix   proc
	ARG x:word,y:word,PageBase:word
	push bp                   ;preserve caller's stack frame
	mov  bp,sp                ;point to local stack frame

	mov  ax,[_ScrnLogicalByteWidth]
	mul  [Y]                  ;offset of pixel's scan line in page
	mov  bx,[X]
	shr  bx,1
	shr  bx,1                 ;X/4 = offset of pixel in scan line
	add  bx,ax                ;offset of pixel in page
	add  bx,[PageBase]        ;offset of pixel in display memory
	mov  ax,SCREEN_SEG
	mov  es,ax                ;point ES:BX to the pixel's address

	mov  ah,byte ptr [X]
	and  ah,011b              ;AH = pixel's plane
	mov  al,READ_MAP          ;AL = index in GC of the Read Map reg
	mov  dx,GC_INDEX          ;set the Read Map to read the pixel's
	out  dx,ax	          ; plane

	mov  al,es:[bx]           ;read the pixel's color
	sub  ah,ah                ;convert it to an unsigned int

	pop  bp                   ;restore caller's stack frame
        ret
_x_get_pix   endp
        end


	end

