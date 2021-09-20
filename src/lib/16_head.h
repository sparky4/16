/* Project 16 Source Code~
 * Copyright (C) 2012-2021 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
 *
 * This file is part of Project 16.
 *
 * Project 16 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Project 16 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 * write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

//#if !defined(__LARGE__) && !defined(__COMPACT__) && !defined(__HUGE__)
//#error Invalid memory model for compiling project 16
//#endif

#if !defined(__i86__) && defined(__i386__)
#error i8088 only
#endif

#ifndef __16_HEAD_H__
#define __16_HEAD_H__
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h> // this is where Open Watcom hides the outp() etc. functions // just for wait
#include <time.h> // just for wait
#include <ctype.h>
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
#include <mem.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <process.h>
#ifdef __WATCOMC__
#include <i86.h>
#include <unistd.h>
#include <alloca.h>
#include <stdint.h> //16_vrs.h
#endif
#ifdef __BORLANDC__
#include <values.h>
#include <dir.h>
#define TILEWH	16
#endif
#include "src/lib/16_t.h"
#include "src/lib/16_tdef.h"
#include "src/lib/nyan/kitten.h"

#define VERSION __DATE__ " " __TIME__

/* Control codes for all keys on the keyboard */
//here temperarly
/*
#define KEY_A		(0x1E)
#define KEY_B		(0x30)
#define KEY_C		(0x2E)
#define KEY_D		(0x20)
#define KEY_E		(0x12)
#define KEY_F		(0x21)
#define KEY_G		(0x22)
#define KEY_H		(0x23)
#define KEY_I		(0x17)
#define KEY_J		(0x24)
#define KEY_K		(0x25)
#define KEY_L		(0x26)
#define KEY_M		(0x32)
#define KEY_N		(0x31)
#define KEY_O		(0x18)
#define KEY_P		(0x19)
#define KEY_Q		(0x10)
#define KEY_R		(0x13)
#define KEY_S		(0x1F)
#define KEY_T		(0x14)
#define KEY_U		(0x16)
#define KEY_V		(0x2F)
#define KEY_W		(0x11)
#define KEY_X		(0x2D)
#define KEY_Y		(0x15)
#define KEY_Z		(0x2C)
#define KEY_1		(0x02)
#define KEY_2		(0x03)
#define KEY_3		(0x04)
#define KEY_4		(0x05)
#define KEY_5		(0x06)
#define KEY_6		(0x07)
#define KEY_7		(0x08)
#define KEY_8		(0x09)
#define KEY_9		(0x0A)
#define KEY_0		(0x0B)
#define KEY_DASH		(0x0C)	// -_
#define KEY_EQUAL		(0x0D)	// =+
#define KEY_LBRACKET	(0x1A)	// [{
#define KEY_RBRACKET	(0x1B)	// ]}
#define KEY_SEMICOLON	(0x27)	// ;:
#define KEY_RQUOTE	(0x28)	// '"
#define KEY_LQUOTE	(0x29)	// `~
#define KEY_PERIOD	(0x33)	// .>
#define KEY_COMMA		(0x34)	// ,<
#define KEY_SLASH		(0x35)	// /?
#define KEY_BACKSLASH	(0x2B)	// \|
#define KEY_F1		(0x3B)
#define KEY_F2		(0x3C)
#define KEY_F3		(0x3D)
#define KEY_F4		(0x3E)
#define KEY_F5		(0x3F)
#define KEY_F6		(0x40)
#define KEY_F7		(0x41)
#define KEY_F8		(0x42)
#define KEY_F9		(0x43)
#define KEY_F10		(0x44)
#define KEY_ESC		(0x01)
#define KEY_BACKSPACE   (0x0E)
#define KEY_TAB		(0x0F)
#define KEY_ENTER		(0x1C)
#define KEY_CONTROL	(0x1D)
#define KEY_LSHIFT	(0x2A)
#define KEY_RSHIFT	(0x36)
#define KEY_PRTSC		(0x37)
#define KEY_ALT		(0x38)
#define KEY_SPACE		(0x39)
#define KEY_CAPSLOCK	(0x3A)
#define KEY_NUMLOCK	(0x45)
#define KEY_SCROLLLOCK	(0x46)
#define KEY_HOME		(0x47)
#define KEY_UP		(0x48)
#define KEY_PGUP		(0x49)
#define KEY_MINUS		(0x4A)
#define KEY_LEFT		(0x4B)
#define KEY_CENTER	(0x4C)
#define KEY_RIGHT		(0x4D)
#define KEY_PLUS		(0x4E)
#define KEY_END		(0x4F)
#define KEY_DOWN		(0x50)
#define KEY_PGDOWN	(0x51)
#define KEY_INS		(0x52)
#define KEY_DEL		(0x53)

#define KEY_LWIN		(0x73)
#define KEY_RWIN		(0x74)
#define KEY_MENU		(0x75)
*/

//static word far* clockw= (word far*) 0x046C; /* 18.2hz clock */
extern	int			profilehandle,debughandle;	//make it into game global

//#define OLD16_VL

#define	nil	((void *)0)

#ifdef __WATCOMC__
//----#define _FCORELEFT 0x90000UL+16UL
extern union REGS CPURegs;

#define _AX CPURegs.x.ax
#define _BX CPURegs.x.bx
#define _CX CPURegs.x.cx
#define _DX CPURegs.x.dx

#define _SI CPURegs.x.si
#define _DI CPURegs.x.di

#define _AH CPURegs.h.ah
#define _AL CPURegs.h.al
#define _BH CPURegs.h.bh
#define _BL CPURegs.h.bl
#define _CH CPURegs.h.ch
#define _CL CPURegs.h.cl
#define _DH CPURegs.h.dh
#define _DL CPURegs.h.dl

#define _CFLAG CPURegs.x.cflag

#define geninterrupt(n) int86(n,&CPURegs,&CPURegs);


typedef union REGPACK	regs_t;
#endif

// local function //
long int filesize(FILE *fp);
void clrstdin();
char *remove_ext(char* mystr, char dot, char sep);
void rotateR(byte arr[], byte n);
void rotateL(byte arr[], byte n);
void printmeminfoline(byte *strc, const byte *pee, size_t h_total, size_t h_used, size_t h_free);
int US_CheckParm(char *parm,char **strings);
byte dirchar(byte in);
void print_mem(void const *vp, size_t n);
void hres (void);
void regidump();

//from https://stackoverflow.com/questions/111928/is-there-a-printf-converter-to-print-in-binary-format
//printf("Leading text "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(byte));
//For multi-byte types
//printf("m: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n",		BYTE_TO_BINARY(m>>8), BYTE_TO_BINARY(m));
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)	\
	(byte & 0x80 ? '1' : '0'),	\
	(byte & 0x40 ? '1' : '0'),	\
	(byte & 0x20 ? '1' : '0'),	\
	(byte & 0x10 ? '1' : '0'),	\
	(byte & 0x08 ? '1' : '0'),	\
	(byte & 0x04 ? '1' : '0'),	\
	(byte & 0x02 ? '1' : '0'),	\
	(byte & 0x01 ? '1' : '0')

#define WORD_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"
#define WORD_TO_BINARY(word)	\
	(word & 0x8000 ? '1' : '0'),	\
	(word & 0x4000 ? '1' : '0'),	\
	(word & 0x2000 ? '1' : '0'),	\
	(word & 0x1000 ? '1' : '0'),	\
	(word & 0x0800 ? '1' : '0'),	\
	(word & 0x0400 ? '1' : '0'),	\
	(word & 0x0200 ? '1' : '0'),	\
	(word & 0x0100 ? '1' : '0'),	\
	(word & 0x0080 ? '1' : '0'),	\
	(word & 0x0040 ? '1' : '0'),	\
	(word & 0x0020 ? '1' : '0'),	\
	(word & 0x0010 ? '1' : '0'),	\
	(word & 0x0008 ? '1' : '0'),	\
	(word & 0x0004 ? '1' : '0'),	\
	(word & 0x0002 ? '1' : '0'),	\
	(word & 0x0001 ? '1' : '0')

#define NIBBLE_TO_BINARY_PATTERN "%c%c%c%c"
#define NIBBLE_TO_BINARY(nibble)	\
	(nibble & 0x08 ? '1' : '0'),	\
	(nibble & 0x04 ? '1' : '0'),	\
	(nibble & 0x02 ? '1' : '0'),	\
	(nibble & 0x01 ? '1' : '0')

#define BOOLEAN_TO_BINARY_PATTERN "%c"
#define BOOLEAN_TO_BINARY(boolean)	\
	(boolean & 0x01 ? '1' : '0')

#define PRINT_OPAQUE_STRUCT(p)  print_mem((p), sizeof(*(p)))

#endif/*__16_HEAD_H__*/
