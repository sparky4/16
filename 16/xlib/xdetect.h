/*-----------------------------------------------------------------------
;
; XDETECT - header file
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

#ifndef _XDETECT_H_
#define _XDETECT_H_

#define I8086    0
#define I80186   1
#define I80286   2
#define I80386SX 3
#define I80386DX 4
#define I80486   5

#define NoGraphics 0
#define MDA        1
#define CGA        2
#define EGAMono    3
#define EGAColor   4
#define VGAMono    5
#define VGAColor   6
#define MCGAMono   7
#define MCGAColor  8

#define BUS_MOUSE     1
#define SERIAL_MOUSE  2
#define INPORT_MOUSE  3
#define PS2_MOUSE     4
#define HP_MOUSE      5

/* VARIABLES =========================================================== */

extern WORD MouseButtonCount;  /* number of mouse buttons */
extern WORD MouseVersion;      /* mouse driver version    */
extern WORD MouseType;         /* mouse type              */
extern WORD MouseIRQ;          /* mouse IRQ number        */

/* FUNCTIONS =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

 int x_graphics_card(void);  /* Detect installed Graphics card type */
 int x_processor(void);      /* Detect processor type */
 int x_coprocessor(void);    /* Detect math co-processor type */
 int x_mousedriver(void);    /* Detect math co-processor type */

#ifdef __cplusplus
}
#endif

#endif


