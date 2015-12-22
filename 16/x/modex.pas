(*
   Turbo Pascal interface to the MODEX library
   Copyright (c) 1993,1994 by Alessandro Scotti
*)
unit ModeX;
interface

const
  (* Video modes *)
  MX_TEXT       = 0;
  MX_320x175    = 1;
  MX_320x200    = 2;
  MX_320x240    = 3;
  MX_320x350    = 4;
  MX_320x400    = 5;
  MX_320x480    = 6;
  MX_360x175    = 7;
  MX_360x200    = 8;
  MX_360x240    = 9;
  MX_360x350    = 10;
  MX_360x400    = 11;
  MX_360x480    = 12;
  MX_400x600    = 13;

  (* Fade effects *)
  MX_FADEIN     = 0;
  MX_FADEOUT    = 1;

  (* Raster ops *)
  OP_SET        = 0;
  OP_AND        = 1;
  OP_OR         = 2;
  OP_XOR        = 3;
  OP_TRANS      = 4;
  OP_ADD        = 5;
  OP_MOVE       = 0;                    (* Alias for OP_SET *)

procedure mxBitBlt( SX, SY: integer; Width, Height: word; DX, DY: integer );
procedure mxCircle( CX, CY: integer; Radius: word; Color: byte );
procedure mxChangeMode( Mode: word );
procedure mxColorToGray( ColorPalette, GrayPalette: pointer; Count: word );
procedure mxFadePalette( Palette: pointer; Cmd, Start, Count, R, G, B: word );
procedure mxFillBox( X, Y: integer; Width, Height: word; Color: byte; Op: word );
procedure mxGammaCorrect( ColorPalette, GammaPalette: pointer; Count: word );
procedure mxGetAspect( var AspectX, AspectY: word );
function  mxGetClipRegion( var X1, Y1, Width, Height: word ): boolean;
function  mxGetClip: boolean;
procedure mxGetImage( Image: pointer; X, Y: integer; Width, Height: word );
procedure mxGetPalette( Palette: pointer; Start, Count: word );
function  mxGetPixel( X, Y: word ): byte;
procedure mxGetScreenSize( var Width, Height: word );
procedure mxGetTextStep( var DeltaX, DeltaY: integer );
function  mxGetVersion: word;
procedure mxGetVirtualScreen( var Width, Height: word );
procedure mxInit;
procedure mxLine( X1, Y1, X2, Y2: integer; Color, Op: word );
procedure mxOutChar( X, Y: integer; C: char );
procedure mxOutText( X, Y: integer; S: pointer );
procedure mxPan( X, Y: word );
procedure mxPutImage( Image: pointer; X, Y: integer; Width, Height, Op: word );
procedure mxPutPixel( X, Y: word; C: byte );
procedure mxPutTile( Tile: pointer; X, Y: integer; Width, Height: word );
procedure mxReadPlane( Plane: byte );
procedure mxRotatePalette( Palette: pointer; Count: word; Step: integer );
procedure mxRowAddress( RowAddress: byte );
function  mxSetClip( Clip: boolean ): boolean;
procedure mxSetClipRegion( X1, Y1, Width, Height: word );
procedure mxSetColor( Index, R, G, B: word );
procedure mxSetFont( Font: pointer; Width, Height: word );
procedure mxSetMode( Mode: word );
procedure mxSetPalette( Palette: pointer; Start, Count: word );
procedure mxSetTextColor( Color, Op: word );
procedure mxSetTextStep( DeltaX, DeltaY: integer );
procedure mxSetVirtualScreen( Width, Height: word );
procedure mxStretchImage( Image: pointer; X, Y: integer; Width, Height, NewWidth, NewHeight, Op: word );
procedure mxSplitScreen( Line: word );
procedure mxStartAddress( StartAddress: word );
procedure mxStartLine( Line: word );
procedure mxTerm;
procedure mxTransPutTile( Tile: pointer; X, Y: integer; Width, Height: word );
procedure mxWaitDisplay;
procedure mxWaitRetrace;
procedure mxWriteMode( Mode: byte );
procedure mxWritePlane( Plane: byte );

procedure mxFillPoly( Count: word; var Map, Points; Color: word );
procedure mxGouraudPoly( Count: word; var Map, Points, Colors; BaseColor: word );
procedure mxTexturePoly( Count: word; var Map, Points, ImgPoints, Texture; Width: word );

procedure mxOutStr( X, Y: integer; S: string );

implementation

procedure mxBitBlt;                                             external;
procedure mxChangeMode( Mode: word );                           external;
procedure mxCircle;                                             external;
procedure mxColorToGray;                                        external;
procedure mxFadePalette;                                        external;
procedure mxFillBox;                                            external;
procedure mxGammaCorrect;                                       external;
procedure mxGetAspect( var AspectX, AspectY: word );            external;
function  mxGetClipRegion;                                      external;
function  mxGetClip: boolean;                                   external;
procedure mxGetImage;                                           external;
procedure mxGetPalette( Palette: pointer; Start, Count: word ); external;
function  mxGetPixel( X, Y: word ): byte;                       external;
procedure mxGetScreenSize( var Width, Height: word );           external;
procedure mxGetTextStep( var DeltaX, DeltaY: integer );         external;
function  mxGetVersion: word;                                   external;
procedure mxGetVirtualScreen( var Width, Height: word );        external;
procedure mxInit;                                               external;
procedure mxLine( X1, Y1, X2, Y2: integer; Color, Op: word );   external;
procedure mxOutChar( X, Y: integer; C: char );                  external;
procedure mxOutText( X, Y: integer; S: pointer );               external;
procedure mxPan( X, Y: word );                                  external;
procedure mxPutImage;                                           external;
procedure mxPutPixel( X, Y: word; C: byte );                    external;
procedure mxPutTile;                                            external;
procedure mxReadPlane( Plane: byte );                           external;
procedure mxRotatePalette;                                      external;
procedure mxRowAddress( RowAddress: byte );                     external;
function  mxSetClip( Clip: boolean ): boolean;                  external;
procedure mxSetClipRegion( X1, Y1, Width, Height: word );       external;
procedure mxSetColor( Index, R, G, B: word );                   external;
procedure mxSetFont( Font: pointer; Width, Height: word );      external;
procedure mxSetMode( Mode: word );                              external;
procedure mxSetPalette( Palette: pointer; Start, Count: word ); external;
procedure mxSetTextColor( Color, Op: word );                    external;
procedure mxSetTextStep( DeltaX, DeltaY: integer );             external;
procedure mxSetVirtualScreen( Width, Height: word );            external;
procedure mxSplitScreen( Line: word );                          external;
procedure mxStartAddress( StartAddress: word );                 external;
procedure mxStartLine;                                          external;
procedure mxStretchImage;                                       external;
procedure mxTerm;                                               external;
procedure mxTransPutTile;                                       external;
procedure mxWaitDisplay;                                        external;
procedure mxWaitRetrace;                                        external;
procedure mxWriteMode( Mode: byte );                            external;
procedure mxWritePlane( Plane: byte );                          external;

procedure mxFillPoly;                                           external;
procedure mxGouraudPoly;                                        external;
procedure mxTexturePoly;                                        external;
{$L MXPB}
{$L MXPF}
{$L MXPG}
{$L MXPT}

{$L MXBB}
{$L MXCC}
{$L MXCG}
{$L MXCL}
{$L MXCR}
{$L MXFB}
{$L MXFP}
{$L MXGI}
{$L MXGM}
{$L MXGP}
{$L MXGV}
{$L MXHL}
{$L MXIT}
{$L MXLN}
{$L MXOT}
{$L MXPI}
{$L MXPN}
{$L MXPP}
{$L MXRA}
{$L MXRP}
{$L MXSA}
{$L MXSC}
{$L MXSI}
{$L MXSL}
{$L MXSM}
{$L MXSP}
{$L MXSS}
{$L MXTL}
{$L MXVS}
{$L MXWD}
{$L MXWM}
{$L MXWP}
{$L MXWR}

(*
   Prints a Turbo Pascal string.
   Note: BP 7.0 supports ASCIIZ strings (PChar type).
*)
procedure mxOutStr;
begin
  S := S + #0;
  mxOutText( X, Y, @S[1] );
end;

end.
