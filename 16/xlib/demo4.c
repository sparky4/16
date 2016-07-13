/*************************************************************************

DEMO 4

Demonstrates RLE encoding to file QUICK and DIRTY
**************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <malloc.h>
#include <dos.h>
#include "xlib.h"
#include "xvbitmap.h"
#include "xbmtools.h"
#include  "xtext.h"
#include  "xrect.h"
#include  "xrletool.h"
#include  "xfileio.h"
/* Macro to make pointer parameters model independent */
#define FARPTR(x) (MK_FP(FP_SEG(x),FP_OFF(x)))

char turtle[] = {
    20,30,
/*  |      .       |       .      |       .      |      .       */
    8,14, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    8, 2,14, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    8, 2, 2, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    8, 0, 2,14, 0, 0,14,14,14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    8, 0, 2, 5, 0, 4, 4, 4, 4,14,14, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    8, 0, 2, 2, 4, 4, 0, 4, 4, 0, 4,14, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 2, 4, 4, 4, 0, 4, 4, 0, 4, 4,14, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 2, 4, 4, 4, 0, 0, 0, 0, 4, 4,14, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 4, 0, 4, 4, 4, 0, 0, 0, 4, 0, 0, 2, 2, 0, 0, 0,
    0, 0, 0, 4, 0, 4, 4, 4, 0, 4, 4, 4, 0,14, 0, 0, 2, 2, 0, 0,
    0, 0, 0, 4, 4, 0, 0, 0, 4, 4, 0, 4, 0, 4, 2, 2, 2, 2, 0, 0,
    0, 0, 0, 4, 4, 4, 0, 4, 4, 0, 4, 4, 0, 4,14, 2, 2, 2, 0, 0,
    0, 0, 0, 4, 4, 4, 0, 0, 0, 4, 4, 0, 4, 0, 0, 2, 2, 0, 0, 0,
    0, 0, 0, 2, 4, 4, 4, 4, 4, 4, 4, 0, 4, 4,14, 2, 0, 0, 0, 0,
    0, 0, 2, 2, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 0,
    2, 2, 2, 2, 4, 0, 0, 4, 4, 0, 4, 4, 0, 4,14, 0, 0, 0, 0, 0,
    0, 2, 2, 0, 4, 4, 4, 0, 0, 4, 4, 0, 4, 0, 2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 4, 4, 0, 4, 4, 4, 0, 4, 4, 2,14, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 0, 4, 0, 2, 2,14,14, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 4, 4, 2, 2, 2, 2, 2,14, 0,
    0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 5,14, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2, 2, 0, 0, 2, 2, 2, 0, 2,14,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 2, 2, 2,14,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 2, 2, 2,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 2, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };


char far *pbm;
char far *newfnt;
int  far *buffsize;

void error(char * string){
  x_text_mode();
  printf("%s\n",string);
  exit(0);
}

void load_user_fonts(){
  FILE *f;
  int i; char c;
  f=fopen("var6x8.fnt","rb");
  newfnt = farmalloc(256*16+4);
  for (i=0;i<256*16+4;i++){
    fread(&c,1,1,f);
    *(newfnt+i)=c;
  }
  fclose(f);
  x_text_init();
  x_register_userfont(newfnt);
  x_set_font(2);
}

void main(){
    int i,j,handle,size,comp_size;
    char far * tempbuff,far * diskbuff;

    pbm = farmalloc(20 * 30 + 2);
    x_bm_to_pbm( MK_FP(FP_SEG(turtle),FP_OFF(turtle)), pbm);

    x_set_mode(X_MODE_360x240,360);
    load_user_fonts();

    x_bgprintf(0,0,0,14,0,
    "This is a demo of RLE compression of 360x240 video screen to ");
    x_bgprintf(0,8,0,14,0,
    "disk. Uncompressed, this screen requires a file of size ");
    x_bgprintf(0,16,0,14,0,"86400 (360x240) bytes.");
    x_bgprintf(0,48,0,14,0,
    "Press a key to draw the image and save it to screen:");

    getch();
    x_rect_fill(0,0,ScrnPhysicalPixelWidth,ScrnPhysicalHeight,0,0);

    for (j=0;j<(ScrnPhysicalHeight/30);j++)
      for (i=0;i<(ScrnPhysicalPixelWidth/20);i++)
	x_put_masked_pbm(i*20, j*30, 0, pbm);

    x_set_start_addr(0,240);

    x_bgprintf(0,0,240*90,14,0,
    "Now compressing and writing to file 'screen.dat'.");
    x_bgprintf(0,8,240*90,14,0,"Please wait... ");


    if((handle=f_open("screen.dat",F_WRONLY))==FILE_ERR)
      error("Unable to open 'screen.dat' for writing.");
    size=0;
    tempbuff=farmalloc(90*240);
    diskbuff=farmalloc(90*240);
    buffsize=(int far *)farmalloc(2);

    for (i=0;i<4;i++){
      x_select_default_plane(i);
      if (!(j=x_buff_RLEncode(MK_FP(0xA000,0),tempbuff,90*240)))
	error("Error buff compressing");
      *buffsize=j;
      f_writefar(handle,(char far *)buffsize,2);
      f_writefar(handle,tempbuff,j);
      size+=j;
    }
    comp_size=size;
    f_close(handle);

    if((handle=f_open("screen.raw",F_WRONLY))==FILE_ERR)
      error("Unable to open 'screen.dat' for writing.");
      for (i=0;i<4;i++){
	x_select_default_plane(i);
	f_writefar(handle,MK_FP(0xA000,0),90*240);
      }
    f_close(handle);

    x_bgprintf(0,8,240*90,14,0,"Done. Press a key to restore.");
    getch();

    x_rect_fill(0,0,ScrnPhysicalPixelWidth,ScrnPhysicalHeight,0,0);
    x_set_start_addr(0,0);

    if((handle=f_open("screen.dat",F_RDONLY))==FILE_ERR)
      error("Unable to open 'screen.dat' for reading.");

    size=0;
    for (i=0;i<4;i++){
      x_select_default_plane(i);
	f_readfar(handle,(char far *)buffsize,2);
	f_readfar(handle,diskbuff,*buffsize);
	j=x_buff_RLDecode(diskbuff,MK_FP(0xA000,0));
	size+=j;
    }
    f_close(handle);

    getch();
    x_text_mode();
    printf("screen size = 86400  compressed = %u",comp_size);
}


