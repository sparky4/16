/*
 * LIB.C v1.2a
 *
 * by Robert Schmidt
 * (C)1993 Ztiff Zox Softwear
 *
 * Simple graphics library to accompany the article
 * 
 *			INTRODUCTION TO MODE X.
 * 
 * This library provides the basic functions for initializing and using
 * unchained (planar) 256-color VGA modes.  Currently supported are:
 *
 *	- 320x200
 *	- 320x240
 *
 * Functions are provided for:
 *
 *	- initializing one of the available modes
 *	- setting the start address of the VGA refresh data
 *	- setting active and visible display pages
 *	- writing and reading a single pixel to/from video memory
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

/*
 * Comment out the following #define if you don't want the testing main()
 * to be included.
 */

#define TESTING

/*
 * Define the port addresses of some VGA registers.
 */

#define CRTC_ADDR	0x3d4	/* Base port of the CRT Controller (color) */

#define SEQU_ADDR	0x3c4	/* Base port of the Sequencer */
#define GRAC_ADDR	0x3ce	/* Base port of the Graphics Controller */


/*
 * Make a far pointer to the VGA graphics buffer segment.  Your compiler
 * might not have the MK_FP macro, but you'll figure something out.
 */

typedef unsigned char UCHAR;
UCHAR *vga = (UCHAR *) MK_FP(0xA000, 0);

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
 *	sets mode 13h, then turns it into an unchained (planar), 4-page
 *	320x200x256 mode.
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

	width   = 320;
	height	= 200;

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
	outpw(CRTC_ADDR, 0x0C);		/* set high byte */
	outpw(CRTC_ADDR+1, visStart >> 8);
	outpw(CRTC_ADDR, 0x0D);		/* set low byte */
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

void putPixel_X(int x, int y, UCHAR color)
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

UCHAR getPixel_X(int x, int y)
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

	outpw(0x3D4, 0x2C11);		/* turn off write protect */
	outpw(0x3D4, 0x0D06);		/* vertical total */
	outpw(0x3D4, 0x3E07);		/* overflow register */
	outpw(0x3D4, 0xEA10);		/* vertical retrace start */
	outpw(0x3D4, 0xAC11);		/* vertical retrace end AND wr.prot */
	outpw(0x3D4, 0xDF12);		/* vertical display enable end */
	outpw(0x3D4, 0xE715);		/* start vertical blanking */
	outpw(0x3D4, 0x0616);		/* end vertical blanking */

	/* Update mode info, so future operations are aware of the
	   resolution */

	height = 240;

	}



/*
 * The library testing routines follows below.
 */


#ifdef TESTING

#include <stdio.h>
#include <conio.h>

void set80x25(void)
	{
	union REGS r;
	r.x.ax = 0x0003;
	int86(0x10, &r, &r);
	}

void doTest(void)
	{
	int p, x, y, pages;

	/* This is the way to calculate the number of pages available. */

	pages = 65536L/(widthBytes*height);

	for (p = 0; p < pages; ++p)
		{
		setActivePage(p);

		/* On each page draw a single colored border, and dump the palette
		   onto a small square about the middle of the page. */

		for (x = 0; x <= width; ++x)
			{
			putPixel_X(x, 0, p+1);
			putPixel_X(x, height-1, p+1);
			}

		for (y = 0; y <= height; ++y)
			{
			putPixel_X(0, y, p+1);
			putPixel_X(width-1, y, p+1);
			}

		for (x = 0; x < 16; ++x)
			for (y = 0; y < 16; ++y)
				putPixel_X(x+(p+3)*16, y+(p+3)*16, x + y*16);

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
//	puts("First, have a look at the 320x200 mode.  I will draw some rubbish");
//	puts("on all of the four pages, then let you cycle through them by");
//	puts("hitting a key on each page.");
//	puts("Press a key when ready...");
//	getch();

//	set320x200x256_X();
//	doTest();

//	set80x25();
//	puts("Then, check out Mode X, 320x240 with 3 (and a half) pages.");
//	puts("Press a key when ready...");
//	getch();

	set320x240x256_X();
	doTest();

	set80x25();
	puts("Where to next?  It's your move!");
	return 0;
	}

#endif
