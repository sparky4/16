(*
    DEMO03 - Simple star animation, morphs between a cube and a sphere
    (c) 1994 by Alessandro Scotti
*)
uses Crt, Modex, Threed;

const
  MAXVTX = 1000;        (* Number of points *)
  EDGE = 70;            (* Length of cube edge *)
  RADIUS = 90;          (* Radius of sphere *)
  WAITCOUNT = 192;      (* Frames to wait for non-morphing shapes *)
  MS = 32;              (* Number of steps for morphing *)
  Trans : TPoint = ( X:0; Y:0; Z:0 );
  InitMorph1: array[ 0..3 ] of integer = ( 0, MS, 0, 0 );
  InitMorph2: array[ 0..3 ] of integer = ( 0, 0, 0, MS );
  InitDelta1: array[ 0..3 ] of integer = ( 0, -1, 0, +1 );
  InitDelta2: array[ 0..3 ] of integer = ( 0, +1, 0, -1 );
type
  T2DPoint = record
    X, Y: integer;
  end;
  T3DPointArray = array[ 0..MAXVTX-1 ] of TPoint;
  P3DPointArray = ^T3DPointArray;
var
  CubeVtx, SphereVtx, Vtx, XVtx: P3DPointArray;
  VVtx  : array[ 0..MAXVTX-1 ] of T2DPoint;
  Page  : word;
  Status, Count, Delta1, Delta2, Morph1, Morph2: integer;

procedure Swap( var A, B: longint );
var
  L: longint;
begin
  L := A; A := B; B := L;
end;

function Toggle( A: longint ): longint;
begin
  Toggle := A;
  if( Random(2) = 0 ) then Toggle := -A;
end;

procedure Init;
label Retry;
var
  I: integer;
begin
  New( CubeVtx );
  New( SphereVtx );
  New( Vtx );
  New( XVtx );
  (* Build vertexes (yes, I know this piece of code is terrible) *)
  Randomize;
  for I:=0 to MAXVTX-1 do begin
    with CubeVtx^[I] do begin
      (* Build cube *)
      X := (longint(Random(2*EDGE))-EDGE)*$10000;
      Y := (longint(Random(2*EDGE))-EDGE)*$10000;
      Z := Toggle( EDGE*$10000 );
      case Random(3) of
        0: Swap( X, Z );
        1: Swap( Y, Z );
      end;
    end;
    with SphereVtx^[I] do begin
      (* Build sphere *)
Retry:
      X := (longint(Random(2*RADIUS))-RADIUS);
      Y := (longint(Random(2*RADIUS))-RADIUS);
      if( X*X+Y*Y > RADIUS*RADIUS ) then goto Retry;
      Z := Toggle( Round( Sqrt( Abs( RADIUS*RADIUS-X*X-Y*Y ) ) ) );
      case Random(3) of
        0: Swap( X, Z );
        1: Swap( Y, Z );
      end;
      X := X * $10000; Y := Y * $10000; Z := Z * $10000;
    end;
  end;
  (* Initialize morphing *)
  Move( CubeVtx^, Vtx^, SizeOf(Vtx^) );
  Status := 0;
  Count := WAITCOUNT;
end;

procedure Morph;
var
  I: integer;
begin
  (* Fully unoptimized, slowest loop I could think of! *)
  for I:=0 to MAXVTX-1 do begin
    Vtx^[I].X := ((CubeVtx^[I].X * Morph1)+(SphereVtx^[I].X * Morph2)) div MS;
    Vtx^[I].Y := ((CubeVtx^[I].Y * Morph1)+(SphereVtx^[I].Y * Morph2)) div MS;
    Vtx^[I].Z := ((CubeVtx^[I].Z * Morph1)+(SphereVtx^[I].Z * Morph2)) div MS;
  end;
end;

var
  AX, AY, AZ: byte;
  I: word;
  C: char;
begin
  mxInit;
  mxSetMode( MX_320x240 );
  Init;
  Page := 240;          (* Start with hidden page *)

  AX := 0;
  AY := 0;
  AZ := 0;
  (* Init 3D transforms, perspective is intentionally exaggerated *)
  tdSetTranslation( Trans );
  tdSetPerspective( 400*$10000, $10000, $10000 );
  C := #0;
  repeat
    tdSetRotation( AX, AY, AZ );                (* Set new angles *)
    tdTransform( Vtx^, XVtx^, MAXVTX );         (* 3D transform points *)
    tdTransformToImage( XVtx^, VVtx, MAXVTX, 160, 120+Page );
    Inc( AX, 1 );                               (* Bump angles *)
    Inc( AY, 1 );
    Inc( AZ, 2 );
    mxSetClipRegion( 0, Page, 320, 240 );       (* Set clip to new page *)
    mxSetClip( TRUE );
    mxFillBox( 0, Page, 320, 240, 0, OP_MOVE ); (* Clear screen *)
    (* Draw points *)
    for I:=0 to MAXVTX-1 do
      mxPutPixel( VVtx[I].X, VVtx[I].Y, 128 + XVtx^[I].Z shr 18 );
    mxStartLine( Page );                        (* Flip pages *)
    Page := 240-Page;
    (* Morph *)
    if( Odd(Status) ) then begin
      Morph;
      Inc( Morph1, Delta1 );
      Inc( Morph2, Delta2 );
      if( Morph1 < 0 )or( Morph2 < 0 ) then Inc( Status );
      if( Status = 4 ) then Status := 0;
    end
    else begin
      Dec( Count );
      if( Count < 0 ) then begin
        Inc( Status );
        Count :=  WAITCOUNT;
        Morph1 := InitMorph1[Status];
        Morph2 := InitMorph2[Status];
        Delta1 := InitDelta1[Status];
        Delta2 := InitDelta2[Status];
      end;
    end;
  until( KeyPressed );

  mxSetMode( MX_TEXT );
  mxTerm;
end.
