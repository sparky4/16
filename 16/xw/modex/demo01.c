/*
    DEMO01 - Sprites, page flipping and palette rotation
    Copyright (c) 1994 Alessandro Scotti
*/
uses Crt, Modex;

#DEFINE MAX_SPRITE 100
type
  (* Sprite structure *)
  TSprite = record
    X, Y : integer;                        (* Sprite coordinates *)
    DX,DY: integer;                        (* Deltas for sprite movement *)
    W, H : integer;                        (* Sprite width and height *)
    Image: array[ 1..16, 1..16 ] of byte;  (* Sprite image data *)
  end;
  (* RGB color structure *)
  TRgb    = record
    R, G, B: byte;
  end;
var
  S      : array[ 1..MAX_SPRITE ] of TSprite;   (* An array of sprites *)
  Palette: array[ byte ] of TRgb;               (* Palette *)
  Page   : word;                                (* Page offset *)
  I      : word;

(* Initializes a sprite structure *)
procedure sxInit( var S: TSprite );
var
  I: word;
begin
  S.X := Random( 320 );  (* Initialize position with random values *)
  S.Y := Random( 240 );
  S.DX := Random( 7 )-3; (* Initialize speed with random values *)
  S.DY := Random( 7 )-3;
  S.W := 16;             (* Size is fixed in this program *)
  S.H := 16;
  (* The image is a square with a hole inside *)
  FillChar( S.Image, SizeOf(S.Image), Random(15)+1 );
  for I:=5 to 12 do FillChar( S.Image[ I, 5 ], 8, 0 );
end;

(* Moves a sprite *)
procedure sxMove( var S: TSprite );
begin
  Inc( S.X, S.DX );     (* Get new position *)
  Inc( S.Y, S.DY );
  (* Check sprite position, change delta if needed *)
  if( S.X > 320 ) then begin
    S.X := 320;
    S.DX := -S.DX;
  end;
  if( S.X < -16 ) then begin
    S.X := -16;
    S.DX := -S.DX;
  end;
  if( S.Y > 240 ) then begin
    S.Y := 240;
    S.DY := -S.DY;
  end;
  if( S.Y < -16 ) then begin
    S.Y := -16;
    S.DY := -S.DY;
  end;
  (* Draw the sprite, note the Page offset added to the *)
  (* Y coordinate of the image *)
  mxPutImage( @S.Image, S.X, Page+S.Y, S.W, S.H, OP_TRANS );
end;

begin
  (* Initialize library *)
  mxInit;

  (* Enter graphics mode *)
  mxSetMode( MX_320x240 );

  (* Print initialization message *)
  mxSetTextColor( 15, OP_TRANS );
  mxOutStr( 4, 4, 'Initializing...' );

  (* Initialize sprites *)
  for I:=1 to MAX_SPRITE do sxInit( S[I] );

  (* Draw background *)
  for I:=1 to 192 do begin
    mxCircle( 160, 480+120, I, I+63 );
    mxCircle( 161, 480+120, I, I+63 );
  end;

  (* Compute and set palette *)
  for I:=1 to 192 do with Palette[I+63] do begin
    R := 0;
    G := 0;
    B := 0;
    if( I < 64 ) then
      R := I shr 1+31
    else if( I < 128 ) then
      G := (I-64) shr 1+31
    else
      B := (I-128) shr 1+31;
  end;
  mxSetPalette( @Palette[64], 64, 192 );

  (* Main loop *)
  Page := 240;
  while( not KeyPressed ) do begin
    (* Set clip region to current page *)
    mxSetClipRegion( 0, Page, 320, 240 );
    mxSetClip( TRUE );
    (* Restore background *)
    mxBitBlt( 0, 480, 320, 240, 0, Page );
    (* Draw sprites *)
    for I:=1 to MAX_SPRITE do sxMove( S[I] );
    (* Print message *)
    mxOutStr( 4, Page+4, 'Some sprites moving...' );
    (* Flip page *)
    mxStartLine( Page );
    Page := 240-Page;
    (* Animate palette *)
    mxSetPalette( @Palette[64], 64, 192 );
    mxRotatePalette( @Palette[64], 192, 3 );
  end;

  mxSetMode( MX_TEXT );
  mxTerm;
end.
