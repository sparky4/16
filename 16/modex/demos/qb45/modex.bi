 
    ' ===== SCREEN RESOLUTIONS =====
 
CONST Mode320x200 = 0, Mode320x400 = 1
CONST Mode360x200 = 2, Mode360x400 = 3
CONST Mode320x240 = 4, Mode320x480 = 5
CONST Mode360x240 = 6, Mode360x480 = 7
 
    ' ===== MODE X SETUP ROUTINES =====
 
DECLARE FUNCTION SET.VGA.MODEX% ALIAS "SET_VGA_MODEX" (BYVAL ModeType%, BYVAL MaxXpos%, BYVAL MaxYpos%, BYVAL Pages%)
DECLARE FUNCTION SET.MODEX% ALIAS "SET_MODEX" (BYVAL Mode%)
 
    ' ===== BASIC GRAPHICS PRIMITIVES =====
 
DECLARE SUB CLEAR.VGA.SCREEN ALIAS "CLEAR_VGA_SCREEN" (BYVAL ColorNum%)
DECLARE SUB SET.POINT ALIAS "SET_POINT" (BYVAL Xpos%, BYVAL Ypos%, BYVAL ColorNum%)
DECLARE FUNCTION READ.POINT% ALIAS "READ_POINT" (BYVAL Xpos%, BYVAL Ypos%)
DECLARE SUB FILL.BLOCK ALIAS "FILL_BLOCK" (BYVAL Xpos1%, BYVAL Ypos1%, BYVAL Xpos2%, BYVAL Ypos2%, BYVAL ColorNum%)
DECLARE SUB DRAW.LINE ALIAS "DRAW_LINE" (BYVAL Xpos1%, BYVAL Ypos1%, BYVAL Xpos2%, BYVAL Ypos2%, BYVAL ColorNum%)
 
    ' ===== DAC COLOR REGISTER ROUTINES =====
 
DECLARE SUB SET.DAC.REGISTER ALIAS "SET_DAC_REGISTER" (BYVAL RegNo%, BYVAL Red%, BYVAL Green%, BYVAL Blue%)
DECLARE SUB GET.DAC.REGISTER ALIAS "GET_DAC_REGISTER" (BYVAL RegNo%, Red%, Green%, Blue%)
DECLARE SUB LOAD.DAC.REGISTERS ALIAS "LOAD_DAC_REGISTERS" (SEG PalData AS ANY, BYVAL StartReg%, BYVAL EndReg%, BYVAL VSync%)
DECLARE SUB READ.DAC.REGISTERS ALIAS "READ_DAC_REGISTERS" (SEG PalData AS ANY, BYVAL StartReg%, BYVAL EndReg%)
 
 
    ' ===== PAGE FLIPPING AND SCROLLING ROUTINES =====
 
DECLARE SUB SET.ACTIVE.PAGE ALIAS "SET_ACTIVE_PAGE" (BYVAL PageNo%)
DECLARE FUNCTION GET.ACTIVE.PAGE% ALIAS "GET_ACTIVE_PAGE"
DECLARE SUB SET.DISPLAY.PAGE ALIAS "SET_DISPLAY_PAGE" (BYVAL PageNo%)
DECLARE FUNCTION GET.DISPLAY.PAGE% ALIAS "GET_DISPLAY_PAGE"
DECLARE SUB SET.WINDOW ALIAS "SET_WINDOW" (BYVAL DisplayPage%, BYVAL XOffset%, BYVAL YOffset%)
DECLARE FUNCTION GET.X.OFFSET% ALIAS "GET_X_OFFSET" ()
DECLARE FUNCTION GET.Y.OFFSET% ALIAS "GET_Y_OFFSET" ()
DECLARE SUB SYNC.DISPLAY ALIAS "SYNC_DISPLAY"
 
    ' ===== TEXT DISPLAY ROUTINES =====
 
DECLARE SUB GPRINTC (BYVAL CharacterNum%, BYVAL Xpos%, BYVAL Ypos%, BYVAL ColorF%, BYVAL ColorB%)
DECLARE SUB TGPRINTC (BYVAL CharacterNum%, BYVAL Xpos%, BYVAL Ypos%, BYVAL ColorF%)
DECLARE SUB PRINT.STR ALIAS "PRINT_STR" (BYVAL StrSeg%, BYVAL StrOfs%, BYVAL MaxLen%, BYVAL Xpos%, BYVAL Ypos%, BYVAL ColorF%, BYVAL ColorB%)
DECLARE SUB TPRINT.STR ALIAS "TPRINT_STR" (BYVAL StrSeg%, BYVAL StrOfs%, BYVAL MaxLen%, BYVAL Xpos%, BYVAL Ypos%, BYVAL ColorF%)
DECLARE SUB SET.DISPLAY.FONT ALIAS "SET_DISPLAY_FONT" (SEG FontData AS ANY, BYVAL FontNumber%)
 
    ' ===== BITMAP (SPRITE) DISPLAY ROUTINES =====
 
DECLARE SUB DRAW.BITMAP ALIAS "DRAW_BITMAP" (SEG Image AS ANY, BYVAL Xpos%, BYVAL Ypos%, BYVAL xWidth%, BYVAL Height%)
DECLARE SUB TDRAW.BITMAP ALIAS "TDRAW_BITMAP" (SEG Image AS ANY, BYVAL Xpos%, BYVAL Ypos%, BYVAL xWidth%, BYVAL Height%)
 
    ' ==== VIDEO MEMORY to VIDEO MEMORY COPY ROUTINES =====
 
DECLARE SUB COPY.PAGE ALIAS "COPY_PAGE" (BYVAL SourcePage%, BYVAL DestPage%)
DECLARE SUB COPY.BITMAP ALIAS "COPY_BITMAP" (BYVAL SourcePage%, BYVAL X1%, BYVAL Y1%, BYVAL X2%, BYVAL Y2%, BYVAL DestPage%, BYVAL DestX1%, BYVAL DestY1%)
 
 
 
 
 
 
