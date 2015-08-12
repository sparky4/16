#define INITROAD_C

#include <stdlib.h>
#include <time.h>
#include <fastgraf.h>
#include "roads.h"
#include "tiles.h"

extern int far *topography;      /* BACKGROUND TILE LIST (ARRAY) */
extern int far *terrain;      /* FOREGROUND TILE LIST (ARRAY) */

/* FOR AI - NUMBER OF ROADS TO PLACE ON SCREEN */
#define MAX_ROADS ((WORLD_WIDTH+WORLD_HEIGHT)/2)
#define MIN_ROADS ((WORLD_WIDTH+WORLD_HEIGHT)/8)
#define CHANCE_CROSSROAD 75 /* 3/4 CHANCE OF A CROSSROAD VS T */

/* DIRECTIONS */
#define NUM_DIRECTIONS  4
#define FIRST_DIRECTION 1

#define DIRECTION_UP    1
#define DIRECTION_DOWN  2
#define DIRECTION_LEFT  3
#define DIRECTION_RIGHT 4

#define ROAD_LEADERS 12 /* USED IN CREATE_ROADS */

/*
 *
 * Randomly creates roads and places them in the foreground tile list.
 * This function overwrites other non-road foreground tiles.
 * (To prevent this, call this function first.)
 *
 */
void create_roads (void)
{
    int entry, keepgoing=TRUE, direction, chance;
    int x,y,nextx,nexty; /* LOCATION OF CURRENTLY PLACED ROAD TILE */
    int roads_left; /* NUMBER OF COMPLETE ROADS LEFT TO PLACE */
    int failed_roads=0;
    int current_tile;

		/* ROADS ARE PICKED FROM THESE LISTS -- INCREASE ROAD_LEADERS  */
        /* AND ADD ROAD TILES TO THESE LISTS TO INCREASE CHANCE OF     */
        /* SPECIFIED ROADS BEING PLACED                                */
    int roads_to_right [ROAD_LEADERS] = { ROAD_H, ROAD_H, ROAD_UR, ROAD_DR,
                                          ROAD_H, ROAD_H, ROAD_H, ROAD_H,
                                          ROAD_H, ROAD_H, ROAD_H, ROAD_H,  };
    int roads_to_left  [ROAD_LEADERS] = { ROAD_H, ROAD_H, ROAD_UL, ROAD_DL,
                                          ROAD_H, ROAD_H, ROAD_H, ROAD_H,
                                          ROAD_H, ROAD_H, ROAD_H, ROAD_H,  };
    int roads_to_down  [ROAD_LEADERS] = { ROAD_V, ROAD_V, ROAD_UL, ROAD_UR,
                                          ROAD_V, ROAD_V, ROAD_V, ROAD_V,
                                          ROAD_V, ROAD_V, ROAD_V, ROAD_V,  };
    int roads_to_up    [ROAD_LEADERS] = { ROAD_V, ROAD_V, ROAD_DL, ROAD_DR,
										  ROAD_V, ROAD_V, ROAD_V, ROAD_V,
                                          ROAD_V, ROAD_V, ROAD_V, ROAD_V,  };

    roads_left=random (MAX_ROADS-MIN_ROADS)+MIN_ROADS;

        /************************************************\
         *     PLACE FIRST TILE AT WORLD SCREEN EDGE    *
        \************************************************/

    while (roads_left-- && failed_roads<=MAX_FAILS)
    {
            /* PICK RANDOM ENTRY POINT */
        keepgoing=TRUE;
        entry=random(NUM_DIRECTIONS)+FIRST_DIRECTION;
        switch (entry)
        {
/********/  case DIRECTION_UP: /* TOP ENTRY */

            x=random(WORLD_WIDTH); y=0;
            current_tile=terrain[WORLD_TILE(x,y)];

            if (!isroad(current_tile))
            {
                current_tile=terrain[WORLD_TILE(x,y)]=
                    roads_to_up[random(ROAD_LEADERS)];
                direction=roadexit(current_tile, DIRECTION_DOWN);
				break;
            }

            else
            {
                roads_left++;
                failed_roads++;
                keepgoing=FALSE;
                break;
            }

/********/  case DIRECTION_DOWN: /* BOTTOM ENTRY */

			x=random(WORLD_WIDTH); y=WORLD_HEIGHT-1;
            current_tile=terrain[WORLD_TILE(x,y)];

            if (!isroad(current_tile))
            {
                current_tile=terrain[WORLD_TILE(x,y)]=
                    roads_to_down[random(ROAD_LEADERS)];
                direction=roadexit(current_tile, DIRECTION_UP);
            }

            else
            {
                roads_left++;
				failed_roads++;
                keepgoing=FALSE;
            }
            break;

/********/  case DIRECTION_LEFT: /* LEFT ENTRY */

            x=0; y=random(WORLD_HEIGHT);
            current_tile=terrain[WORLD_TILE(x,y)];

            if (!isroad(current_tile))
            {
                current_tile=terrain[WORLD_TILE(x,y)]=
					roads_to_left[random(ROAD_LEADERS)];
                direction=roadexit(current_tile, DIRECTION_RIGHT);
            }

            else
            {
                roads_left++;
                failed_roads++;
                keepgoing=FALSE;
            }
            break;

/********/  case DIRECTION_RIGHT: /* RIGHT ENTRY */

            x=WORLD_WIDTH-1; y=random(WORLD_HEIGHT);
            current_tile=terrain[WORLD_TILE(x,y)];

            if (!isroad(current_tile))
            {
                current_tile=terrain[WORLD_TILE(x,y)]=
                    roads_to_right[random(ROAD_LEADERS)];
                direction=roadexit(current_tile, DIRECTION_LEFT);
            }

            else
            {
				roads_left++;
                failed_roads++;
                keepgoing=FALSE;
            }
            break;
        }

        /************************************************\
         * PLACE SUBSEQUENT TILES AWAY FROM ENTRY POINT *
        \************************************************/

        while (keepgoing)
        {
			switch (direction)
            {
/********/      case DIRECTION_UP: /* UP */

                if (--y<0)
                {
                    keepgoing=FALSE;
                    break;
                }

                current_tile=terrain[WORLD_TILE(x,y)];
                if (!isroad(current_tile))
                {
                    current_tile=terrain[WORLD_TILE(x,y)]=
                        roads_to_down[random(ROAD_LEADERS)];
                    direction=roadexit(current_tile, DIRECTION_UP);
                }

                else /* INTERSECTION OCCURS */
                {
                    if (random(100)>=CHANCE_CROSSROAD &&
                        terrain[WORLD_TILE(x,y-1)]==EMPTY_TILE)
                    {
                        keepgoing=FALSE;
                        terrain[WORLD_TILE(x,y)]=makeintersection
                            (current_tile, DIRECTION_DOWN);
					}

                    else /* CROSSROAD AND CONTINUE */
                    {
                        terrain[WORLD_TILE(x,y)]=makeintersection
                            (makeintersection(current_tile, DIRECTION_UP),
                            DIRECTION_DOWN); /* ADD BOTH RAMPS */
                    }
                }
                break;

/********/      case DIRECTION_DOWN:  /* DOWN */

				if (++y>=WORLD_HEIGHT)
                {
                    keepgoing=FALSE;
                    break;
                }

                current_tile=terrain[WORLD_TILE(x,y)];
                if (!isroad(current_tile))
                {
                    current_tile=terrain[WORLD_TILE(x,y)]=
                        roads_to_up[random(ROAD_LEADERS)];
                    direction=roadexit(current_tile, DIRECTION_DOWN);
                }

                else /* INTERSECTION OCCURS */
                {
                    if (random(100)>=CHANCE_CROSSROAD &&
                        terrain[WORLD_TILE(x,y+1)]==EMPTY_TILE)

                    {
                        keepgoing=FALSE;
                        terrain[WORLD_TILE(x,y)]=makeintersection
                            (current_tile, DIRECTION_UP);
                    }

                    else /* CROSSROAD AND CONTINUE */
					{
                        terrain[WORLD_TILE(x,y)]=makeintersection
                            (makeintersection(current_tile, DIRECTION_DOWN),
                            DIRECTION_UP); /* ADD BOTH RAMPS */
                    }
                }
                break;

/********/      case DIRECTION_LEFT: /* LEFT */

                if (--x<0)
                {
                    keepgoing=FALSE;
					break;
                }

                current_tile=terrain[WORLD_TILE(x,y)];
                if (!isroad(current_tile))
                {
                    current_tile=terrain[WORLD_TILE(x,y)]=
                        roads_to_right[random(ROAD_LEADERS)];
                    direction=roadexit(current_tile, DIRECTION_LEFT);
                }

                else /* INTERSECTION OCCURS */
                {
					if (random(100)>=CHANCE_CROSSROAD &&
                        terrain[WORLD_TILE(x-1,y)]==EMPTY_TILE)

                    {
                        keepgoing=FALSE;
                        terrain[WORLD_TILE(x,y)]=makeintersection
                            (current_tile, DIRECTION_RIGHT);
                    }

                    else /* CROSSROAD AND CONTINUE */
                    {
                        terrain[WORLD_TILE(x,y)]=makeintersection
                            (makeintersection(current_tile, DIRECTION_LEFT),
							DIRECTION_RIGHT); /* ADD BOTH RAMPS */
                    }
                }
                break;

/********/      case DIRECTION_RIGHT: /* RIGHT */

                if (++x>=WORLD_WIDTH)
                {
                    keepgoing=FALSE;
                    break;
                }

                current_tile=terrain[WORLD_TILE(x,y)];
                if (!isroad(current_tile))
                {
                    current_tile=terrain[WORLD_TILE(x,y)]=
                        roads_to_left[random(ROAD_LEADERS)];
                    direction=roadexit(current_tile, DIRECTION_RIGHT);
                }

                else /* INTERSECTION OCCURS */
                {
                    if (random(100)>=CHANCE_CROSSROAD &&
                        terrain[WORLD_TILE(x+1,y)]==EMPTY_TILE)

					{
                        keepgoing=FALSE;
                        terrain[WORLD_TILE(x,y)]=makeintersection
                            (current_tile, DIRECTION_LEFT);
                    }

                    else /* CROSSROAD AND CONTINUE */
                    {
                        terrain[WORLD_TILE(x,y)]=makeintersection
                            (makeintersection(current_tile, DIRECTION_RIGHT),
                            DIRECTION_LEFT); /* ADD BOTH RAMPS */
                    }
                }
				break;
            } /* "DIRECTION" HERE */
        } /* STOP "KEEPGOING" HERE */
    }
}

/*
 *
 * Returns the unspecified direction in an angled road.
 *
 */
int roadexit (int road, int direction)
{
	switch (direction)
    {
        case DIRECTION_UP: /* up */
        if (road==ROAD_V)  return DIRECTION_UP;
        if (road==ROAD_UL) return DIRECTION_LEFT;
        if (road==ROAD_UR) return DIRECTION_RIGHT;
        break;

        case DIRECTION_DOWN: /* down */
        if (road==ROAD_V)  return DIRECTION_DOWN;
        if (road==ROAD_DL) return DIRECTION_LEFT;
        if (road==ROAD_DR) return DIRECTION_RIGHT;
        break;

        case DIRECTION_LEFT: /* left */
        if (road==ROAD_DR) return DIRECTION_UP;
        if (road==ROAD_UR) return DIRECTION_DOWN;
        if (road==ROAD_H)  return DIRECTION_LEFT;
        break;

        case DIRECTION_RIGHT: /* right */
        if (road==ROAD_DL) return DIRECTION_UP;
        if (road==ROAD_UL) return DIRECTION_DOWN;
        if (road==ROAD_H)  return DIRECTION_RIGHT;
        break;
    }

    fg_music ("A$");
    return ERROR_TILE;
}

/*
 *
 * Adds a road (ramp) to the specified road, and returns the
 * tile number of what the new road is made of.
 *
 */
int makeintersection (int road, int ramp)
{
	switch (road)
    {
        case ROAD_X:  /* Å */
        return ROAD_X;

        case ROAD_V:  /* ³ */
        if (ramp==DIRECTION_LEFT) return ROAD_TL;
        if (ramp==DIRECTION_RIGHT) return ROAD_TR;
        return ROAD_V;

        case ROAD_H:  /* Ä */
        if (ramp==DIRECTION_UP) return ROAD_TU;
        if (ramp==DIRECTION_DOWN) return ROAD_TD;
		return ROAD_H;

        case ROAD_UR: /* Ú */
        if (ramp==DIRECTION_UP) return ROAD_TR;
        if (ramp==DIRECTION_LEFT) return ROAD_TD;
        return ROAD_UR;

        case ROAD_UL: /* ¿ */
        if (ramp==DIRECTION_UP) return ROAD_TL;
        if (ramp==DIRECTION_RIGHT) return ROAD_TD;
        return ROAD_UL;

        case ROAD_DR: /* À */
		if (ramp==DIRECTION_DOWN) return ROAD_TR;
        if (ramp==DIRECTION_LEFT) return ROAD_TU;
        return ROAD_DR;

        case ROAD_DL: /* Ù */
        if (ramp==DIRECTION_DOWN) return ROAD_TL;
        if (ramp==DIRECTION_RIGHT) return ROAD_TU;
        return ROAD_DL;

        case ROAD_TL: /* ´ */
        if (ramp==DIRECTION_RIGHT) return ROAD_X;
        return ROAD_TL;

		case ROAD_TR: /* Ã */
        if (ramp==DIRECTION_LEFT) return ROAD_X;
        return ROAD_TR;

        case ROAD_TU: /* Á */
        if (ramp==DIRECTION_DOWN) return ROAD_X;
        return ROAD_TU;

        case ROAD_TD: /* Â */
        if (ramp==DIRECTION_UP) return ROAD_X;
        return ROAD_TD;
    }

	fg_music ("A$");
    return ERROR_TILE;
}

/*
    AI USED IN ROAD FUNCTIONS:
    pick random entry point on one of the four sides
    place subsequent tiles with tendency to move from screen
    place subsequent tiles with tendency use V or H pieces if just used
    if hit existing tile, either
    a) cross over road and continue, using X tile
    b) terminate road with a T
    repeat until MAX_ROADS complete
*/

/*
 *
 * Adds edges to places where grass and dirt meet.
 * Called by init_background() to liven up background display.
 *
 */
void add_dirt_edges (void)
{
    register int x, y;
    int tile;

    /* ADD 90 DEGREE EDGES */
    for (y=0; y<WORLD_HEIGHT; y++)
    for (x=0; x<WORLD_WIDTH; x++)
    if (isdirt(topography[WORLD_TILE(x,y)]))
    {
        tile=0; /* ADD UP BINARY TILE NUMBER */
        if (y-1>=0) tile+=isgrass(topography[WORLD_TILE(x,y-1)]);
        if (x+1<WORLD_WIDTH) tile+=isgrass(topography[WORLD_TILE(x+1,y)])*2;
        if (y+1<WORLD_HEIGHT) tile+=isgrass(topography[WORLD_TILE(x,y+1)])*4;
        if (x-1>=0) tile+=isgrass(topography[WORLD_TILE(x-1,y)])*8;

        /* CONVERT BINARY TILE NUMBER TO ACTUAL */
        switch (tile)
        {
            case 1:  tile=DIRTEDGE_U;    break;
            case 2:  tile=DIRTEDGE_R;    break;
            case 3:  tile=DIRTEDGE_UR;   break;
            case 4:  tile=DIRTEDGE_D;    break;
            case 5:  tile=DIRTEDGE_UD;   break;
            case 6:  tile=DIRTEDGE_RD;   break;
            case 7:  tile=DIRTEDGE_URD;  break;
            case 8:  tile=DIRTEDGE_L;    break;
            case 9:  tile=DIRTEDGE_UL;   break;
            case 10: tile=DIRTEDGE_RL;   break;
            case 11: tile=DIRTEDGE_URL;  break;
            case 12: tile=DIRTEDGE_DL;   break;
            case 13: tile=DIRTEDGE_UDL;  break;
            case 14: tile=DIRTEDGE_RDL;  break;
            case 15: tile=DIRTEDGE_URDL; break;
            case 0:  tile=topography[WORLD_TILE(x,y)]; break; /* NO CHANGE */
        }
        topography[WORLD_TILE(x,y)]=tile; /* CHANGE TILE */
    }
}

