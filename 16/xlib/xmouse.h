/*-----------------------------------------------------------------------
;
; XMOUSE - header file
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
;  Terminology & notes:
;         VRAM ==   Video RAM
;         SRAM ==   System RAM
;         X coordinates are in pixels unless explicitly stated
;
;-----------------------------------------------------------------------*/

#ifndef _XMOUSE_H_
#define _XMOUSE_H_

#define LEFT_PRESSED  1
#define RIGHT_PRESSED 2

/* GLOBAL VARS ========================================================= */

extern WORD MouseInstalled;   /* Indicates whether mouse handler installed */
extern WORD MouseHidden;      /* Indicates whether mouse cursor is hidden  */
extern WORD MouseButtonStatus;/* Holds the mouse button status             */
extern WORD MouseX;           /* Current X position of mouse cursor        */
extern WORD MouseY;           /* Current Y position of mouse cursor        */
extern BYTE MouseFrozen;      /* Disallows position updates if TRUE        */
extern BYTE MouseColor;       /* The mouse cursors colour                  */

/* FUNCTIONS =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif



void  x_mouse_init(void);       /* Initialize mouse cursor            */

void  x_define_mouse_cursor(    /* Define and set a cursor shape      */
	   char far MouseDef,
	   int color);

void  x_update_mouse(void);     /* Force mouse cursor position update */

void  x_show_mouse(void);       /* Show mouse cursor if hidden        */

void  x_hide_mouse(void);       /* Hide mouse cursor if visible       */

void  x_mouse_remove(void);     /* Remove installed mouse handler     */

void  x_position_mouse(         /* Set the mouse position             */
	   int x,
	   int y);

void  x_put_cursor(             /* Draw the mouse cursor (NOT FOR     */
	   int x,               /* general use)                       */
	   int y,
	   int topclip,
	   int botclip,
	   WORD ScrnOff);


void x_mouse_window(int x0,     /* Define a mouse window */
                    int y0,
                    int x1,
                    int y1);

#ifdef __cplusplus
}
#endif

#endif
