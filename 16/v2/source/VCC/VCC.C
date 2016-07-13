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

// ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
// ³                        The VergeC Compiler                          ³
// ³              Copyright (C)1998 BJ Eirich (aka vecna)                ³
// ³                            Main module                              ³
// ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

#define VERSION "2.01c"

#ifdef __DJGPP__
#define BUILD_TAG "DJGPP V2\0"
#endif

#ifdef __WATCOMC__
#define BUILD_TAG "Watcom 11.0\0"
#endif

#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "compile.h"
#include "lexical.h"

extern int pp_dump;
extern int pp_nomark;

// ================================= Data ====================================

char outmode, cmode;
char fname[80];
char quiet, verbose;
char *strbuf;
int locate=0;

// -- locals --

//FILE *f;
//int i;
//struct find_t *ft;

// ================================= Code ====================================

void dprint(char *message, ...)
{
  va_list  lst;
  char     string[1024];

  if (quiet) return;

  // compose message
  va_start (lst, message);
  vsprintf (string, message, lst);
  va_end   (lst);

  printf   ("%s \n", string);
}

void vprint(char *message, ...)
{
  va_list  lst;
  char     string[1024];

  if (!verbose) return;

  // compose message
  va_start (lst, message);
  vsprintf (string, message, lst);
  va_end   (lst);

  printf   ("%s \n", string);
}

void err(char *message, ...)
{
  va_list  lst;
  char     string[1024];

  // compose message
  va_start (lst, message);
  vsprintf (string, message, lst);
  va_end   (lst);

  if (quiet)
  {
    FILE *efile = fopen("ERROR.TXT", "w");

    fprintf(efile, "%s \n", string);
    fclose(efile);
  }
  else
  {
    printf("%s \n", string);
  }

  remove("vcctemp.$$$");
  exit(-1);
}

void vcerr(char *message, ...)
{
  va_list  lst;
  char     string[1024];

  // compose message
  va_start (lst, message);
  vsprintf (string, message, lst);
  va_end   (lst);

  err("%s(%d) %s", source_file, lines, string);
}

void vcc_compile_mode_map(char *filename)
{
  FILE *o=0;
  FILE *f=0;
  char *x=0;
  int   z=0;

  x = filename;
  while ('.' != *x) ++x;
  *x = 0;

  CompileMAP(filename);

  sprintf(strbuf, "%s.map", filename);
  f = fopen(strbuf, "rb+");
  if (!f)
    err("unable to open %s.", strbuf);

  fread(strbuf, 1, 6, f);
  fread(&z, 1, 4, f);
  fseek(f, 0, 0);

  o = fopen("outtemp.$$$", "wb");
  if (!o)
    err("unable to open outtemp.$$$");
  x = (char *) malloc(z);
  fread(x, 1, z, f);
  fwrite(x, 1, z, o);
  fclose(f);

  fwrite(&mfuncs, 1, 4, o);
  fwrite(&functbl, 4, mfuncs, o);
  mfuncs = (int) code - (int) outbuf;
  fwrite(&mfuncs, 1, 4, o);
  fwrite(outbuf, 1, code-outbuf, o);
  fclose(o);

  // remove existing map file
  sprintf(strbuf,"%s.map", filename); remove(strbuf);
  // rename temp file to map file name
  rename("outtemp.$$$", strbuf);
}

void vcc_compile_mode_system()
{
  FILE *dump=0;

  CompileSystem();

  dump = fopen("system.vcs", "wb");
  if (!dump) err("unable to open system.vcs");

  fwrite(&numfuncs, 1, 4, dump);
  fwrite(&curstartofs, 1, 4, dump);
  fwrite(&sstartofs, 1, 4, dump);
  fwrite(outbuf, 1, code-outbuf, dump);
  fputc(255, dump);

  fclose(dump);
}

void vcc_compile_mode_all()
{
  FILE *o=0;
  FILE *f=0;
  char *x=0;
  int   z=0;
  struct find_t fileinfo;

  CompileSystem();
  f=fopen("system.vcs","wb");
  fwrite(&numfuncs, 1, 4, f);
  fwrite(&curstartofs, 1, 4, f);
  fwrite(&sstartofs, 1, 4, f);
  fwrite(outbuf, 1, code-outbuf, f);
  fputc(255, f);
  fclose(f);
  free(source);
  free(outbuf);

  if (_dos_findfirst("*.MAP", _A_NORMAL, &fileinfo))
    err("No mapfiles found.");

  while (1)
  {
    int i;
    memcpy(fname, fileinfo.name, 13);

    i=0;
    while (fname[i]!='.') i++;
    fname[i]=0;

    strlwr(fname);
    mfuncs=0;

    CompileMAP(fname);

    sprintf(strbuf,"%s.map", fname);
    f=fopen(strbuf,"rb+");
    fread(strbuf, 1, 6, f);
    fread(&z, 1, 4, f);
    fseek(f, 0, 0);

    o=fopen("outtemp.$$$","wb");
    x=(char *) malloc(z);
    fread(x, 1, z, f);
    fwrite(x, 1, z, o);
    fclose(f);
    free(x);

    fwrite(&mfuncs, 1, 4, o);
    fwrite(&functbl, 4, mfuncs, o);
    mfuncs=(int) code - (int) outbuf;
    fwrite(&mfuncs, 1, 4, o);
    fwrite(outbuf, 1, code-outbuf, o);
    fclose(o);

    sprintf(strbuf,"%s.map", fname);
    remove(strbuf);
    rename("outtemp.$$$",strbuf);

    free(source);
    free(outbuf);

    if (!_dos_findnext(&fileinfo))
      continue;

    break;
  }

  dprint("%i total VC lines compiled.", tlines);
}

int main(int argc, char *argv[])
{
  int loop = 0;
  char c = 0;
  char *argstr = 0;

  strbuf= (char *) malloc(2000);

  cmode = 0;
  pp_dump = 0;
  pp_nomark = 0;

  for (loop = 1; loop < argc; loop++)
  {
    // point to argument string
    argstr = &loop[argv][0];

    c = *argstr;
    // skip leading punctuators, if any
    if ('-' == c || '+' == c || '/' == c)
      ++argstr;

    if (!stricmp(argstr, "v"))
      { verbose = 1; continue; }

    if (!stricmp(argstr, "q"))
      { quiet = 1; continue; }

    // compile SYSTEM.VC only
    if (!stricmp(argstr, "system"))
      { cmode = 2; continue; }

    // compile all available .VC files
    if (!stricmp(argstr, "all"))
      { cmode = 3; continue; }

    // disable line/#include markers
    if (!stricmp(argstr, "ppnomark"))
      { pp_nomark = 1; continue; }

    // dump preprocessor output to temp files
    if (!stricmp(argstr, "ppdump"))
      { pp_dump = 1; continue; }

    // debug locator option
    if ('.' == *argstr)
      { locate = atoi(argstr+1); continue; }

    // at this point, the argument is assumed to be a file

    if (strlen(argstr) > 79)
      { printf("filename '%s' too long!", argstr); argstr[79] = 0; }
    memcpy(fname, argstr, strlen(argstr)+1);

    cmode = 1;
    continue;
  }

  dprint("vcc v.%s Copyright (C)1998 Benjamin Eirich. All rights reserved.", VERSION);
  vprint("%s build %s on %s %s", BUILD_TAG, __FILE__, __DATE__, __TIME__);

  if (!cmode)
    err("No input files.");

  InitCompileSystem();

  switch (cmode)
  {
    case 1: vcc_compile_mode_map(fname); break;
    case 2: vcc_compile_mode_system(); break;
    case 3: vcc_compile_mode_all(); break;

    default: err("you have now entered the twilight zone.");
  }

  remove("vcctemp.$$$");
  remove("ERROR.TXT");

  return 0;
}
