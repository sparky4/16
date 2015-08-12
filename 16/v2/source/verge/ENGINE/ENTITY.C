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
// <zero, 5.6.98>
// + corrected oversight in movement script management by sticking a hack in
//   MoveScript().  Bug caused Fx commands to not work sometimes.
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#define ENTITY_H
#include "verge.h"

// ================================= Data ====================================

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
  char *animofs, *scriptofs;           // anim script | move script
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
  byte *imagedata;                   // CHR frame data
  int fxsize, fysize;                // frame x/y dimensions
  int hx, hy;                        // x/y obstruction hotspot
  int totalframes;                   // total # of frames.
  int lidle, ridle;
  int uidle, didle;
  char lanim[100];
  char ranim[100];
  char uanim[100];
  char danim[100];
} chrdata;

typedef struct
{
  char t[60];
} chrlist_r;

chrlist_r chrlist[100];
byte nmchr,playernum;
entity_r *player=0;
entity_r entity[256];
byte entities=0;
chrdata chr[100];
byte numchrs=0;
byte entidx[256], cc;
byte movesuccess;

// ================================= Code ====================================

int ObstructionAt(int tx, int ty)
{
  if (obstruct[(ty*layer[0].sizex)+tx]) return 1;
  if (tx==-1 || ty==-1) return 1;
  if (tx==layer[0].sizex || ty==layer[0].sizey) return 1;
  return 0;
}

int Zone(int tx, int ty)
{
  return zone[(ty*layer[0].sizex)+tx];
}

void LoadCHR(char *fname, chrdata *c)
{
  VFILE *f;
  char b, *ptr=0;
  int i;

  f=vopen(fname);
  if (!f) err("Could not open CHR file %s.",fname);
  vread(&b, 1, f);
  if (b!=2) err("CHR %s incorrect CHR format version.",fname);
  vread(&c->fxsize, 2, f);
  vread(&c->fysize, 2, f);
  vread(&c->hx, 2, f);
  vread(&c->hy, 2, f);
  vread(strbuf, 4, f);          // skip the hotspot size.
  vread(&c->totalframes, 2, f);

  vread(&i, 4, f);
  ptr=(char *) valloc(i, "LoadCHR:ptr", OID_TEMP);
  vread(ptr, i, f);
  c->imagedata=(byte *) valloc(c->fxsize * c->fysize * c->totalframes, "LoadCHR:c->imagedata", OID_IMAGE);
  ReadCompressedLayer1(c->imagedata, c->fxsize * c->fysize * c->totalframes, ptr);
  vfree(ptr);


  vread(&c->lidle, 4, f);
  vread(&c->ridle, 4, f);
  vread(&c->uidle, 4, f);
  vread(&c->didle, 4, f);

  for (b=0; b<4; b++)
  {
    switch (b)
    {
      case 0: ptr=c->lanim; break;
      case 1: ptr=c->ranim; break;
      case 2: ptr=c->uanim; break;
      case 3: ptr=c->danim; break;
    }
    vread(&i, 4, f);
    if (i>100) err("Animation strand too long. %d",i);
    vread(ptr, i, f);
  }
  vclose(f);
}

int CacheCHR(char *fname)
{
  LoadCHR(fname, &chr[numchrs]);
  numchrs++;
  return (numchrs-1);
}

void FreeCHRList()
{
  int i;

  for (i=0; i<numchrs; i++)
    vfree(chr[i].imagedata);
  memset(chr, i, sizeof chr);
}

void LoadCHRList()
{
  int i;

  for (i=0; i<nmchr; i++)
      if (strlen(chrlist[i].t)) CacheCHR(chrlist[i].t);
}

void DrawEntity(int i)
{
  int a, b, dx, dy;

  dx=entity[i].x-xwin;
  dy=entity[i].y-ywin;
  a=entity[i].chrindex;
  if (a>=numchrs) return;
  b=entity[i].specframe ? entity[i].specframe : entity[i].frame;
  TCopySpriteClip(dx-chr[a].hx, dy-chr[a].hy, chr[a].fxsize, chr[a].fysize,
                  (byte *) (chr[a].imagedata+(b*chr[a].fxsize*
                  chr[a].fysize)));
}

static int cmpent(const void* a, const void* b)
{
  return entity[*(byte*)a].y - entity[*(byte*)b].y;
}

void RenderEntities()
{
  int i;

  qsort(entidx, cc, 1, cmpent);
  for (i=0; i<cc; i++) DrawEntity(entidx[i]);
}

int GetArg(entity_r *p)
{
  int j;
  static char token[10];

  j=0;
  while (*p->animofs==' ') p->animofs++;
  while (*p->animofs>=48 && *p->animofs<=57)
  {
    token[j]=*p->animofs;
    p->animofs++;
    j++;
  }
  token[j]=0;
  return atoi(token);
}

void GetNextCommand(entity_r *p)
{
  byte s;

  while (*p->animofs==' ') p->animofs++;
  s=*p->animofs;
  p->animofs++;
  switch (s)
  {
    case 'F': p->frame=GetArg(p); break;
    case 'W': p->delayct=GetArg(p); break;
    case   0: if (p->moving)
                switch (p->facing)
                {
                  case 0: p->animofs=chr[p->chrindex].danim; break;
                  case 1: p->animofs=chr[p->chrindex].uanim; break;
                  case 2: p->animofs=chr[p->chrindex].lanim; break;
                  case 3: p->animofs=chr[p->chrindex].ranim; break;
                }
              else
                switch (p->facing)
                {
                  case 0: p->animofs=0; p->frame=chr[p->chrindex].didle; break;
                  case 1: p->animofs=0; p->frame=chr[p->chrindex].uidle; break;
                  case 2: p->animofs=0; p->frame=chr[p->chrindex].lidle; break;
                  case 3: p->animofs=0; p->frame=chr[p->chrindex].ridle; break;
                }
              p->delayct=0; break;
  }
}

void AnimateEntity(entity_r *p)
{
  if (!p->animofs && p->moving)
  {
     switch (p->facing)
     {
       case 0: p->animofs=chr[p->chrindex].danim; break;
       case 1: p->animofs=chr[p->chrindex].uanim; break;
       case 2: p->animofs=chr[p->chrindex].lanim; break;
       case 3: p->animofs=chr[p->chrindex].ranim; break;
     }
     p->delayct=0;
  }
  if (!p->animofs && !p->moving)
  {
     switch (p->facing)
     {
       case 0: p->animofs=0; p->frame=chr[p->chrindex].didle; break;
       case 1: p->animofs=0; p->frame=chr[p->chrindex].uidle; break;
       case 2: p->animofs=0; p->frame=chr[p->chrindex].lidle; break;
       case 3: p->animofs=0; p->frame=chr[p->chrindex].ridle; break;
     }
     p->delayct=0;
  }
  if (p->delayct)
  {
    p->delayct--;
  }
  else GetNextCommand(p);
}

int EntityAt(int ex, int ey)
{ int i;

  for (i=0; i<cc; i++)
  {
    if (&entity[entidx[i]]==player) continue;
    if (ex==entity[entidx[i]].tx && ey==entity[entidx[i]].ty)
        return entidx[i]+1;
  }
  return 0;
}

int EntityObsAt(int ex, int ey)
{ int i;

  for (i=0; i<cc; i++)
  {
    if (&entity[entidx[i]]==player) continue;
    if (ex==entity[entidx[i]].tx && ey==entity[entidx[i]].ty &&
        entity[entidx[i]].obsmode2)
        return entidx[i]+1;
  }
  return 0;
}

int AEntityObsAt(int ex, int ey)
{ int i;

  for (i=0; i<cc; i++)
  {
    if (ex==entity[entidx[i]].tx && ey==entity[entidx[i]].ty &&
        entity[entidx[i]].obsmode2)
        return entidx[i]+1;
  }
  return 0;
}

void SiftEntities()
{ int i, dx, dy;

  memset(&entidx, 0, 256); cc=0;
  for (i=0; i<entities; i++)
  {
    dx=entity[i].x-xwin+16;
    dy=entity[i].y-ywin+16;

    if (dx<0 || dx>sx+chr[entity[i].chrindex].fxsize) continue;
    if (dy<0 || dy>sy+chr[entity[i].chrindex].fysize) continue;
    entidx[cc]=i; cc++;
  }
}

void MoveRight(int i)
{
  int tx,ty;

  tx=entity[i].tx+1; ty=entity[i].ty;
  if (entity[i].obsmode1 && (ObstructionAt(tx,ty) || AEntityObsAt(tx,ty)))
     { movesuccess=0; return; }
  if (entity[i].facing!=3)
  {
    entity[i].delayct=0;
    entity[i].animofs=0;
  }
  entity[i].x++; entity[i].facing=3; entity[i].moving=4; entity[i].ctr=15;
  entity[i].tx++; movesuccess=1; entity[i].reset=0;
}

void MoveLeft(int i)
{
  int tx, ty;

  tx=entity[i].tx-1; ty=entity[i].ty;
  if (entity[i].obsmode1 && (ObstructionAt(tx,ty) || AEntityObsAt(tx,ty)))
     { movesuccess=0; return; }

  if (entity[i].facing!=2)
  {
    entity[i].delayct=0;
    entity[i].animofs=0;
  }
  entity[i].x--; entity[i].facing=2; entity[i].moving=3; entity[i].ctr=15;
  entity[i].tx--; movesuccess=1; entity[i].reset=0;
}

void MoveUp(int i)
{
  int tx, ty;

  tx=entity[i].tx; ty=entity[i].ty-1;
  if (entity[i].obsmode1 && (ObstructionAt(tx,ty) || AEntityObsAt(tx,ty)))
     { movesuccess=0; return; }
  if (entity[i].facing!=1)
  {
    entity[i].delayct=0;
    entity[i].animofs=0;
  }
  entity[i].y--; entity[i].facing=1; entity[i].moving=2; entity[i].ctr=15;
  entity[i].ty--; movesuccess=1; entity[i].reset=0;
}

void MoveDown(int i)
{
  int tx, ty;

  tx=entity[i].tx; ty=entity[i].ty+1;
  if (entity[i].obsmode1 && (ObstructionAt(tx,ty) || AEntityObsAt(tx,ty)))
     { movesuccess=0; return; }
  if (entity[i].facing!=0)
  {
    entity[i].delayct=0;
    entity[i].animofs=0;
  }
  entity[i].y++; entity[i].facing=0; entity[i].moving=1; entity[i].ctr=15;
  entity[i].ty++; movesuccess=1; entity[i].reset=0;
}

void Wander1(int i)
{
  if (!entity[i].data1)
  {
    entity[i].data2=rnd(0,3);
    entity[i].data1=entity[i].step+1;
  }
  if (entity[i].data1==1)
  {
    entity[i].delayctr++;
    if (entity[i].delayctr>=entity[i].delay)
       entity[i].data1=0;
    return;
  }
  if (entity[i].data1>1)
  {
    switch(entity[i].data2)
    {
      case 0: MoveUp(i); break;
      case 1: MoveDown(i); break;
      case 2: MoveLeft(i); break;
      case 3: MoveRight(i); break;
    }
    entity[i].data1--;
    if (entity[i].data1==1)
    {
      entity[i].delayctr=0;
      entity[i].animofs=0;
      entity[i].delayct=0;
    }
  }
}

void Wander2(int i)
{
  if (!entity[i].data1)
  {
    entity[i].data3=rnd(0,3);
    entity[i].data1=entity[i].step+1;
  }
  if (entity[i].data1==1)
  {
    entity[i].delayctr++;
    if (entity[i].delayctr>=entity[i].delay)
       entity[i].data1=0;
    return;
  }
  if (entity[i].data1>1)
  {
    switch(entity[i].data3)
    {
      case 0: if (Zone(entity[i].tx,entity[i].ty-1)==entity[i].data2) MoveUp(i); break;
      case 1: if (Zone(entity[i].tx,entity[i].ty+1)==entity[i].data2) MoveDown(i); break;
      case 2: if (Zone(entity[i].tx-1,entity[i].ty)==entity[i].data2) MoveLeft(i); break;
      case 3: if (Zone(entity[i].tx+1,entity[i].ty)==entity[i].data2) MoveRight(i); break;
     }
    entity[i].data1--;
    if (entity[i].data1==1) entity[i].delayctr=0;
  }
}

void Wander3(int i)
{
  if (!entity[i].data1)
  {
    entity[i].data2=rnd(0,3);
    entity[i].data1=entity[i].step+1;
  }
  if (entity[i].data1==1)
  {
    entity[i].delayctr++;
    if (entity[i].delayctr>=entity[i].delay)
       entity[i].data1=0;
    return;
  }
  if (entity[i].data1>1)
  {
    switch(entity[i].data2)
    {
      case 0: if (entity[i].ty>entity[i].data3) MoveUp(i); break;
      case 1: if (entity[i].ty<entity[i].data6) MoveDown(i); break;
      case 2: if (entity[i].tx>entity[i].data2) MoveLeft(i); break;
      case 3: if (entity[i].tx<entity[i].data5) MoveRight(i); break;
    }
    entity[i].data1--;
    if (entity[i].data1==1) entity[i].delayct=0;
  }
}

void Whitespace(int i)
{
  while (*entity[i].scriptofs==' ')
    entity[i].scriptofs++;
}

void GetArgMS(int i)
{ int j;
  char token[10];

  j=0;
  Whitespace(i);
  while (*entity[i].scriptofs>=48 && *entity[i].scriptofs<=57)
  {
    token[j]=*entity[i].scriptofs;
    entity[i].scriptofs++;
    j++;
  }
  token[j]=0;
  entity[i].data1=atoi(token);
}

void GetNextCommandMS(int i)
{ unsigned char s;

  Whitespace(i);
  s=*entity[i].scriptofs;
  entity[i].scriptofs++;
  switch (s)
  {
    case 'U': entity[i].mode=1; GetArgMS(i); break;
    case 'D': entity[i].mode=2; GetArgMS(i); break;
    case 'L': entity[i].mode=3; GetArgMS(i); break;
    case 'R': entity[i].mode=4; GetArgMS(i); break;
    case 'S': entity[i].mode=5; GetArgMS(i); break;
    case 'W': entity[i].mode=6; GetArgMS(i); entity[i].animofs=0;
              entity[i].delayct=0; break;
    case 0:   switch (entity[i].facing)
              {
                case 0: entity[i].animofs=0; entity[i].frame=chr[entity[i].chrindex].didle; break;
                case 1: entity[i].animofs=0; entity[i].frame=chr[entity[i].chrindex].uidle; break;
                case 2: entity[i].animofs=0; entity[i].frame=chr[entity[i].chrindex].lidle; break;
                case 3: entity[i].animofs=0; entity[i].frame=chr[entity[i].chrindex].ridle; break;
              }
              entity[i].movecode=0; entity[i].mode=7; entity[i].data1=0;
              entity[i].scriptofs=0; entity[i].delayct=0; break;
    case 'C': entity[i].mode=8; GetArgMS(i); break;
    case 'B': entity[i].mode=9; break;
    case 'X': entity[i].mode=10; GetArgMS(i); break;
    case 'Y': entity[i].mode=11; GetArgMS(i); break;
    case 'F': entity[i].mode=12; GetArgMS(i); break;
    case 'Z': entity[i].mode=13; GetArgMS(i); break;
    default: err("Invalid entity movement script.");
  }
}

void MoveScript(int i)
{
  if (!entity[i].scriptofs) entity[i].scriptofs=(char *) (int) ms+(int) msbuf[entity[i].movescript];
  if (!entity[i].mode) GetNextCommandMS(i);

  switch(entity[i].mode)
  {
    case 1: MoveUp(i); if (movesuccess) entity[i].data1--; break;
    case 2: MoveDown(i); if (movesuccess) entity[i].data1--; break;
    case 3: MoveLeft(i); if (movesuccess) entity[i].data1--; break;
    case 4: MoveRight(i); if (movesuccess) entity[i].data1--; break;
    case 5: entity[i].speed=entity[i].data1; entity[i].data1=0; break;
    case 6: entity[i].data1--; break;
    case 7: return;
    case 8: ExecuteEvent(entity[i].data1); entity[i].data1=0; break;
    case 9: entity[i].scriptofs=(char *) (int) ms+(int) msbuf[entity[i].movescript];
            entity[i].data1=0; break;
    case 10: if (entity[i].tx<entity[i].data1) MoveRight(i);
             if (entity[i].tx>entity[i].data1) MoveLeft(i);
             if (entity[i].tx==entity[i].data1) entity[i].data1=0; break;
             break;
    case 11: if (entity[i].ty<entity[i].data1) MoveDown(i);
             if (entity[i].ty>entity[i].data1) MoveUp(i);
             if (entity[i].ty==entity[i].data1) entity[i].data1=0; break;
             break;
    case 12: entity[i].facing=entity[i].data1;
             entity[i].data1=0;
             //--- zero 5.6.99
             switch(entity[i].facing)
             {
               case 0: entity[i].frame=chr[entity[i].chrindex].didle; break;
               case 1: entity[i].frame=chr[entity[i].chrindex].uidle;        break;
               case 2: entity[i].frame=chr[entity[i].chrindex].lidle; break;
               case 3: entity[i].frame=chr[entity[i].chrindex].ridle; break;
             }
             //---
             break;
    case 13: entity[i].specframe=entity[i].data1;
             entity[i].data1=0; break;
  }
  if (!entity[i].data1) entity[i].mode=0;
}

void TestActive(int i)
{
  int dx, dy;

  dx=abs(entity[i].x - player->x);
  dy=abs(entity[i].y - player->y);
  if ((dx<=16 && dy<=3) || (dx<=3 && dy<=16))
  {
    if (!entity[i].expand4 && !invc)
    {
      entity[i].expand4=1;
      ExecuteEvent(entity[i].actscript);
    }
  }
  else
    entity[i].expand4=0;
}

void ProcessEntity1(int i)
{
  entity[i].speedct=0;
  if (entity[i].actm) TestActive(i);

  if (!entity[i].moving)
  {
     switch(entity[i].movecode)
     {
       case 0: return;
       case 1: Wander1(i); break;
       case 2: Wander2(i); break;
       case 3: Wander3(i); break;
       case 4: MoveScript(i); break;
       default: err("unknown entity movement pattern.");
     }
     if (!entity[i].reset) return;
  }

  if (entity[i].reset)
  {
    entity[i].animofs=0;
    entity[i].delayct=0;
    entity[i].reset=0;
  }

  if (entity[i].moving)
  {
      if (entity[i].moving==1)
      { entity[i].y++; entity[i].ctr--; AnimateEntity(&entity[i]); }
      if (entity[i].moving==2)
      { entity[i].y--; entity[i].ctr--; AnimateEntity(&entity[i]); }
      if (entity[i].moving==4)
      { entity[i].x++; entity[i].ctr--; AnimateEntity(&entity[i]); }
      if (entity[i].moving==3)
      { entity[i].x--; entity[i].ctr--; AnimateEntity(&entity[i]); }
      if (!entity[i].ctr) { entity[i].reset=1; entity[i].moving=0; }
   }
}

void ProcessEntity(int i)
{ int j;

  if (player==&entity[i]) return;
  if (entity[i].speed<4)
  {
    switch (entity[i].speed)
    {
      case 1: if (entity[i].speedct<3) { entity[i].speedct++; return; }
      case 2: if (entity[i].speedct<2) { entity[i].speedct++; return; }
      case 3: if (entity[i].speedct<1) { entity[i].speedct++; return; }
    }
  }
  if (entity[i].speed<5)
  {
    ProcessEntity1(i);
    entity[i].speedct=0;
  }
  switch (entity[i].speed)
  {
    case 5: for (j=0; j<2; j++) { ProcessEntity1(i); } return;
    case 6: for (j=0; j<3; j++) { ProcessEntity1(i); } return;
    case 7: for (j=0; j<4; j++) { ProcessEntity1(i); } return;
  }
}

void ProcessEntities()
{
  int i;

  SiftEntities();
  for (i=0; i<entities; i++) ProcessEntity(i);
}

void EntityStat()
{
  Con_NextLine();
  sprintf(strbuf,"There are ‚%d~ entities on this map.",entities);
  Con_Printf(strbuf);
  sprintf(strbuf,"‚%d~ of those are onscreen / active.",cc);
  Con_Printf(strbuf);
}

void ListActiveEnts()
{ int i;

  Con_NextLine();
  Con_Printf("Active entities:");
  for (i=0; i<cc; i++)
  {
    sprintf(strbuf,"‚%d~",entidx[i]);
    Con_Printf(strbuf);
  }
}

void EntityS()
{ int i;

  i=atoi((char *) args[1]);
  Con_NextLine();

  sprintf(strbuf,"Desc: ‚%s~", entity[i].desc); Con_Printf(strbuf);
  sprintf(strbuf,"tx: ‚%d~ ty: ‚%d~", entity[i].tx, entity[i].ty); Con_Printf(strbuf);
  sprintf(strbuf,"CHR index: %d", entity[i].chrindex); Con_Printf(strbuf);
}

int AllocateEntity(int x1, int y1, char *fname)
{
  entity[entities].chrindex=CacheCHR(fname);
  entity[entities].x=x1*16;
  entity[entities].y=y1*16;
  entity[entities].tx=x1;
  entity[entities].ty=y1;
  entity[entities].speed=4;
  entity[entities].obsmode1=1;
  entity[entities].obsmode2=1;
  entities++;
  return (entities-1);
}
