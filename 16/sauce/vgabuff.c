#include<malloc.h>

#include "vgautils.h"
#include "vgabuff.h"

extern unsigned char far * dbuffer;
static FILE * fptr;  //used to hold location in graphics file

int vbinit(int height)  {
    height = height * 320;
    dbuffer = (unsigned char far*)_fmalloc(height);
    if(dbuffer)
        return 1;
    else
        return 0;
}//initbuff

vbclose()  {_ffree(dbuffer);}

int getpcximage(char fname[], int plt)
{
    int z,t;  //loop counters   
    FILE * fptr;
    rgbcolor_ptr tcolor;
    unsigned int index = 0;
    char temp;
    int run;
    
    if ((fptr = fopen(fname,"rb")) == 0)  {
        return 0;
    }//end if file not found

    if(!dbuffer)    //if buff memory not allocated ret 0
        return 0;
    
    for(z=0; z<128; z++)
       (char)getc(fptr);
    while(index < 64000)  {
        temp = (char)getc(fptr);
        if(temp>=192)   {
            run = temp - 192;
            temp = (char)getc(fptr);
            while ((run-- > 0) && (index < 64000))
                dbuffer[index++] = temp;
        }//endif
        else
            dbuffer[index++] = temp;
    }//end while

    if(plt)  {
        fseek(fptr,-768L,SEEK_END);
        for(t=0; t<256; t++)  {
            tcolor->red = (unsigned char)(getc(fptr) >> 2);
            tcolor->green = (unsigned char)(getc(fptr) >> 2);
            tcolor->blue = (unsigned char)(getc(fptr) >> 2);
            write_rgbcolor_reg(t,tcolor);
        }//end for
    }//end if
    
    fclose(fptr);
    
    return 1;
}//end getpcximage

int pcxopen(char fname[])
{
    int z;  //loop counter  
    
    if ((fptr = fopen(fname,"rb")) == 0)  {
        return 0;
    }//end if file not found

    if(!dbuffer)    //if buff memory not allocated ret 0
        return 0;
    //read past header
    for(z=0; z<128; z++)
       (char)getc(fptr);

    pcxfadvance();

    return 1;
}//end pcxopen


void pcxfadvance()
{   
    unsigned int index = 0;
    char temp;
    int run;
     
    while(index < 64000)  {
        temp = (char)getc(fptr);
        if(temp>=192)   {
            run = temp - 192;
            temp = (char)getc(fptr);
            while ((run-- > 0) && (index < 64000))
                dbuffer[index++] = temp;
        }//endif
        else
            dbuffer[index++] = temp;
    }//end while

}//end pcxfadvance

void pcxclose()
{
    int t;  //loop counters   
    rgbcolor_ptr tcolor;
    
    fseek(fptr,-768L,SEEK_END);
    for(t=0; t<256; t++)  {
        tcolor->red = (unsigned char)(getc(fptr) >> 2);
        tcolor->green = (unsigned char)(getc(fptr) >> 2);
        tcolor->blue = (unsigned char)(getc(fptr) >> 2);
        write_rgbcolor_reg(t,tcolor);
    }//end for
    
    fclose(fptr);
    
}//end pcxclose
