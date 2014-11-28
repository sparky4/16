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

#if !defined(__COMPILE_INC)
#define __COMPILE_INC

#define VOID 1
#define INT 1
#define CHARPTR 2
#define STRING 3

extern unsigned char *source, *src;
extern unsigned char *outbuf, *code;
extern unsigned char inevent;

typedef struct
{
  char fname[40];
  char argtype[20];
  int numargs, numlocals;
  int returntype;
  int syscodeofs;
} funcdecl;

extern funcdecl funcs[300];
extern int numfuncs;

typedef struct
{
  char vname[40];
  int vsofs;
  int arraylen;
} strdecl;

extern strdecl str[300];
extern int sstartofs;
extern int numstr;

typedef struct
{
  char vname[40];
  int varstartofs;
  int arraylen;
} vardecl;

extern vardecl vars[300];
extern int curstartofs;
extern int numvars;
extern int varidx, startsyscript;
extern char larg[12][40];
extern int c;

extern char *functbl[512];
extern int mfuncs;

extern void CompileMAP(char *fname);
extern void CompileSystem();
extern void Expect(char *str);

extern void EmitC(char c);
extern void EmitW(short int w);
extern void EmitD(int w);
extern void EmitOperand();
extern void EmitString(char *str);
extern void HandleString();

#endif // __COMPILE_INC
