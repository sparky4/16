/* Project 16 Source Code~
 * Copyright (C) 2012-2018 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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

//cpu reg stuff for _AX, _BX, _CX, _DX
#ifdef __WATCOMC__
union regs CPURegs;
#endif

// big global status text buffer
char global_temp_status_text[512];
char global_temp_status_text2[512];

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

// clrstdin() clear any leftover chars tha may be in stdin stream //
void clrstdin()
{
   int ch = 0;
   while( ( ch = getchar() ) != '\n' && ch != EOF );
}

//from http://stackoverflow.com/questions/2736753/how-to-remove-extension-from-file-name
// remove_ext: removes the "extension" from a file spec.
//   mystr is the string to process.
//   dot is the extension separator.
//   sep is the path separator (0 means to ignore).
// Returns an allocated string identical to the original but
//   with the extension removed. It must be freed when you're
//   finished with it.
// If you pass in NULL or the new string can't be allocated,
//   it returns NULL.

char *remove_ext (char* mystr, char dot, char sep) {
	char *retstr, *lastdot, *lastsep;

	// Error checks and allocate string.
	if (mystr == NULL)
		return NULL;
	if ((retstr = malloc(strlen (mystr) + 1)) == NULL)
		return NULL;

	// Make a copy and find the relevant characters.

	strcpy (retstr, mystr);
	lastdot = strrchr (retstr, dot);
	lastsep = (sep == 0) ? NULL : strrchr (retstr, sep);

	// If it has an extension separator.

	if (lastdot != NULL) {
		// and it's before the extenstion separator.

		if (lastsep != NULL) {
			if (lastsep < lastdot) {
				// then remove it.

				*lastdot = '\0';
			}
		} else {
			// Has extension separator with no path separator.

			*lastdot = '\0';
		}
	}

	// Return the modified string.
	free(mystr);
	return retstr;
}


//from http://quiz.geeksforgeeks.org/c-program-cyclically-rotate-array-one/
void rotateR(byte *arr, byte n)
{
	byte x = arr[n-1], i;
	for (i = n-1; i > 0; i--)
		arr[i] = arr[i-1];
	arr[0] = x;
}

void rotateL(byte *arr, byte n)
{
	byte x = arr[n+1], i;
	for (i = n+1; i > 0; i++)
		arr[i] = arr[i+1];
	arr[0] = x;
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

// for input test //
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

//from: http://stackoverflow.com/questions/5349896/print-a-struct-in-c
void print_mem(void const *vp, size_t n)
{
	size_t i;
	unsigned char const *p = vp;
	for (i=0; i<n; i++)
	{
		printf("%02x", p[i]);
		//printf("%c", p[i]);
		if((!(i%16)) && i) printf("\n");
		else printf(" ");
		//printf("%u%%40=%u\n", i, i%40);
	}
	putchar('\n');
	printf("\nstruct size is %zu bytes\n", n);
};

//from: https://groups.google.com/forum/#!topic/comp.lang.asm.x86/QtuVXl43nDo
void hres (void)
{
	__asm {
		mov	ax,3
		int	10h
		mov	ax,1112h
		xor	bx,bx
		int	10h
	}
}

#define REGIDUMP_HEX
#define REGIDUMP_DUMPFLAGS
//#define REGIDUMP_USE_CAPS	//uncomment to use the assembly
//regester dump~
void regidump()
{
	//GENERAL PURPOSE
	unsigned short _ax,_bx,_cx,_dx;
	unsigned short _cflag;
	unsigned char _al,_ah,_bl,_bh,_cl,_ch,_dl,_dh;

	unsigned short _bp,_si,_di,_sp;

	unsigned short _cs,_ds,_es,_ss;	//SEGMENT
//	unsigned short _ip;	//SPECIAL PURPOSE
	_ax=_bx=_cx=_dx=_si=_di=_bp=_sp=_cs=_ds=_es=_ss=_cflag=0;
	_ah=_al=_bh=_bl=_ch=_cl=_dh=_dl=0;

#ifndef REGIDUMP_USE_CAPS
	__asm {
		mov _ax,ax
		mov _bx,bx
		mov _cx,cx
		mov _dx,dx

		mov _si,si
		mov _di,di

		/*mov _ip,ip

		mov _cf,cf
		mov _pf,pf
		mov _af,af
		mov _zf,zf
		mov _sf,sf
		mov _tf,tf
		mov _if,if
		mov _df,df
		mov _of,of*/
	}
#else
_ax=_AX;
_bx=_BX;
_cx=_CX;
_dx=_DX;

_si=_SI;
_di=_DI;

_ah=_AH;
_al=_AL;
_bh=_BH;
_bl=_BL;
_ch=_CH;
_cl=_CL;
_dh=_DH;
_dl=_DL;
#endif
	_cflag=_CFLAG;
	__asm {
		mov _bp,bp
		mov _sp,sp

		mov _cs,cs
		mov _ds,ds
		mov _es,es
		mov _ss,ss
	}
//	printf("integer values:	ax=%04d	bx=%04d	cx=%04d	dx=%04d\n", a, b, c, d);
//	printf("unsigned values:ax=%04u	bx=%04u	cx=%04u	dx=%04u\n", a, b, c, d);
	printf("================================================================================");
	printf("16 bit 8088 register values\n");
	printf("================================================================================");
	printf("general purpose:\n");
#ifndef REGIDUMP_HEX
	printf("	ax=%04u\n	bx=%04u\n	cx=%04u\n	dx=%04u\n\n", _ax, _bx, _cx, _dx);
	printf("	si=%04u\n	di=%04u\n	bp=%04u\n	sp=%04u\n", _si, _di, _bp, _sp);
#else
	printf("	ax=%04x\n	bx=%04x\n	cx=%04x\n	dx=%04x\n\n", _ax, _bx, _cx, _dx);
	printf("	si=%04x\n	di=%04x\n	bp=%04x\n	sp=%04x\n", _si, _di, _bp, _sp);
#endif
	printf("		---------------------------------------\n");



	printf("segment:\n");
#ifndef REGIDUMP_HEX
	printf("	cs=%04u\n	ds=%04u\n	es=%04u\n	ss=%04u\n", _cs, _ds, _es, _ss);
#else
	printf("	cs=%04x\n	ds=%04x\n	es=%04x\n	ss=%04x\n", _cs, _ds, _es, _ss);
#endif
	printf("		---------------------------------------\n");



	printf("cflags:\n");
/*	printf("	ip=%04u\n\n", _ip);
	printf("	cf=%04u\npf=%04u\naf=%04u\nzf=%04u\nsf=%04u\ntf=%04u\nif=%04u\ndf=%04u\nof=%04u\n", _cf, _pf, _af, _zf, _sf, _tf, _if, _df, _of);
	printf("		---------------------------------------\n");*/
#ifdef REGIDUMP_DUMPFLAGS
#ifndef REGIDUMP_HEX
//	printf("	ip=%04u\n\n", _IP);
//	printf("	cf=%04u\npf=%04u\naf=%04u\nzf=%04u\nsf=%04u\ntf=%04u\nif=%04u\ndf=%04u\nof=%04u\n", _CF, _PF, _AF, _ZF, _SF, _TF, _IF, _DF, _OF);
#else
//	printf("	ip=%04x\n\n", _IP);
//	printf("	cf=%04x\npf=%04x\naf=%04x\nzf=%04x\nsf=%04x\ntf=%04x\nif=%04x\ndf=%04x\nof=%04x\n", _CF, _PF, _AF, _ZF, _SF, _TF, _IF, _DF, _OF);
	printf("cflag: "BYTE_TO_BINARY_PATTERN""BYTE_TO_BINARY_PATTERN"\n",		BYTE_TO_BINARY(_cflag>>8), BYTE_TO_BINARY(_cflag));
#endif
	printf("dx: "BYTE_TO_BINARY_PATTERN""BYTE_TO_BINARY_PATTERN"\n",		BYTE_TO_BINARY(_dx>>8), BYTE_TO_BINARY(_dx));
	printf("		---------------------------------------\n");
#endif

	printf("for more info see\n	http://stackoverflow.com/questions/9130349/how-many-registers-are-there-in-8086-8088\n");
	printf("================================================================================");
}
