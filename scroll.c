#include "modex16.h"
#include <stdio.h>
#include <stdlib.h>
#include "dos_kb.h"

//word far *clock= (word far*) 0x046C; /* 18.2hz clock */

typedef struct {
	bitmap_t *data;
	word tileHeight;
	word tileWidth;
	unsigned int rows;
	unsigned int cols;
	//unsigned int tilex,tiley; // tile position on the map
} tiles_t;


typedef struct {
	byte	*data;
	tiles_t *tiles;
	int width;
	int height;
} map_t;


typedef struct {
	map_t *map;
	page_t *page;
	int tx; //appears to be the top left tile position on the viewable screen map
	int ty; //appears to be the top left tile position on the viewable screen map
	word dxThresh; //????
	word dyThresh; //????
} map_view_t;

struct {
	int x; //player exact position on the viewable map
	int y; //player exact position on the viewable map
	int tx; //player tile position on the viewable map
	int ty; //player tile position on the viewable map
	int hp; //hitpoints of the player
} player;


map_t allocMap(int w, int h);
void initMap(map_t *map);
void mapScrollRight(map_view_t *mv, byte offset);
void mapScrollLeft(map_view_t *mv, byte offest);
void mapScrollUp(map_view_t *mv, byte offset);
void mapScrollDown(map_view_t *mv, byte offset);
void mapGoTo(map_view_t *mv, int tx, int ty);
void mapDrawTile(tiles_t *t, word i, page_t *page, word x, word y);
void mapDrawRow(map_view_t *mv, int tx, int ty, word y);
void mapDrawCol(map_view_t *mv, int tx, int ty, word x);
void animatePlayer(map_view_t *src, map_view_t *dest, short d1, short d2, int x, int y, int ls, bitmap_t *bmp);

#define TILEWH 16
#define QUADWH (TILEWH/4)
#define SPEED 2

//place holder definitions
#define MAPX 40
#define MAPY 30
#define SWAP(a, b) tmp=a; a=b; b=tmp;
void main() {
	bitmap_t ptmp; // player sprite
	int q=0;
	page_t screen, screen2;
	map_t map;
	map_view_t mv, mv2;
	map_view_t *bg, *spri, *tmp;
	byte *ptr;

	setkb(1);
	/* create the map */
	map = allocMap(MAPX,MAPY); //20x15 is the resolution of the screen you can make maps smaller than 20x15 but the null space needs to be bgn properly
	initMap(&map);
	mv.map = &map;
	mv2.map = &map;

	/* bg the tiles */
	ptr = map.data;
	ptmp = bitmapLoadPcx("ptmp.pcx"); // load sprite
	modexEnter();
	modexPalUpdate(ptmp.palette);
	screen = modexDefaultPage();
	screen.width += (TILEWH*2);
	mv.page = &screen;
	screen2 = modexNextPage(mv.page);
	screen2.width += (TILEWH*2);
	mv2.page = &screen2;
	//modexShowPage(mv.page);

	/* set up paging */
	bg = &mv;
	spri = &mv2;

//TODO: LOAD map data and position the map in the middle of the screen if smaller then screen
	mapGoTo(bg, 0, 0);
	mapGoTo(spri, 0, 0);

	//TODO: put player in starting position of spot
	//default player position on the viewable map
	player.tx = bg->tx + 10;
	player.ty = bg->ty + 8;
	player.x = player.tx*TILEWH;
	player.y = player.ty*TILEWH;
	modexDrawSpriteRegion(spri->page, player.x-4, player.y-TILEWH, 24, 64, 24, 32, &ptmp);
	modexCopyPageRegion(bg->page, spri->page, player.x-4, player.y-TILEWH-2, player.x-4, player.y-TILEWH-2, 24, 36);
	modexShowPage(spri->page);
	while(!keyp(1))
	{
	//top left corner & bottem right corner of map veiw be set as map edge trigger since maps are actually square
	//to stop scrolling and have the player position data move to the edge of the screen with respect to the direction
	//when player.tx or player.ty == 0 or player.tx == 20 or player.ty == 15 then stop because that is edge of map and you do not want to walk of the map
	
	//TODO: render the player properly with animation and sprite sheet
	//TODO: fexible speeds
	if(keyp(77))
	{
		if(bg->tx >= 0 && bg->tx+20 < MAPX && player.tx == bg->tx + 10)
		{
			for(q=0; q<(TILEWH/SPEED); q++)
			{
				animatePlayer(bg, spri, 1, 1, player.x, player.y, q, &ptmp);
				mapScrollRight(bg, SPEED);
				mapScrollRight(spri, SPEED);
				modexShowPage(spri->page);
			}
			player.tx++;
		}
		else if(player.tx < MAPX)
		{
			for(q=0; q<(TILEWH/SPEED); q++)
			{
				player.x+=SPEED;
				animatePlayer(bg, spri, 1, 0, player.x, player.y, q, &ptmp);
				modexShowPage(spri->page);
			}
			player.tx++;
		}
	}

	if(keyp(75))
	{
		if(bg->tx > 0 && bg->tx+20 <= MAPX && player.tx == bg->tx + 10)
		{
			for(q=0; q<(TILEWH/SPEED); q++)
			{
				
				animatePlayer(bg, spri, 3, 1, player.x, player.y, q, &ptmp);
				mapScrollLeft(bg, SPEED);
				mapScrollLeft(spri, SPEED);
				modexShowPage(spri->page);
			}
			player.tx--;
		}
		else if(player.tx > 1)
		{
			for(q=0; q<(TILEWH/SPEED); q++)
			{
				player.x-=SPEED;
				animatePlayer(bg, spri, 3, 0, player.x, player.y, q, &ptmp);
				modexShowPage(spri->page);
			}
			player.tx--;
		}
	}

	if(keyp(80))
	{
		if(bg->ty >= 0 && bg->ty+15 < MAPY && player.ty == bg->ty + 8)
		{
			for(q=0; q<(TILEWH/SPEED); q++)
			{
				animatePlayer(bg, spri, 2, 1, player.x, player.y, q, &ptmp);
				mapScrollDown(bg, SPEED);
				mapScrollDown(spri, SPEED);
				modexShowPage(spri->page);
			}
			player.ty++;
		}
		else if(player.ty < MAPY)
		{
			for(q=0; q<(TILEWH/SPEED); q++)
			{
				player.y+=SPEED;
				animatePlayer(bg, spri, 2, 0, player.x, player.y, q, &ptmp);
				modexShowPage(spri->page);
			}
			player.ty++;
		}
	}

	if(keyp(72))
	{
		if(bg->ty > 0 && bg->ty+15 <= MAPY && player.ty == bg->ty + 8)
		{
			for(q=0; q<(TILEWH/SPEED); q++)
			{
				animatePlayer(bg, spri, 0, 1, player.x, player.y, q, &ptmp);
				mapScrollUp(bg, SPEED);
				mapScrollUp(spri, SPEED);
				modexShowPage(spri->page);
			}
			player.ty--;
		}
		else if(player.ty > 1)
		{
			for(q=0; q<(TILEWH/SPEED); q++)
			{
				player.y-=SPEED;
				animatePlayer(bg, spri, 0, 0, player.x, player.y, q, &ptmp);
				modexShowPage(spri->page);
			}
			player.ty--;
		}
	}
	
	}

	modexLeave();
	setkb(0);
	printf("Project 16 scroll.exe\n");
	printf("tx: %d\n", bg->tx);
	printf("ty: %d\n", bg->ty);
	printf("player.x: %d\n", player.x);
	printf("player.y: %d\n", player.y);
	printf("player.tx: %d\n", player.tx);
	printf("player.ty: %d\n", player.ty);
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
		  map->tiles->data->data[i] = 0x24;
		else
		  map->tiles->data->data[i] = 0x34;
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
	word x, y;  /* coordinate for bging */

	/* increment the pixel position and update the page */
	mv->page->dx += offset;

	/* check to see if this changes the tile */
	if(mv->page->dx >= mv->dxThresh ) {
	/* go forward one tile */
	mv->tx++;
	/* Snap the origin forward */
	mv->page->data += 4;
	mv->page->dx = mv->map->tiles->tileWidth;


	/* bg the next column */
	x= SCREEN_WIDTH + mv->map->tiles->tileWidth;
		mapDrawCol(mv, mv->tx + 20 , mv->ty-1, x);
	}
}


void
mapScrollLeft(map_view_t *mv, byte offset) {
	word x, y;  /* coordinate for bging */

	/* increment the pixel position and update the page */
	mv->page->dx -= offset;

	/* check to see if this changes the tile */
	if(mv->page->dx == 0) {
	/* go backward one tile */
	mv->tx--;
		
	/* Snap the origin backward */
	mv->page->data -= 4;
	mv->page->dx = mv->map->tiles->tileWidth;

	/* bg the next column */
		mapDrawCol(mv, mv->tx-1, mv->ty-1, 0);
	}
}


void
mapScrollUp(map_view_t *mv, byte offset) {
	word x, y;  /* coordinate for bging */

	/* increment the pixel position and update the page */
	mv->page->dy -= offset;

	/* check to see if this changes the tile */
	if(mv->page->dy == 0 ) {
	/* go down one tile */
	mv->ty--;
	/* Snap the origin downward */
	mv->page->data -= mv->page->width*4;
	mv->page->dy = mv->map->tiles->tileHeight;


	/* bg the next row */
	y= 0;
		mapDrawRow(mv, mv->tx-1 , mv->ty-1, y);
	}
}


void
mapScrollDown(map_view_t *mv, byte offset) {
	word x, y;  /* coordinate for bging */

	/* increment the pixel position and update the page */
	mv->page->dy += offset;

	/* check to see if this changes the tile */
	if(mv->page->dy >= mv->dyThresh ) {
	/* go down one tile */
	mv->ty++;
	/* Snap the origin downward */
	mv->page->data += mv->page->width*4;
	mv->page->dy = mv->map->tiles->tileHeight;


	/* bg the next row */
	y= SCREEN_HEIGHT + mv->map->tiles->tileHeight;
		mapDrawRow(mv, mv->tx-1 , mv->ty+15, y);
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

	/* bg the tiles */
	modexClearRegion(mv->page, 0, 0, mv->page->width, mv->page->height, 0);
	py=0;
	i=mv->ty * mv->map->width + mv->tx;
	for(ty=mv->ty-1; py < SCREEN_HEIGHT+mv->dyThresh && ty < mv->map->height; ty++, py+=mv->map->tiles->tileHeight) {
		mapDrawRow(mv, tx-1, ty, py);
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
mapDrawRow(map_view_t *mv, int tx, int ty, word y) {
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
mapDrawCol(map_view_t *mv, int tx, int ty, word x) {
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
animatePlayer(map_view_t *src, map_view_t *dest, short d1, short d2, int x, int y, int ls, bitmap_t *bmp)
{
	short dire=32*d1;
	short qq;
	short lo = ((TILEWH / SPEED) / 3);
	short loo = (ls + lo);

	if(d2==0) qq = 0;
	else qq = ((ls+1)*SPEED);
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
	}		//TODO: make flexible animation thingy
			if(ls<1) { modexCopyPageRegion(dest->page, src->page, x, y-2, x, y-2, 24, 36);// modexWaitBorder();
			modexDrawSpriteRegion(dest->page, x, y, 24, dire, 24, 32, bmp); }else
			if(4>ls && ls>=1) { modexCopyPageRegion(dest->page, src->page, x, y-2, x, y-2, 24, 36);// modexWaitBorder();
			modexDrawSpriteRegion(dest->page, x, y, 48, dire, 24, 32, bmp); }else
			if(7>ls && ls>=4) { modexCopyPageRegion(dest->page, src->page, x, y-2, x, y-2, 24, 36);// modexWaitBorder();
			modexDrawSpriteRegion(dest->page, x, y, 0, dire, 24, 32, bmp); }else
			if(8>=ls && ls>=7) { modexCopyPageRegion(dest->page, src->page, x, y-2, x, y-2, 24, 36);// modexWaitBorder();
			modexDrawSpriteRegion(dest->page, x, y, 24, dire, 24, 32, bmp); }else ls-=ls;
}
