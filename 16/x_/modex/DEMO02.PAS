(*
    DEMO02 - Texture mapping and palette rotation
    (c) 1994 by Alessandro Scotti
*)
uses Crt, Modex, Plasma, Threed;

const
  LSIZE = 85;
  Trans : TPoint = ( X:0; Y:0; Z:0 );
type
  T2DPoint = record
    X, Y: integer;
  end;
  TTexture = record
    Desc   : array[ 0..3 ] of T2DPoint;
    Width  : word;
    Data   : array[ 1..64*64 ] of byte;
  end;
  TQuad = record
    VtxCnt : word;
    Vtx    : array[ 0..3 ] of word;
    Texture: word;
  end;
var
  Vtx   : array[ 0..7 ] of TPoint;
  XVtx  : array[ 0..7 ] of TPoint;
  VVtx  : array[ 0..7 ] of T2DPoint;
  Face  : array[ 0..5 ] of TQuad;
  Txts  : array[ 0..5 ] of TTexture;
  Nrm   : array[ 0..5 ] of TPoint;
  XNrm  : array[ 0..5 ] of TPoint;
  Page  : word;
  Palette: array[ byte ] of record R, G, B: byte; end;

(* Make a 64x64 plasma to be used as texture *)
procedure MakeTexture( Idx: word );
var
  I: word;
begin
  mxFillBox( 0, 0, 64, 64, 0, OP_SET );
  MakePlasma( 0, 0, 64, 64, 96, Random(192)+1, Random(192)+1, Random(192)+1 );
  mxGetImage( @Txts[Idx].Data, 0, 0, 64, 64 );
  (* Texture vertexes are 8:8 fixed, add $80 (0.5) for best results *)
  with Txts[Idx] do begin
    Desc[0].X := $80; Desc[0].Y := $80;
    Desc[1].X := $80; Desc[1].Y := $3F80;
    Desc[2].X := $3F80; Desc[2].Y := $3F80;
    Desc[3].X := $3F80; Desc[3].Y := $80;
    Width := 64;
  end;
end;

procedure Init;
var
  I: integer;
begin
  (* Build vertexes for a cube *)
  with Vtx[0] do begin X:=-LSIZE; Y:=-LSIZE; Z:=-LSIZE; end;
  with Vtx[1] do begin X:=+LSIZE; Y:=-LSIZE; Z:=-LSIZE; end;
  with Vtx[2] do begin X:=-LSIZE; Y:=+LSIZE; Z:=-LSIZE; end;
  with Vtx[3] do begin X:=+LSIZE; Y:=+LSIZE; Z:=-LSIZE; end;
  with Vtx[4] do begin X:=-LSIZE; Y:=-LSIZE; Z:=+LSIZE; end;
  with Vtx[5] do begin X:=+LSIZE; Y:=-LSIZE; Z:=+LSIZE; end;
  with Vtx[6] do begin X:=-LSIZE; Y:=+LSIZE; Z:=+LSIZE; end;
  with Vtx[7] do begin X:=+LSIZE; Y:=+LSIZE; Z:=+LSIZE; end;
  for I:=0 to 7 do begin          (* Make points 16:16 fixed *)
    Vtx[I].X := Vtx[I].X*$10000;
    Vtx[I].Y := Vtx[I].Y*$10000;
    Vtx[I].Z := Vtx[I].Z*$10000;
  end;
  (* Build faces *)
  with Face[0] do begin Vtx[0]:=0; Vtx[1]:=2; Vtx[2]:=3; Vtx[3]:=1; end;
  with Face[1] do begin Vtx[0]:=4; Vtx[1]:=5; Vtx[2]:=7; Vtx[3]:=6; end;
  with Face[2] do begin Vtx[0]:=0; Vtx[1]:=1; Vtx[2]:=5; Vtx[3]:=4; end;
  with Face[3] do begin Vtx[0]:=1; Vtx[1]:=3; Vtx[2]:=7; Vtx[3]:=5; end;
  with Face[4] do begin Vtx[0]:=2; Vtx[1]:=0; Vtx[2]:=4; Vtx[3]:=6; end;
  with Face[5] do begin Vtx[0]:=7; Vtx[1]:=3; Vtx[2]:=2; Vtx[3]:=6; end;
  for I:=0 to 5 do Face[I].Texture := I;
  (* Build textures and palette *)
  Randomize;
  FillChar( Palette, SizeOf(Palette), 0 );
  MakePlasmaPalette( Palette, PAL_RGB );
  mxSetPalette( @Palette, 0, 193 );
  for I:=0 to 5 do MakeTexture( I );
end;

var
  AX, AY, AZ: byte;
  I: word;
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
  (* Main loop, all magic here! *)
  while( not KeyPressed ) do begin
    tdSetRotation( AX, AY, AZ );                (* Set new angles *)
    tdTransform( Vtx, XVtx, 8 );                (* 3D transform points *)
    tdTransformToImage( XVtx, VVtx, 8, 160, 120+Page );
    Inc( AX, 1 );                               (* Bump angles *)
    Inc( AY, 1 );
    Inc( AZ, 2 );
    mxSetClipRegion( 0, Page, 320, 240 );       (* Set clip to new page *)
    mxSetClip( TRUE );
    mxFillBox( 0, Page, 320, 240, 0, OP_MOVE ); (* Clear screen *)
    mxRotatePalette( @Palette[1], 192, 3 );     (* Rotate palette *)
    (* Draw cube: backface culling is straighforward in this case, so *)
    (* it can be handled by the polygon filling procedure *)
    for I:=0 to 5 do
      mxTexturePoly( 4, Face[I].Vtx, VVtx, Txts[I].Desc, Txts[I].Data, Txts[I].Width );
    mxStartLine( Page );                        (* Flip pages *)
    mxSetPalette( @Palette[1], 1, 192 );        (* Set new palette *)
    Page := 240-Page;
  end;

  mxSetMode( MX_TEXT );
  mxTerm;
end.
