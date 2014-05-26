/*
 * LIB.C v1.2a
 *
 * by Robert Schmidt
 * (C)1993 Ztiff Zox Softwear
 *
 * Simple graphics library to accompany the article
 * 
 *					  INTRODUCTION TO MODE X.
 * 
 * This library provides the basic functions for initializing and using
 * unchained (planar) 256-color VGA modes.  Currently supported are:
 *
 *	  - 320x200
 *	  - 320x240
 *
 * Functions are provided for:
 *
 *	  - initializing one of the available modes
 *	  - setting the start address of the VGA refresh data
 *	  - setting active and visible display pages
 *	  - writing and reading a single pixel to/from video memory
 *
 * The library is provided as a demonstration only, and is not claimed
 * to be particularly efficient or suited for any purpose.  It has only
 * been tested with Borland C++ 3.1 by the author.  Comments on success
 * or disaster with other compilers are welcome.
 *
 * This file is public domain.  Do with it whatever you'd like, but
 * please don't distribute it without the article.
 *
 * Thanks go out to various helpful netters who spotted the 0xE7 bug
 * in the set320x240x256() function!
 *
 * Modified by sparky4 so it can be compiled in open watcom ^^
 */




/*
 * We 'require' a large data model simply to get rid of explicit 'far'
 * pointers and compiler specific '_fmemset()' functions and the likes.
 */
#if !defined(__COMPACT__)
# if !defined(__LARGE__)
#  if !defined(__HUGE__)
#   error Large data model required!  Try compiling with 'wcc -0 -ml lib.c'.
#  endif
# endif
#endif

#include <dos.h>
#include <mem.h>
#include <conio.h>

//code from old library!
/*src\lib\*/
#include "dos_gfx.h"

int old_mode;
//color ‚Ä‚·‚Æ
int gq = LGQ;
//‚Ä‚·‚Æ
int q = 0;
int bakax = 0, bakay = 0;
cord xx = rand()&0%320, yy = rand()&0%240, sx = 0, sy = 0;
byte coor;

/*
 * Comment out the following #define if you don't want the testing main()
 * to be included.
 */
#define TESTING

/*
 * Define the port addresses of some VGA registers.
 */
#define CRTC_ADDR	   0x3d4   /* Base port of the CRT Controller (color) */

#define SEQU_ADDR	   0x3c4   /* Base port of the Sequencer */
#define GRAC_ADDR	   0x3ce   /* Base port of the Graphics Controller */
#define STATUS_ADDR     0x3DA

unsigned char *RowsX[600];
unsigned char write_plane, read_plane;
unsigned short text_mask[16] = { 0x0002, 0x0102, 0x0202, 0x0302,
                                 0x0402, 0x0502, 0x0602, 0x0702,
                                 0x0802, 0x0902, 0x0A02, 0x0B02,
                                 0x0C02, 0x0D02, 0x0E02, 0x0F02 };


/*
 * Make a far pointer to the VGA graphics buffer segment.  Your compiler
 * might not have the MK_FP macro, but you'll figure something out.
 */
byte *vga = (byte *) MK_FP(0xA000, 0);


/*
 * width and height should specify the mode dimensions.  widthBytes
 * specify the width of a line in addressable bytes.
 */
unsigned width, height, widthBytes;

/*
 * actStart specifies the start of the page being accessed by
 * drawing operations.  visStart specifies the contents of the Screen
 * Start register, i.e. the start of the visible page.
 */
unsigned actStart, visStart;

/*
 * set320x200x256_X()
 *	  sets mode 13h, then turns it into an unchained (planar), 4-page
 *	  320x200x256 mode.
 */
void set320x200x256_X(void)
		{
		union REGS r;

		/* Set VGA BIOS mode 13h: */
		r.x.ax = 0x0013;
		int86(0x10, &r, &r);

		/* Turn off the Chain-4 bit (bit 3 at index 4, port 0x3c4): */
		outpw(SEQU_ADDR, 0x0604);

		/* Turn off word mode, by setting the Mode Control register
		of the CRT Controller (index 0x17, port 0x3d4): */
		outpw(CRTC_ADDR, 0xE317);

		/* Turn off doubleword mode, by setting the Underline Location
		   register (index 0x14, port 0x3d4): */
		outpw(CRTC_ADDR, 0x0014);

		/* Clear entire video memory, by selecting all four planes, then
		   writing 0 to entire segment. */
		outpw(SEQU_ADDR, 0x0F02);
		memset(vga+1, 0, 0xffff); /* stupid size_t exactly 1 too small */
		vga[0] = 0;

		/* Update the global variables to reflect dimensions of this
		   mode.  This is needed by most future drawing operations. */
		width		   = 320;
		height  = 200;

		/* Each byte addresses four pixels, so the width of a scan line
		   in *bytes* is one fourth of the number of pixels on a line. */
		widthBytes = width / 4;

		/* By default we want screen refreshing and drawing operations
		   to be based at offset 0 in the video segment. */
		actStart = visStart = 0;

		/*
--------------------
HORIZONTAL SCROLLING
--------------------
Horizontal scrolling is essentially the same as vertical scrolling, all
you do is increment or decrement the VGA offset register by 1 instead of
80 as with vertical scrolling.

However, horizontal scrolling is complicated by two things

  1. Incrementing the offset register by one actually scrolls by FOUR
     pixels (and there are FOUR planes on the VGA, what a coincidence)

  2. You can't draw the image off the screen and then scroll it on
     because of the way the VGA wraps to the next row every 80 bytes
     (80 bytes * 4 planes = 320 pixels), if you tried it, you would
     actually be drawing to the other side of the screen (which is
     entirely visible)

I'll solve these problems one at a time.

Firstly, to get the VGA to scroll by only one pixel you use the horizontal
pixel panning (HPP) register. This register resides at

  PORT:     3C0H
  INDEX:    13h

and in real life, you use it like this

----------------- Pixel Panning ---------------
IN PORT 3DAH (this clears an internal
  flip-flop of the VGA)
OUT 13H TO PORT 3C0H
OUT value TO PORT 3C0H (where "value" is the
  number of pixels to offset)
-----------------------------------------------
*/

//mxSetVirtualScreen(480,360);
		}

/*
 * setActiveStart() tells our graphics operations which address in video
 * memory should be considered the top left corner.
 */
void setActiveStart(unsigned offset)
		{
		actStart = offset;
		}

/*
 * setVisibleStart() tells the VGA from which byte to fetch the first
 * pixel when starting refresh at the top of the screen.  This version
 * won't look very well in time critical situations (games for
 * instance) as the register outputs are not synchronized with the
 * screen refresh.  This refresh might start when the high byte is
 * set, but before the low byte is set, which produces a bad flicker.
 */
void setVisibleStart(unsigned offset)
		{
		visStart = offset;
		outpw(CRTC_ADDR, 0x0C);		 /* set high byte */
		outpw(CRTC_ADDR+1, visStart >> 8);
		outpw(CRTC_ADDR, 0x0D);		 /* set low byte */
		outpw(CRTC_ADDR+1, visStart & 0xff);
		}

/*
 * setXXXPage() sets the specified page by multiplying the page number
 * with the size of one page at the current resolution, then handing the
 * resulting offset value over to the corresponding setXXXStart()
 * function.  The first page is number 0.
 */
void setActivePage(int page)
		{
		setActiveStart(page * widthBytes * height);
		}

void setVisiblePage(int page)
		{
		setVisibleStart(page * widthBytes * height);
		}

void putPixel_X(int x, int y, byte color)
		{
		/* Each address accesses four neighboring pixels, so set
		   Write Plane Enable according to which pixel we want
		   to modify.  The plane is determined by the two least
		   significant bits of the x-coordinate: */
		outp(0x3c4, 0x02);
		outp(0x3c5, 0x01 << (x & 3));

		/* The offset of the pixel into the video segment is
		   offset = (width * y + x) / 4, and write the given
		   color to the plane we selected above.  Heed the active
		   page start selection. */
		vga[(unsigned)(widthBytes * y) + (x / 4) + actStart] = color;

		}

byte getPixel_X(int x, int y)
		{
		/* Select the plane from which we must read the pixel color: */
		outpw(GRAC_ADDR, 0x04);
		outpw(GRAC_ADDR+1, x & 3);

		return vga[(unsigned)(widthBytes * y) + (x / 4) + actStart];

		}

void set320x240x256_X(void)
		{
		/* Set the unchained version of mode 13h: */
		set320x200x256_X();

		/* Modify the vertical sync polarity bits in the Misc. Output
		   Register to achieve square aspect ratio: */
		outp(0x3C2, 0xE3);

		/* Modify the vertical timing registers to reflect the increased
		   vertical resolution, and to center the image as good as
		   possible: */
		outpw(0x3D4, 0x2C11);		   /* turn off write protect */
		outpw(0x3D4, 0x0D06);		   /* vertical total */
		outpw(0x3D4, 0x3E07);		   /* overflow register */
		outpw(0x3D4, 0xEA10);		   /* vertical retrace start */
		outpw(0x3D4, 0xAC11);		   /* vertical retrace end AND wr.prot */
		outpw(0x3D4, 0xDF12);		   /* vertical display enable end */
		outpw(0x3D4, 0xE715);		   /* start vertical blanking */
		outpw(0x3D4, 0x0616);		   /* end vertical blanking */

		/* Update mode info, so future operations are aware of the
		   resolution */
		height = 240;

		}


/*-----------XXXX-------------*/

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// WaitRetrace() - This waits until you are in a Verticle Retrace.         //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
void wait_for_retrace(void)
{
    while (!(inp(STATUS_ADDR) & 0x08));
}

/*tile*/
//king_crimson's code
void putColorBox_X(int x, int y, int w, int h, byte color) {
	outp(0x3c4, 0x02);

	int curx, cury;
	unsigned drawptr;
	for (curx=x; curx<(x+w); curx++) {
		outp(0x3c5, 0x01 << (curx & 3));
		drawptr = (unsigned)(widthBytes * y) + (curx / 4) + actStart;
		for (cury=0; cury<h; cury++) {
			vga[drawptr] = color;
			drawptr += widthBytes;
		}
	}
}

void vScroll(int rows)
{
	// Scrolling = current start + (rows * bytes in a row)
	setVisibleStart(visStart + (rows * width));
}

void scrolly(int bongy)
{
	int boingy=0;
	if(bongy<0)
		boingy=-1;
	else if(bongy>0)
		boingy=1;

	for(int ti=0;ti<TILEWH;ti++)
	{
		delay(1);
		vScroll(boingy);
	}
}

//king_crimson's code
void hScroll(int Cols) {
	wait_for_retrace();
	outp(0x3C0, 0x13);
	outp(0x3C0, Cols & 3);
	outp(0x3D4, 0x13);
	outp(0x3D5, Cols >> 2);
	outp(0x3D4, Cols);
	//setVisibleStart(visStart + (Cols * height));
	setVisibleStart(visStart + (Cols * width));
}

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// setvideo() - This function Manages the video modes					  //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
void setvideo(/*byte mode, */int vq){
		union REGS in, out;

		if(!vq){ // deinit the video
				// change to the video mode we were in before we switched to mode 13h
				mxSetMode( MX_TEXT );
				//in.h.ah = 0x00;
				//in.h.al = old_mode;
				//int86(0x10, &in, &out);
				mxTerm();

		}else if(vq == 1){ // init the video
				// get old video mode
				//in.h.ah = 0xf;
				//int86(0x10, &in, &out);
				//old_mode = out.h.al;

				// enter mode
				mxInit();
				mxSetMode( MX_320x240 );
				mxSetVirtualScreen( 480, 360 );
				//set320x240x256_X();
				//mxSetMode(MX_320x240);
				//mxSetVirtualScreen(560,420);
				//mxSetVirtualScreen((640-TILEWH),(480-TILEWH));
				//mxSetClip( TRUE );
		}
}

/////////////////////////////////////////////////////////////////////////////
//																		 //
// cls() - This clears the screen to the specified color, on the VGA or on //
//		 the Virtual screen.											 //
//																		 //
/////////////////////////////////////////////////////////////////////////////
void cls(byte color, byte *Where){
		_fmemset(Where, color, width*(height*17));
}

//color ‚Ä‚·‚Æ
int colortest(){
		if(gq < NUM_COLORS){
				cls(gq, vga);
				gq++;
		}else gq = 0;
		return gq;
}

//color ‚Ä‚·‚Æ
int colorz(){
		if(gq < HGQ){
//----		  cls(gq, vaddr);
				cls(gq, vga);
				gq++;
		}else gq = LGQ;
		return gq;
}

//slow spectrum down
void ssd(int svq){
		if(sy < height+1){
				if(sx < width+1){
						//plotpixel(xx, yy, coor, vga);
						//ppf(sx, sy, coor, vga);
						putPixel_X(sx, sy, coor);
						//printf("%d %d %d %d\n", sx, sy, svq, coor);
						sx++;
				}else sx = 0;
				if(sx == width){
						sy++;
						if(svq == 7) coor++;
						if(sy == height && svq == 8) coor = rand()%NUM_COLORS;
				}
		}else sy = 0;
}

/*-----------ding-------------*/
int ding(int q){

/*	if(yy<height){
		setActivePage(0);
		setVisiblePage(0);
	}
	if((height)<yy<(height*2)){
		setActivePage(1);
		setVisiblePage(1);
	}
	if((height*2)<yy<(height*3)){
		setActivePage(2);
		setVisiblePage(2);
	}*/
		int d3y;

//++++  if(q <= 4 && q!=2 && gq == BONK-1) coor = rand()%HGQ;
		if((q == 2
		||q==4
		||q==16
		) && gq == BONK){
						if(coor < HGQ && coor < LGQ) coor = LGQ;
						if(coor < HGQ-1){
								coor++;
				}else{ coor = LGQ;
						bakax = rand()%3; bakay = rand()%3;
				}
		}

		if(q==8){ colorz(); return gq; }else
		if(q==10){ ssd(q); /*printf("%d\n", coor);*/ }else
		if(q==5){ colortest(); return gq; }else
		if(q==11){ colorz(); delay(100); return gq; }
		if(q==6){
				coor = rand()%NUM_COLORS;
//----		  cls(coor, vaddr);
				cls(coor, vga);
				//updatevbuff();
		}

		if(q==7||q==9){
				if(gq < HGQ){
						if(q == 7) ssd(q);
						if(q == 9){ ssd(q); coor++; }
						gq++;
				}else gq = LGQ;
		}
		if((q<5 && gq<BONK) || (q==16 && gq<BONK)){ // the number variable make the colors more noticable
				if(q==1){
						if(xx==width){bakax=0;}
						if(xx==0){bakax=1;}
						if(yy==height){bakay=0;}
						if(yy==0){bakay=1;}
				}else if(q==3){
						if(xx!=width||yy!=height){
								if(xx==0){bakax=1;bakay=-1;d3y=1;}
								if(yy==0){bakax=1;bakay=0;d3y=1;}
								if(xx==width){bakax=-1;bakay=-1;d3y=1;}
								if(yy==height){bakax=1;bakay=0;d3y=1;}
						}else if(xx==width&&yy==height) xx=yy=0;
				}
				if(q==3){
						if(d3y){
								if(bakay<0){
										yy--;
										d3y--;
								}else
								if(bakay>0){
										yy++;
										d3y--;
								}
						}
						if(bakax<0){
								xx--;
						}else
						if(bakax>0){
								xx++;
						}
				}else{
						if(q==16)
						{
								if(!bakax){
									xx--;
								}else if(bakax>0){
									xx++;
								}
								if(!bakay){
									yy--;
								}else if(bakay>0){
									yy++;
								}
						}else{
								if(!bakax){
//									xx-=TILEWH;
									xx--;
								}else if(bakax>1){
//									xx+=TILEWH;
									xx++;
								}
								if(!bakay){
//									yy-=TILEWH;
									yy--;
								}else if(bakay>1){
//									yy+=TILEWH;
									yy++;
								}
						}
				}
				// fixer
//				if(q!=16){
//if(q!=16)
//						if(xx<0) xx=(width-TILEWH);
//						if(yy<0) yy=(height-TILEWH);
//						if(xx>(width-TILEWH)) xx=0;
//						if(yy>(height-TILEWH)) yy=0;
//				}

//interesting effects
				if(q==16)
				{
				int tx=0,ty=0;
				tx+=xx+16;
				ty+=yy+16;
				putPixel_X(tx, ty, coor);
				//drawrect(tx, ty, tx+TILEWH, ty+TILEWH, coor);
				//printf("%d %d %d %d %d %d\n", xx, yy, tx, ty, TILEWH);

				// plot the pixel
//----		  ppf(xx, yy, coor, vga);
				}else /*if(xx>=0 && xx<width && yy>=0 && yy<(height*3))*/{
//					putColorBox_X(xx, yy, TILEWH, TILEWH, coor);
//++++0000
//					putPixel_X(xx, yy, coor);
					mxPutPixel(xx, yy, coor);
				} 

//----		  if(q==2) ppf(rand()%, rand()%height, 0, vga);
//				if(q==2) putColorBox_X(rand()%width, rand()%(height*3), TILEWH, TILEWH, 0);
//++++0000
				if(q==2) putPixel_X(rand()%width, rand()%(height*3), 0);
				if(q==16) putPixel_X(rand()%width, rand()%(height*3), 0);
				if(q==2||q==4||q==16){ bakax = rand()%3; bakay = rand()%3; }
				gq++;
//if(xx<0||xx>320||yy<0||yy>(height*3))
//	  printf("%d %d %d %d %d %d\n", xx, yy, coor, bakax, bakay, getPixel_X(xx,yy));
//	  printf("%d\n", getPixel_X(xx,yy));
//0000
//	  drawText(0, 0, 15, getPixel_X(xx,yy));
		}else gq = LGQ;
		return gq;
}


/*
 * The library testing routines follows below.
 */


#ifdef TESTING

#include <stdio.h>
#include <conio.h>

void doTest(void)
		{
		int p, x, y, pages;

		/* This is the way to calculate the number of pages available. */
		pages = 65536L/(widthBytes*height); // apparently this takes the A000 address
//		if(height==240) pages++;

//		printf("%d\n", pages);

		for (p = 0; p <= pages; ++p)
				{
				setActivePage(p);

				/* On each page draw a single colored border, and dump the palette
				   onto a small square about the middle of the page. */

				   //{
						for (x = 0; x <= width; ++x)
								{
								putPixel_X(x, 0, p+1);
								if(p!=pages) putPixel_X(x, height-1, p+1);
										else if(height==240) putPixel_X(x, 99-1, p+1);
								}

						for (y = 0; y <= height; ++y)
								{
								putPixel_X(0, y, p+1);
								if(p!=pages) putPixel_X(width-1, y, p+1);
										else if(height==240) putPixel_X(width-1, y, p+1);
								}

						for (x = 0; x < TILEWH; ++x)
								for (y = 0; y < TILEWH; ++y)
										putPixel_X(x+(p+2)*16, y+(p+2)*TILEWH, x + y*TILEWH);
						//}

				}

		/* Each pages will now contain a different image.  Let the user cycle
		   through all the pages by pressing a key. */
		for (p = 0; p < pages; ++p)
				{
				setVisiblePage(p);
				getch();
				}

		}

/*
 * Library test (program) entry point.
 */

int main(void)
		{
		int key,d,x,y;
		//short int temp;
		// main variables
		d=1; // switch variable
		key=4; // default screensaver number
		x=0;
		y=0;
//	  puts("First, have a look at the 320x200 mode.  I will draw some rubbish");
//	  puts("on all of the four pages, then let you cycle through them by");
//	  puts("hitting a key on each page.");
//	  puts("Press a key when ready...");
//	  getch();

//	  doTest();

//	  puts("Then, check out Mode X, 320x240 with 3 (and a half) pages.");
//	  puts("Press a key when ready...");
//	  getch();

//++++0000
		setvideo(1);
		/*temp = loadfontX("vga8x8.fnt");

		if (temp) {
			putstringX(0, 0, "bakapi!", 2);
		}
		getch();*/
// screen savers

/*while(d!=0){ // on!
				if(!kbhit()){ // conditions of screen saver
						ding(key);
				}else{
						setvideo(0);
						// user imput switch
						printf("Enter 1, 2, 3, 4, or 6 to run a screensaver, or enter 5 to quit.\n", getch());  // prompt the user
						scanf("%d", &key);
						//if(key==3){xx=yy=0;} // crazy screen saver wwww
						if(key==5) d=0;
						setvideo(1);
				}
		}*/ // else off
		while(!kbhit()){ // conditions of screen saver
			ding(4);
		}
		//end of screen savers
		//doTest();
		getch();

		while(!kbhit()){ // conditions of screen saver
//			hScroll(1);
//			scrolly(1);
//			vScroll(1);
//			delay(100);
			for(int i=0;i<TILEWH;i++){
				mxPan(x,y);
				mxWaitRetrace();
				x++;
				y++;
			}
		//delay(100);
		}
		setvideo(0);
		printf("wwww\n%dx%d\n", width,height);
		printf("[%d]\n", mxGetVersion());
		puts("where to next?  It's your move! wwww");
		printf("bakapi ver. 1.04.09.04\nis made by sparky4i†ƒÖ…j feel free to use it ^^\nLicence: GPL v2\n");
		return 0;
		}

#endif
