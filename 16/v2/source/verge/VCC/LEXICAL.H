/*
Copyright (C) 1998 BJ Eirich (aka vecna)
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public Lic
See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#if !defined(__LEXICAL_INC)
#define __LEXICAL_INC

extern void ParseWhitespace();
extern void GetNumber();
extern void GetToken();
extern void InitCompileSystem();
extern int NextIs(char *str);

extern void GetString(void);

extern char token[2000];                 // Token buffer
extern unsigned int token_nvalue;        // int value of token if it's type DIGIT
extern char token_type;                  // type of current token.
extern char token_subtype;               // This is just crap.
extern unsigned char chr_table[256];              // Character type table.
extern char returntypes[];
extern int lines, tlines;
extern char *source_file;

extern char TokenIs(char *str);
extern void Expect(char *str);

extern int numhardfuncs, funcidx;
extern int varcategory, numhardvar0;
extern int numhardvar1;
extern char *hardfuncs[];
extern char *hardvar0[];
extern char *hardvar1[];

#endif // __LEXICAL_INC
