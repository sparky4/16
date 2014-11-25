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
// ³                       Code Generation module                        ³
// ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "funclib.h"
#include "vcc.h"
#include "vccode.h"
#include "lexical.h"
#include "preproc.h"

// ================================= Data ====================================

#define LETTER 1
#define SPECIAL 3

// -- Function arguements type defs

#define VOID 1
#define INT 1
#define CHARPTR 2
#define STRING 3

unsigned char *source, *src;
unsigned char *outbuf, *code;
unsigned char inevent=0;

typedef struct
{
  char fname[40];
  char argtype[20];
  int numargs, numlocals;
  int returntype;
  int syscodeofs;
} funcdecl;

funcdecl funcs[700];
int numfuncs=0;

typedef struct
{
  char vname[40];
  int varstartofs;
  int arraylen;
} vardecl;

vardecl vars[500];
int curstartofs=0;
int numvars=0;
int varidx;

typedef struct
{
  char vname[40];
  int vsofs;
  int arraylen;
} strdecl;

strdecl str[300];
int sstartofs=0;
int numstr=0;

int vctype;        // 0 / 1 :: map / system

// -- local function parameters --

char larg[20][40];

// -- MAP vc stuff --

char *functbl[512];
int mfuncs=0;

// ================================= Code ====================================

void HandleString();
void HandleFunction();
void HandleUserFunction();
void HandleFunctionType();
void EmitOperand();

void EmitC (char c)
{
  if (locate && locate == (int)(code - outbuf))
    vcerr("Located.");

  *code++ = c;
}

void EmitW (short w)
{
  short *c=(short *)code;

  if (locate && locate == (int)(code - outbuf))
    vcerr("Located.");

  *c=w; code+=2;
}

void EmitD (int d)
{
  long *c=(long *)code;

  if (locate && locate == (int)(code - outbuf))
    vcerr("Located.");

  *c=d; code+=4;
}

void EmitString(char *str)
  { while ((*code++ = *str++)) ; }

void HandleStringOperand()
{
  if (NextIs("\""))
  {
    EmitC(s_IMMEDIATE);
    GetString();
    EmitString(token);
    return;
  }
  GetToken();
  if (token_type==IDENTIFIER && varcategory==op_STRING)
  {
    EmitC(s_GLOBAL);
    EmitW(str[varidx].vsofs);
    return;
  }
  if (token_type==IDENTIFIER && varcategory==op_SARRAY)
  {
    EmitC(s_ARRAY);
    EmitW(str[varidx].vsofs);
    Expect("[");
    EmitOperand();
    Expect("]");
    return;
  }
  if (token_type==IDENTIFIER && varcategory==op_SLOCAL)
  {
    EmitC(s_LOCAL);
    EmitC((char) varidx);
    return;
  }
  if (TokenIs("str"))
  {
    EmitC(s_NUMSTR);
    Expect("(");
    EmitOperand();
    Expect(")");
    return;
  }
  if (TokenIs("left"))
  {
    EmitC(s_LEFT);
    Expect("(");
    HandleString();
    Expect(",");
    EmitOperand();
    Expect(")");
    return;
  }
  if (TokenIs("right"))
  {
    EmitC(s_RIGHT);
    Expect("(");
    HandleString();
    Expect(",");
    EmitOperand();
    Expect(")");
    return;
  }
  if (TokenIs("mid"))
  {
    EmitC(s_MID);
    Expect("(");
    HandleString();
    Expect(",");
    EmitOperand();
    Expect(",");
    EmitOperand();
    Expect(")");
    return;
  }
  if (TokenIs("chr"))
  {
    EmitC(s_CHR);
    Expect("(");
    EmitOperand();
    Expect(")");
    return;
  }
  else vcerr("Unknown string operand. ");
}

void HandleString()
{
  while (1)
  {
    HandleStringOperand();
    if (NextIs("+"))
    {
       EmitC(s_ADD);
       GetToken();
       continue;
    }
    else
    {
       EmitC(s_END);
       break;
    }
  }
}

void EmitOperand();
void DoSomething();

void HandleOperand()
{
  GetToken();
  if (token_type==DIGIT)
  {
     EmitC(op_IMMEDIATE);
     EmitD(token_nvalue);
     return;
  }
  if (token_type==IDENTIFIER)
  {
    if (varcategory==op_UVAR)
    {
      EmitC(op_UVAR);
      EmitD(vars[varidx].varstartofs);
      return;
    }
    if (varcategory==op_UVARRAY)
    {
      EmitC(op_UVARRAY);
      EmitD(vars[varidx].varstartofs);
      Expect("[");
      EmitOperand();
      Expect("]");
      return;
    }
    if (varcategory==op_LVAR)
    {
      EmitC(op_LVAR);
      EmitC((char) varidx);
      return;
    }
    if (varcategory==op_HVAR0)
    {
      EmitC(op_HVAR0);
      EmitC((char) varidx);
      return;
    }
    if (varcategory==op_HVAR1)
    {
      EmitC(op_HVAR1);
      EmitC((char) varidx);
      Expect("[");
      EmitOperand();
      Expect("]");
      return;
    }
  }
  if (token_type==FUNCTION && token_subtype==op_BFUNC)
  {
    if (!returntypes[funcidx])
    {
      vcerr("%s() does not return a vlue.", token);
    }
    EmitC(op_BFUNC);
    HandleFunction();
    return;
  }
  if (token_type==FUNCTION && token_subtype==op_UFUNC)
  {
    if (!funcs[funcidx].returntype)
    {
      vcerr("%s() does not return a vlue.", token);
    }
    EmitC(op_UFUNC);
    HandleUserFunction();
    return;
  }
  vcerr("Unknown token.");
}

void EmitOperand()
{
  while (1)                // Modifier-process loop.
  {
     if (NextIs("("))
     {
        EmitC(op_GROUP);
        GetToken();
        EmitOperand();
        Expect(")");
     }
     else HandleOperand();

     if (NextIs("+"))
     {
        EmitC(op_ADD);
        GetToken();
        continue;
     }
     else if (NextIs("-"))
     {
        EmitC(op_SUB);
        GetToken();
        continue;
     }
     else if (NextIs("/"))
     {
        EmitC(op_DIV);
        GetToken();
        continue;
     }
     else if (NextIs("*"))
     {
        EmitC(op_MULT);
        GetToken();
        continue;
     }
     else if (NextIs("%"))
     {
        EmitC(op_MOD);
        GetToken();
        continue;
     }
     else if (NextIs(">>"))
     {
        EmitC(op_SHR);
        GetToken();
        continue;
     }
     else if (NextIs("<<"))
     {
        EmitC(op_SHL);
        GetToken();
        continue;
     }
     else if (NextIs("&"))
     {
        EmitC(op_AND);
        GetToken();
        continue;
     }
     else if (NextIs("|"))
     {
        EmitC(op_OR);
        GetToken();
        continue;
     }
     else if (NextIs("^"))
     {
        EmitC(op_XOR);
        GetToken();
        continue;
     }
     else
     {
        EmitC(op_END);
        break;
     }
  }
}

void HandleFunction()
{
  switch (funcidx)
  {
    case 0: vcfunc_Exit(); break;
    case 1: Message(); break;
    case 2: GenericFunc(3,1); break;
    case 3: GenericFunc(4,1); break;
    case 4: GenericFunc(5,1); break;
    case 5: vc_loadimage(); break;
    case 6: GenericFunc(7,5); break;
    case 7: GenericFunc(8,5); break;
    case 8: GenericFunc(9,0); break;
    case 9: GenericFunc(10,0); break;
    case 10: vc_AllocateEntity(); break;
    case 11: GenericFunc(12,1); break;
    case 12: vc_Map(); break;
    case 13: vc_LoadFont(); break;
    case 14: vc_PlayFLI(); break;
    case 15: GenericFunc(16,2); break;
    case 16: vc_PrintString(); break;
    case 17: vc_LoadRaw(); break;
    case 18: GenericFunc(19,4); break;
    case 19: GenericFunc(20,1); break;
    case 20: GenericFunc(21,7); break;
    case 21: GenericFunc(22,0); break;
    case 22: GenericFunc(23,0); break;
    case 23: GenericFunc(24,1); break;
    case 24: vc_EntityMove(); break;
    case 25: GenericFunc(26,4); break;
    case 26: GenericFunc(27,4); break;
    case 27: GenericFunc(28,5); break;
    case 28: GenericFunc(29,4); break;
    case 29: GenericFunc(30,4); break;
    case 30: GenericFunc(31,5); break;
    case 31: GenericFunc(32,5); break;
    case 32: vc_strlen(); break;
    case 33: vc_strcmp(); break;
    case 34: GenericFunc(35,0); break;
    case 35: GenericFunc(36,1); break;
    case 36: GenericFunc(37,1); break;
    case 37: GenericFunc(38,1); break;
    case 38: GenericFunc(39,3); break;
    case 39: GenericFunc(40,2); break;
    case 40: GenericFunc(41,1); break;
    case 41: GenericFunc(42,1); break;
    case 42: GenericFunc(43,3); break;
    case 43: vc_HookRetrace(); break;
    case 44: vc_HookTimer(); break;
    case 45: GenericFunc(46,2); break;
    case 46: vc_SetRString(); break;
    case 47: GenericFunc(48,4); break;
    case 48: GenericFunc(49,3); break;
    case 49: GenericFunc(50,0); break;
    case 50: vc_PartyMove(); break;
    case 51: GenericFunc(52,1); break;
    case 52: GenericFunc(53,1); break;
    case 53: GenericFunc(54,1); break;
    case 54: GenericFunc(55,0); break;
    case 55: GenericFunc(56,1); break;
    case 56: GenericFunc(57,1); break;
    case 57: GenericFunc(58,5); break;
    case 58: GenericFunc(59,5); break;
    case 59: GenericFunc(60,2); break;
    case 60: vc_HookKey(); break;
    case 61: vc_PlayMusic(); break;
    case 62: GenericFunc(63,0); break;
    case 63: GenericFunc(64,5); break;
    case 64: vc_OpenFile(); break;
    case 65: GenericFunc(66,1); break;
    case 66: vc_QuickRead(); break;
    case 67: GenericFunc(68,1); break;
    case 68: GenericFunc(69,1); break;
    case 69: GenericFunc(70,0); break;
    case 70: GenericFunc(71,0); break;
    case 71: GenericFunc(72,7); break;
    case 72: GenericFunc(73,15); break;
    case 73: vc_CacheSound(); break;
    case 74: GenericFunc(75,0); break;
    case 75: GenericFunc(76,3); break;
    case 76: GenericFunc(77,7); break;
    case 77: GenericFunc(78,4); break;
    case 78: GenericFunc(79,4); break;
    case 79: vc_val(); break;
    case 80: GenericFunc(81,7); break;
    case 81: GenericFunc(82,5); break;
    case 82: vc_Log(); break;
    case 83: GenericFunc(84,2); break;
    case 84: GenericFunc(85,2); break;
    case 85: GenericFunc(86,3); break;
    case 86: GenericFunc(87,1); break;
    case 87: GenericFunc(88,1); break;
    case 88: GenericFunc(89,1); break;
    case 89: vc_fgetline(); break;
    case 90: vc_fgettoken(); break;
    case 91: vc_fwritestring(); break;
    case 92: GenericFunc(93, 3); break;
    case 93: vc_frename(); break;
    case 94: vc_fdelete(); break;
    case 95: vc_fwopen(); break;
    case 96: GenericFunc(97, 1); break;
    case 97: GenericFunc(98, 3); break;
    case 98: GenericFunc(99, 3); break;
    case 99: GenericFunc(100, 6); break;
    case 100: GenericFunc(101, 0); break;
    case 101: GenericFunc(102, 7); break;
    case 102: GenericFunc(103, 1); break;
    case 103: GenericFunc(104, 1); break;
    case 104: GenericFunc(105, 1); break;
    case 105: vc_asc(); break;
    case 106: GenericFunc(107, 1); break;
    case 107: vc_NumForScript(); break;
    case 108: vc_FileSize(); break;
    case 109: GenericFunc(110, 1); break;
    case 110: GenericFunc(111, 0); break;
    default: vcerr("Internal error. Unknown standard function.");
  }
}

void HandleUserFunction()
{
  int i, idx;

  idx=funcidx;
  EmitW((short) idx);
  Expect("(");
  for (i=0; i<funcs[idx].numargs; i++)
  {
    if (i) Expect(",");
    if (funcs[idx].argtype[i]==INT) EmitOperand();
    if (funcs[idx].argtype[i]==STRING) HandleString();
  }
  Expect(")");
}

void HandleFunctionType()
{
  if (token_subtype==op_BFUNC)
  {
    EmitC(opEXEC_STDLIB);
    HandleFunction();
  }
  else
  if (token_subtype==op_UFUNC)
  {
    EmitC(opEXEC_EXTERNFUNC);
    HandleUserFunction();
  }
}

void HandleIfComponent()
{ char ot=0;

  if (NextIs("!"))
  {
   ot=i_ZERO;
   GetToken();
  }
  EmitOperand();

  if (NextIs("="))  { ot=i_EQUALTO;            GetToken(); }
  if (NextIs("!=")) { ot=i_NOTEQUAL;           GetToken(); }
  if (NextIs(">"))  { ot=i_GREATERTHAN;        GetToken(); }
  if (NextIs(">=")) { ot=i_GREATERTHANOREQUAL; GetToken(); }
  if (NextIs("<"))  { ot=i_LESSTHAN;           GetToken(); }
  if (NextIs("<=")) { ot=i_LESSTHANOREQUAL;    GetToken(); }

  if (!ot) EmitC(i_NONZERO);
  else if (ot==i_ZERO) EmitC(i_ZERO);
  else
  {
    EmitC(ot);
    EmitOperand();
  }
}

void HandleIfGroup()
{
  while (1)
  {
    HandleIfComponent();

    if (NextIs("&&"))
    {
       EmitC(i_AND);
       GetToken();
       continue;
    }
    else if (NextIs("||"))
    {
       EmitC(i_OR);
       GetToken();
       continue;
    }
    else
    {
       GetToken();
       if (!TokenIs(")") && !TokenIs(";"))
         vcerr("Syntax error.");
       EmitC(i_UNGROUP);
       break;
    }
  }
}

void ProcessIf()
{
  char *falseofs = 0;
  char *elseofs = 0;
  char *b = 0;

  EmitC(opIF);
  Expect("(");
  HandleIfGroup();

  falseofs=code;
  EmitD(0);          // We'll come back to this and fix it up.

  if (!NextIs("{"))
  {
    DoSomething();
    if (NextIs("else"))
    {
      EmitC(opGOTO);
      elseofs=code;
      EmitD(0);
    }
  }
  else
  {
    Expect("{");
    while (!NextIs("}")) DoSomething();
    Expect("}");
    if (NextIs("else"))
    {
      EmitC(opGOTO);
      elseofs=code;
      EmitD(0);
    }
  }

  b=code;         // Put correct false-execution offset thingy.
  code=falseofs;
  EmitD((int) b - (int) outbuf);
  code=b;

  if (NextIs("else"))
  {
    GetToken();
    if (!NextIs("{"))
    {
      DoSomething();
    }
    else
    {
      Expect("{");
      while (!NextIs("}")) DoSomething();
      Expect("}");
    }
    b=code;         // Put correct else-execution offset thingy.
    code=elseofs;
    EmitD((int) b - (int) outbuf);
    code=b;
  }
}

void ProcessWhile()
{ char *falseofs, *top, *b;

  top=code;
  EmitC(opIF);
  Expect("(");
  HandleIfGroup();

  falseofs=code;
  EmitD(0);          // We'll come back to this and fix it up.

  if (!NextIs("{"))
  {
    DoSomething();
    EmitC(opGOTO);
    EmitD((int) top - (int) outbuf);
  }
  else
  {
    Expect("{");
    while (!NextIs("}")) DoSomething();
    Expect("}");
    EmitC(opGOTO);
    EmitD((int) top - (int) outbuf);
  }

  b=code;         // Put correct false-execution offset thingy.
  code=falseofs;
  EmitD((int) b - (int) outbuf);
  code=b;
}

void ProcessFor()
{ char *src1, *src2, *loopstartpos, *srctmp;

  Expect("(");
  while (!TokenIs(";"))
    DoSomething();    // Emit initialization code.

  src1=src;         // Store position of loop conditional
  while (!NextIs(";")) GetToken(); GetToken();
  src2=src;         // Store position of end-of-loop code

  while (!NextIs(")")) GetToken(); GetToken();
  loopstartpos=(char *) (int) code - (int) outbuf;

  if (!NextIs("{"))
  {
    DoSomething();
  }
  else
  {
    Expect("{");
    while (!NextIs("}")) DoSomething();
    Expect("}");
  }
  srctmp=src;
  src=src2;
  while (!TokenIs(")"))
    DoSomething();
  src=src1;
  EmitC(opIF);
  HandleIfGroup();
  EmitD((int) code - (int) outbuf +9);
  EmitC(opGOTO);
  EmitD((int) loopstartpos);

  src=srctmp;
}

void HandleAssign()
{
  int vc;

  vc=varcategory;
  EmitC(opASSIGN);
  if (vc==op_UVAR)
  {
    EmitC(op_UVAR);
    EmitD(vars[varidx].varstartofs);
  }
  else if (vc==op_UVARRAY)
  {
    EmitC(op_UVARRAY);
    EmitD(vars[varidx].varstartofs);
    Expect("[");
    EmitOperand();
    Expect("]");
  }
  else if (vc==op_LVAR)
  {
    EmitC(op_LVAR);
    EmitC((char) varidx);
  }
  else if (vc==op_HVAR0)
  {
    EmitC(op_HVAR0);
    EmitC((char) varidx);
  }
  else if (vc==op_HVAR1)
  {
    EmitC(op_HVAR1);
    EmitC((char) varidx);
    Expect("[");
    EmitOperand();
    Expect("]");
  }
  else if (vc==op_STRING)
  {
    EmitC(op_STRING);
    EmitW((short) str[varidx].vsofs);
  }
  else if (vc==op_SARRAY)
  {
    EmitC(op_SARRAY);
    EmitW((short) str[varidx].vsofs);
    Expect("[");
    EmitOperand();
    Expect("]");
  }
  else if (vc==op_SLOCAL)
  {
    EmitC(op_SLOCAL);
    EmitW((short) varidx);
  }
  GetToken();
  if (TokenIs("++")) { EmitC(a_INC); GetToken(); return; } else
  if (TokenIs("--")) { EmitC(a_DEC); GetToken(); return; } else
  if (TokenIs("+=")) { EmitC(a_INCSET); } else
  if (TokenIs("-=")) { EmitC(a_DECSET); } else
  if (TokenIs("="))  { EmitC(a_SET); } else
  vcerr("Invalid assignment operator.");
  if (vc==op_STRING) HandleString();
  else if (vc==op_SARRAY) HandleString();
  else if (vc==op_SLOCAL) HandleString();
  else EmitOperand();
  GetToken();
}

int c=0;

void HandleReturn()
{
  if (!vctype)
  {
    Expect(";");
    EmitC(opRETURN);
    return;
  }
  if (!funcs[c].returntype)
  {
    Expect(";");
    EmitC(opRETURN);
    return;
  }
  if (funcs[c].returntype==1)
  {
    EmitC(opSETRETVAL);
    EmitOperand();
    Expect(";");
    EmitC(opRETURN);
    return;
  }
}

void ProcessSwitch()
{ char *buf,*retrptr;

  EmitC(opSWITCH);
  Expect("(");
  EmitOperand();
  Expect(")");
  Expect("{");

  // case .. option loop

  while (!NextIs("}"))
  {
    Expect("case");
    EmitC(opCASE);
    EmitOperand();
    Expect(":");
    retrptr=code;
    EmitD(0);
    while (!NextIs("case") && !NextIs("}")) DoSomething();
    EmitC(opRETURN);
    buf=code;
    code=retrptr;
    EmitD((int) buf - (int) outbuf);
    code=buf;
  }
  Expect("}");
  EmitC(opRETURN);
}

void DoSomething(void)
{
  GetToken();
  if (TokenIs("return"))      { HandleReturn(); return; }
  if (token_type==FUNCTION)   { HandleFunctionType(); Expect(";"); return; }
  if (token_type==IDENTIFIER) { HandleAssign(); return; }
  if (TokenIs("if"))          { ProcessIf(); return; }
  if (TokenIs("while"))       { ProcessWhile(); return; }
  if (TokenIs("for"))         { ProcessFor(); return; }
  if (TokenIs("switch"))      { ProcessSwitch(); return; }
  vcerr("Unknown token.");
}

void DumpSystemIdx()
{ FILE *f;

  vprint("Dumping system.idx index file.");
  f=fopen("system.idx","wb");

  fwrite(&numvars, 1, 4, f);
  fwrite(&vars, sizeof vars / 500, numvars, f);
  fwrite(&numfuncs, 1, 4, f);
  fwrite(&funcs, sizeof funcs / 700, numfuncs, f);
  fwrite(&numstr, 1, 4, f);
  fwrite(&str, sizeof str / 300, numstr, f);
  fclose(f);
}

void ReadSystemIdx()
{ FILE *f;

  if (!(f=fopen("system.idx","rb")))
    err("Could not access system.idx.");

  fread(&numvars, 1, 4, f);
  fread(&vars, sizeof vars / 500, numvars, f);
  fread(&numfuncs, 1, 4, f);
  fread(&funcs, sizeof funcs / 700, numfuncs, f);
  fread(&numstr, 1, 4, f);
  fread(&str, sizeof str / 300, numstr, f);
  fclose(f);
}

void DoLocalVariables(int c)
{
  int na;

  na=funcs[c].numargs;
  while (NextIs("int") || NextIs("string"))
  {
    GetToken();
    if (TokenIs("int"))
    {
      funcs[c].argtype[na]=INT;
      GetToken();
      memcpy(larg[na++], token, 40);
      while (!NextIs(";"))
      {
        Expect(",");
        funcs[c].argtype[na]=INT;
        GetToken();
        memcpy(larg[na++], token, 40);
      }
      Expect(";");
    }
    if (TokenIs("string"))
    {
      funcs[c].argtype[na]=STRING;
      GetToken();
      memcpy(larg[na++], token, 40);
      while (!NextIs(";"))
      {
        Expect(",");
        funcs[c].argtype[na]=STRING;
        GetToken();
        memcpy(larg[na++], token, 40);
      }
      Expect(";");
    }
  }
  funcs[c].numlocals=na;
}

void CompileMAP(char *fname)
{ FILE *f;
  int i;

  // Compiles a map-based VC.
  i=strlen(fname);
  memcpy(strbuf, fname, i);
  strbuf[i]='.';
  strbuf[i+1]='V';
  strbuf[i+2]='C';
  strbuf[i+3]=0;

  PreProcess(strbuf);

  source=(char *) malloc(1000000);
  memset(source, 0, 1000000);
  outbuf=(char *) malloc(1000000);
  memset(outbuf, 0, 1000000);
  if (!(f=fopen("vcctemp.$$$","rb")))
    err("Could not open source file.");
  fread(source, 1, 1000000, f);
  fclose(f);

  ReadSystemIdx();

  src=source;
  code=outbuf;
  vctype=0;

  while (*src)
  {
    functbl[mfuncs]=(char *) (int) code - (int) outbuf;
    mfuncs++;
    Expect("event");
    Expect("{");
    while (!NextIs("}")) DoSomething();
    Expect("}");
    EmitC(opRETURN);
    ParseWhitespace();
  }
  dprint("%s.vc (%i lines)", fname, lines);
}

void SkipBrackets()
{
  while (!NextIs("}"))
  {
    if (!*src)
      err("No matching bracket.");
    GetToken();
    if (TokenIs("{")) SkipBrackets();
  }
  GetToken();
}

void CheckDup()
{
  int i=0; // bitch!

  while (i<numhardfuncs)
  {
    if (!strcmp(hardfuncs[i], token)) break;
    i++;
  }
  if (i<numhardfuncs)
  {
    vcerr("%s: Duplicate identifier.", token);
  }
  i=0;
  while (i<numhardvar0)
  {
    if (!strcmp(hardvar0[i], token)) break;
    i++;
  }
  if (i<numhardvar0)
  {
    vcerr("%s: Duplicate identifier.", token);
  }
  i=0;
  while (i<numhardvar1)
  {
    if (!strcmp(hardvar1[i], token)) break;
    i++;
  }
  if (i<numhardvar1)
  {
    vcerr("%s: Duplicate identifier.", token);
  }
  i=0;
  while (i<numfuncs)
  {
    if (!strcmp(funcs[i].fname, token)) break;
    i++;
  }
  if (i<numfuncs)
  {
    vcerr("%s: Duplicate identifier.", token);
  }

  i=0;
  while (i<numvars)
  {
    if (!strcmp(vars[i].vname, token)) break;
    i++;
  }
  if (i<numvars)
  {
    vcerr("%s: Duplicate identifier.", token);
  }
  i=0;
  while (i<numstr)
  {
    if (!strcmp(str[i].vname, token)) break;
    i++;
  }
  if (i<numstr)
  {
    vcerr("%s: Duplicate identifier.", token);
  }
}

void CompileSystem()
{
  FILE *f = 0;
  int na = 0;
  int type = 0;
  int i = 0;

  // Compiles a map-based VC.

  PreProcess("system.vc");

  source = (char *) malloc(1000000);
  memset(source, 0, 1000000);
  outbuf = (char *) malloc(1000000);
  memset(outbuf, 0, 1000000);

  if (!(f=fopen("vcctemp.$$$","rb")))
    err("Could not open source file.");

  fread(source, 1, 1000000, f);

  fclose(f);

  src=source;
  code=outbuf;

  vprint("First pass...");
  vctype=1; tlines=0;
  while (*src)
  {
    // system.vc is compiled in a two-pass system. The first pass simply
    // goes through the system.vc file and sets up declarations for all
    // global variables and function declarations. So system.vc won't
    // have to worry about forward declarations or anything.

    na=(int) src;
    GetToken();
    if (TokenIs("string"))
    {
      while (1)
      {
        GetToken();
        CheckDup();
        memcpy(str[numstr].vname, token, strlen(token));
        str[numstr].vsofs=sstartofs;
        if (NextIs("["))
        {
          GetToken();
          GetToken();
          str[numstr].arraylen=token_nvalue;
          Expect("]");
        }
        else str[numstr].arraylen=1;
        sstartofs+=str[numstr].arraylen;
        vprint("Decl %s of type string, size %i. [%i]",
          str[numstr].vname, str[numstr].arraylen, str[numstr].vsofs);
        numstr++;
        if (!NextIs(",")) break;
        GetToken();
      }
      Expect(";");
    }
    if (TokenIs("int"))
    {
      GetToken();
      if (NextIs("(")) type=0; else type=1;
      src=(char *) na;
      GetToken();
    }
    if (TokenIs("void") || (TokenIs("int") && !type))
    {
      if (TokenIs("void")) funcs[numfuncs].returntype=0;
      if (TokenIs("int")) funcs[numfuncs].returntype=1;
      GetToken();
      CheckDup();
      memcpy(funcs[numfuncs].fname,token,strlen(token));
      funcs[numfuncs].numargs=0;
      Expect("(");
      while (!NextIs(")"))
      {
        GetToken();
        na=funcs[numfuncs].numargs;
        if (TokenIs("int")) { funcs[numfuncs].argtype[na]=INT; } else
        if (TokenIs("string")) { funcs[numfuncs].argtype[na]=STRING; }
        else vcerr("Invalid arguement declaration.");
        GetToken();
        if (NextIs(",")) GetToken();
        funcs[numfuncs].numargs++;
      }
      Expect(")");
      Expect("{");
      SkipBrackets();
      vprint("Found %s declaration for %s, %i parameters.",
        funcs[numfuncs].returntype ? "int" : "void",
        funcs[numfuncs].fname, funcs[numfuncs].numargs);
      numfuncs++;
    }
    if (TokenIs("int") && type)
    {
      while (1)
      {
        GetToken();
        CheckDup();
        memcpy(vars[numvars].vname, token, strlen(token));
        vars[numvars].varstartofs=curstartofs;
        if (NextIs("["))
        {
          GetToken();
          GetToken();
          vars[numvars].arraylen=token_nvalue;
          Expect("]");
        }
        else vars[numvars].arraylen=1;
        curstartofs+=vars[numvars].arraylen;
        vprint("Decl %s of type int, size %i. [%i]", vars[numvars].vname,
          vars[numvars].arraylen, vars[numvars].varstartofs);
        numvars++;
        if (!NextIs(",")) break;
        GetToken();
      }
      Expect(";");
    }
  }
  dprint("system.vc: %i ints, %i strings, %i functions",
    numvars, numstr, numfuncs);

  vprint("Second pass...");
  src=source; tlines=0;
  while (*src)
  {
    // Everything in system.vc will either be a global var decl or
    // a function definition.

    na=(int) src;
    GetToken();
    if (TokenIs("string"))
    {
      while (1)
      {
        GetToken();
        if (NextIs("["))
        {
          GetToken();
          GetToken();
          Expect("]");
        }
        if (!NextIs(",")) break;
        GetToken();
      }
      Expect(";");
    }
   if (TokenIs("int"))
    {
      GetToken();
      if (NextIs("(")) type=0; else type=1;
      src=(char *) na;
      GetToken();
    }
    if (TokenIs("void") || (TokenIs("int") && !type))
    {
      funcs[c].syscodeofs=(int) code - (int) outbuf;
      GetToken();
      Expect("(");
      i=0;
      memset(&larg, 0, 480);
      while (!NextIs(")"))
      {
        GetToken();
        GetToken();
        memcpy(larg[i], token, 40);
        if (NextIs(",")) GetToken();
        i++;
      }
      Expect(")");
      Expect("{");
      DoLocalVariables(c);
      while (!NextIs("}")) DoSomething();
      EmitC(opRETURN);
      Expect("}");
      c++;
    }
    if (TokenIs("int") && type)
    {
      while (1)
      {
        GetToken();
        if (NextIs("["))
        {
          GetToken();
          GetToken();
          Expect("]");
        }
        if (!NextIs(",")) break;
        GetToken();
      }
      Expect(";");
    }
  }
  dprint("system.vc (%i lines, %i total)", lines, tlines);

  DumpSystemIdx();
}
