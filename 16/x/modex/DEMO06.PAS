(*
    DEMO06 - Magnifying glass
    (c) 1994 Alessandro Scotti
*)
uses Crt, Modex;

const
  R = 40;               (* Lens radius *)
  K : real = 1.8;       (* Magnifying factor, less makes a stronger lens *)
type
  TLine   = array[ 0..319 ] of byte;
  PLine   = ^TLine;
  TScreen = array[ 0..239 ] of PLine;
var
  VScreen: TScreen;                             (* Virtual screen *)
  BallX  : array[ 0..R, 0..R ] of integer;
  BallY  : array[ 0..R, 0..R ] of integer;
  Sprite : array[ -R..R, -R..R ] of byte;
  Page   : word;

(* Returns "lens-view" coordinates of X,Y *)
procedure GetCoords( var X, Y: integer );
var
  LR, Z, SinA, SinB, TgB, Q: real;
begin
  LR := Sqrt( X*X + Y*Y );
  if( LR = 0 ) then Exit;
  if( LR < R ) then begin
    Z := Sqrt( R*R - LR*LR );
    SinA := LR / R;
    SinB := SinA / K;
    TgB := SinB / Sqrt( 1-SinB*SinB );
    Q := LR - TgB*Z;
    X := Round( X * ( Q/LR ) );
    Y := Round( Y * ( Q/LR ) );
  end;
end;

procedure Init;
var
  F      : file;
  Palette: array[ 0..767 ] of record R, G, B: byte; end;
  X, Y,
  X2, Y2 : integer;
begin
  (* Load background image *)
  Assign( F, 'demo06.dat' );
  Reset( F, 1 );
  BlockRead( F, Palette, 768 );
  mxSetPalette( @Palette, 0, 256 );
  for Y:=0 to 239 do begin
    New( VScreen[Y] );
    BlockRead( F, VScreen[Y]^, 320 );
    mxPutImage( VScreen[Y], 0, 480+Y, 320, 1, OP_MOVE );
  end;
  Close( F );
  (* Build lens *)
  for X:=0 to R do begin
    for Y:=0 to R do begin
      X2 := X;
      Y2 := Y;
      GetCoords( X2, Y2 );
      BallX[X, Y] := X2;
      BallY[X, Y] := Y2;
    end;
  end;
end;

procedure PutLens( OX, OY: integer );
var
  X, Y: integer;
begin
  for X:=0 to R do begin
    for Y:=0 to R do begin
      Sprite[Y][X] := VScreen[ OY+BallY[X,Y] ]^[ OX+BallX[X,Y] ];
      Sprite[Y][-X] := VScreen[ OY+BallY[X,Y] ]^[ OX-BallX[X,Y] ];
      Sprite[-Y][X] := VScreen[ OY-BallY[X,Y] ]^[ OX+BallX[X,Y] ];
      Sprite[-Y][-X] := VScreen[ OY-BallY[X,Y] ]^[ OX-BallX[X,Y] ];
    end;
  end;
  (* Draw the sprite *)
  mxPutImage( @Sprite, OX-R, OY-R+Page, 2*R+1, 2*R+1, OP_MOVE );
end;

function Delta: integer;
begin
  Delta := Random(3)+2;
end;

procedure Check( Cond: boolean; var Coord, DeltaC: integer; NewCoord, Sign: integer );
begin
  if( Cond ) then begin
    Coord := NewCoord;
    DeltaC := Sign*Delta;
  end;
end;

var
  X, Y, DX, DY: integer;
  C: char;
begin
  mxInit;
  mxSetMode( MX_320x240 );
  Init;
  Page := 240;
  X := R;
  Y := R;
  Randomize;
  DX := Delta;
  DY := Delta;

  (* Main loop *)
  repeat
    (* Update video *)
    mxBitBlt( 0, 480, 320, 240, 0, Page );
    PutLens( X, Y );
    mxCircle( X, Page+Y, R, 0 );
    (* Update lens coordinates *)
    Inc( X, DX );
    Check( X+R >= 319, X, DX, 319-R, -1 );
    Check( X <= R, X, DX, R, +1 );
    Inc( Y, DY );
    Check( Y+R >= 239, Y, DY, 239-R, -1 );
    Check( Y <= R, Y, DY, R, +1 );
    (* Flip pages: double buffering, avoid wait for display *)
    case Page of
      0  : begin PortW[$3D4] := $000C; Page := 240; end;
      240: begin PortW[$3D4] := $4B0C; Page := 0; end;
    end;
    mxWaitRetrace; (* Wait for hidden page to show *)
  until( KeyPressed );

  mxSetMode( MX_TEXT );
  mxTerm;
end.
