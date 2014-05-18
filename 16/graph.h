////////////////////////////////////////
//  Graph.h
//	 Coded by Denn Man
//
//  Licensed under the Academic Free License version 1.2
//
//  *Special thanks to Micheal Abrash
//   and his columns in Dr. Dobbs Journal
//   (Jul91 and Aug91) documenting
//   ModeX
//
//	 For use with Turbo-C
//  Uses as much assembly as possible
//   in order to optimize drawing speed
//	 Self-sufficient: Nothing else needed
////////////////////////////////////////

#include <alloc.h>
#include <dos.h>
#include <math.h>
#include <stdio.h>


// Start of video memory (SEGMENT)
#define MCGA_MEM 		0xA000
// Video Digital-Analog-Converter Port
#define VIDEO_DAC		0x03C8
// Video DAC Read Port
#define DAC_READ		0x03C7
// Video Data Port
#define VIDEO_DATA	0x03C9
// VGA Sequence Controller
#define SC_INDEX		0x03C4
// CRT Control Register
#define CRTC_INDEX	0x03D4
// Miscellaneous Output Register
#define MISC_OUTPUT	0x03C2
// VGA Map Mask Register
#define MAP_MASK		0x02
// Screen Width
#define SCR_WIDTH		80
// VGA Graphics Controller Register
#define GC_INDEX		0x03CE
// VGA Read Map Register
#define READ_MAP		0x04
// PI
#define PI 3.14159265358

// Datatypes
typedef unsigned char byte;
typedef unsigned int word;
typedef unsigned long dword;

typedef struct tagTILE {
	word height;
	word width;
	byte *data;
} TILE;

// FOR BITMAP FILES!!!
typedef struct tagBITMAPFILEHEADER {
	word	bfType;
	dword	bfSize;
	word	bfReserved1;
	word	bfReserved2;
	dword	bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
	dword	biSize;
	dword biWidth;
	dword	biHeight;
	word	biPlanes;
	word	biBitCount;
	dword	biCompression;
	dword	biSizeImage;
	dword	biXPelsPerMeter;
	dword	biYPelsPerMeter;
	dword	biClrUsed;
	dword	biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagRGBQUAD {
	byte	rgbBlue;
	byte	rgbGreen;
	byte	rgbRed;
	byte	rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPINFO {
	BITMAPINFOHEADER	bmiHeader;
	RGBQUAD				bmiColors[256];
} BITMAPINFO;

typedef struct tagBITMAP {
	BITMAPFILEHEADER	bmFileHeader;
	BITMAPINFO			bmInfo;
	byte					*bmData;
} BITMAP;
// DONE WITH BITMAP FILE INFO

// Monitor settings for ModeX
struct tagCRTParms {
	word V_Total;			// Vertical Total
	word OverFlow;			// Overflow
	word CellHeight;		// Cell Height
	word V_Sync_Start;	// Vertical Sync Start
	word V_Sync_End;		// Vertical Sync End
	word V_Disp;			// Vertical Displayed
	word D_Off;				// Turn off dword mode
	word V_Blank_Start;	// Vertical Blank Start
	word V_Blank_End;		// Vertical Blank End
	word B_On;				// Turn byte mode on
} CRTParms;

byte VIDEO_MODE;	// The current video mode
word MAX_X;			// Last available X
word MAX_Y;			// Last available Y
word SCREEN_WIDTH;
word SCREEN_HEIGHT;
byte MAX_COLORS;	// Last available color
byte FG_COLOR;		// For text and mono stuff
byte BG_COLOR;
word CUR_X;			// Current X
word CUR_Y;			// Current Y
word visibleStart;// Start offset in video memory of screen
word activeStart; // Start offset in video memory to write to

// For rectangle filling (ModeX)
byte LeftClipPlaneMask[] = { 0x0F, 0x0E, 0x0C, 0x08 };
byte RightClipPlaneMask[] = { 0x0F, 0x01, 0x03, 0x07 };

// In case things go awry
char msg_error[81];

void exit_error(void)
{
	printf("%s\a\n", msg_error);
}

////////////////////////////////////////
// init_modeX
//
// -Initialized modeX 320x240x256 graphics
// -Clears video memory
// -Sets page 1 active at offset 0
////////////////////////////////////////
void init_modeX(void)
{
	// Fill the CRT Parameters
	CRTParms.V_Total = 0x0D06;
	CRTParms.OverFlow = 0x3E07;
	CRTParms.CellHeight = 0x4109;
	CRTParms.V_Sync_Start = 0xEA10;
	CRTParms.V_Sync_End = 0xAC11;
	CRTParms.V_Disp = 0xDF12;
	CRTParms.D_Off = 0x0014;
	CRTParms.V_Blank_Start = 0xE715;
	CRTParms.V_Blank_End = 0x0616;
	CRTParms.B_On = 0xE317;

	asm {
		mov ax, 0x0013       // Start in mode 13h
		int 0x10					// Call interrupt

		mov dx, SC_INDEX		// Disable Chain-4 Memory model
		mov ax, 0x0604
		out dx, ax
		mov ax, 0x0100			// Synchronous reset while switching clocks
		out dx, ax

		mov dx, MISC_OUTPUT	// Select 28MHz dot clock, 60 Hz scan rate
		mov al, 0xE3
		out dx, al

		mov dx, SC_INDEX		// Restart Sequencer
		mov ax, 0x0300
		out dx, ax

		mov dx, CRTC_INDEX	// Reprogram the CRT Controller
		mov al, 0x11			// Select Write-Protect bit
		out dx, al
		inc dx					// CRT-Data register into DX now
		in al, dx				// Get current VSync End Register setting
		and al, 0x7F         // Remove write-protect on various registers
		out dx, al
		dec dx					// CRT-Controller
		cld						// Clear the direction
		mov si, OFFSET CRTParms	// Load settings
		mov cx, 20				// 20 Entries
	}
	SetCRTParmsLoop:asm {
		lodsw						// Load the addr:data pair
		out dx, ax				// Send them out
		loop SetCRTParmsLoop	// Do it for each one

		mov dx, SC_INDEX		// Enable write to all bitplanes
		mov ax, 0x0F02
		out dx, ax
		mov ax, MCGA_MEM		// Load video memory
		mov es, ax
		mov di, 0            // Start at the top
		mov ax, 0				// Store 0's
		mov cx, 0x8000			// Do it 8000h times
		rep stosw				// Store them
	}

	// Fill globals
	VIDEO_MODE = 0xFF;
	MAX_X = 319;
	MAX_Y = 239;
	SCREEN_WIDTH = 320;
	SCREEN_HEIGHT = 240;
	MAX_COLORS = 255;
	FG_COLOR = 15;
	BG_COLOR = 0;
	CUR_X = 0;
	CUR_Y = 0;

	activeStart = 0;			// Write to Page 0
	visibleStart = 0;			// Read from Page 0
}

////////////////////////////////////////
//	close_graph
//
//	-Shuts down graphics, sets text mode,
//  and fills appropriate variables
// -Frees memory taken by video buffers
////////////////////////////////////////
void close_graph(void)
{
	asm {
		mov ax, 0x0003
		int 0x10
	}

	VIDEO_MODE = 0x03;
	MAX_X = 80;
	MAX_Y = 25;
	SCREEN_WIDTH = 80;
	SCREEN_HEIGHT = 25;
	MAX_COLORS = 15;
	FG_COLOR = 7;
	BG_COLOR = 0;
	CUR_X = 1;
	CUR_Y = 1;
}

void cleargraph(void)
{
	asm {
		pusha
		push es
		push di

		mov ax, MCGA_MEM
		mov es, ax
		mov di, 0

		mov dx, SC_INDEX		// Enable write to all bitplanes
		mov ax, 0x0F02
		out dx, ax

		mov cx, 0x8000
		mov ax, 0x0000
		rep stosw

		pop di
		pop es
		popa
	}
}

////////////////////////////////////////
// setActiveStart
//
// -Sets the starting memory address
//  at which graphics are written to
////////////////////////////////////////
void setActiveStart(word PageBase)
{
	activeStart = PageBase;
}

////////////////////////////////////////
// setVisibleStart
//
// -Sets the starting memory address
//  at which graphics are displayed from
////////////////////////////////////////
void setVisibleStart(word PageBase)
{
	visibleStart = PageBase;

	asm {
		pusha						// Dont touch!

		mov bx, PageBase		// Load the offset into BX
		mov dx, CRTC_INDEX	// Load the CRT Index Port into DX
		mov al, 0x0C			// Function 0Ch, High Byte of addres
		mov ah, bh				// Load the high byte
		out dx, ax				// Send it
		inc al					// Function 0Dh, Low Byte of address
		mov ah, bl				// Load the low byte
		out dx, ax				// Send it

		popa						// We were never here
	}
}

////////////////////////////////////////
// setActivePage
//
// -Sets page in which graphics are
//  written
////////////////////////////////////////
void setActivePage(byte Page)
{
	// Page = (Page Offset * Width of Screen (Bytes) * Screen Height (Rows))
	setActiveStart(Page * SCR_WIDTH * SCREEN_HEIGHT);
}

////////////////////////////////////////
// setVisiblePage
//
// -Sets page from which graphics are
//  drawn
////////////////////////////////////////
void setVisiblePage(byte Page)
{
	// Page = (Page Offset * Width of Screen (Bytes) * Screen Height (Rows))
	setVisibleStart(Page * SCR_WIDTH * SCREEN_HEIGHT);
}

void vScroll(int rows)
{
	// Scrolling = current start + (rows * bytes in a row)
	setVisibleStart(visibleStart + (rows * SCR_WIDTH));
}

void read_palette(byte *pal)
{
	int i;

	for (i = 0; i < 256; i++)
	{
		asm cli
		outportb(DAC_READ, i);
		pal[(i * 3) + 0] = inportb(VIDEO_DATA);
		pal[(i * 3) + 1] = inportb(VIDEO_DATA);
		pal[(i * 3) + 2] = inportb(VIDEO_DATA);
		asm sti
	}
}

void load_palette(byte *pal)
{
	int i;

	for (i = 0; i < 256; i++)
	{
		asm cli
		outportb(VIDEO_DAC, i);
		outportb(VIDEO_DATA, pal[(i * 3) + 0]);
		outportb(VIDEO_DATA, pal[(i * 3) + 1]);
		outportb(VIDEO_DATA, pal[(i * 3) + 2]);
		asm sti
	}
}

////////////////////////////////////////
// setpalette
//
// -Self explanatory, sets the color
//  of a given index in 256 MCGA mode
////////////////////////////////////////
void setpalette(byte index, byte red, byte green, byte blue)
{
	outportb(VIDEO_DAC, index);
	outportb(VIDEO_DATA, red);
	outportb(VIDEO_DATA, green);
	outportb(VIDEO_DATA, blue);
}

////////////////////////////////////////
// putpixel
//
// -Puts a pixel (in modeX) at (X, Y)
//  in active page
////////////////////////////////////////
void putpixel(word x, word y, byte Color)
{
	asm {
		pusha
		push es
		push di

		mov ax, SCR_WIDTH          // Load screen width, in bytes
		mul y                      // Multiply it by Y to get row
		mov di, x                  // Load X into BX
		shr di, 2						// Divide X by 4 to get plane
		add di, ax						// Add to col offset, row offset
		add di, activeStart			// Add the page offset
		mov ax, MCGA_MEM				// Load video memory into AX
		mov es, ax						// Then ES

		mov cx, x			         // Get X pos
		and cl, 0x03					// Get X plane
		mov ax, 0x0100 + MAP_MASK	// Load Map-Mask register
		shl ah, cl						// Write to only the desired plane
		mov dx, SC_INDEX				// Tell the Map-Mask to do the same
		out dx, ax						// Do it

		mov al, Color					// Load the color
		stosb								// Store it

		pop di
		pop es
		popa
	}
}

////////////////////////////////////////
// getpixel
//
// -Returns color of pixel (in ModeX)
//  at (x, y)
////////////////////////////////////////
byte getpixel(word x, word y)
{
	byte returnvalue;       // To hold color

	asm {
		pusha						// Dont touch anything
		push es
		push di
		mov ax, SCR_WIDTH		// Move screen width (in bytes) into AX
		mul y						// Multiply by y to get row
		mov bx, x				// Move x into BX
		shr bx, 2				// Divide by 4 to get plane
		add bx, ax				// Add the two offsets together
		add bx, activeStart	// Add the PageBase
		mov ax, MCGA_MEM		// Video Memory into AX
		mov es, ax				// Now into ES

		mov ah, byte ptr x	// X into AH
		and ah, 0x03			// Get Plane
		mov al, READ_MAP		// Tell the Video Card
		mov dx, GC_INDEX
		out dx, ax				// Do it

		mov al, es:[bx]		// Get the color
		sub ah, ah				// Clear AH
		mov returnvalue, al	//	Store the color into returnvalue
		pop di
		pop es
		popa						// Put everything back
	}

	return returnvalue;		// Return color
}

////////////////////////////////////////
// vline
//
// -Draws a vertical line from (x,y1)-(x,y2)
//  of color in activePage
////////////////////////////////////////
void vline(word y1, word y2, word x, byte color)
{
	asm {
		pusha                // Dont touch anything here
		push es
		push di
		push si					// For lowY

		mov si, y1				// Move y1 into SI
		mov bx, y2				// y2 into BX
		cmp si, bx				// Compare the two
		jle vline_ordered		// IF y1 <= y2, skip the next lines

		xchg si, bx				// ELSE switch them
	}
	vline_ordered:asm {
		mov ax, MCGA_MEM	  	// Load the video memory into AX
		mov es, ax				// then into ES
		mov ax, SCR_WIDTH		// Load screen width (bytes) into AX
		mul si					// and Multiply by the y1 value
		mov di, ax				// Move it into DI
		mov ax, x				// x into AX
		shr ax, 2				// Divided by 4
		add di, ax				// and added to get first point offset

		mov dx, SC_INDEX		// Point to Sequence Controller
		mov al, MAP_MASK		// Select Map Mask Register (0x02)
		out dx, al				// Do it
		inc dx					// Next Register (SC Data)
		mov cl, byte ptr x	// x into CL
		and cl, 0x03			// x & 0011b to get plane
		mov al, 0x01         // Only one plane, so load 0001b
		shl ax, cl				// Shift it left by the plane number
		out dx, al				// Send it

		mov cx, bx				// Load highY into CX
		sub cx, si				// Subtract lowY to get ydelta
		inc cx					// Add one

		mov al, color			// Load the color into AL
	}
	vline_row_top:asm {
		stosb						// Store the color
		add di, SCR_WIDTH		// Add a row
		dec di					// Subtract one, because STO incs DI
		loop vline_row_top	// Loop until done

		pop si
		pop di
		pop es
		popa                 // Put everything back
	}
}

////////////////////////////////////////
// hline
//
// -Draws a horizontal line from
//  (x1, y)-(x2, y) of color in activePage
// -Faster than a speeding bullet! (Draws
//  four pixels at a time by turning on
//  all bitplanes for read at once.
// -This function is ALL MINE!!! Not one
//  bit of a copy. :)
////////////////////////////////////////
void hline(word x1, word x2, word y, byte color)
{
	asm {
		pusha                      // Dont touch anything
		push es
		push di
		push si							// For lowX

		mov ax, MCGA_MEM				// Load the graphics memory
		mov es, ax

		mov ax, SCR_WIDTH				// Screen width (bytes) into AX
		mul y								// Multiply by y to get row offset
		mov di, ax						// Move row offset into DI

		mov si, x1						// This was a pain to order the variables
		mov cx, x2						// Load x1 into SI, x2 into CX
		cmp si, cx						// Compare them
		jle hline_ordered				// If x1 is <= x2, skip the next stuff

		xchg si, cx						// ELSE switch them
	}
	hline_ordered:asm {
		mov ax, si  					// lowX into AX
		shr ax, 2						// Divided by 4
		add di, ax						// Add the lowX offset to DI

		mov ax, si						// lowX back into AX
		and ax, 0x0003					// lowX AND 0011b to get left bit planes
	}
	_BH = LeftClipPlaneMask[_AX];	//	Load the left mask into BH
	asm {
		mov dx, SC_INDEX				// Sequence Controller into DX
		mov al, MAP_MASK				// Map Mask (Function 0Ch) into AL
		out dx, al						// Send it
		inc dx							// SC Data Register
		mov al, bh						// Load the mask into AL
		out dx, al						// Send it

		mov al, color					// Load color into AL
		stosb								// Store the first byte of line

		mov dx, SC_INDEX           // Loading a mask again
		mov al, MAP_MASK				// ...
		out dx, al						// ...
		inc dx							// ... as before
		mov al, 0x0F					// Turn all bits on, we want MAX SPEED!
		out dx, al						// Send it

		push cx							// Store the highX
		sub cx, si						// Subtract the lowX from highX
		shr cx, 2						// Divide it by 4
		dec cx							// Minus the last byte

		mov al, color					// Load the color into AL
		rep stosb						// Repeat the storing of color

		pop cx							// Get out highX back
		inc cl							// Increment, BUG FIX : Not sure why
		and cl, 0x03					// AND 0011b to get bit planes
	}
	_BL = RightClipPlaneMask[_CL];// Load the right mask into BL
	asm {
		mov dx, SC_INDEX				// Again, we send the mask to the
		mov al, MAP_MASK				// VGA card
		out dx, al						// ...
		inc dx							// ...
		mov al, bl						// ...
		out dx, al						// ... We have seen this already...

		mov al, color					// Load the color into AL
		stosb								// Store the last byte

		pop si
		pop di
		pop es
		popa								// Put everything back, we're done!
	}
}

////////////////////////////////////////
// line
//
// -Draws a line from (x1, y1)-(x2, y2)
//  of color in the active page
// -Uses Bresenhem's Line routine
////////////////////////////////////////
void line(word x1, word y1, word x2, word y2, byte color)
{
	int dx, dy;
	int xinc, yinc;
	int P_right;
	int P_right_up;
	int P;

	if (x1 == x2)
		vline(y1, y2, x1, color);
//	if (y1 == y2)
//		hline(x1, x1, y1, 0, color);

	dx = abs(x2 - x1);
	dy = abs(y2 - y1);

	xinc = (x1 > x2) ? -1 : 1;
	yinc = (y1 > y2) ? -1 : 1;

	if (dx > dy)
	{
		P_right = (2 * dy);
		P_right_up = P_right - (2 * dx);
		P = P_right - dx;

		for (; dx >= 0; dx--)
		{
			putpixel(x1, y1, color);
			if (P > 0)
			{
				x1 += xinc;
				y1 += yinc;
				P += P_right_up;
			}
			else
			{
				x1 += xinc;
				P += P_right;
			}
		}
	}
	else
	{
		P_right = (2 * dx);
		P_right_up = P_right - (2 * dy);
		P = P_right - dy;

		for (; dy >= 0; dy--)
		{
			putpixel(x1, y1, color);
			if (P > 0)
			{
				x1 += xinc;
				y1 += yinc;
				P += P_right_up;
			}
			else
			{
				y1 += yinc;
				P += P_right;
			}
		}
	}
}

void rect(word x1, word y1, word x2, word y2, byte color)
{
	// Just draw the four lines, x is longer because hline is a
	// faster routine than vline.
	hline(x1, x2, y1, color);
	hline(x1, x2, y2, color);
	vline(y1 + 1, y2 - 1, x1, color);
	vline(y1 + 1, y2 - 1, x2, color);
}

////////////////////////////////////////
// fillrect
//
// -Fills a rectangle (in ModeX) of color
//  from (x1, y1) to (x2, y2)
//
// *Adopted from Dr. Dobbs Journal Sep 91
////////////////////////////////////////
void fillrect(word x1, word y1, word x2, word y2, byte color)
{
	asm {
		pusha                         // Dont touch anything
		push ds
		push si
		push es
		push di
		push bp

		cld									// Clear direction flag (left to right)
		mov ax, SCR_WIDTH					// Screen Width (bytes) into AX
		mul y1								// Times y1 value
		mov di, x1							// x1 into DI
		shr di, 2							// Divided by four to get plane
		add di, ax							// Add the y offset
		add di, activeStart				// Add the PageBase
		mov ax, MCGA_MEM					// Now set up the video memory
		mov es, ax							// into ES
		mov dx, SC_INDEX					// Point to Sequence Controller
		mov al, MAP_MASK					// Select Map Mask Register (0x02)
		out dx, al							// Do it
		inc dx								// Next Register (SC Data)
		mov si, x1							// x1 into SI
		and si, 0x0003						// x1 & 0011b to get plane
	}
	_BH = LeftClipPlaneMask[_SI];		// Select appropriate mask
	asm {
		mov si, x2							// Do the same for x2
		and si, 0x0003						// x2 & 0011b
	}
	_BL = RightClipPlaneMask[_SI];	// Mask select
	asm {
		mov cx, x2                    // x2 into CX
		mov si, x1							// x1 into SI
		cmp cx, si							// Compare them
		jle FillDone						// IF CX <= SI jump to FillDone
		dec cx                        // Decrement CX
		and si, 0xFFFC                // x1 & 1111111111111100
		sub cx, si                    // Subtract x1 from x2
		shr cx, 2                     // Divide CX by 4
		jnz MasksSet						// Jump if not zero to MasksSet
		and bh, bl							// BH = BH and BL
	}
	MasksSet:asm {
		mov si, y2							// y2 into SI
		sub si, y1                    // Subtract y1
		jle FillDone                  // IF SI <= 0 jump to FillDone
		mov ah, byte ptr color			// Load color
		mov bp, SCR_WIDTH					// Load screen width (bytes)
		sub bp, cx							// Subtract xdelta from width
		dec bp								// Minus 1
	}
	FillRowsLoop:asm {
		push cx                       // Dont hurt CX
		mov al, bh							// Left Clip Mask into AL
		out dx, al							// Send it out
		mov al, ah							// Color to AL
		stosb									// Send it out
		dec cx								// Minus left edge byte
		js FillLoopBottom					// Jump if SIGNAL flag NOT set
		jz DoRightEdge						// If that's it, do the right.
		mov al, 0x0F						// Turn all bit planes on
		out dx, al							// Do it
		mov al, ah							// Load color into AL
		rep stosb							// Store it over and over
	}
	DoRightEdge:asm {
		mov al, bl							// Right Clip Mask into AL
		out dx, al							// Do it
		mov al, ah							// Load color into AL
		stosb									// Store it
	}
	FillLoopBottom:asm {
		add di, bp							// Next scan line
		pop cx								// What row?
		dec si								// One less row
		jnz FillRowsLoop					// If not zero, go again
	}
	FillDone:asm {
		pop bp
		pop di
		pop es
		pop si
		pop ds
		popa       							// All done, put everything back
	}
}

void circle(word x, word y, word radius, byte color)
{
	float angle;
	float ainc = 1;
	word xc, yc;

	if (radius > 119)			// IF the radius is greater than 119, make the
		ainc = 0.25;			// increment smaller
	if (radius > 61)			// IF it's bigger than 61
		ainc = 0.5;				// do the same


	// Only calculate 1/8th of the circle
	// Four quadrants = (x, y), (-x, y), (-x, -y), (x, -y)
	// Eight octants are made by switching x's and y's.
	// Draw eight pixels per iteration and make the circle
	// eight times faster than conventional routines.
	for (angle = 0; angle <= 45; angle += ainc)
	{
		xc = (word)floor(cos((angle * PI) / 180) * radius);
		yc = (word)floor(sin((angle * PI) / 180) * radius);

		putpixel(x + xc, y + yc, color);
		putpixel(x + xc, y - yc, color);
		putpixel(x - xc, y + yc, color);
		putpixel(x - xc, y - yc, color);

		putpixel(x + yc, y + xc, color);
		putpixel(x + yc, y - xc, color);
		putpixel(x - yc, y + xc, color);
		putpixel(x - yc, y - xc, color);
	}
}

void load_BMP_palette(BITMAP *s)
{
	int i;

	for (i = 0; i < 256; i++)
		setpalette(i, (byte)floor((s->bmInfo.bmiColors[i].rgbRed / 63) * 255),
						  (byte)floor((s->bmInfo.bmiColors[i].rgbGreen / 63) * 255),
						  (byte)floor((s->bmInfo.bmiColors[i].rgbBlue / 63) * 255));
}

////////////////////////////////////////
// load_BMP
//
// -Opens a bitmap (Non-Compressed) and
//  stores the data into a given BITMAP
//  variable
// -Allocate memory BEFORE calling this
//  function for the BITMAP structure
// -Must be 256 Color, Columns divisible
//  by four
////////////////////////////////////////
void load_BMP(char *filename, TILE *target)
{
	FILE *inBMP;											// Create file handle
	BITMAP *tempBMP;
	int color;

	if ((inBMP = fopen(filename, "r+b")) == NULL)// Open it
	{
		sprintf(msg_error, "Cannot open %s", filename);
		atexit(exit_error);								// IF error, exit and
		exit(1);												// tell user.
	}

	tempBMP = (BITMAP *)malloc(sizeof(BITMAP) - 1);

	fseek(inBMP, 0, 0);                          // Goto the beginning
	fread(tempBMP, sizeof(BITMAP) - 1, 1, inBMP);		// And load it all into
																// the given variable

	fseek(inBMP, 54, 0);                         // Goto color offset
	for (color = 0; color < 256; color++)        // and read 'em in.
	{
		tempBMP->bmInfo.bmiColors[color].rgbBlue = fgetc(inBMP);
		tempBMP->bmInfo.bmiColors[color].rgbGreen = fgetc(inBMP);
		tempBMP->bmInfo.bmiColors[color].rgbRed = fgetc(inBMP);
		tempBMP->bmInfo.bmiColors[color].rgbReserved = fgetc(inBMP);
	}

	load_BMP_palette(tempBMP);

	// Allocate space for the data to hold the pixels
	// Goto the pixel offset in the file
	// Read in the pixels
	target->data = (byte *)calloc((tempBMP->bmFileHeader.bfSize - tempBMP->bmFileHeader.bfOffBits), 1);
	target->height = tempBMP->bmInfo.bmiHeader.biHeight;
	target->width = tempBMP->bmInfo.bmiHeader.biWidth;
	fseek(inBMP, tempBMP->bmFileHeader.bfOffBits, 0);
	fread(target->data, target->height * target->width, 1, inBMP);

	free(tempBMP);
	fclose(inBMP);                               // Dont forget to close
}

void show_BMP(TILE *source, word x, word y)
{
	word row, col, offset = 0;

	for (row = source->height; row > 0; row--)
	{
		for (col = 0; col < source->height; col++)
		{
			if (source->data[offset] != 255)
				putpixel(x + col, y + row, source->data[offset]);
			offset++;
		}
	}
}

void show_tile(TILE *tile, word x, word y)
{
	word cx, cy;

	for (cy = 1; cy <= tile->height; cy++)
		for (cx = 0; cx < tile->width; cx++)
			if (tile->data[cx + (cy * tile->width)] != 255)
				putpixel(x + cx, y + cy, tile->data[cx + (cy * tile->width)]);
}

void make_tile(TILE *target, TILE *source)
{
	int row, col, offset = 0;

	target->data = (byte *)malloc(source->height * source->width + source->width);

	target->height = source->height;
	target->width = source->width;

	for (row = source->height * source->width; row >= 0; row -= source->width)
	{
		for (col = 0; col < source->width; col++)
		{
			target->data[offset] = source->data[col + row];
			offset++;
		}
	}
}

void get_tile(word x1, word y1, word x2, word y2, TILE *target)
{
	int cx, cy, offset = 0;

	target->height = abs(y2 - y1);
	target->width = abs(x2 - x1);

	target->data = (byte *)malloc(target->height * target->width + target->width);

	for (cy = 0; cy <= target->height; cy++)
		for (cx = 0; cx < target->width; cx++)
		{
			target->data[offset] = getpixel(x1 + cx, y1 + cy);
			offset++;
		}
}
