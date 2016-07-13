/*
	Screen.CPP version 1.0
	by Robert Schmidt of Ztiff Zox Softwear 1993

	Defines some primitives for handling the screen, some screen
		buffer pointers, and the functions that handle the single
		temporary screen used in TWEAK.

*/

#include <conio.h>
#include <mem.h>
#include <dos.h>
#include <iostream.h>
#include <stdlib.h>
#include "Screen.HPP"

char palette16[16];
char palette256[768];

// editHeight and editWidth hold the dimensions of the editing screen.
//	Not everything is formatted according to those horizontally, but
//	vertically it should work fine.

unsigned editMode, editHeight, editWidth, editSize;

// Now for the screens used in TWEAK.

// This one points to the standard VGA text screen buffer.  textscr[80]
//	addresses the first character/attribute pair on the second line
//	if the current mode is an 80-column one, for example.

unsigned *textScr = (unsigned far *)MK_FP(0xb800,0);

// graphScr points to the standard VGA graphics buffer, being 64Kb.

char *graphScr = (char far *)MK_FP(0xa000,0);

// setBiosMode() sets the given standard BIOS mode.

void setBiosMode(int modeno)
	{
	_AX = modeno;
	geninterrupt(0x10);
	}

// getBiosMode() returns the current BIOS mode.

int getBiosMode(void)
	{
	_AH = 0x0f;
	geninterrupt(0x10);
	return _AL;
	}

// The following two functions saves and restores the temporary screen.
//	The tempScr buffer is allocated and destroyed each time.

void tempBuffer::save(void)
	{
	if (temp)
		delete[] temp;
	if (!(temp = new unsigned[editSize]))
		{
		cout << "Out of memory for swap screen!" << endl;
		exit(1);
		}
	memcpy(temp, link, sizeof(unsigned)*editSize);
	}

void tempBuffer::restore(void)
	{
	setBiosMode(3);
	textmode(editMode);
	if (temp)
		{
		memcpy(link, temp, sizeof(unsigned)*editSize);
		delete[] temp;
		temp = NULL;
		}
	}

void getPalette16()
	{
	for (int c=0; c<16; c++)
		{
		outp(0x3c0, 0x20 | c);
		palette16[c] = inp(0x3c0);
		}
	}

void setEgaPalette(char *p)
	{
	inp(0x3da);
	for (int c=0; c<16; c++)
		{
		outp(0x3c0, c);
		outp(0x3c0, p[c]);
		}
	outp(0x3c0, 0x20);
	}

void setPalette16()
	{
	char egaPal[] = {0,1,2,3,4,5,0x14,7,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f};
	setEgaPalette(egaPal);
	for (int c=0; c<16; c++)
		{
		outp(0x3c0, 0x20 | c);
		outp(0x3c0, palette16[c]);
		}
	}

void getPalette256()
	{
	outp(0x3c7, 0);
	for (int c=0; c<768; c++)
		palette256[c] = inp(0x3c9);
	}

void setPalette256()
	{
	char egaPal[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
	setEgaPalette(egaPal);
	outp(0x3c8, 0);
	for (int c=0; c<768; c++)
		outp(0x3c9, palette256[c]);
	}

void preparePalettes()
	{
	int m=getBiosMode();
	setBiosMode(0x13);
	getPalette256();
	setBiosMode(0x12);
	getPalette16();
	setBiosMode(m);
	}