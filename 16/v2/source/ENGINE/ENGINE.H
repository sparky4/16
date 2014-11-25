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
// CHANGELOG:
// <zero 5.7.99>
// + added ScreenShot() headers
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


#ifndef ENGINE_H
#define ENGINE_H

typedef struct
{
  word start;                        // strand start
  word finish;                       // strand end
  word delay;                        // tile-switch delay
  word mode;                         // tile-animation mode
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

extern layer_r layer[4];                    // Array of layer data
extern vspanim_r vspanim[100];              // tile animation data
extern unsigned short vadelay[100];         // Tile animation delay ctr

extern char mapname[60];                    // MAP filename
extern char vspname[60];                    // VSP filemap
extern char rstring[20];                    // render-order string
extern char numlayers;                      // number of layers in map
extern byte *obstruct, *zone;               // obstruction and zone buffers
extern int bufsize;                         // how many bytes need to be written
extern char layertoggle[4];                 // layer visible toggles

extern word *layers[4];                     // Raw layer data
extern int xwin, ywin;

// -- entity things --

extern char *msbuf[100];                    // ptr-table to script offset
extern char *ms;                            // script text buffer
extern byte nms;                            // number of movescripts

extern char numfollowers;                   // number of party followers
extern byte follower[10];                   // maximum of 10 followers.
extern char laststeps[10];                  // record of last movements
extern int lastent;

// -- vsp data --

extern byte *vsp,*vspmask;                  // VSP data buffer.
extern unsigned short numtiles;             // number of tiles in VSP.
extern unsigned short *tileidx;             // tile index thingamajig
extern char *flipped;                       // bi-direction looping flag
extern byte movegranularity, phantom, speeddemon, movectr;
extern int bindarray[128];

// -- prototypes --

extern void LoadVSP(char *fname);
extern void FreeVSP(void);
extern void LoadMAP(char *fname);
extern void FreeMAP(void);
extern void MAPswitch(void);
extern void MAPstats(void);
extern void ProcessControls(void);
extern void GameTick(void);

extern void ReadCompressedLayer1(byte*, int, char*);

//--- zero 5.7.99
void WritePalette(FILE *f);
void WritePCXLine(unsigned char *p,int len,FILE *pcxf);
void ScreenShot();
//
#endif // ENGINE_H
