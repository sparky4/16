/*****************************************************************************
VGA graphics demo
Chris Giese <geezer@execpc.com>	http://my.execpc.com/~geezer
Release date: ?
This code is public domain (no copyright).
You can do whatever you want with it.

This code uses the BIOS to set graphics mode, and uses the BIOS font.
Should compile cleanly with Turbo C++ 1.0, Turbo C++ 3.0, 16- or 32-bit
Watcom C, or DJGPP. DJGPP version will not work in Windows NT/2000/XP
DOS box.

Some additional things you could do with this:
- Write a function tblit1(), similar to blit1(), that uses an on-off
  transparency mask. Use this function to blit non-rectangular objects
  such as a mouse cursor.
- Write blit_plane(): a fast function to blit from monochrome to monochrome
  or 4-plane bitmaps. Use an external shift() function, written in asm
- Support VBE 1.x banked framebuffer
- Support VBE 2.x linear framebuffer (pmode only, not at A000h:0000)
- Support greater color depths: 15 bpp, 16 bpp, 24 bpp, 32 bpp
- Color reduction, e.g. Heckbert (median-cut) algorithm
- Clipping engine that lets you draw a window that is partially
  obscured by "closer" windows
- Mouse, keyboard, and timer events
- Widgets: push button, checkbox, radio buttons, listbox, dialog, etc.
*****************************************************************************/
#include <string.h> /* [_f]memset() */
/********************************* TURBO C **********************************/
#if defined(__TURBOC__)
#include <dos.h> /* struct REGPACK, intr() */

/* The framebuffer is far outside the 16-bit data segment. The only way to
make the framebuffer work like in-memory bitmaps is to use far pointers.

We still use the SMALL memory model. */
#define	FAR		far
#define	FARPTR(S, O)	MK_FP(S, O)

#define	outportw(P,V)	outport(P,V)

#define	R_AX		r_ax
#define	R_BX		r_bx
#define	R_BP		r_bp
#define	R_ES		r_es

#define	trap(N,R)	intr(N,R)

typedef struct REGPACK regs_t;

#if __TURBOC__<0x300
void vmemset(unsigned char FAR *s, unsigned c, unsigned n)
{
	for(; n != 0; n--)
	{
		*s = c;
		s++;
	}
}
#else
void vmemset(unsigned char FAR *s, unsigned c, unsigned n)
{
	_fmemset(s, c, n);
}
#endif

/********************************* DJGPP ************************************/
#elif defined(__DJGPP__)
#include <dpmi.h> /* __dpmi_... */
#include <dos.h> /* inportb(), outportb() */

#define	FAR		/* nothing */
#define	FARPTR(S, O)	(unsigned char *)((S) * 16L + (O) + \
				__djgpp_conventional_base)

/* near pointers; not supported in Windows NT/2k/XP DOS box */
#include <sys/nearptr.h> /* __djgpp_conventional_base, __djgpp_nearptr_enable() */
#include <stdio.h> /* printf() */
#include <crt0.h> /* _CRT0_FLAG_NEARPTR, _crt0_startup_flags */

#define	R_AX		x.ax
#define	R_BX		x.bx
#define	R_BP		x.bp
#define	R_ES		x.es

#define	trap(N,R)	__dpmi_int(N,R)

typedef __dpmi_regs	regs_t;

void vmemset(unsigned char FAR *s, unsigned c, unsigned n)
{
	memset(s, c, n);
}

/******************************** WATCOM C **********************************/
#elif defined(__WATCOMC__)
#include <dos.h> /* union REGPACK, MK_FP(), intr() */

#if defined(__386__)
#define	FAR		/* nothing */
#define	FARPTR(S, O)	(unsigned char *)((S) * 16L + (O))

void vmemset(unsigned char FAR *s, unsigned c, unsigned n)
{
	memset(s, c, n);
}

#else
#define	FAR		far
#define	FARPTR(S, O)	MK_FP(S, O)

void vmemset(unsigned char FAR *s, unsigned c, unsigned n)
{
	_fmemset(s, c, n);
}
#endif

#define	inportb(P)	inp(P)
#define	outportb(P,V)	outp(P,V)
#define	outportw(P,V)	outpw(P,V)

#define	R_AX		w.ax
#define	R_BX		w.bx
#define	R_BP		w.bp
#define	R_ES		w.es

/* WARNING: for 32-bit code, unused fields of regs_t
must be zeroed before using this macro */
#define	trap(N,R)	intr(N,R)

typedef union REGPACK	regs_t;

#else
#error Not Turbo C, not DJGPP, not Watcom C. Sorry.
#endif

#include <conio.h> /* getch() */

/* need direct access to some VGA registers to select plane,
enable Mode X, and fix screwy CGA addressing */
#define VGA_SEQ_INDEX		0x3C4
#define VGA_SEQ_DATA		0x3C5
#define VGA_GC_INDEX 		0x3CE
#define VGA_GC_DATA 		0x3CF
#define VGA_CRTC_INDEX		0x3D4
#define VGA_CRTC_DATA		0x3D5

/* bitmap "class" */
typedef struct
{
	unsigned wd, ht;
	unsigned char FAR *raster;
	unsigned fore_color, back_color;
/* "member functions" */
	const struct _driver *ops;
} bmp_t;

typedef struct _driver
{
/* "pure virtual functions": color drivers MUST implement these */
	void (*write_pixel)(bmp_t *bmp, unsigned x, unsigned y, unsigned c);
	unsigned (*read_pixel)(bmp_t *bmp, unsigned x, unsigned y);
/* "virtual functions": drivers MAY implement these, for speed
fill rectangular area with solid color */
	void (*fill_rect)(bmp_t *bmp, int x, int y, int wd, int ht);
/* copy monochrome bitmap to this bitmap (used to display text) */
	void (*blit1)(bmp_t *src, bmp_t *dst, unsigned dst_x, unsigned dst_y);
/* copy all or part of one bitmap to another (both of the same depth) */
	void (*blit)(bmp_t *src, bmp_t *dst, unsigned dst_x, unsigned dst_y);
} ops_t;
/*============================================================================
helper functions
============================================================================*/
/*****************************************************************************
*****************************************************************************/
void set_plane(unsigned p)
{
	static unsigned curr_p = -1u;
/**/
	unsigned char pmask;

	p &= 3;
	if(p == curr_p)
		return;
	curr_p = p;
	pmask = 1 << p;
#if 0
	outportb(VGA_GC_INDEX, 4);
	outportb(VGA_GC_DATA, p);
	outportb(VGA_SEQ_INDEX, 2);
	outportb(VGA_SEQ_DATA, pmask);
#else
/* this is a little faster... */
	outportw(VGA_GC_INDEX, (p << 8) | 4);
	outportw(VGA_SEQ_INDEX, (pmask << 8) | 2);
#endif
}
/*****************************************************************************
fast planar (monochrome or 16-color) rectangle fill
*****************************************************************************/
void fill_plane(bmp_t *bmp, int x, int y, int wd, int ht, unsigned c)
{
	unsigned w, wd_in_bytes, off;
	unsigned char lmask, rmask;
	int x2, y2;

	x2 = x + wd - 1;
	w = (x2 >> 3) - (x >> 3) + 1;
	lmask = 0x00FF >> (x & 7); /* FF 7F 3F 1F 0F 07 03 01 */
	rmask = 0xFF80 >> (x2 & 7);/* 80 C0 E0 F0 F8 FC FE FF */
	if(w == 1)
		lmask &= rmask;
	wd_in_bytes = bmp->wd / 8;
	off = wd_in_bytes * y + x / 8;
	if(c)
/* for each row... */
		for(y2 = y; y2 < y + ht; y2++)
		{
/* do partial byte on left */
			bmp->raster[off] |= lmask;
/* do solid bytes in middle */
			if(w > 2)
				vmemset(bmp->raster + off + 1, 0xFF, w - 2);
/* do partial byte on right */
			if(w > 1)
				bmp->raster[off + w - 1] |= rmask;
/* next row */
			off += wd_in_bytes;
		}
	else
	{
		lmask = ~lmask;
		rmask = ~rmask;
		for(y2 = y; y2 < y + ht; y2++)
		{
			bmp->raster[off] &= lmask;
			if(w > 2)
				vmemset(bmp->raster + off + 1, 0, w - 2);
			if(w > 1)
				bmp->raster[off + w - 1] &= rmask;
			off += wd_in_bytes;
		}
	}
}
/*****************************************************************************
fast planar blit
*****************************************************************************/
void blit_plane(bmp_t *src, bmp_t *dst, unsigned dst_x, unsigned dst_y)
{
/* left as an exercise for the reader :)

You may need an external, assembly-language function to shift (left or
right) a long string of bytes. No need to shift by more than 7 bits. */
}
/*============================================================================
driver for monochrome (1-bit) graphics
============================================================================*/
/*****************************************************************************
*****************************************************************************/
static void write_pixel1(bmp_t *bmp, unsigned x, unsigned y, unsigned c)
{
	unsigned wd_in_bytes;
	unsigned off, mask;

	c = (c & 1) * 0xFF;
	wd_in_bytes = bmp->wd / 8;
	off = wd_in_bytes * y + x / 8;
	x = (x & 7) * 1;
	mask = 0x80 >> x;
	bmp->raster[off] = (bmp->raster[off] & ~mask) | (c & mask);
}
/*****************************************************************************
*****************************************************************************/
static unsigned read_pixel1(bmp_t *bmp, unsigned x, unsigned y)
{
	unsigned wd_in_bytes;
	unsigned off, mask;

	wd_in_bytes = bmp->wd / 8;
	off = wd_in_bytes * y + x / 8;
	x = (x & 7) * 1;
	mask = 0x80 >> x;
	return (bmp->raster[off] & mask) != 0;
}
/*****************************************************************************
*****************************************************************************/
static void fill_rect1(bmp_t *bmp, int x, int y, int wd, int ht)
{
	fill_plane(bmp, x, y, wd, ht, bmp->fore_color & 1);
}
/*****************************************************************************
*****************************************************************************/
const ops_t g_ops1 =
{
	write_pixel1,
	read_pixel1,
	fill_rect1,
	NULL, /* blit1 */
	NULL /* blit */
};
/*============================================================================
driver for 2-bit packed pixel (4-color CGA) graphics
============================================================================*/
/*****************************************************************************
*****************************************************************************/
static void write_pixel2(bmp_t *bmp, unsigned x, unsigned y, unsigned c)
{
	unsigned wd_in_bytes, off, mask;

	c = (c & 3) * 0x55;
	wd_in_bytes = bmp->wd / 4;
	off = wd_in_bytes * y + x / 4;
	x = (x & 3) * 2;
	mask = 0xC0 >> x;
	bmp->raster[off] = (bmp->raster[off] & ~mask) | (c & mask);
}
/*****************************************************************************
*****************************************************************************/
const ops_t g_ops2 =
{
	write_pixel2,
	NULL, /* read_pixel */
	NULL, /* fill_rect */
	NULL, /* blit1 */
	NULL /* blit */
};
/*============================================================================
driver for 4-plane 16-color graphics
============================================================================*/
/*****************************************************************************
*****************************************************************************/
static void write_pixel4p(bmp_t *bmp, unsigned x, unsigned y, unsigned c)
{
	unsigned wd_in_bytes, off, mask, p, pmask;

	wd_in_bytes = bmp->wd / 8;
	off = wd_in_bytes * y + x / 8;
	x = (x & 7) * 1;
	mask = 0x80 >> x;
	pmask = 1;
	for(p = 0; p < 4; p++)
	{
		set_plane(p);
		if(pmask & c)
			bmp->raster[off] |= mask;
		else
			bmp->raster[off] &= ~mask;
		pmask <<= 1;
	}
}
/*****************************************************************************
pixel-by-pixel fill is too slow, so use this optimized function:
*****************************************************************************/
static void fill_rect4p(bmp_t *bmp, int x, int y, int wd, int ht)
{
	unsigned char p, pmask;

	pmask = 1;
	for(p = 0; p < 4; p++)
	{
		set_plane(p);
		fill_plane(bmp, x, y, wd, ht, bmp->fore_color & pmask);
		pmask <<= 1;
	}
}
/*****************************************************************************
*****************************************************************************/
const ops_t g_ops4p =
{
	write_pixel4p,
	NULL, /* read_pixel */
	fill_rect4p,
	NULL, /* blit1 */
	NULL /* blit */
};
/*============================================================================
driver for 8-bit 256-color graphics
============================================================================*/
/*****************************************************************************
*****************************************************************************/
static void write_pixel8(bmp_t *bmp, unsigned x, unsigned y, unsigned c)
{
	unsigned wd_in_bytes;
	unsigned off;

	wd_in_bytes = bmp->wd;
	off = wd_in_bytes * y + x;
	bmp->raster[off] = c;
}
/*****************************************************************************
*****************************************************************************/
static void fill_rect8(bmp_t *bmp, int x, int y, int wd, int ht)
{
	unsigned wd_in_bytes, off, y2;

	wd_in_bytes = bmp->wd;
	off = wd_in_bytes * y + x;
	for(y2 = y; y2 < y + ht; y2++)
	{
		vmemset(bmp->raster + off, bmp->fore_color, wd);
		off += wd_in_bytes;
	}
}
/*****************************************************************************
*****************************************************************************/
const ops_t g_ops8 =
{
	write_pixel8,
	NULL, /* read_pixel */
	fill_rect8,
	NULL, /* blit1 */
	NULL /* blit */
};
/*============================================================================
driver for 8-bit 256-color Mode-X graphics
============================================================================*/
/*****************************************************************************
*****************************************************************************/
static void write_pixel8x(bmp_t *bmp, unsigned x, unsigned y, unsigned c)
{
	unsigned wd_in_bytes;
	unsigned off;

	wd_in_bytes = bmp->wd / 4;
	off = wd_in_bytes * y + x / 4;
	set_plane(x & 3);
	bmp->raster[off] = c;
}
/*****************************************************************************
*****************************************************************************/
const ops_t g_ops8x =
{
	write_pixel8x,
	NULL, /* read_pixel */
	NULL, /* fill_rect */
	NULL, /* blit1 */
	NULL /* blit */
};
/*============================================================================
depth-independent routines, which call the depth-dependent routines
============================================================================*/
/*****************************************************************************
*****************************************************************************/
unsigned read_pixel(bmp_t *bmp, unsigned x, unsigned y)
{
	if(x >= bmp->wd || y >= bmp->ht)
		return 0;
	if(bmp->ops->read_pixel == NULL)
		return 0; /* uh-oh */
	return bmp->ops->read_pixel(bmp, x, y);
}
/*****************************************************************************
*****************************************************************************/
void write_pixel(bmp_t *bmp, unsigned x, unsigned y, unsigned c)
{
	if(x >= bmp->wd || y >= bmp->ht)
		return;
	if(bmp->ops->write_pixel == NULL)
		return; /* uh-oh */
	bmp->ops->write_pixel(bmp, x, y, c);
}
/*****************************************************************************
*****************************************************************************/
void fill_rect(bmp_t *bmp, int x, int y, int wd, int ht)
{
	int x2, y2;

/* clip */
	if(x < 0)
	{
		if(wd + x < 0)
			return;
		wd += x;
		x = 0;
	}
	if(x + wd >= (int)bmp->wd)
	{
		if(x >= (int)bmp->wd)
			return;
		wd = bmp->wd - x;
	}
	if(y < 0)
	{
		if(ht + y < 0)
			return;
		ht += y;
		y = 0;
	}
	if(y + ht >= (int)bmp->ht)
	{
		if(y >= (int)bmp->ht)
			return;
		ht = bmp->ht - y;
	}
/* use fast routine if available */
	if(bmp->ops->fill_rect != NULL)
	{
		bmp->ops->fill_rect(bmp, x, y, wd, ht);
		return;
	}
	for(y2 = y; y2 < y + ht; y2++)
		for(x2 = x; x2 < x + wd; x2++)
			write_pixel(bmp, x2, y2, bmp->fore_color);
}
/*****************************************************************************
*****************************************************************************/
void hline(bmp_t *bmp, int x, int y, unsigned wd)
{
	fill_rect(bmp, x, y, wd, 1);
}
/*****************************************************************************
*****************************************************************************/
void vline(bmp_t *bmp, int x, int y, unsigned ht)
{
	fill_rect(bmp, x, y, 1, ht);
}
/*****************************************************************************
blit1 = blit from monochrome bitmap to bitmap of any color depth
*****************************************************************************/
void blit1(bmp_t *src, bmp_t *dst, unsigned dst_x, unsigned dst_y)
{
	unsigned x, y, c;

/* source bitmap _must_ be monochrome */
	if(src->ops != &g_ops1)
		return;
/* use fast routine if available */
	if(src->ops->blit1 != NULL)
	{
		src->ops->blit1(src, dst, dst_x, dst_y);
		return;
	}
	for(y = 0; y < src->ht; y++)
		for(x = 0; x < src->wd; x++)
		{
			c = read_pixel(src, x, y);
/* xxx - on-off transparency?
			if(c == 0)
				continue; */
			if(c != 0)
				c = dst->fore_color;
			else
				c = dst->back_color;
			write_pixel(dst, dst_x + x, dst_y + y, c);
		}
}
/*****************************************************************************
blit = copy from one bitmap to another, both of the same color depth
*****************************************************************************/
void blit(bmp_t *src, bmp_t *dst, unsigned dst_x, unsigned dst_y)
{
	unsigned x, y, c;

/* they must be the same depth */
	if(src->ops != dst->ops)
		return;
/* use fast routine if available */
	if(src->ops->blit != NULL)
	{
		src->ops->blit(src, dst, dst_x, dst_y);
		return;
	}
	for(y = 0; y < src->ht; y++)
		for(x = 0; x < src->wd; x++)
		{
			c = read_pixel(src, x, y);
			write_pixel(dst, dst_x + x, dst_y + y, c);
		}
}
/*****************************************************************************
find 8x8 font in VGA BIOS ROM
*****************************************************************************/
unsigned char FAR *bios_8x8_font(void)
{
	unsigned char FAR *font;
	regs_t regs;

/* use BIOS INT 10h AX=1130h to find font #3 (8x8) in ROM */
	memset(&regs, 0, sizeof(regs)); /* for Watcom C */
	regs.R_AX = 0x1130;
	regs.R_BX = 0x0300;
	trap(0x10, &regs);
/* CauseWay DOS extender seems to return a selector in ES,
instead of real-mode segment value (usu. 0xC000) */
#if defined(__WATCOMC__)&&defined(__386__)
	font = FARPTR(0xC000, regs.R_BP);
#else
	font = FARPTR(regs.R_ES, regs.R_BP);
#endif
	return font;
}
/*****************************************************************************
*****************************************************************************/
void bputs(bmp_t *bmp, unsigned x, unsigned y, const char *s)
{
	unsigned char FAR *font;
	bmp_t src;

	font = bios_8x8_font();
	src.wd = 8;
	src.ht = 8;
	src.ops = &g_ops1;
	for(; *s != '\0'; s++)
	{
		src.raster = font + 8 * (*s);
		blit1(&src, bmp, x, y);
		x += 8;
	}
}
/*============================================================================
DEMO
============================================================================*/
/*****************************************************************************
*****************************************************************************/
static void border3d(bmp_t *bmp, int x, int y, unsigned wd, unsigned ht,
		char down)
{
	if(down)
	{
		bmp->fore_color = 8;
		hline(bmp, x + 0, y + 0, wd - 1);
		vline(bmp, x + 0, y + 0, ht - 1);
		bmp->fore_color = 0;
		hline(bmp, x + 1, y + 1, wd - 3);
		vline(bmp, x + 1, y + 1, ht - 3);
		bmp->fore_color = 7;
		hline(bmp, x + 1, y + ht - 2, wd - 2);
		vline(bmp, x + wd - 2, y + 1, ht - 2);
		bmp->fore_color = 15;
		hline(bmp, x + 0, y + ht - 1, wd);
		vline(bmp, x + wd - 1, y + 0, ht);
	}
	else
	{
		bmp->fore_color = 7;
		hline(bmp, x + 0, y + 0, wd - 1);
		vline(bmp, x + 0, y + 0, ht - 1);
		bmp->fore_color = 15;
		hline(bmp, x + 1, y + 1, wd - 3);
		vline(bmp, x + 1, y + 1, ht - 3);
		bmp->fore_color = 8;
		hline(bmp, x + 1, y + ht - 2, wd - 2);
		vline(bmp, x + wd - 2, y + 1, ht - 2);
		bmp->fore_color = 0;
		hline(bmp, x + 0, y + ht - 1, wd);
		vline(bmp, x + wd - 1, y + 0, ht);
	}
}
/*****************************************************************************
*****************************************************************************/
static void demo(bmp_t *bmp, const char *title)
{
	unsigned x = 10, y = 10, wd = 180, ht = 50;

/* erase screen to blue */
	bmp->fore_color = 1;
	fill_rect(bmp, 0, 0, bmp->wd, bmp->ht);
/* draw gray window with 3D border */
	bmp->fore_color = 7;
	fill_rect(bmp, x, y, wd, ht);
	border3d(bmp, x, y, wd, ht, 0);
/* draw white-on-green title bar */
	bmp->fore_color = 2;
	fill_rect(bmp, x + 2, y + 2, wd - 4, 10);
	bmp->back_color = 2;
	bmp->fore_color = 15;
	bputs(bmp, x + 3, y + 3, title);
/* draw menu bar on existing gray background */
	bmp->back_color = 7;
	bmp->fore_color = 0;
	bputs(bmp, x + 3, y + 13, "File Edit");
/* draw white inner area with 3D border */
	bmp->fore_color = 15;
	fill_rect(bmp, x + 3, y + 21, wd - 6, ht - 24);
	border3d(bmp, x + 3, y + 21, wd - 6, ht - 24, 1);
/* await key pressed */
	getch();
}
/*****************************************************************************
*****************************************************************************/
int main(void)
{
	static const unsigned wd[] =
	{
		640, 320, 640, 320, 320
	};
	static const unsigned ht[] =
	{
		480, 200, 480, 200, 200
	};
	static const ops_t *ops[] =
	{
		&g_ops1, &g_ops2, &g_ops4p, &g_ops8, &g_ops8x
	};
	static const unsigned mode[] =
	{
		0x11, 5, 0x12, 0x13, 0x13
	};
	static const char *title[] =
	{
		"640x480x2", "320x200x4", "640x480x16", "320x200x256",
			"320x200x256 ModeX"
	};
/**/
	regs_t regs;
	unsigned i;
	bmp_t bmp;

#if defined(__DJGPP__)
	if(!(_crt0_startup_flags & _CRT0_FLAG_NEARPTR))
	{
		if(!__djgpp_nearptr_enable())
		{
			printf("Could not enable nearptr access "
				"(Windows NT/2000/XP?)\n");
		}
	}
#endif
	for(i = 0; i < sizeof(wd) / sizeof(wd[0]); i++)
	{
		bmp.raster = FARPTR(0xA000, 0);
		bmp.wd = wd[i];
		bmp.ht = ht[i];
		bmp.ops = ops[i];

		memset(&regs, 0, sizeof(regs)); /* for Watcom C */
		regs.R_AX = mode[i];
		trap(0x10, &regs);
/* to make CGA graphics work like other graphics modes... */
		if(mode[i] == 0x05)
		{
/* 1) turn off screwy CGA addressing */
			outportb(VGA_CRTC_INDEX, 0x17);
			outportb(VGA_CRTC_DATA, inportb(VGA_CRTC_DATA) | 1);
/* 2) turn off doublescan */
			outportb(VGA_CRTC_INDEX, 9);
			outportb(VGA_CRTC_DATA, inportb(VGA_CRTC_DATA) & ~0x80);
/* 3) move the framebuffer from B800:0000 to A000:0000 */
			outportb(VGA_GC_INDEX, 6);
			outportb(VGA_GC_DATA, inportb(VGA_GC_INDEX) & ~0x0C);
		}
/* to convert mode 13h to Mode X... */
		else if(i == 4)
		{
/* 1) turn off Chain-4 addressing */
			outportb(VGA_SEQ_INDEX, 0x04);
			outportb(VGA_SEQ_DATA, inportb(VGA_SEQ_DATA) & ~0x08);
/* 2) turn off doubleword clocking */
			outportb(VGA_CRTC_INDEX, 0x14);
			outportb(VGA_CRTC_DATA, inportb(VGA_CRTC_DATA) & ~0x40);
/* 3) turn off word clocking in case it's on */
			outportb(VGA_CRTC_INDEX, 0x17);
			outportb(VGA_CRTC_DATA, inportb(VGA_CRTC_DATA) | 0x40);
		}
		demo(&bmp, title[i]);
	}
/* return to text mode */
	memset(&regs, 0, sizeof(regs)); /* for Watcom C */
	regs.R_AX = 0x03;
	trap(0x10, &regs);
	return 0;
}
