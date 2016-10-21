/* Project 16 Source Code~
 * Copyright (C) 2012-2016 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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

#include "src/lib/16_head.h"

long int
filesize(FILE *fp)
{
	long int save_pos, size_of_file;

	save_pos = ftell(fp);
	fseek(fp, 0L, SEEK_END);
	size_of_file = ftell(fp);
	fseek(fp, save_pos, SEEK_SET);
	return(size_of_file);
}

void printmeminfoline(byte *strc, const byte *pee, size_t h_total, size_t h_used, size_t h_free)
{
	byte str[64];
	strcat(strc,pee); strcat(strc,"		   "); ultoa((dword)h_total,str,10); strcat(strc,str);
	if(strlen(str)<=4) strcat(strc,"	"); //printf("%u\n", strlen(str));
	strcat(strc,"	"); ultoa((dword)h_used,str,10); strcat(strc,str); strcat(strc,"	"); strcat(strc,"  ");
	ultoa((dword)h_free,str,10); strcat(strc,str);
	strcat(strc,"\n");
}

///////////////////////////////////////////////////////////////////////////
//
//      US_CheckParm() - checks to see if a string matches one of a set of
//              strings. The check is case insensitive. The routine returns the
//              index of the string that matched, or -1 if no matches were found
//
///////////////////////////////////////////////////////////////////////////
int
US_CheckParm(char *parm,char **strings)
{
	char    cp,cs,
			*p,*s;
	int             i;

	while (!isalpha(*parm)) // Skip non-alphas
		parm++;

	for (i = 0;*strings && **strings;i++)
	{
		for (s = *strings++,p = parm,cs = cp = 0;cs == cp;)
		{
			cs = *s++;
			if (!cs)
				return(i);
			cp = *p++;

			if (isupper(cs))
				cs = tolower(cs);
			if (isupper(cp))
				cp = tolower(cp);
		}
	}
	return(-1);
}
#ifdef __BORLANDC__
//===========================================================================

/*
==========================
=
= Quit
=
==========================
*/

void Quit (char *error)
{
	unsigned        finscreen;
	memptr	screen;
	union REGS in, out;

	//ClearMemory ();
	if (!*error)
	{
	 //WriteConfig ();
	}
	else
	{
	 //CA_CacheGrChunk (ERRORSCREEN);
	 //screen = grsegs[ERRORSCREEN];
	}

	//ShutdownId ();
	//IN_Shutdown();
	//modexLeave();
	in.h.ah = 0x00;
	in.h.al = 0x3;
	int86(0x10, &in, &out);

	if (error && *error)
	{
	  //movedata ((unsigned)screen,7,0xb800,0,7*160);
	  //gotoxy (10,4);
		printf("\n");
	  puts(error);
		printf("\n");
	  //gotoxy (1,8);
	  exit(1);
	}
	else
	if (!error || !(*error))
	{
		//clrscr();
		//#ifndef JAPAN
		movedata ((unsigned)screen,7,0xb800,0,4000);
		//gotoxy(1,24);
		//#endif
//asm	mov	bh,0
//asm	mov	dh,23	// row
//asm	mov	dl,0	// collumn
//asm	mov ah,2
//asm	int	0x10
	}

	exit(0);
}

//===========================================================================
#endif

byte dirchar(byte in)
{
	byte out;
	switch(in)
	{
		case 0: //up
			out = 0x1E;
		break;
		case 4: //down
			out = 0x1F;
		break;
		case 1: //left
			out = 0x11;
		break;
		case 3: //right
			out = 0x10;
		break;
		default: //null
			out = 0xB3;
		break;
	}
	return out;
}
