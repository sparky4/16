#include "tile.h"
#include <mem.h>
#include <malloc.h>


extern unsigned char far* dbuffer;

//copy 16x16 tile to dest, loop unrolled 4 times
void putile(unsigned char far* dest, unsigned char far* source);
#pragma aux putile=     \
    "mov bx, 0"   \
    "marker:"     \
    "mov cx, 4"   \
    "rep movsb"   \
    "add di, 304" \
    "mov cx, 4"   \
    "rep movsb"   \
    "add di, 304" \
    "mov cx, 4"   \
    "rep movsb"   \
    "add di, 304" \
    "mov cx, 4"   \
    "rep movsb"   \
    "add di, 304" \
    "add bx, 1"   \
    "cmp bx, 4"   \
    "jnz marker"    \ 
    modify [cx bx]  \
    parm [es di] [ds si];

int tilecreate(tile *ntile, int frames)  {
    int i; //loop counter
    int error = 0;

    ntile->numimages = frames;
    ntile->state = 0;
  
    for(i=0; i<frames; i++)  {
        ntile->images[i] = (unsigned char far*)_fmalloc(256);
        if(!ntile->images[i])
            error = 1;
    }
    
    return error;
}//end tilecreate

void tileclose(tile *dtile)  {
    int i; // loop counter
    
    for(i=0; i<dtile->numimages; i++)
        _ffree(dtile->images[i]);
}//end sprclose

void tileload (tile * in)  {
    unsigned char far* dest;
    unsigned char far* source;
    int x_loc = 0, y_loc = 0;  //frame coordinates in dbuffer
    int i,j;  //loop counters
    
    for(i=0; i < in->numimages; i++)  {
        x_loc = i * (17);
        if (x_loc > (304))  {
            x_loc = 0;
            y_loc += (17);
        }//endif
        source = dbuffer + ((y_loc * 320) + x_loc);
        dest = in->images[i];
        for(j=0; j < 16; j++) {
            _fmemcpy(dest,source,16);
            source += 320;
            dest += 16;
        }//end for
    }//end outer for
   
}//end tileload
    
writetile(tile *out, unsigned char far* dest)  {
    unsigned char far* source;
    
    dest += ((out->y_loc << 8) + (out->y_loc << 6) + out->x_loc);
    source = out->images[out->curimage];
     
    putile(dest,source);
	return 0;
    
}//end writetile

int writetile_xclip(tile *out, unsigned char far* dest)  {
    int dx;     //clipped starting x location
    int dwidth; //clipped width of sprite
    int i;

    unsigned char far * source = out->images[out->curimage];
    

    if(out->x_loc < 0)  {
        if(out->x_loc > -16)  {
            dx = 0;
        //    preclip = - out->x_loc;
            source -= out->x_loc;
            dwidth = 16 + out->x_loc;
        }
        else
            return 0;  //tile not visible
    }
    else if(out->x_loc > 303)  {
        if(out->x_loc < 320)  {
        //    preclip = 0;
            dx = out->x_loc;
            dwidth = 320 - dx;
        }
        else
            return 0;  //tile not visible
    }
    else  {
    //    preclip = 0;
        dwidth = 16;
        dx = out->x_loc;
    }


    dest= dest + (out->y_loc << 8) + (out->y_loc << 6) + dx;  //find first pixel in dest

    for(i = 0; i < 16; i++)  {
        _fmemcpy(dest,source,dwidth);
        source += 16;
        dest += 320;
    }

    //  x_clip = dx;
    //  x_dim_clip = dwidth;

    return 1;
}//writetile_xclip

/*  not sure if this is working
***********************************************************
sprite::writetile_prexclip(unsigned char far* dest)  {
    int dwidth = x_dim_clip;
    unsigned char far * source = images[curimage];
    int i;

    dest= dest + (y_loc << 8) + (y_loc << 6) + x_clip;  //find first pixel in dest
    source += preclip;  //set source at first visible pixel

    for(i = 0; i < 16; i++)  {
        _fmemcpy(dest,source,dwidth);
        source += 16;
        dest += 320;
    }
}      
***************************************************************/    
