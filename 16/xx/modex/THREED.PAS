unit ThreeD;
interface

type
  TVector       = record
    X, Y, Z     : longint;
  end;
  TPoint        = TVector;

function tdFixedMul( F1, F2: longint ): longint;
function tdGetSurfaceLight( var Normal: TPoint ): integer;
function tdTransformToImage( var Source, Dest; Count, DeltaX, DeltaY: integer ): longint;

procedure tdBackPlaneCull( var Poly, Vertex, Dest; Count, Step: word );
procedure tdGetNormal( var Dest, P1, P2, P3: TVector );
procedure tdRotate( var Source, Dest; Count: word );
procedure tdSetLight( var Light: TVector );
procedure tdSetRotation( RX, RY, RZ: word );
procedure tdSetTranslation( var TV: TVector );
procedure tdSetPerspective( PD, XF, YF: longint );
procedure tdTransform( var Source, Dest; Count: word );
procedure tdTransformLight( var Source, Dest; Count: word );

implementation

function tdGetSurfaceLight;                             external;
procedure tdSetRotation( RX, RY, RZ: word );            external;
procedure tdGetNormal;                                  external;
procedure tdSetTranslation( var TV: TVector );          external;
procedure tdTransform( var Source, Dest; Count: word ); external;
procedure tdRotate;                                     external;
function  tdTransformToImage;                           external;
procedure tdSetLight( var Light: TVector );             external;
procedure tdSetPerspective;                             external;
procedure tdTransformLight;                             external;
function tdFixedMul( F1, F2: longint ): longint;        external;
procedure tdBackPlaneCull;                              external;
{$L THREED}

end.
