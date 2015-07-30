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

void* AllocateLargestFreeBlock(size_t* Size)
{
  size_t s0, s1;
  void* p;

  s0 = ~(size_t)0 ^ (~(size_t)0 >> 1);

  while (s0 && (p = malloc(s0)) == NULL)
    s0 >>= 1;

  if (p)
    free(p);

  s1 = s0 >> 1;

  while (s1)
  {
    if ((p = malloc(s0 + s1)) != NULL)
    {
      s0 += s1;
      free(p);
    }
    s1 >>= 1;
  }

  while (s0 && (p = malloc(s0)) == NULL)
    s0 ^= s0 & -s0;

  *Size = s0;
  return p;
}

size_t GetFreeSize(void)
{
  size_t total = 0;
  void* pFirst = NULL;
  void* pLast = NULL;

  for (;;)
  {
    size_t largest;
    void* p = AllocateLargestFreeBlock(&largest);

    if (largest < sizeof(void*))
    {
      if (p != NULL)
        free(p);
      break;
    }

    *(void**)p = NULL;

    total += largest;

    if (pFirst == NULL)
      pFirst = p;

    if (pLast != NULL)
      *(void**)pLast = p;

    pLast = p;
  }

  while (pFirst != NULL)
  {
    void* p = *(void**)pFirst;
    free(pFirst);
    pFirst = p;
  }

  return total;
}

void far* AllocateLargestFarFreeBlock(size_t far* Size)
{
	size_t s0, s1;
	void far* p;

	s0 = ~(size_t)0 ^ (~(size_t)0 >> 1);
	while (s0 && (p = _fmalloc(s0)) == NULL)
		s0 >>= 1;

	if (p)
		_ffree(p);

	s1 = s0 >> 1;
	while (s1)
	{
		if ((p = _fmalloc(s0 + s1)) != NULL)
		{
			s0 += s1;
			_ffree(p);
		}
	s1 >>= 1;
	}
	while (s0 && (p = _fmalloc(s0)) == NULL)
		s0 ^= s0 & -s0;

	*Size = s0;
	return p;
}

size_t GetFarFreeSize(void)
{
	size_t total = 0;
	void far* pFirst = NULL;
	void far* pLast = NULL;
	for(;;)
	{
		size_t largest;
		void far* p = AllocateLargestFarFreeBlock(&largest);
		if (largest < sizeof(void far*))
		{
			if (p != NULL)
			_ffree(p);
			break;
		}
		*(void far* far*)p = NULL;
		total += largest;
		if (pFirst == NULL)
			pFirst = p;

		if (pLast != NULL)
			*(void far* far*)pLast = p;
		pLast = p;
	}

	while (pFirst != NULL)
	{
		void far* p = *(void far* far*)pFirst;
		_ffree(pFirst);
		pFirst = p;
	}
	return total;
}

//near versions
void __near* AllocateLargestNearFreeBlock(size_t* Size)
{
	size_t s0, s1;
	void __near* p;

	s0 = ~(size_t)0 ^ (~(size_t)0 >> 1);
	while (s0 && (p = _nmalloc(s0)) == NULL)
		s0 >>= 1;

	if (p)
		_nfree(p);

	s1 = s0 >> 1;
	while (s1)
	{
		if ((p = _nmalloc(s0 + s1)) != NULL)
		{
			s0 += s1;
			_nfree(p);
		}
	s1 >>= 1;
	}
	while (s0 && (p = _nmalloc(s0)) == NULL)
		s0 ^= s0 & -s0;

	*Size = s0;
	return p;
}

size_t GetNearFreeSize(void)
{
	size_t total = 0;
	void __near* pFirst = NULL;
	void __near* pLast = NULL;
	for(;;)
	{
		size_t largest;
		void __near* p = AllocateLargestNearFreeBlock(&largest);
		if (largest < sizeof(void __near*))
		{
			if (p != NULL)
			_nfree(p);
			break;
		}
		*(void __near* __near*)p = NULL;
		total += largest;
		if (pFirst == NULL)
			pFirst = p;

		if (pLast != NULL)
			*(void __near* __near*)pLast = p;
		pLast = p;
	}

	while (pFirst != NULL)
	{
		void __near* p = *(void __near* __near*)pFirst;
		_nfree(pFirst);
		pFirst = p;
	}
	return total;
}

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
