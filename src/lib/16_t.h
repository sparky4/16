/* Project 16 Source Code~
 * Copyright (C) 2012-2017 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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
/*
 * Just some handy typedefs that make it easier to think about the low
 * level code
 */

#ifndef _TYPE_H_
#define _TYPE_H_

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;
typedef signed char sbyte;
typedef signed short sword;
typedef signed long sdword;

typedef unsigned int iword;
typedef signed int siword;

typedef unsigned long int diword;
typedef signed long int sdiword;

typedef	enum	{false,true}	boolean;
typedef	enum	{ichi,ni,san,yon,go,roku,shichi,hachi,kyu,ju,juichi,juni,jusan,juyon,jugo,juroku}	nibble;
//typedef 	unsigned nibb:4 nibble;

#define HI_NIBBLE(b) (((b) >> 4) & 0x0F)
#define LO_NIBBLE(b) ((b) & 0x0F)
/*typedef unsigned memseg;

memptr should be replaced by memseg in code.

on usage where you need pointer convert memseg type (segment) to far pointer by
MK_FP(segment value, 0)*/
#ifdef __WATCOMC__
//typedef void __based( void ) * memptr;	////old //----typedef void __based(__self) * memptr;
//typedef unsigned short _seg; // it will contains segment value (as Borland _seg)
#define _seg __based( void )
// #define __SEGA __segment
#endif
#ifdef __BORLANDC__
#define _memavl()               coreleft()
// #define __SEGA _seg
#endif

typedef void _seg * memptr;

#ifdef __WATCOMC__	//functions found in borland c but not watcom c
#define _argv __argv
#define _argc __argc
#define geninterrupt(n) int86(n,&CPURegs,&CPURegs);

#define peekb(segm,ofs) (*(byte far*)MK_FP((segm),(ofs)))
#define peek(segm,ofs) (*(word far*)MK_FP((segm),(ofs)))
#define pokeb(segm,ofs,value) (peekb((segm),(ofs)) = (byte)(value))
#define poke(segm,ofs,value) (peek((segm),(ofs)) = (word)(value))

#define inportb(port)         inp(port)
#define inport(port)         inpw(port)
#define outportb(port, val)   outp(port, val)
#define outport(port, val)   outpw(port, val)

//#define asm __asm
#define getvect _dos_getvect
#define setvect _dos_setvect

//from http://www.verycomputer.com/3_65d875cc818b54ec_1.htm
void clrscr(void);
#pragma aux clrscr= \
  " mov ah, 0fh " \
  " int 10h " \
  " xor ah, ah " \
  " int 10h " ;
//from http://stackoverflow.com/questions/18425748/gotoxy-function-using-printf-s-position
inline void gotoxy(int x,int y)
{
	printf("\x1B[%d;%df", y, x);
	fflush(stdout);  // @jxh
}
#endif

#endif/*_TYPE_H_*/
