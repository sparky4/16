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
 * modified by sparky4 so it can be compiled in open watcom ^^
 */




/*
 * We 'require' a large data model simply to get rid of explicit 'far'
 * pointers and compiler specific '_fmemset()' functions and the likes.
 */
#if !defined(__COMPACT__)
# if !defined(__LARGE__)
#  if !defined(__HUGE__)
#   error Large data model required!  Try compiling with 'bcc -ml lib.c'.
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
int xx = rand()&0%320, yy = rand()&0%240, sx = 0, sy = 0;
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


/*
 * Make a far pointer to the VGA graphics buffer segment.  Your compiler
 * might not have the MK_FP macro, but you'll figure something out.
 */
byte *vga = (byte *) MK_FP(0xA000, 0);

//fontAddr = getFont();

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
/*tile*/
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
/*OFFSET = 0
WHILE NOT FINISHED DO
  OFFSET = OFFSET + 80
  IF OFFSET >= (200 * 80) THEN OFFSET = 0
  DRAW TO ROW 200
  SET VGA OFFSET = OFFSET
  DRAW TO ROW -1 (was row 0 before scroll)
END WHILE*//*
void scrolly(){
	int OFFSET = 0
	WHILE NOT FINISHED DO
		OFFSET = OFFSET + 80
		IF OFFSET >= (240 * 80) THEN OFFSET = 0
		RAW TO ROW 240
		SET VGA OFFSET = OFFSET
		DRAW TO ROW -1 (was row 0 before scroll)
	}
}
*/
//---------------------------------------------------
//
// Use the bios to get the address of the 8x8 font
//
// You need a font if you are going to draw text.
//

int far *
getFont()
{
	union REGPACK rg;
	int seg;
	int off;
	memset(&rg, 0, sizeof(rg));

	rg.w.ax = 0x1130;
	rg.h.bh = 0x03;
	intr(0x10, &rg);
	seg = rg.w.es;
	off = rg.w.bp;
	

	return (int far *)MK_FP(seg, off);
}

void drawChar(int x, int y, int color, byte c)
{
		int i, j;
		int mask;
		int far *font = getFont() + (c * 8);

		for (i = 0; i < 8; i++)
		{
				mask = *font;
				for (j = 0; j < 8; j++)
				{
						if (mask & 0x80)
						{
								//pixel(x + j, y + i, color);
								putPixel_X(x + j, y + i, color);
						}
						mask <<= 1;
				}
				font++;
		}
}

void drawText(int x, int y, int color, byte string)
{
		while (string)
		{
				drawChar(x, y, color, string);
				x += 8;
				string++;
		}
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
				in.h.ah = 0x00;
				in.h.al = old_mode;
				int86(0x10, &in, &out);

		}else if(vq == 1){ // init the video
				// get old video mode
				in.h.ah = 0xf;
				int86(0x10, &in, &out);
				old_mode = out.h.al;

				// enter mode
				set320x240x256_X();
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
		setActivePage(0);
		setVisiblePage(0);
		int d3y;

//++++  if(q <= 4 && q!=2 && gq == BONK-1) coor = rand()%HGQ;
		if((q == 2
		||q==4
		||q==16
		) && gq == BONK-1){
						if(coor < HGQ && coor < LGQ) coor = LGQ;
						if(coor < HGQ-1){
								coor++;
				}else{ coor = LGQ;
						bakax = rand()%3; bakay = rand()%3;
				}
		}

		if(q == 5){ colortest(); return gq; }
		if(q == 10){ colorz(); return gq; }
		if(q == 11){ colorz(); delay(100); return gq; }
		if(q == 8){ ssd(q); /*printf("%d\n", coor);*/ }
		if(q == 6){
				coor = rand()%NUM_COLORS;
//----		  cls(coor, vaddr);
				cls(coor, vga);
				//updatevbuff();
		}

		if(q == 7 || q== 9){
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
										xx--;//=TILEWH;
								}else if(bakax>0){
										xx++;//=TILEWH;
								}
								if(!bakay){
										yy--;//=TILEWH;
								}else if(bakay>0){
										yy++;//=TILEWH;
								}
						}else{
								if(!bakax){
										xx-=TILEWH;
								}else if(bakax>1){
										xx+=TILEWH;
								}
								if(!bakay){
										yy-=TILEWH;
								}else if(bakay>1){
										yy+=TILEWH;
								}
						}
				}
				// fixer
				//if(q!=16){
						if(xx<0) xx=width;
						if(yy<0) yy=height;
						if(xx>width) xx=0;
						if(yy>height) yy=0;
				//}

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
				}else if(xx>=0 && xx<width && yy>=0 && PixelY<height){
					putColorBox_X(xx, yy, TILEWH, TILEWH, coor);
//++++0000					putPixel_X(xx, yy, coor);
				} 

//----		  if(q==2) ppf(rand()%, rand()%height, 0, vga);
				if(q==2) putColorBox_X(rand()%width, rand()%height, TILEWH, TILEWH, 0);
				if(q==16) putPixel_X(rand()%width, rand()%height, 0);
				if(q==2||q==4||q==16){ bakax = rand()%3; bakay = rand()%3; }
				gq++;
//if(xx<0||xx>320||yy<0||yy>240)
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

		printf("%d\n", pages);

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
										else putPixel_X(x, 99-1, p+1);
								}

						for (y = 0; y <= height; ++y)
								{
								putPixel_X(0, y, p+1);
								if(p!=pages) putPixel_X(width-1, y, p+1);
										else putPixel_X(width-1, y, p+1);
								}

						for (x = 0; x < 16; ++x)
								for (y = 0; y < 16; ++y)
										putPixel_X(x+(p+2)*16, y+(p+2)*16, x + y*16);
						//}

				drawText(0, 0, 15, p);

				}

		/* Each pages will now contain a different image.  Let the user cycle
		   through all the pages by pressing a key. */
		for (p = 0; p <= pages; ++p)
				{
				setVisiblePage(p);
				//drawText(0, 240, 15, "bakapi");
				getch();
				}

		}

/*
 * Library test (program) entry point.
 */

int main(void)
		{
		int key,d;
		// main variables
		d=1; // switch variable
		key=4; // default screensaver number
//	  puts("First, have a look at the 320x200 mode.  I will draw some rubbish");
//	  puts("on all of the four pages, then let you cycle through them by");
//	  puts("hitting a key on each page.");
//	  puts("Press a key when ready...");
//	  getch();

//	  doTest();

//	  puts("Then, check out Mode X, 320x240 with 3 (and a half) pages.");
//	  puts("Press a key when ready...");
//	  getch();

		setvideo(1);
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
		doTest();
		setvideo(0);
		puts("Where to next?  It's your move! wwww");
		printf("bakapi ver. 1.04.09a\nis made by sparky4i†ƒÖ…j feel free to use it ^^\nLicence: GPL v2\n");
		return 0;
		}

#endif
