(*
    DEMO04 - Multiple textures and triple buffering (3 pages)
    (c) 1994 by Alessandro Scotti
*)
uses Crt, Modex, Threed;

const
  MAXVTX = 256;
  MAXCUB = 2;
  MAXTXT = 2;
  Trans : TPoint = ( X:0; Y:0; Z:0 );
  TxtSunDial: array[ 0..7 ] of word = (
    $7F80,$0080, $0080,$0080, $0080,$7E80, $7F80,$7E80 );
  TxtSapphire : array[ 0..7 ] of word = (
    $0080,$0080, $0080,$1F80, $1F80,$1F80, $1F80,$0080 );
  TxtMarble: array[ 0..7 ] of word = (
    $0080,$8080, $0080,$FD80, $7F80,$FD80, $7F80,$8080 );
type
  T2DPoint = record
    X, Y: integer;
  end;
  TTexture = record
    Desc   : array[ 0..3 ] of record X, Y: word end;
    Width  : word;
    Data   : pointer;
  end;
  TQuad = record
    Vtx    : array[ 0..3 ] of word;
    Texture: word;
  end;
  TCube    = record
    Face   : array[ 0..5 ] of TQuad;
    Base   : integer;
  end;
var
  Vtx, XVtx: array[ 0..MAXVTX ] of TPoint;
  VVtx     : array[ 0..MAXVTX ] of T2DPoint;
  Cube     : array[ 0..MAXCUB ] of TCube;
  ZList    : array[ 0..MAXCUB ] of integer;
  VtxCnt   : word;
  Txts     : array[ 0..MAXTXT ] of TTexture;
  Page     : word;
  Palette  : array[ byte ] of record R, G, B: byte; end;
  TxtDat1, TxtDat2: pointer;

(* Add a new entry to the vertex array *)
procedure AddVtx( PX, PY, PZ: longint );
begin
  with Vtx[VtxCnt] do begin X:=PX*$10000; Y:=PY*$10000; Z:=PZ*$10000; end;
  Inc( VtxCnt );
end;

procedure MakeCube( var C: TCube; X1,Y1,Z1, X2,Y2,Z2, TX,TY,TZ, Texture: integer );
const
  FaceIdx: array[ 0..23 ] of integer = (
    0,1,2,3, 0,4,5,1, 1,5,6,2, 2,6,7,3, 3,7,4,0, 6,5,4,7 );
var
  I, VC: integer;
begin
  VC := VtxCnt;
  C.Base := VC;
  AddVtx( X1+TX, Y1+TY, Z1+TZ );
  AddVtx( X2+TX, Y1+TY, Z1+TZ );
  AddVtx( X2+TX, Y2+TY, Z1+TZ );
  AddVtx( X1+TX, Y2+TY, Z1+TZ );
  AddVtx( X1+TX, Y1+TY, Z2+TZ );
  AddVtx( X2+TX, Y1+TY, Z2+TZ );
  AddVtx( X2+TX, Y2+TY, Z2+TZ );
  AddVtx( X1+TX, Y2+TY, Z2+TZ );
  for I:=0 to 23 do C.Face[I shr 2].Vtx[I and 3] := VC+FaceIdx[I];
  for I:=0 to 5 do C.Face[I].Texture := Texture;
end;

procedure MakeTexture( Idx: integer; var VtxData );
var
  P: ^word;
  I: integer;
begin
  P := @VtxData;
  with Txts[Idx] do begin
    for I:=0 to 3 do begin
      Desc[I].X := P^; Inc( P );
      Desc[I].Y := P^; Inc( P );
    end;
    Width := 129;
    Data := TxtDat1;
  end;
end;

procedure Init;
var
  I: integer;
  V: integer;
  F: file;
  P: array[ 1..768 ] of byte;
begin
  (* Initialize objects *)
  VtxCnt := 0;
  MakeCube( Cube[0], -64,-64,8, 64,64,-8, 0,0,0, 1 );   (* Sundial *)
  Cube[0].Face[0].Texture := 0;
  V := VtxCnt;
  MakeCube( Cube[1], -16,-16,16, 16,16,-16, 0,0,0, 2 ); (* Sapphire *)
  tdSetTranslation( Trans );
  tdSetRotation( 32, 32, 00 );
  tdRotate( Vtx[V], XVtx[V], 8 );       (* Got to rotate this cube *)
  for I:=V to V+7 do begin
    Vtx[I].X := XVtx[I].X;
    Vtx[I].Y := XVtx[I].Y;
    Vtx[I].Z := XVtx[I].Z + 100*$10000;
  end;
  MakeCube( Cube[2], -64,-4,48, 64,4,-48, 0,68,56, 1 ); (* Marble *)
  (* Load texture and palette *)
  Assign( F, 'DEMO04.DAT' );
  Reset( F, 1 );
  BlockRead( F, P, SizeOf(P) );
  mxSetPalette( @P, 0, 256 );
  GetMem( TxtDat1, 63*1024 );
  BlockRead( F, TxtDat1^, 129*286 );
  Close( F );
  TxtDat2 := Ptr( Seg(TxtDat1^), Ofs(TxtDat1^)+129*254 );
  (* Init textures *)
  MakeTexture( 0, TxtSundial );
  MakeTexture( 1, TxtMarble );
  MakeTexture( 2, TxtSapphire );
  Txts[2].Data := TxtDat2;
end;

(* Sort procedure, not worth optimizing with only a few objects *)
procedure SortObjects;
var
  I, J, K: integer;
  ZMax: array[ 0..MAXCUB ] of longint;
  ZI: integer;
  L: longint;
begin
  for I:=0 to MAXCUB do begin
    L := XVtx[Cube[I].Base].Z;
    for J:=1 to 7 do
      if( L > XVtx[Cube[I].Base+J].Z ) then L := XVtx[Cube[I].Base+J].Z;
    ZMax[I] := L;
    ZList[I] := I;
  end;
  for I:=0 to MAXCUB-1 do begin
    ZI := I;
    for J:=I+1 to MAXCUB do
      if( ZMax[ZList[J]] > ZMax[ZList[ZI]] ) then ZI := J;
    if( ZI <> I ) then begin
      K := ZList[I];
      ZList[I] := ZList[ZI];
      ZList[ZI] := K;
    end;
  end;
end;

var
  AX, AY, AZ: byte;
  I, J, K: word;
begin
  mxInit;
  mxSetMode( MX_320x240 );
  Init;
  Page := 240;          (* Start with hidden page *)

  (* Init 3D transforms, perspective is intentionally exaggerated *)
  AX := 0; AY := 0; AZ := 0;
  tdSetTranslation( Trans );
  tdSetPerspective( 600*$10000, $10000, $10000 );
  (* Main loop, all magic here! *)
  while( not KeyPressed ) do begin
    tdSetRotation( AX, AY, AZ );                (* Set new angles *)
    tdTransform( Vtx, XVtx, VtxCnt );           (* 3D transform points *)
    tdTransformToImage( XVtx, VVtx, VtxCnt, 160, 120+Page );
    Inc( AX, 1 );                               (* Bump angles *)
    Inc( AY, 2 );
    Inc( AZ, 1 );
    mxSetClipRegion( 0, Page, 320, 240 );       (* Set clip to new page *)
    mxSetClip( TRUE );
    mxFillBox( 0, Page, 320, 240, 0, OP_MOVE ); (* Clear screen *)
    (* Draw objects *)
    SortObjects;
    for I:=0 to MAXCUB do with Cube[ZList[I]] do begin
      for J:=0 to 5 do begin
        K := Face[J].Texture;
        mxTexturePoly( 4, Face[J].Vtx, VVtx, Txts[K].Desc, Txts[K].Data^, Txts[K].Width );
      end;
    end;
    (* Flip page: at 320x240 the Start Address Register Low is always zero *)
    case Page of
      0  : begin PortW[$3D4] := $000C; Page := 240; end;
      240: begin PortW[$3D4] := $4B0C; Page := 480; end;
      480: begin PortW[$3D4] := $960C; Page := 0; end;
    end;
    mxWaitRetrace; (* If the frame rate seems low, try to remove this line *)
  end;

  mxSetMode( MX_TEXT );
  mxTerm;
end.
