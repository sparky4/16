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

#include "src/lib/16_head.h"

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
