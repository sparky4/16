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
;=- VGA register variables for the various videomodes                       -=
;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

; Same as Mode 01H
MODE00H DB 67H, 00H,  03H,08H,03H,00H,02H
        DB 2DH,27H,28H,90H,2BH,0A0H,0BFH,1FH,00H,4FH,06H,07H,00H,00H,00H,31H
        DB 9CH,8EH,8FH,14H,1FH,96H,0B9H,0A3H,0FFH
        DB 00H,00H,00H,00H,00H,10H,0EH,00H,0FFH
        DB 00H,01H,02H,03H,04H,05H,14H,07H,38H,39H,3AH,3BH,3CH,3DH,3EH,3FH
        DB 0CH,00H,0FH,08H,00H

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

; Same as Mode 02H
MODE03H DB 67H, 00H,  03H,00H,03H,00H,02H
        DB 5FH,4FH,50H,82H,55H,81H,0BFH,1FH,00H,4FH,0EH,0FH,00H,00H,00H,00H
        DB 9CH,0EH,8FH,28H,01H,96H,0B9H,0A3H,0FFH
        DB 00H,00H,00H,00H,00H,10H,0EH,00H,0FFH
        DB 00H,01H,02H,03H,04H,05H,14H,07H,38H,39H,3AH,3BH,3CH,3DH,3EH,3FH
        DB 0CH,00H,0FH,08H,00H
;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

; Same as Mode 05H
MODE04H DB 63H, 00H,  03H,09H,03H,00H,02H
        DB 2DH,27H,28H,90H,2BH,80H,0BFH,1FH,00H,0C1H,00H,00H,00H,00H,00H,31H
        DB 9CH,8EH,8FH,14H,00H,96H,0B9H,0A2H,0FFH
        DB 00H,00H,00H,00H,00H,30H,0FH,00H,0FFH
        DB 00H,3BH,3DH,3FH,02H,04H,06H,07H,10H,11H,12H,13H,14H,15H,16H,17H
        DB 01H,00H,03H,00H,00H

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

MODE06H DB 63H, 00H,  03H,01H,01H,00H,06H
        DB 5FH,4FH,50H,82H,54H,80H,0BFH,1FH,00H,0C1H,00H,00H,00H,00H,00H,00H
        DB 9CH,8EH,8FH,28H,00H,96H,0B9H,0C2H,0FFH
        DB 00H,00H,00H,00H,00H,00H,0DH,00H,0FFH
        DB 00H,3FH,3FH,3FH,3FH,3FH,3FH,3FH,3FH,3FH,3FH,3FH,3FH,3FH,3FH,3FH
        DB 01H,00H,01H,00H,00H

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

MODE07H DB 066H, 00H,  03H,00H,03H,00H,02H
        DB 0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH
        DB 0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH
        DB 00H,00H,00H,00H,00H,10H,0AH,00H,0FFH
        DB 00H,08H,08H,08H,08H,08H,08H,08H,10H,18H,18H,18H,18H,18H,18H,18H
        DB 0EH,00H,0FH,00H,00H

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

MODE0DH DB 063H, 00H,  03H,09H,0FH,00H,06H
        DB 2DH,27H,28H,90H,2BH,80H,0BFH,1FH,00H,0C0H,00H,00H,00H,00H,00H,31H
        DB 9CH,8EH,8FH,14H,00H,96H,0B9H,0E3H,0FFH
        DB 00H,00H,00H,00H,00H,10H,05H,00H,0FFH
        DB 00H,01H,02H,03H,04H,05H,14H,07H,38H,39H,3AH,3BH,3CH,3DH,3EH,3FH
        DB 01H,00H,0FH,00H,00H

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

MODE0EH DB 63H, 00H,  03H,01H,0FH,00H,06H
        DB 5FH,4FH,50H,82H,54H,80H,0BFH,1FH,00H,0C0H,00H,00H,00H,00H,00H,59H
        DB 9CH,8EH,8FH,28H,00H,96H,0B9H,0E3H,0FFH
        DB 00H,00H,00H,00H,00H,00H,05H,0FH,0FFH
        DB 00H,01H,02H,03H,04H,05H,14H,07H,38H,39H,3AH,3BH,3CH,3DH,3EH,3FH
        DB 01H,00H,0FH,00H,00H

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

MODE0FH DB 0A2H, 00H,  03H,01H,0FH,00H,06H
        DB 0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH
        DB 0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH
        DB 00H,00H,00H,00H,00H,00H,05H,05H,0FFH
        DB 00H,08H,00H,00H,18H,18H,00H,00H,00H,08H,00H,00H,00H,18H,00H,00H
        DB 0BH,00H,05H,00H,00H

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

MODE10H DB 0A3H, 00H,  03H,01H,0FH,00H,06H
        DB 5FH,4FH,50H,82H,54H,80H,0BFH,1FH,00H,40H,00H,00H,00H,00H,00H,00H
        DB 83H,85H,5DH,28H,0FH,63H,0BAH,0E3H,0FFH
        DB 00H,00H,00H,00H,00H,00H,05H,0FH,0FFH
        DB 00H,01H,02H,03H,04H,05H,06H,07H,08H,09H,0AH,0BH,0CH,0DH,0EH,0FH
        DB 01H,00H,0FH,00H,00H

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

MODE11H DB 063H, 00H,  03H,01H,0FH,00H,06H
        DB 5FH,4FH,50H,82H,54H,80H,0BH,3EH,00H,40H,00H,00H,00H,00H,00H,59H
        DB 0EAH,8CH,0DFH,28H,0FH,0E7H,004H,0C3H,0FFH
        DB 00H,00H,00H,00H,00H,00H,05H,05H,0FFH
        DB 00H,3FH,3FH,3FH,3FH,3FH,3FH,3FH,3FH,3FH,3FH,3FH,3FH,3FH,3FH,3FH
        DB 01H,00H,0FH,00H,00H

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

MODE12H DB 063H, 00H,  03H,01H,0FH,00H,06H
        DB 5FH,4FH,50H,82H,54H,80H,0BH,3EH,00H,40H,00H,00H,00H,00H,00H,59H
        DB 0EAH,8CH,0DFH,28H,00H,0E7H,04H,0E3H,0FFH
        DB 00H,00H,00H,00H,00H,00H,05H,0FH,0FFH
        DB 00H,01H,02H,03H,04H,05H,06H,07H,08H,09H,0AH,0BH,0CH,0DH,0EH,0FH
        DB 01H,00H,0FH,00H,00H

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

MODE13H DB 63H, 00H,  03H,01H,0FH,00H,0EH
        DB 5FH,4FH,50H,82H,54H,80H,0BFH,1FH,00H,41H,00H,00H,00H,00H,00H,00H
        DB 9CH,0EH,8FH,28H,40H,96H,0B9H,0A3H,0FFH
        DB 00H,00H,00H,00H,00H,40H,05H,0FH,0FFH
        DB 00H,01H,02H,03H,04H,05H,06H,07H,08H,09H,0AH,0BH,0CH,0DH,0EH,0FH
        DB 41H,00H,0FH,00H,00H

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CHAIN4 DB 63H, 00H,  03H,01H,0FH,00H,06H
       DB 5FH,4FH,50H,82H,54H,80H,0BFH,1FH,00H,41H,00H,00H,00H,00H,00H,00H
       DB 9CH,0EH,8FH,28H,00H,96H,0B9H,0E3H,0FFH
       DB 00H,00H,00H,00H,00H,40H,05H,0FH,0FFH
       DB 00H,01H,02H,03H,04H,05H,06H,07H,08H,09H,0AH,0BH,0CH,0DH,0EH,0FH
       DB 41H,00H,0FH,00H,00H

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

MODEJ DB 63H, 00H, 03H,01H,03H,00H,02H
      DB 5FH,4FH,50H,82H,55H,81H,0BFH,1FH,00H,47H,06H,07H,00H,00H,00H
      DB 00H,83H,85H,57H,28H,1FH,60H,0B8H,0A3H,0FFH
      DB 00H,00H,00H,00H,00H,10H,0EH,00H,0FFH
      DB 00H,01H,02H,03H,04H,05H,14H,07H,10H,11H,3AH,3BH,3CH,3DH,3EH,3FH
      DB 0CH,00H,0FH,00H,00H

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

MODEK DB 63H, 00H, 03H,01H,03H,00H,02H
      DB 5FH,4FH,50H,82H,55H,81H,0BFH,1FH,00H,47H,06H,07H,00H,00H,00H
      DB 00H,9CH,8EH,8FH,28H,1FH,96H,0B9H,0A3H,0FFH
      DB 00H,00H,00H,00H,00H,10H,0EH,00H,0FFH
      DB 00H,01H,02H,03H,04H,05H,14H,07H,10H,11H,3AH,3BH,3CH,3DH,3EH,3FH
      DB 0CH,00H,0FH,00H,00H

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


MODE_L DB 67H, 00H, 03H,08H,03H,00H,02H
       DB 2DH,27H,28H,90H,2BH,0A0H,0BFH,1FH,00H,47H,06H,07H,00H,00H,00H
       DB 31H,83H,85H,57H,14H,1FH,60H,0B8H,0A3H,0FFH
       DB 00H,00H,00H,00H,00H,10H,0EH,00H,0FFH
       DB 00H,01H,02H,03H,04H,05H,14H,07H,10H,11H,3AH,3BH,3CH,3DH,3EH,3FH
       DB 0CH,00H,0FH,00H,00H

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

MODEM DB 67H, 00H,  03H,08H,03H,00H,02H
      DB 2DH,27H,28H,90H,2BH,0A0H,0BFH,1FH,00H,47H,06H,07H,00H,00H,00H,31H
      DB 9CH,8EH,8FH,14H,1FH,96H,0B9H,0A3H,0FFH
      DB 00H,00H,00H,00H,00H,10H,0EH,00H,0FFH
      DB 00H,01H,02H,03H,04H,05H,14H,07H,38H,39H,3AH,3BH,3CH,3DH,3EH,3FH
      DB 0CH,00H,0FH,00H,00H

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
; Videomode Structure Definition
VMODE STRUC
   videomode      DW ?   ; Videomode Number
   mode_width     DW ?   ; Width in pixels
   mode_height    DW ?   ; height in pixels
   width_bytes    DW ?   ; Number of bytes per screen
   colors         DW ?   ; Number of colors
   attrib         DW ?   ; Videomode attributes
VMODE ENDS

; Videomode attributes
TVU_TEXT           EQU        0001h
TVU_GRAPHICS       EQU        0002h
TVU_MONOCHROME     EQU        0004h
TVU_PLANAR         EQU        0008h
TVU_UNCHAINED      EQU        0010h
TVU_B8000_ADDR     EQU        0020h

;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
; Videomode Info Structure
MODE VMODE <MODE03H, 80, 25, 2000, 16, TVU_TEXT>
;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
