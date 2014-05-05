#include<malloc.h>
#include<string.h>

#include "sprite.h"

extern unsigned char far* dbuffer;

int sprcreate(sprite *spr, int width, int height, int frames, int trans)  {
    int i; //loop counter
    int error = 0;

    spr->x_dim = width;
    spr->y_dim = height;
    spr->numimages = frames;
    spr->state = 0;
    spr->transparent = trans;
    for(i=0; i<frames; i++)  {
        spr->images[i] = (unsigned char far*)_fmalloc(width * height);
        if(!spr->images[i])
            error = 1;
    }
    spr->background = (unsigned char far*)_fmalloc(width * height);
    if(!spr->background)
        error = 1;

    return error;
}//end sprcreate

void sprclose(sprite *spr)  {
    int i; // loop counter
    
    for(i=0; i<spr->numimages; i++)
        _ffree(spr->images[i]);
    _ffree(spr->background);
}//end sprclose

void sprload (sprite * spr)  {
    unsigned char far* dest;
    unsigned char far* source;
    int x_loc = 0, y_loc = 0;  //frame coordinates in dbuffer
    int i,j;  //loop counters
    
    for(i=0; i < spr->numimages; i++)  {
        x_loc = i * (spr->x_dim + 1);
        if (x_loc > (320 - spr->x_dim))  {
            x_loc = 0;
            y_loc += (spr->y_dim + 1);
        }//endif
        source = dbuffer + ((y_loc * 320) + x_loc);
        dest = spr->images[i];
        for(j=0; j < spr->y_dim; j++) {
            _fmemcpy(dest,source,spr->x_dim);
            source += 320;
            dest += spr->x_dim;
        }//end for
    }//end outer for
   
}//end sprload

void sprinit(sprite *spr,int x,int y,int fimage)  {
    spr->state = 1;
    spr->x_loc = x;
    spr->y_loc = y;
    spr->curimage = fimage;
    sprback(spr);
}//end sprinit

void sprback(sprite * spr)  {
   int width = spr->x_dim;
   int height = spr->y_dim;
   int xloc = spr->x_loc;
   int yloc = spr->y_loc;
   unsigned char far* source = dbuffer;
   unsigned char * dest = spr->background;
   int i;       //loop counter
   
   source += ((yloc << 8) + (yloc << 6) + xloc);
   
   for(i=0; i < height; i++) {
      puturow(dest,source,width);
      source += 320;
      dest += width;
   }//end for
}//end sprback

void sprwrite(sprite *spr)  {
    unsigned char far* dest = dbuffer;
    unsigned char * source = spr->images[spr->curimage];
    int height = spr->y_dim;
    int width = spr->x_dim;
    int i,x,y; //loop counters

    dest += ((spr->y_loc << 8) + (spr->y_loc << 6) + spr->x_loc);
 
    if(!(spr->transparent))
        for(i=0; i < height; i++) {
            _fmemcpy(dest,source,width);
            dest += 320;
            source += width;
        }//end for
    else  {
        for(y=0; y<height; y++) {
            for(x=0; x<width; x++)  {
                if((*source != 0))
                dest[x] = *source;
                source++;
            }//endfor
            dest += 320;
        }//endfor
    }//end else
}//end sprwrite

void sprerase(sprite * spr)  {
    unsigned char far* dest = dbuffer;
    unsigned char * source = spr->background;
    int height = spr->y_dim;
    int width = spr->x_dim;
    int i;  //loop counter
   
    dest += ((spr->y_loc << 8) + (spr->y_loc << 6) + spr->x_loc);
    
    
    for(i=0; i<height; i++) {
        puturow(dest,source,width);
        dest += 320;
        source += width;
    }//end for
}//end sprerase

void sprcopy(sprite *dest, sprite *source)  {
    int i; //loop counter
    
    dest->x_dim = source->x_dim;
    dest->y_dim = source->y_dim;
    dest->state = source->state;
    dest->numimages = source->numimages;
    dest->curimage = source->curimage;
    dest->transparent = source->transparent;
    for(i=0; i<source->numimages; i++)  
        dest->images[i] = source->images[i];
    dest->background = (unsigned char far*)_fmalloc(source->x_dim * source->y_dim);
}//end copy

/*

sprcopy(sprite *dest, sprite *source)  {
    int i; //loop counter
    
    dest->x_dim = source->x_dim;
    dest->y_dim = source->y_dim;
    dest->numimages = source->numimages;
    dest->curimage = source->curimage;
    dest->transparent = source->transparent;
    for(i=0; i<numimages; i++)  
        dest->images[i] = source->images[i];
    dest->background = (unsigned char far*)_fmalloc(width * height);
}//end copy

int collide(sprite * orig,sprite *check)  {
    if(orig->x_loc > check->x_loc)  {
        if((orig->x_loc - check->x_loc) >= check->x_dim)
            return 0;
    }
    else if((check->x_loc - orig->x_loc) >= orig->x_dim)
        return 0;
    if(orig->y_loc > check->y_loc)  {
        if((orig->y_loc - check->y_loc) >= check->y_dim)
            return 0;
    }
    else if((check->y_loc - orig->y_loc) >= orig->y_dim)
        return 0;
    return 1;
}//end collide






writesprclip(sprite * in,unsigned char far * dest)  {
    unsigned char far * source = in->images[in->curimage];

    int sx,sy;
    int width;
    int bx = 0,by = 0, bwidth = 0,bheight = 0;
    unsigned char pixel;

    width = in->x_dim;
    bwidth = width;
    bheight = in->y_dim;
    sx = in->x_loc;
    sy = in->y_loc;

    if(!(sx>319 || sy>199 || sx+width<1 || sy+bheight<1))  {
    if(sx<0)  {
        bx = -sx;
        sx = 0;
        bwidth -= bx;
    }
    else if(sx+width >= 319)  {
        bx = 0;
        bwidth = 319 -sx;
    }

    if(sy<0)  {
        by = -sy;
        sy = 0;
        bheight -= by;
    }
    else if(sy+bheight>199)  {
        by = 0;
        bheight = 199 - sy;
    }

    dest = dest + (sy << 8) + (sy << 6) + sx;
    in->preclip = (by * width) + bx;
    source = source + in->preclip;

    if(in->transparent)  {
        for(int y = 0; y < bheight; y++)  {
            for(int x = 0; x < bwidth; x++)  {
                if((pixel=source[x]))
                    dest[x] = pixel;
            }//end for x
            dest +=320;
            source+=width;
        }//end for y
    }//end if
    else  {
        for(int y =0; y < bheight; y++)  {
            _fmemcpy(dest,source,bwidth);
            dest += 320;
            source += width;
        }//end for y
    }//end else

    in->x_clip = sx;
    in->y_clip = sy;
    in->x_dim_clip = bwidth;
    in->y_dim_clip = bheight;
    }

}//end writesprclip

*/
