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

#include "maped.h"

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>

#include "config.h"
#include "keyboard.h"
#include "timer.h"
#include "vdriver.h"

#include "colbak.h"
#include "colstuff.h"

#include "smalfont.h"
#include "gui.h"

#include "tilesel.h"

#define byte unsigned char
#define word unsigned short

// ================================= Data ====================================

char *field, *destruct;
char *background, *sprites;
int startofs=16;
int turnlength, turnctr;
int destructed, difficulty=4;
int blocks, score, scoremultiplier;
int level;

// Current piece falling

char piece[3], tempspot;
int xpos, ypos, ypofs;
char newpiece;
char nextpiece[3];

// ================================= Code ====================================

void BlitPlayField()
{
  int i, j;
  char c;

  for (i=0; i<12; i++)
    for (j=0; j<6; j++)
    {
      c=field[(i*6)+j];
      if (c) TCopySprite(128+(j*16), startofs+4+(i*16), 16, 16, sprites+((c+1)*256));
    }
}

void BlitPlayFieldwDestruct(char flash)
{
  int i, j;
  char c;

  for (i=0; i<12; i++)
    for (j=0; j<6; j++)
    {
      c=field[(i*6)+j];
      if (c && !destruct[(i*6)+j])
        TCopySprite(128+(j*16), startofs+4+(i*16), 16, 16, sprites+((c+1)*256));
      if (destruct[(i*6)+j])
        TCopySprite(128+(j*16), startofs+4+(i*16), 16, 16, sprites+(flash*256));
    }
}

void BlitPiece()
{
  ypofs=((100 * turnctr) / turnlength) * 16 / 100;

  TCopySprite(128+(xpos*16), startofs+4+(ypos*16)+ypofs, 16, 16, sprites+((piece[0]+1)*256));
  TCopySprite(128+(xpos*16), startofs+20+(ypos*16)+ypofs, 16, 16, sprites+((piece[1]+1)*256));
  TCopySprite(128+(xpos*16), startofs+36+(ypos*16)+ypofs, 16, 16, sprites+((piece[2]+1)*256));

  TCopySprite(276, startofs+70, 16, 16, sprites+((nextpiece[0]+1)*256));
  TCopySprite(276, startofs+90, 16, 16, sprites+((nextpiece[1]+1)*256));
  TCopySprite(276, startofs+110, 16, 16, sprites+((nextpiece[2]+1)*256));
}

void GenerateNewPiece()
{
  newpiece=0;
  xpos=2;
  ypos=0;
  if (field[2])  key[SCAN_ESC]=1;
  if (field[8])  key[SCAN_ESC]=1;
  if (field[14]) key[SCAN_ESC]=1;
  if (field[20]) key[SCAN_ESC]=1;

  if (key[SCAN_ESC])
  {
    if (difficulty==4 && blocks<240) Message("You really suck at columns, man.",300);
    if (difficulty==5 && blocks<80)  Message("You really suck at columns, man.",300);
    if (score>3200 && score<=6400) Message("Not bad.",300);
    if (score>6400) Message("Behold, the Columns God walks among us.",500);
  }

  memcpy(&piece, &nextpiece, 3);
  nextpiece[0]=random(1,difficulty);
  nextpiece[1]=random(1,difficulty);
  nextpiece[2]=random(1,difficulty);
}

void DestroyBlock(int x, int y)
{
  int i;

  for (i=y; i>0; i--)
  {
    field[(i*6)+x]=field[((i-1)*6)+x];
  }
  blocks++;
  score+=scoremultiplier;
}

void ClearConsecutiveColumns()
{
  int x, y;
  char c;

  // Scan for things to destroy, but don't destroy them, just mark
  // them in the Destroy field.

  destructed=0;
  memset(destruct, 0, (6*13));

  for (y=0; y<12; y++)
    for (x=0; x<6; x++)
    {
      c=field[(y*6)+x];

      // Test vertical match
      if (y<10)
      {
        if ((field[((y+1)*6)+x]==c) && (field[((y+2)*6)+x]==c) && c)
        {
          // vertical match. Set destruct field.
          destruct[((y+0)*6)+x]=1;
          destruct[((y+1)*6)+x]=1;
          destruct[((y+2)*6)+x]=1;
          destructed=1;
        }
      }
      // Test horizontal match
      if (x<4)
      {
        if ((field[(y*6)+x+1]==c) && (field[(y*6)+x+2]==c) && c)
        {
          // horizontal match. Set destruct field.
          destruct[(y*6)+x+0]=1;
          destruct[(y*6)+x+1]=1;
          destruct[(y*6)+x+2]=1;
          destructed=1;
        }
      }
      // Test negative diagonal match
      if (x<4 && y<10)
      {
         if ((field[((y+1)*6)+x+1]==c) && (field[((y+2)*6)+x+2]==c) && c)
         {
           // negative diagonal match. set destruct field.
           destruct[((y+0)*6)+x+0]=1;
           destruct[((y+1)*6)+x+1]=1;
           destruct[((y+2)*6)+x+2]=1;
           destructed=1;
         }
      }
      // Test positive diagonal match
      if (x>=2 && y<10)
      {
         if ((field[((y+1)*6)+x-1]==c) && (field[((y+2)*6)+x-2]==c) && c)
         {
           // positive diagonal match. set destruct field.
           destruct[((y+0)*6)+x-0]=1;
           destruct[((y+1)*6)+x-1]=1;
           destruct[((y+2)*6)+x-2]=1;
           destructed=1;
         }
      }
    }

  // Show which tiles are being destroyed before clearing out for increase
  // visual coolishness.

  if (destructed)
  {
    timer_count=0;
    while (timer_count<50)
    {
      if (timer_count<10 || (timer_count>20 && timer_count<30) ||
         (timer_count>40 && timer_count<50)) c=1;
      else c=0;

      CopySprite(16,16,320,200,background);
      BlitPlayFieldwDestruct(c);
      TCopySprite(276, startofs+70, 16, 16, sprites+((nextpiece[0]+1)*256));
      TCopySprite(276, startofs+90, 16, 16, sprites+((nextpiece[1]+1)*256));
      TCopySprite(276, startofs+110, 16, 16, sprites+((nextpiece[2]+1)*256));

GotoXY(55,130);
sprintf(strbuf,"Blocks: %d", blocks);
printstring(strbuf);
GotoXY(55,140);
sprintf(strbuf,"Score: %d", score);
printstring(strbuf);
GotoXY(55,150);
sprintf(strbuf,"Level: %d", level);
printstring(strbuf);

      ShowPage();
    }
  }

  // Apply destruct-o patch to the field.

  for (y=0; y<12; y++)
    for (x=0; x<6; x++)
      if (destruct[(y*6)+x]) DestroyBlock(x,y);
}

void NextTurn()
{
  ypos++;
  if (field[((ypos+3)*6)+xpos])
  {
    if (ypos<3) key[SCAN_ESC]=1;
    // Hit something; set piece here.
    field[(ypos*6)+xpos]=piece[0];
    field[((ypos+1)*6)+xpos]=piece[1];
    field[((ypos+2)*6)+xpos]=piece[2];
    newpiece=1;

    // Scan for consecutive things to delete.
    do
    {
      ClearConsecutiveColumns();
      scoremultiplier*=2;
    } while (destructed);
  }
}

int CanGoLeft()
{
  if (!xpos) return 0;
  if (field[((ypos+1)*6)+xpos-1]) return 0;
  if (field[((ypos+2)*6)+xpos-1]) return 0;
  if (field[((ypos+3)*6)+xpos-1]) return 0;
  return 1;
}

int CanGoRight()
{
  if (xpos==5) return 0;
  if (field[((ypos+1)*6)+xpos+1]) return 0;
  if (field[((ypos+2)*6)+xpos+1]) return 0;
  if (field[((ypos+3)*6)+xpos+1]) return 0;
  return 1;
}

void RotatePiece()
{
  tempspot=piece[0];
  piece[0]=piece[1];
  piece[1]=piece[2];
  piece[2]=tempspot;
}

void ProcessLevels()
{
  if (blocks>80)  { level=2; turnlength=90; }
  if (blocks>160) { level=3; turnlength=80; }
  if (blocks>240) { level=4; turnlength=70; }
  if (blocks>320) { level=5; turnlength=60; }
  if (blocks>400) { level=6; turnlength=50; }
  if (blocks>480) { level=7; turnlength=40; }
  if (blocks>560) { level=8; turnlength=30; }
  if (blocks>640) { level=9; turnlength=20; }
}

extern FILE *pcxf;
extern int width, depth;
extern int bytes;
extern int vidoffset;

void LoadBackground()
{
  int i;

  if (!(pcxf=fopen("colback.pcx","rb")))
  {
    background=background_;
  }
  else
  {
    fclose(pcxf);
    background=(char *) valloc(64000,"col bakg",0);
    LoadPCXHeaderNP("colback.pcx");
    for (i=0; i<depth; i++)
    {
       vidoffset=width*i;
       ReadPCXLine(background);
    }
    fclose(pcxf);
  }

  if (!(pcxf=fopen("colspr.pcx","rb")))
  {
    sprites=sprites_;
  }
  else
  {
    fclose(pcxf);
    LoadPCXHeaderNP("colspr.pcx");
    sprites=(char *) valloc(width*depth,"col spr",0);
    for (i=0; i<depth; i++)
    {
       vidoffset=width*i;
       ReadPCXLine(sprites);
    }
    fclose(pcxf);
  }
}

void Columns()
{
  ClearScreen();
  srand(time(NULL));
  LoadBackground();
  field=(char *) valloc(6*13,"col field",0);
  destruct=(char *) valloc(6*13,"col dest",0);
  memset(field, 0, 6*12);
  memset(field+(6*12), 255, 6);
  newpiece=1;
  turnlength=100;
  turnctr=0;
  timer_count=0;
  blocks=0;
  score=0;
  level=1;
  GenerateNewPiece();
  GenerateNewPiece();
  while (!key[SCAN_ESC])
  {
    CopySprite(16,16,320,200,background);
    if (key[SCAN_LEFT])
    {
      if (xpos && CanGoLeft()) xpos--;
      key[SCAN_LEFT]=0;
    }
    if (key[SCAN_RIGHT])
    {
      if (xpos<5 && CanGoRight()) xpos++;
      key[SCAN_RIGHT]=0;
    }
    if (key[SCAN_DOWN])
    {
      timer_count*=5;
    }
    if (key[SCAN_UP])
    {
      RotatePiece();
      key[SCAN_UP]=0;
    }
    ProcessLevels();
    turnctr+=timer_count;
    timer_count=0;
    switch (difficulty)
    {
      case 4: scoremultiplier=5;
      case 5: scoremultiplier=10;
      case 6: scoremultiplier=15;
    }
    if (turnctr>=turnlength)
    {
      NextTurn();
      turnctr-=turnlength;
    }
    if (newpiece) GenerateNewPiece();
    BlitPlayField();
    BlitPiece();

GotoXY(55,130);
sprintf(strbuf,"Blocks: %d", blocks);
printstring(strbuf);
GotoXY(55,140);
sprintf(strbuf,"Score: %d", score);
printstring(strbuf);
GotoXY(55,150);
sprintf(strbuf,"Level: %d", level);
printstring(strbuf);

    ShowPage();
  }
  vfree(sprites);
  vfree(background);
  vfree(field);
  vfree(destruct);
}
