//from https://github.com/sparky4/16/commit/7872dbf5d0240f01177588bd7966c3e042ced554
#include "16/src/lib/omodex16.h"
#include "16/src/lib/bitmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
//#include "dos_kb.h"
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

//word far *clock= (word far*) 0x046C; /* 18.2hz clock */

typedef struct {
	bitmap_t *data;
	word tileHeight;
	word tileWidth;
	unsigned int rows;
	unsigned int cols;
	unsigned int tilex,tiley; // tile position on the map
} otiles_t;


typedef struct {
	byte	*data;
	otiles_t *tiles;
	int width;
	int height;
} omap_t;


typedef struct {
	omap_t *map;
	opage_t *page;
	int tx; //???? appears to be the tile position on the viewable screen map
	int ty; //???? appears to be the tile position on the viewable screen map
	word dxThresh; //????
	word dyThresh; //????
} omap_view_t;

struct {
	int tx; //player position on the viewable map
	int ty; //player position on the viewable map
} player;


omap_t allocMap(int w, int h);
void oinitMap(omap_t *map);
void omapScrollRight(omap_view_t *mv, byte offset);
void omapScrollLeft(omap_view_t *mv, byte offest);
void omapScrollUp(omap_view_t *mv, byte offset);
void omapScrollDown(omap_view_t *mv, byte offset);
void omapGoTo(omap_view_t *mv, int tx, int ty);
void omapDrawTile(otiles_t *t, word i, opage_t *page, word x, word y);
void omapDrawRow(omap_view_t *mv, int tx, int ty, word y);
void omapDrawCol(omap_view_t *mv, int tx, int ty, word x);

//#define SWAP(a, b) tmp=a; a=b; b=tmp;
void main() {
	static global_game_variables_t gvar;
//	int show1=1;
//	int tx, ty;
//	int x, y;
	//int ch=0x0;
//	byte ch;
//	int q=0;
	opage_t screen;//,screen2;
	omap_t map;
	omap_view_t mv;//, mv2;
	omap_view_t *draw;//, *show, *tmp;
	byte *ptr;

	//default player position on the viewable map
	player.tx = 10;
	player.ty = 8;

//	setkb(1);
	IN_Startup(&gvar);
	/* create the map */
	map = allocMap(40,30); //20x15 is the resolution of the screen you can make omapS smaller than 20x15 but the null space needs to be drawn properly
	oinitMap(&map);
	mv.map = &map;
//	mv2.map = &map;

	/* draw the tiles */
	ptr = map.data;
	omodexEnter(1, 1, &gvar);
	screen = omodexDefaultPage();
	screen.width += (TILEWH*2);
	mv.page = &screen;
	omapGoTo(&mv, 16, 16);
//	screen2=omodexNextPage(mv.page);
//	mv2.page = &screen2;
//	omapGoTo(&mv2, 16, 16);
//	omodexShowPage(mv.page);

	/* set up paging */
//	show = &mv;
//	draw = &mv2;
	draw = &mv;
//IN_StartAck (&gvar); while (!IN_CheckAck (&gvar)){ } omodexLeave(); IN_Shutdown(&gvar); exit(0);

	//TODO: set player position data here according to the viewable map screen thingy

	while(!gvar.in.inst->Keyboard[sc_Escape]) {
	//TODO: top left corner & bottem right corner of map veiw be set as map edge trigger since omapS are actually square
	//to stop scrolling and have the player position data move to the edge of the screen with respect to the direction
	//when player.tx or player.ty == 0 or player.tx == 20 or player.ty == 15 then stop because that is edge of map and you do not want to walk of the map
	if(gvar.in.inst->Keyboard[77]){
//		for(q=0; q<TILEWH; q++) {
		omapScrollRight(draw, 1);
//		omodexShowPage(draw->page);
//		omapScrollRight(draw, 1);
//		SWAP(draw, show);
//		}
	}

	if(gvar.in.inst->Keyboard[75]){
//		for(q=0; q<TILEWH; q++) {
 		omapScrollLeft(draw, 1);
//		omodexShowPage(draw->page);
// 		omapScrollLeft(show, 1);
//		SWAP(draw, show);
//		}
	}

	if(gvar.in.inst->Keyboard[80]){
//		for(q=0; q<TILEWH; q++) {
		omapScrollDown(draw, 1);
//		omodexShowPage(draw->page);
//		omapScrollDown(show, 1);
//		SWAP(draw, show);
//		}
	}

	if(gvar.in.inst->Keyboard[72]){
//		for(q=0; q<TILEWH; q++) {
		omapScrollUp(draw, 1);
//		omodexShowPage(draw->page);
//		omapScrollUp(show, 1);
//		SWAP(draw, show);
//		}
	}

	//keyp(ch);
	omodexShowPage(draw->page);

	}

	omodexLeave();

//	setkb(0);
	IN_Shutdown(&gvar);
}


omap_t
allocMap(int w, int h) {
	omap_t result;

	result.width =w;
	result.height=h;
	result.data = malloc(sizeof(byte) * w * h);

	return result;
}


void
oinitMap(omap_t *map) {
	/* just a place holder to fill out an alternating pattern */
	int x, y;
	int i;
	int tile = 1;
	map->tiles = malloc(sizeof(otiles_t));

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
omapScrollRight(omap_view_t *mv, byte offset) {
	word x;//, y;  /* coordinate for drawing */

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
		omapDrawCol(mv, mv->tx + 20 , mv->ty-1, x);
	}
}


void
omapScrollLeft(omap_view_t *mv, byte offset) {
	//word x, y;  /* coordinate for drawing */

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
		omapDrawCol(mv, mv->tx-1, mv->ty-1, 0);
	}
}


void
omapScrollUp(omap_view_t *mv, byte offset) {
	word /*x,*/ y;  /* coordinate for drawing */

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
		omapDrawRow(mv, mv->tx-1 , mv->ty-1, y);
	}
}


void
omapScrollDown(omap_view_t *mv, byte offset) {
	word /*x,*/ y;  /* coordinate for drawing */

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
		omapDrawRow(mv, mv->tx-1 , mv->ty+15, y);
	}

}


void
omapGoTo(omap_view_t *mv, int tx, int ty) {
	int /*px,*/ py;
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
	omodexClearRegion(mv->page, 0, 0, mv->page->width, mv->page->height, 0);
	py=0;
	i=mv->ty * mv->map->width + mv->tx;
	for(ty=mv->ty-1; py < SCREEN_HEIGHT+mv->dyThresh && ty < mv->map->height; ty++, py+=mv->map->tiles->tileHeight) {
		omapDrawRow(mv, tx-1, ty, py);
	i+=mv->map->width - tx;
	}
}


void
omapDrawTile(otiles_t *t, word i, opage_t *page, word x, word y) {
	word rx;
	word ry;
	rx = (i % t->cols) * t->tileWidth;
	ry = (i / t->cols) * t->tileHeight;
	omodexDrawBmpRegion(page, x, y, rx, ry, t->tileWidth, t->tileHeight, t->data);
}


void
omapDrawRow(omap_view_t *mv, int tx, int ty, word y) {
	word x;
	int i;

	/* the position within the map array */
	i=ty * mv->map->width + tx;
	for(x=0; x<SCREEN_WIDTH+mv->dxThresh && tx < mv->map->width; x+=mv->map->tiles->tileWidth, tx++) {
	if(i>=0) {
		/* we are in the map, so copy! */
		omapDrawTile(mv->map->tiles, mv->map->data[i], mv->page, x, y);
	}
	i++; /* next! */
	}
}


void
omapDrawCol(omap_view_t *mv, int tx, int ty, word x) {
	int y;
	int i;

	/* location in the map array */
	i=ty * mv->map->width + tx;

	/* We'll copy all of the columns in the screen,
	   i + 1 row above and one below */
	for(y=0; y<SCREEN_HEIGHT+mv->dyThresh && ty < mv->map->height; y+=mv->map->tiles->tileHeight, ty++) {
	if(i>=0) {
		/* we are in the map, so copy away! */
		omapDrawTile(mv->map->tiles, mv->map->data[i], mv->page, x, y);
	}
	i += mv->map->width;
	}
}
