{$E-,N+}
uses Crt, Modex;

const
  DEFVERT = 12;         (* Vertex count *)
  DEFREPL = 3;          (* Repetition count *)
  DEFQIXS = 2;          (* Qixs *)
  FADESPEED = 48;
type
  TPoint = record
    X, Y : integer;
  end;
  TRGB = record
    R, G, B: byte;
  end;
  TQix = record
    Color: integer;
    Vert : array[ 0..DEFVERT-1, 0..DEFREPL-1 ] of TPoint;
    Delta: array[ 0..DEFVERT-1 ] of TPoint;
  end;
var
  Page : integer;
  MaxX,
  MaxY : word;
  Qix  : array[ 0..DEFQIXS-1 ] of TQix;
  Pal  : array[ byte ] of TRGB;

type
  TReal = double;
  TRPoint = record
    X, Y: TReal;
  end;
  TMatrix = array[ 0..3, 0..3 ] of TReal;
var
  M: TMatrix;
  G: array[ 0..DEFVERT-1 ] of TRPoint;
  C: array[ 0..DEFVERT-1 ] of TRPoint;

procedure BumpPal( Idx, DR, DG, DB, Steps: integer );
var
  I: integer;
begin
  for I:=1 to Steps do begin
    Pal[Idx+1].R := Pal[Idx].R + DR;
    Pal[Idx+1].G := Pal[Idx].G + DG;
    Pal[Idx+1].B := Pal[Idx].B + DB;
    Inc( Idx );
  end;
end;

procedure InitPalette;
begin
  with Pal[0] do begin R:=0; G:=0; B:=0; end;
  with Pal[1] do begin R:=0; G:=0; B:=62; end;
  BumpPal( 1,   0, 2, -2,  31 );
  BumpPal( 32,  2, -2, 0,  31 );
  BumpPal( 63,  -2, 2, 2,  31 );
  BumpPal( 94,  2, 0, -2,  31 );
  BumpPal( 125, -2, -2, 2, 31 );
end;

procedure Init( var Qix: TQix; Color: integer );
var
  I: integer;
begin
  FillChar( Qix.Vert, SizeOf(Qix.Vert), 0 );
  for I:=0 to DEFVERT-1 do begin
    Qix.Vert[I, DEFREPL-1].X := Random( MaxX );
    Qix.Vert[I, DEFREPL-1].Y := Random( MaxY );
    Qix.Delta[I].X := Random(5)+1;
    Qix.Delta[I].Y := Random(5)+1;
  end;
  Qix.Color := Color;

  (* Initialize matrix (Catmull-Rom) *)
  M[0,0] := -1/2; M[0,1] := 3/2; M[0,2] := -3/2; M[0,3] := 1/2;
  M[1,0] := 1; M[1,1] := -5/2; M[1,2] := 2; M[1,3] := -1/2;
  M[2,0] := -1/2; M[2,1] := 0; M[2,2] := 1/2; M[2,3] := 0;
  M[3,0] := 0; M[3,1] := 1; M[3,2] := 0; M[3,3] := 0;
end;

procedure mxBezier( var Qix: TQix; I0, Idx, N: integer );
var
  I, J: integer;
  T, T2, T3: TReal;
  X0, Y0, X, Y: TReal;
  Delta: TReal;
begin
  (* Compute coefficients *)
  for I:=0 to 3 do begin
    C[I].X := 0;
    for J:=0 to 3 do C[I].X := C[I].X + M[I,J]*Qix.Vert[(I0+J) mod DEFVERT,Idx].X;
    C[I].Y := 0;
    for J:=0 to 3 do C[I].Y := C[I].Y + M[I,J]*Qix.Vert[(I0+J) mod DEFVERT,Idx].Y;
  end;
  X0 := C[3].X;
  Y0 := C[3].Y;
  Delta := 1 / N;
  T := 0;
  for I:=1 to N do begin
    T := T + Delta;
    T2 := T*T;
    T3 := T*T2;
    X := C[0].X*T3 + C[1].X*T2 + C[2].X*T + C[3].X;
    Y := C[0].Y*T3 + C[1].Y*T2 + C[2].Y*T + C[3].Y;
    mxLine( Round(X0), Page+Round(Y0), Round(X), Page+Round(Y), Qix.Color, OP_SET );
    X0 := X;
    Y0 := Y;
  end;
end;

procedure Plot( var Qix: TQix; Idx: integer );
var
  I, J: integer;
begin
  for I:=0 to DEFVERT-1 do begin
    mxBezier( Qix, I, Idx, 12 );
  end;
end;

procedure Update( var Qix: TQix; Idx: integer );
var
  I: integer;
begin
  for I:=0 to DEFVERT-1 do with Qix do begin
    Inc( Vert[I,Idx].X, Delta[I].X );
    if( Vert[I,Idx].X < 0 ) then begin
      Vert[I,Idx].X := 0;
      Delta[I].X := Random( 5 )+1;
    end;
    if( Vert[I,Idx].X > MaxX ) then begin
      Vert[I,Idx].X := MaxX;
      Delta[I].X := -Random( 5 )-1;
    end;
    Inc( Vert[I,Idx].Y, Delta[I].Y );
    if( Vert[I,Idx].Y < 0 ) then begin
      Vert[I,Idx].Y := 0;
      Delta[I].Y := Random( 5 )+1;
    end;
    if( Vert[I,Idx].Y > MaxY ) then begin
      Vert[I,Idx].Y := MaxY;
      Delta[I].Y := -Random( 5 )-1;
    end;
  end;
end;

procedure Copy( var Qix: TQix; Dest, Src: integer );
var
  I: integer;
begin
  for I:=0 to DEFVERT-1 do with Qix do begin
    Vert[I,Dest].X := Vert[I,Src].X;
    Vert[I,Dest].Y := Vert[I,Src].Y;
  end;
end;

procedure AnimateQix;
var
  Q, Idx, I, J, P, Count: integer;
begin
  Count := 0;
  P := DEFREPL-1;
  I := 0;
  J := 1;
  repeat
    mxSetClipRegion( 0, Page, MaxX+1, MaxY+1 );
    mxSetClip( TRUE );
    mxFillBox( 0, Page, MaxX+1, MaxY+1, 0, OP_SET );
    for Q:=0 to DEFQIXS-1 do begin
      Copy( Qix[Q], I, P );
      Update( Qix[Q], I );
      for Idx:=0 to DEFREPL-1 do begin
        Plot( Qix[Q], Idx );
      end;
    end;
    I := (I+1) mod DEFREPL;
    J := (J+1) mod DEFREPL;
    P := (P+1) mod DEFREPL;
    Inc( Count );
    mxStartLine( Page );
    if( Count >= FADESPEED ) then begin
      for Q:=0 to DEFQIXS-1 do begin
        Inc( Qix[Q].Color );
        if( Qix[Q].Color > 156 ) then
          Qix[Q].Color := 1;
      end;
      Count := 0;
    end;
    Page := 240-Page;
  until( KeyPressed );
end;

var
  I: integer;
begin
  Randomize;
  mxInit;
  mxSetMode( MX_320x240 );
  mxGetScreenSize( MaxX, MaxY );
  for I:=0 to DEFQIXS-1 do
    Init( Qix[I], (I*(155 div DEFQIXS)) mod 155 + 1 );
  InitPalette;
  mxSetPalette( @Pal, 0, 157 );
  Page := 240;
  Dec( MaxX );
  Dec( MaxY );
  AnimateQix;
  mxSetMode( MX_TEXT );
  mxTerm;
end.
