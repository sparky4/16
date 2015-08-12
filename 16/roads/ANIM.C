#define ANIM_C

#include <fastgraf.h>
#include <stdlib.h>
#include "roads.h"
#include "tiles.h"

extern int far *topography;      /* BACKGROUND TILE LIST (ARRAY) */
extern int far *terrain;      /* FOREGROUND TILE LIST (ARRAY) */
extern int view_x, view_y;     /* VIEW AREA (UPPER LEFT CORNER) */
extern int viewpage;           /* CURRENTLY VIEWED PAGE */

int frogmode;
int frogwatchmode=0;
int animatemode=1;

/* ANIMATION VARIABLES */
struct ANIMATION fire, water1, water2, uranium, frog;
struct ANIMATION *anim_list[ANIM_LIST_TOTAL];

/*
 *
 * Animates all cells to the next frame.
 *
 */
void animate (void)
{
    register int x, y;
    int a, i, tile;
    long time;
    int total_updates=0;

    gogofrog();
    time=fg_getclock();

    /* UPDATE ALL ANIM TYPES FOR NEXT FRAME IF TIME TO DO SO */
    for (a=0; a<ANIM_LIST_TOTAL; a++) /* CHECK ALL ANIM TYPES */
    {
        if (anim_list[a]->next<1) /* EVENT_DRIVEN ANIMATION */
        {
            if (anim_list[a]->next==-1) /* EVENT OCCURING! */
            {
                anim_list[a]->next=0; /* TURN EVENT OFF */
                total_updates++;
            }
        }

        else if (anim_list[a]->next<=time) /* IS ANIM READY FOR NEXT FRAME? */
        {
            anim_list[a]->next=time+anim_list[a]->delay; /* SET NEXT FRAME TIME */
            anim_list[a]->current+=1;
            if (anim_list[a]->current>=anim_list[a]->first+anim_list[a]->total)
                anim_list[a]->current=anim_list[a]->first;
            total_updates++;
        }
    }

    if (total_updates==0) return; /* NO ANIMATION TODAY.  SORRY */

    /* VISUALLY UPDATE ALL VIEWABLE ANIMATIONS */
    for (y=0; y<VIEW_HEIGHT; y++)
    for (x=0; x<VIEW_WIDTH; x++)
    {
        i=VIEW_TO_WORLD(VIEW_TILE(x,y)); /* CONVERT VIEW COORDS TO WORLD */

        if (is_anim(terrain[i]))
        {
            a=0;
            while (anim_list[a]->anm!=terrain[i]) a++;
            tile=anim_list[a]->current;

                /* COPY TILE TO MIX THEN TO CURRENT SCREEN */
            pagecopy_tile_op (topography[i], MIXING_TILE, TILEPAGE, TILEPAGE);
            pagecopy_tile_tr (tile, MIXING_TILE, TILEPAGE, TILEPAGE);
            pagecopy_tile_op (MIXING_TILE, VIEW_TILE(x,y), TILEPAGE, viewpage);
        }
    }
}

#define UP    1
#define DOWN  2
#define LEFT  3
#define RIGHT 4
#define CHANCE_FROG_NOT_TURN 95

/*
 *
 * Move frog somewhere new.
 *
 */
void gogofrog (void)
{
    int fails=0, r, suc;
    int newx, newy;
    static int x, y;
    static int facing=RIGHT;
    static int walking=FALSE;
    static long nextfrog;

    if (frogmode==3) return; /* NO FROG!  GO AWAY */

    if (frogmode==2)
    {
        terrain[WORLD_TILE(x,y)]=EMPTY_TILE;
        update_tile (WORLD_TILE(x,y));
        frogmode=3;
        return;
    }

    if (frogmode==1)
    {
        if (nextfrog>fg_getclock())
            return; /* NOT TIME TO ANIMATE OUR FROGGIE */

        frog.next=-1; /* TURN ON ANIMATION EVENT FLAG */

        /* DETERMINE IF FROG CHANGES FACING */
        if (random(100)>CHANCE_FROG_NOT_TURN)
        {
            walking=FALSE;

            NEW_FACING:

            /* CHANGE FACING */
            r=random(100);
            switch (facing)
            {
                case RIGHT:
                case LEFT:
                if (r<50) facing=UP;
                else facing=DOWN;
                break;

                case DOWN:
                case UP:
                if (r<50) facing=RIGHT;
                else facing=LEFT;
                break;
            }

            /* UPDATE FACING IMAGE */
            switch (facing)
            {
                case RIGHT: frog.current=FROG_FACE_RIGHT; break;
                case LEFT:  frog.current=FROG_FACE_LEFT;  break;
                case DOWN:  frog.current=FROG_FACE_DOWN;  break;
                case UP:    frog.current=FROG_FACE_UP;    break;
            }
            nextfrog=fg_getclock()+frog.delay;
            return;
        }

        /* DETERMINE IF FROG STARTS/STOPS WALKING */
        if (walking==FALSE)
        {
            walking=TRUE;
            switch (facing)
            {
                case RIGHT: frog.current=FROG_WALK_RIGHT; break;
                case LEFT:  frog.current=FROG_WALK_LEFT;  break;
                case DOWN:  frog.current=FROG_WALK_DOWN;  break;
                case UP:    frog.current=FROG_WALK_UP;    break;
            }

            frog.next=-1; /* TURN ON ANIMATION EVENT FLAG */
            nextfrog=fg_getclock()+frog.delay;
            return;
        }

        /* DETERMINE IF WE CAN WALK OUR FROGGIE THIS WAY! */
        newx=x;
        newy=y;
        switch (facing)
        {
            case RIGHT: newx++; break;
            case LEFT:  newx--; break;
            case DOWN:  newy++; break;
            case UP:    newy--; break;
        }

        /* CAN'T MOVE -- OBSTRUCTION OR END OF WORLD! */
        if (newx<0 || newy<0 || newx>=WORLD_WIDTH || newy>=WORLD_HEIGHT
            || terrain[WORLD_TILE(newx,newy)]!=EMPTY_TILE)
        {
            /* UPDATE FACING IMAGE TO REFLECT FROG STANDS FAST */
            if (random(100)<50) switch (facing)
            {
                case RIGHT: frog.current=FROG_FACE_RIGHT; break;
                case LEFT:  frog.current=FROG_FACE_LEFT;  break;
                case DOWN:  frog.current=FROG_FACE_DOWN;  break;
                case UP:    frog.current=FROG_FACE_UP;    break;
            }
            else
            {
                frog.next=-1;
                goto NEW_FACING;
            }
            nextfrog=fg_getclock()+frog.delay;
            return;
        }

        /* CAN MOVE!  MOVE FROG ALONG */
        switch (facing)
        {
            case RIGHT:
            if (++frog.current>=FROG_NUM_WALKS+FROG_WALK_RIGHT)
                frog.current=FROG_WALK_RIGHT;
            break;

            case LEFT:
            if (++frog.current>=FROG_NUM_WALKS+FROG_WALK_LEFT)
                frog.current=FROG_WALK_LEFT;
            break;

            case DOWN:
            if (++frog.current>=FROG_NUM_WALKS+FROG_WALK_DOWN)
                frog.current=FROG_WALK_DOWN;
            break;

            case UP:
            if (++frog.current>=FROG_NUM_WALKS+FROG_WALK_UP)
                frog.current=FROG_WALK_UP;
            break;

        }

            /* DON'T MOVE FROG'S X/Y AT CERTAIN FRAMES */
        if (frog.current==FROG_WALK_UP+1 ||
            frog.current==FROG_WALK_UP+3 ||
            frog.current==FROG_WALK_LEFT+1 ||
            frog.current==FROG_WALK_LEFT+3 ||
            frog.current==FROG_WALK_RIGHT+1 ||
            frog.current==FROG_WALK_RIGHT+3 ||
            frog.current==FROG_WALK_DOWN+1 ||
            frog.current==FROG_WALK_DOWN+3 )
        {
            frog.next=-1;
            nextfrog=fg_getclock()+frog.delay;
            return;
        }

        terrain[WORLD_TILE(x,y)]=EMPTY_TILE;
        terrain[WORLD_TILE(newx,newy)]=ANM_FROG;
        frog.next=-1;

/*
 *
 * Simply put, frog watch mode doesn't work.
 * I got to the point where I said, gosh darnit, who needs
 * to watch a stinking frog anyway?  It's left as is...
 *
 */
        if (frogwatchmode)
        {

            if (newx>x)      if (view_x<WORLD_WIDTH-view_x) view_x++;
            else if (newx<x) if (view_x>0) view_x--;
            else if (newy>y) if (view_y<WORLD_HEIGHT-view_y) view_y++;
            else if (newy<y) if (view_y>0) view_y--;
            redraw (NONFLIP_REFRESH);
/*            if (newx>x)      suc=redraw (NONFLIP_SCROLL_RIGHT);
            else if (newx<x) suc=redraw (NONFLIP_SCROLL_LEFT);
            else if (newy>y) suc=redraw (NONFLIP_SCROLL_DOWN);
            else if (newy<y) suc=redraw (NONFLIP_SCROLL_UP);*/
            viewpage=!viewpage; /* SWAP PAGES */
            update_tile (WORLD_TILE(x,y));
            fg_setvpage (viewpage); /* VIEW CORRECT PAGE */
        } else update_tile (WORLD_TILE(x,y));

        x=newx;
        y=newy;
        nextfrog=fg_getclock()+frog.delay;
        return;
    }

/********************* ADD NEW FROG ************************/

    /* LOCATE VIEWABLE TILE TO PLACE FROG */
    do {
        x=random(VIEW_WIDTH)+view_x;
        y=random(VIEW_HEIGHT)+view_y;
        fails++;
    } while (terrain[WORLD_TILE(x,y)]!=EMPTY_TILE && fails<50);

    if (fails>=50) /* COULDN'T PLACE FROG */
    {
        fg_music ("L64EC.DE.C$");
        frogmode=3;
        frog.next=0;
        return;
    }

    terrain[WORLD_TILE(x,y)]=ANM_FROG; /* INSTALL FROG! */
    frog.next=-1; /* UPDATE EVENT */
    frogmode=1;
    nextfrog=fg_getclock()+frog.delay;

    return;
}

/*
 *
 * Initializes any animation information, as in at the program start.
 *
 */
void init_anim (void)
{
    fire.first=fire.current=34;
    fire.total=6;
    fire.delay=1;
    fire.next=fg_getclock()+fire.delay;
    fire.anm=ANM_FIRE;

    water1.first=water1.current=60;
    water1.total=3;
    water1.delay=3;
    water1.next=fg_getclock()+water1.delay;
    water1.anm=ANM_WATER1;

    water2.first=water2.current=63;
    water2.total=3;
    water2.delay=4;
    water2.next=fg_getclock()+water2.delay;
    water2.anm=ANM_WATER2;

    uranium.first=uranium.current=66;
    uranium.total=10;
    uranium.delay=4;
    uranium.next=fg_getclock()+uranium.delay;
    uranium.anm=ANM_URANIUM;

    frog.first=frog.current=FROG_FACE_RIGHT;
    frog.total=FROG_NUM_WALKS;
    frog.delay=3;
    frog.next=-1;
    frog.anm=ANM_FROG;

    anim_list[0]=&fire;
    anim_list[1]=&water1;
    anim_list[2]=&water2;
    anim_list[3]=&uranium;
    anim_list[4]=&frog;
}

