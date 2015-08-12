#define BOOKENDS_C

#include <stdio.h>
#include <fastgraf.h>
#include <alloc.h>
#include <stdlib.h>
#include "roads.h"
#include "fx.h"         /* FOR FADING */
#include "version.h"    /* FOR HEADER */

extern int far *topography;      /* BACKGROUND TILE LIST (ARRAY) */
extern int far *terrain;      /* FOREGROUND TILE LIST (ARRAY) */
extern int viewpage;           /* CURRENTLY VIEWED PAGE */

int startup_vmode=-1;   /* STARTUP VIDEO MODE TO RETURN TO AT PROGRAM END */

/*
 *
 * Initializes/Sets-up the video mode and paging.
 *
 */
void init_video (void)
{
			/* INITIALIZE VIDEO MODE */
    if (fg_testmode (VMODE, VPAGES)==0)
    {
        printf ("Sorry, your video card does not support mode %d.\n", VMODE);
        exit (1);
    }

    startup_vmode=fg_getmode();
    fg_setcolor(0);
    fg_setmode (VMODE);
    fg_erase();

        /* SETUP SPRITE IMAGES */
    fg_setpage (TILEPAGE);
    fg_erase();
    fg_showgif (IMAGES, 0);
    fg_setpage (viewpage);
    fg_erase();
    fg_tcdefine (0,1); /* TREAT COLOR 0 AS TRANSPARENT */
}

/*
 *
 * "Officially" shuts down the program.  Restores video, frees
 * allocated data including fonts, unhooks Sound Blaster,
 * and (optionally) exits with a message and errorcode
 *
 */
void program_shutdown (char *msg, int errcode)
{
    fg_kbinit(0); /* UNLATCH LOW-LEVEL KEYBOARD HANDLER */

        /* FREE DATA */
    if (topography!=NULL) farfree (topography);
    if (terrain!=NULL) farfree (terrain);

        /* RESTORE ORIGINAL VIDEO MODE TO USER */
    if (startup_vmode!=-1)
    {
        fade_out_all ();
        fg_setmode (startup_vmode);
        fg_reset();
    }

        /* FREE FONTS AND SOUND BLASTER SOMEWHERE IN HERE */

    printf (HEADER); /* PRINT HEADER */

		/* REPORT MESSAGE */
    if (*msg!=NULL)
        printf ("ROADS:  %s\n", msg);

        /* QUIT WITH ERROR CODE, IF SUPPLIED ONE */
    if (errcode!=-1)
		exit (errcode);

        /* OTHERWISE, RETURN TO CALLER WHO WILL HANDLE EXITING */
    else return;
}

/*
 *
 * Initialize dynamic data, which is freed with program_shutdown()
 *
 */
void init_data (void)
{
    topography=farcalloc (WORLD_TILES_TOTAL,sizeof (int));
    terrain=farcalloc (WORLD_TILES_TOTAL,sizeof (int));

    if (topography==NULL || terrain==NULL)
       program_shutdown ("Not enough memory -- It's not my fault, I swear!", 1);
}

