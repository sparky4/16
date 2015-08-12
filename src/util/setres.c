/*----------------------------------------------------------------------------
Sets text video modes
Chris Giese	<geezer@execpc.com>	http://my.execpc.com/~geezer/
This code is public domain (no copyright).
You can do whatever you want with it.
Release date: March 31, 2009

Compile with Turbo C, Borland C for DOS, or 16-bit Watcom C. I used:
	bcc -O2 -2 -Z -d -mt -f- -w -c setres.c
	tlink /Lc:\bc\lib /x/t c0t.obj setres.obj,setres.com,,cs.lib

On one of my systems, the following video modes are supported:
  40x25[b ] 40x50[b ]  80x25[b ]  80x50[b ] 132x25[v ] 132x43[v ]
  40x30[tb] 40x60[tb]  80x30[tb]  80x60[tb]  90x25[tb]  90x30[tb]
  90x50[tb] 90x60[tb] 132x30[tv] 132x50[tv] 132x60[tv]

[ b]=VGA BIOS (INT 10h AH=00h) modes	[tb]=tweaked VGA BIOS modes
[ v]=VBE BIOS (INT 10h AX=4F0xh) modes	[tv]=tweaked VBE BIOS modes

The 'tweaked' modes start with a VGA or VBE BIOS mode, then write to the
CRTC registers to change the resolution. By default, this will happen only
if INT 10h AX=4F00h reports that the hardware is register-compatible with
VGA and INT 10h AX=4F01h reports that the mode is also register-compatible
with VGA. '-v' option overrides this -- use it at your own risk. Tweaking
does not (should not?) change the sync frequencies to unsupported values.
----------------------------------------------------------------------------*/
#include <string.h> /* strcpy() */
#include <stdlib.h> /* realloc(), atoi() */
#include <stdio.h> /* printf(), putchar() */
#include <ctype.h> /* tolower() */
/* union REGS, struct SREGS, int86(), int86x(), FP_SEG(), FP_OFF(), */
#include <dos.h> /* pokeb(), poke(), inportb(), outportb() */
#if 0
#include <stdint.h>
#else
typedef unsigned char	uint8_t;
typedef unsigned short	uint16_t;
typedef unsigned long	uint32_t;
#endif

#if defined(__TURBOC__)
#include <conio.h> /* clrscr() */

#elif defined(__WATCOMC__)
#if defined(__386__)
#error 16-bit program -- compile with WCC.EXE
#endif
#define	inportb(P)	inp(P)
#define	outportb(P,V)	outp(P,V)
#define	pokeb(S,O,V)	*(uint8_t  far *)MK_FP(S,O)=(V)
#define	poke(S,O,V)	*(uint16_t far *)MK_FP(S,O)=(V)
#define	peek(S,O)	*(uint16_t far *)MK_FP(S,O)
void clrscr(void)
{
	union REGS regs;

	regs.h.ah = 0x02;
	regs.h.bh = 0; /* page number */
	regs.h.dh = 0; /* top-most row */
	regs.h.dl = 0; /* left-most column */
	int86(0x10, &regs, &regs);
}

#else
#error Sorry, unsupported compiler
#endif

/* besides the CRTC, VGA sequencer register #1 is also modified */
#define VGA_SEQ_INDEX		0x3C4
#define VGA_SEQ_DATA		0x3C5
/* emulation (color or mono) is read from this register --
it determines the CRTC I/O address (0x3B4 or 0x3D4) */
#define	VGA_MISC_READ		0x3CC
#define	VGA_CRTC_INDEX	(g_crtc_io + 0)
#define	VGA_CRTC_DATA	(g_crtc_io + 1)

/* this is like an X11 "modeline" */
typedef struct
{
	unsigned disp, blank_start, sync_start, sync_end, blank_end, total;
} timing_t;

/* for 30-line modes, use the 16-pixel-high font instead of 8-pixel */
static timing_t g_60_lines =
{
/*		blank	sync	sync	blank
	disp	start	start	end	end	tot
	----	-----	-----	----	-----	--- */
	480,	488,	490,	493,	517,	525
};

static timing_t g_90_cols =
{
/*		blank	sync	sync	blank
	disp	start	start	end	end	tot
	----	-----	-----	----	-----	--- */
	90,	90,	95,	108,	98,	112
};

typedef struct
{
	unsigned char cols, rows;
	unsigned mode_num;	/* VGA (<0x100) or VBE (>=0x100) mode number */
	unsigned char set_font;	/* set 8x8 font after mode-set or no? */
	timing_t *horiz, *vert;	/* CRTC timing 'tweaks' */
} mode_t;

static mode_t *g_mode;
static unsigned g_crtc_io, g_num_modes;
/*****************************************************************************
*****************************************************************************/
static void add_mode(unsigned cols, unsigned rows, unsigned mode_num,
		unsigned set_font, timing_t *horiz, timing_t *vert)
{
	mode_t *new_mode;

	new_mode = realloc(g_mode, (g_num_modes + 1) * sizeof(mode_t));
	if(new_mode == NULL)
	{
		printf("Error: out of memory\n");
		exit(2);
	}
	g_mode = new_mode;
	new_mode = &g_mode[g_num_modes];
	g_num_modes++;
	new_mode->cols		= cols;
	new_mode->rows		= rows;
	new_mode->mode_num	= mode_num;
	new_mode->set_font	= set_font;
	new_mode->horiz		= horiz;
	new_mode->vert		= vert;
}
/*****************************************************************************
*****************************************************************************/
static void dump_modes(void)
{
	unsigned scn_wd, csr_x, i;
	union REGS regs;

	scn_wd = peek(0x40, 0x4A);
	printf("The following video modes are supported\n");
	for(i = 0; i < g_num_modes; i++)
	{
/* get cursor X position */
		regs.h.ah = 0x03;
		regs.h.bh = 0;
		int86(0x10, &regs, &regs);
		csr_x = regs.h.dl;
/* emit newline now if next listing will wrap */
		if(csr_x + 7 >= scn_wd)
			printf("\n");
/* resolution listing is 7 characters wide */
		printf("%4ux%-2u ", g_mode[i].cols, g_mode[i].rows);
	}
	printf("\n");
}
/*****************************************************************************
*****************************************************************************/
static mode_t *find_mode(unsigned cols, unsigned rows)
{
	unsigned i;

	for(i = 0; i < g_num_modes; i++)
	{
		if(g_mode[i].cols == cols && g_mode[i].rows == rows)
			return &g_mode[i];
	}
	return NULL;
}
/*****************************************************************************
*****************************************************************************/
static void set_horiz(timing_t *t)
{
	unsigned i;

/* remove write-protection from CRTC registers 0-5 (and 6-7) */
	outportb(VGA_CRTC_INDEX, 17);
	outportb(VGA_CRTC_DATA, inportb(VGA_CRTC_DATA) & ~0x80);
/* set horizontal displayed */
	outportb(VGA_CRTC_INDEX, 1);
	outportb(VGA_CRTC_DATA, t->disp - 1);
/* set horizontal blanking start */
	outportb(VGA_CRTC_INDEX, 2);
	outportb(VGA_CRTC_DATA, t->blank_start);
/* set horizontal sync start */
	outportb(VGA_CRTC_INDEX, 4);
	outportb(VGA_CRTC_DATA, t->sync_start);
/* set horizontal sync end */
	outportb(VGA_CRTC_INDEX, 5);
	i = inportb(VGA_CRTC_DATA) & ~0x1F;
	outportb(VGA_CRTC_DATA, (t->sync_end & 0x1F) | i);
/* set horizontal blanking end */
	outportb(VGA_CRTC_INDEX, 3);
	i = inportb(VGA_CRTC_DATA) & ~0x1F;
	outportb(VGA_CRTC_DATA, (t->blank_end & 0x1F) | i);
/* set horizontal total */
	outportb(VGA_CRTC_INDEX, 0);
	outportb(VGA_CRTC_DATA, t->total - 5);
/* set "offset" (words per line) */
	outportb(VGA_CRTC_INDEX, 19);
	outportb(VGA_CRTC_DATA, t->disp / 2);
/* make characters 8 or 9 pixels wide */
	outportb(VGA_SEQ_INDEX, 1);
	i = inportb(VGA_SEQ_DATA) & ~0x01;
	if(t->disp == 90)
		i |= 0x01;
	outportb(VGA_SEQ_DATA, i);
}
/*****************************************************************************
*****************************************************************************/
static void set_vert(timing_t *t)
{
	unsigned i, j;

/* remove write-protection from CRTC registers 6-7 (and 0-5) */
	outportb(VGA_CRTC_INDEX, 17);
	outportb(VGA_CRTC_DATA, inportb(VGA_CRTC_DATA) & ~0x80);
/* set vertical displayed */
	i = t->disp - 1;
	outportb(VGA_CRTC_INDEX, 18);
	outportb(VGA_CRTC_DATA, i);
	outportb(VGA_CRTC_INDEX, 7);
	j = inportb(VGA_CRTC_DATA) & ~0x42;
	if(i & 0x100)
		j |= 0x02;
	if(i & 0x200)
		j |= 0x40;
	outportb(VGA_CRTC_DATA, j);
/* set vertical blanking start */
	i = t->blank_start;
	outportb(VGA_CRTC_INDEX, 21);
	outportb(VGA_CRTC_DATA, i);
	outportb(VGA_CRTC_INDEX, 7);
	j = inportb(VGA_CRTC_DATA) & ~0x08;
	if(i & 0x100)
		j |= 0x08;
	outportb(VGA_CRTC_DATA, j);
/* set vertical sync (retrace) start */
	i = t->sync_start;
	outportb(VGA_CRTC_INDEX, 16);
	outportb(VGA_CRTC_DATA, i);
	outportb(VGA_CRTC_INDEX, 7);
	j = inportb(VGA_CRTC_DATA) & ~0x84;
	if(i & 0x100)
		j |= 0x04;
	if(i & 0x200)
		j |= 0x80;
	outportb(VGA_CRTC_DATA, j);
/* set vertical sync (retrace) end */
	outportb(VGA_CRTC_INDEX, 17);
	i = inportb(VGA_CRTC_DATA) & ~0x0F;
	outportb(VGA_CRTC_DATA, (t->sync_end & 0x0F) | i);
/* set vertical blanking end */
	outportb(VGA_CRTC_INDEX, 22);
/*	i = inportb(VGA_CRTC_DATA) & ~0x7F;
	outportb(VGA_CRTC_DATA, (t->blank_end & 0x7F) | i); */
	outportb(VGA_CRTC_DATA, t->blank_end);
/* set vertical total */
	i = t->total - 2;
	outportb(VGA_CRTC_INDEX, 6);
	outportb(VGA_CRTC_DATA, i);
	outportb(VGA_CRTC_INDEX, 7);
	j = inportb(VGA_CRTC_DATA) & ~0x21;
	if(i & 0x100)
		j |= 0x01;
	if(i & 0x200)
		j |= 0x20;
	outportb(VGA_CRTC_DATA, j);
}
/*****************************************************************************
*****************************************************************************/
static void set_mode(mode_t *m)
{
	union REGS regs;

/* set (initial) mode; using either the VGA... */
	if(m->mode_num < 0x100)
	{
		regs.x.ax = m->mode_num;
		int86(0x10, &regs, &regs);
	}
/* ...or VBE BIOS */
	else
	{
		regs.x.ax = 0x4F02;
		regs.x.bx = m->mode_num;
		int86(0x10, &regs, &regs);
	}
/* set 8x8 font for 50- and 60-row VGA and tweaked modes */
	if(m->set_font)
	{
		regs.x.ax = 0x1112;
		regs.h.bl = 0;
		int86(0x10, &regs, &regs);
	}
	if(m->horiz || m->vert)
	{
/* get CRTC address */
		if((inportb(VGA_MISC_READ) & 0x01) == 0)
			g_crtc_io = 0x3B4;	/* monochrome emulation */
		else
			g_crtc_io = 0x3D4;	/* color emulation */
/* tweak CRTC timing */
		if(m->horiz)
			set_horiz(m->horiz);
		if(m->vert)
			set_vert(m->vert);
	}
/* let the BIOS know what we've done so text output works properly */
	pokeb(0x40, 0x84, m->rows - 1);
	poke(0x40, 0x4A, m->cols);
}
/*****************************************************************************
*****************************************************************************/
static void usage(void)
{
	printf("Sets text video modes. Usage:\n"
		"\tSETRES [-vd] cols rows\t\t"	"-v skips VGA compatability test\n"
		"\t\t\t\t\t"			"-d prints debug messages\n"
		"\tSETRES -l[vd]\t\t\t"		"lists available modes\n"
		"\tSETRES -a\t\t\t"		"displays author info\n");
	exit(1);
}
/*****************************************************************************
*****************************************************************************/
int main(int arg_c, char *arg_v[])
{
/* structure used by INT 10h AX=4F00h */
#pragma pack(1)
	static struct
	{
		char sig[4];
		uint8_t ver_minor;
		uint8_t ver_major;
		char far *oem_name;
		uint32_t capabilities;	 /* b1=1 for non-VGA board */
		uint16_t far *mode_list;
		char res0[494];		/* fields we don't care about */
	} vbe_info;
/* structure used by INT 10h AX=4F01h */
#pragma pack(1)
	static struct
	{
		uint16_t mode_attrib;	/* b4=0 for text modes */
		char res0[16];		/* fields we don't care about */
/* OEM modes and VBE 1.2+ only: */
		uint16_t wd;
		uint16_t ht;
		uint8_t char_wd;
		uint8_t char_ht;
		char res1[232];		/* fields we don't care about */
	} mode_info;
/* command-line options: */
	char assume_vga, list_modes, info, debug;
	unsigned i, num_count, wd, ht;
	uint16_t far *mnp; /* Mode Number Pointer */
	struct SREGS sregs;
	union REGS regs;
	mode_t *mode;
	char *s;

/* process command-line */
	if(arg_c < 2)
		usage();
	num_count = 0;
	assume_vga = list_modes = info = debug = 0;
	for(i = 1; i < arg_c; i++)
	{
/* options */
		if(arg_v[i][0] == '-')
		{
			for(s = &arg_v[i][1]; *s != '\0'; s++)
			{
				if(tolower(*s) == 'v')
					assume_vga = 1;
				else if(tolower(*s) == 'l')
					list_modes = 1;
				else if(tolower(*s) == 'a')
					info = 1;
				else if(tolower(*s) == 'd')
					debug = 1;
				else
				{
					printf("Error: invalid option '%c'\n",
						*s);
					usage();
				}
			}
		}
/* not an option, not a number */
		else if(atoi(arg_v[i]) == 0)
		{
			printf("Error: invalid parameter '%s'\n", arg_v[i]);
			usage();
		}
/* 1st number on command line = rows */
		else if(num_count == 0)
		{
			wd = atoi(arg_v[i]);
			num_count++;
		}
/* 2nd number on command line = cols */
		else if(num_count == 1)
		{
			ht = atoi(arg_v[i]);
			num_count++;
		}
/* too many numbers */
		else
		{
			printf("Error: invalid parameter '%s'\n", arg_v[i]);
			usage();
		}
	}
	if(info)
	{
		printf(	"Sets text video modes\n"
			"Chris Giese	<geezer@execpc.com>	http://my.execpc.com/~geezer/\n"
			"This code is public domain (no copyright).\n"
			"You can do whatever you want with it.\n"
			"Release date: March 31, 2009\n");
		return 1;
	}
/* I assume these VGA text modes are supported on all systems: */
	if(debug)
		printf("Adding VGA BIOS modes...\n");
	add_mode(40, 25, 1, 0, NULL, NULL);
	add_mode(40, 50, 1, 1, NULL, NULL);
	add_mode(80, 25, 3, 0, NULL, NULL);
	add_mode(80, 50, 3, 1, NULL, NULL);
/* check if VBE present */
	if(debug)
		printf("VBE BIOS...");
	strcpy(vbe_info.sig, "VBE2");
	sregs.es = FP_SEG(&vbe_info);
	regs.x.di = FP_OFF(&vbe_info);
	regs.x.ax = 0x4F00;
	int86x(0x10, &regs, &regs, &sregs);
/* (the indentation got a little ugly, so I'm going to use goto)
need VBE 1.2 or better */
	if(regs.x.ax != 0x004F)
	{
		if(debug)
			printf("not detected\n");
		goto NO_VBE;
	}
	if(debug)
		printf("version %u.%u; OEM name '%Fs'\n", vbe_info.ver_major,
			vbe_info.ver_minor,vbe_info.oem_name);
	if(vbe_info.ver_major < 1 ||
		(vbe_info.ver_major == 1 && vbe_info.ver_minor < 2))
	{
		if(debug)
			printf("Warning: VBE 1.2+ required\n");
		goto NO_VBE;
	}
/* iterate over VBE modes */
	if(debug)
		printf("Hex VBE mode numbers:\n");
	for(mnp = vbe_info.mode_list; *mnp != 0xFFFF; mnp++)
	{
		if(debug)
			printf("%4X ", *mnp);
/* get mode info */
		sregs.es = FP_SEG(&mode_info);
		regs.x.di = FP_OFF(&mode_info);
		regs.x.cx = *mnp;
		regs.x.ax = 0x4F01;
		int86x(0x10, &regs, &regs, &sregs);
		if(regs.x.ax != 0x004F)
			continue;
/* ignore all but text modes */
		if(mode_info.mode_attrib & 0x10)
			continue;
/* add VBE text mode to list */
		if(debug)
			printf("\nAdding VBE mode: %ux%u\n",
				mode_info.wd, mode_info.ht);
		add_mode(mode_info.wd, mode_info.ht, *mnp, 0, NULL, NULL);
	}
	if(debug)
	{
		if(assume_vga)
			printf("\n-v option; assuming hardware is VGA-compatible\n");
		else
		{
			printf("\nHardware is ");
			if(vbe_info.capabilities & 0x01)
				printf("NOT ");
			printf("register-compatible with VGA\n");
		}
	}
/* check if board is register-compatible with VGA
(unless overriden with '-v' option...) */
	if(!assume_vga && (vbe_info.capabilities & 0x01))
		goto NO_VBE;
/* add 'tweaked' VGA modes to list */
	if(debug)
		printf("Adding tweaked VGA modes...\n");
	add_mode(40, 30, 1, 0, NULL,		&g_60_lines);
	add_mode(40, 60, 1, 1, NULL,		&g_60_lines);
	add_mode(80, 30, 3, 0, NULL,		&g_60_lines);
	add_mode(80, 60, 3, 1, NULL,		&g_60_lines);
	add_mode(90, 25, 3, 0, &g_90_cols,	NULL);
	add_mode(90, 30, 3, 0, &g_90_cols,	&g_60_lines);
	add_mode(90, 50, 3, 1, &g_90_cols,	NULL);
	add_mode(90, 60, 3, 1, &g_90_cols,	&g_60_lines);
/* prepare to tweak VBE modes */
	for(i = 0; i < g_num_modes; i++)
	{
		static unsigned rows[] = { 25, 30, 50, 60 };
/**/
		unsigned j, set_font, tweak_vert;

/* find VBE modes... */
		if(g_mode[i].mode_num < 0x100)
			continue;
/* ...with 25, 30, 50, or 60 rows */
		if(g_mode[i].rows != 25 && g_mode[i].rows != 30
			&& g_mode[i].rows != 50 && g_mode[i].rows != 60)
				continue;
/* check if this mode is register-compatible with VGA */
		if(!assume_vga)
		{
			sregs.es = FP_SEG(&mode_info);
			regs.x.di = FP_OFF(&mode_info);
			regs.x.cx = g_mode[i].mode_num;
			regs.x.ax = 0x4F01;
			int86x(0x10, &regs, &regs, &sregs);
			if(regs.x.ax != 0x004F)
				continue;
/* xxx - b5 of mode_info.mode_attrib may be VBE 2.0+ only,
according to Ralf Brown's list */
			if(mode_info.mode_attrib & 0x20)
			{
				if(debug)
					printf("VBE mode 0x%X is NOT "
						"register-compatible with "
						"VGA; will not tweak\n",
						g_mode[i].mode_num);
				continue;
			}
		}
/* now find mode with same number of columns
and complementary number of rows */
		for(j = 0; j < sizeof(rows) / sizeof(rows[0]); j++)
		{
			if(g_mode[i].rows == rows[j])
				continue;
/* if the complementary mode does not already exist... */
			mode = find_mode(g_mode[i].cols, rows[j]);
			if(mode != NULL)
				continue;
/* ...add it */
			set_font = (rows[j] >= 50);
			tweak_vert = (rows[j] == 30 || rows[j] == 60);
			if(debug)
				printf("Adding tweaked VBE mode: %ux%u\n",
					g_mode[i].cols, rows[j]);
			add_mode(g_mode[i].cols, rows[j],
				g_mode[i].mode_num, set_font, NULL,
				(tweak_vert ? &g_60_lines : NULL));
		}
	}
NO_VBE:
/* just list the supported modes */
	if(list_modes)
	{
		dump_modes();
		return 0;
	}
/* otherwise we need two numbers on the command-line
(the case of more than 2 numbers was handled above) */
	if(num_count != 2)
	{
		printf("Error: must specify cols and rows to set video mode\n");
		usage();
	}
/* see if selected resolution supported */
	mode = find_mode(wd, ht);
	if(mode == NULL)
	{
		printf("Error: mode %ux%u not supported\n", wd, ht);
		dump_modes();
		return 3;
	}
/* SET MODE */
	set_mode(mode);
#if 1
/* clear screen */
	clrscr();
#else
/* DEBUG: display horizontal and vertical 'rulers' */
/*	textattr(0x17);	doesn't work -- no blue background after clrscr() */
	clrscr();
	for(i = 1; i < 0xFF00; i += 2)
		pokeb(0xB800, i, 0x17);
	for(i = 0; i < wd - 1; )
	{
		if(i % 10 == 0)
		{
			printf("%03u", i);
			i += 3;
		}
		else
		{
			putchar(' ');
			i++;
		}
	}
	putchar('\n');
	for(i = 1; i < ht - 4; i++)
		printf("%u\n", i);
	printf("Current screen resolution is %ux%u\n", wd, ht);
#endif
	return 0;
}
