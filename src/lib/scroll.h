#ifndef SCROLL_H
#define SCROLL_H

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
void mapScrollRight(map_view_t *mv, byte offset, short lp);
void mapScrollLeft(map_view_t *mv, byte offest, short lp);
void mapScrollUp(map_view_t *mv, byte offset, short lp);
void mapScrollDown(map_view_t *mv, byte offset, short lp);
void mapGoTo(map_view_t *mv, int tx, int ty);
void mapDrawTile(tiles_t *t, word i, page_t *page, word x, word y);
void mapDrawRow(map_view_t *mv, int tx, int ty, word y, word poopoffset);
void mapDrawCol(map_view_t *mv, int tx, int ty, word x, word poopoffset);
void mapDrawWRow(map_view_t *mv, int tx, int ty, word y);
void mapDrawWCol(map_view_t *mv, int tx, int ty, word x);
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
#endif
