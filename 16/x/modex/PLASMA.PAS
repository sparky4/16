unit Plasma;
interface

const
  PAL_RGB       = 0;
  PAL_CLOUDS    = 1;
  PAL_LANDSCAPE = 2;

procedure MakePlasma( X, Y: integer; W, H: word; C1, C2, C3, C4: byte );
procedure MakePlasmaPalette( var Palette; What: word );

implementation uses Modex;

procedure NewColor( XA, YA, X, Y, XB, YB: integer );
var
  Color: longint;
begin
  Color := Abs( XA-XB )+Abs( YA-YB );
  Color := Random( Color shl 1 )-Color;
  Color := (Color+mxGetPixel( XA, YA )+mxGetPixel( XB, YB )+1) shr 1;
  if( Color < 1 ) then Color := 1;
  if( Color > 192 ) then Color := 192;
  if( mxGetPixel( X, Y ) = 0 ) then
    mxPutPixel( X, Y, Lo(Color) );
end;

procedure Divide( X1, Y1, X2, Y2: integer );
var
  X, Y, Color: integer;
begin
  if not( (X2-X1<2)and(Y2-Y1<2) ) then begin
    X := (X1+X2) shr 1;
    Y := (Y1+Y2) shr 1;
    NewColor( X1, Y1, X, Y1, X2, Y1 );
    NewColor( X2, Y1, X2, Y, X2, Y2 );
    NewColor( X1, Y2, X, Y2, X2, Y2 );
    NewColor( X1, Y1, X1, Y, X1, Y2 );
    Color := (mxGetPixel( X1, Y1 )+mxGetPixel( X2, Y1 )+
              mxGetPixel( X2, Y2 )+mxGetPixel( X1, Y2 )) shr 2;
    mxPutPixel( X, Y, Color );
    Divide( X1, Y1, X, Y );
    Divide( X, Y1, X2, Y );
    Divide( X, Y, X2, Y2 );
    Divide( X1, Y, X, Y2 );
  end;
end;

procedure MakePlasma;
begin
  Dec( W );
  Dec( H );
  mxPutPixel( X, Y, C1 );
  mxPutPixel( X, Y+H, C2 );
  mxPutPixel( X+W, Y+H, C3 );
  mxPutPixel( X+W, Y, C4 );
  Divide( X, Y, X+W, Y+H );
end;

procedure MakePlasmaPalette;
type
  TPal = array[ byte ] of record R, G, B: byte end;
var
  I: word;
begin
  FillChar( TPal(Palette)[1], 192*3, 0 );
  case What of
    PAL_CLOUDS:
      for I:=1 to 192 do begin
        TPal(Palette)[I].R := Abs( I-96 )*63 div 96;
        TPal(Palette)[I].G := Abs( I-96 )*63 div 96;
        TPal(Palette)[I].B := 63;
      end;
    PAL_LANDSCAPE:
      begin
        for I:=0 to 31 do begin
          TPal(Palette)[I+1].R := I;
          TPal(Palette)[I+1].G := I;
          TPal(Palette)[I+1].B := I + I shr 1+15;
        end;
        for I:=32 to 63 do begin
          TPal(Palette)[I+1].R := 0;
          TPal(Palette)[I+1].G := I;
          TPal(Palette)[I+1].B := 0;
        end;
        for I:=64 to 191 do begin
          TPal(Palette)[I+1].R := (I-64) div 3 + 15;
          TPal(Palette)[I+1].G := (I-64) div 3 + 15;
          TPal(Palette)[I+1].B := (I-64) div 3 + 15;
        end;
      end;
    else
      for I:=1 to 64 do begin
        TPal(Palette)[I].G := I-1;
        TPal(Palette)[I].B := 64-I;
        TPal(Palette)[I+64].R := I-1;
        TPal(Palette)[I+64].G := 64-I;
        TPal(Palette)[I+128].B := I-1;
        TPal(Palette)[I+128].R := 64-I;
      end;
  end;
end;

end.