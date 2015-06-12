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

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#define VC_H
#include <math.h>
#include "verge.h"
#include "vccode.h"
#include "sincos.h"
#include "mikmod.h"

#define USERFUNC_MARKER 10000

// ================================= Data ====================================

char *sysvc, *mapvc, *basevc;         // VC pool ptrs
char *code;                           // VC current instruction pointer (IP)

int *globalint;                       // system.vc global int variables
int maxint;                           // maximum allocated # of ints
char *stringbuf;                      // vc string workspace
int vcreturn;                         // return value of last function
char *movescriptbuf;                  // VC EntityMove buffer
char vctrack=0;                       // VC call tracking to verge.log

quad *vcstack;                        // VC stack (seperate from main stack)
quad *vcsp;                           // VC stack pointer [esp]

int mapevents;                        // number of map events in this VC
char *mapvctbl[1024];                 // map VC offset table

int hookretrace=0;
int hooktimer=0;
int invc=0;

// -----------------

typedef struct
{
  char fname[40];
  char argtype[20];
  int numargs, numlocals;
  int returntype;
  int syscodeofs;
} funcdecl;

funcdecl *funcs;
int numfuncs;

typedef struct
{
  char vname[40];
  int vsofs;
  int arraylen;
} strdecl;

strdecl *str;
int numstr=0;
int stralloc;

typedef struct
{
  char vname[40];
  int varstartofs;
  int arraylen;
} vardecl;

vardecl *vars;
int numvars;
char kill=0;

// -- local func vars --

typedef struct
{
  int nargs[20];
  char s[2560];
} lvars;

//lvars *lvar;
lvars lvar;

// ============================== Prototypes =================================

void ResolveString(char *buffer);
void ExecuteSection();
void ExecuteEvent(int i);
void ExecuteUserFunc(int i);

// ================================= Code ====================================

int ProcessOperand();                 // Mutually dependant functions suck.
int ProcessIfOperand();               // Hell yeah they do, bitch.
void HandleExternFunc();
void HandleStdLib();
void ExecuteBlock();

void LoadSystemVC()
{
  VFILE *f=0;
  int i=0;

  Log("Initializing VC interpreter");
  if (!(f=vopen("system.idx"))) err("Could not open system.idx.");
  vread(&numvars, 4, f);
  vars=(vardecl *) valloc(numvars*sizeof(vardecl), "LoadSystemVC:vars", OID_VC);
  vread(vars, numvars*48, f);
  vread(&numfuncs, 4, f);
  funcs=(funcdecl *) valloc(numfuncs*sizeof(funcdecl), "LoadSystemVC:funcs", OID_VC);
  vread(funcs, numfuncs*76, f);
  vread(&numstr, 4, f);
  str=(strdecl *) valloc(numstr*sizeof(strdecl), "LoadSystemVC:str", OID_VC);
  vread(str, numstr*44, f);
  vclose(f);

  if (!(f=vopen("system.vcs"))) err("Could not open system.vcs");
  i=filesize(f);
  sysvc=(char *) valloc(i, "LoadSystemVC:sysvc", OID_VC);
  vread(&numfuncs, 4, f);
  vread(&maxint, 4, f);
  vread(&stralloc, 4, f);

  globalint=(int *) valloc(maxint ? maxint*4 : 4, "globalint", OID_VC);
  if (stralloc)
  {
    stringbuf=(char *) valloc((stralloc*256),
      "LoadSystemVC:stringbuf", OID_VC);
  }
  else
  {
    stringbuf=(char *) valloc(256,
      "LoadSystemVC:stringbuf (256)", OID_VC);
  }
  vread(sysvc, i, f);
  vclose(f);

  // Initialize VC stack
  vcstack=(quad *) valloc(6000, "vcstack", OID_VC);
  vcsp=vcstack;

  movescriptbuf=(char *) valloc(65535, "movescriptbuf", OID_VC);

Log("system vclib init: %d funcs, %d ints (%d bytes), %d strings (%d bytes)",
  numfuncs, numvars, maxint*4, numstr, stralloc*256);
}

void LoadMapVC(VFILE *f)
{ int codesize=0;

  vread(&mapevents, 4, f);
  vread(mapvctbl, 4*mapevents, f);
  vread(&codesize, 4, f);
  mapvc=(char *) valloc(codesize, "mapvc", OID_VC);
  vread(mapvc, codesize, f);
}

byte GrabC()
{
  return *code++;
}

word GrabW(void)
{
  word *ptr=(word *) code;
  code+=2;
  return *ptr;
}

quad GrabD(void)
{
  quad *ptr=(quad *) code;
  code+=4;
  return *ptr;
}

void GrabString(char *str)
{ int i=0;

  i=0;
  while (*code)
  {
    str[i]=*code;
    code++;
    i++;
  }
  str[i]=0;
  code++;
}

int ReadInt(char category, int loc, int ofs)
{
  switch (category)
  {
    case op_UVAR:    return globalint[loc];
    case op_UVARRAY: return globalint[loc];
    case op_HVAR0:   switch (loc)
                     {
                       case 0: return xwin;
                       case 1: return ywin;
                       case 2: return cameratracking;
                       case 3: return timer_count;
                       case 4: return up;
                       case 5: return down;
                       case 6: return left;
                       case 7: return right;
                       case 8: return b1;
                       case 9: return b2;
                       case 10: return b3;
                       case 11: return b4;
                       case 12: return sx;
                       case 13: return sy;
                       case 14: return playernum;
                       case 15: return cc;
                       case 16: return tracker;
                       case 17: return mx;
                       case 18: return my;
                       case 19: return mb;
                       case 20: return vctrack;
                       case 21: return width;
                       case 22: return depth;
                       case 23: return mp_volume;
                       case 24: return (int)vsp;
                       case 25: return lastent;
                       case 26: return last_pressed;
                     }
    case op_HVAR1:   switch (loc)
                     {
                       case 0: return (int) screen[ofs];
                       case 1: return entity[ofs].x;
                       case 2: return entity[ofs].y;
                       case 3: return entity[ofs].tx;
                       case 4: return entity[ofs].ty;
                       case 5: return entity[ofs].facing;
                       case 6: return entity[ofs].moving;
                       case 7: return entity[ofs].specframe;
                       case 8: return entity[ofs].speed;
//                       case 9: return entity[ofs].movecode;
                       case 10: return entidx[ofs];
                       case 11: return key[ofs];
                       case 12: return layer[ofs].hline;
                       case 13: return (int) (*(byte *)ofs);
                       case 14: return (int) (*(word *)ofs);
                       case 15: return (int) (*(quad *)ofs);
                       case 16: return (int) pal[ofs];
                       case 17: return (int) (*(char *)ofs);
                       case 18: return (int) (*(short*)ofs);
                       case 19: return (int) (*(int  *)ofs);
                     }
     case op_LVAR:   return lvar.nargs[loc];
    default:
      err("VC Execution error: Invalid ReadInt category %d", (int) category);
  }
  return 0;
}

void WriteInt(char category, int loc, int ofs, int value)
{
  switch (category)
  {
    case op_UVAR:    globalint[loc]=value; break;
    case op_UVARRAY: globalint[loc]=value; break;
    case op_HVAR0:   switch (loc)
                     {
                       case 0: xwin=value; return;
                       case 1: ywin=value; return;
                       case 2: cameratracking=value; return;
                       case 3: timer_count=value; return;
                       case 16: tracker=value; return;
                       case 20: vctrack=value; return;
                       case 23: mp_volume=value; return;
                       case 26: last_pressed=value; return;
                     }
    case op_HVAR1:   switch (loc)
                     {
                       case 0: screen[ofs]=(byte) value; return;
                       case 1: entity[ofs].x=value; return;
                       case 2: entity[ofs].y=value; return;
                       case 3: entity[ofs].tx=value; return;
                       case 4: entity[ofs].ty=value; return;
                       case 5: entity[ofs].facing=value; return;
                       case 6: entity[ofs].moving=value; return;
                       case 7: entity[ofs].specframe=value; return;
                       case 8: entity[ofs].speed=value; return;
//                       case 9: entity[ofs].movecode=value; return;
//                       case 10:
                       case 11: key[ofs]=value; return;
                       case 12: layer[ofs].hline=value; return;
                       case 13: (*(byte *)ofs)=(byte) value; return;
                       case 14: (*(word *)ofs)=(word) value; return;
                       case 15: (*(quad *)ofs)=(quad) value; return;
                       case 16: pal[ofs]=value; return;
                       case 17: (*(char *)ofs)=(byte) value; return;
                       case 18: (*(short*)ofs)=(word) value; return;
                       case 19: (*(int  *)ofs)=(quad) value; return;
                     }
    case op_LVAR:   lvar.nargs[loc]=value; return;
    default:
      err("VC Execution error: Invalid WriteInt category %d", (int) category);
  }
}

int ResolveOperand()
{
  int cr=0;
  int d=0;
  byte c=0;

  cr=ProcessOperand();                 // Get base number
  while (1)
  {
     c=GrabC();
     switch (c)
     {
        case op_ADD: cr += ProcessOperand(); continue;
        case op_SUB: cr -= ProcessOperand(); continue;
        case op_DIV: d=ProcessOperand();
                     if (!d) cr=0; else cr /= d; continue;
        case op_MULT: cr = cr * ProcessOperand(); continue;
        case op_MOD: d=ProcessOperand();
                     if (!d) cr=0; else cr %= d; continue;
        case op_SHL: cr = cr << ProcessOperand(); continue;
        case op_SHR: cr = cr >> ProcessOperand(); continue;
        case op_AND: cr = cr & ProcessOperand(); continue;
        case op_OR:  cr = cr | ProcessOperand(); continue;
        case op_XOR: cr = cr ^ ProcessOperand(); continue;
        case op_END: break;
     }
     break;
  }
  return cr;
}

int ProcessOperand()
{ byte op_desc=0;
  byte c=0;
  quad d=0;
  quad ofs=0;

  op_desc=GrabC();
  switch (op_desc)
  {
     case op_IMMEDIATE: return GrabD();
     case op_HVAR0: c=GrabC(); return ReadInt(op_HVAR0, c, 0);
     case op_HVAR1: c=GrabC(); ofs=ResolveOperand(); return ReadInt(op_HVAR1, c, ofs);
     case op_UVAR:  d=GrabD(); return ReadInt(op_UVAR, d, 0);
     case op_UVARRAY: d=GrabD(); d+=ResolveOperand(); return ReadInt(op_UVARRAY, d, 0);
     case op_LVAR: c=GrabC(); return lvar.nargs[c];
     case op_BFUNC: HandleStdLib();
                    return vcreturn;
     case op_UFUNC: HandleExternFunc();
                    return vcreturn;
     case op_GROUP: return ResolveOperand();
     default: sprintf(strbuf,"VC Execution error: Invalid operand %d.",op_desc);
              err(strbuf); break;
  }
  return 0;
}

void HandleStringOperand(char *buffer)
{
  char tbuf[256];
  char *d=0;
  word w=0;
  int c=0;
  int i=0;
  int j=0;
  int z=0;

  c=GrabC();
  switch (c)
  {
    case s_IMMEDIATE: GrabString(buffer); break;
    case s_GLOBAL: w=GrabW();
                   d=(char *) (int) stringbuf + (w*256);
                   z=strlen(d);
                   if (strlen(buffer)+z+1>250)
                     err("Combined string length exceeds 250 char max.");
                   memcpy(buffer, d, z+1);
                   break;
    case s_ARRAY:  w=GrabW();
                   w+=ResolveOperand();
                   d=(char *) (int) stringbuf + (w*256);
                   z=strlen(d);
                   if (strlen(buffer)+z+1>250)
                     err("Combined string length exceeds 250 char max.");
                   memcpy(buffer, d, z+1);
                   break;
    case s_NUMSTR: i=ResolveOperand();
                   sprintf(buffer,"%d",i);
                   break;
    case s_LEFT:   ResolveString(tbuf);
                   i=ResolveOperand();
                   c=i<strlen(tbuf) ? i : strlen(tbuf);
                   if (strlen(buffer)+c+1>250)
                     err("Combined string length exceeds 250 char max.");
                   memcpy(buffer, tbuf, c);
                   buffer[c]=0;
                   break;
    case s_RIGHT:  ResolveString(tbuf);
                   i=ResolveOperand();
                   c=i<strlen(tbuf) ? i : strlen(tbuf);
                   if (strlen(buffer)+c+1>250)
                     err("Combined string length exceeds 250 char max.");
                   memcpy(buffer, tbuf+strlen(tbuf)-c, c);
                   buffer[c]=0;
                   break;
    case s_MID:    ResolveString(tbuf);
                   i=ResolveOperand();
                   j=ResolveOperand();
                   i=i<strlen(tbuf) ? i : strlen(tbuf);
                   j=j<strlen(tbuf)-i ? j : strlen(tbuf)-i;
                   if (strlen(buffer)+j+1>250)
                     err("Combined string length exceeds 250 char max.");
                   memcpy(buffer, tbuf+i, j);
                   buffer[j]=0;
                   break;
    case s_CHR:    if (strlen(buffer)+2>50)
                     err("Combined string length exceeds 250 char max.");
                   buffer[0]=(char) ResolveOperand();
                   buffer[1]=0;
                   break;
    case s_LOCAL:  c=GrabC();
                   d=&lvar.s[c*256];
                   z=strlen(d);
                   if (strlen(buffer)+z+1>250)
                     err("Combined string length exceeds 250 char max.");
                   memcpy(buffer, d, z+1);
                   break;
    default: err("Invalid VC string operand %d", (int) c);
  }
}

void ResolveString(char *buffer)
{ char c=0;

  HandleStringOperand(buffer);
  while (1)
  {
    c=GrabC();
    switch (c)
    {
      case s_ADD:
        HandleStringOperand((char *) (int) buffer + strlen(buffer)); break;
      case s_END: return;
      default:
        err("VC execution error: Unknown string operator %d",(int) c);
    }
  }
}

void vcpush(quad info)
{
  if (vcsp>=vcstack+1500) err("VC stack overflow.");
  *vcsp=info;
  vcsp++;
}

quad vcpop()
{
  if (vcsp==vcstack) err("VC stack underflow.");
  vcsp--;
  return *vcsp;
}

void  ReadVCVar()
{ int i=0;
  int j=0;

  strlwr((char *) args[1]);
  for (i=0; i<=numvars; i++)
    if (!strcmp(vars[i].vname, (char *) args[1])) break;
  if (i<numvars)
  {
    j=vars[i].varstartofs;
    if (vars[i].arraylen>1)
      j+=atoi((char *) args[2]);
    j=globalint[j];
    sprintf(strbuf,"%s:%d",vars[i].vname, j);
    Con_Printf(strbuf);
    return;
  }
  for (i=0; i<=numstr; i++)
    if (!strcmp(str[i].vname, (char *) args[1])) break;
  if (i<numstr)
  {
    j=(int) stringbuf + (i*256);
    sprintf(strbuf,"%s:%s",str[i].vname, (char *) j);
    Con_Printf(strbuf);
    return;
  }
  Con_Printf("No such VC variable.");
}

void WriteVCVar()
{ int i=0;
  int j=0;

  strlwr((char *) args[1]);
  for (i=0; i<=numvars; i++)
    if (!strcmp(vars[i].vname, (char *) args[1])) break;
  if (i<numvars)
  {
    j=vars[i].varstartofs;
    if (vars[i].arraylen>1)
    {
      j+=atoi((char *) args[2]);
      globalint[j]=atoi((char *) args[3]);
    }
    else globalint[j]=atoi((char *) args[2]);
    sprintf(strbuf,"%s:%d", vars[i].vname, atoi((char *) args[2]));
    Con_Printf(strbuf);
    return;
  }
  for (i=0; i<=numstr; i++)
    if (!strcmp(str[i].vname, (char *) args[1])) break;
  if (i<numstr)
  {
    j=(int) stringbuf + (i*256);
    memcpy((char *) j, (char *) args[2], strlen((char *) args[2])+1);
    sprintf(strbuf,"%s:%s", str[i].vname, (char *) args[2]);
    Con_Printf(strbuf);
    return;
  }
  Con_Printf("No such VC variable.");
}

// ======================= VC Standard Function Library =======================

int ClipOn=1;
int LucentOn=0;

void vc_Exit_()
{
  ResolveString(strbuf);
  err(strbuf);
}

void vc_Message()
{
  int i=0;
  char str1[100];

  ResolveString(str1);
  i=ResolveOperand();
  Message(str1, i);
}

void vc_Malloc()
{
  int i=0;

  i=ResolveOperand();
  vcreturn=(int) valloc(i, "vcreturn", OID_TEMP);

  Log("VC allocating %u bytes, ptr at %u.", i, vcreturn);

  if (!vcreturn) Message("Warning: VC failed malloc", 750);
}

void vc_Free()
{
  int i=0;

  i=ResolveOperand();
  vfree((char *) i);

  Log("VC freeing allocated heap at %u.", i);
}

void vc_pow()
{
  int i=0;
  int j=0;

  i=ResolveOperand();
  j=ResolveOperand();
  vcreturn=pow(i,j);
}

void vc_loadimage()
{
  byte *t=0;
  char s[256];

  ResolveString(s);
  t=VLoadImageBuf(s);
  vcreturn=(int) t;
}

void vc_copysprite()
{
  int a=0;
  int b=0;
  int c=0;
  int d=0;
  byte *e=0;

  a=ResolveOperand();
  b=ResolveOperand();
  c=ResolveOperand();
  d=ResolveOperand();
  e=(byte *) ResolveOperand();
  if (!ClipOn && !LucentOn) CopySprite(a,b,c,d,e);
  if ( ClipOn && !LucentOn) CopySpriteClip(a,b,c,d,e);
  if (!ClipOn &&  LucentOn) CopySpriteLucent(a,b,c,d,e);
  if ( ClipOn &&  LucentOn) CopySpriteLucentClip(a,b,c,d,e);
}

void vc_tcopysprite()
{
  int a=0;
  int b=0;
  int c=0;
  int d=0;
  byte *e=0;

  a=ResolveOperand();
  b=ResolveOperand();
  c=ResolveOperand();
  d=ResolveOperand();
  e=(byte *) ResolveOperand();
  if (!ClipOn && !LucentOn) TCopySprite(a,b,c,d,e);
  if ( ClipOn && !LucentOn) TCopySpriteClip(a,b,c,d,e);
  if (!ClipOn &&  LucentOn) TCopySpriteLucent(a,b,c,d,e);
  if ( ClipOn &&  LucentOn) TCopySpriteLucentClip(a,b,c,d,e);
}

void vc_EntitySpawn()
{
  int a=0;
  int b=0;
  char chrname[80];

  a=ResolveOperand();
  b=ResolveOperand();
  ResolveString(chrname);
  vcreturn=AllocateEntity(a, b, chrname);
}

void vc_SetPlayer()
{
  int i=0;

  i=ResolveOperand();
  if (i>=entities) err("SetPlayer(): Entity index out of range");

  player=&entity[i];
  playernum=i;

//  entity[i].movecode=0;
  entity[i].moving=0;
}

void vc_Map()
{
  hookretrace=0;
  hooktimer=0;
  kill=1;
  ResolveString(startmap);
}

void vc_LoadFont()
{
  char vcstr[80];

  ResolveString(vcstr);
  vcreturn=LoadFont(vcstr);
}

void vc_PlayFLI()
{
  char s[256];
  BITMAP flibuf;
  byte *data=0;
  VFILE *f=0;
  int i=0;

  ResolveString(s);
  flibuf.w=sx;
  flibuf.h=sy;
  flibuf.data=screen;
  if (!(f=vopen(s))) err("PlayFLI: Could not open %s.",s);
  i=filesize(f);
  data=(byte *) malloc(i);
  if (!data) err("Not enough memory to play FLI.");
  vread(data, i, f);
  vclose(f);
  play_memory_fli(data, &flibuf, 0, ShowPage);
  timer_count=0;
  set_intensity(63);
}

void vc_PrintString()
{
  char s[256];
  int i=0;

  i=ResolveOperand();
  ResolveString(s);
  printstring(i,s);
}

void vc_LoadRaw()
{
  char s[256];
  char *t=0;
  VFILE *f=0;
  int i=0;

  ResolveString(s);
  if (!(f=vopen(s))) err("LoadRaw(): Could not open file %s",s);
  i=filesize(f);
  t=(char *) valloc(i, "LoadRaw:t", OID_VC);
  vread(t, i, f);
  vclose(f);
  vcreturn=(int) t;
}

void vc_SetTile()
{
  int x=0;
  int y=0;
  int layr=0;
  int value=0;

  x=ResolveOperand();
  y=ResolveOperand();
  layr=ResolveOperand();
  value=ResolveOperand();

  switch (layr)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5: layers[layr][(y*layer[layr].sizex)+x] = (short) value; break;
    case 6: obstruct[(y*layer[0].sizex)+x] = (byte) value; break;
    case 7: zone[(y*layer[0].sizex)+x] = (byte) value; break;
    default: err("SetTile(): Invalid layer value");
  }
}

void vc_ScaleSprite()
{
  int x,y,iw,ih,dw,dh,image;

  x=ResolveOperand();
  y=ResolveOperand();
  iw=ResolveOperand();
  ih=ResolveOperand();
  dw=ResolveOperand();
  dh=ResolveOperand();
  image=ResolveOperand();
  if (!LucentOn) CopySpriteZoomClip(x,y,iw,ih,dw,dh,(byte *)image);
  if ( LucentOn) CopySpriteZoomLucentClip(x,y,iw,ih,dw,dh,(byte *)image);
}

void vc_EntityMove()
{
  int i;

  i=ResolveOperand();
  entity[i].moving=0;
  entity[i].speedct=0;

   entity[i].delayct=0;
   entity[i].mode=0;
   entity[i].data1=0;

  ResolveString((char *) (int) movescriptbuf + (int) (i*256));
  entity[i].scriptofs=(char *) (int) movescriptbuf + (int) (i*256);
  entity[i].movecode=4;
}

void vc_HLine()
{
  int x, y, x2, c;

  x=ResolveOperand();
  y=ResolveOperand();
  x2=ResolveOperand();
  c=ResolveOperand();
  if (!LucentOn) HLineClip(x,y,x2,c);
  if ( LucentOn) HLineLucentClip(x,y,x2,c);

}

void vc_VLine()
{
  int x, y, y2, c;

  x=ResolveOperand();
  y=ResolveOperand();
  y2=ResolveOperand();
  c=ResolveOperand();
  if (!LucentOn) VLineClip(x,y,y2,c);
  if ( LucentOn) VLineLucentClip(x,y,y2,c);
}

void vc_Line()
{
  int x, y, x2, y2, c;

  x=ResolveOperand();
  y=ResolveOperand();
  x2=ResolveOperand();
  y2=ResolveOperand();
  c=ResolveOperand();
  if (!LucentOn) LineClip(x,y,x2,y2,c);
  if ( LucentOn) LineLucentClip(x,y,x2,y2,c);
}

void vc_Circle()
{
  int x, y, radius, c;

  x=ResolveOperand();
  y=ResolveOperand();
  radius=ResolveOperand();
  c=ResolveOperand();
  if (!LucentOn) CircleClip(x,y,radius,c);
  if ( LucentOn) CircleLucentClip(x,y,radius,c);
}

void vc_CircleFill()
{
  int x, y, radius, c;

  x=ResolveOperand();
  y=ResolveOperand();
  radius=ResolveOperand();
  c=ResolveOperand();
  if (!LucentOn) CircleFillClip(x,y,radius,c);
  if ( LucentOn) CircleFillLucentClip(x,y,radius,c);
}

void vc_Rect()
{
  int x, y, x2, y2, c;

  x=ResolveOperand();
  y=ResolveOperand();
  x2=ResolveOperand();
  y2=ResolveOperand();
  c=ResolveOperand();
  if (!LucentOn) RectClip(x,y,x2,y2,c);
  if ( LucentOn) RectLucentClip(x,y,x2,y2,c);
}

void vc_RectFill()
{
  int x, y, x2, y2, c;

  x=ResolveOperand();
  y=ResolveOperand();
  x2=ResolveOperand();
  y2=ResolveOperand();
  c=ResolveOperand();
  if (!LucentOn) RectFillClip(x,y,x2,y2,c);
  if ( LucentOn) RectFillLucentClip(x,y,x2,y2,c);
}

void vc_strlen()
{
  char str[256];

  ResolveString(str);
  vcreturn=strlen(str);
}

void vc_strcmp()
{
  char str1[256], str2[256];

  ResolveString(str1);
  ResolveString(str2);
  vcreturn=strcmp(str1, str2);
}

void vc_FontWidth()
{
  int i;

  i=ResolveOperand();
  if (i>=numfonts)
  {
    vcreturn=0;
    return;
  }
  vcreturn=font[i].width;
}

void vc_FontHeight()
{
  int i;

  i=ResolveOperand();
  if (i>=numfonts)
  {
    vcreturn=0;
    return;
  }
  vcreturn=font[i].height;
}

void vc_SetPixel()
{
  int x, y, c;

  x=ResolveOperand();
  y=ResolveOperand();
  c=ResolveOperand();
  SetPixelClip(x,y,c);
}

void vc_GetPixel()
{
  int x, y;

  x=ResolveOperand();
  y=ResolveOperand();
  vcreturn=GetPixelClip(x,y);
}

void vc_EntityOnScreen()
{
  int i,j;

  i=ResolveOperand();
  for (j=0; j<cc; j++)
   if (entidx[j]==i)
   {
     vcreturn=1;
     return;
   }
  vcreturn=0;
}

void vc_GetTile()
{
  int x, y, l;

  x=ResolveOperand();
  y=ResolveOperand();
  l=ResolveOperand();

  switch (l)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5: vcreturn=(int) layers[l][(y*layer[l].sizex)+x]; break;
    case 6: vcreturn=(int) obstruct[(y*layer[0].sizex)+x]; break;
    case 7: vcreturn=(int) zone[(y*layer[0].sizex)+x]; break;
    default: err("GetTile(): Invalid layer value");
  }
}

void vc_SetResolution()
{
  int xres, yres;

  xres=ResolveOperand();
  yres=ResolveOperand();
  ShutdownVideo(0);
  vcreturn=InitVideo(xres,yres);
}

void vc_SetRString()
{
  ResolveString(rstring);
}

void vc_SetClipRect()
{
  cx1=ResolveOperand();
  cy1=ResolveOperand();
  cx2=ResolveOperand();
  cy2=ResolveOperand();
}

void vc_SetRenderDest()
{
  sx=ResolveOperand();
  sy=ResolveOperand();
  screen=(byte *) ResolveOperand();
}

void vc_RestoreRenderSettings()
{
  cx1=0;
  cy1=0;
  cx2=tsx-1;
  cy2=tsy-1;
  sx=tsx;
  sy=tsy;
  screen=vscreen;
}

void vc_PartyMove()
{
  player=0;

  entity[playernum].moving=0;
  entity[playernum].speedct=0;
  entity[playernum].delayct=0;
  entity[playernum].mode=0;
  entity[playernum].data1=0;

  vcpush(cameratracking);
  vcpush(tracker);
  if (cameratracking==1)
  {
    cameratracking=2;
    tracker=playernum;
  }

  ResolveString((char *) (int) movescriptbuf + (int) (playernum*256));
  entity[playernum].scriptofs=(char *) (int) movescriptbuf + (int) (playernum*256);
  entity[playernum].movecode=4;

  while (entity[playernum].movecode)
  {
    while (timer_count)
    {
      ProcessEntities();
      timer_count--;
    }
    Render();
    ShowPage();
  }
  tracker=vcpop();
  cameratracking=vcpop();
  player=&entity[playernum];
  timer_count=0;
}

void vc_WrapBlit()
{
  int a,b,c,d,e;

  a=ResolveOperand();
  b=ResolveOperand();
  c=ResolveOperand();
  d=ResolveOperand();
  e=ResolveOperand();
  if (!LucentOn) WrapBlit(a,b,c,d,(byte *) e);
            else WrapBlitLucent(a,b,c,d,(byte *) e);
}


void vc_TWrapBlit()
{
  int a,b,c,d,e;

  a=ResolveOperand();
  b=ResolveOperand();
  c=ResolveOperand();
  d=ResolveOperand();
  e=ResolveOperand();
  if (!LucentOn) WrapBlitMasked(a,b,c,d,(byte *) e);
            else WrapBlitLucentMasked(a,b,c,d,(byte *) e);
}

void vc_SetMousePos()
{
  int x, y;

  x=ResolveOperand();
  y=ResolveOperand();
  SetMouse(x, y);
}

void vc_HookRetrace()
{
  int script=0;
  char which;

  which=GrabC();
  switch (which)
  {
    case 1: script=ResolveOperand(); break;
    case 2: script=GrabD(); break;
  }
  if (which==2) script+=USERFUNC_MARKER;
  hookretrace=script;
}

void vc_HookTimer()
{
  int script=0;
  char which;

  which=GrabC();
  switch (which)
  {
    case 1: script=ResolveOperand(); break;
    case 2: script=GrabD(); break;
  }
  if (which==2) script+=USERFUNC_MARKER;
  hooktimer=script;
}

void vc_HookKey()
{
  int key, script=0;
  char which;

  key=ResolveOperand();
  which=GrabC();

  switch (which)
  {
    case 1: script=ResolveOperand(); break;
    case 2: script=GrabD(); break;
  }
  if (which==2) script+=USERFUNC_MARKER;
  bindarray[key]=script;
}

void vc_PlayMusic()
{
  char songname[256];

  ResolveString(songname);
  PlayMusic(songname);
}

void vc_PaletteMorph()
{
  int r,g,b,percent,intensity,i,wr,wg,wb;

  r=ResolveOperand(); if (r<0) r=0; if (r>63) r=63;
  g=ResolveOperand(); if (g<0) g=0; if (g>63) g=63;
  b=ResolveOperand(); if (b<0) b=0; if (b>63) b=63;
  percent=100-ResolveOperand();
  intensity=ResolveOperand();

  for (i=0; i<256; i++)
  {
     wr=pal[(i*3)];
     wg=pal[(i*3)+1];
     wb=pal[(i*3)+2];

     wr=((wr*percent)+(r*(100-percent)))/100;
     wg=((wg*percent)+(g*(100-percent)))/100;
     wb=((wb*percent)+(b*(100-percent)))/100;

     pal2[(i*3)]=wr*intensity/63;
     pal2[(i*3)+1]=wg*intensity/63;
     pal2[(i*3)+2]=wb*intensity/63;
  }
  SetPalette(pal2);
}

void EnforceNoDirectories(char *s)
{
  char y[256], *p;

  strcpy(y, s);
  p=(char *) (int) y + (int) strlen(y);
  while (p>y && *p != '/' && *p != '\\')
    p--;
  if (*p == '/' || *p == '\\') p++;
  strcpy(s, p);
}

void vc_OpenFile()
{
  VFILE *f;
  char fname[256];

  ResolveString(fname);
  EnforceNoDirectories(fname);

  f=vopen(fname);
  vcreturn=(quad) f;

  Log(" --> VC opened file %s, ptr %u", fname, (quad) f);
}

void vc_CloseFile()
{
  VFILE *f;

  f=(VFILE *) ResolveOperand();
  vclose(f);

  Log(" --> VC closed file at ptr %u", (quad) f);
}

void vc_QuickRead()
{
  char fname[256], c, *p;
  int d=0, l, i;
  VFILE *f;
  word w=0;

  ResolveString(fname);
  EnforceNoDirectories(fname);
  c=GrabC();
  if (c==op_STRING)
  {
    w=GrabW();
    d=0;
  }
  if (c==op_SARRAY)
  {
    w=GrabW();
    d=ResolveOperand();
  }
  l=ResolveOperand();
  l=l ? l : 1;
  if (!(f=vopen(fname))) err("QuickRead() - could not open %s",fname);

  p=(char *) (quad) stringbuf + ((w+d)*256);
  for (i=0; i<l; i++)
    vgets(p, 255, f);
  while (*p)
  {
    if (*p == 10 || *p == 13) *p=0;
    p++;
  }
  vclose(f);
}

void vc_AddFollower()
{
  int i;

  i=ResolveOperand();
  if (i>=entities) err("AddFollower(): Not a valid entity index. (%d)",i);
  follower[(int)numfollowers]=i;
  numfollowers++;
//  ResetFollowers();
}

void vc_FlatPoly()
{
  int a, b, c, d, e, f, g;

  a=ResolveOperand();
  b=ResolveOperand();
  c=ResolveOperand();
  d=ResolveOperand();
  e=ResolveOperand();
  f=ResolveOperand();
  g=ResolveOperand();
  FlatPoly(a,b,c,d,e,f,g);
}

void vc_TMapPoly()
{
  int a,b,c,d,e,f,g,h,i,j,k,l,m,n,o;

  a=ResolveOperand();
  b=ResolveOperand();
  c=ResolveOperand();
  d=ResolveOperand();
  e=ResolveOperand();
  f=ResolveOperand();
  g=ResolveOperand();
  h=ResolveOperand();
  i=ResolveOperand();
  j=ResolveOperand();
  k=ResolveOperand();
  l=ResolveOperand();
  m=ResolveOperand();
  n=ResolveOperand();
  o=ResolveOperand();
  TMapPoly(a,b,c,d,e,f,g,h,i,j,k,l,m,n,(char *) o);
}

void vc_CacheSound()
{
  char fname[256];

  ResolveString(fname);
  vcreturn=CacheSound(fname);

}

void vc_PlaySound()
{
  int a, b, c;

  a=ResolveOperand();
  b=ResolveOperand();
  c=ResolveOperand();
  PlaySound(a, b, c);
}

void vc_RotScale()
{
  int a, b, c, d, e, f, g;

  a=ResolveOperand();
  b=ResolveOperand();
  c=ResolveOperand();
  d=ResolveOperand();
  e=ResolveOperand();
  f=ResolveOperand();
  g=ResolveOperand();

  RotScale(a, b, c, d, e*3.14159 / 180, (float) f/1000, (byte *) g);
}

void vc_MapLine()
{
  int x, y, sy, l;
  int xtc, ytc, xofs, yofs;

  x=ResolveOperand();
  y=ResolveOperand();
  sy=ResolveOperand();
  l=ResolveOperand();

  xtc=x >> 4;
  ytc=y >> 4;
  xofs= -(x&15);
  yofs=  (y&15);

  MapLine(xofs, sy, yofs, (word *) (layers[l]+((ytc*layer[l].sizex)+xtc)));
}

void vc_TMapLine()
{
  int x, y, sy, l;
  int xtc, ytc, xofs, yofs;

  x=ResolveOperand();
  y=ResolveOperand();
  sy=ResolveOperand();
  l=ResolveOperand();

  xtc=x >> 4;
  ytc=y >> 4;
  xofs= -(x&15);
  yofs=  (y&15);

  TMapLine(xofs, sy, yofs, (word *) (layers[l]+((ytc*layer[l].sizex)+xtc)));
}

void vc_val()
{
  char s[256];

  ResolveString(s);
  vcreturn=atoi(s);
}

void vc_TScaleSprite()
{
  int x,y,iw,ih,dw,dh,image;

  x=ResolveOperand();
  y=ResolveOperand();
  iw=ResolveOperand();
  ih=ResolveOperand();
  dw=ResolveOperand();
  dh=ResolveOperand();
  image=ResolveOperand();
  if (!LucentOn) TCopySpriteZoomClip(x,y,iw,ih,dw,dh,(byte *) image);
  if ( LucentOn) TCopySpriteZoomLucentClip(x,y,iw,ih,dw,dh,(byte *) image);
}

void vc_GrabRegion()
{
  int x1, y1, x2, y2;
  int i, j, w, h;
  char *ptr;

  x1=ResolveOperand();
  y1=ResolveOperand();
  x2=ResolveOperand()+1;
  y2=ResolveOperand()+1;
  ptr=(char *) ResolveOperand();

  if (x2>sx-1) x2=sx-1;
  if (y2>sy-1) y2=sy-1;
  if (x1<0) x1=0;
  if (y1<0) y1=0;

  w=x2 - x1;
  h=y2 - y1;

  for (j=y1; j<y2; j++)
    for (i=x1; i<x2; i++)
      ptr[((j-y1)*w)+i]=screen[(j*sx)+i];
}

void vc_Log()
{
  char s[256];

  ResolveString(s);
  Log(s);
}

void vc_fseekline()
{
  int line, i;
  VFILE *f;

  line=ResolveOperand();
  f=(VFILE *) ResolveOperand();

  vseek(f, 0, 0);
  line=( line==0 ? 0 : line-1 );

  for (i=0; i<line; i++)
    vgets(strbuf, 255, f);
}

void vc_fseekpos()
{
  int pos;
  VFILE *f;

  pos=ResolveOperand();
  f=(VFILE *) ResolveOperand();
  vseek(f, pos, 0);
}

void vc_fread()
{
  char *buffer;
  int len;
  VFILE *f;

  buffer=(char *) ResolveOperand();
  len=ResolveOperand();
  f=(VFILE *) ResolveOperand();
  vread(buffer, len, f);
}

void vc_fgetbyte()
{
  VFILE *f;
  byte i;

  f=(VFILE *) ResolveOperand();
  vread(&i, 1, f);
  vcreturn=(int) i;
}

void vc_fgetword()
{
  VFILE *f;
  word i;

  f=(VFILE *) ResolveOperand();
  vread(&i, 2, f);
  vcreturn=(int) i;
}

void vc_fgetquad()
{
  VFILE *f;
  quad i;

  f=(VFILE *) ResolveOperand();
  vread(&i, 4, f);
  vcreturn=(int) i;
}

void vc_fgetline()
{
  char c, *p;
  VFILE *f;
  word w=0;
  quad d;

  c=GrabC();
  if (c==op_STRING)
  {
    w=GrabW();
    d=0;
  }
  if (c==op_SARRAY)
  {
    w=GrabW();
    d=ResolveOperand();
  }
  f=(VFILE *) ResolveOperand();
  p=(char *) (quad) stringbuf + (w*256);
  vgets(p, 255, f);
  while (*p)
  {
    if (*p == 10 || *p == 13) *p=0;
    p++;
  }
}

void vc_fgettoken()
{
  char c, *p;
  VFILE *f;
  word w=0;
  quad d;

  c=GrabC();
  if (c==op_STRING)
  {
    w=GrabW();
    d=0;
  }
  if (c==op_SARRAY)
  {
    w=GrabW();
    d=ResolveOperand();
  }
  f=(VFILE *) ResolveOperand();
  p=(char *) (quad) stringbuf + (w*256);
  vscanf(f, "%s", p);
}

void vc_fwritestring()
{
  char s[256];
  FILE *f;

  ResolveString(s);
  f=(FILE *) ResolveOperand();
  fprintf(f,"%s\n",s);
}

void vc_fwrite()
{
  int a, b, c;

  a=ResolveOperand();
  b=ResolveOperand();
  c=ResolveOperand();
  fwrite((char *) a, 1, b, (FILE *) c);
}

void vc_frename()
{
  char s1[256], s2[256];

  ResolveString(s1);
  ResolveString(s2);
  EnforceNoDirectories(s1);
  EnforceNoDirectories(s2);
  rename(s1, s2);

  Log(" --> VC renamed %s to %s.", s1, s2);
}

void vc_fdelete()
{
  char s[256];

  ResolveString(s);
  EnforceNoDirectories(s);
  remove(s);

  Log(" --> VC deleted %s.", s);
}

void vc_fwopen()
{
  char s[256];

  ResolveString(s);
  EnforceNoDirectories(s);
  vcreturn=(int) fopen(s,"wb");

  Log(" --> VC opened %s for writing, ptr %u.", s, vcreturn);
}

void vc_fwclose()
{
  FILE *f;

  f=(FILE *) ResolveOperand();
  fclose(f);

  Log(" --> VC close file opened for writing, ptr %u.", (int) f);
}

void vc_memcpy()
{
  int a, b, c;

  a=ResolveOperand();
  b=ResolveOperand();
  c=ResolveOperand();
  memcpy((char *) a, (char *) b, c);
}

void vc_memset()
{
  int a, b, c;

  a=ResolveOperand();
  b=ResolveOperand();
  c=ResolveOperand();
  memset((char *) a, (byte) b, c);
}

void vc_Silhouette()
{
  int x=ResolveOperand();
  int y=ResolveOperand();
  int w=ResolveOperand();
  int h=ResolveOperand();
  byte *img=(byte *) ResolveOperand();
  byte c=(byte) ResolveOperand();

  if (ClipOn)
  {
    LucentOn ? SilhouetteLucentClip(x,y,w,h,c,img)
             : SilhouetteClip(x,y,w,h,c,img);
  }
  else
  {
    LucentOn ? SilhouetteLucent(x,y,w,h,c,img)
             : Silhouette(x,y,w,h,c,img);
  }
}

void vc_SilhouetteScale()
{
  int x=ResolveOperand();
  int y=ResolveOperand();
  int sw=ResolveOperand();
  int sh=ResolveOperand();
  int dw=ResolveOperand();
  int dh=ResolveOperand();
  byte *img=(byte *) ResolveOperand();
  byte c=(byte) ResolveOperand();

  if (ClipOn)
  {
    LucentOn ? SilhouetteZoomLucentClip(x,y,sw,sh,dw,dh,c,img)
             : SilhouetteZoomClip(x,y,sw,sh,dw,dh,c,img);
  }
  else
  {
    LucentOn ? SilhouetteZoomLucent(x,y,sw,sh,dw,dh,c,img)
             : SilhouetteZoom(x,y,sw,sh,dw,dh,c,img);
  }
}

void vc_Tint()
{
  int x=ResolveOperand();
  int y=ResolveOperand();
  int w=ResolveOperand();
  int h=ResolveOperand();
  byte *img=(byte *)ResolveOperand();
  byte c=(byte)ResolveOperand();

  ClipOn ? TintClip(x,y,w,h,c,img)
         : Tint(x,y,w,h,c,img);
}

void vc_TintScale()
{
  int x=ResolveOperand();
  int y=ResolveOperand();
  int sw=ResolveOperand();
  int sh=ResolveOperand();
  int dw=ResolveOperand();
  int dh=ResolveOperand();
  byte *img=(byte *) ResolveOperand();
  byte c=(byte) ResolveOperand();

  ClipOn ? TintZoomClip(x,y,sw,sh,dw,dh,c,img)
         : TintZoom(x,y,sw,sh,dw,dh,c,img);
}

void vc_Mosaic()
{
  int a,b,c,d,e,f,g;

  a=ResolveOperand();
  b=ResolveOperand();
  c=ResolveOperand();
  d=ResolveOperand();
  e=ResolveOperand();
  f=ResolveOperand();
  g=ResolveOperand();
  Mosaic(a,b,(byte *) c,d,e,f,g);
}

void vc_WriteVars()
{
  FILE *f;

  f=(FILE *) ResolveOperand();
  fwrite(globalint, 4, maxint, f);
  fwrite(stringbuf, 256, stralloc, f);
}

void vc_ReadVars()
{
  VFILE *f;

  f=(VFILE *) ResolveOperand();
  vread(globalint, 4*maxint, f);
  vread(stringbuf, 256*stralloc, f);
}

void vc_Asc()
{
  char s[256];

  ResolveString(s);
  vcreturn=(int) s[0];
}

void vc_NumForScript()
{
  vcreturn=GrabD();
}

void vc_Filesize()
{
  char s[256];
  VFILE *f;

  ResolveString(s);
  f=vopen(s);
  vcreturn=filesize(f);
  vclose(f);
}

void vc_FTell()
{
  VFILE *f=(VFILE *)ResolveOperand();
  vcreturn=vtell(f);
}

void vc_CheckCorrupt()
{
  Log("checking for corruption...");
  CheckCorruption();
}

void HandleStdLib()
{
  int x=0;
  byte c=0;

  c=GrabC();
  switch (c)
  {
    case 1: vc_Exit_(); break;
    case 2: vc_Message(); break;
    case 3: vc_Malloc(); break;
    case 4: vc_Free(); break;
    case 5: vc_pow(); break;
    case 6: vc_loadimage(); break;
    case 7: vc_copysprite(); break;
    case 8: vc_tcopysprite(); break;
    case 9: Render(); break;
    case 10: ShowPage(); break;
    case 11: vc_EntitySpawn(); break;
    case 12: vc_SetPlayer(); break;
    case 13: vc_Map(); break;
    case 14: vc_LoadFont(); break;
    case 15: vc_PlayFLI(); break;
    case 16: curx=ResolveOperand(); cury=ResolveOperand(); break;
    case 17: vc_PrintString(); break;
    case 18: vc_LoadRaw(); break;
    case 19: vc_SetTile(); break;
    case 20: allowconsole=ResolveOperand(); break;
    case 21: vc_ScaleSprite(); break;
    case 22: ProcessEntities(); break;
    case 23: UpdateControls(); break;
    case 24: UnPress(ResolveOperand()); break;
    case 25: vc_EntityMove(); break;
    case 26: vc_HLine(); break;
    case 27: vc_VLine(); break;
    case 28: vc_Line(); break;
    case 29: vc_Circle(); break;
    case 30: vc_CircleFill(); break;
    case 31: vc_Rect(); break;
    case 32: vc_RectFill(); break;
    case 33: vc_strlen(); break;
    case 34: vc_strcmp(); break;
    case 35: break; //CD_Stop(); break;
    case 36: ResolveOperand(); break; //CD_Play(ResolveOperand()); break;
    case 37: vc_FontWidth(); break;
    case 38: vc_FontHeight(); break;
    case 39: vc_SetPixel(); break;
    case 40: vc_GetPixel(); break;
    case 41: vc_EntityOnScreen(); break;
    case 42: vcreturn=0;
             if ((x=ResolveOperand()))
               vcreturn=rand()%x;
             break;
    case 43: vc_GetTile(); break;
    case 44: vc_HookRetrace(); break;
    case 45: vc_HookTimer(); break;
    case 46: vc_SetResolution(); break;
    case 47: vc_SetRString(); break;
    case 48: vc_SetClipRect(); break;
    case 49: vc_SetRenderDest(); break;
    case 50: vc_RestoreRenderSettings(); break;
    case 51: vc_PartyMove(); break;
    case 52: vcreturn=sintbl[ResolveOperand()]; break;
    case 53: vcreturn=costbl[ResolveOperand()]; break;
    case 54: vcreturn=tantbl[ResolveOperand()]; break;
    case 55: ReadMouse(); break;
    case 56: ClipOn=ResolveOperand(); break;
    case 57: LucentOn=ResolveOperand(); break;
    case 58: vc_WrapBlit(); break;
    case 59: vc_TWrapBlit(); break;
    case 60: vc_SetMousePos(); break;
    case 61: vc_HookKey(); break;
    case 62: vc_PlayMusic(); break;
    case 63: StopMusic(); break;
    case 64: vc_PaletteMorph(); break;
    case 65: vc_OpenFile(); break;
    case 66: vc_CloseFile(); break;
    case 67: vc_QuickRead(); break;
    case 68: vc_AddFollower(); break;
//    case 69: vc_KillFollower(); break;
//    case 70: vc_KillAllFollowers(); break;
//    case 71: ResetFollowers();
    case 72: vc_FlatPoly(); break;
    case 73: vc_TMapPoly(); break;
    case 74: vc_CacheSound(); break;
    case 75: FreeAllSounds(); break;
    case 76: vc_PlaySound(); break;
    case 77: vc_RotScale(); break;
    case 78: vc_MapLine(); break;
    case 79: vc_TMapLine(); break;
    case 80: vc_val(); break;
    case 81: vc_TScaleSprite(); break;
    case 82: vc_GrabRegion(); break;
    case 83: vc_Log(); break;
    case 84: vc_fseekline(); break;
    case 85: vc_fseekpos(); break;
    case 86: vc_fread(); break;
    case 87: vc_fgetbyte(); break;
    case 88: vc_fgetword(); break;
    case 89: vc_fgetquad(); break;
    case 90: vc_fgetline(); break;
    case 91: vc_fgettoken(); break;
    case 92: vc_fwritestring(); break;
    case 93: vc_fwrite(); break;
    case 94: vc_frename(); break;
    case 95: vc_fdelete(); break;
    case 96: vc_fwopen(); break;
    case 97: vc_fwclose(); break;
    case 98: vc_memcpy(); break;
    case 99: vc_memset(); break;
    case 100: vc_Silhouette(); break;
    case 101: vcreturn=(int) InitMosaicTable(); break;
    case 102: vc_Mosaic(); break;
    case 103: vc_WriteVars(); break;
    case 104: vc_ReadVars(); break;
    case 105: ExecuteEvent(ResolveOperand()); break;
    case 106: vc_Asc(); break;
    case 107: ExecuteUserFunc(ResolveOperand()); break;
    case 108: vc_NumForScript(); break;
    case 109: vc_Filesize(); break;
    case 110: vc_FTell(); break;
    case 111: vc_CheckCorrupt(); break;
    default: err("VC Execution error: Invalid STDLIB index. (%d)",(int) c);
  }
}

// ========================== VC Interpretation Core ==========================

int ProcessIf()
{
  byte exec, c;

  exec=ProcessIfOperand();               // Get base value;

  while (1)
  {
    c=GrabC();
    switch (c)
    {
      case i_AND: exec=exec & ProcessIfOperand(); continue;
      case i_OR: exec=exec | ProcessIfOperand(); continue;
      case i_UNGROUP: break;
    }
    break;
  }
  return exec;
}

int ProcessIfOperand()
{
  byte op_desc;
  int eval;

  eval=ResolveOperand();
  op_desc=GrabC();
  switch (op_desc)
  {
     case i_ZERO: if (!eval) return 1; else return 0;
     case i_NONZERO: if (eval) return 1; else return 0;
     case i_EQUALTO: if (eval == ResolveOperand()) return 1; else return 0;
     case i_NOTEQUAL: if (eval != ResolveOperand()) return 1; else return 0;
     case i_GREATERTHAN: if (eval > ResolveOperand()) return 1; else return 0;
     case i_GREATERTHANOREQUAL: if (eval >= ResolveOperand()) return 1; else return 0;
     case i_LESSTHAN: if (eval < ResolveOperand()) return 1; else return 0;
     case i_LESSTHANOREQUAL: if (eval <= ResolveOperand()) return 1; else return 0;
     case i_GROUP: if (ProcessIf()) return 1; else return 0;
  }
  return 0;
}

void HandleIf()
{
  char *d;

  if (ProcessIf())
  {
    GrabD();
    return;
  }
  d=(char *) GrabD();
  code=(char *) (int) basevc + (int) d;

  return;
}

void HandleExternFunc()
{
  word i, j, k;
  //lvars *ob   = NULL;
  //lvars *save = NULL;
  lvars temp;
  lvars ob;

  // save lvar
  memcpy(&temp, &lvar, sizeof(lvars));

  memset(&ob, 0, sizeof(lvars));
  //ob = (lvars *)valloc(sizeof(lvars)); //2640); //sizeof(lvars)); //2640);
  //MSS_CHECK_POINTER_VALIDITY(ob);
  //MSS_SET_BLOCK_LABEL(ob, "ob");

  k = 0;
  i = GrabW();
  for (j=0; j<funcs[i].numargs; j++)
  {
    switch (funcs[i].argtype[j])
    {
      case 1: ob.nargs[j] = ResolveOperand(); break;
      case 3: ResolveString(ob.s + (k << 8)), k++; break;
    }
  }

  // copy in ob
  memcpy(&lvar, &ob, sizeof(lvars));
  //lvar = &ob;
  //save = lvar
  //lvar = ob;
  //ob   = save;

  vcpush((quad)basevc);
  vcpush((quad)code);

  basevc = sysvc;
  code = (char *)(sysvc + funcs[i].syscodeofs);

  if (vctrack)
  {
    Log(" --> Entering user func %s, codeofs %d",
      funcs[i].fname, funcs[i].syscodeofs);
  }

  ExecuteBlock();
  basevc = (char *)vcpop();

  // restore lvar
  memcpy(&lvar, &temp, sizeof(lvars));
  //memcpy(lvar, &temp, sizeof(lvars));
  //vfree(lvar);
  //lvar = ob;

  if (vctrack)
  {
    Log(" --> Returned from %s", funcs[i].fname);
  }

//  MSS_CHECK_ALL_BLOCKS;
}

void HandleAssign()
{
  byte c, d;
  word w;
  quad location=0, value, ofs=0;

  c=GrabC();
  if (c==op_STRING)
  {
    w=GrabW();
    c=GrabC();
    if (c!=a_SET) err("VC execution error: Corrupt string assignment");
    location=(quad) stringbuf + (w*256);
    ResolveString((char *) location);
    return;
  }
  if (c==op_SARRAY)
  {
    w=GrabW();
    w+=ResolveOperand();
    c=GrabC();
    if (c!=a_SET) err("VC execution error: Corrupt string assignment");
    location=(quad) stringbuf + (w*256);
    ResolveString((char *) location);
    return;
  }
  if (c==op_SLOCAL)
  {
    w=GrabW();
    c=GrabC();
    if (c!=a_SET) err("VC execution error: Corrupt string assignment");
    location=(quad) &lvar.s[w*256];
    ResolveString((char *) location);
    return;
  }
  switch (c)
  {
    case op_UVAR:    location=GrabD(); break;
    case op_UVARRAY: location=GrabD(); location+=ResolveOperand(); break;
    case op_HVAR0:   location=(int) GrabC(); break;
    case op_HVAR1:   location=(int) GrabC(); ofs=ResolveOperand(); break;
    case op_LVAR:    location=(int) GrabC(); break;
    default: err("VC Execution error: Unknown assignment category.");
  }
  value=ReadInt(c, location, ofs);
  d=GrabC();
  switch(d)
  {
    case a_SET: value=ResolveOperand(); break;
    case a_INC: value++; break;
    case a_DEC: value--; break;
    case a_INCSET: value+=ResolveOperand(); break;
    case a_DECSET: value-=ResolveOperand(); break;
    default:
      err("VC Execution error: Invalid assignment operator %d.", (int) d);
  }
  WriteInt(c, location, ofs, value);
}

void HandleSwitch()
{
  int realvalue=0;
  int compvalue=0;
  byte c=0;
  byte *next=0;

  realvalue=ResolveOperand();
  c=GrabC();
  while (c!=opRETURN)
  {
    compvalue=ResolveOperand();
    next=(byte *) GrabD();
    if (compvalue!=realvalue)
    {
      code=(char *) (int) basevc+(int) next;
      c=GrabC();
      continue;
    }
    ExecuteSection();
    c=GrabC();
  }
}

void ExecuteVC()
{
  byte c=0;

  while (1)
  {
    if (kill) break;
    c=GrabC();
    switch (c)
    {
      case opEXEC_STDLIB: HandleStdLib(); break;
      case opEXEC_LOCALFUNC: break;
      case opEXEC_EXTERNFUNC: HandleExternFunc(); break;
      case opIF: HandleIf(); break;
      case opELSE: break;
      case opGOTO: code=basevc+GrabD(); break;
      case opSWITCH: HandleSwitch(); break;
      case opASSIGN: HandleAssign(); break;
      case opRETURN: code=(char *) vcpop(); break;
      case opSETRETVAL: vcreturn=ResolveOperand(); break;
      default:
        err("Internal VC execution error. (%d)",(int) code - (int) basevc);
    }
    if ((int) code != -1) continue; else break;
  }
}

void ExecuteBlock()
{
  byte c=0;

  while (1)
  {
    if (kill) break;
    c=GrabC();
    switch (c)
    {
      case opEXEC_STDLIB: HandleStdLib(); break;
      case opEXEC_LOCALFUNC: break;
      case opEXEC_EXTERNFUNC: HandleExternFunc(); break;
      case opIF: HandleIf(); break;
      case opELSE: break;
      case opGOTO: code=basevc+GrabD(); break;
      case opSWITCH: HandleSwitch(); break;
      case opASSIGN: HandleAssign(); break;
      case opRETURN: code=(char *) vcpop(); break;
      case opSETRETVAL: vcreturn=ResolveOperand(); break;
      default:
        err("Internal VC execution error. (%d)",(int) code - (int) basevc);
    }
    if (c != opRETURN) continue; else break;
  }
}

void ExecuteSection()
{
  byte c=0;

  while (1)
  {
    if (kill) break;
    c=GrabC();
    switch (c)
    {
      case opEXEC_STDLIB: HandleStdLib(); break;
      case opEXEC_LOCALFUNC: break;
      case opEXEC_EXTERNFUNC: HandleExternFunc(); break;
      case opIF: HandleIf(); break;
      case opELSE: break;
      case opGOTO: code=basevc+GrabD(); break;
      case opSWITCH: HandleSwitch(); break;
      case opASSIGN: HandleAssign(); break;
      case opRETURN: break;
      case opSETRETVAL: vcreturn=ResolveOperand(); break;
      default:
        err("Internal VC execution error. (%d)", (int) code - (int) basevc);
    }
    if (c != opRETURN) continue; else break;
  }
}

void ExecuteEvent(int i)
{
  invc++;
  vcpush((quad) code);
  vcpush((quad) basevc);
  if (i>mapevents) err("VC event out of bounds (%d)",i);
  basevc=mapvc;
  code=(char *) (int) mapvc + (int) mapvctbl[i];
  vcpush(-1);
  ExecuteVC();
  basevc=(char *) vcpop();
  code=(char *) vcpop();
  invc--;
}

void ExecuteUserFunc(int i)
{
  //lvars *ob=0;
  lvars temp;

  // save lvar
  memcpy(&temp, &lvar, sizeof(lvars));

  vcpush((quad) code);
  vcpush((quad) basevc);

  if (i > numfuncs)
    err("VC event out of bounds");

  basevc = sysvc;
  code = (char *)(sysvc + funcs[i].syscodeofs);

  vcpush(-1);

  memset(&lvar, 0, sizeof(lvar));
  //ob = lvar;
  //lvar = (lvars *)valloc(sizeof(lvars)); //2640);
  //MSS_SET_BLOCK_LABEL(lvar, "lvar");

  ExecuteVC();
  basevc = (char *) vcpop();
  code = (char *) vcpop();

  //free(lvar);
  //lvar = ob;

  // restore lvar
  memcpy(&lvar, &temp, sizeof(lvars));
}

void HookRetrace()
{
  if (!hookretrace) return;
  if (hookretrace<USERFUNC_MARKER) ExecuteEvent(hookretrace);
  if (hookretrace>=USERFUNC_MARKER) ExecuteUserFunc(hookretrace-USERFUNC_MARKER);
}

void HookTimer()
{
  if (!hooktimer) return;
  if (hooktimer<USERFUNC_MARKER) ExecuteEvent(hooktimer);
  if (hooktimer>=USERFUNC_MARKER) ExecuteUserFunc(hooktimer-USERFUNC_MARKER);
}

void HookKey(int script)
{
  if (!script) return;
  if (script<USERFUNC_MARKER) ExecuteEvent(script);
  if (script>=USERFUNC_MARKER) ExecuteUserFunc(script-USERFUNC_MARKER);
}
