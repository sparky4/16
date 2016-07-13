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
// ³                  The VergeC Compiler version 2.01                   ³
// ³              Copyright (C)1998 BJ Eirich (aka vecna)                ³
// ³                           Lexical Parser                            ³
// ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// CHANGELOG:
// <aen, may 16>
// + ConvHexToDec() was severely screwed (would not return correct values at
//   *all*; own fault)--fixed
// <aen, may 14>
// + fixed some problems with tick mark parsing. didn't like certain chars.
// <aen, may 9>
// + fixed floating point exception crash in ConvHexToDec(), which occured
//   when using very large hex numbers (i think; like $ffffffff). was due to
//   use of pow()
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "compile.h"
#include "vcc.h"
#include "vccode.h"

// ================================= Data ====================================

// Character types

#define LETTER 1
#define DIGIT 2
#define SPECIAL 3

// ---------------

char token[2000];                 // Token buffer
int token_nvalue;                 // int value of token if it's type DIGIT
char token_type;                  // type of current token.
char token_subtype;               // This is just crap.
unsigned char chr_table[256];              // Character type table.

int lines, tlines;                // current line number being processed in
char *source_file;                // the current source file

// ----------------

char *hardfuncs[]=
{
  // A
  "exit",               "message",              "malloc",
  "free",               "pow",                  "loadimage",
  "copysprite",         "tcopysprite",          "render",
  "showpage",           "entityspawn",          "setplayer",
  "map",                "loadfont",             "playfli",

  // B
  "gotoxy",             "printstring",          "loadraw",
  "settile",            "allowconsole",         "scalesprite",
  "processentities",    "updatecontrols",       "unpress",
  "entitymove",         "hline",                "vline",
  "line",               "circle",               "circlefill",   // 30

  // C
  "rect",               "rectfill",             "strlen",
  "strcmp",             "cd_stop",              "cd_play",
  "fontwidth",          "fontheight",           "setpixel",
  "getpixel",           "entityonscreen",       "random",
  "gettile",            "hookretrace",          "hooktimer",

  // D
  "setresolution",      "setrstring",           "setcliprect",
  "setrenderdest",      "restorerendersettings","partymove",
  "sin",                "cos",                  "tan",
  "readmouse",          "setclip",              "setlucent",
  "wrapblit",           "twrapblit",            "setmousepos",  // 60

  // E
  "hookkey",            "playmusic",            "stopmusic",
  "palettemorph",       "fopen",                "fclose",
  "quickread",          "addfollower",          "killfollower",
  "killallfollowers",   "resetfollowers",       "flatpoly",
  "tmappoly",           "cachesound",           "freeallsounds",

  // F
  "playsound",          "rotscale",             "mapline",
  "tmapline",           "val",                  "tscalesprite",
  "grabregion",         "log",                  "fseekline",
  "fseekpos",           "fread",                "fgetbyte",
  "fgetword",           "fgetquad",             "fgetline",     // 90

  // G
  "fgettoken",          "fwritestring",         "fwrite",
  "frename",            "fdelete",              "fwopen",
  "fwclose",            "memcpy",               "memset",
  "silhouette",         "initmosaictable",      "mosaic",
  "writevars",          "readvars",             "callevent",    // 105

  // H
  "asc",                "callscript",           "numforscript",
  "filesize",           "ftell",                "checkcorrupt"
 };

char returntypes[]=
{
  // A
  0, 0, 1,
  0, 1, 1,
  0, 0, 0,
  0, 1, 0,
  0, 1, 0,

  // B
  0, 0, 1,
  0, 0, 0,
  0, 0, 0,
  0, 0, 0,
  0, 0, 0,

  // C
  0, 0, 1,
  1, 0, 0,
  1, 1, 0,
  1, 1, 1,
  1, 0, 0,

  // D
  0, 0, 0,
  0, 0, 0,
  1, 1, 1,
  0, 0, 0,
  0, 0, 0,

  // E
  0, 0, 0,
  0, 1, 0,
  0, 0, 0,
  0, 0, 0,
  0, 1, 0,

  // F
  0, 0, 0,
  0, 1, 0,
  0, 0, 0,
  0, 0, 1,
  1, 1, 0,

  // G
  0, 0, 0,
  0, 0, 1,
  0, 0, 0,
  0, 1, 0,
  0, 0, 0,

  // H
  1, 0, 1,
  1, 1, 0
};

char *hardvar0[]=
{
  "xwin",               "ywin",                 "cameratracking",
  "timer",              "up",                   "down",
  "left",               "right",                "b1",
  "b2",                 "b3",                   "b4",
  "screenx",            "screeny",              "player",       // 15

  "numentsonscreen",    "tracker",              "mx",
  "my",                 "mb",                   "vctrace",
  "image_width",        "image_height",         "music_volume", // 24
  "vsp",                "lastent",              "last_pressed"
};

char *hardvar1[]=
{
  "screen",             "entity.x",             "entity.y",
  "entity.tx",          "entity.ty",            "entity.facing",
  "entity.moving",      "entity.specframe",     "entity.speed",
  "entity.movecode",    "entsonscreen",         "key",
  "layer.hline",        "byte",                 "word",         // 15

  "quad",               "pal",                  "sbyte",
  "sword",              "squad"
};

int funcidx      = 0;
int varcategory  = 0;

int numhardfuncs = 111;
int numhardvar0  = 27;
int numhardvar1  = 20;

// ================================= Code ====================================

int streq(char *a, char *b)
{
  while (*a)
  {
    if (*a++ != *b++)
      return 0;
  }
  return (*b==0);
}

char TokenIs(char *str)
  { return streq(str,token); }

void ParseWhitespace(void)
{
  while (1)
  {
    while (*src<=' ' && *src>2)
      if (!*src++)
        return;

    if (src[0]=='/' && src[1]=='/')
    {
      while (*src && (*src != '\n'))
        src++;
      continue;
    }

    if (src[0]=='/' && src[1]=='*')
    {
      while (!(src[0]=='*' && src[1]=='/'))
      {
        src++;
        if (!*src)
          return;
        if (*src == 1)
        {
          src++;
          source_file = src;
          while (*src++);
          tlines++;
          continue;
        }
        if (*src==2)
        {
          src++;
          lines = (int) *(int *)src;
          src += 4;
          continue;
        }
      }
      src+=2;
      continue;
    }

    if (*src == 1)
    {
      src++;
      source_file = src;
      while (*src++);
      continue;
    }
    if (*src==2)
    {
      src++;
      lines = (int) *(int *)src;
      src += 4;
      tlines++;
      continue;
    }
    break;
  }
}

int GetStringIdx(char i)
{
  int j, k=0;

  for (j=0; j<i; j++)
  {
    if (funcs[c].argtype[j]==STRING) k++;
 }
 return k;
}

void CheckLibFunc()
{ int i;

  token_nvalue=0;
  token_type=0;

  if (TokenIs("if") || TokenIs("else") || TokenIs("for") ||
      TokenIs("while") || TokenIs("switch") || TokenIs("case") ||
      TokenIs("goto"))
  {
    token_type=RESERVED;
    return;
  }
  if (TokenIs("and"))
  {
     token_type=CONTROL;
     token[0]='&'; token[1]='&'; token[2]=0;
     return;
  }
  if (TokenIs("or"))
  {
     token_type=CONTROL;
     token[0]='|'; token[1]='|'; token[2]=0;
     return;
  }
  i=0;
  while (i<numhardfuncs)
  {
    if (!strcmp(hardfuncs[i], token)) break;
    i++;
  }
  if (i<numhardfuncs)
  {
    token_type=FUNCTION;
    token_subtype=op_BFUNC;
    funcidx=i;
  }
  i=0;
  while (i<numhardvar0)
  {
    if (!strcmp(hardvar0[i], token)) break;
    i++;
  }
  if (i<numhardvar0)
  {
    token_type=IDENTIFIER;
    varcategory=op_HVAR0;
    varidx=i;
  }
  i=0;
  while (i<numhardvar1)
  {
    if (!strcmp(hardvar1[i], token)) break;
    i++;
  }
  if (i<numhardvar1)
  {
    token_type=IDENTIFIER;
    varcategory=op_HVAR1;
    varidx=i;
  }

  i=0;
  while (i<numfuncs)
  {
    if (!strcmp(funcs[i].fname, token)) break;
    i++;
  }
  if (i<numfuncs)
  {
    token_type=FUNCTION;
    token_subtype=op_UFUNC;
    funcidx=i;
  }

  i=0;
  while (i<numvars)
  {
    if (!strcmp(vars[i].vname, token)) break;
    i++;
  }
  if (i<numvars)
  {
    token_type=IDENTIFIER;
    varidx=i;
    varcategory=op_UVAR;
    if (vars[varidx].arraylen>1) varcategory=op_UVARRAY;
  }
  i=0;
  while (i<funcs[c].numlocals)
  {
    if (!strcmp(larg[i], token)) break;
    i++;
  }
  if (i<funcs[c].numlocals)
  {
    token_type=IDENTIFIER;
    varidx=i;
    switch (funcs[c].argtype[varidx])
    {
      case INT: varcategory=op_LVAR; break;
      case STRING: varcategory=op_SLOCAL;
                   varidx=GetStringIdx(varidx);
                   break;
      default: vcerr("um.");
    }
  }
  i=0;
  while (i<numstr)
  {
    if (!strcmp(str[i].vname, token)) break;
    i++;
  }
  if (i<numstr)
  {
    token_type=IDENTIFIER;
    varidx=i;
    varcategory=op_STRING;
    if (str[i].arraylen>1) varcategory=op_SARRAY;
  }
}

void GetIdentifier(void)
{
  int i;

  i=0;
  while ((chr_table[*src] == LETTER) || (chr_table[*src] == DIGIT))
    token[i++] = *src++;
  token[i]=0;
  strlwr(token);
  CheckLibFunc();
}

void ConvHexToDec()
{
  static const char *const hextbl="0123456789abcdef\0";
  static int pow_lut[]={1,16,256,4096,65536,1048576,16777216,268435456};
  int i=0, pos=0, z=0;
  unsigned int j=0;

  if (strlen(token)>9)
    vcerr("Hex number exceeds 8 digit maximum.");

  strlwr(token);
  i=strlen(token)-1;
  token_nvalue=0;
  for (pos=0; i>0; pos++,i--)
  {
    z=token[i];
    for (j=0; j<16; j++)
      if (hextbl[j]==z) break;
    if (j>=16)
      vcerr("Invalid hex number.");
    token_nvalue += (j * pow_lut[pos]);
  }
}

void DoTickMarks()
{
  token_nvalue=token[1];
}

void GetNumber()
{
  int i;

  if ('\'' == *src)
  {
    token[0]=*src++;
    token[1]=*src++;
    token[2]=*src++;
    token[3]=0;
  }
  else
  {
    i=0;
    while (chr_table[*src] != SPECIAL)
      token[i++]=*src++;
    token[i]=0;
  }
  if (token[0]=='$') ConvHexToDec();
  else if (token[0]=='\'') DoTickMarks();
  else token_nvalue=atoi(token);
}

void GetPunctuation()
{ char c;

  c=*src;
  switch (c)
  {
     case '(': token[0]='('; token[1]=0; src++; break;
     case ')': token[0]=')'; token[1]=0; src++; break;
     case '{': token[0]='{'; token[1]=0; src++; break;
     case '}': token[0]='}'; token[1]=0; src++; break;
     case '[': token[0]='['; token[1]=0; src++; break;
     case ']': token[0]=']'; token[1]=0; src++; break;
     case ',': token[0]=','; token[1]=0; src++; break;
     case ':': token[0]=':'; token[1]=0; src++; break;
     case ';': token[0]=';'; token[1]=0; src++; break;
     case '/': token[0]='/'; token[1]=0; src++; break;
     case '*': token[0]='*'; token[1]=0; src++; break;
     case '^': token[0]='^'; token[1]=0; src++; break;
     case '%': token[0]='%'; token[1]=0; src++; break;
     case '\"': token[0]='\"'; token[1]=0; src++; break;
     case '+' : token[0]='+';
                src++;
                if (*src=='+')
                { token[1]='+';
                  src++; }
                else if (*src=='=')
                { token[1]='=';
                  src++; }
                else token[1]=0;
                token[2]=0;
                break;
     case '-' : token[0]='-';
                src++;
                if (*src=='-')
                { token[1]='-';
                  src++; }
                else if (*src=='=')
                { token[1]='=';
                  src++; }
                else token[1]=0;
                token[2]=0;
                break;
     case '>' : token[0]='>';
                src++;
                if (*src=='=')
                { token[1]='=';
                  token[2]=0;
                  src++; break; }
                if (*src=='>')
                { token[1]='>';
                  token[2]=0;
                  src++; break; }
                token[1]=0;
                break;
      case '<': token[0]='<';
                src++;
                if (*src=='=')
                { token[1]='=';
                  token[2] = 0;
                  src++; break; }
                if (*src=='<')
                { token[1]='<';
                  token[2] = 0;
                  src++; break; }
                token[1]=0;
                break;
      case '!': token[0]='!';
                src++;
                if (*src=='=')
                { token[1]='=';
                  token[2]=0;
                  src++; break; }
                token[1]=0;
                break;
      case '=': token[0]='=';
                src++;
                if (*src=='=')
                { token[1]=0;
                  src++; }
                else token[1]=0;
                break;
      case '&': token[0]='&';
                src++;
                if (*src=='&')
                { token[1]='&';
                  token[2]=0;
                  src++; break; }
                token[1]=0;
                break;
      case '|': token[0]='|';
                src++;
                if (*src=='|')
                { token[1]='|';
                  token[2]=0;
                  src++; break; }
                token[1]=0;
                break;
       default: src++;                   // This should be an error.
  }
}

void GetString(void)
{
  int i;

  // Expects a "quoted" string. Places the contents of the string in
  // token[] but does not include the quotes.

  Expect("\"");
  i=0;
  while (*src!='\"')
  {
    token[i++]=*src++;
    if (i>250)
      vcerr("String exceeds 250 character maximum.");
  }
  src++;
  token[i]=0;
}

void GetToken(void)
{
  int i;

  // Simply reads in the next statement and places it in the
  // token buffer.

  ParseWhitespace();
  i=0;

  switch (chr_table[*src])
  {
    case  LETTER: { token_type = IDENTIFIER; GetIdentifier();  break; }
    case   DIGIT: { token_type = DIGIT;      GetNumber();      break; }
    case SPECIAL: { token_type = CONTROL;    GetPunctuation(); break; }
  }

  if (!*src && inevent)
    err("Unexpected end of file");
}

void Expect(char *str)
{
  GetToken();
  if (TokenIs(str)) return;
  vcerr("error: %s expected, %s got", str, token);
}

int ExpectNumber()
{
  GetToken();
  if (token_type!=DIGIT) err("error: Numerical value expected");
  return token_nvalue;
}

void InitCompileSystem()
{ int i;

  vprint("Building chr_table[].");
  for (i=0; i<256; i++) chr_table[i]=SPECIAL;
  for (i='0'; i<='9'; i++) chr_table[i]=DIGIT;
  for (i='A'; i<='Z'; i++) chr_table[i]=LETTER;
  for (i='a'; i<='z'; i++) chr_table[i]=LETTER;

  chr_table[10]=0;
  chr_table[13]=0;
  chr_table[' ']=0;
  chr_table['_']=LETTER;
  chr_table['.']=LETTER;
  chr_table['$']=DIGIT;
  chr_table[39]=DIGIT;
}

char lasttoken[2048];

int NextIs(char *str)
{ char *ptr,tt,tst;
  int i,nv;

  ptr=src;
  tt=token_type;
  tst=token_subtype;
  nv=token_nvalue;
  memcpy(lasttoken, token, 2048);
  GetToken();
  src=ptr;
  token_nvalue=nv;
  tst=token_subtype;
  tt=token_type;
  //if (!strcmp(str,token)) i=1; else i=0;
  i=streq(str,token);
  memcpy(token, lasttoken, 2048);
  return i;
}
