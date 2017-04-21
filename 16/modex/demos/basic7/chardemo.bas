DEFINT A-Z
DECLARE SUB PRINT.STRING (Text$, Xpos%, Ypos%, Colour%)
DECLARE FUNCTION MakePal$ (Red%, Green%, Blue%)
DECLARE SUB LOAD.FONT (FontFile$, FontNum%)
DECLARE SUB ERROR.OUT (Text$)

	REM $INCLUDE: 'MODEX.BI'

	REM $INCLUDE: 'UTILS.BI'

TYPE FONT
	SetData AS STRING * 1024
END TYPE


TYPE VGAPalette
	PalData AS STRING * 768
END TYPE


	' Alternate form of LOAD_DAC_REGISTERS so we can pass an offset into
	' a String instead of the Address of the String

DECLARE SUB LOAD.DACS ALIAS "LOAD_DAC_REGISTERS" (BYVAL Addr&, BYVAL StartReg%, BYVAL EndReg%, BYVAL VSync%)


	'
	'MODE X DEMO of Multiple Character Sets and Block Color Cycling
	'
	'By Matt Pritchard
	'

COMMON SHARED CharSet() AS FONT

DIM Pal AS VGAPalette

	REM $DYNAMIC

DIM SHARED CharSet(0 TO 3) AS FONT


	LOAD.FONT "SYSTEM.FNT", 0
	LOAD.FONT "ROM_8x8.FNT", 1
	LOAD.FONT "SPACEAGE.FNT", 2


	IF SET.MODEX(Mode320x240) = False THEN
		ERROR.OUT "ERROR SETTING MODE X"
	END IF


	A$ = "": B$ = ""
	FOR X = 0 TO 31: A$ = A$ + MakePal$(31 - X, X, 0): NEXT X
	FOR X = 0 TO 31: A$ = A$ + MakePal$(0, 31 - X, X): NEXT X
	FOR X = 0 TO 31: A$ = A$ + MakePal$(X, 0, 31 - X): NEXT X
   
	FOR X = 0 TO 31: B$ = B$ + MakePal$(31 - X, X, X): NEXT X
	FOR X = 0 TO 31: B$ = B$ + MakePal$(X, 31 - X, X): NEXT X
	FOR X = 0 TO 31: B$ = B$ + MakePal$(X, X, 31 - X): NEXT X

	Black$ = STRING$(192, 0)
	White$ = STRING$(128 * 3, 48)

	Pal1$ = Black$ + A$ + A$ + B$ + B$ + A$

	LOAD.DACS SSEGADD(Black$), 64, 127, 1
	LOAD.DACS SSEGADD(Black$), 20, 63, 0

	LOAD.DACS SSEGADD(White$), 128, 255, 0

	'*** Background ***

	FOR X = 0 TO 319
		FOR Y = 0 TO 239
		   IF ((X + Y) AND 1) = 1 THEN SET.POINT X, Y, 64 + X \ 5 ELSE SET.POINT X, Y, 20 + Y \ 6
		NEXT Y
	NEXT X

	'*** Draw Font Displays ***

	PRINT.STRING "FONT: SYSTEM.FNT", 11, 7, 15
	PRINT.STRING "FONT: ROM_8x8.FNT", 11, 17, 15
	PRINT.STRING "FONT: SPACEAGE.FNT", 11, 27, 15
	PRINT.STRING "PRESS ANY KEY TO CONTINUE", 8, 29, 14
	

	FOR F = 0 TO 2
		SET.DISPLAY.FONT CharSet(F), 1
		Yp = F * 80 + 10
		FOR Y = 0 TO 96 STEP 32
			FOR X = 0 TO 31
				TGPRINTC 128 + Y + X, X * 10 + 1, Yp, 128 + Y
			NEXT X
		Yp = Yp + 10
		NEXT Y
	NEXT F

	DO
	LOOP UNTIL SCAN.KEYBOARD

	Offset = 0
	Restart = 192
	MaxOfs = 192 + 96 * 6

	Delay = 100

	Offset2 = 0
	Offset2Dir = 3
	Offset2Min = 192
	Offset2Max = Offset2Min + 192 * 6

	DO
		LOAD.DACS SSEGADD(Pal1$) + Offset, 64, 127, 1
		Offset = Offset + 3
		IF Offset >= MaxOfs THEN Offset = Restart
		IF Delay THEN
			Delay = Delay - 1
		ELSE
			LOAD.DACS SSEGADD(Pal1$) + Offset2, 20, 60, 0
			IF Offset2 = Offset2Max THEN Offset2Dir = -3
			IF Offset2 = Offset2Min THEN Offset2Dir = 3
			Offset2 = Offset2 + Offset2Dir
		END IF

	LOOP UNTIL SCAN.KEYBOARD

	ERROR.OUT "DEMO OVER"

REM $STATIC
SUB ERROR.OUT (Text$)

	SET.VIDEO.MODE 3

	DOS.PRINT Text$

	END

END SUB

SUB LOAD.FONT (FontFile$, FontNum) STATIC

	IF LEN(DIR$(FontFile$)) = 0 THEN ERROR.OUT "FILE NOT FOUND: " + FontFile$

	OPEN FontFile$ FOR BINARY AS #1

		SEEK #1, 1
		GET #1, , CharSet(FontNum)

	CLOSE #1

END SUB

FUNCTION MakePal$ (Red, Green, Blue) STATIC

	MakePal$ = CHR$(Red) + CHR$(Green) + CHR$(Blue)

END FUNCTION

SUB PRINT.STRING (Text$, Xpos, Ypos, Colour)

	TPRINT.STR SSEG(Text$), SADD(Text$), LEN(Text$), Xpos * 8, Ypos * 8, Colour

END SUB

