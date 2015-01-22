#define TILEIO_C

#include <fastgraf.h>
#include "roads.h"
#include "tiles.h" /* NEEDS ACCESS BECAUSE OF ANIMS */

extern int far *topography;      /* BACKGROUND TILE LIST (ARRAY) */
extern int far *terrain;      /* FOREGROUND TILE LIST (ARRAY) */
extern struct ANIMATION *anim_list[ANIM_LIST_TOTAL];

int view_x=0, view_y=0;        /* VIEW AREA (UPPER LEFT CORNER) */
int viewpage=0;                /* CURRENTLY VIEWED PAGE */

/*
 *
 * Redraws the screen view area.  Returns 1 on successful scroll.
 *
 */
int redraw (int draw_type)
{
    int new_draw_type;
    int pageflip=1;

    switch (draw_type)
    {
        case NONFLIP_SCROLL_RIGHT:
            new_draw_type=SCROLL_RIGHT; pageflip=0; break;
        case NONFLIP_SCROLL_LEFT:
            new_draw_type=SCROLL_LEFT; pageflip=0; break;
        case NONFLIP_SCROLL_UP:
            new_draw_type=SCROLL_UP; pageflip=0; break;
        case NONFLIP_SCROLL_DOWN:
            new_draw_type=SCROLL_DOWN; pageflip=0; break;
        case NONFLIP_REFRESH:
            new_draw_type=REFRESH; pageflip=0; break;
        default:
            new_draw_type=draw_type; break;
    }

    switch (new_draw_type)
	{
        case REFRESH: /* Full Refresh */
        place_tile_block (0, 0, VIEW_WIDTH-1, VIEW_HEIGHT-1);
        break;

        case SCROLL_UP:
        if (view_y<=0) return 0; /* DON'T ALLOW TO SCROLL BEYOND WORLD */
        view_y--;
        fg_transfer (0, SCREEN_WIDTH-1, 0, SCREEN_HEIGHT-TILE_HEIGHT-1, 0, SCREEN_HEIGHT-1, viewpage, !viewpage);
        place_tile_block (0, 0, VIEW_WIDTH-1, 0);
        break;

        case SCROLL_DOWN:
        if (view_y>=WORLD_HEIGHT-VIEW_HEIGHT) return 0; /* HEY! */
        view_y++;
        fg_transfer (0, SCREEN_WIDTH-1, TILE_HEIGHT, SCREEN_HEIGHT-1, 0, SCREEN_HEIGHT-TILE_HEIGHT-1, viewpage, !viewpage);
        place_tile_block (0, VIEW_HEIGHT-1, VIEW_WIDTH-1, VIEW_HEIGHT-1);
        break;

        case SCROLL_LEFT:
        if (view_x<=0) return 0; /* DON'T ALLOW TO SCROLL BEYOND WORLD */
        view_x--;
        fg_transfer (0, SCREEN_WIDTH-TILE_WIDTH-1, 0, SCREEN_HEIGHT-1, TILE_WIDTH, SCREEN_HEIGHT-1, viewpage, !viewpage);
        place_tile_block (0, 0, 0, VIEW_HEIGHT-1);
        break;

        case SCROLL_RIGHT:
        if (view_x>=WORLD_WIDTH-VIEW_WIDTH) return 0; /* HEY! */
        view_x++;
        fg_transfer (TILE_WIDTH, SCREEN_WIDTH-1, 0, SCREEN_HEIGHT-1, 0, SCREEN_HEIGHT-1, viewpage, !viewpage);
        place_tile_block (VIEW_WIDTH-1, 0, VIEW_WIDTH-1, VIEW_HEIGHT-1);
        break;

        case SCROLL_UR:
        if (view_x>=WORLD_WIDTH-VIEW_WIDTH) return redraw (SCROLL_UP);
        if (view_y<=0) return redraw(SCROLL_RIGHT);
        view_y--;  view_x++;
        fg_transfer (TILE_WIDTH, SCREEN_WIDTH-1, 0, SCREEN_HEIGHT-TILE_HEIGHT-1, 0, SCREEN_HEIGHT-1, viewpage, !viewpage);
        place_tile_block (0, 0, VIEW_WIDTH-1, 0);
        place_tile_block (VIEW_WIDTH-1, 0, VIEW_WIDTH-1, VIEW_HEIGHT-1);
        break;

        case SCROLL_DR:
        if (view_x>=WORLD_WIDTH-VIEW_WIDTH) return redraw(SCROLL_DOWN);
        if (view_y>=WORLD_HEIGHT-VIEW_HEIGHT) return redraw(SCROLL_RIGHT);
        view_y++;  view_x++;
        fg_transfer (TILE_WIDTH, SCREEN_WIDTH-1, TILE_HEIGHT, SCREEN_HEIGHT-1, 0, SCREEN_HEIGHT-TILE_HEIGHT-1, viewpage, !viewpage);
        place_tile_block (0, VIEW_HEIGHT-1, VIEW_WIDTH-1, VIEW_HEIGHT-1);
        place_tile_block (VIEW_WIDTH-1, 0, VIEW_WIDTH-1, VIEW_HEIGHT-1);
        break;

        case SCROLL_DL:
        if (view_x<=0) return redraw (SCROLL_DOWN);
        if (view_y>=WORLD_HEIGHT-VIEW_HEIGHT) return redraw(SCROLL_LEFT);
        view_y++;  view_x--;
        fg_transfer (0, SCREEN_WIDTH-TILE_WIDTH-1, TILE_HEIGHT, SCREEN_HEIGHT-1, TILE_WIDTH, SCREEN_HEIGHT-TILE_HEIGHT-1, viewpage, !viewpage);
        place_tile_block (0, VIEW_HEIGHT-1, VIEW_WIDTH-1, VIEW_HEIGHT-1);
        place_tile_block (0, 0, 0, VIEW_HEIGHT-1);
        break;

        case SCROLL_UL:
        if (view_x<=0) return redraw (SCROLL_UP);
        if (view_y<=0) return redraw (SCROLL_LEFT);
        view_y--;  view_x--;
        fg_transfer (0, SCREEN_WIDTH-TILE_WIDTH-1, 0, SCREEN_HEIGHT-TILE_HEIGHT-1, TILE_WIDTH, SCREEN_HEIGHT-1, viewpage, !viewpage);
        place_tile_block (0, 0, VIEW_WIDTH-1, 0);
        place_tile_block (0, 0, 0, VIEW_HEIGHT-1);
        break;
    }

    if (pageflip)
    {
        viewpage=!viewpage; /* SWAP PAGES */
        fg_setvpage (viewpage);
    }

    return 1; /* SUCCESSFUL SCROLL */
}

/*
 *
 * Redraws a rectangular region of the active (!shown) page with tiles.
 *
 */

void place_tile_block (int x1, int y1, int x2, int y2)
{
    register int x, y;
    int tile, a;

    for (y=view_y+y1; y<=view_y+y2; y++)
    for (x=view_x+x1; x<=view_x+x2; x++)
    {
        tile=terrain[WORLD_TILE(x,y)];

            /* DON'T PLACE FOREGROUND IF EMPTY */
        if (tile==EMPTY_TILE)
            pagecopy_tile_op (topography[WORLD_TILE(x,y)],VIEW_TILE(x-view_x,y-view_y), TILEPAGE, !viewpage);

        else
        {
            if (is_anim(tile))
            {
                a=0;
                while (anim_list[a]->anm!=tile) a++;
                tile=anim_list[a]->current;
            }

            pagecopy_tile_op (topography[WORLD_TILE(x,y)], VIEW_TILE(x-view_x,y-view_y), TILEPAGE, !viewpage);
            pagecopy_tile_tr (tile, VIEW_TILE(x-view_x,y-view_y), TILEPAGE, !viewpage);
        }
    }
}

/*
 *
 * Updates on the visual page a single world tile.
 *
 */
void update_tile (int tile)
{
    register int viewtile;

    if (!is_viewable (tile)) return; /* DO NOT UPDATE THE UNSEEN */

    viewtile=VIEW_TILE(WORLD_TILE_X(tile)-view_x,WORLD_TILE_Y(tile)-view_y);

    if (terrain[tile]==EMPTY_TILE)
        pagecopy_tile_op (topography[tile], viewtile, TILEPAGE, viewpage);

    else
    {
        pagecopy_tile_op (topography[tile], MIXING_TILE, TILEPAGE, TILEPAGE);
        pagecopy_tile_tr (terrain[tile], MIXING_TILE, TILEPAGE, TILEPAGE);
        pagecopy_tile_op (MIXING_TILE, viewtile, TILEPAGE, viewpage);
    }
}

