#define TILE_H

/*
 *
 * Regular non-animated tiles.
 *
 */
#define OBJ_SIGN        20
#define OBJ_ROCK1       56
#define OBJ_ROCK2       57
#define OBJ_ROCK3       58
#define OBJ_FIRST_GEM   23
#define OBJ_TOTAL_GEM   4
#define OBJ_BUSHES      27

/*
 *
 * Animation designations.  Edit actual tile numbers in anim.c.
 *
 */
#define ANIM_LIST_TOTAL 5 /* INCREASE WHEN ADDING struct ANIMATIONs */
#define ANM_START       500

#define ANM_FIRE        500
#define ANM_WATER1      501
#define ANM_WATER2      502
#define ANM_URANIUM     503
#define ANM_FROG        504

/*
 *
 * Frog with sneakers
 *
 */
#define FROG_FACE_RIGHT 80
#define FROG_WALK_RIGHT 81
#define FROG_FACE_DOWN  85
#define FROG_WALK_DOWN  86
#define FROG_FACE_LEFT  90
#define FROG_WALK_LEFT  91
#define FROG_FACE_UP    95
#define FROG_WALK_UP    96
#define FROG_NUM_WALKS  4

/*
 *
 * Dirt-into-grass edges.
 *
 */
#define DIRTEDGE_U    40
#define DIRTEDGE_R    41
#define DIRTEDGE_D    42
#define DIRTEDGE_L    43
#define DIRTEDGE_UR   44
#define DIRTEDGE_UD   45
#define DIRTEDGE_UL   46
#define DIRTEDGE_RD   47
#define DIRTEDGE_RL   48
#define DIRTEDGE_DL   49
#define DIRTEDGE_URD  50
#define DIRTEDGE_URL  51
#define DIRTEDGE_RDL  52
#define DIRTEDGE_URDL 53
#define DIRTEDGE_UDL  54

/*
 *
 * Road tiles
 *
 */
#define ROAD_V   0  /* ³ */
#define ROAD_H   1  /* Ä */
#define ROAD_UR  2  /* Ú */
#define ROAD_UL  3  /* ¿ */
#define ROAD_DR  4  /* À */
#define ROAD_DL  5  /* Ù */
#define ROAD_X   6  /* Å */
#define ROAD_TL  7  /* ´ */
#define ROAD_TR  8  /* Ã */
#define ROAD_TU  9  /* Á */
#define ROAD_TD 10  /* Â */

/* AIDS TO ROADS */
#define FIRST_ROAD_TILE 0
#define NUM_ROAD_TILES 11
#define LAST_ROAD_TILE 10

/*
 *
 * More tile definitions, mainly for grouping tiles together.
 *
 */
#define NUM_GRASS_TILES 7   /* NUMBER OF "GRASS" TILES */
#define FIRST_GRASS_TILE 11 /* INDEX OF FIRST GRASS TILE */

#define NUM_DIRT_TILES 2
#define FIRST_DIRT_TILE 18

#define NUM_LAND_TILES 9 /* INCLUDES GRASS AND DIRT */
#define FIRST_LAND_TILE 11

#define CHANCE_LAND_GROUPING 75 /* PERCENT CHANCE GROUPING WILL OCCUR */

#define MIN_TERRAIN (WORLD_TILES_TOTAL/100)
#define MAX_TERRAIN (WORLD_TILES_TOTAL/2)

/*
 *
 * General animation defines -- others are at top of this file.
 *
 */
#define ANM_END         (ANM_START+ANIM_LIST_TOTAL)
#define is_anim(x) ((x)>=ANM_START && (x)<ANM_END)

/*
 *
 * Macros for testing a tile index' contents.
 *
 */
#define isdirt(d) ((d)>=FIRST_DIRT_TILE && (d)<FIRST_DIRT_TILE+NUM_DIRT_TILES)
#define isgrass(g) ((g)>=FIRST_GRASS_TILE && (g)<FIRST_GRASS_TILE+NUM_GRASS_TILES)
#define island(l) (isdirt(l) || island(l))

