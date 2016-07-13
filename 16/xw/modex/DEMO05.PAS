(*
   DEMO05 - A Gouraud-shaded rotating torus
   (c) 1994 Alessandro Scotti
*)
uses Crt, Modex, Threed;

(* Define ALTPAL for alternate palette *)
{$define ALTPAL}

const
  MAXVTX1 = 15; RADIUS1 = 70;   (* MAXVTX1+1 must be multiple of 4 *)
  MAXVTX2 = 15; RADIUS2 = 30;
  MAXVTX  = (MAXVTX1+1)*(MAXVTX2+1)-1;
  MAXFACE = MAXVTX;
  Trans : TPoint = ( X:0; Y:0; Z:0 );           (* Object translation *)
  Light : TPoint = ( X:0; Y:0; Z:-63*$10000 );  (* Light direction *)
type
  TQuad   = record
    QVtx  : array[ 0..3 ] of integer;
  end;
var
  Vtx, XVtx : array[ 0..MAXVTX ] of TPoint;     (* Points *)
  VVtx      : array[ 0..MAXVTX ] of record X, Y: integer end;
  Face      : array[ 0..MAXFACE ] of TQuad;     (* Polys *)
  Culled    : array[ 0..MAXFACE ] of integer;
  GNrm,XGNrm: array[ 0..MAXVTX ] of TVector;    (* Gouraud normals *)
  VtxLight  : array[ 0..MAXVTX ] of integer;    (* Points brightness *)
  Page      : word;

function GetVtx( I1, I2: integer ): integer;
begin
  GetVtx := (I1 mod (MAXVTX1+1))*(MAXVTX2+1) + I2 mod (MAXVTX2+1);
end;

procedure Init;
var
  R, N, X, Y, Z: real;
  I, J, K, V: integer;
begin
  (* Build vertexes *)
  for I:=0 to MAXVTX1 do begin
    K := (I + (MAXVTX1+1) shr 2) mod (MAXVTX1+1);
    R := RADIUS1 + RADIUS2*Cos( 2*K*Pi / (MAXVTX1+1) );
    for J:=0 to MAXVTX2 do begin
      V := I*(MAXVTX2+1)+J;                     (* Index of current vertex *)
      (* Compute coordinates of current vertex *)
      X := R*Cos(2*J*Pi / (MAXVTX2+1));         (* Get coordinates *)
      Y := R*Sin(2*J*Pi / (MAXVTX2+1));
      Z := RADIUS2*Sin(2*K*Pi / (MAXVTX1+1));
      Vtx[V].X := Round( X )*$10000;            (* Save coordinates *)
      Vtx[V].Y := Round( Y )*$10000;
      Vtx[V].Z := Round( Z )*$10000;
      (* Compute direction of Gouraud normal thru current vertex *)
      X := X - RADIUS1*Cos(2*J*Pi / (MAXVTX2+1));
      Y := Y - RADIUS1*Sin(2*J*Pi / (MAXVTX2+1));
      N := Sqrt( X*X + Y*Y + Z*Z );             (* Get vector length *)
      GNrm[V].X := Trunc( X*$10000/N );         (* Save normal vector *)
      GNrm[V].Y := Trunc( Y*$10000/N );
      GNrm[V].Z := Trunc( Z*$10000/N );
    end;
  end;
  (* Generate faces so that depth-sorting is not needed: there are still *)
  (* some *very* little errors, but this is the best I could devise *)
  J := 0;
  K := 0;
  for I:=0 to MAXFACE do with Face[I] do begin
    QVtx[0] := GetVtx( J, K );
    QVtx[1] := GetVtx( J, K+1 );
    QVtx[2] := GetVtx( J+1, K+1 );
    QVtx[3] := GetVtx( J+1, K );
    Inc( K );
    if( K > MAXVTX2 ) then begin
      K := 0;
      Inc( J );
    end;
  end;
{$ifndef ALTPAL}
  for I:=0 to 63 do mxSetColor( I+64, 0, 0, I );     (* Blue palette *)
{$else}
  for I:=0 to 31 do mxSetColor(I+64, 0, I shl 1, 0); (* Green neon palette *)
  for I:=32 to 63 do mxSetColor ( I+64, (I-32) shl 1, 63, (I-32) shl 1 );
{$endif}
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
  tdSetLight( Light );
  tdSetPerspective( 400*$10000, $10000, $10000 );
  (* Main loop, all magic here! *)
  while( not KeyPressed ) do begin
    tdSetRotation( AX, AY, AZ );                (* Set new angles *)
    tdTransform( Vtx, XVtx, MAXVTX+1 );         (* 3D transform points *)
    tdTransformToImage( XVtx, VVtx, MAXVTX+1, 160, 120+Page );
    tdRotate( GNrm, XGNrm, MAXVTX+1 );          (* Rotate Gouraud normals *)
    tdTransformLight( XGNrm, VtxLight, MAXVTX+1 );
    (* Backplane culling is not really needed here! *)
    FillChar( Culled, SizeOf(Culled), 0 );
    tdBackPlaneCull( Face, XVtx, Culled, MAXFACE+1, SizeOf(TQuad) );
    Inc( AX, 1 );                               (* Bump angles *)
    Inc( AY, 2 );
    Inc( AZ, 3 );
    mxSetClipRegion( 0, Page, 320, 240 );       (* Set clip to new page *)
    mxSetClip( TRUE );
    mxFillBox( 0, Page, 320, 240, 0, OP_MOVE ); (* Clear screen *)
    (* Draw polygons *)
    for I:=0 to MAXFACE do with Face[I] do
      if( Culled[I] >= 0 ) then mxGouraudPoly( 4, QVtx, VVtx, VtxLight, 64 );
    (* Flip page: at 320x240 the Start Address Register Low is always zero *)
    case Page of
      0  : begin PortW[$3D4] := $000C; Page := 240; end;
      240: begin PortW[$3D4] := $4B0C; Page := 480; end;
      480: begin PortW[$3D4] := $960C; Page := 0; end;
    end;
    mxWaitRetrace; (* Uncomment this instruction if screen flickers *)
  end;

  mxSetMode( MX_TEXT );
  mxTerm;
end.
