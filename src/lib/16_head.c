/* Project 16 Source Code~
 * Copyright (C) 2012-2016 sparky4 & pngwen & andrius4669 & joncampbell123
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

/* Function: Wait **********************************************************
*
*     Parameters:    wait - time in microseconds
*
*     Description:    pauses for a specified number of microseconds.
*
*/
void wait(clock_t wait){
	clock_t goal;

	if(!wait) return;

	goal = wait + clock();
	while((goal > clock()) && !kbhit()) ;
} /* End of wait */

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

/*
==========================
=
= Quit
=
==========================
*/

/*void Quit(char *error, ...)
{
	short exit_code=0;
	unsigned	finscreen;

	va_list ap;

	va_start(ap,error);

#ifndef CATALOG
	if (!error)
	{
		CA_SetAllPurge ();
		CA_CacheGrChunk (PIRACY);
		finscreen = (unsigned)grsegs[PIRACY];
	}
#endif

	//ShutdownId ();

	if (error && *error)
	{
		vprintf(error,ap);
		exit_code = 1;
	}
#ifndef CATALOG
	else
	if (!NoWait)
	{
		movedata (finscreen,0,0xb800,0,4000);
		bioskey (0);
	}
#endif

	va_end(ap);

#ifndef CATALOG
	if (!error)
	{
		_argc = 2;
		_argv[1] = "LAST.SHL";
		_argv[2] = "ENDSCN.SCN";
		_argv[3] = NULL;
		if (execv("LOADSCN.EXE", _argv) == -1)
		{
			clrscr();
			puts("Couldn't find executable LOADSCN.EXE.\n");
			exit(1);
		}
	}
#endif

	exit(exit_code);
}*/

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
