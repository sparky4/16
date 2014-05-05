#ifndef tile_h
#define tile_h


#define NUMTIMAGE 10    // size of tile image array

// all tiles are 16x16 images with no background scanning
typedef struct tile_typ
    {
    int x_loc;
    int y_loc;
    short numimages;
    short curimage;
    short state;
    unsigned char far * images[NUMTIMAGE];
    }tile;

//FUNCTIONS

// allocate memory for a tile with images[frames]
// sets tile.state = 0;  returns 1 if memory not allocated
int tilecreate(tile *ntile, int frames);

//frees image memory used by tile
void tileclose(tile *dtile);

// reads all tile images from double buffer
// images must be in double buffer b4 called
void tileload (tile * in);

// writes tile to screen(dest points to upper left corner)
// at location (tile.x_loc,tile.y_loc)
writetile(tile *out, unsigned char far* dest);

// writes tile to screen(dest) clipping the left or right
// edge if necessary, returns 0 if tile completely off screen 
int writetile_xclip(tile *out, unsigned char far* dest);

#endif
