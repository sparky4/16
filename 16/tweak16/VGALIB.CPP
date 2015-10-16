#include <dos.h>
#include <conio.h>
#include <string.h>

#include "misc.hpp"
#include "screen.hpp"
#include "vgalib.hpp"


GraphicsAPI::GraphicsAPI(int xr, int yr, int vxr, int vyr, int clrs)
	{
	int segm, offs;
	xres = xr;
	yres = yr;
	vxres = vxr;
	vyres = vyr;
	colors = clrs;
	libID = "Generic Graphics";
	asm	{
		push	bp
		mov		ax, 1130h
		mov		bh, 03h
		int		10h
		mov		ax, bp
		pop		bp
		mov		[segm], es
		mov		[offs], ax
		}
	font = (unsigned char *)MK_FP(segm, offs);
	fontHeight = fontWidth = 8;
	}

void GraphicsAPI::hLine(int x, int y, int l)
	{
	while (l--)
		putPixel(x++, y);
	}

void GraphicsAPI::vLine(int x, int y, int l)
	{
	while (l--)
		putPixel(x, y++);
	}

/*
 *	Generic line drawing routine, using Bresenham's algorithm.
 *	Taken from Richard Wilton: "PC & PS/2 Video Systems" p. 166-7
 */

void GraphicsAPI::line(int x1, int y1, int x2, int y2)
	{
	if (x1==x2)
		{
		sort(y1, y2);
		vLine(x1, y1, y2-y1+1);
		return;
		}
	if (y1==y2)
		{
		sort(x1, x2);
		hLine(x1, y1, x2-x1+1);
		}
	int dx = absolute(x2-x1);
	int dy = absolute(y2-y1);
	if (dx >= dy)
		{
		if (x1>x2)
			{
			swap(x1, x2);
			swap(y1, y2);
			}
		int yincr = 1;
		if (y2<y1)
			yincr = -yincr;
		int d = 2 * dy - dx;
		int aincr = 2 * (dy - dx);
		int bincr = 2 * dy;
		int x = x1;
		int y = y1;
		putPixel(x, y);
		while (++x <= x2)
			{
			if (d >= 0)
				{
				y += yincr;
				d += aincr;
				}
			else
				d += bincr;
			putPixel(x, y);
			}
		}
	else
		{
		if (y1>y2)
			{
			swap(x1, x2);
			swap(y1, y2);
			}                           
		int xincr = 1;
		if (x2<x1)
			xincr = -xincr;
		int d = 2*dx - dy;
		int aincr = 2 * (dx - dy);
		int bincr = 2 * dx;
		int x = x1;
		int y = y1;
		putPixel(x, y);
		while (++y <= y2)
			{
			if (d >= 0)
				{
				x += xincr;
				d += aincr;
				}
			else
				d += bincr;
			putPixel(x, y);
			}
		}
	}

void GraphicsAPI::rectangle(int x1, int y1, int x2, int y2)
	{
	sort(x1, x2);
	sort(y1, y2);
	hLine(x1, y1, x2-x1);
	hLine(x1, y2, x2-x1);
	vLine(x1, y1, y2-y1);
	vLine(x2, y1, y2-y1);
	}

void GraphicsAPI::bar(int x1, int y1, int x2, int y2)
	{
	int width = x2-x1+1;
	for (int y = y1; y < y2; ++y)
		hLine(x1, y, width);
	}

void GraphicsAPI::wipe()
	{
	bar(0, 0, vxres, vyres);
	}

void GraphicsAPI::putChar(int x, int y, int ch)
	{
	unsigned char *fptr = font + fontHeight*ch;
	for (int j=0; j<fontHeight; ++j, ++fptr)
		{
		char mask = *fptr;
		for (int i=0; i<fontWidth; ++i)
			{
			asm shl		[mask], 1;
			asm jnc		skip
				putPixel(x+i, y+j, color);
			skip:
			}
		}
	}

void GraphicsAPI::putText(int x, int y, char *txt)
	{
	switch (hJustify)
		{
		case HCENTER:
			x -= fontWidth * strlen(txt) / 2;
			break;
		case RIGHT:
			x -= fontWidth * strlen(txt);
			break;
		}
	switch (vJustify)
		{
		case VCENTER:
			y -= fontHeight / 2;
			break;
		case BOTTOM:
			y -= fontHeight;
			break;
		}

	while (*txt)
		{
		putChar(x, y, *(txt++));
		x += fontWidth;
		}
	}

void GraphicsAPI::setTextJustify(HJustify h, VJustify v)
	{
	hJustify = h;
	vJustify = v;
	}


/*
 *	VGAGraphicsAPI
 */

unsigned char *VGAGraphicsAPI::videoBuf =
	(unsigned char *)MK_FP(0xa000,0);

VGAGraphicsAPI::VGAGraphicsAPI(int xr, int yr, int vxr, int xb, int clrs)
	: GraphicsAPI(xr, yr, vxr, 65536L/xb, clrs), xbytes(xb)
	{
	libID = "VGA Graphics";
	outp(0x3ce, 0x05);
	int t = inp(0x3cf);
	outpw(0x3ce, 0x05 | (t & 0xfc) << 8);	// write mode 0
	outpw(0x3c4, 0x0f02);					// enable all planes
	}


void VGAGraphicsAPI::syncWithRefresh()
	{
	while (inp(0x3da)&0x8);
	while (!(inp(0x3da)&0x8));
	}

unsigned VGAGraphicsAPI::getOffset(int x, int y)
	{
	return y * xbytes + x/(vxres/xbytes);
	}

void VGAGraphicsAPI::setBase(int x=0, int y=0)
	{
	unsigned offset = getOffset(x,y);
	inp(0x3da);
	outp(0x3c0, 0x33);
	outp(0x3c0, getPelPan(x));
	outpw(0x3d4, 0x0c | (offset & 0xff00));
	outpw(0x3d4, 0x0d | (offset & 0x00ff)<<8);
	syncWithRefresh();
	}


/*
 *	Planar16
 */

Planar16::Planar16(int xr, int yr, int vxr)
	: VGAGraphicsAPI(xr, yr, vxr, vxr>>3, 16)
	{
	libID = "4-plane 16-color mode";
	outpw(0x3ce, 0x0f01);					// enable set/reset
	}

void Planar16::putPixel(int x, int y, int c)
	{
	outpw(0x3ce, 0x00 | (c<<8));		// set/reset to select color
	outpw(0x3ce, 0x08 | 0x8000>>(x&7));	// bit mask to select bit
	unsigned char *pos = graphScr+y*xbytes+(x>>3);
	*pos = *pos;
	}

int Planar16::getPixel(int x, int y)
	{
	return videoBuf[y*xbytes+x];
	}

void Planar16::hLine(int x, int y, int l)
	{
	outpw(0x3ce, 0x00 | (color<<8));	// set/reset to select color
	unsigned char *pos = graphScr+y*xbytes+(x>>3);
	int mask;
	int shift = x & 7;
	if (shift > 0)
		{
		mask = 0x00ff >> shift;
		l -= 8 - shift;
		if (l<0)
			mask &= 0xff << -l;
		outpw(0x3ce, 0x08 | mask << 8);	// bit mask to select first bits
		*pos = *pos;
		++pos;
		}
	if (l >= 8)
		{
		outpw(0x3ce, 0xff08);			// bit mask to select 8 bits
		memset(pos, 0, l>>3);
		pos += l>>3;
		l -= l & 0xf8;
		}
	if (l >= 0)
		{
		mask = 0xff00 << (8-l);
		outpw(0x3ce, 0x08 | mask);		// bit mask to select last bits
		*pos = *pos;
		}
	}

int Planar16::getPelPan(int x)
	{
	return x & 7;
	}

/*
 *	Chained256
 */

Chained256::Chained256(int xr, int yr, int vxr)
	: VGAGraphicsAPI(xr, yr, vxr, vxr, 256)
	{
	libID = "Chained 256-color mode";
	}

void Chained256::putPixel(int x, int y, int c)
	{
	videoBuf[y*xbytes+x] = c;
	}

int Chained256::getPixel(int x, int y)
	{
	return videoBuf[y*xbytes+x];
	}

void Chained256::hLine(int x, int y, int l)
	{
	memset(graphScr+y*xbytes+x, color, l);
	}

unsigned Chained256::getOffset(int x, int y)
	{
	return (y * xbytes + x/(vxres/xbytes)) >> 2; 
	}

int Chained256::getPelPan(int x)
	{
	return 2*(x & 3);
	}


/*
 *	Unchained256
 */

Unchained256::Unchained256(int xr, int yr, int vxr)
	: VGAGraphicsAPI(xr, yr, vxr, vxr>>2, 256)
	{
	libID = "Unchained 256-color mode";
	}

void Unchained256::putPixel(int x, int y, int c)
	{
	outpw(0x3c4, 0x02 | 0x0100<<(x&3));
	videoBuf[y*xbytes+(x>>2)] = c;
	}

int Unchained256::getPixel(int x, int y)
	{
	return videoBuf[y*xbytes+x];
	}

void Unchained256::hLine(int x, int y, int l)
	{
	unsigned char *pos = graphScr+y*xbytes+(x>>2);
	int mask;
	int shift = x & 3;
	if (shift > 0)
		{
		mask = 0x000f << shift & 0x000f;
		l -= 4-shift;
		if (l<0)
			mask &= 0x0f >> -l;
		outpw(0x3c4, 0x02 | mask << 8);	// write enable first pixels
		*(pos++) = color;
		}
	if (l >= 4)
		{
		outpw(0x3c4, 0x0f02);			// write enable 4 pixels
		memset(pos, color, l>>2);
		pos += l>>2;
		l -= l & 0xfc;
		}
	if (l >= 0)
		{
		mask = 0x0f00 >> (4-l) & 0x0f00;
		outpw(0x3c4, 0x02 | mask);		// write enable last pixels
		*pos = color;
		}
	}

int Unchained256::getPelPan(int x)
	{
	return 2*(x & 3);
	}
