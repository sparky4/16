'File:     TEST6A.BAS
'Descp.:   A Mode "X" demonstration for Quickbasic 4.5
'Author:   Matt Pritchard
'Date:     14 April, 1993
'
DECLARE SUB DEMO.RES (Mode%, Xmax%, Ymax%)
DECLARE SUB ERROR.OUT (Message$)
DECLARE FUNCTION GET.KEY% ()
DECLARE SUB LOAD.SHAPES ()
DECLARE SUB PAGE.DEMO ()
DECLARE SUB PRINT.TEXT (Text$, Xpos%, Ypos%, ColorF%, ColorB%)
DECLARE SUB TPRINT.TEXT (Text$, Xpos%, Ypos%, ColorF%)
DEFINT A-Z


TYPE ShapeType
	ImgData AS STRING * 512
	xWidth  AS INTEGER
	yWidth  AS INTEGER
END TYPE

TYPE Sprite
	Xpos    AS INTEGER
	Ypos    AS INTEGER
	XDir    AS INTEGER
	YDir    AS INTEGER
	Shape   AS INTEGER
END TYPE


CONST MaxShapes = 32

	REM $INCLUDE: 'UTILS.BI'
	REM $INCLUDE: 'MODEX.BI'

DIM SHARED Img(32) AS ShapeType
COMMON SHARED Img() AS ShapeType


	CALL INIT.RANDOM

	CALL LOAD.SHAPES

	CALL DEMO.RES(Mode320x200, 320, 200)
	CALL DEMO.RES(Mode320x400, 320, 400)

	CALL DEMO.RES(Mode360x200, 360, 200)
	CALL DEMO.RES(Mode360x400, 360, 400)

	CALL DEMO.RES(Mode320x240, 320, 240)
	CALL DEMO.RES(Mode320x480, 320, 480)

	CALL DEMO.RES(Mode360x240, 360, 240)
	CALL DEMO.RES(Mode360x480, 360, 480)

	CALL PAGE.DEMO

	SET.VIDEO.MODE 3
	DOS.PRINT "THIS MODE X DEMO IS FINISHED"
	END

SUB DEMO.RES (Mode, Xmax, Ymax)

	IF SET.MODEX%(Mode) = 0 THEN
		ERROR.OUT "Unable to SET_MODEX" + STR$(Mode)
	END IF

	XCenter = Xmax \ 2
	
	X1 = 10
	Y1 = 10
	X2 = Xmax - 1
	Y2 = Ymax - 1

	FOR Z = 0 TO 3
		Colr = 31 - Z * 2
		DRAW.LINE X1 + Z, Y1 + Z, X2 - Z, Y1 + Z, Colr
		DRAW.LINE X1 + Z, Y1 + Z, X1 + Z, Y2 - Z, Colr
		DRAW.LINE X1 + Z, Y2 - Z, X2 - Z, Y2 - Z, Colr
		DRAW.LINE X2 - Z, Y1 + Z, X2 - Z, Y2 - Z, Colr
	NEXT Z

	XChars = Xmax \ 10
	YChars = Ymax \ 10

	FOR X = 0 TO XChars - 1
		TGPRINTC 48 + ((X + 1) MOD 10), X * 10 + 1, 1, 9 + ((X \ 8) MOD 7)
		DRAW.LINE X * 10 + 9, 0, X * 10 + 9, 3, 15
	NEXT X

	FOR Y = 0 TO YChars - 1
		TGPRINTC 48 + ((Y + 1) MOD 10), 1, Y * 10 + 1, 9 + ((Y \ 10) MOD 7)
		DRAW.LINE 0, Y * 10 + 9, 3, Y * 10 + 9, 15
	NEXT Y

	' Draw Lines

	FOR X = 0 TO 63
		N = 15 + X * .75
		SET.DAC.REGISTER 64 + X, N, N, N
		SET.DAC.REGISTER 128 + X, 0, N, N

		DRAW.LINE 103 - X, 60, 40 + X, 123, 64 + X
		DRAW.LINE 40, 60 + X, 103, 123 - X, 128 + X

	NEXT X
	TPRINT.TEXT "LINE TEST", 37, 130, c.BLUE

	Y = 60: Gap = 0
	FOR X = 0 TO 9
		FILL.BLOCK 120, Y, 120 + X, Y + Gap, 64 + X
		FILL.BLOCK 140 - (15 - X), Y, 150 + X, Y + Gap, 230 + X
		FILL.BLOCK 170 - (15 - X), Y, 170, Y + Gap, 128 + X
		Y = Y + Gap + 2
		Gap = Gap + 1
	NEXT X
	TPRINT.TEXT "FILL TEST", 110, 46, c.GREEN


	FOR X = 190 TO 250 STEP 2
		FOR Y = 60 TO 122 STEP 2
		  SET.POINT X, Y, X + Y + X + Y
		NEXT Y
	NEXT X

	TPRINT.TEXT "PIXEL TEST", 182, 130, c.RED

	FOR X = 190 TO 250 STEP 2
		FOR Y = 60 TO 122 STEP 2
		  IF READ.POINT(X, Y) <> ((X + Y + X + Y) AND 255) THEN
			 ERROR.OUT "READ.PIXEL Failure"
		  END IF
		NEXT Y
	NEXT X



	Msg$ = " This is a MODE X demo "
	PRINT.TEXT Msg$, XCenter - (LEN(Msg$) * 4), 20, c.bRED, c.BLUE
	Msg$ = "Screen Resolution is     by    "
	Xp = XCenter - (LEN(Msg$) * 4)
	PRINT.TEXT Msg$, Xp, 30, c.bGREEN, c.BLACK

	PRINT.TEXT LTRIM$(STR$(Xmax)), Xp + 8 * 21, 30, c.bPURPLE, c.BLACK
	PRINT.TEXT LTRIM$(STR$(Ymax)), Xp + 8 * 28, 30, c.bWHITE, c.BLACK

	FOR X = 0 TO 15
		SET.DAC.REGISTER 230 + X, 63 - X * 4, 0, 15 + X * 3
		DRAW.LINE 30 + X, Ymax - 6 - X, Xmax - 20 - X, Ymax - 6 - X, 230 + X
	NEXT X
	TPRINT.TEXT "Press <ANY KEY> to Continue", XCenter - (26 * 4), Ymax - 18, c.YELLOW

	X = GET.KEY%
	IF X = KyESC THEN ERROR.OUT "ABORT"

END SUB

SUB ERROR.OUT (Message$)

	SET.VIDEO.MODE 3
	DOS.PRINT Message$
	END

END SUB

FUNCTION GET.KEY%

	DO
		X = SCAN.KEYBOARD
	LOOP UNTIL X

	GET.KEY% = X

END FUNCTION

SUB LOAD.SHAPES

DIM Grid(1 TO 32, 1 TO 32)

	FOR Shape = 0 TO MaxShapes - 1

		FOR Y = 1 TO 32
		FOR X = 1 TO 32
			Grid(X, Y) = 0
		NEXT X
		NEXT Y

		Style = RANDOM.INT(6)
		Colour = 1 + RANDOM.INT(15)
		
		SELECT CASE Style

		CASE 0:         ' Solid Box

			DO
				xWidth = 3 + RANDOM.INT(30)
				yWidth = 3 + RANDOM.INT(30)
			LOOP UNTIL ((xWidth * yWidth) <= 512)

			FOR Y = 1 TO yWidth
				FOR X = 1 TO xWidth
					Grid(X, Y) = Colour
				NEXT X
			NEXT Y

		CASE 1:         ' Hollow Box

			DO
				xWidth = 5 + RANDOM.INT(28)
				yWidth = 5 + RANDOM.INT(28)
			LOOP UNTIL ((xWidth * yWidth) <= 512)

			FOR Y = 1 TO yWidth
				FOR X = 1 TO xWidth
					Grid(X, Y) = Colour
				NEXT X
			NEXT Y

			HollowX = 1 + RANDOM.INT(xWidth \ 2 - 1)
			HollowY = 1 + RANDOM.INT(yWidth \ 2 - 1)

			FOR Y = HollowY + 1 TO yWidth - HollowY
				FOR X = HollowX + 1 TO xWidth - HollowX
					Grid(X, Y) = nil
				NEXT X
			NEXT Y

		CASE 2:         ' Solid Diamond

			xWidth = 3 + 2 * RANDOM.INT(10)
			yWidth = xWidth
			Centre = xWidth \ 2

			FOR Y = 0 TO Centre
				FOR X = 0 TO Y
					Grid(Centre - X + 1, Y + 1) = Colour
					Grid(Centre + X + 1, Y + 1) = Colour
					Grid(Centre - X + 1, yWidth - Y) = Colour
					Grid(Centre + X + 1, yWidth - Y) = Colour
				NEXT X
			NEXT Y


		CASE 3:         ' Hollow Diamond


			xWidth = 3 + 2 * RANDOM.INT(10)
			yWidth = xWidth
			Centre = xWidth \ 2
			sWidth = RANDOM.INT(Centre)

			FOR Y = 0 TO Centre
				FOR X = 0 TO Y
					IF X + (Centre - Y) >= sWidth THEN
						Grid(Centre - X + 1, Y + 1) = Colour
						Grid(Centre + X + 1, Y + 1) = Colour
						Grid(Centre - X + 1, yWidth - Y) = Colour
						Grid(Centre + X + 1, yWidth - Y) = Colour
					END IF
				NEXT X
			NEXT Y

		CASE 4:         ' Ball

			xWidth = 7 + 2 * RANDOM.INT(8)
			yWidth = xWidth
			Centre = 1 + xWidth \ 2

			FOR Y = 1 TO yWidth
				FOR X = 1 TO xWidth
					D = SQR(((Centre - X) * (Centre - X)) + ((Centre - Y) * (Centre - Y)))
					IF D < Centre THEN Grid(X, Y) = 150 + Colour * 2 + D * 3
				NEXT X
			NEXT Y

		CASE 5:         ' Ball


			xWidth = 7 + 2 * RANDOM.INT(8)
			yWidth = xWidth
			Centre = 1 + xWidth \ 2
			sWidth = RANDOM.INT(xWidth)

			FOR Y = 1 TO yWidth
				FOR X = 1 TO xWidth
					D = SQR(((Centre - X) * (Centre - X)) + ((Centre - Y) * (Centre - Y)))
					IF D < Centre AND D >= sWidth THEN Grid(X, Y) = 150 + Colour * 2 + D * 3
				NEXT X
			NEXT Y

		END SELECT

		Img(Shape).xWidth = xWidth
		Img(Shape).yWidth = yWidth

		A$ = STRING$(xWidth * yWidth, nil)

		c = 1
		FOR Y = 1 TO yWidth
			FOR X = 1 TO xWidth
				MID$(A$, c, 1) = CHR$(Grid(X, Y))
				c = c + 1
			NEXT X
		NEXT Y

		Img(Shape).ImgData = A$
		

	NEXT Shape

END SUB

SUB PAGE.DEMO

CONST MaxSprites = 64

DIM Obj(MaxSprites) AS Sprite
DIM LastX(MaxSprites, 1), LastY(MaxSprites, 1)
DIM LastObjects(1)

	ScreenX = 360: ScreenY = 240

	IF SET.VGA.MODEX%(Mode320x200, ScreenX, ScreenY, 3) = 0 THEN
		ERROR.OUT "Unable to SET_VGA_MODEX" + STR$(Mode)
	END IF

	SET.ACTIVE.PAGE 0

	CLEAR.VGA.SCREEN c.BLACK
				
	PRINT.TEXT "This is a Test of the Following Functions:", 10, 9, c.bWHITE, c.BLACK

	DRAW.LINE 10, 18, 350, 18, c.YELLOW
	PRINT.TEXT "SET_ACTIVE_PAGE", 10, 20, c.bBLUE, c.BLACK
	PRINT.TEXT "SET_DISPLAY_PAGE", 10, 30, c.GREEN, c.BLACK
	PRINT.TEXT "SET_DAC_REGISTER", 10, 40, c.RED, c.BLACK
	PRINT.TEXT "CLEAR_VGA_SCREEN", 10, 50, c.CYAN, c.BLACK

	PRINT.TEXT "TDRAW_BITMAP", 10, 60, c.PURPLE, c.BLACK
	PRINT.TEXT "COPY_PAGE", 10, 70, c.GREEN, c.BLACK
	PRINT.TEXT "COPY_BITMAP", 10, 80, c.CYAN, c.BLACK

	PRINT.TEXT "GPRINTC", 10, 90, c.BLUE, c.BLACK
	PRINT.TEXT "TGPRINTC", 10, 100, c.GREEN, c.BLACK
	PRINT.TEXT "SET_WINDOW", 10, 110, c.RED, c.BLACK

	PRINT.TEXT "VIRTUAL SCREEN SIZES", 190, 20, c.bBLUE, c.BLACK
	PRINT.TEXT "    SMOOTH SCROLLING", 190, 30, c.GREEN, c.BLACK
	PRINT.TEXT "    SPRITE ANIMATION", 190, 40, c.CYAN, c.BLACK
	PRINT.TEXT "       PAGE FLIPPING", 190, 50, c.RED, c.BLACK
	PRINT.TEXT "       COLOR CYCLING", 190, 60, c.PURPLE, c.BLACK


	FOR X = 0 TO 60
		SET.DAC.REGISTER 50 + X, 3 + X, 0, 60 - X
		SET.DAC.REGISTER 150 + X, 3 + X, 0, 60 - X
	NEXT X

	c = 0: DC = 1
	FOR X = 0 TO ScreenX \ 2
		DRAW.LINE ScreenX \ 2 - 1, ScreenY \ 4, X, ScreenY - 1, c + 50
		DRAW.LINE ScreenX \ 2, ScreenY \ 4, ScreenX - X - 1, ScreenY - 1, c + 50
		c = c + DC
		IF c = 0 OR c = 60 THEN DC = -DC
	NEXT X
				
	TPRINT.TEXT "Press <ANY KEY> to Continue", 72, 190, c.bWHITE
	TPRINT.TEXT "< > = Faster   < > = Slower", 72, 204, c.bGREEN
	TPRINT.TEXT "< > = Fewer Shapes  < > = More Shapes", 32, 218, c.bCYAN

	TGPRINTC 43, 80, 204, c.YELLOW
	TGPRINTC 45, 200, 204, c.YELLOW

	TGPRINTC 25, 40, 218, c.YELLOW
	TGPRINTC 24, 200, 218, c.YELLOW

	COPY.PAGE 0, 1
	COPY.PAGE 0, 2

	FOR X = 1 TO MaxSprites
		DO
			Obj(X).XDir = RANDOM.INT(7) - 3
			Obj(X).YDir = RANDOM.INT(7) - 3
		LOOP WHILE (Obj(X).XDir = 0 AND Obj(X).YDir = 0)

		Obj(X).Shape = X MOD MaxShapes

		SpriteX = Img(Obj(X).Shape).xWidth
		SpriteY = Img(Obj(X).Shape).yWidth

		Obj(X).Xpos = 1 + RANDOM.INT(ScreenX - SpriteX - 2)
		Obj(X).Ypos = 1 + RANDOM.INT(ScreenY - SpriteY - 2)

		LastX(X, 0) = Obj(X).Xpos
		LastX(X, 1) = Obj(X).Xpos
		LastY(X, 0) = Obj(X).Ypos
		LastY(X, 1) = Obj(X).Ypos
	NEXT X

	CurrentPage = 0

	'View Shift...

	ViewX = 0
	ViewY = 0
	ViewMax = 3
	ViewCnt = 0
	ViewXD = 1
	ViewYD = 1

	SetColor = 3: SDir = 1
	PrevColor = 0: PDir = 1

	VisObjects = MaxSprites \ 2
	LastObjects(0) = 0
	LastObjects(1) = 0

DRAW.LOOP:


	SET.ACTIVE.PAGE CurrentPage

	' Erase Old Images

	FOR X = 1 TO LastObjects(CurrentPage)
	
		X1 = LastX(X, CurrentPage) AND &HFFFC
		Y1 = LastY(X, CurrentPage)
		X2 = ((LastX(X, CurrentPage) + Img(Obj(X).Shape).xWidth)) OR 3
		Y2 = Y1 + Img(Obj(X).Shape).yWidth - 1

		COPY.BITMAP 2, X1, Y1, X2, Y2, CurrentPage, X1, Y1
	
	NEXT X

	' Draw new images

	FOR X = 1 TO VisObjects

		SpriteX = Img(Obj(X).Shape).xWidth
		SpriteY = Img(Obj(X).Shape).yWidth

		' Move Sprite

REDOX:
		NewX = Obj(X).Xpos + Obj(X).XDir
		IF NewX < 0 OR NewX + SpriteX > ScreenX THEN
			Obj(X).XDir = -Obj(X).XDir
			IF RANDOM.INT(20) = 1 THEN
				DO
					Obj(X).XDir = RANDOM.INT(7) - 3
					Obj(X).YDir = RANDOM.INT(7) - 3
				LOOP WHILE (Obj(X).XDir = 0 AND Obj(X).YDir = 0)
				GOTO REDOX
			END IF
		END IF
		Obj(X).Xpos = Obj(X).Xpos + Obj(X).XDir

REDOY:
		NewY = Obj(X).Ypos + Obj(X).YDir
		IF NewY < 0 OR NewY + SpriteY > ScreenY THEN
			Obj(X).YDir = -Obj(X).YDir
			IF RANDOM.INT(20) = 1 THEN
				DO
					Obj(X).XDir = RANDOM.INT(7) - 3
					Obj(X).YDir = RANDOM.INT(7) - 3
				LOOP WHILE (Obj(X).XDir = 0 AND Obj(X).YDir = 0)
				GOTO REDOY
			END IF
		END IF
		Obj(X).Ypos = Obj(X).Ypos + Obj(X).YDir

		'Draw Sprite

		TDRAW.BITMAP Img(Obj(X).Shape), Obj(X).Xpos, Obj(X).Ypos, SpriteX, SpriteY

		LastX(X, CurrentPage) = Obj(X).Xpos
		LastY(X, CurrentPage) = Obj(X).Ypos

	NEXT X

	LastObjects(CurrentPage) = VisObjects

	' Pan Screen Back & Forth

	ViewCnt = ViewCnt + 1
	IF ViewCnt >= ViewMax THEN
		ViewX = ViewX + ViewXD
		IF ViewX = 0 OR ViewX = 39 THEN ViewXD = -ViewXD
		IF ViewXD < 0 THEN
			ViewY = ViewY + ViewYD
			IF ViewY = 0 OR ViewY = 39 THEN ViewYD = -ViewYD
		END IF
	
		SET.WINDOW CurrentPage, ViewX, ViewY

		ViewCnt = 0
	ELSE
		SET.DISPLAY.PAGE CurrentPage
	END IF

	' Cycle Colors

	SET.DAC.REGISTER 50 + PrevColor, 3 + PrevColor, 0, 60 - PrevColor
	SET.DAC.REGISTER 50 + SetColor, SetColor, 10, 63 - SetColor

	SET.DAC.REGISTER 150 + PrevColor, 3 + PrevColor, 0, 60 - PrevColor
	SET.DAC.REGISTER 150 + SetColor, 63, 63, SetColor

	SetColor = SetColor + SDir
	IF SetColor = 60 OR SetColor = 0 THEN SDir = -SDir

	PrevColor = PrevColor + PDir
	IF PrevColor = 60 OR PrevColor = 0 THEN PDir = -PDir

	CurrentPage = 1 - CurrentPage

	Code = SCAN.KEYBOARD

	IF Code = False THEN GOTO DRAW.LOOP

	IF Code = KyPlus THEN
		IF ViewMax < 12 THEN ViewMax = ViewMax + 1
		GOTO DRAW.LOOP
	END IF

	IF Code = KyMinus THEN
		IF ViewMax > 1 THEN ViewMax = ViewMax - 1
		IF ViewCnt >= ViewMax THEN ViewCnt = 0
		GOTO DRAW.LOOP
	END IF

	IF Code = KyUp THEN
		IF VisObjects < MaxSprites THEN VisObjects = VisObjects + 1
		GOTO DRAW.LOOP
	END IF
		
	IF Code = KyDown THEN
		IF VisObjects > 1 THEN VisObjects = VisObjects - 1
		GOTO DRAW.LOOP
	END IF


END SUB

SUB PRINT.TEXT (Text$, Xpos, Ypos, ColorF, ColorB)

	IF LEN(Text$) = 0 THEN EXIT SUB
	PRINT.STR VARSEG(Text$), SADD(Text$), LEN(Text$), Xpos, Ypos, ColorF, ColorB


END SUB

SUB TPRINT.TEXT (Text$, Xpos, Ypos, ColorF)

	IF LEN(Text$) = 0 THEN EXIT SUB

	TPRINT.STR VARSEG(Text$), SADD(Text$), LEN(Text$), Xpos, Ypos, ColorF

END SUB

