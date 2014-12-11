#define ROADS_C

#include <fastgraf.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h> /* FOR RANDOM */
#include "roads.h"
#include "tiles.h"    /* DUE TO R AND C CHEATS        */
#include "fx.h"       /* FOR FADING STUFF             */
#include "version.h"  /* INFO ON THIS VERSION         */
#include "keys.h"     /* KEY AND SCANCODE DEFINITIONS */

extern int far *topography;      /* BACKGROUND TILE LIST (ARRAY) */
extern int far *terrain;      /* FOREGROUND TILE LIST (ARRAY) */
extern int view_x, view_y;     /* VIEW AREA (UPPER LEFT CORNER) */
extern int viewpage;           /* CURRENTLY VIEWED PAGE */
extern int world_type;         /* TENDENCY TO GRASS */

extern int edgemode;           /* BLOCKY GRASS/DIRT OR EDGED? */
extern int animatemode;
extern int frogmode;
extern int frogwatchmode;

int keyboardmode=0;

/* PROTOTYPES FOR INTERNAL FUNCTIONS */
void time_test (void);
void cheat (int type);
void toggle_mode (int type);
void make_world (int type);
void view_tile_page (void);
void move_view (void);
int keycheck (void);
void init_all (void);
void gogofrog (void);

#pragma argsused
void main (int argc, char *argv[])
{
    char quitting_time=0;       /* QUIT PROGRAM LOOP   */

    printf (HEADER);

    if (argc>1)
    {
        printf (KEY_HELP);
        exit (2);
    }

    printf ("Loading ... [Escape quits] ... [Type ROADS /? for more keys!]\n");
    init_all(); /* INITIALIZE ALL SYSTEMS */

    while (!quitting_time) /* LOOP FOREVER */
    {
        quitting_time=keycheck(); /* CHECK FOR REGULAR KEYS */
        if (animatemode) animate(); /* PERFORM ALL ANIMATIONS */
    }

    program_shutdown("Thank you for running ROADS!", 0);
}

#define TIMETEST_LENGTH 10 /* TIME TEST LENGTH IN SECONDS */

/*
 *
 * Performs time testing to try and guess a FPS.
 *
 */
void time_test (void)
{
    int x, dir;
    long end_time;
    int frames_shown[2];

    for (x=0; x<2; x++) /* TEST TWICE, ONCE WITH ANIMATION */
    {
        while (redraw(SCROLL_UL)); /* SCROLL UPPER LEFT TO START */
        fg_music ("L64FAC.AE.B$");
        frames_shown[x]=0; dir=0;
        end_time=TIMETEST_LENGTH*182/10;
        end_time+=fg_getclock();

        while (fg_getclock()<end_time)
        {
            frames_shown[x]++;

            switch (dir)
            {
                case 0: if (!redraw (SCROLL_DR   )) dir++; break;
                case 1: if (!redraw (SCROLL_UP   )) dir++; break;
                case 2: if (!redraw (SCROLL_DL   )) dir++; break;
                case 3: if (!redraw (SCROLL_RIGHT)) dir++; break;
                case 4: if (!redraw (SCROLL_UL   )) dir++; break;
                case 5: if (!redraw (SCROLL_DOWN )) dir++; break;
                case 6: if (!redraw (SCROLL_UR   )) dir++; break;
                case 7: if (!redraw (SCROLL_LEFT )) dir=0; break;
            }

            if (x==0) animate(); /* ANIMATION ON FIRST TEST ONLY */
        }
    }

    program_shutdown ("",-1); /* DON'T EXIT YET */
    printf ("%d Frames in %i seconds (%i FPS) with animation\n",
        frames_shown[0], TIMETEST_LENGTH, frames_shown[0]/TIMETEST_LENGTH);
    printf ("%d Frames in %i seconds (%i FPS) without animation\n",
        frames_shown[1], TIMETEST_LENGTH, frames_shown[1]/TIMETEST_LENGTH);
    exit (0);
}

/*
 *
 * Turns on cheats (fills screen with anims for testing)
 *
 */
void cheat (int type)
{
    register int x;

    if (type==0)
    {
        fade_out_all();
        for (x=0; x<WORLD_TILES_TOTAL; x++)
            terrain[x]=ANM_FIRE;
        redraw(REFRESH);
        animatewhilefading=0;
        fade_in_all();
        animatewhilefading=1;
    }

    else if (type==1)
    {
        fade_out_all();
        for (x=0; x<WORLD_TILES_TOTAL; x++)
            if (!isroad(terrain[x])) terrain[x]=ANM_FIRE;
        redraw(REFRESH);
        animatewhilefading=0; /* DON'T ANIMATE IN FADES -- TOO MANY ANIMS! */
        fade_in_all();
        animatewhilefading=1;
    }
}

/*
 *
 * Toggles modes on and off (animation, edging, etc.)
 *
 */
void toggle_mode (int type)
{
    if (type==0)
    {
        animatemode=!animatemode;
        fg_music ("L64G.AG.A$");
    }

    else if (type==1)
    {
        edgemode=!edgemode;
        fg_music ("S1L20B..G..F..$");
        if (edgemode)
        {
            add_dirt_edges();
            redraw(REFRESH);
        }
    }
    else if (type==2)
    {
        keyboardmode=!keyboardmode;
        fg_music ("L40BABAGFG.$");
    }
    else if (type==3)
    {
        frogwatchmode=!frogwatchmode;
        fg_music ("O1L30D.ED.A$");
    }
}

/*
 *
 * Initializes the world, foreground and/or background.
 *
 */
void make_world (int type)
{
    if (type==0)
    {
        fade_out_all();
        init_world();
        redraw(REFRESH);
        fade_in_all();
    }

    else if (type==1)
    {
        init_background();
        redraw(REFRESH);
    }

    else if (type==2)
    {
        init_foreground();
        redraw(REFRESH);
    }
}

/*
 *
 * Shows the contents of the tile page.  Hit any key to fade back.
 *
 */
void view_tile_page (void)
{
    char key1, key2;

    fade_out_all();
    fg_setvpage (TILEPAGE);
    fade_in_all();

    fg_kbinit(0);

    do {
      fg_intkey (&key1, &key2);
      animate();
    } while (!(key1+key2)); /* DO LOOP WHILE KEYS ARE NOT HIT */

    fg_kbinit(1);

    fade_out_all();
    fg_setvpage (viewpage);
    fade_in_all();
}

/*
 *
 * Scans for arrow keys and scrolls the view area in reaction to them.
 *
 */
void move_view (void)
{
    static char left=0, right=0, up=0, down=0; /* KEYBOARD VARS       */

        /* CHECK FOR ARROWS BEING PRESSED */
    if (fg_kbtest(SCAN_RIGHT))      right++; else right=0;
    if (fg_kbtest(SCAN_LEFT))       left++;  else left=0;
    if (fg_kbtest(SCAN_UP))         up++;    else up=0;
    if (fg_kbtest(SCAN_DOWN))       down++;  else down=0;

        /* MAKE SURE COUNTERS DON'T GO TOO HIGH */
    if (right>100)  right=100;
    if (left>100)   left=100;
    if (up>100)     up=100;
    if (down>100)   down=100;

        /* IF "TAP" KEYBOARD MODE IS ON, DON'T MOVE UNTIL KEYS RELEASED */
    if (keyboardmode && (right>1 || left>1 || up>1 || down>1)) return;

        /* MOVE, CHECKING FOR DIAGONAL MOVEMENT FIRST */
    if (up && right)        redraw (SCROLL_UR);
    else if (down && left)  redraw (SCROLL_DL);
    else if (up && left)    redraw (SCROLL_UL);
    else if (down && right) redraw (SCROLL_DR);
    else if (right)         redraw (SCROLL_RIGHT);
    else if (left)          redraw (SCROLL_LEFT);
    else if (up)            redraw (SCROLL_UP);
    else if (down)          redraw (SCROLL_DOWN);
}

/*
 *
 * Initializes all systems and brings display up.
 *
 */
void init_all (void)
{
    fg_kbinit(1);       /* LATCH LOW-LEVEL KEYBOARD HANDLER */
    randomize();        /* ALLOW RANDOMIZATIONS             */

    init_anim();        /* CALL BEFORE WORLD CREATION       */
    init_data();        /* CALL BEFORE WORLD CREATION       */
    init_world();       /* RANDOMIZE THE WORLD              */
    init_video();       /* SET OUR VIDEO MODE ETC.          */

    fade_init();        /* ALLOW FADING                     */
    fade_blackout();    /* SET ALL COLORS TO BLACK          */
    redraw(REFRESH);    /* DRAW THE SCREEN (UNSEEN)         */
    fade_in_all();      /* FADE IN SCREEN                   */
}

/*
 *
 * Keycheck checks all keys and reacts upon them.
 * Returns 1 if a key has indicated the user has requested to quit.
 *
 */
int keycheck (void)
{
    if (fg_kbtest(SCAN_T)) time_test();
    if (fg_kbtest(SCAN_C)) cheat (0);
    if (fg_kbtest(SCAN_R)) cheat (1);
    if (fg_kbtest(SCAN_A)) toggle_mode(0);
    if (fg_kbtest(SCAN_E)) toggle_mode(1);
    if (fg_kbtest(SCAN_K)) toggle_mode(2);
    if (fg_kbtest(SCAN_W)) toggle_mode(3);
    if (fg_kbtest(SCAN_F))
    {
        fg_music ("L50O4BAFDEF.$");
        switch (frogmode)
        {
            case 1: frogmode=2; break;
            case 3: frogmode=0; break;
        }
    }
    if (fg_kbtest(SCAN_G)) /* RERANDOMIZE GRASS/DIRT TENDENCY */
    {
        world_type=random(100);
        fg_music ("S1L20C..B..A..$");
    }
    if (fg_kbtest(SCAN_SPACE))  make_world (0);
    if (fg_kbtest(SCAN_B))      make_world (1);
    if (fg_kbtest(SCAN_ENTER))  make_world (2);
    if (fg_kbtest(SCAN_S))      view_tile_page();

    move_view(); /* RESPOND TO ARROW KEYS MOVING VIEW */

    if (fg_kbtest(SCAN_ESC) || fg_kbtest(SCAN_Q)) /* ESCAPE TO QUIT */
        return 1;

    return 0;
}

