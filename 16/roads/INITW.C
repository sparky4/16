#define INITW_C

#include <stdlib.h>
#include <time.h>
#include "roads.h"
#include "tiles.h"

extern int view_x, view_y;   /* VIEW AREA (UPPER LEFT CORNER) */
extern int viewpage;         /* CURRENTLY VIEWED PAGE */
extern int startup_vmode;    /* VIDEO MODE STARTUP SETTINGS, -1 IF NOT INIT */

int far *topography;      /* BACKGROUND TILE LIST (ARRAY) */
int far *terrain;      /* FOREGROUND TILE LIST (ARRAY) */
int world_type=75;      /* TENDENCY TO GRASS */
int edgemode=1;         /* BLOCKY GRASS/DIRT OR EDGED? */
extern int frogmode;

/*
 *
 * Loads the world foreground tile list with roads.
 *
 */
void init_foreground(void)
{
    register int x, tile, fails;

        /* INITIALIZE FOREGROUND */
    for (x=0; x<WORLD_TILES_TOTAL; x++)
        terrain[x]=EMPTY_TILE; /* FILL WITH EMPTY TILES */

    create_roads (); /* LAY DOWN SOME ROADS */

        /* ADD RANDOM TERRAIN */
    for (x=0; x<random(MAX_TERRAIN-MIN_TERRAIN)+MIN_TERRAIN; x++)
    {
        fails=0;

            /* ATTEMPT TO PLACE TERRAIN -- QUIT ON EXCESSIVE FAILURE */
        do {
            tile=random(WORLD_TILES_TOTAL);
            fails++;
        } while (terrain[tile]!=EMPTY_TILE && fails<MAX_FAILS);

        if (fails<MAX_FAILS)
        {
            switch (random(12)+1)
            {
                case 1: terrain[tile]=OBJ_SIGN;     break;
                case 2: terrain[tile]=OBJ_FIRST_GEM+random(OBJ_TOTAL_GEM); break;
                case 3: terrain[tile]=ANM_URANIUM;  break;
                case 4:
                case 5: terrain[tile]=ANM_FIRE;     break;
                case 6: if (random(100)<50) terrain[tile]=ANM_WATER1;
                        else terrain[tile]=ANM_WATER2;  break;
                case 7: terrain[tile]=OBJ_ROCK1;    break;
                case 8: terrain[tile]=OBJ_ROCK2;    break;
                case 9: terrain[tile]=OBJ_ROCK3;    break;
                case 10:
                case 11:
                case 12: terrain[tile]=OBJ_BUSHES;  break;
            }
        }

        else break;
    }

    frogmode=3;
}

/*
 *
 * Loads the world background tile list with grassy tiles.
 *
 */
void init_background(void)
{
    register int x, y, landtype_left, landtype_up, landtype_here;

        /* INITIALIZE BACKGROUND BY RANDOMLY PICKING TILES */

    /* DO FIRST TILE */
    topography[0]=random(NUM_LAND_TILES)+FIRST_LAND_TILE;

    /* DO FIRST ROW */
    for (x=1; x<WORLD_WIDTH; x++)
    {
        landtype_left=isdirt(topography[x-1]);
        landtype_here=landtype_left;
        if (random(100)>=CHANCE_LAND_GROUPING) /* NO GROUPING */
        {
            if (random(100)>=world_type) landtype_here=1;
            else landtype_here=0;
        }

        if (landtype_here==0) topography[x]=
            random (NUM_GRASS_TILES)+FIRST_GRASS_TILE;   /* GRASS */
        else topography[x]=
            random (NUM_DIRT_TILES)+FIRST_DIRT_TILE;     /* DIRT */
    }

    /* DO FIRST COLUMN */
    for (y=1; y<WORLD_HEIGHT; y++)
    {
        landtype_up=isdirt(topography[WORLD_TILE(0,y-1)]);
        landtype_here=landtype_up;
		if (random(100)>=CHANCE_LAND_GROUPING) /* NO GROUPING */
        {
            if (random(100)>=world_type) landtype_here=1;
            else landtype_here=0;
        }

        if (landtype_here==0) topography[WORLD_TILE(0,y)]=
            random (NUM_GRASS_TILES)+FIRST_GRASS_TILE;   /* GRASS */
        else topography[WORLD_TILE(0,y)]=
            random (NUM_DIRT_TILES)+FIRST_DIRT_TILE;     /* DIRT */
    }

    /* DO SUBSEQUENT ROWS */
    for (y=1; y<WORLD_HEIGHT; y++)
    for (x=1; x<WORLD_WIDTH; x++)
    {
        landtype_left=isdirt(topography[WORLD_TILE(x-1,y)]);
        landtype_up=isdirt(topography[WORLD_TILE(x,y-1)]);
        landtype_here=landtype_left;

        if (random(100)>=CHANCE_LAND_GROUPING) /* UNGROUP */
        {
            if (random(100)>=world_type) landtype_here=1;
            else landtype_here=0;
        }
        else if (random(2)) landtype_here=landtype_up;

        if (landtype_here==0) topography[WORLD_TILE(x,y)]=
            random (NUM_GRASS_TILES)+FIRST_GRASS_TILE;   /* GRASS */
        else topography[WORLD_TILE(x,y)]=
            random (NUM_DIRT_TILES)+FIRST_DIRT_TILE;     /* DIRT */
    }

    if (edgemode) add_dirt_edges ();
}

/*
 *
 * Initializes background and foreground tile lists.
 *
 */
void init_world(void)
{
    init_background();
    init_foreground();
    view_x=random(WORLD_WIDTH-VIEW_WIDTH);
    view_y=random(WORLD_HEIGHT-VIEW_HEIGHT);
}
