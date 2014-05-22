/*
    MODEX.H - C/C++ include file for the MODEX library
    Copyright (c) 1994 Alessandro Scotti
*/

#ifndef _MODEX_H_               // Avoid nested inclusions
#define _MODEX_H_

//
// Video modes
//
#define MX_TEXT     0           // 80x25 text
#define MX_320x175  1           // 320x175x256
#define MX_320x200  2           // 320x200x256, 4 pages, aspect 6:5
#define MX_320x240  3           // 320x240x256, 3 pages, aspect 1:1
#define MX_320x350  4           // 320x350x256
#define MX_320x400  5           // 320x400x256, 2 pages
#define MX_320x480  6           // 320x480x256, 1 page
#define MX_360x175  7           // 360x175x256
#define MX_360x200  8           // 360x200x256, 3 pages
#define MX_360x240  9           // 360x240x256, 2 pages
#define MX_360x350  10          // 360x350x256
#define MX_360x400  11          // 360x400x256, 1 page
#define MX_360x480  12          // 360x480x256, 1 page
#define MX_400x600  13          // 400x600x256, 1 page

//
// Fade effects
//
#define MX_FADEIN   0
#define MX_FADEOUT  1

//
// Raster ops
//
#define OP_SET      0           // No operator
#define OP_AND      1           // And
#define OP_OR       2           // Or
#define OP_XOR      3           // Xor
#define OP_TRANS    4           // Transparent
#define OP_ADD      5           // Additive
#define OP_MOVE     0           // Alias for OP_SET

//
// Temporary definitions
//
#define MXBYTE      unsigned char
#define MXBOOL      short int
#define MXSINT      short int
#define MXUINT      unsigned short int
#define MXAPI       //far pascal
#define MXPTR       void far *

// Functions

#ifdef __cplusplus                      // Avoid C++ name mangling
extern "C" {
#endif

//
// Initialization
//
MXSINT  MXAPI mxInit( void );             // Returns 0 if successful
void    MXAPI mxTerm( void );
MXUINT  MXAPI mxGetVersion( void );
//
// Mode setting
//
void    MXAPI mxChangeMode( MXUINT mode );
void    MXAPI mxSetMode( MXUINT mode );
void    MXAPI mxGetAspect( MXUINT far *aspectx, MXUINT far *aspecty );
void    MXAPI mxGetScreenSize( MXUINT far *width, MXUINT far *height );
//
// Hardware support
//
void    MXAPI mxWriteMode( MXBYTE wm );
void    MXAPI mxSplitScreen( MXUINT line );
void    MXAPI mxStartAddress( MXUINT sa );
void    MXAPI mxStartLine( MXUINT sl );
void    MXAPI mxWaitDisplay( void );
void    MXAPI mxWaitRetrace( void );
void    MXAPI mxWritePlane( MXBYTE wp );
void    MXAPI mxReadPlane( MXBYTE rp );
void    MXAPI mxRowAddress( MXBYTE ra );
//
// Virtual screen
//
void    MXAPI mxGetVirtualScreen( MXUINT far *width, MXUINT far *height );
void    MXAPI mxSetVirtualScreen( MXUINT width, MXUINT height );
void    MXAPI mxPan( MXUINT x, MXUINT y );
//
// Clipping
//
MXBOOL  MXAPI mxGetClip( void );
MXBOOL  MXAPI mxGetClipRegion( MXSINT far *x, MXSINT far *y, MXSINT far *w, MXSINT far *h );
MXBOOL  MXAPI mxSetClip( MXBOOL );
void    MXAPI mxSetClipRegion( MXUINT x, MXUINT y, MXUINT width, MXUINT height );
//
// Graphics
//
void    MXAPI mxBitBlt( MXSINT sx, MXSINT sy, MXUINT width, MXUINT height, MXSINT dx, MXSINT dy );
void    MXAPI mxFillBox( MXSINT x, MXSINT y, MXUINT width, MXUINT height, MXUINT color, MXUINT op );
MXBYTE  MXAPI mxGetPixel( MXSINT x, MXSINT y );
void    MXAPI mxPutPixel( MXSINT x, MXSINT y, MXBYTE color );
void    MXAPI mxLine( MXSINT x1, MXSINT y1, MXSINT x2, MXSINT y2, MXUINT color, MXUINT op );
void    MXAPI mxGetImage( MXPTR img, MXSINT x, MXSINT y, MXUINT width, MXUINT height );
void    MXAPI mxPutImage( MXPTR img, MXSINT x, MXSINT y, MXUINT w, MXUINT h, MXUINT op );
void    MXAPI mxPutTile( MXPTR tile, MXSINT x, MXSINT y, MXUINT width, MXUINT height );
void    MXAPI mxTransPutTile( MXPTR tile, MXSINT x, MXSINT y, MXUINT w, MXUINT h );
void    MXAPI mxCircle( MXSINT x, MXSINT y, MXUINT radius, MXBYTE color );
void    MXAPI mxStretchImage( MXPTR img, MXSINT x, MXSINT y, MXUINT w, MXUINT h, MXUINT neww, MXUINT newh, MXUINT op );
//
// Palette
//
void    MXAPI mxColorToGray( MXPTR source, MXPTR dest, MXUINT count );
void    MXAPI mxGammaCorrect( MXPTR source, MXPTR dest, MXUINT count );
void    MXAPI mxGetColor( MXUINT index, MXSINT far *r, MXSINT far *g, MXSINT far *b );
void    MXAPI mxSetColor( MXUINT index, MXSINT red, MXSINT green, MXSINT blue );
void    MXAPI mxGetPalette( MXPTR palette, MXUINT index, MXUINT count );
void    MXAPI mxSetPalette( MXPTR palette, MXUINT index, MXUINT count );
void    MXAPI mxFadePalette( MXPTR, MXUINT, MXUINT, MXUINT, MXUINT, MXUINT, MXUINT );
void    MXAPI mxRotatePalette( MXPTR palette, MXUINT count, MXSINT step );
//
// Text
//
MXSINT  MXAPI mxSetFont( MXPTR font, MXUINT charwidth, MXUINT charheight );
void    MXAPI mxSetTextColor( MXUINT color, MXUINT op );
void    MXAPI mxGetTextStep( MXSINT far *deltax, MXSINT far *deltay );
void    MXAPI mxSetTextStep( MXSINT deltax, MXSINT deltay );
void    MXAPI mxOutChar( MXSINT x, MXSINT y, char c );
void    MXAPI mxOutText( MXSINT x, MXSINT y, char far *sz );
//
// Convex polygons
//
void    MXAPI mxFillPoly( MXUINT, MXPTR, MXPTR, MXUINT, MXUINT );
void    MXAPI mxGouraudPoly( MXUINT, MXPTR, MXPTR, MXPTR, MXUINT, MXUINT );
void    MXAPI mxTexturePoly( MXUINT, MXPTR, MXPTR, MXPTR, MXPTR, MXUINT, MXUINT );

#ifdef __cplusplus
}
#endif

//
// Remove temporary defines
//
#undef  MXBYTE
#undef  MXBOOL
#undef  MXSINT
#undef  MXUINT
#undef  MXPTR
#undef  MXAPI

#endif  // _MODEX_H_
