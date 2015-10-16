/*
	09TO10 1.0 - convert TWEAK 0.9 files to TWEAK 1.0 files

	by Robert Schmidt of Ztiff Zox Softwear, 1992-93

	For documentation, see TWEAK.DOC.

	Most of the starting definitions was taken from the TWEAK095.CPP
	file included elsewhere, so comments are removed.
*/

#ifndef __LARGE__
# ifndef __COMPACT__
#  ifndef __HUGE__
#   error A large data model is required!
#  endif
# endif
#endif


#include <stdio.h>
#include <dos.h>
#include <stdlib.h>
#include <fstream.h>
#include <io.h>

#include "Register.hpp"


struct vgaRegisterInfo
	{
	char *name;
	unsigned port;
	unsigned char index;
	};

vgaRegisterInfo table[] =
	{
		{"Misc. output", 			0x3c2,	0x00},

		{"Horizontal total",		0x3d4,  0x00},
		{"Horizontal disp. enable",	0x3d4,  0x01},
		{"Horizontal blank start",	0x3d4,  0x02},
		{"Horizontal blank end",	0x3d4,  0x03},
		{"Horizontal retrace start",0x3d4,  0x04},
		{"Horizontal retrace end",	0x3d4,  0x05},
		{"Vertical total",			0x3d4,  0x06},
		{"Overflow register",		0x3d4,	0x07},
		{"Preset row scan",			0x3d4,	0x08},
		{"Max scan line/char ht.",	0x3d4,	0x09},

		{"Vertical retrace start",	0x3d4,	0x10},
		{"Vertical retrace end",	0x3d4,	0x11},
		{"Vert. disp. enable end",	0x3d4,	0x12},
		{"Offset/Logical width",	0x3d4,	0x13},
		{"Underline location",		0x3d4,	0x14},
		{"Vertical blank start",	0x3d4,	0x15},
		{"Vertical blank end",		0x3d4,	0x16},
		{"Mode control",			0x3d4,	0x17},

		{"Clock mode register",		0x3c4,	0x01},
		{"Color plane write enable",0x3c4,	0x02},
		{"Character gen. select",	0x3c4,	0x03},
		{"Memory mode register",	0x3c4,	0x04},

		{"Set/reset register",		0x3ce,	0x00},
		{"Set/reset enable",		0x3ce,	0x01},
		{"Color compare",			0x3ce,	0x02},
		{"Data rotate & function",	0x3ce,	0x03},
		{"Mode register",			0x3ce,	0x05},
		{"Miscellaneous register",	0x3ce,	0x06},
		{"Color don't care",		0x3ce,	0x07},
		{"Bit mask register",		0x3ce,	0x08},

		{"Mode control",			0x3c0,	0x10},
		{"Screen border colour",	0x3c0,	0x11},
		{"Color plane enable",		0x3c0,	0x12},
		{"Horizontal panning",		0x3c0,	0x13},
		{"Color select",			0x3c0,	0x14}
	};

const registers = sizeof (table) / sizeof (vgaRegisterInfo);


class vgaRegTable
	{
	unsigned char value[registers];
	unsigned char selectedReg;
public:
	void out();
	void in();
	void print(unsigned char selected);
	void printOne(unsigned char r, int isSelected);
	unsigned char& operator [] (unsigned char n)
					{ return value[n]; }
	};


// The main program starts here.

main(int argc, char **argv)
	{
	if (argc < 3)
		{
		printf("09TO10 version 1.0\n\r"
			   "by Robert Schmidt of Ztiff Zox Softwear 1993\n\r"
			   "\n\r"
			   "Converts TWEAK version 0.9 files to TWEAK version 1.0 files.\n\r"
			   "\n\r"
			   "Syntax:  09TO10 <oldfile> <newfile>\n\r"
			   );
		return 0;
		}

	vgaRegTable rtab;

	char *fname = argv[1];
	FILE *f;
	int r;

	// Open file in selected mode.
	if (!(f=fopen(fname,"rb")))
		{
		perror(fname);
		return 0;
        }
	// Read file:
	for (r=0; r<registers; r++)
		if (fread(&(rtab[r]),1,1,f) == 0)
			{
			perror(fname);
			return 0;
			}
	fclose(f);

	ofstream out(argv[2], ios::trunc|ios::binary|ios::out);
	Register reg;
	for (r=0; r<registers; r++)
		{
		reg.setPort(table[r].port);
		reg.setIndex(table[r].index);
		reg.setValue(rtab[r]);
		out << reg;
		}

	return 0;
	}
