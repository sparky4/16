;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;=-                                                                         -=
;=-                   Tauron VGA Utilities Version 3.0                      -=
;=-                      Released September 20, 1998                        -=
;=-                                                                         -=
;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;=- Copyright (c) 1997, 1998 by Jeff Morgan  =-= This code is FREE provided -=
;=- All Rights Reserved.                     =-= that you put my name some- -=
;=-                                          =-= where in your credits.     -=
;=- DISCLAIMER:                              =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
;=- I assume no responsibility whatsoever for any effect that this package, -=
;=- the information contained therein or the use thereof has on you, your   -=
;=- sanity, computer, spouse, children, pets or anything else related to    -=
;=- you or your existance. No warranty is provided nor implied with this    -=
;=- source code.                                                            -=
;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

SETVIDEOMODE PROC
   PUSH BP
   MOV BP,SP

   MOV AX,word ptr [BP+4]
   MOV Mode.videomode, AX
   CMP AX,VMODE_00H
   JE SET00H
   CMP AX,VMODE_03H
   JE SET03H
   CMP AX,VMODE_04H
   JE SET04H
   CMP AX,VMODE_06H
   JE SET06H
;   CMP AX,VMODE_07H
;   JE SET07H
   CMP AX,VMODE_0DH
   JE SET0DH
   CMP AX,VMODE_0EH
   JE SET0EH
;   CMP AX,VMODE_0FH
;   JE SET0FH
   CMP AX,VMODE_10H
   JE SET10H
   CMP AX,VMODE_11H
   JE SET11H
   CMP AX,VMODE_12H
   JE SET12H
   CMP AX,VMODE_13H
   JE SET13H
   CMP AX,VMODE_CHAIN4
   JE SETCHAIN4
   CMP AX,VMODE_MODE_X
   JE SETMODE_X
   CMP AX,VMODE_MODE_A
   JE SETMODE_A
   CMP AX,VMODE_MODE_B
   JE SETMODE_B
   CMP AX,VMODE_MODE_C
   JE SETMODE_C
   CMP AX,VMODE_MODE_D
   JE SETMODE_D
   CMP AX,VMODE_MODE_E
   JE SETMODE_E
   CMP AX,VMODE_MODE_F
   JE SETMODE_F
   CMP AX,VMODE_MODE_G
   JE SETMODE_G
   CMP AX,VMODE_MODE_H
   JE SETMODE_H
   CMP AX,VMODE_MODE_I
   JE SETMODE_I
   CMP AX,VMODE_MODE_J
   JE SETMODE_J
   CMP AX,VMODE_MODE_K
   JE SETMODE_K
   CMP AX,VMODE_MODE_L
   JE SETMODE_L
   CMP AX,VMODE_MODE_M
   JE SETMODE_M
   JMP SET_OUT

SET00H:
   MOV SI,OFFSET MODE00H
   CALL SETMODE

   PUSH 16                 ; Bytes per character
   PUSH 0                  ; Offset of font in Video Memory
   PUSH 0A000H
   PUSH FONT_8x14_OFF      ; Offset of Font in BIOS
   PUSH 0C000H
   CALL LOADFONT
   CALL SETPALETTE16
   MOV Mode.mode_width,40;
   MOV Mode.mode_height,25;
   MOV Mode.width_bytes,1000;
   MOV Mode.colors,16;
   MOV Mode.attrib,TVU_TEXT;
   JMP SET_OUT

SET03H:
   MOV SI,OFFSET MODE03H
   CALL SETMODE

   PUSH 16                ; Bytes per character
   PUSH 0                  ; Offset of font in Video Memory
   PUSH 0A000H
   PUSH FONT_8x14_OFF      ; Offset of Font in BIOS
   PUSH 0C000H
   CALL LOADFONT
;   CALL SETPALETTE16
   MOV Mode.mode_width,80;
   MOV Mode.mode_height,25;
   MOV Mode.width_bytes,2000;
   MOV Mode.colors,16;
   MOV Mode.attrib,TVU_TEXT;
   JMP SET_OUT

SET04H:
   MOV SI,OFFSET MODE04H
   CALL SETMODE
   CALL SETPALETTE4
   MOV Mode.mode_width,320;
   MOV Mode.mode_height,200;
   MOV Mode.width_bytes,65535;
   MOV Mode.colors,4;
   MOV Mode.attrib,TVU_GRAPHICS;
   OR Mode.attrib,TVU_B8000_ADDR
   JMP SET_OUT

SET06H:
   MOV SI,OFFSET MODE06H
   CALL SETMODE
   CALL SETPALETTE4
   MOV Mode.mode_width,640;
   MOV Mode.mode_height,200;
   MOV Mode.width_bytes,16384;
   MOV Mode.colors,4;
   MOV Mode.attrib,TVU_GRAPHICS;
   OR Mode.attrib,TVU_B8000_ADDR
   OR Mode.attrib,TVU_PLANAR
   JMP SET_OUT

SET07H:
   MOV SI,OFFSET MODE07H
   CALL SETMODE

   PUSH 16                ; Bytes per character
   PUSH 0                  ; Offset of font in Video Memory
   PUSH 0A000H
   PUSH FONT_8x14_OFF      ; Offset of Font in BIOS
   PUSH 0C000H
   CALL LOADFONT
   MOV Mode.mode_width,80;
   MOV Mode.mode_height,25;
   MOV Mode.width_bytes,2000;
   MOV Mode.colors,2;
   MOV Mode.attrib,TVU_TEXT
   OR Mode.attrib,TVU_MONOCHROME
   JMP SET_OUT

SET0DH:
   MOV SI,OFFSET MODE0DH
   CALL SETMODE
   CALL SETPALETTE16
   MOV Mode.mode_width,320;
   MOV Mode.mode_height,200;
   MOV Mode.width_bytes,8000;
   MOV Mode.colors,16;
   MOV Mode.attrib,TVU_GRAPHICS;
   OR Mode.attrib,TVU_PLANAR
   JMP SET_OUT

SET0EH:
   MOV SI,OFFSET MODE0EH
   CALL SETMODE
   CALL SETPALETTE16
   MOV Mode.mode_width,640;
   MOV Mode.mode_height,200;
   MOV Mode.width_bytes,16000;
   MOV Mode.colors,16;
   MOV Mode.attrib,TVU_GRAPHICS;
   OR Mode.attrib,TVU_PLANAR
   JMP SET_OUT

SET0FH:
   MOV SI,OFFSET MODE0FH
   CALL SETMODE
   MOV Mode.mode_width,640;
   MOV Mode.mode_height,350;
   MOV Mode.width_bytes,28000;
   MOV Mode.colors,2;
   MOV Mode.attrib,TVU_GRAPHICS
   OR Mode.attrib,TVU_MONOCHROME
   JMP SET_OUT

SET10H:
   MOV SI,OFFSET MODE10H
   CALL SETMODE
   CALL SETPALETTE16
   MOV Mode.mode_width,640;
   MOV Mode.mode_height,350;
   MOV Mode.width_bytes,28000;
   MOV Mode.colors,16;
   MOV Mode.attrib,TVU_GRAPHICS;
   OR Mode.attrib,TVU_PLANAR
   JMP SET_OUT

SET11H:
   MOV SI,OFFSET MODE11H
   CALL SETMODE
   CALL SETPALETTE16
   MOV Mode.mode_width,640;
   MOV Mode.mode_height,480;
   MOV Mode.width_bytes,38400;
   MOV Mode.colors,2;
   MOV Mode.attrib,TVU_GRAPHICS;
   OR Mode.attrib,TVU_PLANAR
   JMP SET_OUT

SET12H:
   MOV SI,OFFSET MODE12H
   CALL SETMODE
   CALL SETPALETTE16
   MOV Mode.mode_width,640
   MOV Mode.mode_height,480
   MOV Mode.width_bytes,38400
   MOV Mode.colors,16
   MOV Mode.attrib,TVU_GRAPHICS
   OR Mode.attrib,TVU_PLANAR
   JMP SET_OUT

SET13H:
   MOV SI,OFFSET MODE13H
   CALL SETMODE
   CALL SETPALETTE256
   MOV Mode.mode_width,320;
   MOV Mode.mode_height,200;
   MOV Mode.width_bytes,64000;
   MOV Mode.colors,256;
   MOV Mode.attrib,TVU_GRAPHICS;
   JMP SET_OUT

SETCHAIN4:
   MOV SI,OFFSET CHAIN4
   CALL SETMODE
   CALL SETPALETTE256
   MOV Mode.mode_width,320;
   MOV Mode.mode_height,200;
   MOV Mode.width_bytes,16000;
   MOV Mode.colors,256;
   MOV Mode.attrib,TVU_GRAPHICS;
   OR Mode.attrib,TVU_UNCHAINED
   JMP SET_OUT

SETMODE_X:
   MOV SI,OFFSET CHAIN4
   CALL SETMODE

   MOV DX,MISC_ADDR
   MOV AX,0E3H
   OUT DX,AL

   MOV DX,CRTC_ADDR

   ; turn off write protect
   MOV AX,02C11H
   OUT DX,AX
   ; vertical total
   MOV AX,00D06H
   OUT DX,AX
   ; overflow register
   MOV AX,03E07H
   OUT DX,AX
   ; vertical retrace start
   MOV AX,0EA10H
   OUT DX,AX
   ; vertical retrace end AND wr.prot
   MOV AX,0AC11H
   OUT DX,AX
   ; vertical display enable end
   MOV AX,0DF12H
   OUT DX,AX
   ; start vertical blanking
   MOV AX,0E715H
   OUT DX,AX
   ; end vertical blanking
   MOV AX,00616H
   OUT DX,AX

   CALL SETPALETTE256
   MOV Mode.mode_width,320;
   MOV Mode.mode_height,240;
   MOV Mode.width_bytes,19200;
   MOV Mode.colors,256;
   MOV Mode.attrib,TVU_GRAPHICS
   OR Mode.attrib,TVU_UNCHAINED
   JMP SET_OUT

SETMODE_A:
   MOV SI,OFFSET CHAIN4
   CALL SETMODE

   ; turn off double scanning mode
   MOV DX,CRTC_ADDR
   MOV AX,9
   OUT DX,AL
   INC DX
   IN AL,DX
   AND AL,0E0h
   OUT DX,AL

   ; change the vertical resolution flags to 350
   MOV DX,03CCH
   IN AL,DX
   AND AL,02Fh
   OR AL,80h
   MOV DX,MISC_ADDR
   OUT DX,AL

   MOV DX,CRTC_ADDR
   ; turn off write protect
   MOV AX,02C11H
   OUT DX,AX
   ; vertical total
   MOV AX,0BF06H
   OUT DX,AX
   ; overflow register
   MOV AX,01F07H
   OUT DX,AX
   ; vertical retrace start
   MOV AX,08310H
   OUT DX,AX
   ; vertical retrace end AND wr.prot
   MOV AX,08511H
   OUT DX,AX
   ; vertical display enable end
   MOV AX,05D12H
   OUT DX,AX
   ; start vertical blanking
   MOV AX,06315H
   OUT DX,AX
   ; end vertical blanking
   MOV AX,0BA16H
   OUT DX,AX

   CALL SETPALETTE256

   MOV Mode.mode_width,320;
   MOV Mode.mode_height,350;
   MOV Mode.width_bytes,28000;
   MOV Mode.colors,256;
   MOV Mode.attrib,TVU_GRAPHICS
   OR Mode.attrib,TVU_UNCHAINED
   JMP SET_OUT

SETMODE_B:
   MOV SI,OFFSET CHAIN4
   CALL SETMODE

   ; turn off double scanning mode
   MOV DX,CRTC_ADDR
   MOV AX,9
   OUT DX,AL
   INC DX
   IN AL,DX
   AND AL,0E0h
   OUT DX,AL

   ; change the vertical resolution flags to 400
   MOV DX,03CCH
   IN AL,DX
   AND AL,02Fh
   OR AL,40h
   MOV DX,MISC_ADDR
   OUT DX,AL

   CALL SETPALETTE256

   MOV Mode.mode_width,320;
   MOV Mode.mode_height,400;
   MOV Mode.width_bytes,32000;
   MOV Mode.colors,256;
   MOV Mode.attrib,TVU_GRAPHICS
   OR Mode.attrib,TVU_UNCHAINED
   JMP SET_OUT

SETMODE_C:
   MOV SI,OFFSET CHAIN4
   CALL SETMODE

   ; turn off double scanning mode
   MOV DX,CRTC_ADDR
   MOV AX,9
   OUT DX,AL
   INC DX
   IN AL,DX
   AND AL,0E0h
   OUT DX,AL

   ; change the vertical resolution flags to 480
   MOV DX,03CCH
   IN AL,DX
   AND AL,02Fh
   OR AL,0C0h
   MOV DX,MISC_ADDR
   OUT DX,AL

   MOV DX,CRTC_ADDR
   ; turn off write protect
   MOV AX,02C11H
   OUT DX,AX
   ; vertical total
   MOV AX,00D06H
   OUT DX,AX
   ; overflow register
   MOV AX,03E07H
   OUT DX,AX
   ; vertical retrace start
   MOV AX,0EA10H
   OUT DX,AX
   ; vertical retrace end AND wr.prot
   MOV AX,0AC11H
   OUT DX,AX
   ; vertical display enable end
   MOV AX,0DF12H
   OUT DX,AX
   ; start vertical blanking
   MOV AX,0E715H
   OUT DX,AX
   ; end vertical blanking
   MOV AX,00616H
   OUT DX,AX

   CALL SETPALETTE256

   MOV Mode.mode_width,320;
   MOV Mode.mode_height,480;
   MOV Mode.width_bytes,38400;
   MOV Mode.colors,256;
   MOV Mode.attrib,TVU_GRAPHICS
   OR Mode.attrib,TVU_UNCHAINED
   JMP SET_OUT

SETMODE_D:
   MOV SI,OFFSET MODE13H
   CALL SETMODE

   ; Turn off Chain 4
   MOV DX,SEQ_ADDR
   MOV AX,0604H
   OUT DX,AX
   ; Activate a synchronous reset
   MOV AX,0100H
   OUT DX,AX
   ; Select 28 mhz pixel clock
   MOV DX,MISC_ADDR
   MOV AL,067H
   OUT DX,AL
   ; Release synchronous reset
   MOV DX,SEQ_ADDR
   MOV AX,0300H
   OUT DX,AX

   ; change the vertical resolution flags to 400
   MOV DX,03CCH
   IN AL,DX
   AND AL,02Fh
   OR AL,40h
   MOV DX,MISC_ADDR
   OUT DX,AL
   CALL SETPALETTE256

   MOV DX,CRTC_ADDR
   ; turn off write protect
   MOV AX,02C11H
   OUT DX,AX

   MOV AX,06B00H
   OUT DX,AX
   MOV AX,05901H
   OUT DX,AX
   MOV AX,05A02H
   OUT DX,AX
   MOV AX,08E03H
   OUT DX,AX
   MOV AX,05E04H
   OUT DX,AX
   MOV AX,08A05H
   OUT DX,AX
   MOV AX,00008H
   OUT DX,AX
   MOV AX,0C009H
   OUT DX,AX
   MOV AX,0000AH
   OUT DX,AX
   MOV AX,0000BH
   OUT DX,AX
   MOV AX,0000CH
   OUT DX,AX
   MOV AX,0000DH
   OUT DX,AX
   MOV AX,0000EH
   OUT DX,AX
   MOV AX,0000FH
   OUT DX,AX
   MOV AX,0AC11H
   OUT DX,AX
   MOV AX,02D13H
   OUT DX,AX
   MOV AX,00014H
   OUT DX,AX
   MOV AX,0E317H
   OUT DX,AX
   MOV AX,0FF18H
   OUT DX,AX

   CALL SETPALETTE256

   MOV Mode.mode_width,360;
   MOV Mode.mode_height,200;
   MOV Mode.width_bytes,18000;
   MOV Mode.colors,256;
   MOV Mode.attrib,TVU_GRAPHICS
   OR Mode.attrib,TVU_UNCHAINED
   JMP SET_OUT


SETMODE_E:
   MOV SI,OFFSET MODE13H
   CALL SETMODE

   ; Turn off Chain 4
   MOV DX,SEQ_ADDR
   MOV AX,0604H
   OUT DX,AX
   ; Activate a synchronous reset
   MOV AX,0100H
   OUT DX,AX
   ; Select 28 mhz pixel clock
   MOV DX,MISC_ADDR
   MOV AL,067H
   OUT DX,AL
   ; Release synchronous reset
   MOV DX,SEQ_ADDR
   MOV AX,0300H
   OUT DX,AX

   ; change the vertical resolution flags to 480
   MOV DX,03CCH
   IN AL,DX
   AND AL,02Fh
   OR AL,0C0h
   MOV DX,MISC_ADDR
   OUT DX,AL
   CALL SETPALETTE256

   MOV DX,CRTC_ADDR
   ; turn off write protect
   MOV AX,02C11H
   OUT DX,AX

   MOV AX,06B00H
   OUT DX,AX
   MOV AX,05901H
   OUT DX,AX
   MOV AX,05A02H
   OUT DX,AX
   MOV AX,08E03H
   OUT DX,AX
   MOV AX,05E04H
   OUT DX,AX
   MOV AX,08A05H
   OUT DX,AX
   MOV AX,00D06H
   OUT DX,AX
   MOV AX,03E07H
   OUT DX,AX
   MOV AX,00008H
   OUT DX,AX
   MOV AX,0C009H
   OUT DX,AX
   MOV AX,0000AH
   OUT DX,AX
   MOV AX,0000BH
   OUT DX,AX
   MOV AX,0000DH
   OUT DX,AX
   MOV AX,0000CH
   OUT DX,AX
   MOV AX,0000EH
   OUT DX,AX
   MOV AX,0000FH
   OUT DX,AX
   MOV AX,0EA10H
   OUT DX,AX
   MOV AX,0AC11H
   OUT DX,AX
   MOV AX,0DF12H
   OUT DX,AX
   MOV AX,02D13H
   OUT DX,AX
   MOV AX,00014H
   OUT DX,AX
   MOV AX,0E715H
   OUT DX,AX
   MOV AX,00616H
   OUT DX,AX
   MOV AX,0E317H
   OUT DX,AX
   MOV AX,0FF18H
   OUT DX,AX

   CALL SETPALETTE256

   MOV Mode.mode_width,360;
   MOV Mode.mode_height,240;
   MOV Mode.width_bytes,21600;
   MOV Mode.colors,256;
   MOV Mode.attrib,TVU_GRAPHICS
   OR Mode.attrib,TVU_UNCHAINED
   JMP SET_OUT

SETMODE_F:
   MOV SI,OFFSET MODE13H
   CALL SETMODE

   ; Turn off Chain 4
   MOV DX,SEQ_ADDR
   MOV AX,0604H
   OUT DX,AX
   ; Activate a synchronous reset
   MOV AX,0100H
   OUT DX,AX
   ; Select 28 mhz pixel clock
   MOV DX,MISC_ADDR
   MOV AL,067H
   OUT DX,AL
   ; Release synchronous reset
   MOV DX,SEQ_ADDR
   MOV AX,0300H
   OUT DX,AX

   ; change the vertical resolution flags to 350
   MOV DX,03CCH
   IN AL,DX
   AND AL,02Fh
   OR AL,080h
   MOV DX,MISC_ADDR
   OUT DX,AL
   CALL SETPALETTE256

   MOV DX,CRTC_ADDR
   ; turn off write protect
   MOV AX,02C11H
   OUT DX,AX

   MOV AX,06B00H
   OUT DX,AX
   MOV AX,05901H
   OUT DX,AX
   MOV AX,05A02H
   OUT DX,AX
   MOV AX,08E03H
   OUT DX,AX
   MOV AX,05E04H
   OUT DX,AX
   MOV AX,08A05H
   OUT DX,AX
   MOV AX,0BF06H
   OUT DX,AX
   MOV AX,01F07H
   OUT DX,AX
   MOV AX,00008H
   OUT DX,AX
   MOV AX,04009H
   OUT DX,AX
   MOV AX,0000AH
   OUT DX,AX
   MOV AX,0000BH
   OUT DX,AX
   MOV AX,0000DH
   OUT DX,AX
   MOV AX,0000CH
   OUT DX,AX
   MOV AX,0000EH
   OUT DX,AX
   MOV AX,0000FH
   OUT DX,AX
   MOV AX,08310H
   OUT DX,AX
   MOV AX,08511H
   OUT DX,AX
   MOV AX,05D12H
   OUT DX,AX
   MOV AX,02D13H
   OUT DX,AX
   MOV AX,00014H
   OUT DX,AX
   MOV AX,06315H
   OUT DX,AX
   MOV AX,0BA16H
   OUT DX,AX
   MOV AX,0E317H
   OUT DX,AX
   MOV AX,0FF18H
   OUT DX,AX

   CALL SETPALETTE256

   MOV Mode.mode_width,360;
   MOV Mode.mode_height,350;
   MOV Mode.width_bytes,31500;
   MOV Mode.colors,256;
   MOV Mode.attrib,TVU_GRAPHICS
   OR Mode.attrib,TVU_UNCHAINED
   JMP SET_OUT

SETMODE_G:
   MOV SI,OFFSET MODE13H
   CALL SETMODE

   ; Turn off Chain 4
   MOV DX,SEQ_ADDR
   MOV AX,0604H
   OUT DX,AX
   ; Activate a synchronous reset
   MOV AX,0100H
   OUT DX,AX
   ; Select 28 mhz pixel clock
   MOV DX,MISC_ADDR
   MOV AL,067H
   OUT DX,AL
   ; Release synchronous reset
   MOV DX,SEQ_ADDR
   MOV AX,0300H
   OUT DX,AX

   ; change the vertical resolution flags to 400
   MOV DX,03CCH
   IN AL,DX
   AND AL,02Fh
   OR AL,040h
   MOV DX,MISC_ADDR
   OUT DX,AL

   MOV DX,CRTC_ADDR
   ; turn off write protect
   MOV AX,02C11H
   OUT DX,AX

   MOV AX,06B00H
   OUT DX,AX
   MOV AX,05901H
   OUT DX,AX
   MOV AX,05A02H
   OUT DX,AX
   MOV AX,08E03H
   OUT DX,AX
   MOV AX,05E04H
   OUT DX,AX
   MOV AX,08A05H
   OUT DX,AX
   MOV AX,00008H
   OUT DX,AX
   MOV AX,04009H
   OUT DX,AX
   MOV AX,0000AH
   OUT DX,AX
   MOV AX,0000BH
   OUT DX,AX
   MOV AX,0000DH
   OUT DX,AX
   MOV AX,0000CH
   OUT DX,AX
   MOV AX,0000EH
   OUT DX,AX
   MOV AX,0000FH
   OUT DX,AX
   MOV AX,0AC11H
   OUT DX,AX
   MOV AX,02D13H
   OUT DX,AX
   MOV AX,00014H
   OUT DX,AX
   MOV AX,0E317H
   OUT DX,AX
   MOV AX,0FF18H
   OUT DX,AX

   CALL SETPALETTE256

   MOV Mode.mode_width,360;
   MOV Mode.mode_height,400;
   MOV Mode.width_bytes,36000;
   MOV Mode.colors,256;
   MOV Mode.attrib,TVU_GRAPHICS
   OR Mode.attrib,TVU_UNCHAINED
   JMP SET_OUT

SETMODE_H:

   MOV SI,OFFSET MODE13H
   CALL SETMODE

   ; Turn off Chain4
   MOV DX,SEQ_ADDR
   MOV AX,0604H
   OUT DX,AX

   ; Activate a synchronous reset
   MOV AX,0100H
   OUT DX,AX

   ; Select 28 mhz pixel clock
   MOV DX,MISC_ADDR
   MOV AL,067H
   OUT DX,AL

   ; Release synchronous reset
   MOV DX,SEQ_ADDR
   MOV AX,0300H
   OUT DX,AX

   ; change the vertical resolution flags to 480
   MOV DX,03CCH
   IN AL,DX
   AND AL,02Fh
   OR AL,0C0h
   MOV DX,MISC_ADDR
   OUT DX,AL


   MOV DX,CRTC_ADDR
   ; turn off write protect
   MOV AX,02C11H
   OUT DX,AX

   MOV AX,06B00H
   OUT DX,AX
   MOV AX,05901H
   OUT DX,AX
   MOV AX,05A02H
   OUT DX,AX
   MOV AX,08E03H
   OUT DX,AX
   MOV AX,05E04H
   OUT DX,AX
   MOV AX,08A05H
   OUT DX,AX
   MOV AX,00D06H
   OUT DX,AX
   MOV AX,03E07H
   OUT DX,AX
   MOV AX,00008H
   OUT DX,AX
   MOV AX,04009H
   OUT DX,AX
   MOV AX,0000AH
   OUT DX,AX
   MOV AX,0000BH
   OUT DX,AX
   MOV AX,0000DH
   OUT DX,AX
   MOV AX,0000CH
   OUT DX,AX
   MOV AX,0000EH
   OUT DX,AX
   MOV AX,0000FH
   OUT DX,AX
   MOV AX,0EA10H
   OUT DX,AX
   MOV AX,0AC11H
   OUT DX,AX
   MOV AX,0DF12H
   OUT DX,AX
   MOV AX,02D13H
   OUT DX,AX
   MOV AX,00014H
   OUT DX,AX
   MOV AX,0E715H
   OUT DX,AX
   MOV AX,00616H
   OUT DX,AX
   MOV AX,0E317H
   OUT DX,AX
   MOV AX,0FF18H
   OUT DX,AX

   CALL SETPALETTE256

   MOV Mode.mode_width,360;
   MOV Mode.mode_height,480;
   MOV Mode.width_bytes,43200;
   MOV Mode.colors,256;
   MOV Mode.attrib,TVU_GRAPHICS
   OR Mode.attrib,TVU_UNCHAINED
   JMP SET_OUT

SETMODE_I:
   MOV SI,OFFSET MODE10H
   CALL SETMODE

   MOV DX,03CCH
   IN AL,DX
   AND AL,03FH
   OR AL,40H

   MOV DX,03C2H
   OUT DX,AL

   MOV DX,CRTC_ADDR
   MOV AX,9C10H
   OUT DX,AX
   IODELAY

   MOV AX,8311H
   OUT DX,AX
   IODELAY

   MOV AX,8F12H
   OUT DX,AX
   IODELAY

   MOV AX,9615H
   OUT DX,AX
   IODELAY

   MOV AX,0B916H
   OUT DX,AX
   IODELAY

   CALL SETPALETTE16
   MOV Mode.mode_width,640;
   MOV Mode.mode_height,400;
   MOV Mode.width_bytes,32000;
   MOV Mode.colors,16;
   MOV Mode.attrib,TVU_GRAPHICS
   OR Mode.attrib,TVU_PLANAR
   JMP SET_OUT

SETMODE_J:
   MOV SI,OFFSET MODEJ
   CALL SETMODE
   CALL SETPALETTE16

   PUSH 8                 ; Bytes per character
   PUSH 0                  ; Offset of font in Video Memory
   PUSH 0A000H
   PUSH FONT_8x8_OFF      ; Offset of Font in BIOS
   PUSH 0C000H
   CALL LOADFONT

   MOV Mode.mode_width,80;
   MOV Mode.mode_height,43;
   MOV Mode.width_bytes,3440;
   MOV Mode.colors,16;
   MOV Mode.attrib,TVU_TEXT
   JMP SET_OUT

SETMODE_K:
   MOV SI,OFFSET MODEK
   CALL SETMODE
   CALL SETPALETTE16

   PUSH 8                 ; Bytes per character
   PUSH 0                  ; Offset of font in Video Memory
   PUSH 0A000H
   PUSH FONT_8x8_OFF      ; Offset of Font in BIOS
   PUSH 0C000H
   CALL LOADFONT

   MOV Mode.mode_width,80;
   MOV Mode.mode_height,50;
   MOV Mode.width_bytes,4000;
   MOV Mode.colors,16;
   MOV Mode.attrib,TVU_TEXT
   JMP SET_OUT

SETMODE_L:
   MOV SI,OFFSET MODE_L
   CALL SETMODE
   CALL SETPALETTE16

   PUSH 8                 ; Bytes per character
   PUSH 0                  ; Offset of font in Video Memory
   PUSH 0A000H
   PUSH FONT_8x8_OFF      ; Offset of Font in BIOS
   PUSH 0C000H
   CALL LOADFONT

   MOV Mode.mode_width,40;
   MOV Mode.mode_height,43;
   MOV Mode.width_bytes,3440;
   MOV Mode.colors,16;
   MOV Mode.attrib,TVU_TEXT
   JMP SET_OUT

SETMODE_M:
   MOV SI,OFFSET MODEM
   CALL SETMODE
   CALL SETPALETTE16

   PUSH 8                 ; Bytes per character
   PUSH 0                  ; Offset of font in Video Memory
   PUSH 0A000H
   PUSH FONT_8x8_OFF      ; Offset of Font in BIOS
   PUSH 0C000H
   CALL LOADFONT

   MOV Mode.mode_width,40;
   MOV Mode.mode_height,50;
   MOV Mode.width_bytes,4000;
   MOV Mode.colors,16;
   MOV Mode.attrib,TVU_TEXT

SET_OUT:

   POP BP
   RET 2
SETVIDEOMODE ENDP

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

   PUBLIC SETMODE
SETMODE PROC

   ; Send MISC regs
   MOV DX,MISC_ADDR
   MOV AL,[SI]
   OUT DX,AL
   IODELAY
   INC SI

   MOV DX,STATUS_ADDR
   MOV AL,[SI]
   OUT DX,AL
   IODELAY
   INC SI

   ; Send SEQ regs
   MOV CX,0
REG_LOOP:
   MOV DX,SEQ_ADDR
   MOV AL,CL
   OUT DX,AL
   IODELAY

   MOV DX,SEQ_ADDR
   INC DX
   MOV AL,[SI]
   OUT DX,AL
   IODELAY
   INC SI
   INC CX
   CMP CL,5
   JL REG_LOOP

   ; Clear Protection bits
   MOV AH,0EH
   MOV AL,11H
   AND AH,7FH
   MOV DX,CRTC_ADDR
   OUT DX,AX
   IODELAY

   ; Send CRTC regs
   MOV CX,0
REG_LOOP2:
   MOV DX,CRTC_ADDR
   MOV AL,CL
   OUT DX,AL
   IODELAY

   MOV DX,CRTC_ADDR
   INC DX
   MOV AL,[SI]
   OUT DX,AL
   IODELAY

   INC SI
   INC CX
   CMP CL,25
   JL REG_LOOP2

   ; Send GRAPHICS regs
   MOV CX,0
REG_LOOP3:
   MOV DX,GRACON_ADDR
   MOV AL,CL
   OUT DX,AL
   IODELAY

   MOV DX,GRACON_ADDR
   INC DX
   MOV AL,[SI]
   OUT DX,AL
   IODELAY

   INC SI
   INC CX
   CMP CL,9
   JL REG_LOOP3

   MOV DX,STATUS_ADDR
   IN AL,DX
   IODELAY

   ; Send ATTRCON regs
   MOV CX,0
REG_LOOP4:
   MOV DX,ATTRCON_ADDR
   IN AX,DX

   MOV AL,CL
   OUT DX,AL
   IODELAY

   MOV AL,[SI]
   OUT DX,AL
   IODELAY

   INC SI
   INC CX
   CMP CL,21
   JL REG_LOOP4

   MOV AL,20H
   OUT DX,AL
   IODELAY

   RET
SETMODE ENDP

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
; Assumes [bp+4] = color
; Assumes [bp+6] = r value
; Assumes [bp+8] = g value
; Assumes [bp+10] = b value
   PUBLIC SETPAL
SETPAL PROC
   PUSH BP
   MOV BP,SP
   PUSHA

   MOV AX,WORD PTR [BP+4]
   ; Send color
   MOV DX,03C8H
   OUT DX,AL

   ; Write R value
   MOV DX,03C9H
   MOV AL,BYTE PTR [BP+6]
   OUT DX,AL

   ; Write G value
   MOV DX,03C9H
   MOV AL,BYTE PTR [BP+8]
   OUT DX,AL

   ; Write B value
   MOV DX,03C9H
   MOV AL,BYTE PTR [BP+10]
   OUT DX,AL

   POPA
   POP BP
   RET 8
SETPAL ENDP

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

SETPALETTE4 PROC
   PUSH 0
   PUSH 0
   PUSH 0
   PUSH 0
   CALL SETPAL

   PUSH 42
   PUSH 42
   PUSH 0
   PUSH 1
   CALL SETPAL

   PUSH 42
   PUSH 0
   PUSH 42
   PUSH 2
   CALL SETPAL

   PUSH 63
   PUSH 63
   PUSH 63
   PUSH 3
   CALL SETPAL

   RET
SETPALETTE4 ENDP

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

SETPALETTE16 PROC

   MOV SI,OFFSET PALETTE
   MOV CX,16
   MOV DX,0
PAL_LOOP:
   PUSH WORD PTR [SI+2]
   PUSH WORD PTR [SI+1]
   PUSH WORD PTR [SI]
   PUSH DX
   CALL SETPAL

   INC DX
   ADD SI,3
   LOOP PAL_LOOP

   RET
SETPALETTE16 ENDP

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

SETPALETTE256 PROC

   MOV SI,OFFSET PALETTE
   MOV CX,256
   MOV DX,0
PAL_LOOP2:
   PUSH WORD PTR [SI+2]
   PUSH WORD PTR [SI+1]
   PUSH WORD PTR [SI]
   PUSH DX
   CALL SETPAL

   INC DX
   ADD SI,3
   LOOP PAL_LOOP2

   RET
SETPALETTE256 ENDP

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
