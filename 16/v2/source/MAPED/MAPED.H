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

#ifndef MAPED_INC
#define MAPED_INC

extern const char *const ME2_VERSION;

#include <stdio.h>

#define byte unsigned char
#define word unsigned short

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

typedef struct {
  char t[200];
} movescript;

extern layer_r layer[4];
extern vspanim_r vspanim[100];
extern zoneinfo zones[256];
extern unsigned short vadelay[100];
extern entity_r entity[256];              // Entity records.
extern byte entities;                     // number of allocated entities.
extern chrlist_r chrlist[100];
extern byte nmchr;
extern movescript ms[100];                  // move scripts arrays
extern byte nms;                          // number of movement scripts

extern char *obstruct, *zone, curzone;
extern char numlayers, wrap;
extern unsigned short *layers[6];
extern char rstring[20];
extern char mapname[60], vspname[60];
extern char musname[60];
extern short xstart, ystart;

// -- vsp related data --

extern unsigned short numtiles;
extern unsigned char *vsp;

// -- editing related data --

extern int xwin, ywin, bmode;
extern char mouse_scroll;
extern short lt, rt;
extern char layertoggle[10], el, modified;

extern word *copybuf;
extern int copybuf_wide,copybuf_deep;
extern int selx1, sely1, selx2, sely2;
extern int shifted, pasting;

extern int numzones;                      // Number of active zones.

extern int moving_up,moving_down,moving_left,moving_right;

extern void LoadTransTable();
extern int  Exist(char *fname);
extern void err(char *str, ...);
extern void errf(char *str, char *str1);
extern void errn(char *str, int n);
extern int  random(int min, int max);
extern void PlayMusic(char *fname);
extern void StopMusic();
extern void LoadVSP(char *fname);
extern void SaveVSP(char *fname);
extern void EmitC (char c);
extern void EmitW (short int w);
extern void WriteCompressedLayer1(unsigned char *p, int len);
extern void ReadCompressedLayer1(unsigned char *dest, int len, FILE *f);
extern void WriteCompressedLayer2(unsigned short *p,int len);
extern void ReadCompressedLayer2(unsigned short *dest, int len, FILE *f);
extern void CountUsedZones();
extern void CountMoveScripts();
extern void CountCHRs();
extern void SaveMAP(char *fname);
extern void LoadOldMAP(FILE *f);
extern void LoadMAP(char *fname);
extern void InitTileIDX();
extern void CheckTimerStuff();
extern void SaveNewCFG();
extern void ShellToDOS();
extern void ShellVERGE();
extern void ShellMAP();
extern void ShellEditMAP();
extern void ShellEditSystem();
extern void CompileAll();
extern void CompileMAP();
extern void tickhandler(void);
extern void RenderHighlight();
extern int EntityThere(int xw, int yw);
extern void AllocateEntity(int xw, int yw);
extern void ProcessEntity(int xw, int yw);
extern void DeleteEntity(int xw, int yw);
extern void Browse(char n, char *m1, char *m2, char *m3, char *m4, char *dest);

// Includes for a_memory.c

void *valloc(int amount, char *desc, int owner);
void *qvalloc(int amount);
int  vfree(void *pointer);
void qvfree(void *pointer);
void FreeByOwner(int owner);
void MemReport(void);
void CheckCorruption(void);

#define free ERROR_YEAH_REPLACE_THIS_WITH_VFREE

#endif // MAPED_INC
