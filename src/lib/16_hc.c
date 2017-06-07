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
	heap test stuff
*/

#include "src/lib/16_hc.h"
#include <malloc.h>

//from ftp://213.85.246.177/pub/FreeBSD/ports/archivers/arj/work/arj-3.10.22/environ.c
#if 0
//#ifdef __WATCOMC__
long HC_Newfarcoreleft()
{
	void __huge *hp;		static long rc=736L;	long s_rc;

	s_rc=rc;	rc+=2L;
	do
		hp=halloc(rc-=2L, 1024);
	while(hp==NULL&&rc>0L);
	if(hp!=NULL)
		hfree(hp);
	if(rc<s_rc)
		return(rc*1024L);
	do
	{
		hp=halloc(rc+=16L, 1024);
		if(hp!=NULL)
			hfree(hp);
	} while(hp!=NULL);
	return((rc-16L)*1024L);
}
#endif

void
#ifdef __BORLANDC__
 *
#endif
#ifdef __WATCOMC__
 __near*
#endif
HC_LargestFreeBlock(size_t* Size)
{
	size_t s0, s1;
#ifdef __BORLANDC__
	void * p;
#endif
#ifdef __WATCOMC__
	void __near* p;
#endif

	s0 = ~(size_t)0 ^ (~(size_t)0 >> 1);
#ifdef __BORLANDC__
	while (s0 && (p = malloc(s0)) == NULL)
#endif
#ifdef __WATCOMC__
	while (s0 && (p = _nmalloc(s0)) == NULL)
#endif
		s0 >>= 1;

	if (p)
#ifdef __BORLANDC__
		free(p);
#endif
#ifdef __WATCOMC__
		_nfree(p);
#endif

	s1 = s0 >> 1;
	while (s1)
	{
#ifdef __BORLANDC__
		if ((p = malloc(s0 + s1)) != NULL)
#endif
#ifdef __WATCOMC__
		if ((p = _nmalloc(s0 + s1)) != NULL)
#endif
		{
			s0 += s1;
#ifdef __BORLANDC__
			free(p);
#endif
#ifdef __WATCOMC__
			_nfree(p);
#endif
		}
	s1 >>= 1;
	}
#ifdef __BORLANDC__
	while (s0 && (p = malloc(s0)) == NULL)
#endif
#ifdef __WATCOMC__
	while (s0 && (p = _nmalloc(s0)) == NULL)
#endif
		s0 ^= s0 & -s0;

	*Size = s0;
	return p;
}

size_t HC_coreleft(void)
{
	size_t total = 0;
	void __near* pFirst = NULL;
	void __near* pLast = NULL;
	for(;;)
	{
		size_t largest;
		void __near* p = (void __near *)HC_LargestFreeBlock(&largest);
		if (largest < sizeof(void __near*))
		{
			if (p != NULL)
#ifdef __BORLANDC__
			free(p);
#endif
#ifdef __WATCOMC__
			_nfree(p);
#endif
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
#ifdef __BORLANDC__
		free(pFirst);
#endif
#ifdef __WATCOMC__
		_nfree(pFirst);
#endif
		pFirst = p;
	}
	return total;
}

void far* HC_LargestFarFreeBlock(unsigned long* Size)
{
	unsigned long s0, s1;
	void far* p;

	s0 = ~(unsigned long)0 ^ (~(unsigned long)0 >> 1);
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

unsigned long HC_farcoreleft(void)
{
	unsigned long total = 0UL;
	void far* pFirst = NULL;
	void far* pLast = NULL;
	for(;;)
	{
		unsigned long largest;
		void far* p = HC_LargestFarFreeBlock(&largest);
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

#ifdef __WATCOMC__
/*void huge* LargestHugeFreeBlock(size_t* Size)
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

void __based(__self)* LargestBasedFreeBlock(size_t* Size)
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
	segu = _bHC_heapseg( 1024 );
	if( segu == _NULLSEG ) {
		printf( "Unable to allocate based heap\n" );
		return 0;

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
#if 0
size_t HC_GetFreeSize(void)
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
	HCL_heapstat0(heap_status);
	return h_free;
}
#endif

unsigned long HC_GetFarFreeSize(void)
{
	struct _heapinfo fh_info;
	int heap_status;
	unsigned long fh_free=0, fh_total=0, fh_used=0;

	fh_info._pentry = NULL;
	for(;;) {
		heap_status = _fheapwalk( &fh_info );
		if( heap_status != _HEAPOK ) break;
		if((fh_info._useflag == _USEDENTRY ? "USED" : "FREE")=="FREE") fh_free += fh_info._size;
		if((fh_info._useflag == _USEDENTRY ? "USED" : "FREE")=="USED") fh_used += fh_info._size;
		fh_total += fh_info._size;
	}
	HCL_heapstat0(heap_status);
	return fh_free;
}

size_t HC_GetNearFreeSize(void)
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
	HCL_heapstat0(heap_status);
	return nh_free;
}

void HC_heapdump(global_game_variables_t *gvar)
{
	struct _heapinfo fh_info, nh_info;//, h_info;
	int heap_status;
	size_t nh_free, fh_free, nh_total, fh_total, nh_used, fh_used;//,	h_free, h_total, h_used;
	byte	scratch[1024],str[16];

	HC_OpenDebug(gvar);

#if 0
	strcpy(scratch,"\n	== default ==\n\n");
	write(gvar->handle.heaphandle,scratch,strlen(scratch));
	h_info._pentry = NULL;
	h_free=0; h_total=0; h_used=0;
	for(;;) {
		heap_status = _heapwalk( &h_info );
		if( heap_status != _HEAPOK ) break;
		strcpy(scratch,"  "); strcat(scratch,(h_info._useflag == _USEDENTRY ? "USED" : "FREE")); strcat(scratch," block at ");
		sprintf(str, "%Fp", h_info._pentry); //ultoa((dword)h_info._pentry,str,16);
			strcat(scratch,str); strcat(scratch," of size "); ultoa(h_info._size,str,10); strcat(scratch,str); strcat(scratch,"\n");
		if((h_info._useflag == _USEDENTRY ? "USED" : "FREE")=="FREE") h_free += h_info._size;
		if((h_info._useflag == _USEDENTRY ? "USED" : "FREE")=="USED") h_used += h_info._size;
		h_total += h_info._size;
		write(gvar->handle.heaphandle,scratch,strlen(scratch));
	}
	HCL_heapstat(gvar, heap_status, &scratch);
#endif

	//near
	strcpy(scratch,"\n	== near ==\n\n");
	write(gvar->handle.heaphandle,scratch,strlen(scratch));
	nh_info._pentry = NULL;
	nh_free=0; nh_total=0; nh_used=0;
	for(;;) {
		heap_status = _nheapwalk( &nh_info );
		if( heap_status != _HEAPOK ) break;
		strcpy(scratch,"  "); strcat(scratch,(nh_info._useflag == _USEDENTRY ? "USED" : "FREE")); strcat(scratch," block at ");
		sprintf(str, "%Fp", nh_info._pentry); //ultoa((dword)nh_info._pentry,str,16);
			strcat(scratch,str); strcat(scratch," of size "); ultoa(nh_info._size,str,10); strcat(scratch,str); strcat(scratch,"\n");
/*		printf( "  %s block at %Fp of size %4.4X\n",
(nh_info._useflag == _USEDENTRY ? "USED" : "FREE"),
nh_info._pentry, nh_info._size );*/
		if((nh_info._useflag == _USEDENTRY ? "USED" : "FREE")=="FREE") nh_free += nh_info._size;
		if((nh_info._useflag == _USEDENTRY ? "USED" : "FREE")=="USED") nh_used += nh_info._size;
		nh_total += nh_info._size;
		write(gvar->handle.heaphandle,scratch,strlen(scratch));
	}
	HCL_heapstat(gvar, heap_status, &scratch);

	//far
	strcpy(scratch,"\n	== far ==\n\n");
	write(gvar->handle.heaphandle,scratch,strlen(scratch));
	fh_info._pentry = NULL;
	fh_free=0; fh_total=0; fh_used=0;
	for(;;) {
		heap_status = _fheapwalk( &fh_info );
		if( heap_status != _HEAPOK ) break;
		strcpy(scratch,"  "); strcat(scratch,(fh_info._useflag == _USEDENTRY ? "USED" : "FREE")); strcat(scratch," block at ");
		sprintf(str, "%Fp", fh_info._pentry); //ultoa((dword)fh_info._pentry,str,16);
			strcat(scratch,str); strcat(scratch," of size "); ultoa(fh_info._size,str,10); strcat(scratch,str); strcat(scratch,"\n");
		/*printf( "  %s block at %Fp of size %4.4X\n",
(fh_info._useflag == _USEDENTRY ? "USED" : "FREE"),
fh_info._pentry, fh_info._size );*/
		if((fh_info._useflag == _USEDENTRY ? "USED" : "FREE")=="FREE") fh_free += fh_info._size;
		if((fh_info._useflag == _USEDENTRY ? "USED" : "FREE")=="USED") fh_used += fh_info._size;
		fh_total += fh_info._size;
		write(gvar->handle.heaphandle,scratch,strlen(scratch));
	}
	HCL_heapstat(gvar, heap_status, &scratch);

	strcpy(scratch,"\n");
	strcat(scratch,kittengets(2,0,"Memory Type         Total      Used       Free\n"));
	strcat(scratch,"----------------  --------   --------   --------\n");
//	printmeminfoline(&scratch, "Default", h_total, h_used, h_free);
	printmeminfoline(&scratch, "Near", nh_total, nh_used, nh_free);
	printmeminfoline(&scratch, "Far", fh_total, fh_used, fh_free);
	strcat(scratch,"----------------  --------   --------   --------\n");
	strcat(scratch,"HC_coreleft = ");			ultoa((dword)HC_coreleft(),str,10);			strcat(scratch,str);	strcat(scratch,"\n");
	strcat(scratch,"HC_farcoreleft = ");			ultoa((dword)HC_farcoreleft(),str,10);		strcat(scratch,str);	strcat(scratch,"\n");
//	strcat(scratch,"HC_Newfarcoreleft = ");		ultoa((dword)HC_Newfarcoreleft(),str,10);		strcat(scratch,str);	strcat(scratch,"\n");
//	strcat(scratch,"HC_GetFreeSize = ");		ultoa((dword)HC_GetFreeSize(),str,10);		strcat(scratch,str);	strcat(scratch,"\n");
	strcat(scratch,"HC_GetNearFreeSize = ");	ultoa((dword)HC_GetNearFreeSize(),str,10);	strcat(scratch,str);	strcat(scratch,"\n");
	strcat(scratch,"HC_GetFarFreeSize = "); 	ultoa((dword)HC_GetFarFreeSize(),str,10);	strcat(scratch,str);	strcat(scratch,"\n");
	strcat(scratch,"coreleft = ");				ultoa((dword)coreleft(),str,10);				strcat(scratch,str);	strcat(scratch,"\n");
	strcat(scratch,"farcoreleft = ");			ultoa((dword)farcoreleft(),str,10);			strcat(scratch,str);	strcat(scratch,"\n");
	strcat(scratch,"stackavail = ");			ultoa((dword)stackavail(),str,10);			strcat(scratch,str);	strcat(scratch,"\n");
	write(gvar->handle.heaphandle,scratch,strlen(scratch));
	HC_CloseDebug(gvar);
}

void HCL_heapstat(global_game_variables_t *gvar, int heap_status, byte *str)
{
	switch( heap_status ) {
		case _HEAPEND:
			strcpy((str),"OK - end of heap\n");
		break;
		case _HEAPEMPTY:
			strcpy((str),"OK - heap is empty\n");

		break;
		case _HEAPBADBEGIN:
			strcpy((str),"ERROR - heap is damaged\n");
		break;
		case _HEAPBADPTR:
			strcpy((str),"ERROR - bad pointer to heap\n");
		break;
		case _HEAPBADNODE:
			strcpy((str),"ERROR - bad node in heap\n");
	}
	write(gvar->handle.heaphandle,(str),strlen((str)));
}

void HCL_heapstat0(int heap_status)
{
	switch( heap_status ) {
		case _HEAPEND:
			//printf("OK - end of heap\n");
		break;
		case _HEAPEMPTY:
			//printf("OK - heap is empty\n");
		break;
		case _HEAPBADBEGIN:
			printf("ERROR - heap is damaged\n");
		break;
		case _HEAPBADPTR:
			printf("ERROR - bad pointer to heap\n");
		break;
		case _HEAPBADNODE:
			printf("ERROR - bad node in heap\n");
	}
}

unsigned long farcoreleft()
{
//----	_fheapgrow();
	return HC_farcoreleft();
//stack overflows	return HC_GetFarFreeSize();
}

unsigned long coreleft()
{
	_nheapgrow();
	return _memavl();
//	return HC_GetNearFreeSize();
}
#endif

/*
============================
=
= HC_OpenDebug / HC_CloseDebug
=
= Opens a binary file with the handle "heaphandle"
=
============================
*/
void HC_OpenDebug(global_game_variables_t *gvar)
{
#ifdef __BORLANDC__
	unlink("heap.16b");
	gvar->handle.heaphandle = open(gvar->handle.heapdumpfilename, O_CREAT | O_WRONLY | O_TEXT);
#endif
#ifdef __WATCOMC__
	unlink("heap.16w");
	gvar->handle.heaphandle = open(gvar->handle.heapdumpfilename, O_CREAT | O_WRONLY | O_TEXT);
#endif
}

void HC_CloseDebug(global_game_variables_t *gvar)
{
	close(gvar->handle.heaphandle);

#ifdef __BORLANDC__
	strcpy(gvar->handle.heapdumpfilename, "heap.16b");
#endif
#ifdef __WATCOMC__
	strcpy(gvar->handle.heapdumpfilename, "heap.16w");
#endif
}
