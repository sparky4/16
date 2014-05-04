/*----------------------------------------------------------------------------
blit() function for VBE 1.x banked framebuffer
Chris Giese	<geezer@execpc.com>	http://my.execpc.com/~geezer/
This code is public domain (no copyright).
You can do whatever you want with it.

Compile with any of the following DOS compilers:
- Turbo C++ 1.0
- Borland C++ 3.1
- 16-bit Watcom C
- DJGPP

April 4, 2008
- Initial release

More notes:
- The 'BPP' macro can be changed to 2, 3, or 4 to work with color
  depths 16, 24, and 32, respectively. BPP can also be a run-
  time variable; letting you use the same blit() function for
  various color depths.
- Some video boards (Intel i810 integrated video) report compliance
  with VBE 2.x but _still_ do not support a linear framebuffer
- Left as an exercise for the reader: it should be easy to convert
  blit_mem_to_fb() to blit_fb_to_mem()
- Left as an exercise for the reader: it should be easy to convert
  blit_mem_to_fb() to blot_fb(); a function that draws a solid
  filled rectangle. (Hint: replace MEMCPY()s with MEMSET()s)
- To handle arbitrary blitting in a system with a banked framebuffer,
  you need four functions: blit_mem_to_mem(), blit_mem_to_fb(),
  blit_fb_to_mem(), and blit_fb_to_fb()
----------------------------------------------------------------------------*/
#include <string.h> /* memset(), [_f]memcpy() */
/* EOF, FILE, fopen(), fwrite(), fputc(), fclose(), remove(), printf() */
#include <stdio.h>
#include <conio.h> /* getch(), outp[ortb]() */
/* union REGS, struct SREGS, int86(), int86x() */
#include <dos.h> /* FP_SEG(), FP_OFF(), MK_FP() */
#if 0
/* C99 fixed-width types */
#include <stdint.h>
#else
typedef unsigned char	uint8_t;
typedef unsigned short	uint16_t;
typedef unsigned long	uint32_t;
#endif

#if defined(__TURBOC__)
#if __TURBOC__==0x401
#error Sorry, 'huge' is broken in Turbo C++ 3.0
#endif

/* This code exposes some bugs in Borland C++ 3.1, so... */
#if __TURBOC__==0x410
#pragma option -Od /* ...disable all optimizations */
#endif

#define	HUGE		huge
#define	MEMCPY(D,S,N)	_fmemcpy(D,S,N)
#define	MEMSET(D,C,N)	_fmemset(D,C,N)

#elif defined(__WATCOMC__)&&!defined(__386__)
#define	outportb(P,V)	outp(P,V)
#define	HUGE		huge
#define	MEMCPY(D,S,N)	_fmemcpy(D,S,N)
#define	MEMSET(D,C,N)	_fmemset(D,C,N)

#elif defined(__DJGPP__)
/* __djgpp_conventional_base, __djgpp_nearptr_enable() */
#include <sys/nearptr.h>
#include <dpmi.h> /* __dpmi_regs, __dpmi_int(), __dpmi_allocate_dos_memory() */
#include <crt0.h> /* _CRT0_FLAG_NEARPTR, _crt0_startup_flags */

#define	__386__		1
#define	HUGE		/* nothing */
#define	MEMCPY(D,S,N)	memcpy(D,S,N)
#define	MEMSET(D,C,N)	memset(D,C,N)

#else
#error Sorry, unsupported compiler
#endif

#define MIN(X,Y) 	(((X) < (Y)) ? (X) : (Y))

/* structure used by INT 10h AX=4F01h */
#pragma pack(1)
typedef struct
{
	uint16_t mode_attrib;	 /* b5=1 for non-VGA mode */
	uint8_t win_a_attrib;
	uint8_t win_b_attrib;
	uint16_t k_per_gran;
	uint16_t win_size;
	uint16_t win_a_seg;
	uint16_t win_b_seg;
	char reserved1[4];
/* this is not always the expected value;
rounded up to the next power of 2 for some video boards: */
	uint16_t bytes_per_row;
/* OEM modes and VBE 1.2 only: */
	uint16_t wd;
	uint16_t ht;
	uint8_t char_wd;
	uint8_t char_ht;
	uint8_t planes;
	uint8_t depth;
	uint8_t banks;
	uint8_t memory_model;
	uint8_t k_per_bank;
	uint8_t num_pages;	 /* ? */
	char reserved2;
/* VBE 1.2 only */
	uint8_t red_width;
	uint8_t red_shift;
	uint8_t green_width;
	uint8_t green_shift;
	uint8_t blue_width;
	uint8_t blue_shift;
	char reserved3[3];
	uint32_t lfb_adr;
	char reserved4[212];
} vbe_mode_info_t;

typedef struct
{
	unsigned wd, ht;
	unsigned long bytes_per_row;
	unsigned char HUGE *raster;
} img_t;

typedef struct
{
	int src_x, src_y, dst_x, dst_y;
	unsigned wd, ht;
} clip_t;

static img_t g_fb;
static unsigned g_use_win_a, g_gran_per_64k;
/*****************************************************************************
no _fmemcpy() in Turbo C++ 1.0
*****************************************************************************/
#if defined(__TURBOC__)
#if __TURBOC__<0x300
void far * far _fmemcpy(void far *dst_ptr, const void far *src_ptr,
		unsigned n)
{
	unsigned char huge *dst = dst_ptr;
	unsigned char huge *src = src_ptr;

	for(; n != 0; n--)
	{
		*dst = *src;
		dst++;
		src++;
	}
	return dst_ptr;
}
#endif
#endif
/*****************************************************************************
*****************************************************************************/
static void set_bank(unsigned b)
{
	static unsigned curr_bank = -1u;
/**/
	union REGS regs;

	if(b == curr_bank)
		return;
	curr_bank = b;
	regs.x.ax = 0x4F05;
/* g_use_win_a and g_gran_per_64k were set by INT 10h AX=4F01h */
	regs.x.bx = g_use_win_a ? 0x0000 : 0x0001;
	regs.x.dx = b * g_gran_per_64k;
	int86(0x10, &regs, &regs);
}
/*****************************************************************************
If using Borland C, compile this without optimizations.
Even without optimizations, it probably won't work with Turbo C++ 3.0
*****************************************************************************/
#define BPP	1 /* bytes per pixel */
static void blit_mem_to_fb(img_t *src_img, clip_t *clip)
{
	unsigned bank, y, row_with_bank_switch, max_y;
	unsigned char HUGE *src;
	unsigned long off32;
	uint16_t off16, i;

	src = src_img->raster +
		src_img->bytes_per_row * clip->src_y + clip->src_x * BPP;
/* calculate 32-bit offset into framebuffer corresponding to
(clip->dst_x, clip->dst_y) */
	off32 = g_fb.bytes_per_row * clip->dst_y + clip->dst_x * BPP;
/* use low 16 bits for offset into 64K bank; use top 16 bits for bank */
	off16 = (uint16_t)off32;
	off32 >>= 16;
	bank = (uint16_t)off32;
/* set bank and increment bank number */
	set_bank(bank);
	bank++;
	for(y = clip->dst_y; ; )
	{
/* in which row does the next bank-switch occur? */
		off32 = bank;
		off32 <<= 16;
		row_with_bank_switch = (unsigned)
			(off32 / g_fb.bytes_per_row);
/* blit until we reach that row or until we reach (clip->dst_y + clip->ht) */
		max_y = MIN(clip->dst_y + clip->ht, row_with_bank_switch);
		for(; y < max_y; y++)
		{
			MEMCPY(g_fb.raster + off16, src, clip->wd * BPP);
			off16 += g_fb.bytes_per_row;
			src += src_img->bytes_per_row;
		}
/* exit now if done */
		if(y >= clip->dst_y + clip->ht)
			break;
/* else it's a row with bank switch. There are 3 possibilities:
line of pixels in this row starts AFTER bank switch */
		if(off16 < g_fb.bytes_per_row) /* overflowed 64K (clever!) */
		{
			set_bank(bank);
			MEMCPY(g_fb.raster + off16, src, clip->wd * BPP);
		}
/* line of pixels in this row ends BEFORE bank switch */
		else if(off16 + (unsigned long)clip->wd * BPP <= 0x10000L)
		{
			MEMCPY(g_fb.raster + off16, src, clip->wd * BPP);
			set_bank(bank);
		}
/* line of pixels in this row STRADDLES bank switch */
		else
		{
			i = (uint16_t)(0x10000uL - off16);
			MEMCPY(g_fb.raster + off16, src + 0,
				i);
			set_bank(bank);
			MEMCPY(g_fb.raster + 0    , src + i,
				clip->wd * BPP - i);
		}
		off16 += g_fb.bytes_per_row;
		src += src_img->bytes_per_row;
		bank++;
		y++;
	}
}
/*****************************************************************************
*****************************************************************************/
int main(void)
{
	static const unsigned vbe_mode_num = 0x101; /* 640x480x256 */
/* 14x14 test bitmap */
	static unsigned char raster[] =
		"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D"
		"\x01\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D"
		"\x02\x02\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D"
		"\x03\x03\x03\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D"
		"\x04\x04\x04\x04\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D"
		"\x05\x05\x05\x05\x05\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D"
		"\x06\x06\x06\x06\x06\x06\x06\x07\x08\x09\x0A\x0B\x0C\x0D"
		"\x07\x07\x07\x07\x07\x07\x07\x07\x08\x09\x0A\x0B\x0C\x0D"
		"\x08\x08\x08\x08\x08\x08\x08\x08\x08\x09\x0A\x0B\x0C\x0D"
		"\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x0A\x0B\x0C\x0D"
		"\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x0B\x0C\x0D"
		"\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0C\x0D"
		"\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0D"
		"\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D";
/**/
	union REGS regs;
	unsigned i;
	clip_t clip;
	img_t img;

#if !defined(__386__)
	static vbe_mode_info_t vbe_mode_info;
	struct SREGS sregs;

/* get info for VBE mode */
	regs.x.ax = 0x4F01;
	regs.x.cx = vbe_mode_num;
	sregs.es = FP_SEG(&vbe_mode_info);
	regs.x.di = FP_OFF(&vbe_mode_info);
	int86x(0x10, &regs, &regs, &sregs);
	if(regs.x.ax != 0x004F)
	{
		printf("Error getting info for VBE mode 0x%X\n",
			vbe_mode_num);
		return 1;
	}
/* init g_fb */
	g_fb.wd = vbe_mode_info.wd;
	g_fb.ht = vbe_mode_info.ht;
	g_fb.bytes_per_row = vbe_mode_info.bytes_per_row;
	g_gran_per_64k = 64 / vbe_mode_info.k_per_gran;
	if(vbe_mode_info.win_a_attrib == 7)
	{
		g_fb.raster = (unsigned char HUGE *)
			MK_FP(vbe_mode_info.win_a_seg, 0);
		g_use_win_a = 1;
	}
	else if(vbe_mode_info.win_b_attrib == 7)
	{
		g_fb.raster = (unsigned char HUGE *)
			MK_FP(vbe_mode_info.win_b_seg, 0);
		g_use_win_a = 0;
	}
	else
	{
		printf("Error locating banked framebuffer "
			"for VBE mode 0x%X\n", vbe_mode_num);
		return -1;
	}
#else /* if defined(__DJGPP__) */
	vbe_mode_info_t *vbe_mode_info;
	int conv_mem_seg, conv_mem_sel;
	__dpmi_regs dregs;

/* turn off data segment limit, for nearptr access */
	if(!(_crt0_startup_flags & _CRT0_FLAG_NEARPTR))
	{
		if(!__djgpp_nearptr_enable())
		{
			printf("Error: can't enable near pointer "
				"access to framebuffer (WinNT/2k/XP?)\n");
			return 1;
		}
	}
/* allocate conventional memory for INT 10h AX=4F01h buffer */
	conv_mem_seg = __dpmi_allocate_dos_memory(256 / 16, &conv_mem_sel);
	if(conv_mem_seg == -1)
	{
		printf("Error: can't allocate conventional memory\n");
		return 1;
	}
	vbe_mode_info = (vbe_mode_info_t *)
		(conv_mem_seg * 16uL + __djgpp_conventional_base);
printf("vbe_mode_info: sel=0x%X, real-mode seg=0x%X, linear=0x%lX, near=0x%p\n",
 conv_mem_sel, conv_mem_seg, conv_mem_seg * 16uL, vbe_mode_info);
/* get info for VBE mode */
	dregs.x.ax = 0x4F01;
	dregs.x.cx = vbe_mode_num;
	dregs.x.es = conv_mem_seg;
	dregs.x.di = 0;
	__dpmi_int(0x10, &dregs);
	if(dregs.x.ax != 0x004F)
	{
		printf("Error getting info for VBE mode 0x%X\n",
			vbe_mode_num);
		return 1;
	}
/* init g_fb */
	g_fb.wd = vbe_mode_info->wd;
	g_fb.ht = vbe_mode_info->ht;
	g_fb.bytes_per_row = vbe_mode_info->bytes_per_row;
	g_gran_per_64k = 64 / vbe_mode_info->k_per_gran;
	if(vbe_mode_info->win_a_attrib == 7)
	{
		g_fb.raster = (unsigned char HUGE *)
			(vbe_mode_info->win_a_seg * 16uL +
				__djgpp_conventional_base);
		g_use_win_a = 1;
	}
	else if(vbe_mode_info->win_b_attrib == 7)
	{
		g_fb.raster = (unsigned char HUGE *)
			(vbe_mode_info->win_b_seg * 16uL +
				__djgpp_conventional_base);
		g_use_win_a = 0;
	}
	else
	{
		printf("Error locating banked framebuffer "
			"for VBE mode 0x%X\n", vbe_mode_num);
		return -1;
	}
#endif
/* init img */
	img.wd = 14;
	img.ht = 14;
	img.bytes_per_row = img.wd;
	img.raster = raster;
/* init clip_t */
	clip.wd = img.wd;
	clip.ht = img.ht;
	clip.src_x = 0;
	clip.src_y = 0;
/* set graphics mode */
	regs.x.bx = vbe_mode_num;
	regs.x.ax = 0x4F02;
	int86(0x10, &regs, &regs);
/* green palette */
#define	VGA_DAC_WRITE_INDEX	0x3C8
#define	VGA_DAC_DATA		0x3C9
	outportb(VGA_DAC_WRITE_INDEX, 0);
	for(i = 0; i < 16; i++)
	{
		outportb(VGA_DAC_DATA, /* red= */0 >> 2);
		outportb(VGA_DAC_DATA, /* green= */i * 3);
		outportb(VGA_DAC_DATA, /* blue= */0 >> 2);
	}
/* test banked blit function */
	clip.dst_y = 0;
	for(; clip.dst_y + clip.ht < g_fb.ht; clip.dst_y += clip.ht)
	{
		clip.dst_x = 0;
		for(; clip.dst_x + clip.wd < g_fb.wd; clip.dst_x += clip.wd)
			blit_mem_to_fb(&img, &clip);
	}
	if(getch() == 0)
		(void)getch();
/* set text mode */
	regs.x.ax = 0x0003;
	int86(0x10, &regs, &regs);
	return 0;
}
