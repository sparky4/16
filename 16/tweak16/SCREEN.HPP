/*
	Screen.HPP version 1.0
	by Robert Schmidt of Ztiff Zox Softwear 1993

	Declares some primitives for handling the screen, some screen
		buffer pointers, and the functions that handle the single
		temporary screen used in TWEAK.

	See Screen.CPP for definitions and documentation.
*/

#ifndef _Screen_HPP
#define _Screen_HPP

#include <mem.h>

#define	REGENABLE_COLOR		0x0e
#define REGDISABLE_COLOR	0x07
#define CURSOR_COLOR		0x0f
#define BITHEADER_COLOR		0x04
#define BITPATTERN_COLOR	0x0c
#define TESTHEADER_COLOR	0x09
#define TESTSTRING_COLOR	0x0b
#define PROMPT_COLOR		0x0a
#define ERROR_COLOR			0x8d
#define HELP_COLOR			0x4f
#define INFO_COLOR			0x0f

extern unsigned editMode, editHeight, editWidth, editSize;

extern unsigned *textScr;
extern char *graphScr;

int getBiosMode(void);
void setBiosMode(int);
void preparePalettes();
void setPalette16();
void setPalette256();

class	tempBuffer
	{
	unsigned *link, *temp;
public:
	tempBuffer(unsigned *l)	{ link = l; temp = NULL; }
    ~tempBuffer()			{ if (temp) delete[] temp; }
	void save(void);
	void restore(void);
	};

#endif