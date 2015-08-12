#define ROADS_H

/* GENERIC TILING DEFINITIONS */

#define WORLD_WIDTH       105    /* IN TILES, WIDTH OF "PLAYFIELD"  */
#define WORLD_HEIGHT      100    /* IN TILES, HEIGHT OF "PLAYFIELD" */
#define WORLD_TILES_TOTAL 10500  /* (WORLD_WIDTH*WORLD_HEIGHT)     */

#define TILE_WIDTH       16  /* IN PIXELS, WIDTH OF TILE    */
#define TILE_HEIGHT      16  /* IN PIXELS, HEIGHT OF TILE   */
#define VIEW_WIDTH       20  /* (SCREEN_WIDTH/TILE_WIDTH)   */
#define VIEW_HEIGHT      15  /* (SCREEN_HEIGHT/TILE_HEIGHT) */
#define VIEW_TILES_TOTAL 300 /* (VIEW_WIDTH*VIEW_HEIGHT)    */
#define MIXING_TILE      299 /* (VIEW_TILES_TOTAL-1)        */
#define EMPTY_TILE -1   /* STANDARD INDEX FOR A SEE-THROUGH TILE */
#define ERROR_TILE -2   /* STANDARD INDEX FOR ERRORS! */

/* PROTOTYPES */

void create_roads (void);
int roadexit (int road, int direction);
int makeintersection (int road, int ramp);
void init_foreground (void);
void init_background (void);
void add_dirt_edges (void);
void init_world(void);
int redraw (int draw_type);
void init_video (void);
void init_data (void);
void program_shutdown (char *msg, int errcode);
void place_tile_block (int x1, int y1, int x2, int y2);
void init_anim (void);
void animate (void);
void update_tile (int tile);
void gogofrog (void);

/* VIDEO MODE DEFINITIONS */
#define VMODE  22
#define VPAGES 3
#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   240
#define IMAGES "roads.gif" /* IMAGE CONTAINING TILES */

/* VIDEO PAGE DEFINITIONS */
#define VIEWPAGE1 0
#define VIEWPAGE2 1
#define TILEPAGE  2

/* STANDARD DEFINITIONS */
#define FALSE 0
#define TRUE 1

/* CHECKS IF A TILE IS A ROAD OR NOT */
#define isroad(r) (r>=FIRST_ROAD_TILE && r<=LAST_ROAD_TILE)

#define REFRESH         0
#define SCROLL_UP       1
#define SCROLL_DOWN     2
#define SCROLL_LEFT     3
#define SCROLL_RIGHT    4
#define SCROLL_UR       5
#define SCROLL_DR       6
#define SCROLL_DL       7
#define SCROLL_UL       8
#define NONFLIP_SCROLL_UP       11
#define NONFLIP_SCROLL_DOWN     12
#define NONFLIP_SCROLL_LEFT     13
#define NONFLIP_SCROLL_RIGHT    14
#define NONFLIP_REFRESH         15

#define MAX_FAILS 50 /* MAXIMUM NUMBER OF ATTEMPTS TO PLACE TILES */

/*
 *
 * MACROS TO CONVERT TILES FROM "ROW/COLUMN" FORMAT TO "INDEX" FORMAT
 *
 */

        /* WORLD SPACE CONVERSIONS */
#define WORLD_TILE(x,y) ((x)+(y)*WORLD_WIDTH) /* CONVERTS DUAL X,Y TO SINGLE */
#define WORLD_TILE_X(z) ((z)%WORLD_WIDTH)     /* CONVERTS SINGLE TO DUAL X   */
#define WORLD_TILE_Y(z) ((z)/WORLD_WIDTH)     /* CONVERTS SINGLE TO DUAL Y   */

        /* VIEW SPACE CONVERSIONS */
#define VIEW_TILE(x,y)  ((x)+(y)*VIEW_WIDTH)  /* CONVERTS DUAL X,Y TO SINGLE */
#define VIEW_TILE_X(z)  ((z)%VIEW_WIDTH)      /* CONVERTS SINGLE TO DUAL X   */
#define VIEW_TILE_Y(z)  ((z)/VIEW_WIDTH)      /* CONVERTS SINGLE TO DUAL Y   */

/* RETURNS 1 IF A SINGLE WORLD TILE INDEX IS WITHIN THE VIEWING SCREEN */
#define is_viewable(x)  (WORLD_TILE_X(x)>=view_x &&          \
                        WORLD_TILE_X(x)<view_x+VIEW_WIDTH && \
                        WORLD_TILE_Y(x)>=view_y &&           \
                        WORLD_TILE_Y(x)<view_y+VIEW_HEIGHT)  \

/* CONVERTS A WORLD INDEX INTO A VIEW INDEX -- USE IS_VIEWABLE TO MAKE */
/* SURE THE WORLD INDEX IS WITHIN THE VIEWING AREA!                    */
#define WORLD_TO_VIEW(x) VIEW_TILE(WORLD_TILE_X((x))-view_x,WORLD_TILE_Y((x))-view_y)

#define DONS_VIEW_TO_WORLD(x) (WORLD_TILE (view_x, view_y) + (VIEW_TILE_Y (x) * WORLD_WIDTH) + VIEW_TILE_X (x)  /* CONVERT VIEW SINGLE TO WORLD SINGLE */
#define VIEW_TO_WORLD(x) (WORLD_TILE (VIEW_TILE_X (x) + view_x, VIEW_TILE_Y (x) + view_y)) /* CONVERT VIEW SINGLE TO WORLD SINGLE */

/*
 *
 * MACROS TO EASE COPYING OF TILES BETWEEN AND AMONG PAGES
 *
 */

   /* INTENRAL MACRO USED IN COPY TILE MACROS -- USER, DO NOT CALL! */
#define COPY_TILE_CORE(from,to,spage,dpage)        \
    ((from)%VIEW_WIDTH)*TILE_WIDTH,         \
    (((from)%VIEW_WIDTH)+1)*TILE_WIDTH-1,   \
    ((from)/VIEW_WIDTH)*TILE_HEIGHT,        \
    (((from)/VIEW_WIDTH)+1)*TILE_WIDTH-1,   \
    ((to)%VIEW_WIDTH)*TILE_WIDTH,           \
    (((to)/VIEW_WIDTH)+1)*TILE_HEIGHT-1,    \
    spage, dpage

    /* TRANSPARENT TILE COPY FROM HIDDEN PAGE TO ACTIVE PAGE */
#define copy_tile_tr(from_tile,to_tile) \
    fg_tcxfer (COPY_TILE_CORE(from_tile,to_tile,TILEPAGE,VIEWPAGE))

    /* OPAQUE TILE COPY FROM HIDDEN PAGE TO ACTIVE PAGE */
#define copy_tile_op(from_tile,to_tile) \
    fg_transfer (COPY_TILE_CORE(from_tile,to_tile,TILEPAGE,VIEWPAGE))

    /* TRANSPARENT TILE COPY */
#define pagecopy_tile_tr(from_tile,to_tile,sourcepage,destpage) \
    fg_tcxfer (COPY_TILE_CORE(from_tile,to_tile,sourcepage,destpage))

    /* OPAQUE TILE COPY FROM */
#define pagecopy_tile_op(from_tile,to_tile,sourcepage,destpage) \
    fg_transfer (COPY_TILE_CORE(from_tile,to_tile,sourcepage,destpage))

struct ANIMATION
{
    int anm;        /* NUMBER USED TO ENCODE ANIMATION IN WORLD */
    int total;      /* TOTAL NUMBER OF TILES USED IN ANIMATION  */
    int first;      /* FIRST TILE NO. IN ANIMATION              */
    int current;    /* TILE CURRENTLY BEING SHOWN IN ANIMATION  */
    int delay;      /* DELAY BETWEEN TILES IN CLOCK TICKS       */
    long next;      /* TIME OF NEXT UPDATE                      */
};

