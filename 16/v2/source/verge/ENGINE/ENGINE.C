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
// + added ScreenShot() on F11
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


#define ENGINE_H
#include "verge.h"

// ================================= Data ====================================

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

zoneinfo zones[256];                 // zone data records
layer_r layer[6];                    // Array of layer data
vspanim_r vspanim[100];              // tile animation data
unsigned short vadelay[100];         // Tile animation delay ctr

char mapname[60];                    // MAP filename
char vspname[60];                    // VSP filemap
char musname[60];                    // MAP bkgrd music default filename
char rstring[20];                    // render-order string
char numlayers;                      // number of layers in map
byte *obstruct, *zone;               // obstruction and zone buffers
char layertoggle[8];                 // layer visible toggles
word xstart, ystart;                 // MAP start x/y location
int bufsize;                         // how many bytes need to be written
int numzones;                        // number of active zones

word *layers[6];                     // Raw layer data
int xwin=0, ywin=0;                  // camera offset

// -- vsp data --

byte *vsp=0,*vspmask;                // VSP data buffer.
unsigned short numtiles;             // number of tiles in VSP.
unsigned short *tileidx;             // tile index thingamajig
char *flipped;                       // bi-direction looping flag

// entity stuffs

char *msbuf[100];                    // ptr-table to script offset
char *ms;                            // script text buffer
byte nms;                            // number of movescripts

char numfollowers=0;                 // number of party followers
byte follower[10];                   // maximum of 10 followers.
char laststeps[10]={ 0 };            // record of last movements
int lastent;

// -- stuff --

byte movegranularity; // means nothing now, please remove
byte movectr=0;

byte phantom=0;                      // walk-through-walls
byte speeddemon=0;                   // doublespeed cheat
int bindarray[128];                  // bind script offset

// ================================= Code ====================================

void ReadCompressedLayer1(byte *dest, int len, char *buf)
{
  int j,n;
  byte run, w;

  n=0;
  do
  {
     w=*buf; buf++;
     if (w==0xFF)
     {
        run=*buf; buf++;
        w=*buf; buf++;
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

void ReadCompressedLayer2(word *dest, int len, word *buf)
{
  int j,n;
  byte run;
  word w;

  n=0;
  do
  {
    w=*buf; buf++;
    if ((w & 0xFF00)==0xFF00)
    {
      run=(w & 0x00FF);
      w=*buf; buf++;
      for(j=0; j<run; j++)
        dest[n+j]=w;
      n+=run;
    }
    else
    {
      dest[n]=w;
      n++;
    }
  } while(n<len);
}

void LoadVSP(char *fname)
{
  VFILE *f;
  int i;
  word ver;
  char *cb;

// Mwahaha! The Indefatigable Grue has snuck into the V2 source code! It is forever corrupted by his evil touch! Cower in fear, oh yes, FEAR! MwahahaHA..ha...hem...

  if (!(f=vopen(fname))) err("*error* Could not open VSP file %s.",fname);
  vread(&ver, 2, f);
  vread(&pal, 768, f);
  vread(&numtiles, 2, f);

  vsp=(byte *) valloc(numtiles*256, "vsp", OID_IMAGE);
  if (ver==2)
  {
    vread(vsp, (256*numtiles), f);
  }
  else
  {
    vread(&i, 4, f);
    cb=(char *) valloc(i, "LoadVSP:cb", OID_TEMP);
    vread(cb, i, f);
    ReadCompressedLayer1(vsp, 256*numtiles, cb);
    vfree(cb);
  }
  vread(&vspanim, sizeof vspanim, f);
  vclose(f);

  // Now calculate VSP transparency mask.

  vspmask=(byte *) valloc(numtiles*256, "vspmask", OID_MISC);
  for (i=0; i<(numtiles*256); i++)
  {
    if (vsp[i]) vspmask[i]=0;
    else vspmask[i]=255;
  }

  // Allocate and build tileidx.

  tileidx=(unsigned short *) valloc(numtiles*2, "tileidx", OID_MISC);
  for (i=0; i<numtiles; i++)
      tileidx[i]=i;

  flipped=(char *) valloc(numtiles, "flipped", OID_MISC);
  animate=TRUE;
}

void FreeVSP()
{
  animate=FALSE;
  vfree(vsp);
  vfree(vspmask);
  vfree(tileidx);
  vfree(flipped);
  vfree(mapvc);
}

void LoadMAP(char *fname)
{
  VFILE *f;
  char *cb;
  int i;

// No matter where you go, you're there.

  Logp("Loading MAP %s.",fname);
  memcpy(mapname, fname, strlen(fname)+1);
  memcpy(strbuf, "MAPù5", 6);
  if (!(f=vopen(fname))) err("Could not find %s.",fname);
  vread(strbuf, 6, f);
  if (strcmp(strbuf,"MAPù5"))
      err("%s is not a recognized MAP file.",fname);

// Lalala! Can you find Waldo hiding in the code? Here's a hint, he likes to dress like a candy-cane.

  vread(&i, 4, f);
  vread(vspname, 60, f);
  vread(musname, 60, f);
  vread(rstring, 20, f);
  vread(&xstart, 2, f);
  vread(&ystart, 2, f);
  vread(strbuf, 51, f);
  vread(&numlayers, 1, f);
  for (i=0; i<numlayers; i++)
      vread(&layer[i], 12, f);

  memset(&layertoggle, 0, 8);
  for (i=0; i<numlayers; i++)
  {
     vread(&bufsize, 4, f);
     layers[i]=(unsigned short *) valloc(layer[i].sizex*(layer[i].sizey+2)*2, "LoadMAP:layers[i]", OID_MAP);
     cb=(char *) valloc(bufsize, "LoadMAP:cb", OID_TEMP);
     vread(cb, bufsize, f);
     ReadCompressedLayer2(layers[i],(layer[i].sizex * layer[i].sizey), (word *) cb);
     vfree(cb);
     layertoggle[i]=1;
  }
  obstruct=(byte *) valloc(layer[0].sizex*(layer[0].sizey+2), "obstruct", OID_MAP);
  zone=(byte *) valloc(layer[0].sizex*(layer[0].sizey+2), "zone", OID_MAP);

  vread(&bufsize, 4, f);
  cb=(char *) valloc(bufsize, "LoadMAP:cb (2)", OID_TEMP);
  vread(cb, bufsize, f);
  ReadCompressedLayer1(obstruct,(layer[0].sizex * layer[0].sizey), cb);
  vfree(cb);
  vread(&bufsize, 4, f);
  cb=(char *) valloc(bufsize, "LoadMAP:cb (3)", OID_TEMP);
  vread(cb, bufsize, f);
  ReadCompressedLayer1(zone,(layer[0].sizex * layer[0].sizey), cb);
  vfree(cb);
  memset(&zones, 0, sizeof zones);
  vread(&numzones, 4, f);
  vread(&zones, numzones*50, f);

  memset(&chrlist, 0, sizeof chrlist);
  vread(&nmchr, 1, f);
  vread(&chrlist, 60*nmchr, f);

// Cheese is good, cheese is nice. Cheese is better, than body lice.

  memset(&entity, 0, sizeof entity);
  vread(&entities, 1, f);
  vread(&entity, (sizeof(entity)/256*entities), f);
  for (i=0; i<entities; i++)
  {
    entity[i].tx=entity[i].x;
    entity[i].ty=entity[i].y;
    entity[i].x*=16;
    entity[i].y*=16;
  }

  vread(&nms, 1, f);
  vread(&i, 4, f);
  vread(&msbuf, nms*4, f);
  if (nms)
  {
    ms=(char *) valloc(i, "LoadMAP:ms", OID_MAP);
    vread(ms, i, f);
  }
  else
  {
    vseek(f, i, 0);
    ms=(char *) malloc(16);
  }
  vread(&i, 4, f); // # of things
  LoadMapVC(f);
  vclose(f);

  memset(&chr, 0, sizeof chr);
  LoadVSP(vspname);
  LoadCHRList();
  Logp(" [%d] ",mapevents);
  if (strlen(musname))
    PlayMusic(musname);
  LogDone();
  ExecuteEvent(0);
}

void FreeMAP()
{
  int i;

  vfree(obstruct);
  vfree(zone);
  for (i=0; i<numlayers; i++)
    vfree(layers[i]);
  vfree(ms);
  player=0;
  memset(&entity, 0, sizeof entity);
  entities=0; nmchr=0; numchrs=0;
  xwin=0; ywin=0;
  vfree(mapvc);
}

void MAPswitch()
{
  hookretrace=0;
  hooktimer=0;
  kill=1;
  memcpy(startmap, (char *) args[1], strlen((char *) args[1]));
  key[SCAN_RQUOTA]=1;
  conlines=2;
}

// The_Edge rules. Really. I mean it. Wyrdwad too. They're as cool as Omniphile!

void MAPstats()
{ int a;

  Con_NextLine();
  Con_Printf("{||||||||||||||||||||}");
  sprintf(strbuf,"MAP stats for €%s~ - %d~ layers",mapname,numlayers); Con_Printf(strbuf);
  sprintf(strbuf,"Base dimensions %d~ x %d~", layer[0].sizex, layer[0].sizey);
  Con_Printf(strbuf);  a=layer[0].sizex*layer[0].sizey;
  sprintf(strbuf,"MAP using %d~ bytes of memory",
          a*(2+(numlayers*2))); Con_Printf(strbuf);
  sprintf(strbuf,"%d~ active zones.",numzones); Con_Printf(strbuf);
  Con_Printf("{||||||||||||||||||||}");
  sprintf(strbuf,"VSP file: €%s~",vspname); Con_Printf(strbuf);
  sprintf(strbuf,"VSP has %d~ tiles using %d~ bytes",numtiles,
          (numtiles*256)+800+(numtiles*3)); Con_Printf(strbuf);
  Con_Printf("{||||||||||||||||||||}");
}

int PlayerObstructed(char dir)
{
  if (phantom) return 0;
  switch (dir)
  {
    case 0: if (ObstructionAt(player->tx, player->ty+1)) return 1; break;
    case 1: if (ObstructionAt(player->tx, player->ty-1)) return 1; break;
    case 2: if (ObstructionAt(player->tx-1, player->ty)) return 1; break;
    case 3: if (ObstructionAt(player->tx+1, player->ty)) return 1; break;
  }
  switch (dir)
  {
    case 0: if (EntityObsAt(player->tx, player->ty+1)) return 1; break;
    case 1: if (EntityObsAt(player->tx, player->ty-1)) return 1; break;
    case 2: if (EntityObsAt(player->tx-1, player->ty)) return 1; break;
    case 3: if (EntityObsAt(player->tx+1, player->ty)) return 1; break;
  }
  return 0;
}

void CheckZone()
{
  int curzone;
  static int lz=0, zonedelay=0;

  curzone=zone[(player->ty * layer[0].sizex) + player->tx];
  if (lz!=curzone)
  {
    zonedelay=0;
    lz=curzone;
  }
  if (!zones[curzone].percent) return;
  if (zonedelay < zones[curzone].delay)
  {
    zonedelay++;
    return;
  }
  if (curzone)
    if (zones[curzone].script &&
        zones[curzone].percent >= rnd(0,255))
    {
       ExecuteEvent(zones[curzone].script);
       zonedelay=0;
    }
}

int InvFace()
{
  switch(player -> facing)
  {
    case 0: return 1;
    case 1: return 0;
    case 2: return 3;
    case 3: return 2;
  }
  return -1;
}

void Activate()
{
  byte tz;
  int ax=0, ay=0;

  switch(player->facing)
  {
    case 0: ax=player->tx; ay=player->ty+1; break;
    case 1: ax=player->tx; ay=player->ty-1; break;
    case 2: ax=player->tx-1; ay=player->ty; break;
    case 3: ax=player->tx+1; ay=player->ty; break;
  }
  tz=zone[(ay*layer[0].sizex)+ax];
  if (zones[tz].aaa)
  {
    ExecuteEvent(zones[tz].script);
    return;
  }
  if ((tz=EntityAt(ax, ay)))
  {
    tz--;
    if (entity[tz].face)
    {
      entity[tz].facing=InvFace();
      AnimateEntity(&entity[tz]);
    }
    if (entity[tz].actscript)
    {
      lastent=tz;
      ExecuteEvent(entity[tz].actscript);
    }
  }
}

void ResetFollowers()
{
  int i;

  player->x=player->tx<<4;
  player->y=player->ty<<4;
  player->moving=0;
  player->movecnt=0;
  player->reset=1;
  for (i=0; i<numfollowers; i++)
  {
    entity[follower[i]].x=player->x;
    entity[follower[i]].y=player->y;
    entity[follower[i]].tx=player->tx;
    entity[follower[i]].ty=player->ty;
    entity[follower[i]].facing=player->facing;
    entity[follower[i]].reset=1;
  }
  memset(laststeps, 0, 10);
}

void MoveFollowers()
{
  int i;

  for (i=0; i<numfollowers; i++)
  {
    entity[follower[i]].moving=laststeps[i+1];
    entity[follower[i]].movecnt=15;
    if (entity[follower[i]].reset ||
        entity[follower[i]].facing != laststeps[i+1]-1)
    {
      //player->animofs=chr[player->chrindex].uanim;
      entity[follower[i]].delayct=0;
      entity[follower[i]].reset=0;
    }
    entity[follower[i]].facing=laststeps[i+1]-1;
  }
}

//--- zero 5.7.99

void WritePalette(FILE *f)
{ char b;
  int i;
  byte pal3[768];

  for (i=0; i<768; i++)
      pal3[i]=pal2[i] << 2;

  b=12; fwrite(&b, 1, 1, f);
  fwrite(pal3, 1, 768, f);
}

void WritePCXLine(unsigned char *p,int len,FILE *pcxf)
{ int i;
  unsigned char byt, samect, repcode;

  i=0;
  do
  {   byt=p[i++];
      samect=1;
      while (samect<(unsigned) 63 && i<len && byt==p[i])
      {
         samect++;
         i++;
      }
      if (samect>1 || (byt & 0xC0) != 0)
      {
         repcode=0xC0 | samect;
         fwrite(&repcode,1,1,pcxf);
      }
      fwrite(&byt,1,1,pcxf);
  } while (i<len);
}


void ScreenShot()
{
  unsigned char b1;
  unsigned short int w1;
  int i,n;
  char fnamestr[13];
  static int ss=0;
  FILE *pcxf;

  n=0;
  do
  {
    sprintf(fnamestr,"%d.pcx",n);
    pcxf=fopen(fnamestr,"r");
    i=(int)pcxf;
    if(pcxf) fclose(pcxf);
    n++;
  } while(i);
  n--;

  // Takes a snapshot of the current screen.

   sprintf(fnamestr,"%d.pcx",n);

   pcxf=fopen(fnamestr,"wb");
   ss++;

// Write PCX header

   b1=10; fwrite(&b1, 1, 1, pcxf); // manufacturer always = 10
   b1=5; fwrite(&b1, 1, 1, pcxf);  // version = 3.0, >16 colors
   b1=1; fwrite(&b1, 1, 1, pcxf);  // encoding always = 1
   b1=8; fwrite(&b1, 1, 1, pcxf);  // 8 bits per pixel, for 256 colors
   w1=0; fwrite(&w1, 1, 2, pcxf);  // xmin = 0;
   w1=0; fwrite(&w1, 1, 2, pcxf);  // ymin = 0;
 w1=sx-1; fwrite(&w1, 1, 2, pcxf);  // xmax = 319;
 w1=sy-1; fwrite(&w1, 1, 2, pcxf);  // ymax = 199;
 w1=sx; fwrite(&w1, 1, 2, pcxf);  // hres = 320;
 w1=sy; fwrite(&w1, 1, 2, pcxf);  // vres = 200;

 fwrite(screen,1,48,pcxf);

 b1=0; fwrite(&b1, 1, 1, pcxf);   // reserved always = 0.
 b1=1; fwrite(&b1, 1, 1, pcxf);   // number of color planes. Just 1 for 8bit.
 w1=sx; fwrite(&w1, 1, 2, pcxf); // number of bytes per line

 w1=0; fwrite(&w1, 1, 1, pcxf);
 fwrite(screen, 1, 59, pcxf);          // filler

 for (w1=0; w1<sy; w1++)
     WritePCXLine(screen+w1*tsx, sx, pcxf);

 WritePalette(pcxf);
 fclose(pcxf);
// timer_count=0;
}
//---


void LastMove(char dir)
{
  laststeps[9]=laststeps[8];
  laststeps[8]=laststeps[7];
  laststeps[7]=laststeps[6];
  laststeps[6]=laststeps[5];
  laststeps[5]=laststeps[4];
  laststeps[4]=laststeps[3];
  laststeps[3]=laststeps[2];
  laststeps[2]=laststeps[1];
  laststeps[1]=laststeps[0];
  laststeps[0]=dir;

  MoveFollowers();
}

void ProcessControls1()
{
  if (!player->moving)
  {
    if (down && !PlayerObstructed(0))
    {
      player->ty++;
      player->moving=1;
      player->movecnt=15;
      player->y++;
      if (player->reset || player->facing != 0)
      {
        player->animofs=chr[player->chrindex].danim;
        player->delayct=0;
        player->reset=0;
      }
      player->facing=0;
      LastMove(1);
      return;
    }
    if (up && !PlayerObstructed(1))
    {
      player->ty--;
      player->moving=2;
      player->movecnt=15;
      player->y--;
      if (player->reset || player->facing != 1)
      {
        player->animofs=chr[player->chrindex].uanim;
        player->delayct=0;
        player->reset=0;
      }
      player->facing=1;
      LastMove(2);
      return;
    }
    if (left && !PlayerObstructed(2))
    {
      player->tx--;
      player->moving=3;
      player->movecnt=15;
      player->x--;
      if (player->reset || player->facing != 2)
      {
        player->animofs=chr[player->chrindex].lanim;
        player->delayct=0;
        player->reset=0;
      }
      player->facing=2;
      LastMove(3);
      return;
    }
    if (right && !PlayerObstructed(3))
    {
      player->tx++;
      player->moving=4;
      player->movecnt=15;
      player->x++;
      if (player->reset || player->facing != 3)
      {
        player->animofs=chr[player->chrindex].ranim;
        player->delayct=0;
        player->reset=0;
      }
      player->facing=3;
      LastMove(4);
      return;
    }
    if (down) player->facing=0;
    if (up) player->facing=1;
    if (left) player->facing=2;
    if (right) player->facing=3;
  }

  switch (player->moving)
  {
    case 0: player->reset=1; player->animofs=0; player->delayct=0; break;
    case 1: player->y++; player->movecnt--; break;
    case 2: player->y--; player->movecnt--; break;
    case 3: player->x--; player->movecnt--; break;
    case 4: player->x++; player->movecnt--; break;
  }
  if (!player->movecnt && player->moving)
  {
    player->moving=0;
    CheckZone();
  }
  if (!player->movecnt && b1) Activate();
}

void ProcessControls()
{ int i;

  if (key[SCAN_RQUOTA])
  {
    key[SCAN_RQUOTA]=0; last_pressed=0;
    ActivateConsole();
  }
  if (key[SCAN_ALT] && key[SCAN_X]) err("Exiting: ALT-X pressed.");
//--- zero 5.7.99
  if (key[SCAN_F11])
  {
    Message("ss",50);
    ScreenShot();
    key[SCAN_F11]=0;
  }
//---
  if (!player) return;

  if (player->speed<4)
  {
    switch (player->speed)
    {
      case 1: if (player->speedct<3) { player->speedct++; return; } break;
      case 2: if (player->speedct<2) { player->speedct++; return; } break;
      case 3: if (player->speedct<1) { player->speedct++; return; } break;
    }
  }
  if (player->speed<5)
  {
    ProcessControls1();
    player->speedct=0;
    AnimateEntity(player);
  }
  switch (player->speed)
  {
    case 5: for (i=0; i<2; i++) { ProcessControls1(); AnimateEntity(player); } return;
    case 6: for (i=0; i<3; i++) { ProcessControls1(); AnimateEntity(player); } return;
    case 7: for (i=0; i<4; i++) { ProcessControls1(); AnimateEntity(player); } return;
  }
}

void GameTick()
{
  UpdateControls();
  CheckMessages();
  if (bindarray[last_pressed])
    HookKey(bindarray[last_pressed]);
  ProcessControls();
  if (speeddemon && key[SCAN_CTRL]) ProcessControls();
  ProcessEntities();
}

