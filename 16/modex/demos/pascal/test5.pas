{                      ModeX Turbo Pascal Demo Program                       }
{                  Converted to Turbo Pascal by Scott Wyatt                  }
{          Original program written in QuickBasic by Matt Prichard           }
{                       Released to the Public Domain                        }
{                                                                            }
{          Thanks to Matt Prichard for his *EXCELLENT* ModeX Library         }
{		   Additional Comments by Matt Pritchard							 }

Uses Crt;

{$L modex2.obj}		{ This file is the external ModeX Library .OBJ }
{$F+}

	{ Mode Setting Routines }

Function SET_VGA_MODEX (Mode,MaxXpos,MaxYpos,Pages : integer) : integer; external;
Function SET_MODEX (Mode:integer) : Integer; external;

	{ Graphics Primitives }

Procedure CLEAR_VGA_SCREEN (Color:integer); external;
Procedure SET_POINT (Xpos,Ypos,Color : integer); external;
Function READ_POINT (Xpos,Ypos:integer) : integer; external;
Procedure FILL_BLOCK (Xpos1,Ypos1,Xpos2,Ypos2,Color:integer); external;
Procedure DRAW_LINE (Xpos1,Ypos1,Xpos2,Ypos2,Color:integer); external;

	{ VGA DAC Routines }

Procedure SET_DAC_REGISTER (RegNo,Red,Green,Blue:integer); external;
Procedure GET_DAC_REGISTER (RegNo,Red,Green,Blue:integer); external;

	{ Page and Window Control Routines }

Procedure SET_ACTIVE_PAGE (PageNo:integer); external;
Function GET_ACTIVE_PAGE : integer; external;
Procedure SET_DISPLAY_PAGE (PageNo:integer); external;
Function GET_DISPLAY_PAGE : integer; external;
Procedure SET_WINDOW (DisplayPage,XOffset,YOffset : integer); external;
Function GET_X_OFFSET : integer; external;
Function GET_Y_OFFSET : integer; external;
Procedure SYNC_DISPLAY; external;

	{ Text Display Routines }

Procedure GPRINTC (CharNum,Xpos,Ypos,ColorF,ColorB:integer); external;
Procedure TGPRINTC ( CharNum,Xpos,Ypos,ColorF : integer); external;
Procedure PRINT_STR (Var Text;MaxLen,Xpos,Ypos,ColorF,ColorB:integer); external;
Procedure TPRINT_STR (Var Text;MaxLen,Xpos,Ypos,ColorF:integer); external;
Procedure SET_DISPLAY_FONT (Var FontData;FontNumber:integer); external;

	{ Sprite and VGA memory -> Vga memory Copy Routines }

Procedure DRAW_BITMAP (Var Image;Xpos,Ypos,Width,Height:integer); external;
Procedure TDRAW_BITMAP (Var Image;Xpos,Ypos,Width,Height:integer); external;
Procedure COPY_PAGE (SourcePage,DestPage:integer); external;
Procedure COPY_BITMAP (SourcePage,X1,Y1,X2,Y2,DestPage,DestX1,DestY1:integer); external;

{$F-}


TYPE Sprite = Record
	Xpos : INTEGER;
	Ypos : INTEGER;
	XDir : INTEGER;
	YDir : INTEGER;
	Shape : INTEGER;
	LastX : INTEGER;
	LastY : INTEGER;
     END;


CONST MaxShapes = 32;
      Circle_16 : Array[1..16,1..16] of byte =
      ((  0,  0,  0,  0,  0,  0, 20, 20, 20, 20,  0,  0,  0,  0,  0,  0),
       (  0,  0,  0,  0, 20, 20, 20, 20, 20, 20, 20, 20,  0,  0,  0,  0),
       (  0,  0,  0, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,  0,  0,  0),
       (  0,  0, 20, 20, 20, 20,  0,  0,  0,  0, 20, 20, 20, 20,  0,  0),
       (  0, 20, 20, 20, 20,  0,  0,  0,  0,  0,  0, 20, 20, 20, 20,  0),
       (  0, 20, 20, 20,  0,  0,  0,  0,  0,  0,  0,  0, 20, 20, 20,  0),
       ( 20, 20, 20,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 20, 20, 20),
       ( 20, 20, 20,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 20, 20, 20),
       ( 20, 20, 20,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 20, 20, 20),
       ( 20, 20, 20,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 20, 20, 20),
       (  0, 20, 20, 20,  0,  0,  0,  0,  0,  0,  0,  0, 20, 20, 20,  0),
       (  0, 20, 20, 20, 20,  0,  0,  0,  0,  0,  0, 20, 20, 20, 20,  0),
       (  0,  0, 20, 20, 20, 20,  0,  0,  0,  0, 20, 20, 20, 20,  0,  0),
       (  0,  0,  0, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,  0,  0,  0),
       (  0,  0,  0,  0, 20, 20, 20, 20, 20, 20, 20, 20,  0,  0,  0,  0),
       (  0,  0,  0,  0,  0,  0, 20, 20, 20, 20,  0,  0,  0,  0,  0,  0));
      Square_16 : Array[1..16,1..16] of byte =
      (( 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21),
       ( 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21),
       ( 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21),
       ( 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21),
       ( 21, 21, 21, 21,  0,  0,  0,  0,  0,  0,  0,  0, 21, 21, 21, 21),
       ( 21, 21, 21, 21,  0,  0,  0,  0,  0,  0,  0,  0, 21, 21, 21, 21),
       ( 21, 21, 21, 21,  0,  0,  0,  0,  0,  0,  0,  0, 21, 21, 21, 21),
       ( 21, 21, 21, 21,  0,  0,  0,  0,  0,  0,  0,  0, 21, 21, 21, 21),
       ( 21, 21, 21, 21,  0,  0,  0,  0,  0,  0,  0,  0, 21, 21, 21, 21),
       ( 21, 21, 21, 21,  0,  0,  0,  0,  0,  0,  0,  0, 21, 21, 21, 21),
       ( 21, 21, 21, 21,  0,  0,  0,  0,  0,  0,  0,  0, 21, 21, 21, 21),
       ( 21, 21, 21, 21,  0,  0,  0,  0,  0,  0,  0,  0, 21, 21, 21, 21),
       ( 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21),
       ( 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21),
       ( 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21),
       ( 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21));
       Diamond : Array[1..8,1..8] of byte =
      ((  0,  0,  0, 22, 22,  0,  0,  0),
       (  0,  0, 22, 22, 22, 22,  0,  0),
       (  0, 22, 22,  0,  0, 22, 22,  0),
       ( 22, 22,  0,  0,  0,  0, 22, 22),
       ( 22, 22,  0,  0,  0,  0, 22, 22),
       (  0, 22, 22,  0,  0, 22, 22,  0),
       (  0,  0, 22, 22, 22, 22,  0,  0),
       (  0,  0,  0, 22, 22,  0,  0,  0));
       Rectangle : Array[1..8,1..3] of byte =
      (( 23, 23, 23),
       ( 23, 23, 23),
       ( 23, 23, 23),
       ( 23, 23, 23),
       ( 23, 23, 23),
       ( 23, 23, 23),
       ( 23, 23, 23),
       ( 23, 23, 23));

	   { Global Variables ? }

Var
  XCenter,X1,Y1,X2,Y2,Z,Colr,XChars,YChars,X,Y,N,Gap : Integer;
  s : string;
  s1 : Array[1..35] of Char;
  ch : Char;
  obj : Array[1..64] of Sprite;
  ScreenX,ScreenY : Integer;
  c, dc, SpriteX, SpriteY, CurrentPage, LastPage : Integer;
  SetColor, SDir, PrevColor, PDir : Byte;
  XView, YView : Integer;
  XView_Change, YView_Change : Integer;
  Right : Boolean;
  Number_Of_Shapes : Byte;


	{ Error Handler - Returns to Text Mode & Displays Error }

Procedure ERROR_OUT(s : string);
  Begin
    asm
      mov ah,0
      mov al,3
      int 10h
    end;
  WriteLn(s);
  Halt(0);
END;

	{ Routine to Print a PASCAL string using Print_Str }

Procedure Print_Text(s : string; X,Y,BColor,FColor : integer);
Var
  s1 : Array[1..135] of Char;
  i : byte;
Begin
  For i := 1 to Length(s) DO
    s1[i] := s[i];
  Print_Str(s1,Length(s),X,Y,BColor,FColor);
End;

	{ Routine to Transparently Print a PASCAL string using TPrint_Str }

Procedure TPrint_Text(s : string; X,Y,Color : integer);
Var
  s1 : Array[1..135] of Char;
  i : byte;
Begin
  For i := 1 to Length(s) DO
    s1[i] := s[i];
  TPrint_Str(s1,Length(s),X,Y,Color);
End;

	{ Routines to show test patterns for a given mode }

Procedure Demo_Res(Mode, Xmax, Ymax : integer);
Begin

  Str(mode,s);
  If Set_ModeX(Mode) = 0 Then
    Error_Out('Unable to SET_MODEX '+s);
  Clear_VGA_Screen(0);

  XCenter := Xmax div 2;
  X1 := 10;
  Y1 := 10;
  X2 := Xmax - 1;
  Y2 := Ymax - 1;

  FOR Z := 0 TO 3 DO
    Begin
      Colr := 31 - Z * 2;
      Draw_Line(X1 + Z, Y1 + Z, X2 - Z, Y1 + Z, Colr);
      Draw_Line(X1 + Z, Y1 + Z, X1 + Z, Y2 - Z, Colr);
      Draw_Line(X1 + Z, Y2 - Z, X2 - Z, Y2 - Z, Colr);
      Draw_Line(X2 - Z, Y1 + Z, X2 - Z, Y2 - Z, Colr);
    End;

  XChars := Xmax div 10;
  YChars := Ymax div 10;

  FOR X := 0 TO XChars - 1 DO
    Begin
      TGPRINTC(48 + ((X + 1) MOD 10), X * 10 + 1, 1, 9 + ((X div 8) MOD 7));
      DRAW_LINE(X * 10 + 9, 0, X * 10 + 9, 3, 15);
    End;
  FOR Y := 0 TO YChars - 1 DO
    Begin
      TGPRINTC(48 + ((Y + 1) MOD 10), 1, Y * 10 + 1, 9 + ((Y div 10) MOD 7));
      DRAW_LINE(0, Y * 10 + 9, 3, Y * 10 + 9, 15);
    End;

   { Test Line Drawing }

  FOR X := 0 TO 63 DO
    Begin
      N := 15 + ((X * 3) div 4);
      SET_DAC_REGISTER(64 + X, N, N, N);
      SET_DAC_REGISTER(128 + X, 0, N, N);
      DRAW_LINE(103 - X, 60, 40 + X, 123, 64 + X);
      DRAW_LINE(40, 60 + X, 103, 123 - X, 128 + X);
    End;
  s := 'Line Test';
  PRINT_Text(s,37,130,1,0);

  { Test Block Fills }

  Y := 60;
  Gap := 0;
  FOR X := 0 TO 9 DO
    Begin
      FILL_BLOCK(120, Y, 120 + X, Y + Gap, 64 + X);
      FILL_BLOCK(140 - (15 - X), Y, 150 + X, Y + Gap, 230 + X);
      FILL_BLOCK(170 - (15 - X), Y, 170, Y + Gap, 128 + X);
      Y := Y + Gap + 2;
      Gap := Gap + 1;
    End;
  s := 'Fill Test';
  Print_Text(s,110, 46, 2,0);

  { Test Pixel Write and Read }

  FOR X := 190 TO 250 DO
    FOR Y := 60 TO 122 DO
      SET_POINT( X, Y, X + Y + X + Y);

  s := 'Pixel Test';
  Print_Text(s,182, 130, 3,0);

  FOR X := 190 TO 250 DO
    FOR Y := 60 TO 122 DO
      IF READ_POINT(X, Y) <> ((X + Y + X + Y) AND 255) THEN
        WriteLn('READ_PIXEL Failure');

	{ Display rest of screen }

  s := ' This is a MODE X demo ';
  Print_Text(s,XCenter - (Length(s) * 4), 20, 3, 1);
  s := 'Screen Resolution is     by    ';
  X := XCenter - (Length(s) * 4);
  Print_Text(s,X,30,4,0);
  Str(XMax,s);
  Print_Text(s, X + 8 * 21, 30, 8, 0);
  Str(YMax,s);
  Print_Text(s, X + 8 * 28, 30, 15, 0);

  FOR X := 0 TO 15 DO
    Begin
      SET_DAC_REGISTER( 230 + X, 63 - X * 4, 0, 15 + X * 3);
      DRAW_LINE(30 + X, Ymax - 6 - X, Xmax - 20 - X, Ymax - 6 - X, 230 + X);
    End;
  s := 'Press <ANY KEY> to Continue';
  For x := 1 to length(s) DO
    s1[x] := s[x];
  TPrint_Str(s1, length(s), XCenter - (26 * 4), Ymax - 18, 5);

  Ch := ReadKey;
  IF Ch = #27 Then
    Error_Out('Abort');

End;


	{ Initialize Sprites for Sprite Demo }

Procedure Init_Sprites;
Var i : byte;
Begin
  For i := 1 to 64 DO
    Begin
      Obj[i].XPos := Random(300)+10;
      Obj[i].YPos := Random(200)+20;
      Obj[i].XDir := Random(10)-5;
      Obj[i].YDir := Random(10)-5;
      If (Obj[i].XDir = 0) AND (Obj[i].YDir = 0) Then
        Begin
          Obj[i].XDir := Random(5) + 1;
          Obj[i].YDir := Random(5) + 1;
        End;
      Obj[i].Shape := Random(4)+1;
      Obj[i].LastX := obj[i].XPos;
      Obj[i].LastY := obj[i].YPos;
    End;
End;

Procedure Set_Sprites(number : byte);
Var i : Byte;
Begin
  For i := 1 to number DO
    Begin
      obj[i].LastX := obj[i].XPos;
      obj[i].LastY := obj[i].YPos;
      obj[i].XPos := obj[i].XPos + obj[i].XDir;
      obj[i].YPos := obj[i].YPos + obj[i].YDir;
      If (obj[i].XPos > 335) OR (obj[i].XPos < 5 ) Then
        obj[i].XDir := -(obj[i].XDir);
      If (obj[i].YPos > 220) OR (obj[i].YPos < 5) Then
        obj[i].YDir := -(obj[i].YDir);
    End;
  For i := 1 to number DO
    Case obj[i].Shape of
      1 : TDraw_Bitmap(Circle_16,obj[i].XPos,obj[i].YPos,16,16);
      2 : TDraw_Bitmap(Square_16,obj[i].XPos,obj[i].YPos,16,16);
      3 : TDraw_Bitmap(Diamond,obj[i].XPos,obj[i].YPos,8,8);
      4 : TDraw_Bitmap(Rectangle,obj[i].XPos,obj[i].YPos,3,8);
    End;
End;

Procedure Remove_Sprites(p,number : byte);
Var i : byte;
Begin
  For i := 1 to number DO
    Copy_Bitmap(2,obj[i].LastX,obj[i].LastY,obj[i].LastX+16,obj[i].LastY+16,p,Obj[i].LastX,Obj[i].LastY);
End;

Procedure Page_Demo;
Begin
  Number_Of_Shapes := 64;
  XView_Change := 1;
  YView_Change := 1;
  XView := 1;
  YView := 1;
  Right := TRUE;
  ScreenX := 360;
  ScreenY := 240;
  PrevColor := 0;
  SetColor := 3;
  SDir := 1;
  PDir := 1;
  Str(0,s);

  IF SET_VGA_MODEX(0, ScreenX, ScreenY, 3) = 0 THEN
    ERROR_OUT('Unable to SET_VGA_MODEX' + S);

  SET_ACTIVE_PAGE(0);
  CLEAR_VGA_SCREEN(0);
  PRINT_TEXT('This is a Test of the Following Functions:', 10, 9, 15, 0);
  DRAW_LINE( 10, 18, 350, 18, 4);
  Print_Text('SET_ACTIVE_PAGE', 10, 20, 1, 0);
  Print_Text('SET_DISPLAY_PAGE', 10, 30, 3,0);
  Print_Text('SET_DAC_REGISTER', 10, 40, 3, 0);
  Print_Text('CLEAR_VGA_SCREEN', 10, 50, 13, 0);
  Print_Text('TDRAW_BITMAP', 10, 60, 14, 0);
  Print_Text('COPY_PAGE', 10, 70, 3, 0);
  Print_Text('COPY_BITMAP', 10, 80, 13, 0);
  Print_Text('GPRINTC', 10, 90, 1, 0);
  Print_Text('TGPRINTC', 10, 100, 3, 0);
  Print_Text('SYNC_DISPLAY', 10, 110, 3, 0);
  Print_Text('SET_WINDOW', 10, 120, 14, 0);
  Print_Text('VIRTUAL SCREEN SIZES', 190, 20, 1, 0);
  Print_Text('    SMOOTH SCROLLING', 190, 30, 3, 0);
  Print_Text('    SPRITE ANIMATION', 190, 40, 13, 0);
  Print_Text('       PAGE FLIPPING', 190, 50, 3, 0);
  Print_Text('       COLOR CYCLING', 190, 60, 14, 0);

  FOR X := 0 TO 60 DO
    Begin
      SET_DAC_REGISTER( 50 + X, 3 + X, 0, 60 - X);
      SET_DAC_REGISTER( 150 + X, 3 + X, 0, 60 - X);
    End;

  c := 0;
  DC := 1;
  FOR X := 0 TO ScreenX div 2 DO
    Begin
      DRAW_LINE( ScreenX div 2 - 1, ScreenY div 4, X, ScreenY - 1, c + 50);
      DRAW_LINE( ScreenX div 2, ScreenY div 4, ScreenX - X - 1, ScreenY - 1, c + 50);
      c := c + DC;
      IF (c = 0) OR (c = 60) THEN DC := -DC;
    End;

  TPrint_Text('Press <ESC> to Continue', 82, 190, 15);
  TPrint_Text('<+> = Fewer Shapes  <-> = More Shapes', 32, 204, 12);
  COPY_PAGE( 0, 1);
  COPY_PAGE( 0, 2);

  Ch := #0;
  CurrentPage := 1;
  LastPage := 0;
  Set_Sprites(Number_Of_Shapes);
  For c := 1 to 4 DO
    Set_Dac_Register(19+c,63-(c*10),0,0);

  While Ch <> #27 DO
    Begin
      Set_Active_Page(currentpage);
      Set_Sprites(Number_Of_Shapes);
      If Right Then
        Begin
          XView := XView + XView_Change;
          If (XView > 38) OR (XView < 2) Then
            Begin
              XView_Change := -(XView_Change);
              Right := FALSE;
            End;
        End
      Else
        Begin
          YView := YView + YView_Change;
          If (YView > 38) OR (YView < 2) Then
            Begin
              YView_Change := -(YView_Change);
              Right := TRUE;
            End;
        End;

      Set_Window(currentpage,XView,YView);
      Set_Display_Page(currentpage);
      Set_Dac_Register(50 + PrevColor, 3 + PrevColor, 0, 60 - PrevColor);
      Set_Dac_Register(50 + SetColor, SetColor, 10, 63 - SetColor);
      Set_Dac_Register(150 + PrevColor, 3 + PrevColor, 0, 60 - PrevColor);
      Set_Dac_Register(150 + SetColor, 63, 63, SetColor);
      SetColor := SetColor + SDir;
      IF (SetColor = 60) OR (SetColor = 0) THEN SDir := -SDir;
      PrevColor := PrevColor + PDir;
      IF (PrevColor = 60) OR (PrevColor = 0) THEN PDir := -PDir;
      Remove_Sprites(lastpage,Number_Of_Shapes);

      If Keypressed Then
        Begin
          Ch := ReadKey;
          Case Ch of
            '-' : If Number_Of_Shapes > 1 Then
                    Begin
                      c := Number_Of_Shapes;
                      Copy_Bitmap(2,obj[c].XPos,obj[c].YPos,obj[c].XPos+16,obj[c].YPos+16,
                         currentpage,obj[c].XPos,obj[c].YPos);
                      Dec(Number_Of_Shapes);
                    End;
            '+' : If Number_Of_Shapes < 64 Then Inc(Number_Of_Shapes);
          End;
        End;
      lastpage := (lastpage+1) MOD 2;
      currentpage := (currentpage+1) MOD 2;
    End;
END;

	{ MAIN ROUTINE - Run Through Demos and Exit }

Begin

  Randomize;
  Init_Sprites;

  Demo_Res(0, 320, 200);
  Demo_Res(1, 320, 400);
  Demo_Res(2, 360, 200);
  Demo_Res(3, 360, 400);
  Demo_Res(4, 320, 240);
  Demo_Res(5, 320, 480);
  Demo_Res(6, 360, 240);
  Demo_Res(7, 360, 480);
  Page_Demo;

  asm
    mov ah,0
    mov al,3
    int 10h
  end;
  WriteLn('THIS MODE X DEMO IS FINISHED');

END.