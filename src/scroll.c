#include "src\lib\modex16.h"
#include <stdio.h>
#include <stdlib.h>
#include "src\lib\dos_kb.h"
#include "src\lib\wtest\wtest.c"

//word far *clock= (word far*) 0x046C; /* 18.2hz clock */

typedef struct {
	bitmap_t *data;
	word tileHeight;
	word tileWidth;
	unsigned int rows;
	unsigned int cols;
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

//TODO: make this into actor_t
struct {
	int x; //player exact position on the viewable map
	int y; //player exact position on the viewable map
	int tx; //player tile position on the viewable map
	int ty; //player tile position on the viewable map
	int triggerx; //player's trigger box tile position on the viewable map
	int triggery; //player's trigger box tile position on the viewable map
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
void animatePlayer(map_view_t *src, map_view_t *dest, /*map_view_t *top, */short d1, short d2, int x, int y, int ls, int lp, bitmap_t *bmp);

#define TILEWH 16
#define QUADWH (TILEWH/4)
#define SPEED 4

//place holder definitions
#define MAPX 200
#define MAPY 150
#define TRIGGX 10
#define TRIGGY 9
//#define SWAP(a, b) tmp=a; a=b; b=tmp;
void main() {
	bitmap_t ptmp; // player sprite
	word q=1;
	const char *cpus;
	static int persist_aniframe = 0;    /* gonna be increased to 1 before being used, so 0 is ok for default */
	page_t screen, screen2, screen3;
	map_t map;
	map_view_t mv, mv2, mv3;
	map_view_t *bg, *spri, *mask;//, *tmp;
	byte *ptr;

	setkb(1);
	/* create the map */
	map = allocMap(MAPX,MAPY); //20x15 is the resolution of the screen you can make maps smaller than 20x15 but the null space needs to be drawn properly
	initMap(&map);
	mv.map = &map;
	mv2.map = &map;
	mv3.map = &map;

	/* draw the tiles */
	ptr = map.data;
	ptmp = bitmapLoadPcx("data\ptmp.pcx"); // load sprite
	modexEnter();
	modexPalUpdate(ptmp.palette);
	screen = modexDefaultPage();
	screen.width += (TILEWH*2);
	screen.height += (TILEWH*2)+QUADWH;
	mv.page = &screen;
	screen2 = modexNextPage(mv.page);
	mv2.page = &screen2;
	screen3 = screen2;
	mv3.page = &screen3;

	/* set up paging */
	bg = &mv;
	spri = &mv2;
	mask = &mv3;

//TODO: LOAD map data and position the map in the middle of the screen if smaller then screen
	mapGoTo(bg, 0, 0);
	mapGoTo(spri, 0, 0);
	//mapGoTo(mask, 0, 0);

	//TODO: put player in starting position of spot
	//default player position on the viewable map
	player.tx = bg->tx + 10;
	player.ty = bg->ty + 8;
	player.x = player.tx*TILEWH;
	player.y = player.ty*TILEWH;
	player.triggerx = player.tx;
	player.triggery = player.ty+1;
	//TODO: erase player initial draw
	modexDrawSpriteRegion(spri->page, player.x-4, player.y-TILEWH, 24, 64, 24, 32, &ptmp);
	//temp draw trigger box
	modexClearRegion(spri->page, player.triggerx*16, player.triggery*16, 16, 16, 0);
	modexClearRegion(bg->page, player.triggerx*16, player.triggery*16, 16, 16, 0);
	modexShowPage(spri->page);
	while(!keyp(1))//!keyp(1))
	{
	//top left corner & bottem right corner of map veiw be set as map edge trigger since maps are actually square
	//to stop scrolling and have the player position data move to the edge of the screen with respect to the direction
	//when player.tx or player.ty == 0 or player.tx == 20 or player.ty == 15 then stop because that is edge of map and you do not want to walk of the map

	#define INC_PER_FRAME if(q&1) persist_aniframe++; if(persist_aniframe>4) persist_aniframe = 1;	

	//temp testing
		/*if(bg->tx >= 0 && bg->tx+20 < MAPX && player.tx == bg->tx + 10)
		{
			for(q=1; q<=(TILEWH/SPEED); q++)
			{
				//INC_PER_FRAME;
				//animatePlayer(bg, spri, mask, 1, 1, player.x, player.y, persist_aniframe, q, &ptmp);
				//animatePlayer(bg, spri, 1, 1, player.x, player.y, persist_aniframe, q, &ptmp);
				mapScrollRight(bg, SPEED);
				//mapScrollRight(spri, SPEED);
				//mapScrollRight(mask, SPEED);
				modexShowPage(bg->page);
			}
			player.tx++;
		}
		else if(player.tx < MAPX)
		{
			for(q=1; q<=(TILEWH/SPEED); q++)
			{
				INC_PER_FRAME;
				player.x+=SPEED;
				//animatePlayer(bg, spri, mask, 1, 0, player.x, player.y, persist_aniframe, q, &ptmp);
				animatePlayer(bg, spri, 1, 0, player.x, player.y, persist_aniframe, q, &ptmp);
				modexShowPage(spri->page);
			}
			player.tx++;
		}
		else
		{
break;
		}*/
	if(keyp(77) && !keyp(75))
	{
		if(bg->tx >= 0 && bg->tx+20 < MAPX && player.tx == bg->tx + 10 && !(player.tx+1 == TRIGGX && player.ty == TRIGGY))
		{
			for(q=1; q<=(TILEWH/SPEED); q++)
			{
				INC_PER_FRAME;
				//animatePlayer(bg, spri, mask, 1, 1, player.x, player.y, persist_aniframe, q, &ptmp);
				animatePlayer(bg, spri, 1, 1, player.x, player.y, persist_aniframe, q, &ptmp);
				mapScrollRight(bg, SPEED);
				mapScrollRight(spri, SPEED);
				//mapScrollRight(mask, SPEED);
				modexShowPage(spri->page);
			}
			player.tx++;
		}
		else if(player.tx < MAPX && !(player.tx+1 == TRIGGX && player.ty == TRIGGY))
		{
			for(q=1; q<=(TILEWH/SPEED); q++)
			{
				INC_PER_FRAME;
				player.x+=SPEED;
				//animatePlayer(bg, spri, mask, 1, 0, player.x, player.y, persist_aniframe, q, &ptmp);
				animatePlayer(bg, spri, 1, 0, player.x, player.y, persist_aniframe, q, &ptmp);
				modexShowPage(spri->page);
			}
			player.tx++;
		}
		else
		{
			modexCopyPageRegion(spri->page, bg->page, player.x-4, player.y-TILEWH, player.x-4, player.y-TILEWH, 24, 32);
			modexDrawSpriteRegion(spri->page, player.x-4, player.y-TILEWH, 24, 32, 24, 32, &ptmp);
			modexShowPage(spri->page);
		}
		player.triggerx = player.tx+1;
		player.triggery = player.ty;
	}

	if(keyp(75) && !keyp(77))
	{
		if(bg->tx > 0 && bg->tx+20 <= MAPX && player.tx == bg->tx + 10 && !(player.tx-1 == TRIGGX && player.ty == TRIGGY))
		{
			for(q=1; q<=(TILEWH/SPEED); q++)
			{
				INC_PER_FRAME;
				//animatePlayer(bg, spri, mask, 3, 1, player.x, player.y, persist_aniframe, q, &ptmp);
				animatePlayer(bg, spri, 3, 1, player.x, player.y, persist_aniframe, q, &ptmp);
				mapScrollLeft(bg, SPEED);
				mapScrollLeft(spri, SPEED);
				//mapScrollLeft(mask, SPEED);
				modexShowPage(spri->page);
			}
			player.tx--;
		}
		else if(player.tx > 1 && !(player.tx-1 == TRIGGX && player.ty == TRIGGY))
		{
			for(q=1; q<=(TILEWH/SPEED); q++)
			{
				INC_PER_FRAME;
				player.x-=SPEED;
				//animatePlayer(bg, spri, mask, 3, 0, player.x, player.y, persist_aniframe, q, &ptmp);
				animatePlayer(bg, spri, 3, 0, player.x, player.y, persist_aniframe, q, &ptmp);
				modexShowPage(spri->page);
			}
			player.tx--;
		}
		else
		{
			modexCopyPageRegion(spri->page, bg->page, player.x-4, player.y-TILEWH, player.x-4, player.y-TILEWH, 24, 32);
			modexDrawSpriteRegion(spri->page, player.x-4, player.y-TILEWH, 24, 96, 24, 32, &ptmp);
			modexShowPage(spri->page);
		}
		player.triggerx = player.tx-1;
		player.triggery = player.ty;
	}

	if(keyp(80) && !keyp(72))
	{
		if(bg->ty >= 0 && bg->ty+15 < MAPY && player.ty == bg->ty + 8 && !(player.tx == TRIGGX && player.ty+1 == TRIGGY))
		{
			for(q=1; q<=(TILEWH/SPEED); q++)
			{
				INC_PER_FRAME;
				//animatePlayer(bg, spri, mask, 2, 1, player.x, player.y, persist_aniframe, q, &ptmp);
				animatePlayer(bg, spri, 2, 1, player.x, player.y, persist_aniframe, q, &ptmp);
				mapScrollDown(bg, SPEED);
				mapScrollDown(spri, SPEED);
				//mapScrollDown(mask, SPEED);
				modexShowPage(spri->page);
			}
			player.ty++;
		}
		else if(player.ty < MAPY && !(player.tx == TRIGGX && player.ty+1 == TRIGGY))
		{
			for(q=1; q<=(TILEWH/SPEED); q++)
			{
				INC_PER_FRAME;
				player.y+=SPEED;
				//animatePlayer(bg, spri, mask, 2, 0, player.x, player.y, persist_aniframe, q, &ptmp);
				animatePlayer(bg, spri, 2, 0, player.x, player.y, persist_aniframe, q, &ptmp);
				modexShowPage(spri->page);
			}
			player.ty++;
		}
		else
		{
			modexCopyPageRegion(spri->page, bg->page, player.x-4, player.y-TILEWH, player.x-4, player.y-TILEWH, 24, 32);
			modexDrawSpriteRegion(spri->page, player.x-4, player.y-TILEWH, 24, 64, 24, 32, &ptmp);
			modexShowPage(spri->page);
		}
		player.triggerx = player.tx;
		player.triggery = player.ty+1;
	}

	if(keyp(72) && !keyp(80))
	{
		if(bg->ty > 0 && bg->ty+15 <= MAPY && player.ty == bg->ty + 8 && !(player.tx == TRIGGX && player.ty-1 == TRIGGY))
		{
			for(q=1; q<=(TILEWH/SPEED); q++)
			{
				INC_PER_FRAME;
				//animatePlayer(bg, spri, mask, 0, 1, player.x, player.y, persist_aniframe, q, &ptmp);
				animatePlayer(bg, spri, 0, 1, player.x, player.y, persist_aniframe, q, &ptmp);
				mapScrollUp(bg, SPEED);
				mapScrollUp(spri, SPEED);
				//mapScrollUp(mask, SPEED);
				modexShowPage(spri->page);
			}
			player.ty--;
		}
		else if(player.ty > 1 && !(player.tx == TRIGGX &&  player.ty-1 == TRIGGY))
		{
			for(q=1; q<=(TILEWH/SPEED); q++)
			{
				INC_PER_FRAME;
				player.y-=SPEED;
				//animatePlayer(bg, spri, mask, 0, 0, player.x, player.y, persist_aniframe, q, &ptmp);
				modexShowPage(spri->page);
				animatePlayer(bg, spri, 0, 0, player.x, player.y, persist_aniframe, q, &ptmp);
			}
			player.ty--;
		}
		else
		{
			modexCopyPageRegion(spri->page, bg->page, player.x-4, player.y-TILEWH, player.x-4, player.y-TILEWH, 24, 32);
			modexDrawSpriteRegion(spri->page, player.x-4, player.y-TILEWH, 24, 0, 24, 32, &ptmp);
			modexShowPage(spri->page);
		}
		player.triggerx = player.tx;
		player.triggery = player.ty-1;
	}
	//modexClearRegion(mask->page, 66, 66, 2, 40, 0);

	if((player.triggerx == TRIGGX && player.triggery == TRIGGY) && keyp(KEY_ENTER))
	{
		short i;
		for(i=600; i>=400; i--)
		{
			sound(i);
		}
		nosound();
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
	printf("player.triggx: %d\n", player.triggerx);
	printf("player.triggy: %d\n", player.triggery);
	printf("dxThresh: %d\n", bg->dxThresh);
	printf("dyThresh: %d\n", bg->dyThresh);
	printf("temporary player sprite http://www.pixiv.net/member_illust.php?mode=medium&illust_id=45556867\n");
	switch(detectcpu())
	{
		case 0: cpus = "8086/8088 or 186/88"; break;
		case 1: cpus = "286"; break;
		case 2: cpus = "386 or newer"; break;
		default: cpus = "internal error"; break;
	}
	printf("detected CPU type: %s\n", cpus);
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


	/* draw the next column */
	x= SCREEN_WIDTH + mv->map->tiles->tileWidth;
		mapDrawCol(mv, mv->tx + 20 , mv->ty-1, x);
	}
}


void
mapScrollLeft(map_view_t *mv, byte offset) {
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

	/* draw the next column */
		mapDrawCol(mv, mv->tx-1, mv->ty-1, 0);
	}
}


void
mapScrollUp(map_view_t *mv, byte offset) {
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


	/* draw the next row */
	y= 0;
		mapDrawRow(mv, mv->tx-1 , mv->ty-1, y);
	}
}


void
mapScrollDown(map_view_t *mv, byte offset) {
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


	/* draw the next row */
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

	/* draw the tiles */
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
