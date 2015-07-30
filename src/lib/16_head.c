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

void __near* LargestFreeBlock(size_t* Size)
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

size_t _coreleft(void)
{
	size_t total = 0;
	void __near* pFirst = NULL;
	void __near* pLast = NULL;
	for(;;)
	{
		size_t largest;
		void __near* p = LargestFreeBlock(&largest);
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

void far* LargestFarFreeBlock(size_t* Size)
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

size_t _farcoreleft(void)
{
	size_t total = 0;
	void far* pFirst = NULL;
	void far* pLast = NULL;
	for(;;)
	{
		size_t largest;
		void far* p = LargestFarFreeBlock(&largest);
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

void huge* LargestHugeFreeBlock(size_t* Size)
{
	size_t s0, s1;
	void huge* p;

	s0 = ~(size_t)0 ^ (~(size_t)0 >> 1);
	while (s0 && (p = halloc((dword)s0, 1)) == NULL)
		s0 >>= 1;

	if (p)
		hfree(p);

	s1 = s0 >> 1;
	while (s1)
	{
		if ((p = halloc((dword)(s0 + s1), 1)) != NULL)
		{
			s0 += s1;
			hfree(p);
		}
	s1 >>= 1;
	}
	while (s0 && (p = halloc((dword)s0, 1)) == NULL)
		s0 ^= s0 & -s0;

	*Size = s0;
	return p;
}

size_t _hugecoreleft(void)
{
	size_t total = 0;
	void huge* pFirst = NULL;
	void huge* pLast = NULL;
	for(;;)
	{
		size_t largest;
		void huge* p = LargestHugeFreeBlock(&largest);
		if (largest < sizeof(void huge*))
		{
			if (p != NULL)
			hfree(p);
			break;
		}
		*(void huge* huge*)p = NULL;
		total += largest;
		if (pFirst == NULL)
			pFirst = p;

		if (pLast != NULL)
			*(void huge* huge*)pLast = p;
		pLast = p;
	}

	while (pFirst != NULL)
	{
		void huge* p = *(void huge* huge*)pFirst;
		hfree(pFirst);
		pFirst = p;
	}
	return total;
}

/*void __based(__self)* LargestBasedFreeBlock(size_t* Size)
{
	__segment segu;
	size_t s0, s1;
	void __based(__self)* p;

	s0 = ~(size_t)0 ^ (~(size_t)0 >> 1);
	while (s0 && (p = _bmalloc(segu, s0)) == NULL)
		s0 >>= 1;

	if (p)
		_ffree(p);

	s1 = s0 >> 1;
	while (s1)
	{
		if ((p = _bmalloc(segu, s0 + s1)) != NULL)
		{
			s0 += s1;
			_ffree(p);
		}
	s1 >>= 1;
	}
	while (s0 && (p = _bmalloc(segu, s0)) == NULL)
		s0 ^= s0 & -s0;

	*Size = s0;
	return p;
}

size_t _basedcoreleft(void)
{
	__segment segu;
	size_t total = 0;
	void __based(segu)* pFirst = NULL;
	void __based(segu)* pLast = NULL;
	// allocate based heap
	segu = _bheapseg( 1024 );
	if( segu == _NULLSEG ) {
		printf( "Unable to allocate based heap\n" );
		return 0;
		//exit( 1 );
	}
	else

	for(;;)
	{
		size_t largest;
		void __based(segu)* p = LargestBasedFreeBlock(&largest);
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
}*/

size_t GetFreeSize(void)
{
	struct _heapinfo h_info;
	int heap_status;
	size_t h_free=0, h_total=0, h_used=0;

	h_info._pentry = NULL;
	for(;;) {
		heap_status = _heapwalk( &h_info );
		if( heap_status != _HEAPOK ) break;
		if((h_info._useflag == _USEDENTRY ? "USED" : "FREE")=="FREE") h_free += h_info._size;
		if((h_info._useflag == _USEDENTRY ? "USED" : "FREE")=="USED") h_used += h_info._size;
		h_total += h_info._size;
	}
	heapstat(heap_status);
	return h_free;
}

size_t GetFarFreeSize(void)
{
	struct _heapinfo fh_info;
	int heap_status;
	size_t fh_free=0, fh_total=0, fh_used=0;

	fh_info._pentry = NULL;
	for(;;) {
		heap_status = _fheapwalk( &fh_info );
		if( heap_status != _HEAPOK ) break;
		if((fh_info._useflag == _USEDENTRY ? "USED" : "FREE")=="FREE") fh_free += fh_info._size;
		if((fh_info._useflag == _USEDENTRY ? "USED" : "FREE")=="USED") fh_used += fh_info._size;
		fh_total += fh_info._size;
	}
	heapstat(heap_status);
	return fh_free;
}

size_t GetNearFreeSize(void)
{
	struct _heapinfo nh_info;
	int heap_status;
	size_t nh_free=0, nh_total=0, nh_used=0;

	nh_info._pentry = NULL;
	for(;;) {
		heap_status = _nheapwalk( &nh_info );
		if( heap_status != _HEAPOK ) break;
		if((nh_info._useflag == _USEDENTRY ? "USED" : "FREE")=="FREE") nh_free += nh_info._size;
		if((nh_info._useflag == _USEDENTRY ? "USED" : "FREE")=="USED") nh_used += nh_info._size;
		nh_total += nh_info._size;
	}
	heapstat(heap_status);
	return nh_free;
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

void print_normal_entry(char *text, dword total, dword used, dword free)
{
	printf("%-17s", text);
	convert("%8sB ", total);
	convert("%9sB ", used);
	convert("%9sB\n", free);
}

/*
 * As for printf(), but format may only contain a single format specifier,
 * which must be "%s" and is replaced with the string form of num with commas
 * separating groups of three digits.
 *
 * e.g. convert("%s bytes", 1234567) -> "1,234,567 bytes"
 */
void convert(const char *format, dword num)
{
    int c, i, j, n;
    char des[4*sizeof(dword)+3];
    union REGS regs;
    struct SREGS sregs;
    char mycountry[48]; /* probably 34 bytes are enough... */
    char ksep = ',';    /* or . */

    regs.x.ax = 0x3800;
    sregs.ds = FP_SEG(&mycountry);
    regs.x.dx = FP_OFF(&mycountry);
    intdosx(&regs,&regs,&sregs);
    if (regs.x.cflag == 0) {
      ksep = mycountry[7];        /* 1000's separator  */
      /* dsep = mycountry[9];     ** decimal separator */
    }

    n = sprintf(des, "%lu", num);
    /* insert commas in the string */
    c = 3;
    for (i = n - 3; i > 0; i--) {
        if (c%3==0) {
            for (j = n; j >= i; j--)
                des[j+1] = des[j];
            des[i]=ksep;        /* ',' */
            n++;
        }
        c++;
    }
    printf(format, des);
}

void heapdump(void)
{
	struct _heapinfo fh_info, nh_info, h_info;
	int heap_status;
	size_t h_free, nh_free, fh_free, h_total, nh_total, fh_total, h_used, nh_used, fh_used;

	printf("\n	== default ==\n\n");
	h_info._pentry = NULL;
	h_free=0; h_total=0; h_used=0;
	for(;;) {
		heap_status = _heapwalk( &h_info );
		if( heap_status != _HEAPOK ) break;
		printf( "  %s block at %Fp of size %4.4X\n",
(h_info._useflag == _USEDENTRY ? "USED" : "FREE"),
h_info._pentry, h_info._size );
		if((h_info._useflag == _USEDENTRY ? "USED" : "FREE")=="FREE") h_free += h_info._size;
		if((h_info._useflag == _USEDENTRY ? "USED" : "FREE")=="USED") h_used += h_info._size;
		h_total += h_info._size;
	}
	heapstat(heap_status);

	//near
	printf("\n	== near ==\n\n");
	nh_info._pentry = NULL;
	nh_free=0; nh_total=0; nh_used=0;
	for(;;) {
		heap_status = _nheapwalk( &nh_info );
		if( heap_status != _HEAPOK ) break;
		printf( "  %s block at %Fp of size %4.4X\n",
(nh_info._useflag == _USEDENTRY ? "USED" : "FREE"),
nh_info._pentry, nh_info._size );
		if((nh_info._useflag == _USEDENTRY ? "USED" : "FREE")=="FREE") nh_free += nh_info._size;
		if((nh_info._useflag == _USEDENTRY ? "USED" : "FREE")=="USED") nh_used += nh_info._size;
		nh_total += nh_info._size;
	}
	heapstat(heap_status);

	//far
	printf("\n	== far ==\n\n");
	fh_info._pentry = NULL;
	fh_free=0; fh_total=0; fh_used=0;
	for(;;) {
		heap_status = _fheapwalk( &fh_info );
		if( heap_status != _HEAPOK ) break;
		printf( "  %s block at %Fp of size %4.4X\n",
(fh_info._useflag == _USEDENTRY ? "USED" : "FREE"),
fh_info._pentry, fh_info._size );
		if((fh_info._useflag == _USEDENTRY ? "USED" : "FREE")=="FREE") fh_free += fh_info._size;
		if((fh_info._useflag == _USEDENTRY ? "USED" : "FREE")=="USED") fh_used += fh_info._size;
		fh_total += fh_info._size;
	}
	heapstat(heap_status);

printf("\n");
printf(kittengets(2,0,"Memory Type         Total      Used       Free\n"));
printf(      "----------------  --------   --------   --------\n");
print_normal_entry(kittengets(2,1,"Default"), (dword)h_total, (dword)h_used, (dword)h_free);
print_normal_entry(kittengets(2,1,"Near"), (dword)nh_total, (dword)nh_used, (dword)nh_free);
print_normal_entry(kittengets(2,1,"Far"), (dword)fh_total, (dword)fh_used, (dword)fh_free);
printf(      "----------------  --------   --------   --------\n");
//printf("memavl = %lu\n", (dword)_memavl());
printf("stackavail = %u\n", stackavail());
}

void heapstat(int heap_status)
{
	switch( heap_status ) {
		case _HEAPEND:
			//printf( "OK - end of heap\n" );
		break;
		case _HEAPEMPTY:
			//printf( "OK - heap is empty\n" );
		break;
		case _HEAPBADBEGIN:
			printf( "ERROR - heap is damaged\n" );
		break;
		case _HEAPBADPTR:
			printf( "ERROR - bad pointer to heap\n" );
		break;
		case _HEAPBADNODE:
			printf( "ERROR - bad node in heap\n" );
	}
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
