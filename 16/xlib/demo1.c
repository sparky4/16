/* VERY QUICK AND ULTRA-DIRTY DEMO USING XLIB */

/* Simple Demo of MODE X Split screen and panning  */
/* Compile using Turbo C and Tasm                  */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <malloc.h>
#include <dos.h>
#include "xlib_all.h"

#define MAX_OBJECTS  10

static char *texttest[6] =
 {"This is a demonstration of the fonts ",
  "available in XLIB. Notice fixed and  ",
  "variabe spaced fonts are supported but",
  "are limited to a maximum of 8 pixels in",
  "width. Height of the characters is    ",
  "ofcourse unlimited..."};

typedef struct {
   int X,Y,Width,Height,XDir,YDir,XOtherPage,YOtherPage;
   char far * Image;
   char far * bg;
   char far * bgOtherPage;
} AnimatedObject;

AnimatedObject objects[MAX_OBJECTS];
int object_count=0;

static char  bm[] = {4,12,
  /* plane 0 */
  2,2,2,2,2,1,1,1,2,1,1,1,2,3,3,1,
  2,0,0,3,2,0,0,3,2,0,0,3,2,0,0,3,
  2,3,3,1,2,1,1,1,2,1,1,1,2,2,2,2,
  /* plane 1 */
  2,2,2,2,1,1,1,1,1,1,1,1,1,3,3,1,
  1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,
  1,3,3,1,1,1,1,1,1,1,1,1,2,2,2,2,
  /* plane 2 */
  2,2,2,2,1,1,1,1,1,1,1,1,1,3,3,1,
  1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,
  1,3,3,1,1,1,1,1,1,1,1,1,2,2,2,2,
  /* plane 3 */
  2,2,2,2,1,1,1,2,1,1,1,2,1,3,3,2,
  3,0,0,2,3,0,0,2,3,0,0,2,3,0,0,2,
  1,3,3,2,1,1,1,2,1,1,1,2,2,2,2,2};

static char  bm2[] = {4,12,
   /* plane 0 */
   2,2,2,2,2,4,4,4,2,4,4,4,2,2,2,4,
   2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,
   2,2,2,4,2,4,4,4,2,4,4,4,2,2,2,2,
   /* plane 1 */
   2,2,2,2,4,4,4,4,4,4,4,4,4,2,2,4,
   4,0,0,4,4,0,0,4,4,0,0,4,4,0,0,4,
   4,2,2,4,4,4,4,4,4,4,4,4,2,2,2,2,
   /* plane 2 */
   2,2,2,2,4,4,4,4,4,4,4,4,4,2,2,4,
   4,0,0,4,4,0,0,4,4,0,0,4,4,0,0,4,
   4,2,2,4,4,4,4,4,4,4,4,4,2,2,2,2,
   /* plane 2 */
   2,2,2,2,4,4,4,2,4,4,4,2,4,2,2,2,
   2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,
   4,2,2,2,4,4,4,2,4,4,4,2,2,2,2,2};

int textwindow_x=0,textwindow_y=0;
char far * pal,far * pal2;
char palscrolldir=1;
char far * userfnt1;



void drawtext(int height){
  int i;
  for (i=0;i<6;i++)
    x_printf(textwindow_x+5,50+i*(height+2),VisiblePageOffs,9,texttest[i]);
}

/* initialize a new object */
void init_object(int x,int y,int width, int height, int xdir, int ydir,
  char far * image){
  int i;
  objects[object_count].X = objects[object_count].XOtherPage = x;
  objects[object_count].Y = objects[object_count].YOtherPage = y;
  objects[object_count].Width = width;
  objects[object_count].Height = height;
  objects[object_count].XDir = xdir;
  objects[object_count].YDir = ydir;
  objects[object_count].Image = image;
  objects[object_count].bg = (char far *) farmalloc(4*width*height+20);
  objects[object_count].bgOtherPage = (char far *) farmalloc(4*width*height+20);
  x_get_pbm(x,y,(unsigned)width,height,VisiblePageOffs,
	objects[object_count].bg);
  x_get_pbm(x,y,(unsigned)width,height,HiddenPageOffs,
	objects[object_count].bgOtherPage);
  object_count++;
}

/* Move the specified object, bouncing at the edges of the screen and
   remembering where the object was before the move for erasing next time */
void MoveObject(AnimatedObject * ObjectToMove) {
   int X, Y;
   char far *cptr;
   X = ObjectToMove->X + ObjectToMove->XDir;
   Y = ObjectToMove->Y + ObjectToMove->YDir;
   if ((X < 0) || (X > (ScrnLogicalPixelWidth-((ObjectToMove->Width)<<2)))) {
	  ObjectToMove->XDir = -ObjectToMove->XDir;
	  X = ObjectToMove->X + ObjectToMove->XDir;
   }
   if ((Y < 0) || (Y > (ScrnLogicalHeight-ObjectToMove->Height))) {
	  ObjectToMove->YDir = -ObjectToMove->YDir;
	  Y = ObjectToMove->Y + ObjectToMove->YDir;
   }
   /* Remember previous location for erasing purposes */
   ObjectToMove->XOtherPage = ObjectToMove->X;
   ObjectToMove->YOtherPage = ObjectToMove->Y;
   ObjectToMove->X = X; /* set new location */
   ObjectToMove->Y = Y;
   cptr = ObjectToMove->bg;
   ObjectToMove->bg = ObjectToMove->bgOtherPage;
   ObjectToMove->bgOtherPage = cptr;
}

void animate(void){
 int i;
 for(i=object_count-1;i>=0;i--){
  x_put_pbm(objects[i].XOtherPage,objects[i].YOtherPage,
	 HiddenPageOffs,objects[i].bgOtherPage);
 }
 for(i=0;i<object_count;i++){
  MoveObject(&objects[i]);

  x_get_pbm(objects[i].X,objects[i].Y,
	(unsigned)objects[i].Width,objects[i].Height,HiddenPageOffs,
	objects[i].bg);
  x_put_masked_pbm(objects[i].X,objects[i].Y,HiddenPageOffs,
	objects[i].Image);
 }
}

void clear_objects(void){
 int i;
 for(i=object_count-1;i>=0;i--){
  x_put_pbm(objects[i].XOtherPage,objects[i].YOtherPage,
	 HiddenPageOffs,objects[i].bgOtherPage);
 }
}


void textwindow(int Margin){
   int x0=0+Margin;
   int y0=0+Margin;
   int x1=ScrnPhysicalPixelWidth-Margin;
   int y1=ScrnPhysicalHeight-Margin;
   x_rect_fill(x0, y0, x1,y1,VisiblePageOffs,1);
   x_line(x0,y0,x1,y0,2,VisiblePageOffs);
   x_line(x0,y1,x1,y1,2,VisiblePageOffs);
   x_line(x0,y0,x0,y1,2,VisiblePageOffs);
   x_line(x1,y0,x1,y1,2,VisiblePageOffs);
   x_line(x0+2,y0+2,x1-2,y0+2,2,VisiblePageOffs);
   x_line(x0+2,y1-2,x1-2,y1-2,2,VisiblePageOffs);
   x_line(x0+2,y0+2,x0+2,y1-2,2,VisiblePageOffs);
   x_line(x1-2,y0+2,x1-2,y1-2,2,VisiblePageOffs);
   textwindow_x=x0;
   textwindow_y=y0;

}


void wait_for_keypress(void){
  x_show_mouse();
  while(kbhit()) getch();
  palscrolldir^=1;

  do {
	x_rot_pal_struc(pal,palscrolldir);
	MouseFrozen=1;
	x_put_pal_struc(pal);
	x_update_mouse();
  } while (!kbhit() && !(MouseButtonStatus==LEFT_PRESSED));
  while(MouseButtonStatus==LEFT_PRESSED);
  while(kbhit()) getch();

}


void exitfunc(void){
  x_mouse_remove();
  x_remove_vsync_handler();
  x_text_mode();
  printf("Thanks to everyone who assisted in the development of XLIB.\n");
  printf("\nSpecial thanks to Matthew Mackenzie for contributing \n");
  printf("lots of code, documentation and ideas.\n\n");
  printf("If you make any money using this code and you're the generous\n");
  printf("type please send us some, or at least a copy of your program!\n");
}

int terminate(void){
  exit(0);
}

void intro_1(void){
  x_set_rgb(1,40,40,40); /* BG Gray */
  x_set_rgb(2,63,63,0);  /* Bright Yellow  */
  x_set_rgb(3,63,0,0);   /* Bright Red     */
  x_set_rgb(4,0,63,0);   /* Bright Green   */
  x_set_rgb(5,0,0,63);   /* Bright Blue    */
  x_set_rgb(6,0,0,28);   /* Dark Blue      */
  x_set_rgb(7,0,28,0);   /* Dark Green     */
  x_set_rgb(8,28,0,0);   /* Dark red       */
  x_set_rgb(9,0,0,38);   /* Med Blue       */

  textwindow(20);
  x_set_font(1);
  x_printf(textwindow_x+54,textwindow_y+4,VisiblePageOffs,6,"     XLIB Version 6.0");
  x_printf(textwindow_x+53,textwindow_y+3,VisiblePageOffs,2,"     XLIB Version 6.0");
  x_set_font(0);
  x_printf(textwindow_x+24,textwindow_y+18,VisiblePageOffs,6,"       Not the Unix version");
  x_printf(textwindow_x+23,textwindow_y+17,VisiblePageOffs,2,"       Not the Unix version");

  x_printf(textwindow_x+24,168,VisiblePageOffs,6,"     Press any key to continue");
  x_printf(textwindow_x+23,167,VisiblePageOffs,2,"     Press any key to continue");
}

void subsequent_page(void){
  x_hide_mouse();
  textwindow(20);
  x_set_font(1);
  x_printf(textwindow_x+54,textwindow_y+4,VisiblePageOffs,6,"     XLIB Version 6.0");
  x_printf(textwindow_x+53,textwindow_y+3,VisiblePageOffs,2,"     XLIB Version 6.0");
  x_set_font(0);
  x_printf(textwindow_x+24,168,VisiblePageOffs,6,"     Press any key to continue");
  x_printf(textwindow_x+23,167,VisiblePageOffs,2,"     Press any key to continue");
}

void load_user_fonts(void){
  FILE *f;
  f=fopen("var6x8.fnt","rb");
  /* read char by char as fread wont read to far pointers in small model */
  { int i; char c;
	for (i=0;i<256*8+4;i++){
	  fread(&c,1,1,f);
	  *(userfnt1+i)=c;
	}
  }

  fclose(f);

  x_register_userfont(userfnt1);

}



void main(){
  int  i, j, xinc, yinc, Margin;
  char ch;
  WORD curr_x=0, curr_y=0;

  pal    = (char far *) farmalloc(256*3);
  pal2   = (char far *) farmalloc(256*3);
  userfnt1 = (char far *) farmalloc(256*16+4);


  /* INITIALIZE XLIB */

  /* we set up Mode X 360x200x256 with a logical width of ~ 500 */
  /* pixels; we actually get 496 due to the fact that the width */
  /* must be divisible by 8                                     */

  x_text_mode(); /* make sure VGA is in color mode, if possible */
  x_set_mode(X_MODE_360x200,500);           /* actually is set to 496      */
  x_install_vsync_handler(2);
  x_set_splitscreen(ScrnPhysicalHeight-60); /* split screen 60 pixels high */
  x_set_doublebuffer(220);
  x_text_init();
  x_hide_splitscreen();
  x_mouse_init();
  MouseColor=2;
  atexit(exitfunc);

  /* DRAW BACKGROUND LINES */

  for(j=0;j<ScrnPhysicalHeight;j++){
   x_line(0,j,ScrnLogicalPixelWidth,j,16+(j%239),VisiblePageOffs);
  }

  ctrlbrk(terminate);
  x_get_pal_struc(pal, 240,16);
  load_user_fonts();

  intro_1();
  x_set_font(2);
  x_hide_mouse();
  x_printf(textwindow_x+5,50   ,VisiblePageOffs,9, "   Hi, folks. This is yet another FREEWARE Mode X");
  x_printf(textwindow_x+5,50+8 ,VisiblePageOffs,9, " graphics library. It is by no means complete,");
  x_printf(textwindow_x+5,50+16,VisiblePageOffs,9, " but I believe it contains a rich enough set of");
  x_printf(textwindow_x+5,50+24,VisiblePageOffs,9, " functions to achieve its design goal - to be");
  x_printf(textwindow_x+5,50+32,VisiblePageOffs,9, " a game development oriented library for");
  x_printf(textwindow_x+5,50+40,VisiblePageOffs,9, " Borland TC/BC/BC++ and TASM programmers.");

  x_printf(textwindow_x+5,50+48,VisiblePageOffs,9, "   This library comes with TASM and C sources.");
  x_printf(textwindow_x+5,50+56,VisiblePageOffs,9, " It was inspired by the DDJ Graphics column and");
  x_printf(textwindow_x+5,50+64,VisiblePageOffs,9, " many INTERNET and USENET authors who, unlike the");
  x_printf(textwindow_x+5,50+72,VisiblePageOffs,9, " majority of programmers (you know who you are!),");
  x_printf(textwindow_x+5,50+80,VisiblePageOffs,9, " willingly share their code and ideas with others.");

  x_printf(textwindow_x+5,50+88,VisiblePageOffs,9, "   I can't afford, nor do I want, to copyright");
  x_printf(textwindow_x+5,50+96,VisiblePageOffs,9, " this code - but if you use it, some credit would ");
  x_printf(textwindow_x+5,50+104,VisiblePageOffs,9," be appreciated. ");

  wait_for_keypress();

  subsequent_page();
  x_set_font(0);
  x_printf(textwindow_x+24,textwindow_y+18,VisiblePageOffs,6,"Supported 256 colour resolutions.");
  x_printf(textwindow_x+23,textwindow_y+17,VisiblePageOffs,3,"Supported 256 colour resolutions.");
  x_set_font(2);
  x_printf(textwindow_x+5,50   ,VisiblePageOffs,9, " 320x200   Standard for games       ~ 4 pages");
  x_printf(textwindow_x+5,50+8 ,VisiblePageOffs,9, " 320x240   DDJ Mode X square pixels ~ 3.5 pages");
  x_printf(textwindow_x+5,50+16,VisiblePageOffs,9, " 360x200   My favourite for games   ~ 3 pages  ");
  x_printf(textwindow_x+5,50+24,VisiblePageOffs,9, " 360x240                            ~ 2.8 pages");
  x_printf(textwindow_x+5,50+32,VisiblePageOffs,9, " 320x400                            ~ 2 pages  ");
  x_printf(textwindow_x+5,50+40,VisiblePageOffs,9, " 320x480   All subsequent modes support");
  x_printf(textwindow_x+5,50+48,VisiblePageOffs,9, " 360x400     less than two pages.");
  x_printf(textwindow_x+5,50+56,VisiblePageOffs,9, " 360x480");
  x_printf(textwindow_x+5,50+64,VisiblePageOffs,9, " 376x282,360x360,376x308,376x564,256x200,256x240");
  x_printf(textwindow_x+5,50+72,VisiblePageOffs,9, " Phew! and they'll run on all VGA cards and ");
  x_printf(textwindow_x+5,50+80,VisiblePageOffs,9, " monitors (some of the weird ones are best suited");
  x_printf(textwindow_x+5,50+88,VisiblePageOffs,9, " to monitors with both vert & horiz adjustments)");
  x_printf(textwindow_x+5,50+98,VisiblePageOffs,2, "  ");
  x_printf(textwindow_x+5,50+106,VisiblePageOffs,2," Overkill? Maybe!! ");


  wait_for_keypress();

  subsequent_page();
  x_printf(textwindow_x+24,textwindow_y+18,VisiblePageOffs,6,"      Text display functions.");
  x_printf(textwindow_x+23,textwindow_y+17,VisiblePageOffs,3,"      Text display functions.");
  x_set_font(2);
  x_printf(textwindow_x+5,50   ,VisiblePageOffs,9, "   Several text printing functions are provided.");
  x_printf(textwindow_x+5,50+8 ,VisiblePageOffs,9, " They support the VGA ROM 8x14 and 8x8 fonts as");
  x_printf(textwindow_x+5,50+16,VisiblePageOffs,9, " well as user-defined fonts (like this 6x8 font).");
  x_printf(textwindow_x+5,50+24,VisiblePageOffs,9, " Furthermore, a function similar to printf is");
  x_printf(textwindow_x+5,50+32,VisiblePageOffs,9, " included which provides formatted text output.");
  x_printf(textwindow_x+5,50+40,VisiblePageOffs,9, " User defined fonts may be proportionally spaced");
  x_printf(textwindow_x+5,50+58,VisiblePageOffs,9, " but have a maximum width of 8 pixels.");


  wait_for_keypress();

  subsequent_page();
  x_printf(textwindow_x+24,textwindow_y+18,VisiblePageOffs,6,"    Advanced screen functions.");
  x_printf(textwindow_x+23,textwindow_y+17,VisiblePageOffs,3,"    Advanced screen functions.");
  x_set_font(2);
  x_printf(textwindow_x+5,50   ,VisiblePageOffs,9, "   The library supports virtual screens larger");
  x_printf(textwindow_x+5,50+8 ,VisiblePageOffs,9, " than the physical screen, panning of such");
  x_printf(textwindow_x+5,50+16,VisiblePageOffs,9, " screens, and a split screen option.");
  x_printf(textwindow_x+5,50+24,VisiblePageOffs,9, "   These functions can be used together or");
  x_printf(textwindow_x+5,50+32,VisiblePageOffs,9, " in isolation, and in the lower resolutions");
  x_printf(textwindow_x+5,50+40,VisiblePageOffs,9, " double buffering can also be accomplished.");

  x_rect_fill(0, 0, ScrnPhysicalPixelWidth,60,SplitScrnOffs,5);
  x_line(0,0,ScrnPhysicalPixelWidth,0,2,SplitScrnOffs);
  x_set_font(1);
  x_printf(10,10,SplitScrnOffs,2, " This is a split screen, tops for scores.");
  x_set_font(0);
  for (i=ScrnPhysicalHeight;i>ScrnPhysicalHeight-60;i--){
	x_adjust_splitscreen(i);
  }
  x_printf(10,25,SplitScrnOffs,2, " Even better for scrolling games etc.");

  x_cp_vid_rect(0,0,ScrnLogicalPixelWidth,ScrnLogicalHeight,0,0,
		VisiblePageOffs,HiddenPageOffs,
		ScrnLogicalPixelWidth,ScrnLogicalPixelWidth);


  x_show_mouse();
  wait_for_keypress();

  curr_x=curr_y=0;


  init_object(60,90,4, 12, -1, 1, MK_FP(FP_SEG(bm2),FP_OFF(bm2)));
  init_object(30,30,4, 12, 1, 1, MK_FP(FP_SEG(bm),FP_OFF(bm)));
  init_object(80,120,4, 12, 2, 1, MK_FP(FP_SEG(bm),FP_OFF(bm)));
  init_object(300,200,4, 12, 1, -2, MK_FP(FP_SEG(bm),FP_OFF(bm)));
  init_object(360,30,4, 12, -1, -1, MK_FP(FP_SEG(bm),FP_OFF(bm)));
  init_object(360,10,4, 12, -2, 2, MK_FP(FP_SEG(bm),FP_OFF(bm)));

  x_hide_mouse();

  while (!kbhit()&& !(MouseButtonStatus==LEFT_PRESSED)){
	animate();
	if (objects[0].X>=curr_x+ScrnPhysicalPixelWidth-32 &&
	curr_x < MaxScrollX) curr_x++;
	else if (objects[0].X < curr_x+16 && curr_x > 0) curr_x--;
	if (objects[0].Y>=curr_y+ScrnPhysicalHeight-92 &&
	   curr_y < MaxScrollY) curr_y++;
	else if (objects[0].Y < curr_y+16 && curr_y > 0) curr_y--;
	x_page_flip(curr_x,curr_y);
	while(StartAddressFlag);
  }
  while(MouseButtonStatus==LEFT_PRESSED);
  while(kbhit()) getch();

  clear_objects();
  x_page_flip(curr_x,curr_y);
  while(StartAddressFlag);


  x_set_start_addr(0,0);


  for (j=0;j<4;j++){
	x_hide_splitscreen();
	delay(100);
	x_show_splitscreen();
	delay(100);
  }


  for (i=ScrnPhysicalHeight-60;i<=ScrnPhysicalHeight;i++){
	x_adjust_splitscreen(i);
  }

  x_hide_mouse();
  subsequent_page();
  x_printf(textwindow_x+24,textwindow_y+18,VisiblePageOffs,6,"        Palette functions.");
  x_printf(textwindow_x+23,textwindow_y+17,VisiblePageOffs,3,"        Palette functions.");
  x_set_font(2);
  x_printf(textwindow_x+5,50   ,VisiblePageOffs,9, "   A number of palette manipulation functions");
  x_printf(textwindow_x+5,50+8 ,VisiblePageOffs,9, " are provided. You have already seen some of");
  x_printf(textwindow_x+5,50+16,VisiblePageOffs,9, " them in action. Another common operation is");
  x_printf(textwindow_x+5,50+24,VisiblePageOffs,9, " palette fading.                     ");

  i=0;
  ch=255;
  while (x_cpcontrast_pal_struc(pal, pal2,ch-=2)){
	x_put_pal_struc(pal2);
	x_rot_pal_struc(pal,palscrolldir);
	i++;
  };
  for (j=0;j<i;j++){
	x_cpcontrast_pal_struc(pal, pal2,ch+=2);
	x_put_pal_struc(pal2);
	x_rot_pal_struc(pal,palscrolldir);
  };
  wait_for_keypress();

  subsequent_page();
  x_printf(textwindow_x+24,textwindow_y+18,VisiblePageOffs,6,"    NEW Version 3.0 Functions!");
  x_printf(textwindow_x+23,textwindow_y+17,VisiblePageOffs,3,"    NEW Version 3.0 Functions!");
  x_set_font(2);
  x_printf(textwindow_x+5,50   ,VisiblePageOffs,9, " NEW functions not demonstrated here include:");
  x_printf(textwindow_x+5,50+10,VisiblePageOffs,9, "  - RLE data compression");
  x_printf(textwindow_x+5,50+20,VisiblePageOffs,9, "  - FAST compiled masked bitmaps");
  x_printf(textwindow_x+5,50+30,VisiblePageOffs,9, "  - Hardware detection");

  x_show_mouse();
  wait_for_keypress();

  x_hide_mouse();
  for (i = 0; i < 150; i++) {
	  x_circle(0, 0, i, 181 - i, VisiblePageOffs);
	  x_circle(360 - i, 0, i, i + 30, VisiblePageOffs);
	  x_circle(0, 200 - i, i, i + 30, VisiblePageOffs);
	  x_circle(360 - i, 200 - i, i, 181 - i, VisiblePageOffs);
  }
  for (i = 0; i < 100; i++)
	x_filled_circle(80 + i, i, 201 - (i << 1), 30+i, VisiblePageOffs);
  x_show_mouse();
  wait_for_keypress();

  subsequent_page();
  x_printf(textwindow_x+24,textwindow_y+18,VisiblePageOffs,6,"    NEW Version 4.0 Functions!");
  x_printf(textwindow_x+23,textwindow_y+17,VisiblePageOffs,3,"    NEW Version 4.0 Functions!");
  x_set_font(2);
  x_printf(textwindow_x+5,50   ,VisiblePageOffs,9, " NEW functions not demonstrated here include:");
  x_printf(textwindow_x+5,50+10,VisiblePageOffs,9, "  - FAST VRAM-based masked bitmaps, including");
  x_printf(textwindow_x+5,50+18,VisiblePageOffs,9, "      support for clipping regions");
  x_printf(textwindow_x+5,50+28,VisiblePageOffs,9, "  - Faster, smaller compiled bitmaps");
  x_printf(textwindow_x+5,50+38,VisiblePageOffs,9, "  - Improved planar bitmap performance and");
  x_printf(textwindow_x+5,50+46,VisiblePageOffs,9, "      additional support for clipping");
  x_printf(textwindow_x+5,50+56,VisiblePageOffs,9, "  - mouse module");
  x_printf(textwindow_x+5,50+66,VisiblePageOffs,9, "  - Detection of math co-processor and mouse");
  x_printf(textwindow_x+5,50+76,VisiblePageOffs,9, "  - Bezier curve module");
  x_printf(textwindow_x+5,50+86,VisiblePageOffs,9, "  - Four new resolutions, including one with");
  x_printf(textwindow_x+5,50+94,VisiblePageOffs,9, "      square pixels (376x282)");
  x_printf(textwindow_x+5,50+104,VisiblePageOffs,9, "  - More bug fixes");

  wait_for_keypress();

  subsequent_page();
  x_printf(textwindow_x+24,textwindow_y+18,VisiblePageOffs,6,"    NEW Version 5.0 Functions!");
  x_printf(textwindow_x+23,textwindow_y+17,VisiblePageOffs,3,"    NEW Version 5.0 Functions!");
  x_set_font(2);
  x_printf(textwindow_x+5,50   ,VisiblePageOffs,9, " - *FAST* filled and clipped triangles ideal for");
  x_printf(textwindow_x+5,50+10,VisiblePageOffs,9, "   3D work. Thanks to S. Dollins for the code.");
  x_printf(textwindow_x+5,50+20,VisiblePageOffs,9, " - Filled and clipped polygons, C++ Compatible");
  x_printf(textwindow_x+5,50+30,VisiblePageOffs,9, " - header files, and of course bug fixes!");

  x_show_mouse();
  wait_for_keypress();

  subsequent_page();
  x_printf(textwindow_x+24,textwindow_y+18,VisiblePageOffs,6,"    NEW Version 6.0 Functions!");
  x_printf(textwindow_x+23,textwindow_y+17,VisiblePageOffs,3,"    NEW Version 6.0 Functions!");
  x_set_font(2);
  x_printf(textwindow_x+5,50   ,VisiblePageOffs,9, " - Fast flood filling functions ");
  x_printf(textwindow_x+5,50+10,VisiblePageOffs,9, " - New pbm flipping put functions.");
  x_printf(textwindow_x+5,50+20,VisiblePageOffs,9, " - Timer synchronized vertical retrace");
  x_printf(textwindow_x+5,50+30,VisiblePageOffs,9, " - Tripple buffering extensions");
  x_printf(textwindow_x+5,50+40,VisiblePageOffs,9, " Checkout demo 9 and 10 for previews");
  


  x_show_mouse();
  wait_for_keypress();


  randomize();
  x_hide_mouse();
  while(kbhit()) getch();
  palscrolldir^=1;
  do {
	int x0,x1,x2,y0,y1,y2,i;
	i=random(256);
	x0=random(ScrnLogicalPixelWidth);
	x1=random(ScrnLogicalPixelWidth);
	x2=random(ScrnLogicalPixelWidth);
	y0=random(ScrnPhysicalHeight);
	y1=random(ScrnPhysicalHeight);
	y2=random(ScrnPhysicalHeight);
	x_triangle(x0,y0,x1,y1,x2,y2,i,VisiblePageOffs);
  } while (!kbhit() && !(MouseButtonStatus==LEFT_PRESSED));
  while(MouseButtonStatus==LEFT_PRESSED);
  while(kbhit()) getch();
  x_show_mouse();

  subsequent_page();
  x_printf(textwindow_x+24,textwindow_y+18,VisiblePageOffs,6,"             PLEASE...");
  x_printf(textwindow_x+23,textwindow_y+17,VisiblePageOffs,3,"             PLEASE...");
  x_set_font(2);
  x_printf(textwindow_x+5,50   ,VisiblePageOffs,9, "   Please mention my name in programs that use XLIB");
  x_printf(textwindow_x+5,50+8 ,VisiblePageOffs,9, " just to make me feel it was worth the effort.");
  x_printf(textwindow_x+5,50+16,VisiblePageOffs,9, " If you have any bug to report please feel free to");
  x_printf(textwindow_x+5,50+24,VisiblePageOffs,9, " mail me a message. Any hints, suggestions and");
  x_printf(textwindow_x+5,50+32,VisiblePageOffs,9, " contributions are welcome and encouraged.");
  x_printf(textwindow_x+5,50+52,VisiblePageOffs,9, " I have contributed this code to the public domain.");
  x_printf(textwindow_x+5,50+60,VisiblePageOffs,9, "    Please respect my wishes and leave it there.");

  x_printf(textwindow_x+5,50+80,VisiblePageOffs,9, "   Finally, I hope you all find this stuff useful,");
  x_printf(textwindow_x+5,50+96,VisiblePageOffs,9, " Themie Gouthas - EGG@DSTOS3.DSTO.GOV.AU");

  wait_for_keypress();

  x_hide_mouse();

  x_shift_rect (27, 27, 360-27, 177, 27, 23, VisiblePageOffs);
  x_rect_fill(25, 173, 335, 176, VisiblePageOffs, 1);
  for (i = 0; i < 50; i++) {
	x_shift_rect (27, 26, 360-27, 177 - (i * 3), 27, 23, VisiblePageOffs);
  }
}


