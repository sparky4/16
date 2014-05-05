#include "vgautils.h"

void setvgamode(int mode)
{
   union REGS inregs,outregs;

   inregs.h.ah = 0;                   //set video mode subfunction
   inregs.h.al = (unsigned char)mode; //video mode to change to
   int86(0x10, &inregs, &outregs);
}//end set_graphics_mode

void write_rgbcolor_reg(int index, rgbcolor_ptr color)
{
   outp(COLOR_REGISTER_WR, index);  //set register
   outp(COLOR_DATA,color->red);
   outp(COLOR_DATA,color->green);
   outp(COLOR_DATA,color->blue);
}//end write_rgbcolor_reg

rgbcolor_ptr read_rgbcolor_reg(int index, rgbcolor_ptr color)
{
   outp(COLOR_REGISTER_RD, index);
   color->red = (unsigned char)inp(COLOR_DATA);
   color->green = (unsigned char)inp(COLOR_DATA);
   color->blue = (unsigned char)inp(COLOR_DATA);
   return color;
}//emd read_rgbcolor_reg

void read_palette(int start_reg,int end_reg,rgb_palette_ptr pal)
{
   int index;
   rgbcolor color;

   for(index = start_reg; index<=end_reg; index++)
      {
      read_rgbcolor_reg(index,(rgbcolor_ptr)&color);

      pal->colors[index].red = color.red;
      pal->colors[index].green = color.green;
      pal->colors[index].blue = color.blue;
      }//end for

   pal->start_reg = start_reg;
   pal->end_reg = end_reg;
}//end read_palette

void write_palette(int start_reg,int end_reg, rgb_palette_ptr pal)
{
   int index;

   for(index=start_reg; index <= end_reg; index++)
      write_rgbcolor_reg(index,&(pal->colors[index]));
}//end write_palette

void putchr(char out, int color,  unsigned char far* dest)  {
    unsigned char far* chrs = (unsigned char far*)ROMCHRS;
    int i,j;    //loop counters
    int mask;   //bitmask to extract bits from character set
    
    chrs += (out << 3);  //set romptr to char * 8 bytes per char

    for(i=0; i<8; i++)  {
        mask = 128;
        for(j=0; j<8; j++)  {
            if(*chrs & mask)
                dest[j] = color;
            mask = mask >> 1;
        }
        chrs++;
        dest += 320;
    }
}//end putchr
            
