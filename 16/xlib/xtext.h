/*-----------------------------------------------------------------------
;
; XTEXT - header file
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

#ifndef _XTEXT_H_
#define _XTEXT_H_


#define FONT_8x8  0
#define FONT_8x15 1
#define FONT_USER 2

/* FUNCTIONS =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

WORD x_text_init(void);            /* Initialize text functionns  */

void x_set_font(
	 WORD FontId);             /* Set the font style          */

void x_register_userfont(          /* register a user defined font */
	 char far *UserFontPtr);

unsigned int  x_char_put(          /* Draw a text character using  */
	 char ch,                  /* the currently active font    */
	 WORD X,
	 WORD Y,
	 WORD PgOffs,
	 WORD Color);


unsigned int  x_get_char_width(    /* Get the character width      */
	 char ch);


/* the folowing function is from xprintf.c but is included due to its     */
/* close relationship with this module                                    */

void x_printf(                          /* formatted text output */
	WORD x,
	WORD y,
	WORD ScrnOffs,
	WORD color,
	char *ln,...);

void x_bgprintf(                          /* formatted text output */
	WORD x,
	WORD y,
	WORD ScrnOffs,
	WORD fgcolor,
	WORD bgcolor,
	char *ln,...);

#ifdef __cplusplus
}
#endif


/* VARIABLES =========================================================== */

extern BYTE CharHeight;     /* Char height of currently active font        */
extern BYTE CharWidth;      /* Char width of currently active font         */
extern BYTE FirstChar;      /* First char in the curr. active font         */

extern BYTE UserCharHeight; /* Char height of currentle regist'd user font */
extern BYTE UserCharWidth;  /* Char height of currentle regist'd user font */
extern BYTE UserFirstChar;  /* First char of the curr. regist'd usera font */


#endif


