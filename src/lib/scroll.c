#include "src\scroll.h"
#include "src\lib\modex16.h"
#include <stdio.h>
#include <stdlib.h>
#include "src\lib\dos_kb.h"
#include "src\lib\wtest\wtest.c"

//word far *clock= (word far*) 0x046C; /* 18.2hz clock */

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
	map->tiles->data->width = (TILEWH*2);
	map->tiles->data->height= TILEWH;
	map->tiles->data->data = malloc((TILEWH*2)*TILEWH);
	map->tiles->tileHeight = TILEWH;
	map->tiles->tileWidth =TILEWH;
	map->tiles->rows = 1;
	map->tiles->cols = 2;

	i=0;
	for(y=0; y<TILEWH; y++) {
	for(x=0; x<(TILEWH*2); x++) {
		if(x<TILEWH)
		  map->tiles->data->data[i] = 28;//0x24;
		else
		  map->tiles->data->data[i] = 0;//0x34;
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
mapScrollRight(map_view_t *mv, byte offset, short lp) {
	word x, y;  /* coordinate for drawing */

	/* increment the pixel position and update the page */
	mv->page->dx += offset;

	/* check to see if this changes the tile */
	if(mv->page->dx >= mv->dxThresh ) {
	/* go forward one tile */
	mv->tx++;
	/* Snap the origin forward */
	mv->page->data += 4;
	mv->page->dx = mv->map->tiles->tileWidth;
	//}

	/* draw the next column */
	x= SCREEN_WIDTH + mv->map->tiles->tileWidth;
	if(lp%4)
		mapDrawCol(mv, mv->tx + 20 , mv->ty-1, x, mv->page->dx);
	}
}


void
mapScrollLeft(map_view_t *mv, byte offset, short lp) {
	word x, y;  /* coordinate for drawing */

	/* increment the pixel position and update the page */
	mv->page->dx -= offset;

	/* check to see if this changes the tile */
	if(mv->page->dx == 0) {
	/* go backward one tile */
	mv->tx--;
		
	/* Snap the origin backward */
	mv->page->data -= 4;
	mv->page->dx = mv->map->tiles->tileWidth;
	//}
	/* draw the next column */
	if(lp%4)
		mapDrawCol(mv, mv->tx-1, mv->ty-1, 0, mv->page->dx);
	}
}


void
mapScrollUp(map_view_t *mv, byte offset, short lp) {
	word x, y;  /* coordinate for drawing */

	/* increment the pixel position and update the page */
	mv->page->dy -= offset;

	/* check to see if this changes the tile */
	if(mv->page->dy == 0 ) {
	/* go down one tile */
	mv->ty--;
	/* Snap the origin downward */
	mv->page->data -= mv->page->width*4;
	mv->page->dy = mv->map->tiles->tileHeight;
	//}

	/* draw the next row */
	y= 0;
	if(lp%3)
		mapDrawRow(mv, mv->tx-1 , mv->ty-1, y, mv->page->dy);
	}
}


void
mapScrollDown(map_view_t *mv, byte offset, short lp) {
	word x, y;  /* coordinate for drawing */

	/* increment the pixel position and update the page */
	mv->page->dy += offset;

	/* check to see if this changes the tile */
	if(mv->page->dy >= mv->dyThresh ) {
	/* go down one tile */
	mv->ty++;
	/* Snap the origin downward */
	mv->page->data += mv->page->width*4;
	mv->page->dy = mv->map->tiles->tileHeight;
	//}

	/* draw the next row */
	y= SCREEN_HEIGHT + mv->map->tiles->tileHeight;
	if(lp%3)
		mapDrawRow(mv, mv->tx-1 , mv->ty+15, y, mv->page->dy);
	}

}


void
mapGoTo(map_view_t *mv, int tx, int ty) {
	int px, py;
	unsigned int i;

	/* set up the coordinates */
	mv->tx = tx;
	mv->ty = ty;
	mv->page->dx = mv->map->tiles->tileWidth;
	mv->page->dy = mv->map->tiles->tileHeight;

	/* set up the thresholds */
	mv->dxThresh = mv->map->tiles->tileWidth * 2;
	mv->dyThresh = mv->map->tiles->tileHeight * 2;

	/* draw the tiles */
	modexClearRegion(mv->page, 0, 0, mv->page->width, mv->page->height, 0);
	py=0;
	i=mv->ty * mv->map->width + mv->tx;
	for(ty=mv->ty-1; py < SCREEN_HEIGHT+mv->dyThresh && ty < mv->map->height; ty++, py+=mv->map->tiles->tileHeight) {
		mapDrawWRow(mv, tx-1, ty, py);
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


void 
mapDrawRow(map_view_t *mv, int tx, int ty, word y, word poopoffset) {
	word x;
	int i;
	poopoffset%=SPEED;
//printf("y: %d\n", poopoffset);
	/* the position within the map array */
	i=ty * mv->map->width + tx;
	for(x=poopoffset; x<(SCREEN_WIDTH+mv->dxThresh)/(poopoffset+1) && tx < mv->map->width; x+=mv->map->tiles->tileWidth, tx++) {
	if(i>=0) {
		/* we are in the map, so copy! */
		mapDrawTile(mv->map->tiles, mv->map->data[i], mv->page, x, y);
	}
	i++; /* next! */
	}
}


void 
mapDrawCol(map_view_t *mv, int tx, int ty, word x, word poopoffset) {
	int y;
	int i;
	poopoffset%=SPEED;
//printf("x: %d\n", poopoffset);
	/* location in the map array */
	i=ty * mv->map->width + tx;

	/* We'll copy all of the columns in the screen, 
	   i + 1 row above and one below */
	for(y=poopoffset; y<(SCREEN_HEIGHT+mv->dyThresh)/(poopoffset+1) && ty < mv->map->height; y+=mv->map->tiles->tileHeight, ty++) {
	if(i>=0) {
		/* we are in the map, so copy away! */
		mapDrawTile(mv->map->tiles, mv->map->data[i], mv->page, x, y);
	}
	i += mv->map->width;
	}
}

void 
mapDrawWRow(map_view_t *mv, int tx, int ty, word y) {
	word x;
	int i;

	/* the position within the map array */
	i=ty * mv->map->width + tx;
	for(x=0; x<SCREEN_WIDTH+mv->dxThresh && tx < mv->map->width; x+=mv->map->tiles->tileWidth, tx++) {
	if(i>=0) {
		/* we are in the map, so copy! */
		mapDrawTile(mv->map->tiles, mv->map->data[i], mv->page, x, y);
	}
	i++; /* next! */
	}
}

void 
mapDrawWCol(map_view_t *mv, int tx, int ty, word x) {
	int y;
	int i;

	/* location in the map array */
	i=ty * mv->map->width + tx;

	/* We'll copy all of the columns in the screen, 
	   i + 1 row above and one below */
	for(y=0; y<SCREEN_HEIGHT+mv->dyThresh && ty < mv->map->height; y+=mv->map->tiles->tileHeight, ty++) {
	if(i>=0) {
		/* we are in the map, so copy away! */
		mapDrawTile(mv->map->tiles, mv->map->data[i], mv->page, x, y);
	}
	i += mv->map->width;
	}
}

void
animatePlayer(map_view_t *src, map_view_t *dest, /*map_view_t *top, */short d1, short d2, int x, int y, int ls, int lp, bitmap_t *bmp)
{
	short dire=32*d1; //direction
	short qq; //scroll offset

	if(d2==0) qq = 0;
	else qq = ((lp)*SPEED);
	switch (d1)
	{
		case 0:
			//up
			x=x-4;
			y=y-qq-TILEWH;
		break;
		case 1:
			// right
			x=x+qq-4;
			y=y-TILEWH;
		break;
		case 2:
			//down
			x=x-4;
			y=y+qq-TILEWH;
		break;
		case 3:
			//left
			x=x-qq-4;
			y=y-TILEWH;
		break;
	}
	modexCopyPageRegion(dest->page, src->page, x-4, y-4, x-4, y-4, 28, 40);
	if(2>ls && ls>=1) { modexDrawSpriteRegion(dest->page, x, y, 48, dire, 24, 32, bmp); }else
	if(3>ls && ls>=2) { modexDrawSpriteRegion(dest->page, x, y, 24, dire, 24, 32, bmp); }else
	if(4>ls && ls>=3) { modexDrawSpriteRegion(dest->page, x, y, 0, dire, 24, 32, bmp); }else
	if(5>ls && ls>=4) { modexDrawSpriteRegion(dest->page, x, y, 24, dire, 24, 32, bmp); }
	//TODO: mask copy //modexCopyPageRegion(dest->page, src->page, x-4, y-4, x-4, y-4, 28, 40);
	//modexClearRegion(top->page, 66, 66, 2, 40, 0);
	//modexCopyPageRegion(dest->page, top->page, 66, 66, 66, 66, 2, 40);
	//turn this off if XT
	if(detectcpu() > 0) modexWaitBorder();
}
