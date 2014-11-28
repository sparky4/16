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

const char *const ME2_VERSION = "2.01a";

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// CHANGELOG:
// <vecna, may 14>
// + erased valloc(), replaced with a_memory.c
// + added Log() -- output to MAPED.LOG
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include <dos.h>
#include <stdio.h>

#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "config.h"
#include "gui.h"
#include "keyboard.h"
#include "mikmod.h"
#include "mouse.h"
#include "render.h"
#include "timer.h"
#include "vdriver.h"

#include "ssaver.h"
#include "smalfont.h"
#include "tilesel.h"
#include "minimap.h"
#include "guicomp.h"
#include "config.h"

#define byte unsigned char
#define word unsigned short

// Includes for a_memory.c

void *valloc(int amount, char *desc, int owner);
void *qvalloc(int amount);
int  vfree(void *pointer);
void qvfree(void *pointer);
void FreeByOwner(int owner);
void MemReport(void);
void CheckCorruption(void);

// ================================= Data ====================================

typedef struct
{
  unsigned short start;              // strand start
  unsigned short finish;             // strand end
  unsigned short delay;              // tile-switch delay
  unsigned short mode;               // tile-animation mode
} vspanim_r;

typedef struct
{
  char pmultx,pdivx;                 // parallax multiplier/divisor for X
  char pmulty,pdivy;                 // parallax multiplier/divisor for Y
  unsigned short sizex, sizey;       // layer dimensions.
  unsigned char trans, hline;        // transparency flag | hline (raster fx)
} layer_r;

typedef struct
{
  char name[40];                     // zone name/desc
  unsigned short script;             // script to call thingy
  unsigned short percent;            // chance of executing
  unsigned short delay;              // step-delay
  unsigned short aaa;                // Accept Adjacent Activation
  unsigned short entityscript;       // script to call for entities
} zoneinfo;

typedef struct
{
  char zonename[16];                   // zone description
  unsigned short int callevent;        // event number to call
  unsigned char percent;               // chance (in 255) of event occurance
  unsigned char delay;                 // step-delay before last occurance
  unsigned char aaa;                   // accept adjacent activation
  char savedesc[31];                   // savegame description
} oldzone;

typedef struct
{
  int x, y;                            // xwc, ywx position
  word tx, ty;                         // xtc, ytc position
  byte facing;                         // direction entity is facing
  byte moving, movecnt;                // direction entity is moving
  byte frame;                          // bottom-line frame to display
  byte specframe;                      // special-frame set thingo
  byte chrindex, reset;                // CHR index | Reset animation
  byte obsmode1, obsmode2;             // can be obstructed | Is an obstruction
  byte speed, speedct;                 // entity speed, speedcount :)
  byte delayct;                        // animation frame-delay
  char *animofs, *moveofs;             // anim script | move script
  byte face, actm;                     // auto-face | activation mode
  byte movecode, movescript;           // movement type | movement script
  byte ctr, mode;                      // sub-tile move ctr, mode flag (internal)
  word step, delay;                    // step, delay
  word stepctr, delayctr;              // internal use counters
  word data1, data2, data3;            //
  word data4, data5, data6;            //
  int  actscript;                      // activation script
  int  expand1, expand2;               //
  int  expand3, expand4;               //
  char desc[20];                       // Entity description.
} entity_r;

typedef struct
{
  char t[60];
} chrlist_r;

typedef struct
{
  char t[200];
} movescript;

typedef struct
{
  unsigned short x;                    // xwc position
  unsigned short y;                    // ywc position
  unsigned char facing;                // direction entity is facing
  unsigned char moving;                // direction entity is moving
  unsigned char movcnt;                // how far left to move in this tile
  unsigned char framectr;              // frame sequence counter
  unsigned char specframe;             // special-frame set thingo
  unsigned char chrindex, movecode;    // CHR index / movement pattern code
  unsigned char activmode, obsmode;    // activation mode, obstruction mode
  unsigned char v1,v2,v3;              // packing stuff.
  unsigned int actscript, movescript;  // script references
  unsigned char speed, speedct;        // entity speed, speedcount :)
  unsigned short step, delay,          // Misc data entries
                 data1, data2,         // More misc data
                 data3, data4,         // yet more crappy misc data.
                 delayct,adjactv;      // yet more internal crap
  unsigned short x1,y1,x2,y2;          // bounding box coordinates
  unsigned char curcmd, cmdarg;        // Script commands/arguments
  unsigned char *scriptofs;            // offset in script parsing
  unsigned char face,chasing,          // face player when activated | chasing
                chasespeed, chasedist; // chasing variables
  unsigned short cx,cy;                // current-tile pos (moving adjusted)
  int expand1;                         // always room for improvement
  char entitydesc[20];                 // Editing description
} oldent_r;

zoneinfo zones[256];                 // zone stuff.
layer_r layer[6];                    // Array of layer data
vspanim_r vspanim[100];              // tile animation data
unsigned short vadelay[100];         // Tile animation delay ctr
oldzone tzone;                       // zone for oldmap-to-v2 converting
entity_r entity[256];                // Entity records.
oldent_r oldent;                     // entity for oldmap-to-v2 converting
byte entities=0;                     // number of allocated entities.
chrlist_r chrlist[100];              // Max. 100 CHRs per map.
movescript ms[100];                  // move scripts arrays
byte nmchr=0;                        // number of active/loaded CHRs.
byte nms=0;                          // number of movement scripts

char mapname[60];                    // MAP filename
char vspname[60];                    // VSP filemap
char rstring[20];                    // render-order string
char musname[60];                    // MAP default music
char numlayers;                      // number of layers in map
short xstart, ystart;                // MAP start locations
char *obstruct, *zone;               // obstruction and zone buffers
int numzones=0;                      // Number of active zones.
unsigned short *layers[6];           // Raw layer data
char wrap, soundokay=0;

// -- vsp related data --

unsigned short numtiles;             // number of allocated tiles in VSP
unsigned char *vsp;                  // VSP ptr
char vsp_locked=0;
char vsp_pw[8];

// -- map compression

char *cb,*csrc;                      // compression workspace buffer
int bufsize;                         // how many bytes need to be written

// -- editing related data --

int xwin=0, ywin=0;                  // Editing viewplane
short lt=0, rt=0;                    // left-button tile, right-button tile
char curzone=0;                      // current zone.
char scrollmode=0;                   // tile or pixel scroll modes.
char mouse_scroll=1;
char layertoggle[10];                // layer visible toggles
char el=0;                           // current editing layer.
char modified=0;                     // Current buffer modified or not.

// -- music stuff --

char is_playing=0;
char cmname[60];
UNIMOD *mf=0;

// -- copy/paste & block fill --

word *copybuf=0;
int copybuf_wide=0,copybuf_deep=0;
int selx1=0, sely1=0, selx2=0, sely2=0;
int shifted=0, pasting=0;
extern char curtrack;

char vcedprog[80];
byte* translucency_table; //zero

int moving_up=0,moving_down=0,moving_left=0,moving_right=0; // aen

// ================================= Code ====================================

//added by zero
void LoadTransTable()
{
  FILE *fp;

  if (!translucency_table)
    translucency_table=(byte *) valloc(65536,"trans tbl",0);

  fp=fopen("trans.tbl","rb");
  if (!fp)
  {
    vfree(translucency_table);
    translucency_table=0;
    return;
  }

  fread(translucency_table, 65536, 1, fp);
  fclose(fp);
}

int Exist(char *fname)
{ FILE *tempf;

  tempf=fopen(fname,"rb");
  if (tempf)
  {
     fclose(tempf);
     return 1;
  }
  else return 0;
}

void err(char *str, ...)
{
   va_list argptr;
   char msg[256];

   va_start(argptr, str);
   vsprintf(msg, str, argptr);
   va_end(argptr);

  ShutdownVideo();
  ShutdownTimer();
  ShutdownKeyboard();
  //CD_DeInit(); // wasn't even valid! CD_Deinit() was incorrect <aen, apr 21>
               // Actually, Wat is by default case insensitive, so nyeh.;) <vec>
  //if (curtrack != 1) //CD_Stop();

  printf("%s \n",msg);
  exit(-1);
}

void errf(char *str, char *str1)
{
  sprintf(strbuf,str,str1);
  err(strbuf);
}

void errn(char *str, int n)
{
  sprintf(strbuf,str,n);
  err(strbuf);
}

int random(int lo, int hi)
{
  int range=hi-lo+1;
  int i=rand() % range;
  return i+lo;
}

void PlayMusic(char *fname)
{
  if (!soundokay) return;
  if (is_playing)
  {
    MD_PlayStop();
    ML_Free(mf);
    is_playing=0;
  }
  mf=ML_LoadFN(fname);
  if (!mf)
  {
    Message("Could not load specified music.",500);
    return;
  }
  MP_Init(mf);
  md_numchn=mf->numchn; mp_loop=1;
  MD_PlayStart();
  is_playing=1;
}

void StopMusic()
{
  if (!soundokay) return;
  MD_PlayStop();
  ML_Free(mf);
  is_playing=0;
}

void LoadVSP(char *fname)
{ FILE *f;
  short ver;

  if (!(f=fopen(fname,"rb")))
     errf("VSP file %s not found.",fname);

  fread(&ver, 1, 2, f);
  if (ver!=2 && ver!=3)
     errf("VSP file %s is not the correct VSP format.",fname);
  if (ver==2)
  {
    fread(&pal, 1, 768, f);
    set_intensity(63);
    fread(&numtiles, 1, 2, f);
    vsp=(char *) valloc(256*numtiles, "vsp data", 0);
    fread(vsp, 256, numtiles, f);
    fread(&vspanim, 8, 100, f);
    fclose(f);
  }
  if (ver==3)
  {
    fread(&pal, 1, 768, f);
    set_intensity(63);
    fread(&numtiles, 1, 2, f);
    vsp=(char *) valloc(256*numtiles, "vsp data", 0);
    fread(&bufsize, 1, 4, f);
    ReadCompressedLayer1(vsp, 256*numtiles, f);
    fread(&vspanim, 8, 100, f);
    fclose(f);
  }
}

void SaveVSP(char *fname)
{ FILE *f;
  short ver;

  f=fopen(fname,"wb");
  ver=3;
  fwrite(&ver, 1, 2, f);
  fwrite(&pal, 1, 768, f);
  fwrite(&numtiles, 1, 2, f);
  cb=(char *) valloc(numtiles*256, "vsp tempbuf", 0);
  bufsize=0;
  WriteCompressedLayer1(vsp, numtiles*256);
  fwrite(&bufsize, 1, 4, f);
  fwrite(cb, 1, bufsize, f);
  //free(bufsize); // what the hell??? @_@
  fwrite(&vspanim, 8, 100, f);
  fclose(f);
}

void EmitC (char c)
{
  *csrc=c;
  csrc++;
  bufsize++;
}

void EmitW (short int w)
{
  char *ptr;

  ptr=(char *)&w;
  *csrc=*ptr;
  csrc++;
  ptr++;
  *csrc=*ptr;
  csrc++;
  bufsize+=2;
}

void WriteCompressedLayer1(unsigned char *p, int len)
{ int i;
  unsigned char byt,samect;
  unsigned char repcode;

  csrc=cb;
  i=0; bufsize=0;
  do
  {   byt=p[i++];
      samect=1;
      while (samect<254 && i<len && byt==p[i])
      {
         samect++;
         i++;
      }
      if (samect==2 && byt != 0xFF) { EmitC(byt); }
      if (samect==3 && byt != 0xFF) { EmitC(byt); EmitC(byt); }
      if (samect>3 || byt == 0xFF)
      {
         repcode=0xFF;
         EmitC(repcode);
         repcode=(char) samect;
         EmitC(repcode);
      }
      EmitC(byt);
  } while (i<len);
}

void ReadCompressedLayer1(unsigned char *dest, int len, FILE *f)
{ int j,n;
  unsigned char run;
  unsigned char w;

  n=0;
  do
  {
     fread(&w, 1, 1, f);
     if (w==0xFF)
     {
         fread(&run, 1, 1, f);
         fread(&w, 1, 1, f);
         for (j=0; j<run; j++)
             dest[n+j]=w;
         n+=run;
     }
     else
      {
         dest[n]=w;
         n++;
      }
  } while (n<len);
}

void WriteCompressedLayer2(unsigned short *p,int len)
{ int i;
  unsigned short byt,samect;
  unsigned char repcode;

  csrc=cb;
  i=0; bufsize=0;
  do
  {   byt=p[i++];
      samect=1;
      while (samect<255 && i<len && byt==p[i])
      {
         samect++;
         i++;
      }
      if (samect>1)
      {
         repcode=(char) samect;
         EmitC(repcode);
         repcode=0xFF;
         EmitC(repcode);
      }
      EmitW(byt);
  } while (i<len);
}

void ReadCompressedLayer2(unsigned short *dest, int len, FILE *f)
{ int j,n;
  unsigned char run;
  unsigned short w;

  n=0;
  do
  {
     fread(&w, 1, 2, f);
     if ((w & 0xFF00)==0xFF00)
     {
        run=(char) (w & 0x00FF);
        fread(&w, 1, 2, f);
        for (j=0; j<run; j++)
            dest[n+j]=w;
        n+=run;
     }
     else
     {
        dest[n]=w;
        n++;
     }
  } while (n<len);
}

void CountUsedZones()
{ int i;

  i=255;
  while (i)
  {
     if (zones[i].script) break;
     if (zones[i].percent) break;
     if (zones[i].delay) break;
     if (zones[i].aaa) break;
     if (zones[i].entityscript) break;
     if (strlen(zones[i].name)) break;
     i--;
  }
  numzones=i+1;
}

void CountMoveScripts()
{ int i;

  i=99;
  while (i)
  {
     if (strlen(ms[i].t)) break;
     i--;
  }
  nms=i+1;
}

void CountCHRs()
{ int i;

  i=99;
  while (i)
  {
     if (strlen(chrlist[i].t)) break;
     i--;
  }
  nmchr=i+1;
}

void SaveMAP(char *fname)
{ FILE *f;
  int i, ofstbl[100], ct, t;

  memcpy(strbuf, "MAPù5", 6);
  f=fopen(fname, "wb");
  fwrite(strbuf, 1, 6, f);
  fwrite(strbuf, 1, 4, f);
  fwrite(vspname, 1, 60, f);
  fwrite(musname, 1, 60, f);
  fwrite(rstring, 1, 20, f);
  fwrite(&xstart, 1, 2, f);
  fwrite(&ystart, 1, 2, f);
  fwrite(&wrap, 1, 1, f);
  fwrite(strbuf, 1, 50, f);
  fwrite(&numlayers, 1, 1, f);
  for (i=0; i<numlayers; i++)
      fwrite(&layer[i], 1, 12, f);

  for (i=0; i<numlayers; i++)
  {
     cb=(char *) valloc(layer[i].sizex*layer[i].sizey*2,"cmprs buf", 0);
     WriteCompressedLayer2(layers[i],(layer[i].sizex*layer[i].sizey));
     fwrite(&bufsize, 1, 4, f);
     fwrite(cb, 1, bufsize, f);
     vfree(cb);
  }
  // Compress and write Obstruction data

  cb=(char *) valloc(layer[0].sizex*layer[0].sizey*2, "cmprs buf", 0);
  WriteCompressedLayer1(obstruct, (layer[0].sizex*layer[0].sizey));
  fwrite(&bufsize, 1, 4, f);
  fwrite(cb, 1, bufsize, f);
  WriteCompressedLayer1(zone, (layer[0].sizex*layer[0].sizey));
  fwrite(&bufsize, 1, 4, f);
  fwrite(cb, 1, bufsize, f);
  vfree(cb);

  CountUsedZones();
  fwrite(&numzones, 1, 4, f);
  fwrite(&zones, numzones, 50, f);

  CountCHRs();
  fwrite(&nmchr, 1, 1, f);
  fwrite(&chrlist, 60, nmchr, f);

  fwrite(&entities, 1, 1, f);
  fwrite(&entity, sizeof(entity)/256, entities, f);

  CountMoveScripts();
  fwrite(&nms, 1, 1, f);     // Calc offset buffer
  ct=0;
  for (i=0; i<nms; i++)
  {
    ofstbl[i]=ct;
    t=strlen(&ms[i].t)+1;
    ct+=t;
  }
  fwrite(&ct, 1, 4, f);        // string table length
  fwrite(&ofstbl, nms, 4, f);  // write offset buffer
  for (i=0; i<nms; i++)        // write string table
  {
    fwrite(&ms[i].t, 1, strlen(&ms[i].t)+1, f);
  }

  ct=0;  // 0 Things
  fwrite(&ct, 1, 4, f);
  ct=ftell(f);

  i=1; fwrite(&i, 1, 4, f);
  i=0; fwrite(&i, 1, 4, f);
  i=1; fwrite(&i, 1, 4, f);
  i=9; fwrite(&i, 1, 1, f);

  fseek(f,6,0);
  fwrite(&ct, 1, 4, f);
  fclose(f);
}

void LoadOldMAP(FILE *f)
{ int i, j, ct, ofstbl[100];
  char pm,pd,pc,c;

  // Reads a VERGE 1 format version 4 MAP file.
  fseek(f, 1, 0);
  fread(&vspname, 1, 13, f);
  fread(&musname, 1, 13, f);
  fread(&pc, 1, 1, f);             // }
  fread(&pm, 1, 1, f);             // } Parallax controls
  fread(&pd, 1, 1, f);             // }
  fread(strbuf, 1, 32, f);         // unused - misc
  fread(&xstart, 1, 2, f);
  fread(&ystart, 1, 2, f);
  fread(strbuf, 1, 2, f);          // unused - misc
  fread(&layer[0].sizex, 1, 2, f);
  fread(&layer[0].sizey, 1, 2, f);
  layer[1].sizex=layer[0].sizex;
  layer[1].sizey=layer[0].sizey;
  fread(strbuf, 1, 28, f);         // unused - header pad

  layers[0]=(unsigned short *) valloc((layer[0].sizex*(layer[0].sizey+1)*2)+2, "layer data", 0);
  layers[1]=(unsigned short *) valloc((layer[1].sizex*(layer[1].sizey+1)*2)+2, "layer data", 1);
   obstruct=(char *) valloc(layer[0].sizex*(layer[0].sizey+2), "obstruct map", 0);
       zone=(char *) valloc(layer[0].sizex*(layer[0].sizey+2), "zone map", 0);
         cb=(char *) valloc(layer[0].sizex*(layer[0].sizey+2), "comprs buf", 0);

  fread(layers[0], 2, layer[0].sizex * layer[0].sizey, f);
  fread(layers[1], 2, layer[1].sizex * layer[1].sizey, f);
  fread(cb,        1, layer[0].sizex * layer[0].sizey, f);

  // Convert MAP Properties layer to respective Zone and Obstruction layers

  for (j=0; j<layer[0].sizey; j++)
    for (i=0; i<layer[0].sizex; i++)
    {
      if ((cb[(j*layer[0].sizex)+i] & 1)==1)
         c=1; else c=0;
       obstruct[(j*layer[0].sizex)+i]=c;
       c=cb[(j*layer[0].sizex)+i] >> 1;
       zone[(j*layer[0].sizex)+i]=c;
    }
  vfree(cb);

  // Load and convert zone data records.

  for (i=0; i<128; i++)
  {
    fread(&tzone, 1, sizeof tzone, f);
    zones[i].script=tzone.callevent;
    zones[i].percent=tzone.percent;
    zones[i].delay=tzone.delay;
    zones[i].aaa=tzone.aaa;
    zones[i].entityscript=0;
    memcpy(zones[i].name, tzone.zonename, 16);
  }

  for (i=0; i<100; i++)                  // Load and convert CHR list
    fread(&chrlist[i].t, 1, 13, f);
//  DoCHRdealy();

  fread(&entities, 1, 1, f); fseek(f, 3, 1);
  memset(&entity, 0, sizeof entity);
  for (i=0; i<entities; i++)             // Load and convert entity records
  {
    fread(&oldent, 1, 88, f);
    entity[i].x=oldent.x;
    entity[i].y=oldent.y;
    entity[i].chrindex=oldent.chrindex-5;
    entity[i].obsmode1=oldent.obsmode^1;
    entity[i].obsmode2=1;
    entity[i].movecode=oldent.movecode;
    if (entity[i].movecode==3) entity[i].movecode=2;
    else if (entity[i].movecode==2) entity[i].movecode=3;
    entity[i].speed=oldent.speed;
    entity[i].face=oldent.face;
    entity[i].actm=oldent.activmode;
    entity[i].movescript=oldent.movescript;
    entity[i].step=oldent.step;
    entity[i].delay=oldent.delay;
    entity[i].data2=0;
    entity[i].data3=0;
    entity[i].data5=0;
    entity[i].data6=0;
    entity[i].actscript=oldent.actscript;
    if (entity[i].movecode==2) entity[i].data2=oldent.data3;
    if (entity[i].movecode==3)
    {
      entity[i].data2=oldent.x1;
      entity[i].data3=oldent.y1;
      entity[i].data5=oldent.x2;
      entity[i].data6=oldent.y2;
    }
    memcpy(entity[i].desc, oldent.entitydesc, 20);
  }

  fread(&nms, 1, 1, f);
  fread(&ct, 1, 4, f);
  fread(&ofstbl, nms, 4, f);
  ofstbl[nms]=ct;
  for (i=0; i<nms; i++)
   fread(&ms[i].t, 1, (ofstbl[i+1]-ofstbl[i]), f);
  fclose(f);

  numlayers=2;
  layertoggle[0]=1; layertoggle[1]=1; layertoggle[2]=0; layertoggle[3]=0;
  layertoggle[4]=0; layertoggle[5]=0; layertoggle[6]=0; layertoggle[7]=0;
  memcpy(rstring,"1E2",3);
  switch (pc)
  {
    case 0:
    case 1: layer[0].pmultx=1; layer[0].pmulty=1; layer[0].pdivx=1; layer[0].pdivy=1;
            layer[1].pmultx=1; layer[1].pmulty=1; layer[1].pdivx=1; layer[1].pdivy=1; break;
    case 2: layer[0].pmultx=pm; layer[0].pmulty=pm; layer[0].pdivx=pd; layer[0].pdivy=pd;
            layer[1].pmultx=1; layer[1].pmulty=1; layer[1].pdivx=1; layer[1].pdivy=1; break;
    case 3: layer[0].pmultx=1; layer[0].pmulty=1; layer[0].pdivx=1; layer[0].pdivy=1;
            layer[1].pmultx=pm; layer[1].pmulty=pm; layer[1].pdivx=pd; layer[1].pdivy=pd; break;
  }

  LoadVSP(vspname);
  sprintf(strbuf, "MapEd %s Copyright (C)1998 vecna", ME2_VERSION);
  Message(strbuf,700);
  Message("Old VERGE format 4 MAP loaded.",300);
  if (strlen(musname))
    PlayMusic(musname);
}

void LoadMAP(char *fname)
{ FILE *f;
  int i, ct, ofstbl[100];

  memcpy(strbuf, "MAPù5", 6);
  if (!(f=fopen(fname, "rb"))) errf("Could not find %s.",fname);
  fread(strbuf, 1, 6, f);
  if (strcmp(strbuf,"MAPù5"))
  {
     if (strbuf[0]==4)
     {
        LoadOldMAP(f);
        return;
     }
     errf("%s is not a recognized MAP file.",fname);
  }

  fread(&i, 1, 4, f);
  fread(vspname, 1, 60, f);
  fread(musname, 1, 60, f);
  fread(rstring, 1, 20, f);
  fread(&xstart, 1, 2, f);
  fread(&ystart, 1, 2, f);
  fread(&wrap, 1, 1, f);
  fread(strbuf, 1, 50, f);
  fread(&numlayers, 1, 1, f);
  for (i=0; i<numlayers; i++)
      fread(&layer[i], 1, 12, f);

  for (i=0; i<numlayers; i++)
  {
     fread(&bufsize, 1, 4, f);
     layers[i]=(unsigned short *) valloc(layer[i].sizex*(layer[i].sizey+2)*2, "layer data", i);
     ReadCompressedLayer2(layers[i],(layer[i].sizex * layer[i].sizey),f);
     layertoggle[i]=1;
  }
  while (i<10)
  {
     layertoggle[i]=0;
     i++;
  }
  obstruct=(char *) valloc(layer[0].sizex*(layer[0].sizey+2),"obstruct map", 0);
  zone=(char *) valloc(layer[0].sizex*(layer[0].sizey+2), "zone map", 0);

  fread(&bufsize, 1, 4, f);
  ReadCompressedLayer1(obstruct,(layer[0].sizex * layer[0].sizey), f);
  fread(&bufsize, 1, 4, f);
  ReadCompressedLayer1(zone,(layer[0].sizex * layer[0].sizey), f);

  memset(&zones, 0, sizeof zones);
  fread(&numzones, 1, 4, f);
  fread(&zones, numzones, 50, f);

  memset(&chrlist, 0, sizeof chrlist);
  fread(&nmchr, 1, 1, f);
  fread(&chrlist, 60, nmchr, f);
//  DoCHRdealy();

  fread(&entities, 1, 1, f);
  fread(&entity, sizeof(entity)/256, entities, f);

  fread(&nms, 1, 1, f);
  fread(&ct, 1, 4, f);
  fread(&ofstbl, nms, 4, f);
  ofstbl[nms]=ct;
  for (i=0; i<nms; i++)
    fread(&ms[i].t, 1, (ofstbl[i+1]-ofstbl[i]), f);
  fclose(f);

  LoadVSP(vspname);
  sprintf(strbuf, "MapEd %s Copyright (C)1998 vecna", ME2_VERSION);
  Message(strbuf,700);
  Message("V2 MAP loaded.",300);
  if (strlen(musname))
    PlayMusic(musname);
}

void InitTileIDX()
{ short i;

  memset(vadelay, 0, 200);
  for (i=0; i<MAXTILES; i++)
      tileidx[i]=i;
}

void CheckTimerStuff()
{ static inss=0;

  if (backupct>30000)
  {
    Message("Generating Auto-backup",150);
    SaveMAP("$$BACKUP.MAP");
    SaveVSP("$$BACKUP.VSP");
    backupct=0;
  }
  if (idlect>8000 && !inss)
  {
    inss=1;
    ScreenSaver();
    idlect=0;
    inss=0;
  }
}

extern int difficulty;

void SaveNewCFG()
{ FILE *f;

  f=fopen("maped.cfg","w");
  //if (nocdaudio) fprintf(f,"nocdaudio\n");
  fprintf(f,"vidmode %d \n", vm);
  fprintf(f,"pad %d \n", pad);
  fprintf(f,"scrollmode %d \n",scrollmode);
  fprintf(f,"mouse_scroll %d \n",mouse_scroll&1); // aen
  fprintf(f,"black %d \n",black);
  fprintf(f,"white %d \n",brightw);
  fprintf(f,"winbg %d \n",winbg);
  fprintf(f,"shadow %d \n",darkw);
  fprintf(f,"darkred %d \n",darkred);
  fprintf(f,"titlebg %d \n",titlebg);
  fprintf(f,"th %d \n", th);
  fprintf(f,"mh %d \n", mh);
  fprintf(f,"md_device %d \n", md_device==3?3:0);
  fprintf(f,"amxofs %d \n",amxofs);
  fprintf(f,"amyofs %d \n",amyofs);
  fprintf(f,"mmxofs %d \n",mmxofs);
  fprintf(f,"mmyofs %d \n",mmyofs);
  fprintf(f,"cnxofs %d \n",cnxofs);
  fprintf(f,"cnyofs %d \n",cnyofs);
  fprintf(f,"lmxofs %d \n",lmxofs);
  fprintf(f,"lmyofs %d \n",lmyofs);
  fprintf(f,"lvxofs %d \n",lvxofs);
  fprintf(f,"lvyofs %d \n",lvyofs);
  fprintf(f,"mpxofs %d \n",mpxofs);
  fprintf(f,"mpyofs %d \n",mpyofs);
  fprintf(f,"vaxofs %d \n",vaxofs);
  fprintf(f,"vayofs %d \n",vayofs);
  fprintf(f,"zexofs %d \n",zexofs);
  fprintf(f,"zeyofs %d \n",zeyofs);
  fprintf(f,"exofs %d \n",exofs);
  fprintf(f,"eyofs %d \n",eyofs);
  fprintf(f,"prxofs %d \n",prxofs);
  fprintf(f,"pryofs %d \n",pryofs);
  fprintf(f,"rsxofs %d \n",rsxofs);
  fprintf(f,"rsyofs %d \n",rsyofs);
  fprintf(f,"vced %s \n",vcedprog);
  fprintf(f,"difficulty %d \n",difficulty);
  fclose(f);
}

#include "controls.c"

void ShellToDOS()
{
  ShutdownVideo();
  ShutdownKeyboard();
  ShutdownTimer();
  MD_PlayStop();
  MD_Exit();

  system("COMMAND.COM");

  if (!MD_Init())
  {
    printf("Couldn't initialize sound: %s.\n", myerr);
    delay(500);
    soundokay=0;
  }
  if (strlen(musname)) PlayMusic(musname);
  InitVideo(vm);
  set_intensity(63);
  InitMouse();
  InitKeyboard();
  InitTimer();
}

void ShellVERGE()
{
  ShutdownVideo();
  ShutdownKeyboard();
  ShutdownTimer();
  MD_PlayStop();
  MD_Exit();

  system("VERGE");

  if (!MD_Init())
  {
    printf("Couldn't initialize sound: %s.\n", myerr);
    delay(500);
    soundokay=0;
  }
  if (strlen(musname)) PlayMusic(musname);
  InitVideo(vm);
  set_intensity(63);
  InitMouse();
  InitKeyboard();
  InitTimer();
}

void ShellMAP()
{
  ShutdownVideo();
  ShutdownKeyboard();
  ShutdownTimer();
  MD_PlayStop();
  MD_Exit();

  sprintf(strbuf,"verge %s",mapname);
  system(strbuf);

  if (!MD_Init())
  {
    printf("Couldn't initialize sound: %s.\n", myerr);
    delay(500);
    soundokay=0;
  }
  if (strlen(musname)) PlayMusic(musname);
  InitVideo(vm);
  set_intensity(63);
  InitMouse();
  InitKeyboard();
  InitTimer();
}

void ShellEditMAP()
{
  char fn[80],*p;

  key[SCAN_ALT]=0;
  key[SCAN_V]=0;
  ShutdownVideo();
  ShutdownKeyboard();
  ShutdownTimer();
  MD_PlayStop();
  MD_Exit();

  sprintf(fn,"%s",mapname);
  p=fn;
  while (*p)
  {
    if (*p=='.') *p=0;
    p++;
  }
  sprintf(strbuf,"%s %s.vc", vcedprog, fn);
  system(strbuf);

  if (!MD_Init())
  {
    printf("Couldn't initialize sound: %s.\n", myerr);
    delay(500);
    soundokay=0;
  }
  if (strlen(musname)) PlayMusic(musname);
  InitVideo(vm);
  set_intensity(63);
  InitMouse();
  InitKeyboard();
  InitTimer();
}

void ShellEditSystem()
{
  key[SCAN_ALT]=0;
  key[SCAN_S]=0;
  ShutdownVideo();
  ShutdownKeyboard();
  ShutdownTimer();
  MD_PlayStop();
  MD_Exit();

  sprintf(strbuf,"%s system.vc", vcedprog);
  system(strbuf);

  if (!MD_Init())
  {
    printf("Couldn't initialize sound: %s.\n", myerr);
    delay(500);
    soundokay=0;
  }
  if (strlen(musname)) PlayMusic(musname);
  InitVideo(vm);
  set_intensity(63);
  InitMouse();
  InitKeyboard();
  InitTimer();
}

char s[256];

void CompileAll()
{
  FILE *f;
  char *p;

  MD_PlayStop();
  MD_Exit();

  sprintf(strbuf,"vcc all q",mapname);
  system(strbuf);

  if (!MD_Init())
  {
    printf("Couldn't initialize sound: %s.\n", myerr);
    delay(500);
    soundokay=0;
  }
  if (strlen(musname)) PlayMusic(musname);

  if (!(f=fopen("error.txt","r")))
  {
    Message("All VC scripts sucessfully compiled.",300);
    return;
  }

  fgets(s,99,f);
  fclose(f);
  p=s;
  while (*p)
  {
    if (*p==13 || *p==10) *p=0;
    p++;
  }
  VCNotify(s);
}

void CompileMAP()
{
  FILE *f;
  char *p;

  MD_PlayStop();
  MD_Exit();

  sprintf(strbuf,"vcc %s q",mapname);
  system(strbuf);

  if (!MD_Init())
  {
    printf("Couldn't initialize sound: %s.\n", myerr);
    delay(500);
    soundokay=0;
  }
  if (strlen(musname)) PlayMusic(musname);

  if (!(f=fopen("error.txt","r"))) return;

  fgets(s,99,f);
  fclose(f);
  p=s;
  while (*p)
  {
    if (*p==13 || *p==10) *p=0;
    p++;
  }
  VCNotify(s);
}

void tickhandler(void)
{
  MP_HandleTick();
  MD_SetBPM(mp_bpm);
}

void RenderHighlight()
{ int zx, zy;
  int xw, yw;

  if (mh)
  {
    xw=xwin&15;
    yw=ywin&15;
    zx=((mx+xw)&~15)-xw;
    zy=((my+yw)&~15)-yw;

    DrawHighlight(zx-1, zy-1);
  }
}

int EntityThere(int xw, int yw)
{ int i;

  for (i=0; i<entities; i++)
  {
    if (entity[i].x==xw && entity[i].y==yw) return i+1;
  }
  return 0;
}

void AllocateEntity(int xw, int yw)
{
  memset(&entity[entities], 0, sizeof(entity) / 256);
  entity[entities].x=xw;
  entity[entities].y=yw;
  entities++;
}

void ProcessEntity(int xw, int yw)
{ int a;

  a=EntityThere(xw,yw);
  if (!a)
  {
     AllocateEntity(xw,yw);
     while (mb)
       ReadMouse(); // har! this was readmouse() @_@ <aen, apr 21>
     mb=0;
     return;
  }
  EntityEditor(a-1);
}

void DeleteEntity(int xw, int yw)
{ int i,a;
  char t[60];

  a=EntityThere(xw,yw)-1;
  sprintf(t,"Delete entity %d?",a);
  if (!Confirm(t)) return;
  for (i=a; i<entities; i++)
  {
    entity[i]=entity[i+1];
  }
  entities--;
}

int bxsize=130, bysize=170;
int bxofs=80, byofs=25;
int bmode=0;

void Browse(char n, char *m1, char *m2, char *m3, char *m4, char *dest)
{ struct find_t *f;
  char i, *d, mode;
  char *blah;
  int fc=0, ofs=0, cfs=0, a, b;
  char done=0, moving=0;
  int mxo, myo, cursorblink;
  char ctf=0, cb=0, c, t, *str;
  //int zx,zy,zw;
  char dragging=0;

  f=(struct find_t *) valloc(sizeof *f, "browse() tmp",0);
  blah=(char *) valloc(13*256,"browse()tmp2", 0);
  //memset(blah, 0, 13*256);
  n++;

  for (i=0; i<n; i++)
  {
    switch (i)
    {
      case 0: d="AJDLMMZZ.GAH"; mode=0xFF; break;
      case 1: d=m1; mode=0xFF; break;
      case 2: d=m2; mode=0xFF; break;
      case 3: d=m3; mode=0xFF; break;
      case 4: d=m4; mode=0xFF; break;
      default: err("uhh..."); break;
    }
    if (_dos_findfirst(d, mode, f)) continue;
    memcpy(blah+(fc*13), f->name, 13); fc++;
    while (!_dos_findnext(f))
    {
      memcpy(blah+(fc*13), f->name, 13); fc++;
    }
  }
  vfree(f);
  last_pressed=0;
  a=fc<22?117:2574/fc;
//  errn("%d",fc);
  do
  {
    if (!bmode)
    {
      RenderMap();
      RenderGUI();
    }
    else
    {
      RenderSelector();
      RenderMessages();
    }
    if (ctf && systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      bxofs = (mx-mxo);
      byofs = (my-myo);
      if (!mb) moving=0;
      if (bxofs<16) bxofs = 16;
      if (byofs<16) byofs = 16;
      if (bxofs+bxsize>335) bxofs = 336-bxsize;
      if (byofs+bysize>sy+16) byofs = (sy+16)-bysize;
    }

    Window(bxofs, byofs, bxofs+bxsize, byofs+bysize, "Browse");
    Button(bxofs+65, byofs+157, "OK");
    Button(bxofs+96, byofs+157, "Cancel");
    FilledBox(bxofs+20, byofs+15, 80, 135, black);
    FilledBox(bxofs+100, byofs+15, 10, 135, 2);
    HLine(bxofs+20, byofs+15, 90, darkw);
    VLine(bxofs+20, byofs+15, 135, darkw);
    VLine(bxofs+99, byofs+16, 134, white);
    VLine(bxofs+109, byofs+16, 134, white);
    HLine(bxofs+21, byofs+149, 89, white);
    stdwindow(bxofs+100, byofs+16, bxofs+109, byofs+25);
    stdwindow(bxofs+100, byofs+140, bxofs+109, byofs+149);
    b=byofs+24+(fc != 22 ? ((117-a)*ofs)/(fc-22) : 0);

    //draws the drag box
    stdwindow(bxofs+100, b, bxofs+109, b+a);

    FilledBox(bxofs+21, byofs+((cfs-ofs)*6)+16, 78, 6, darkw);

    // Render actual filename thingies
    i=0;
    while (1)
    {
      if (i+ofs<fc && i<22)
      {
        GotoXY(bxofs+22, byofs+17+(i*6));
        printstring(blah+((i+ofs)*13));
      }
      else break;
      i++;
    }

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase

    if(dragging&&!mb)
    {
      dragging=0;
    }

    if((mb==1&&mx>bxofs+100&&mx<bxofs+110&&my>byofs+25&&my<byofs+135)||dragging)
    {
      if(my>b&&my<b+a)
      {
        dragging=1;
      }
      else
      {
        if(my<b)
        {
          if (ofs<22) ofs=0;
          else ofs-=22;
          cfs=ofs;
        }
        if(my>b+a)
        {
          if (ofs+45>fc) ofs=fc-22;
          else ofs+=22;
          cfs=ofs;
        }
      }
    }
    if(!dragging)
    {
    if (mb==1 && mx>bxofs+20 && mx<bxofs+100 && my>byofs+15 && my<byofs+149)
    {
      i=(my-byofs-16)/6;
      i+=ofs;
      memcpy(dest,blah+(i*13),13);
      done=1;
      break;
    }
    if (mb==1 && mx>bxofs+100 && mx<bxofs+110 && my>byofs+15 && my<byofs+25)
    {
      WaitRelease();
      key[SCAN_UP]=1;
      last_pressed=SCAN_UP;
    }
    if (mb==1 && mx>bxofs+100 && mx<bxofs+110 && my>byofs+139 && my<byofs+149)
    {
      WaitRelease();
      key[SCAN_DOWN]=1;
      last_pressed=SCAN_DOWN;
    }
    if (mb==1 && mx>bxofs+65 && mx<bxofs+30+65 && my>byofs+157 && my<byofs+10+157 && !moving)
    {
      memcpy(dest, blah, 13); done=1;
      ButtonPressed(bxofs+65, byofs+157, "OK");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mb==1 && mx>bxofs+96 && mx<bxofs+30+96 && my>byofs+157 && my<byofs+10+157 && !moving)
    {
      *dest=0; done=1;
      ButtonPressed(bxofs+96, byofs+157, "Cancel");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mb==1 && mx>(bxofs+bxsize-9) && (mx<bxofs+bxsize-2)
    && my>(byofs+1) && (my<byofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mb==1 && mx>bxofs && mx<bxofs+bxsize && my>byofs && my<(byofs+9) && !moving)
    {
      moving=1;
      mxo=mx-bxofs;
      myo=my-byofs;
      continue;
    }

    if (mb==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
    }

    if (last_pressed)
    {
      if (key[SCAN_LSHIFT] || key[SCAN_RSHIFT]) c=key_shift_tbl[last_pressed];
      else c=key_ascii_tbl[last_pressed];

      if (last_pressed==SCAN_ESC)
      {
        key[SCAN_ESC]=0;
        done=1;
        break;
      }
      if (key[SCAN_PGUP])
      {
        if (ofs<22) ofs=0;
        else ofs-=22;
        cfs=ofs;
        key[SCAN_PGUP]=0;
        continue;
      }
      if (key[SCAN_PGDN])
      {
        if (ofs+45>fc) ofs=fc-22;
        else ofs+=22;
        if (fc<22) ofs=0;
        cfs=ofs;
        key[SCAN_PGDN]=0;
        continue;
      }
      if (key[SCAN_UP])
      {
        if (cfs) cfs--;
        if (ofs>cfs) ofs--;
        key[SCAN_UP]=0;
        last_pressed=0;
        continue;
      }
      if (key[SCAN_DOWN])
      {
        if (cfs<fc-1) cfs++;
        if (cfs-ofs>21) ofs++;
        key[SCAN_DOWN]=0;
        last_pressed=0;
        continue;
      }
      if (last_pressed==SCAN_ENTER)
      {
        memcpy(dest,blah+(cfs*13),13);
        key[SCAN_ENTER]=0;
        done=1;
        ctf=0;
        str=0;
        last_pressed=0;
        break;
      }

      if (last_pressed==SCAN_TAB)
      {
        switch (ctf)
        {
          case 0: ctf=0; str=0; break;
        }
        key[SCAN_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (!ctf)
        continue;

      if (last_pressed==SCAN_BACKSP && strlen(str))
      {
        str[strlen(str)-1]=0;
        key[SCAN_BACKSP]=0;
        last_pressed=0;
        continue;
      }
      else if (last_pressed==SCAN_BACKSP)
      {
        last_pressed=0;
        continue;
      }

      t=strlen(str);
      str[t]=c;
      str[t+1]=0;
      last_pressed=0;
      }
    }
  } while (!done);
  vfree(blah);
}

#include "vergepal.h"
#define LOGFILE "maped.log"

void Log(char *text, ...)
{
   va_list argptr;
   char msg[256];
   FILE *logf;

   va_start(argptr, text);
   vsprintf(msg, text, argptr);
   va_end(argptr);

   logf=fopen(LOGFILE,"aw");
   fprintf(logf,"%s\n",msg);
   fflush(logf);
   fclose(logf);
}

int main(int argc, char *argv[])
{

  remove(LOGFILE);
  md_mixfreq=44100;
  md_dmabufsize=5000;
  md_mode=DMODE_16BITS|DMODE_STEREO;
  md_device=0;
  memset(&chrs, 0, sizeof chrs);

  Log("GetConfig");
  GetConfig("maped.cfg");
  Log("LoadTransTbl");
  translucency_table=0; //zero
  LoadTransTable(); //zero

  Log("RegisterModLoaders");
  ML_RegisterLoader(&load_mod);
  ML_RegisterLoader(&load_s3m);
  ML_RegisterLoader(&load_xm);
  ML_RegisterLoader(&load_uni);

  Log("RegisterSndDrivers");
  MD_RegisterDriver(&drv_nos);
  MD_RegisterDriver(&drv_sb);
  MD_RegisterDriver(&drv_gus);
  MD_RegisterPlayer(tickhandler);
  soundokay=1;
  Log("Initialize snd lib");
  if (!MD_Init())
  {
    printf("Couldn't initialize sound: %s.\n", myerr);
    delay(500);
    soundokay=0;
  }

  //Log("Initialize CD audio");
  //CD_Init();

  Log("Initialize timer");
  InitTimer();
  Log("Initialize video");
  InitVideo(vm);
  Log("Initialize mouse");
  InitMouse();
  Log("Initialize keyboard IRQ handler");
  InitKeyboard();

  Log("Initializing map data");
  if (argc==1 && !Exist("untitled.map"))
  {
    SetPalette(vergepal);
    memcpy(pal, vergepal, 768);
    memcpy(mapname,"UNTITLED.MAP",13);
    memcpy(vspname,"UNTITLED.VSP",13);
    memcpy(rstring,"1E",2);
    numlayers=1;

    // aen: default newmap dimensions set to 100x100
    layer[0].pmultx=1;  layer[0].pmulty=1;
    layer[0].pdivx=1;   layer[0].pdivy=1;
    layer[0].sizex=100; layer[0].sizey=100;
    layer[0].trans=0;   layer[0].hline=0;

    layers[0]=(unsigned short *) valloc((layer[0].sizex*layer[0].sizey*2)+4,"layer data",0);
    //memset(layers[0],0,(layer[0].sizex*layer[0].sizey)*2);
    obstruct=(char *) valloc((layer[0].sizex*layer[0].sizey)+4,"obstruct map",0);
    zone=(char *) valloc((layer[0].sizex*layer[0].sizey)+4,"zone map", 0);

    // aen: default number of tiles set to 100
    numtiles=100;
    vsp=(char *) valloc(256 * numtiles,"vsp data", 0);
    wrap=0; el=0; layertoggle[0]=1;
    InitTileIDX();
  }
  else
  {
    if (argc==1) memcpy(mapname, "untitled.map",13);
    else memcpy(mapname, argv[1], strlen(argv[1]));
    LoadMAP(mapname);
    InitTileIDX();
 }
Log("Entering main loop");
thingy:
  while (!(key[SCAN_ALT] && key[SCAN_X]))
  {
    ProcessControls();
    while (tick)
    {
      tick--;
      PollMovement();
    }
    RenderMap();
    RenderHighlight();
    RenderGUI();
    ShowPage();
  }
  if (modified)
     if (!Confirm("Lose unsaved changes?"))
     {
       key[SCAN_ALT]=0;
       key[SCAN_X]=0;
       goto thingy;
     }

  ShutdownVideo();
  ShutdownKeyboard();
  ShutdownTimer();
  SaveNewCFG();
  remove("$$BACKUP.MAP");
  remove("$$BACKUP.VSP");

  //if (curtrack != 1) CD_Stop();
  //CD_DeInit(); // wasn't even valid! CD_Deinit() was incorrect <aen, apr 21>
  MD_PlayStop(); // Mordred
  ML_Free(mf);

  MD_Exit();
  if (translucency_table)
    vfree(translucency_table); //zero
  return 0;
}
