#include "modex16.h"
#include <stdio.h>
#include <stdlib.h>

word far *clock= (word far*) 0x046C; /* 18.2hz clock */

typedef struct {
    bitmap_t *data;
    word tileHeight;
    word tileWidth;
    word rows;
    word cols;
} tiles_t;


typedef struct {
    byte    *data;
    tiles_t *tiles;
    word width;
    word height;
} map_t;


typedef struct {
    map_t *map;
    page_t *page;
    word tx;
    word ty;
} map_view_t;




map_t allocMap(int w, int h);
void initMap(map_t *map);
void mapScrollRight(map_view_t *mv, byte offset);
void mapScrollLeft(map_view_t *mv, byte offest);
void mapScrollUp(map_view_t *mv, byte offset);
void mapScrollDown(map_view_t *mv, byte offset);
void mapGoTo(map_view_t *mv, byte tx, byte ty);
void mapDrawTile(tiles_t *t, word i, page_t *page, word x, word y);

void main() {
    int show1=1;
    int tx, ty;
    int x, y;
    page_t screen;
    map_t map;
    map_view_t mv;
    byte *ptr;
    
    /* create the map */
    map = allocMap(80,60);
    initMap(&map);
    mv.map = &map;

    /* draw the tiles */
    ptr = map.data;
    modexEnter();
    screen = modexDefaultPage();
    screen.width = 352;
    mv.page = &screen;
    mapGoTo(&mv, 0, 0);

    /* scroll all the way to the right */
    for(x=0; x<(80*16-SCREEN_WIDTH); x++) {
	mapScrollRight(&mv, 1);
	modexShowPage(mv.page);
    }
    

    /* spin for a time */
    for(x=0; x<500; x++) {
        modexWaitBorder();
    }

    modexLeave();
}


map_t
allocMap(int w, int h) {
    map_t result;
    
    result.width =w;
    result.height=h;
    result.data = malloc(sizeof(byte) * w * h);

    return result;
}


void
initMap(map_t *map) {
    /* just a place holder to fill out an alternating pattern */
    int x, y;
    int i;
    int tile = 1;
    map->tiles = malloc(sizeof(tiles_t));

    /* create the tile set */
    map->tiles->data = malloc(sizeof(bitmap_t));
    map->tiles->data->width = 32;
    map->tiles->data->height= 16;
    map->tiles->data->data = malloc(32*16);
    map->tiles->tileHeight = 16;
    map->tiles->tileWidth = 16;
    map->tiles->rows = 1;
    map->tiles->cols = 2;

    i=0;
    for(y=0; y<16; y++) {
	for(x=0; x<32; x++) {
	    if(x<16)
	      map->tiles->data->data[i] = 0x00;
	    else
	      map->tiles->data->data[i] = 0x47;
	    i++;
	}
    }

    i=0;
    for(y=0; y<map->height; y++) {
        for(x=0; x<map->width; x++) {
            map->data[i] = tile;
            tile = tile ? 0 : 1;
            i++;
        }
        tile = tile ? 0 : 1;
    }
}


void
mapScrollRight(map_view_t *mv, byte offset) {
    word x, y;  /* coordinate for drawing */
    unsigned int i;

    /* increment the pixel position and update the page */
    mv->page->dx += offset;

    /* check to see if this changes the tile */
    if(mv->page->dx >= 16) {
	/* go forward one tile */
	mv->tx++;
	/* Snap the origin forward */
	mv->page->data += 4;
	mv->page->dx =0;


	/* draw the next column */
	x= SCREEN_WIDTH;
	i= mv->ty * mv->map->width + mv->tx + 20;
	for(y=0; y<240; y+=16) {
	    mapDrawTile(mv->map->tiles, mv->map->data[i], mv->page, (int)mv->page->dx + x, (int)mv->page->dy+y);
	    i += mv->map->width;
	}
    }
}


void
mapScrollLeft(map_view_t *mv, byte offest) {
}


void
mapScrollUp(map_view_t *mv, byte offset) {
}


void
mapScrollDown(map_view_t *mv, byte offset) {
}


void
mapGoTo(map_view_t *mv, byte tx, byte ty) {
    int px, py;
    unsigned int i;

    /* set up the coordinates */
    mv->tx = tx;
    mv->ty = ty;
    mv->page->dx = 0;
    mv->page->dy = 0;

    /* draw the tiles */
    modexClearRegion(mv->page, 0, 0, mv->page->width, mv->page->height, 0);
    py=0;
    i=mv->ty * mv->map->width + mv->tx;
    for(ty=mv->ty; py < SCREEN_HEIGHT && ty < mv->map->height; ty++, py+=mv->map->tiles->tileHeight) {
	px=0;
	for(tx=mv->tx; px < SCREEN_WIDTH+16 && tx < mv->map->width+1; tx++, px+=mv->map->tiles->tileWidth) {
	    mapDrawTile(mv->map->tiles, mv->map->data[i], mv->page, px, py);
	    i++;
	}
	i+=mv->map->width - tx;
    }
}


void
mapDrawTile(tiles_t *t, word i, page_t *page, word x, word y) {
    word rx;
    word ry;
    rx = (i % t->cols) * t->tileWidth;
    ry = (i / t->cols) * t->tileHeight;
    modexDrawBmpRegion(page, x, y, rx, ry, t->tileWidth, t->tileHeight, t->data);
}
