/*----------------------------------------------------------------------
;
; XLIB - header file
;
;
;
; ****** XLIB - Mode X graphics library                ****************
; ******                                               ****************
; ****** Written By Themie Gouthas                     ****************
;
; egg@dstos3.dsto.gov.au
; teg@bart.dsto.gov.au
;
; Header file contributed by Darren Lyon (darren@nicky.DIALix.oz.au)
;
;
;  Terminology & notes:
;         VRAM ==   Video RAM
;         SRAM ==   System RAM
;         X coordinates are in pixels unless explicitly stated
;
;----------------------------------------------------------------------*/

#ifndef _XLIB_H_
#define _XLIB_H_

#define BYTE unsigned char
#define WORD unsigned int


#define X_MODE_320x200  0
#define X_MODE_320x240  1
#define X_MODE_360x200  2
#define X_MODE_360x240  3
#define X_MODE_376x282  4
#define X_MODE_320x400  5
#define X_MODE_320x480  6
#define X_MODE_360x400  7
#define X_MODE_360x480  8
#define X_MODE_360x360  9
#define X_MODE_376x308  10
#define X_MODE_376x564  11
#define X_MODE_256x400  12
#define X_MODE_256x480  13


#define BACKWARD 0
#define FORWARD  1

#define X_MODE_INVALID  -1
#define ERROR            1
#define OK               0

/* FUNCTIONS =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

 WORD x_set_mode(
		WORD mode,                /* Initialize x mode           */
		WORD WidthInPixels);

  void x_select_default_plane( /*Enables Read/Write access to a    */
		BYTE plane);   /* a specified plane                */

 void x_set_splitscreen(            /* set the split screen start  */
		WORD line);         /*scan line (initialize)       */

 void x_set_start_addr(
		WORD X,             /* Set the screen start        */
		WORD Y);            /* address in video ram        */

 void x_hide_splitscreen(void); /* Disable the split screen (Split */
				/* Screen RAM remains reserved)    */

 void x_show_splitscreen(void); /* Enable the split screen (given it */
				/* was previously "hidden")          */


 void x_adjust_splitscreen(   /* Adjust the start scan line of the */
		WORD line);   /* split screen                      */


 WORD x_set_doublebuffer(         /* Enable double buffering with a    */
		WORD PageHeight); /* specified page height             */

 WORD x_set_tripplebuffer(         /* Enable tripple buffering with a    */
				WORD PageHeight); /* specified page height             */


 void x_page_flip(        /* Swap visible page (only when double   */
		WORD X,   /* buffering is active) & sets virt start*/
		WORD Y);

 void x_set_cliprect(      /* Define the clipping rectangle */
		WORD left, /* NOTE: left and right are in bytes */
		WORD top,
		WORD righ,
		WORD bottom);

 void x_text_mode(void);   /* return to text mode */
 void x_wait_vsync(void);  /* wait for vsync */

#ifdef __cplusplus
}
#endif


/* VARIABLES =========================================================== */

extern BYTE InGraphics;             /* non zero if in X graphics mode      */
extern WORD CurrXMode;              /* contains current X graphics mode id */
extern WORD ScrnPhysicalByteWidth;  /* Physical screen width in bytes      */
extern WORD ScrnPhysicalPixelWidth; /* Physical screen width in pixels     */
extern WORD ScrnPhysicalHeight;     /* Physical screen height in pixels    */
extern WORD ErrorValue;             /* Error return value                  */
extern WORD SplitScrnOffs;          /* Offset in VRAM  of split screen     */
extern WORD SplitScrnScanLine;      /* Scan line split screen starts at    */
				    /* initially. Resizing the split scrn  */
				    /* using the other functions does not  */
				    /* change this value                   */
extern WORD SplitScrnVisibleHeight; /* Height of the visible portion of the*/
				    /* split screen.                       */
extern WORD Page0_Offs;             /* Offset in VRAM of main virt. screen */
extern WORD Page1_Offs;             /* Offset in VRAM of 2nd virt. screen  */
extern WORD Page2_Offs;             /* Offset in VRAM of 3rd virt. screen  */
extern WORD ScrnLogicalByteWidth;   /* Virtual screen width in bytes       */
extern WORD ScrnLogicalPixelWidth;  /* Virtual screen width in pixels      */
extern WORD ScrnLogicalHeight;      /* Virtual screen height in pixels     */
extern WORD MaxScrollX;             /* Max X position of physical screen   */
				    /*  within virtual screen              */
extern WORD MaxScrollY;             /* Max Y position of physical screen   */
				    /*  within virtual screen              */
extern WORD DoubleBufferActive;     /* Indicates whether double buffering  */
									/* is active                           */
extern WORD TrippleBufferActive;    /* Indicates whether tripple           */
				    /* buffering is active                 */
extern WORD VisiblePageIdx;         /* Index number of visible page 0 or 1 */
extern WORD HiddenPageOffs;         /* Offset of Hidden Pg | only valid    */
extern WORD VisiblePageOffs;        /* Offset of Visible Pg| for D.B. mode */
extern WORD WaitingPageOffs;        /* Offset of Waiting Pg| for T.B. mode */
extern WORD NonVisual_Offs;         /* Offset of first non-visible VRAM    */

extern WORD StartAddressFlag;

extern WORD TopClip;                /* Clipping rectangle                  */
extern WORD BottomClip;
extern WORD LeftClip;
extern WORD RightClip;

extern WORD PhysicalStartPixelX;    /* Coordinates of physical (visible) */
extern WORD PhysicalStartByteX;     /* screen relative to the virtual    */
extern WORD PhysicalStartY;         /* screen's U.L. corner              */

extern char* VsyncPaletteBuffer;

#endif


