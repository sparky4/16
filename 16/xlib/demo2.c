/*************************************************************************

DEMO 2

Demonstrates the speed difference between compiled bitmap,  conventional
masked planar bitmap, and video bitmap blits.

**************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <malloc.h>
#include <dos.h>
#include "xlib.h"
#include "xcbitmap.h"
#include "xpbitmap.h"
#include "xvbitmap.h"
#include "xbmtools.h"
#include  "xtext.h"
#include  "xrect.h"
#include  "xrect.h"

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


char far *pbm, far *vbm;
char far *newfnt;

char far *
AllocatedSprite(int width, int height, int logical_width, char far * bitmap)
{
    char far * result;
    int size;

    pbm = farmalloc(width * height + 2);
    x_bm_to_pbm(bitmap,pbm);

    result = farmalloc((width * height * 7) / 2 + 25);
    if (result == 0)
	return(0);
    size = x_compile_bitmap(logical_width, bitmap, result);

    return farrealloc(result, size);
}



void load_user_fonts(){
  FILE *f;
  int i; char c;
//  f=fopen("c:\\font.fnt","rb");
  f=fopen("smalbold.fnt","rb");
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

void
main()
{
    int i,j,k;

    double ratio,ratio2;
    struct time pbm_t0,pbm_t1,cbm_t0,cbm_t1,vbm_t0,vbm_t1;
    long vbm_ticks,pbm_ticks,cbm_ticks;
    char far * TurtleSprite;

    x_set_mode(X_MODE_320x240,360);

    TurtleSprite = AllocatedSprite(20, 30, 90,FARPTR(turtle));

    vbm = x_make_vbm(MK_FP(_DS,turtle),&NonVisual_Offs);
    if (!vbm) {
      sound(1000);
      delay(100);
      nosound();
    }

    load_user_fonts();
    x_bgprintf(0,0,0,14,0,
	  "This is a demo to show the speed difference between");
    x_bgprintf(0,8,0,14,0,
	  "compiled, planar and video bitmaps.  The bitmap:");

    x_put_cbitmap(140, 18, 0, TurtleSprite);

    x_bgprintf(0,48,0,14,0,
	  "This demo uses the NON CLIPPED, MASKED versions of");
    x_bgprintf(0,56,0,14,0,
	   "       'PBM' and VBM' put functions");
    x_bgprintf(0,78,0,14,0,
	  "Press a key to start with PLANAR BITMAP demo");

    getch();
    x_rect_fill(0,0,ScrnPhysicalPixelWidth,ScrnPhysicalHeight,0,0);


    gettime(&pbm_t0);
    for (k=0;k<3;k++)
      for (j=0;j<(ScrnPhysicalHeight/30);j++)
	for (i=0;i<(ScrnPhysicalPixelWidth/20);i++)
	  x_put_masked_pbm(i*20, j*30, 0, pbm);
    gettime(&pbm_t1);
    pbm_ticks = (((long)pbm_t1.ti_hund
		+((long)pbm_t1.ti_sec*100)
		+((long)pbm_t1.ti_min*6000)) -
		(((long)pbm_t0.ti_hund
		+((long)pbm_t0.ti_sec*100)
		+((long)pbm_t0.ti_min*6000))));


    x_bgprintf(0,0,0,14,0,
	  "Press a key to start with the VIDEO BITMAP demo");

    getch();
    x_rect_fill(0,0,ScrnPhysicalPixelWidth,ScrnPhysicalHeight,0,0);


    gettime(&vbm_t0);
    for (k=0;k<3;k++)
      for (j=0;j<(ScrnPhysicalHeight/30);j++)
	for (i=0;i<(ScrnPhysicalPixelWidth/20);i++)
	  x_put_masked_vbm(i*20, j*30, 0, vbm);
    gettime(&vbm_t1);
    vbm_ticks = (((long)vbm_t1.ti_hund
		+((long)vbm_t1.ti_sec*100)
		+((long)vbm_t1.ti_min*6000)) -
		(((long)vbm_t0.ti_hund
		+((long)vbm_t0.ti_sec*100)
		+((long)vbm_t0.ti_min*6000))));




    x_bgprintf(0,0,0,14,0,
	  "Now Press a key to start with COMPILED BITMAP demo");

    getch();
    x_rect_fill(0,0,ScrnPhysicalPixelWidth,ScrnPhysicalHeight,0,0);

    gettime(&cbm_t0);
    for (k=0;k<3;k++)
      for (j=0;j<(ScrnPhysicalHeight/30);j++)
	for (i=0;i<(ScrnPhysicalPixelWidth/20);i++)
	  x_put_cbitmap(i*20, j*30, 0, TurtleSprite);
    gettime(&cbm_t1);
    cbm_ticks = (((long)cbm_t1.ti_hund
		+((long)cbm_t1.ti_sec*100)
		+((long)cbm_t1.ti_min*6000)) -
		(((long)cbm_t0.ti_hund
		+((long)cbm_t0.ti_sec*100)
		+((long)cbm_t0.ti_min*6000))));
    if (cbm_ticks==0){
      cbm_ticks=1;
      pbm_ticks+=1;
      vbm_ticks+=1;
    }
    ratio = pbm_ticks*(double)1.0/cbm_ticks;
    ratio2 = vbm_ticks*(double)1.0/cbm_ticks;

    x_set_font(1);
    x_bgprintf(0,ScrnPhysicalHeight-30,0,14,0,
	  "s/100 PBM=%ld VBM=%ld CBM=%ld - %4.1lf:%4.1lf:1",
	  pbm_ticks,vbm_ticks,cbm_ticks,ratio,ratio2);
    x_set_font(2);
    x_bgprintf(0,ScrnPhysicalHeight-16,0,14,0,
	  "Timing is rough but still,  quite impressive ! ");
    x_bgprintf(0,ScrnPhysicalHeight-8,0,14,0,
	  "Clipping slows things down considerably. ");


    getch();
    x_text_mode();
}

