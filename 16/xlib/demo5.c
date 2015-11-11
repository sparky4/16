/*************************************************************************

DEMO 5

Demonstrates Planar Bitmap Clipping Functions

Themie Gouthas
**************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <malloc.h>
#include <dos.h>
#include "xlib.h"
#include "xpbitmap.h"
#include "xpbmclip.h"
#include "xrect.h"
#include "xbmtools.h"
#include "xline.h"
#include "xtext.h"
/* Macro to make pointer parameters model independent */
#define FARPTR(x) (MK_FP(FP_SEG(x),FP_OFF(x)))

int i,j;

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


char far *vbm;

void error(char * string){
  x_text_mode();
  printf("%s\n",string);
  exit(0);
}

typedef struct {
  unsigned int  size;           /* valid only for the first header */
  unsigned char ImageWidth;
  unsigned char ImageHeight;
  unsigned int  ImagePtr;
  unsigned int  MaskPtr;
} alignment_header;


void main(){
    int i,j,k,handle,size,comp_size;
    char far *bm;

    alignment_header  far * tempbuff;
    x_set_mode(X_MODE_360x240,360);
    ;NonVisual_Offs=16384;

    x_text_init();
    x_set_font(FONT_8x15);
    x_bgprintf(0,0,0,14,0," This is a demo of PBM clipping.");
    getch();

    bm=farmalloc(20*30+2);
    x_bm_to_pbm(MK_FP(_DS,turtle),bm);

    x_set_cliprect(4,5,50,150);

    x_line(0,TopClip-1,320,TopClip-1,23,0);
    x_line(0,BottomClip+1,320,BottomClip+1,23,0);
    x_line((LeftClip<<2)-1,0,(LeftClip<<2)-1,200,23,0);
    x_line((RightClip<<2)+4,0,(RightClip<<2)+4,200,23,0);

    for (k=0;k<8;k++){
     for (j=1;j<ScrnPhysicalHeight;j+=30)
      for (i=0;i<ScrnPhysicalPixelWidth-20;i+=20)
	x_put_pbm(i+k+1,j,0,bm);
    }

    x_line(0,TopClip-1,320,TopClip-1,23,0);
    x_line(0,BottomClip+1,320,BottomClip+1,23,0);
    x_line((LeftClip<<2)-1,0,(LeftClip<<2)-1,200,23,0);
    x_line((RightClip<<2)+4,0,(RightClip<<2)+4,200,23,0);

    x_rect_fill((LeftClip<<2),TopClip,(RightClip<<2)+3,BottomClip,0,0);

    x_bgprintf(0,BottomClip+4,0,14,0," Now the clipping...");
    getch();

    for (k=0;k<8;k++){
     for (j=1;j<ScrnPhysicalHeight;j+=30)
      for (i=0;i<ScrnPhysicalPixelWidth-20;i+=20)
	x_put_pbm_clipxy(i+k+1,j,0,bm);
    }
    for (k=0;k<8;k++){
     for (j=1;j<ScrnPhysicalHeight;j+=30)
      for (i=0;i<ScrnPhysicalPixelWidth-20;i+=20)
	x_put_pbm_clipxy(i+7+1,j+k,0,bm);
    }
    for (k=7;k>=0;k--){
     for (j=1;j<ScrnPhysicalHeight;j+=30)
      for (i=0;i<ScrnPhysicalPixelWidth-20;i+=20)
	x_put_pbm_clipxy(i+k+1,j+7,0,bm);
    }
    for (k=7;k>=0;k--){
     for (j=1;j<ScrnPhysicalHeight;j+=30)
      for (i=0;i<ScrnPhysicalPixelWidth-20;i+=20)
	x_put_pbm_clipxy(i+1,j+k,0,bm);
    }

    getch();
    x_text_mode();
    printf("%d %d %d %d\n",LeftClip,TopClip,RightClip,BottomClip);

}

