#ifndef sprite_h
#define sprite_h

#define NUMFRAME 11    //max # of frames per sprite

typedef struct sprite_typ
    {
    int x_loc;
    int y_loc;
    int x_dim;
    int y_dim;
    short numimages;
    short curimage;
    short state;
    short transparent;
    unsigned char far * images[NUMFRAME];
    unsigned char far  * background;
    }sprite;

int sprcreate(sprite *spr, int width, int height, int frames, int trans);
void sprclose(sprite *spr);
void sprload (sprite * spr);
void sprinit(sprite *spr,int x,int y,int fimage);
void sprback(sprite * spr);
void sprwrite(sprite *spr);
void sprwrite(sprite *spr);
void sprerase(sprite *spr);
void sprcopy(sprite *dest, sprite *source);

void puturow(unsigned char far* dest, unsigned char far* source, int size);
#pragma aux puturow =          \
    "mov bx, 0x3"             \
    "and bx, cx"               \
    "shr cx, 0x2"             \
    "rep movsd"                \
    "mov cx, bx"               \
    "rep movsb"                 \
    parm [es di] [ds si] [cx]  \
    modify [bx];
#endif
