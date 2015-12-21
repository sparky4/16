(*
    DEMO07 - Hardware scrolling
    Copyright (c) 1994 Alessandro Scotti
*)
uses Crt, Modex;

const
  (* Change this if scrolling seems jerky (this simple program does *)
  (* not handle vertical retrace/display very well) *)
  STEPS = 5;

procedure Check( Cond: boolean; var Coord, DeltaC: integer; NewCoord, Sign: integer );
begin
  if( Cond ) then begin
    Coord := NewCoord;
    DeltaC := Sign*(Random(3)+2);
  end;
end;

var
  I, X, Y, DX, DY: integer;
begin
  (* Initialize library and graphics mode *)
  mxInit;
  mxSetMode( MX_320x200 );
  (* Set a 640x400 virtual screen *)
  mxSetVirtualScreen( 640, 400 );
  mxSetClip( TRUE );

  X := 0;
  Y := 0;
  DX := 1;
  DY := 1;

  (* Main loop: draw lines, circles, points and rectangles in separate *)
  (* 320x200 windows, while smoothly panning virtual screen *)
  while( not KeyPressed ) do begin
    (* Points *)
    mxSetClipRegion( 0, 0, 320, 200 );
    for I:=1 to STEPS do
      mxPutPixel( Random(320), Random(200), Random(16) );
    (* Lines *)
    mxSetClipRegion( 0, 200, 320, 200 );
    for I:=1 to STEPS do
      mxLine( Random(320), Random(200)+200, Random(320), Random(200)+200, Random(16), OP_SET );
    (* Circles *)
    mxSetClipRegion( 320, 0, 320, 200 );
    for I:=1 to STEPS do
      mxCircle( Random(320)+320, Random(200), Random(100), Random(16) );
    (* Boxes *)
    mxSetClipRegion( 320, 200, 320, 200 );
    for I:=1 to STEPS do
      mxFillBox( Random(320)+320, Random(200)+200, Random(100)+1, Random(100)+1, Random(16), OP_SET );
    (* Pan *)
    Inc( X, DX );
    Check( X+320 >= 639, X, DX, 319, -1 );
    Check( X < 0, X, DX, 0, +1 );
    Inc( Y, DY );
    Check( Y+200 >= 399, Y, DY, 199, -1 );
    Check( Y < 0, Y, DY, 0, +1 );
    mxPan( X, Y );
    mxWaitRetrace;
  end;

  (* Shutdown *)
  mxSetMode( MX_TEXT );
  mxTerm;
end.
