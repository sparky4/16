/* Project 16 Source Code~
 * Copyright (C) 2012-2015 sparky4 & pngwen & andrius4669
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

#if !defined(__LARGE__) && !defined(__COMPACT__) && !defined(__HUGE__)
#error Invalid memory model for compiling project 16
#endif

#if !defined(__i86__) && defined(__i386__)
#error i8088 only
#endif

#ifndef _LIBHEAD_H_
#define _LIBHEAD_H_
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h> // just for wait
#include <time.h> // just for wait
#include <malloc.h>
#include <ctype.h>
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
#include <mem.h>
#include <string.h>
#ifdef __WATCOMC__
#include <i86.h>
#include <unistd.h>
#include <alloca.h>
#endif
#include "src/lib/nyan/kitten.h"
#include "src/lib/types.h"

//0000 test type def wwww
/*struct list {
	struct list __based(__self) *next;
	int         value;
};*/

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

static dword far* clockdw= (dword far*) 0x046C; /* 18.2hz clock */
extern	int			profilehandle,debughandle;	//make it into game global

#define __DEBUG__

#define	nil	((void *)0)
#ifdef __BORLANDC__
#define _FCORELEFT 0x90000UL-16UL
#endif
#ifdef __WATCOMC__
#define _FCORELEFT 0x90000UL+16UL

#define peekb(segm,ofs) (*(byte far*)MK_FP((segm),(ofs)))
#define peekw(segm,ofs) (*(word far*)MK_FP((segm),(ofs)))
#define pokeb(segm,ofs,value) (peekb((segm),(ofs)) = (byte)(value))
#define pokew(segm,ofs,value) (peekw((segm),(ofs)) = (word)(value))

typedef union REGPACK	regs_t;
#endif

typedef	enum	{false,true}	boolean;

#ifdef __WATCOMC__
typedef void __based(__self) * memptr;
#endif
#ifdef __BORLANDC__
typedef void _seg * memptr;
#endif

typedef struct
{
// 	int showmemhandle;
	int			profilehandle,debughandle;
	int heaphandle;
} handle_t;

typedef struct
{
	int old_mode;	//old video mode before game!
	word frames_per_second;
	clock_t t;
	dword tiku;
	word far* clock;
	handle_t handle;	//handles for file logging
} global_game_variables_t;

/* local function */
void wait(clock_t wait);

long int filesize(FILE *fp);
void printmeminfoline(byte *strc, const byte *pee, size_t h_total, size_t h_used, size_t h_free);
int US_CheckParm(char *parm,char **strings);

extern void CA_OpenDebug (global_game_variables_t *gvar);
extern void CA_CloseDebug (global_game_variables_t *gvar);

#endif/*_LIBHEAD_H_*/
