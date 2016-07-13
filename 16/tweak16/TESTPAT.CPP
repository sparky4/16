/*
	testpat.cpp version 1.6
	by Robert Schmidt of Ztiff Zox Softwear 1993

	Defines the member functions of the TestPatterns class declared in
		testpat.hpp.

	1.1
		Fixed a couple of bugs in the text mode test.  On some computers
		it would overwrite data in the C000h segment, if RAM had been mapped
		there by QEMM or something similar.

	Modified June 13-14, 1993 by Peter McDermott
		The 16 color test pattern has been changed.  Now it puts dots from the
		upper left hand corner to down the top and left of the screen to the
		edges.  There is a longer, white dot every 100 pixels.  Also, a color
		bar is displayed across the page.

	1.6
		Added support for the mode autodetecting scheme, and for VGALIB,
		my mode independant VGA graphics library, to provide more
		intelligent test screens.
*/

#include <dos.h>
#include <mem.h>
#include <conio.h>
#include <math.h>
#include <string.h>
#include <iostream.h>
#include <stdio.h>

#include "misc.hpp"
#include "screen.hpp"
#include "vgalib.hpp"
#include "testpat.hpp"
#include "detect.hpp"
#include "screen.hpp"

extern char *graphScr;
extern unsigned *textScr;
extern unsigned editHeight;

// These are the text strings identifying each test to the user.

char *TestPatterns::string[TestPatterns::tests] =
	{
	"Graphics autodetect",
	"Text screen, 16 point",
	"Text screen, 8 point",
	"4 planes, 16 colors",
	"1 plane, 256 colors",
	"4 planes, 256 colors"
	};


void color16(unsigned char color)					// Peter
{
  // load set/reset register with color to be displayed
  outport(0x3ce,color<<8);
}

// following is a point routine for 16 color mode
void init16()										// Peter
{
  // default drawing color is white
  color16(15);
  // load set/reset enable for all display planes
  outport(0x3ce,0x0f01);
  // load map mask register for all display planes
  outport(0x3c4,0x0f02);
  // load function select correctly (logical OR)
  outport(0x3ce,0x2003);
  // set to read mode 0
  outportb(0x3ce,0x05);
  int ModeReg=inportb(0x3cf);
  outport(0x3ce,ModeReg && 0xf7);  // make sure we're in read mode 0
}


void point16(unsigned int x, unsigned int y)		// Peter
{
  outportb(0x3d4,0x13);				//get screen width in words
  unsigned width=inportb(0x3d5)*2;	//convert to bytes
  // load bitmask register with correct value
  outport(0x3ce,(0x80 >> (x % 8) << 8) | 0x08);
  // load the latch register with the data already in display memory
  _AX=graphScr[y*width+x/8];
  // calculate the position in memory of the pixel to be written
  graphScr[y*width+x/8] = 0x00;
}


// TestPatterns::run() puts the selected test onto the screen.

void TestPatterns::run(RegisterTable &regTab)
	{
	unsigned a,c;
	unsigned long offset;

	regTab.out();

	outportb(0x3c4,0x02);            	//get write plane enable
	unsigned plane=inportb(0x3c5);

	outportb(0x3d4,0x13);				//get screen width in words
	unsigned long width=inportb(0x3d5)*2;	//convert to bytes

	// Now select the correct initialization method:

	switch (testNo)
		{
		case test4x16:
		case test1x256:
		case test4x256:
			// All graphics modes: clear the screen, but take care of
			//	write enabling all planes.
			outport(0x3c4,0x0f02);
			memset(graphScr, 0, 0xffff);
			graphScr[0xffff] = 0;
			outportb(0x3c4,0x02);
			outportb(0x3c5,plane);
			break;

		case testText16:				// set 8x16 font
			_AX = 0x1104;
			_BL = 0;
			geninterrupt(0x10);
			goto commonText;
		case testText8:					// set 8x8 font
			_AX = 0x1102;
			_BL = 0;
			geninterrupt(0x10);
commonText:
			// Just blank the text screen.
			memset(textScr, 0, 8000);
		}

	switch (testNo)
		{
		case autoDetect:
			ModeInfo minfo(regTab);
			GraphicsAPI *g = minfo.getGraphicsAPI();
			if (!g)
				{
				setBiosMode(3);
				cout << "This is not a graphics mode.  Either: -" << endl
					 << " - Use TAB (->|) to select one of the two "
						"available text screens," << endl
					 << " - Base your mode on one of the BIOS graphics "
						"modes, by pressing B and " << endl
					 << "   typing a graphics mode number, for example "
						"0x12 or 0x13, or" << endl
					 << " - Load a graphics mode file by pressing L and "
						"typing the name of the file." << endl << endl
					 << "Now press any key to return to the editor." << endl;
				getch();
				}
			else
				{
				g->setColor(0);
				g->wipe();
				int width = g->getVirtualWidth();
				int height = g->getVirtualHeight();
				int colors = g->getColors();
				char txt1[40];
				int i, j, maxTick = max(width, height);

				if (g->getColors() == 256)
					setPalette256();
				else
					setPalette16();

				for (i = 2; i <= maxTick; i += 2)
					{
					int coord = i-1;
					int color, length;
					if (i % 10 == 0)
						if (i % 50 == 0)
							if (i % 100 == 0)
								{
								color = 15, length = 10;
								itoa(i, txt1, 10);
								}
							else
								color = 14, length = 7;
						else
							color = 13, length = 5;
					else
						color = 9, length = 2;
					if (i <= width)
						{
						g->setColor(color);
						g->vLine(coord, 0, length);
						if (length == 10)
							{
							g->setTextJustify(GraphicsAPI::RIGHT,
											  GraphicsAPI::TOP);
							g->putText(coord, length, txt1);
							}
						}
					if (i <= height)
						{
						g->setColor(color);
						g->hLine(0, coord, length);
						if (length == 10)
							{
							g->setTextJustify(GraphicsAPI::LEFT,
											  GraphicsAPI::BOTTOM);
							g->putText(length, coord, txt1);
							}
						}
					}

				int middle = width/2;
				int line = 30;
				g->setTextJustify(GraphicsAPI::HCENTER, GraphicsAPI::TOP);
				g->setColor(10);
				g->putText(middle, line, g->getLibID());
				g->setColor(12);
				g->putText(middle, line+=12, "Physical resolution");
				sprintf(txt1, "%d x %d", g->getWidth(), g->getHeight());
				g->setColor(15);
				g->putText(middle, line+=9, txt1);

				g->setColor(12);
				g->putText(middle, line+=12, "Virtual resolution");
				sprintf(txt1, "%d x %d", width, height);
				g->setColor(15);
				g->putText(middle, line+=9, txt1);

				g->setColor(12);
				g->putText(middle, line+=12, "Page resolution");
				sprintf(txt1, "%3.1f x %3.1f", minfo.xpages, minfo.ypages);
				g->setColor(15);
				g->putText(middle, line+=9, txt1);

				int hPalSize = width - 40;
				int vPalSize = height - (line+=22);
				int palSide = sqrt(colors);
				int hPalPix = hPalSize/palSide;
				int vPalPix = vPalSize/palSide;
				hPalSize = palSide * hPalPix + 1;
				vPalSize = palSide * vPalPix + 1;
				for (i = 0; i < colors; ++i)
					{
					g->setColor(i);
					int x = width-hPalSize+(i/palSide)*hPalPix;
					int y = height-vPalSize+(i%palSide)*vPalPix;
					g->bar(x, y, x+hPalPix, y+vPalPix);
					}

				int basex=0, basey=0, bdirx=0, bdiry=0, quit=0;
				while (!quit)
					{
					if (kbhit())
						{
						int key = getch();
						if (!key && kbhit())
							key = getch() << 8;

						switch (key)
							{
							case 0x4700:
								basex = basey = bdirx = bdiry = 0;
								break;
							case 0x4800:			//UP
								--bdiry;
								break;
							case 0x4b00:
								--bdirx;
								break;
							case 0x4c00:
								bdirx = bdiry = 0;
								break;
							case 0x4d00:
								++bdirx;
								break;
							case 0x5000:			//DOWN
								++bdiry;
								break;
							case 27:
							case 13:
								quit = 1;
								break;
							}
						}
					basex += bdirx;
					basey += bdiry;
					if (basex > width-g->getWidth())
						basex = width-g->getWidth(), bdirx = 0;
					if (basey > height-g->getHeight())
						basey = height-g->getHeight(), bdiry = 0;
					if (basex < 0)
						basex = bdirx = 0;
					if (basey < 0)
						basey = bdiry = 0;

					g->setBase(basex, basey);
//					bdirx = bdiry = 0;			// for debugging
					}
				delete g;
				}
			break;

		case testText16:
		case testText8:

			// Fill top line with the sequence "0123456789" lt grey/black:
			a = 0;
			for (c=0; c<width; c++)
				textScr[a++] = ('0'+(c+1)%10) | 0x0700;

			// Then fill 4 lines with the ASCII set in white on blue:
			for (c=0; c<5*width; c++)
				textScr[a++] = c | 0x1f00;

			// Now fill the rest with the sequence "ABCDEFGHIJ" in all color
			//	combinations (except blinking!):
			c = 0;
			while (a < 0x4000)	// fixed 0x4800 bug May 7. 1993
				textScr[a++] = ('A'+c%('K'-'A')) | ((c&0x7f)<<8), c++;
			getch();
			break;


		case test1x256:
			width *= 4;

			// Fill the first 32 lines with 1 pixel wide colored vertical
			//	lines.
			for (c=0; c<width; c++)
				for (a=0; a<32; a++)
					graphScr[a*width+c]=c;

			// Fill the rest with 1 pixel high horizontal lines.
			c=0;
			offset=32*width;
			do	{
				memset(graphScr+offset, c++, width);
							//horizontal lines, 1 color each
				offset+=width;
				}
			while (offset <= 0x10000-width);
			getch();
			break;

		case test4x256:

			// This test is affected by the Write Plane Enable register.
			// First put up 32 horizontal lines in the 32 first colors.
			for (c=0; c<(width<<5); c++)
				graphScr[c]=c/width;

			// Then fill the rest with vertical lines.  This is too slow!
			offset=c;
			c=0;
			a=1;
			do	{
				outportb(0x3c5,a);	//Set write plane enable
				graphScr[offset]=c;

				if ((a<<=1)>8)
					{
					a=1;
					++offset;
					}
				if ((++c)==width<<2)
					c=0;
				}
			while (offset <= 0xffff);
			getch();
			break;
		case test4x16:

// This is Peter's 16-color test.  The original test is commented out below.

			init16();
			color16(15);
			for (a=0; a<(width*8); a+=10) {
			  if (! (a % 100)) {
				color16(15);
				point16(a,1);
				point16(a,2);
				point16(a,3); }
			  else
				color16(2);

			  point16(a,0); }

			for (a=0; a<600; a+=10) {
			  if (! (a % 100)) {
				color16(15);
				point16(1,a);
				point16(2,a);
				point16(3,a); }
			  else
				color16(2);

			  point16(0,a); }

			// draw 16 color bars across the screen

			for (int x = 0; x < (width-2)*8; x++) {
			  color16(x / ((width-2)*8/16));
			  for (int y=10; y < 190; y++) {
				point16(x+8,y); }}

/*
			// Original 16-color test
			// Fill first 32 lines with thick vertical stripes alternating
			//	between black and the color selected by Write Plane Enable.
			for (c=0; c<(width<<5); c++)
				graphScr[c]=0x0f;

			// Fill the rest with various bit patterns, in all colors.
			for (a=0; a<256; a++)
				{
				outportb(0x3c5,a);
				memset(graphScr+(a+40)*width, a, width);
				}
*/
			getch();
			break;
		}
	}


// tellTest() puts the name of the current test at the correct position on
//	the edit screen.

void TestPatterns::tellTest()
	{
	gotoxy(42,editHeight);
	textattr(TESTHEADER_COLOR);
	cprintf("Current test: ");
	textattr(TESTSTRING_COLOR);
	cprintf(string[testNo]);
	clreol();
	}
