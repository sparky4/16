#ifndef __MEM_H
#include <mem.h>
#endif

/* DEFINITIONS */
#define VCOLORS     256         /* Number of screen colors */

/* GLOBAL VARIABLES */
#ifdef FX_C
char default_palette[VCOLORS*3]; /* Stores the palette before fades */
char empty_palette[VCOLORS*3];   /* Stores an empty palette for quick clearing */
char abortfadeonkeyhit=0;        /* Quit fading on keyhit? */
char animatewhilefading=1;       /* Animate screen while performing fade? */
#else
extern char default_palette[VCOLORS*3];
extern char empty_palette[VCOLORS*3];
extern char abortfadeonkeyhit;
extern char animatewhilefading;
#endif

/* PROTOTYPES */
void fade_in (int DACstart, int DACend);
void fade_out (int DACstart, int DACend);

/* MACROS */
#define fade_init()     fg_getdacs (0, VCOLORS, default_palette); /* COPY PALETTE */
#define fade_blackout() memset (empty_palette, 0, VCOLORS*3);\
                        fg_setdacs(0, VCOLORS, empty_palette);    /* SET DACS TO ZERO */

#define fade_out_all() fade_out(0,255)
#define fade_in_all()  fade_in(0,255)

