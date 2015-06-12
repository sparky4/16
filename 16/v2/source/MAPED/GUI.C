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

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <malloc.h>
#include <stdlib.h>

#include "config.h"
#include "guicomp.h"
#include "maped.h"
#include "mouse.h"
#include "render.h"
#include "timer.h"
#include "vdriver.h"
#include "keyboard.h"

#include "smalfont.h"
#include "columns.h"
#include "tilesel.h"

// ================================= Data ====================================

struct message
{
  char text[44];                     // message text
  int exptime;                       // message expire time
};

struct message messages[6];          // 5-message buffer
char num_msgs=0;                     // number of active messages
char playingmod[80];                 // currently playing mod.
extern char scrollmode;

// ================================= Code ====================================

void CheckMessageExpirations()
{ int i;

  for (i=0; i<num_msgs; i++)
    if (systemtime>messages[i].exptime)
    {
       memcpy(messages[i].text, messages[i+1].text, (48*(num_msgs-i+1)));
       num_msgs--;
    }
}

void Message(char *text, int duration)
{
  if (num_msgs<5)
  {
    memcpy(messages[num_msgs].text, text, strlen(text));
    messages[num_msgs].exptime=systemtime+duration;
    num_msgs++;
    return;
  }

  memcpy(&messages[0].text, &messages[1].text, 192);
  memcpy(messages[4].text, text, strlen(text)+1);
  messages[num_msgs].exptime=systemtime+duration;
}

void DrawScreenControls()
{ int i;
  byte c;

  if (el<6)
  {
    if (pasting)
    {
      GotoXY(298-5,tsy-19+13);
      printstring("P");
    }
  }

  // Put current-tile boxes if applicable.
  if (el<6)
  {
    FilledBox(298,tsy-19,18,18,white);
    FilledBox(317,tsy-19,18,18,white);
    CopySprite(299,tsy-18,16,16,vsp+(256*lt));
    CopySprite(318,tsy-18,16,16,vsp+(256*rt));
  }

  if (el==7)
  {
    sprintf(strbuf,"Zone %d",curzone);
    GotoXY(280,tsy-16);
    printstring(strbuf);
    GotoXY(280,tsy-10);
    printstring(zones[curzone].name);

    c=zone[((((ywin*layer[0].pmulty/layer[0].pdivy)+my-16)/16) *
               layer[0].sizex)+(((xwin*layer[0].pmultx/layer[0].pdivx)+mx-16)/16)];

    if (c)
    {
       sprintf(strbuf,"zone %d",c);
       GotoXY(280,17);
       printstring(strbuf);
       GotoXY(280,23);
       printstring(zones[c].name);
    }
  }

  if (el==8)
  {
    sprintf(strbuf,"%d entities",entities);
    GotoXY(285,17); printstring(strbuf);

    if (c=EntityThere((mx+xwin-16)/16,(my+ywin-16)/16))
    {
      c--;
      GotoXY(285,tsy-10);
      printstring(entity[c].desc);
    }
  }

  // Put the current-tile mouse thingy.
  GotoXY(17,17);
  dec_to_asciiz((xwin+mx-16)/16,strbuf);
  printstring(strbuf); printstring(", ");
  dec_to_asciiz((ywin+my-16)/16,strbuf);
  printstring(strbuf);

  // Put the layer editor/selector.

  for (i=0; i<6; i++)
  {
    if (el==i) c=white; else c=darkw;
    FilledBox(17+(i*10),tsy-7,9,7,c);
    if (layertoggle[i]) c=darkred; else c=0;
    FilledBox(18+(i*10),tsy-6,7,6,c);
    GotoXY(20+(i*10),tsy-5); dec_to_asciiz(i+1,strbuf);
    if (i<numlayers) printstring(strbuf);
  }

  for (i=6; i<10; i++)
  {
    if (el==i) c=white; else c=darkw;
    FilledBox(17+(i*10),tsy-7,9,7,c);
    if (layertoggle[i]) c=darkred; else c=0;
    FilledBox(18+(i*10),tsy-6,7,6,c);
    GotoXY(20+(i*10),tsy-5);
    switch (i)
    {
      case 6: printstring("O"); break;
      case 7: printstring("N"); break;
      case 8: printstring("E"); break;
      case 9: printstring("T"); break;
    }
  }
}

void RenderMessages()
{ int i;

  CheckMessageExpirations();
  for (i=0; i<num_msgs; i++)
  {
    GotoXY(17,23+(i*6));
    printstring(messages[i].text);
  }
}

void RenderGUI()
{
  RenderMessages();
  DrawScreenControls();
  DrawMouse();
}

// ============================= Dialog Code =================================

int mmxofs=60, mmyofs=50;

void DoMainMenu()
{
  key[SCAN_ESC]=0;
  MainMenu(mmxofs, mmyofs);
}

void MainMenu(int dx, int dy)
{ char done=0;
  //int mxo, myo, cursorblink;

  mmxofs=dx;
  mmyofs=dy;
  do
  {
    if (dx<16) dx=16;
    if (dy<16) dy=16;
    if (dx+80>335) dx=336-80;
    if (dy+89>sy+16) dy=(sy+16)-89;

    RenderMap();
    RenderGUI();
    stdwindow(dx, dy, dx+80, dy+89);

    if (mx>dx && mx<dx+80)
    {
       if (my>dy+2 && my<dy+9) FilledBox(dx+2, dy+2, 76, 7, darkw);
       if (my>dy+8 && my<dy+15) FilledBox(dx+2, dy+8, 76, 7, darkw);
       if (my>dy+14 && my<dy+21) FilledBox(dx+2, dy+14, 76, 7, darkw);
       if (my>dy+20 && my<dy+26) FilledBox(dx+2, dy+20, 76, 7, darkw);
       if (my>dy+29 && my<dy+36) FilledBox(dx+2, dy+29, 76, 7, darkw);
       if (my>dy+35 && my<dy+42) FilledBox(dx+2, dy+35, 76, 7, darkw);
       if (my>dy+41 && my<dy+48) FilledBox(dx+2, dy+41, 76, 7, darkw);
       if (my>dy+47 && my<dy+54) FilledBox(dx+2, dy+47, 76, 7, darkw);
       if (my>dy+53 && my<dy+60) FilledBox(dx+2, dy+53, 76, 7, darkw);
       if (my>dy+59 && my<dy+66) FilledBox(dx+2, dy+59, 76, 7, darkw);
       if (my>dy+65 && my<dy+72) FilledBox(dx+2, dy+65, 76, 7, darkw);
       if (my>dy+71 && my<dy+78) FilledBox(dx+2, dy+71, 76, 7, darkw);
       if (my>dy+80 && my<dy+87) FilledBox(dx+2, dy+80, 76, 7, darkw);
    }

    GotoXY(dx+3, dy+3); printstring("New MAP");
    GotoXY(dx+3, dy+9); printstring("Load MAP");
    GotoXY(dx+3, dy+15); printstring("Save MAP");
    GotoXY(dx+3, dy+21); printstring("Load VSP");
    HLine(dx+2, dy+28, 76, white);
    GotoXY(dx+3, dy+30); printstring("MAP Properties");
    GotoXY(dx+3, dy+36); printstring("Layers"); GotoXY(dx+73, dy+36); printstring(">");
    GotoXY(dx+3, dy+42); printstring("Options");
    GotoXY(dx+3, dy+48); printstring("MOD Player");
    GotoXY(dx+3, dy+54); printstring("CD Player");
    GotoXY(dx+3, dy+60); printstring("Columns");
    GotoXY(dx+3, dy+66); printstring("Execute"); GotoXY(dx+73, dy+66); printstring(">");
    GotoXY(dx+3, dy+72); printstring("Exit to DOS");
    HLine(dx+2, dy+79, 76, white);
    GotoXY(dx+3, dy+81); printstring("About MapEd");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    if (mb==1 && mx>dx && mx<dx+80)
    {
       if (my>dy+2 && my<dy+9) { NewMAP(); done=1; WaitRelease(); break; }
       if (my>dy+8 && my<dy+15) { done=1; WaitRelease(); LoadMAPDialog(); break; }
       if (my>dy+14 && my<dy+21) { done=1; WaitRelease(); key[SCAN_F10]=1; break; }
       if (my>dy+20 && my<dy+26) { done=1; WaitRelease(); LoadVSPDialog(); break; }
       if (my>dy+29 && my<dy+36) { done=1; WaitRelease(); MapPropertiesDialog(); break; }
       if (my>dy+35 && my<dy+42) { done=1; WaitRelease(); LayersSubMenu(dx,dy); break; }
       if (my>dy+41 && my<dy+48) { done=1; WaitRelease(); OptionsDialog(); break; }
       if (my>dy+47 && my<dy+54) { done=1; WaitRelease(); MPDialog(); break; }
       if (my>dy+53 && my<dy+60) { done=1; WaitRelease(); CDDialog(); break; }
       if (my>dy+59 && my<dy+66) { done=1; WaitRelease(); Columns(); break; }
       if (my>dy+65 && my<dy+72) { done=1; WaitRelease(); ExecuteSubMenu(0, 0); break; }
       if (my>dy+71 && my<dy+78) { done=1; WaitRelease(); key[SCAN_X]=1; key[SCAN_ALT]=1; break; }
       if (my>dy+80 && my<dy+87) { done=1; About(); break; }
    }

    if (mb==1)
    {
      WaitRelease();
      done=1;
    }
    if (key[SCAN_ESC]==1) done=1;
  } while (!done);
  key[SCAN_ESC]=0;
}

// tilesel.c
extern FILE *pcxf;
extern int width, depth;
extern int vidoffset;
extern char *ovsp;
extern char *t;
extern word ontiles;

static int TileMatch(int a, int b)
  { return !memcmp(ovsp+(a*256), ovsp+(b*256), 256); }

int ilxsize=145, ilysize=122+10;
int ilxofs=16, ilyofs=16;

// make this configurable at some point
#define MIN_TILE_X 50
#define MIN_TILE_Y 50

static int BitToNum(int num)
{
  int i;

  if (!num)
    return 0;

  for (i=0; i<32; i++)
  {
    if (num & (1<<i))
      break;
  }

  return i+1;
}

//zero
byte TileCmp(byte *one, byte *two)
  { return !memcmp(one,two, 256); }

void ImageToLayer()
{ char done=0, moving=0;
  int mxo, myo, cursorblink;
  char ctf=0, cb=0, c, tt, *str;
  char nstr1[80], nstr2[80];

  int i,j,k,m;
  FILE *f=0;
  int num_padx=0,num_pady=0; // in tiles
  int minpadx=0,minpady=0; // in tiles
  int tilesx=0,tilesy=0; // in tiles
  int totalx=0,totaly=0; // in tiles
  int excessx=0,excessy=0; // in pixels
  //int chk_1=1,chk_2=0,chk_3=0,chk_4=0,chk_5=0,chk_6=0; // radio control
  int chk_layer=1;
  int chk_grabexcess=0,chk_centerinpad=0,chk_nodup=1; //,chk_padisblank=1;
  char txt_imagename[80];
        // ...
        int ly=0;
        int wide=0,deep=0;
        int lsz=0;
        int matched=0,plot=0; //,mapat=0;
        int tilesripped=0;
        int startx=0,starty=0;
  memset(txt_imagename,0,80);
  bmode=0; // for browser; update map/gui
  width=0; depth=0; // for pcx loading code

  sprintf(nstr1, "%d", num_padx);
  sprintf(nstr2, "%d", num_pady);
  last_pressed=0;
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      ilxofs = (mx-mxo);
      ilyofs = (my-myo);
      if (!mb) moving=0;
      if (ilxofs<16) ilxofs = 16;
      if (ilyofs<16) ilyofs = 16;
      if (ilxofs+ilxsize>335) ilxofs = 336-ilxsize;
      if (ilyofs+ilysize>sy+16) ilyofs = (sy+16)-ilysize;
    }

    Window(ilxofs, ilyofs, ilxofs+ilxsize, ilyofs+ilysize, "Import Image As Layer");
    Button(ilxofs+109, ilyofs+12, "Browse");
    Button(ilxofs+109, ilyofs+30, "OK");
    GotoXY(ilxofs+5, ilyofs+25); printstring("Layer:");
    // layer select radio buttons
    for (i=0; i<6; i++)
    {
      CheckBox(ilxofs+29+(i*10), ilyofs+33, chk_layer & (1<<i));
      GotoXY(ilxofs+31+(i*10), ilyofs+25);
      sprintf(strbuf, "%i", i+1); printstring(strbuf);
    }
    GotoXY(ilxofs+5, ilyofs+14); printstring("Image:");
    //TextField(ilxofs+29, ilyofs+12, 75, txt_imagename, ctf == 1 ? cb : 0);
    GotoXY(ilxofs+29, ilyofs+14); printstring(txt_imagename);
    HLine(ilxofs+1, ilyofs+44, 143, white);
    GotoXY(ilxofs+5, ilyofs+49); printstring("Image Dimensions:");
    if (width+depth)
    {
      GotoXY(ilxofs+5+pixels("Image Dimensions:")+3, ilyofs+49);
      sprintf(strbuf, "%dx%d", width, depth);
      printstring(strbuf);
    }
    GotoXY(ilxofs+5, ilyofs+60); printstring("Tiles X:");
    if (width+depth)
    {
      GotoXY(ilxofs+5+pixels("Tiles X:")+3, ilyofs+60);
      sprintf(strbuf, "%d", tilesx);
      printstring(strbuf);
    }
    GotoXY(ilxofs+5, ilyofs+71); printstring("Tiles Y:");
    if (width+depth)
    {
      GotoXY(ilxofs+5+pixels("Tiles Y:")+3, ilyofs+71);
      sprintf(strbuf, "%d", tilesy);
      printstring(strbuf);
    }
    GotoXY(ilxofs+51, ilyofs+60); printstring("Pad X:");
    GotoXY(ilxofs+51, ilyofs+71); printstring("Pad Y:");
    TextField(ilxofs+74, ilyofs+58, 12, nstr1, ctf == 1 ? cb : 0);
    TextField(ilxofs+74, ilyofs+69, 12, nstr2, ctf == 2 ? cb : 0);
    GotoXY(ilxofs+95, ilyofs+60); printstring("Total X:");
    if (width+depth)
    {
      GotoXY(ilxofs+95+pixels("Total X:")+3, ilyofs+60);
      sprintf(strbuf, "%d", totalx);
      printstring(strbuf);
    }
    GotoXY(ilxofs+95, ilyofs+71); printstring("Total Y:");
    if (width+depth)
    {
      GotoXY(ilxofs+95+pixels("Total Y:")+3, ilyofs+71);
      sprintf(strbuf, "%d", totaly);
      printstring(strbuf);
    }
    GotoXY(ilxofs+5, ilyofs+81); printstring("Excess Pixels X:");
    if (width+depth)
    {
      GotoXY(ilxofs+5+pixels("Excess Pixels X:")+3, ilyofs+81);
      sprintf(strbuf, "%d", excessx);
      printstring(strbuf);
    }
    GotoXY(ilxofs+5, ilyofs+91); printstring("Excess Pixels Y:");
    if (width+depth)
    {
      GotoXY(ilxofs+5+pixels("Excess Pixels Y:")+3, ilyofs+91);
      sprintf(strbuf, "%d", excessy);
      printstring(strbuf);
    }
    CheckBox(ilxofs+5, ilyofs+101, chk_grabexcess);
    GotoXY(ilxofs+17, ilyofs+102); printstring("Grab Excess");
    CheckBox(ilxofs+5, ilyofs+110, chk_centerinpad);
    GotoXY(ilxofs+17, ilyofs+111); printstring("Center Within Padding");
    CheckBox(ilxofs+5, ilyofs+119, chk_nodup);
    GotoXY(ilxofs+17, ilyofs+120); printstring("Eliminate Duplicates");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if ((mb==1 && mx>ilxofs+109 && mx<ilxofs+30+109 && my>ilyofs+12 && my<ilyofs+10+12 && !moving) || (key[SCAN_ALT] && key[SCAN_B]))
    {
      // insert code for button "Browse" here
      ButtonPressed(ilxofs+109, ilyofs+12, "Browse");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      Browse(1, "*.pcx", "", "", "", txt_imagename);
      key[SCAN_ESC]=0; // just in case ESC exited
      f=fopen(txt_imagename, "rb");
      if (f)
      {
        LoadPCXHeaderNP(txt_imagename);
        fclose(pcxf);

        if (chk_grabexcess) i=15; else i=0;
        tilesx=((width+i)&~15)/16;
        tilesy=((depth+i)&~15)/16;
        excessx=width & 15;
        excessy=depth & 15;
        if (tilesx<MIN_TILE_X) { minpadx=MIN_TILE_X-tilesx; num_padx=minpadx; }
        if (tilesy<MIN_TILE_Y) { minpady=MIN_TILE_Y-tilesy; num_pady=minpady; }
        sprintf(nstr1, "%d", minpadx);
        sprintf(nstr2, "%d", minpady);
        totalx=tilesx+minpadx;
        totaly=tilesy+minpady;
      }
      WaitRelease(); continue;
    }

    if ((mb==1 && mx>ilxofs+109 && mx<ilxofs+30+109 && my>ilyofs+30 && my<ilyofs+10+30 && !moving) || (key[SCAN_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      ButtonPressed(ilxofs+109, ilyofs+30, "OK");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }

      if (strlen(txt_imagename))
      {
        done=1;

        ly=BitToNum(chk_layer) -1;

        startx=0;
        starty=0;
        if (chk_centerinpad)
        {
          startx=(totalx-tilesx)/2;
          starty=(totaly-tilesy)/2;
        }

        LoadPCXHeaderNP(txt_imagename);

        wide=width;
        deep=depth;
        if (chk_grabexcess)
        {
          // round up to the nearest multiple of 16 if grabbing excess
          wide=(width+15)&~15;
          deep=(depth+15)&~15;
        }
        // allocate transit image bay, which may have padding ^
        t=(char *) valloc(wide * deep,"img2layer",0);

        for (i=0; i<depth; i++)
        {
          vidoffset=wide*i;
          ReadPCXLine(t);
        }
        fclose(pcxf);

        deep=tilesy;
        wide=tilesx;
        ontiles=(wide*deep)+1; // +1 for our empty tile

        ovsp=(char *) valloc(ontiles*256,"img2layer2",0);

        layer[ly].pmultx=1;     layer[ly].pmulty=1;
        layer[ly].pdivx=1;      layer[ly].pdivy=1;
        // totalx & totaly are the sum of tilesx/tilesy & num_padx/num_pady
        layer[ly].sizex=totalx; layer[ly].sizey=totaly;
        layer[ly].trans=0;      layer[ly].hline=0;

        lsz = totalx * totaly;

        if (layers[ly]) vfree(layers[ly]);
        layers[ly]=(unsigned short *) valloc((lsz*2)+4,"layer data",ly);

        tilesripped=0;
        memset(ovsp,0,256);
        tilesripped++;

        for (i=0; i<deep; i++)
        {
          for (j=0; j<wide; j++)
          {
            GrabTileAt((j*16), (i*16), tilesripped, 0);
            plot=tilesripped;

            // eliminate duplicates?
            if (chk_nodup)
            {
              matched=0;
              for (k=0; k<tilesripped; k++)
              {
                if (TileMatch(tilesripped, k))
                {
                  matched=1;
                  plot=k;
                  break;
                }
              }
                //begin zero
              for(m=0; m<numtiles; m++)
              {
                if(TileCmp(vsp+m*256,ovsp+tilesripped*256))
                {
                  matched=1;
                  plot=m-numtiles;
//                  plot=0;
                  break;
                }
              }
              //end zero!
              if (!matched) tilesripped++;
            }
            // grabbing all; keep it regardless
            else tilesripped++;

            layers[ly][((i+starty)*totalx)+j+startx]=plot;
          }
        }

        // adjust indices past current tile amount
        for (i=0; i<(totalx*totaly); i++)
          layers[ly][i]+=numtiles;

        if (!ly)
        {
          if (obstruct) vfree(obstruct);
          obstruct=(char *) valloc(lsz+4,"obstruct",0);
          if (zone) vfree(zone);
          zone=(char *) valloc(lsz+4,"zone", 0);
        }

        // reallocate tilebay and patch in ovsp
        str=(char *)valloc((numtiles+tilesripped)*256,"vsp data",0);
        memcpy(str, vsp, (numtiles*256));
        vfree(vsp); vsp=str; str=0;
        memcpy(vsp+(numtiles*256), ovsp, (tilesripped*256));
        numtiles+=tilesripped;

        // did we make a new layer?
        if (ly+1>numlayers)
        {
          numlayers++;
          sprintf(rstring, "%s%i", rstring, ly+1); // update render-order
        }

        vfree(t);
        vfree(ovsp);
      }
      // ...
      WaitRelease(); continue;
    }

    if (mb==1 && mx>ilxofs+29 && mx<ilxofs+8+29 && my>ilyofs+33 && my<ilyofs+7+33 && !moving)
    {
      chk_layer ^= 1;
      chk_layer &= 1;
      //chk_1 ^= 1;
      //chk_2=0,chk_3=0,chk_4=0,chk_5=0,chk_6=0;
      WaitRelease(); continue;
    }

    if (mb==1 && mx>ilxofs+39 && mx<ilxofs+8+39 && my>ilyofs+33 && my<ilyofs+7+33 && !moving)
    {
      if (numlayers+1 >= 2)
      {
      chk_layer ^= 2;
      chk_layer &= 2;
      //chk_2 ^= 1;
      //chk_1=0,chk_3=0,chk_4=0,chk_5=0,chk_6=0;
      WaitRelease(); continue;
      }
    }

    if (mb==1 && mx>ilxofs+49 && mx<ilxofs+8+49 && my>ilyofs+33 && my<ilyofs+7+33 && !moving)
    {
      if (numlayers+1 >= 3)
      {
      chk_layer ^= 4;
      chk_layer &= 4;
      //chk_3 ^= 1;
      //chk_1=0,chk_2=0,chk_4=0,chk_5=0,chk_6=0;
      WaitRelease(); continue;
      }
    }

    if (mb==1 && mx>ilxofs+59 && mx<ilxofs+8+59 && my>ilyofs+33 && my<ilyofs+7+33 && !moving)
    {
      if (numlayers+1 >= 4)
      {
      chk_layer ^= 8;
      chk_layer &= 8;
      //chk_4 ^= 1;
      //chk_1=0,chk_2=0,chk_3=0,chk_5=0,chk_6=0;
      WaitRelease(); continue;
      }
    }

    if (mb==1 && mx>ilxofs+69 && mx<ilxofs+8+69 && my>ilyofs+33 && my<ilyofs+7+33 && !moving)
    {
      if (numlayers+1 >= 5)
      {
      chk_layer ^= 16;
      chk_layer &= 16;
      //chk_5 ^= 1;
      //chk_1=0,chk_2=0,chk_3=0,chk_4=0,chk_6=0;
      WaitRelease(); continue;
      }
    }

    if (mb==1 && mx>ilxofs+79 && mx<ilxofs+8+79 && my>ilyofs+33 && my<ilyofs+7+33 && !moving)
    {
      if (numlayers+1 >= 6)
      {
      chk_layer ^= 32;
      chk_layer &= 32;
      //chk_6 ^= 1;
      //chk_1=0,chk_2=0,chk_3=0,chk_4=0,chk_5=0;
      WaitRelease(); continue;
      }
    }

    // if no checkboxes are marked, mark the first.
    //if (!(chk_1+chk_2+chk_3+chk_4+chk_5+chk_6)) chk_1=1;
    if (!chk_layer) chk_layer=1;

//    if (mb==1 && mx>ilxofs+29 && mx<ilxofs+75+29 && my>ilyofs+12 && my<ilyofs+9+12 && !moving)
//    {
//      ctf=1; str=txt_imagename; cb=1;
//      cursorblink=systemtime+40; continue;
//    }

    if (mb==1)
    {
      if (mx>ilxofs+74 && mx<ilxofs+12+74 && my>ilyofs+58 && my<ilyofs+9+58 && !moving)
      {
        if (ctf==2)
        {
          if (atoi(str)<minpady)
            sprintf(str, "%d", minpady);
        }
        ctf=1; str=nstr1; cb=1;
        cursorblink=systemtime+40; continue;
      }

      if (mx>ilxofs+74 && mx<ilxofs+12+74 && my>ilyofs+69 && my<ilyofs+9+69 && !moving)
      {
        if (ctf==1)
        {
          if (atoi(str)<minpadx)
            sprintf(str, "%d", minpadx);
        }
        ctf=2; str=nstr2; cb=1;
        cursorblink=systemtime+40; continue;
      }

      switch (ctf)
      {
        case 1: if (atoi(str) < minpadx) sprintf(str, "%d", minpadx); break;
        case 2: if (atoi(str) < minpady) sprintf(str, "%d", minpady); break;
      }
    }

    if (mb==1 && mx>ilxofs+5 && mx<ilxofs+8+5 && my>ilyofs+101 && my<ilyofs+7+101 && !moving)
    {
      chk_grabexcess ^= 1;
      // <barf>
      if (excessx+excessy)
      {
        if (chk_grabexcess)
        {
          if (excessx) tilesx++;
          if (excessy) tilesy++;
        }
        else
        {
          if (excessx) tilesx--;
          if (excessy) tilesy--;
        }
        if (tilesx<MIN_TILE_X) { minpadx=MIN_TILE_X-tilesx; num_padx=minpadx; }
        if (tilesy<MIN_TILE_Y) { minpady=MIN_TILE_Y-tilesy; num_pady=minpady; }
        sprintf(nstr1, "%d", minpadx);
        sprintf(nstr2, "%d", minpady);
        totalx=tilesx+minpadx;
        totaly=tilesy+minpady;
      }
      WaitRelease(); continue;
    }

    if (mb==1 && mx>ilxofs+5 && mx<ilxofs+8+5 && my>ilyofs+110 && my<ilyofs+7+110 && !moving)
    {
      chk_centerinpad ^= 1; WaitRelease(); continue;
    }

    if (mb==1 && mx>ilxofs+5 && mx<ilxofs+8+5 && my>ilyofs+119 && my<ilyofs+7+119 && !moving)
    {
      chk_nodup ^= 1; WaitRelease(); continue;
    }

    if (mb==1 && mx>(ilxofs+ilxsize-9) && (mx<ilxofs+ilxsize-2)
    && my>(ilyofs+1) && (my<ilyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mb==1 && mx>ilxofs && mx<ilxofs+ilxsize && my>ilyofs && my<(ilyofs+9) && !moving)
    {
      moving=1;
      mxo=mx-ilxofs;
      myo=my-ilyofs;
      continue;
    }

    if (mb==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
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

      if (last_pressed==SCAN_ENTER)
      {
        key[SCAN_ENTER]=0;

        // keep minimally required padding
        if (ctf==1)
        {
          if (atoi(str)<minpadx)
            sprintf(str, "%d", minpadx);
        }
        if (ctf==2)
        {
          if (atoi(str)<minpady)
            sprintf(str, "%d", minpady);
        }

        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if (last_pressed==SCAN_TAB && !key[SCAN_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=nstr1; break;
          case 1: ctf=2; str=nstr2; break;
          case 2: ctf=0; str=0; break;
          //case 3: ctf=0; str=0; break;
        }
        key[SCAN_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (last_pressed==SCAN_TAB && key[SCAN_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=2; str=nstr2; break;
          case 1: ctf=0; str=0; break;
          case 2: ctf=1; str=nstr1; break;
          //case 3: ctf=2; str=nstr1; break;
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

      tt=strlen(str);
      str[tt]=c;
      str[tt+1]=0;
      last_pressed=0;
    }

  } while (!done);

  //num_padx=atoi(nstr1);
  //num_pady=atoi(nstr2);
}

int dtxsize=102, dtysize=52;
int dtxofs=100, dtyofs=70;
// aen
void Destroyer()
{ char done=0, moving=0;
  int mxo, myo, cursorblink;
  char ctf=0, cb=0, c, t, *str;
  int zn_check=0,zd_check=0,ob_check=0,ent_check=0;

  last_pressed=0;
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      dtxofs = (mx-mxo);
      dtyofs = (my-myo);
      if (!mb) moving=0;
      if (dtxofs<16) dtxofs = 16;
      if (dtyofs<16) dtyofs = 16;
      if (dtxofs+dtxsize>335) dtxofs = 336-dtxsize;
      if (dtyofs+dtysize>sy+16) dtyofs = (sy+16)-dtysize;
    }

    Window(dtxofs, dtyofs, dtxofs+dtxsize, dtyofs+dtysize, "Destroyer");
    CheckBox(dtxofs+4, dtyofs+11, zn_check);
    CheckBox(dtxofs+4, dtyofs+31, ob_check);
    CheckBox(dtxofs+4, dtyofs+21, zd_check);
    GotoXY(dtxofs+15, dtyofs+12); printstring("Zones");
    GotoXY(dtxofs+15, dtyofs+32); printstring("Obstructions");
    GotoXY(dtxofs+15, dtyofs+22); printstring("Zone Data");
    Button(dtxofs+68, dtyofs+11, "ok");
    Button(dtxofs+68, dtyofs+24, "cancel");
    CheckBox(dtxofs+4, dtyofs+41, ent_check);
    GotoXY(dtxofs+15, dtyofs+42); printstring("Entities");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mb==1 && mx>dtxofs+4 && mx<dtxofs+8+4 && my>dtyofs+11 && my<dtyofs+7+11 && !moving)
    {
      zn_check ^= 1; WaitRelease(); continue;
    }

    if (mb==1 && mx>dtxofs+4 && mx<dtxofs+8+4 && my>dtyofs+21 && my<dtyofs+7+21 && !moving)
    {
      zd_check ^= 1; WaitRelease(); continue;
    }

    if (mb==1 && mx>dtxofs+4 && mx<dtxofs+8+4 && my>dtyofs+31 && my<dtyofs+7+31 && !moving)
    {
      ob_check ^= 1; WaitRelease(); continue;
    }

    if (mb==1 && mx>dtxofs+4 && mx<dtxofs+8+4 && my>dtyofs+41 && my<dtyofs+7+41 && !moving)
    {
      ent_check ^= 1; WaitRelease(); continue;
    }

    if (mb==1 && mx>dtxofs+68 && mx<dtxofs+30+68 && my>dtyofs+11 && my<dtyofs+10+11 && !moving)
    {
      // insert code for button "ok" here
      ButtonPressed(dtxofs+68, dtyofs+11, "ok");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }

      if (Confirm("Are you sure?"))
      {
        done=1;
        if (ob_check) memset(obstruct, 0, (layer[0].sizex*layer[0].sizey)+4);
        if (zn_check) memset(zone, 0, (layer[0].sizex*layer[0].sizey)+4);
        if (zd_check)
        {
          numzones=0;
          memset(&zones, 0, sizeof zones);
        }
        if (ent_check)
        {
          entities=0;
          nmchr=0;
          nms=0;
          memset(&ms, 0, sizeof ms);
          memset(&entity, 0, sizeof entities);
          memset(&chrlist, 0, sizeof chrlist);
        }
      }
      WaitRelease(); continue;
    }

    if (mb==1 && mx>dtxofs+68 && mx<dtxofs+30+68 && my>dtyofs+24 && my<dtyofs+10+24 && !moving)
    {
      done=1;
      // insert code for button "cancel" here
      ButtonPressed(dtxofs+68, dtyofs+24, "cancel");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mb==1 && mx>(dtxofs+dtxsize-9) && (mx<dtxofs+dtxsize-2)
    && my>(dtyofs+1) && (my<dtyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mb==1 && mx>dtxofs && mx<dtxofs+dtxsize && my>dtyofs && my<(dtyofs+9) && !moving)
    {
      moving=1;
      mxo=mx-dtxofs;
      myo=my-dtyofs;
      continue;
    }

    if (mb==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
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

      if (last_pressed==SCAN_ENTER)
      {
        key[SCAN_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if (last_pressed==SCAN_TAB && !key[SCAN_LSHIFT])
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

      if (last_pressed==SCAN_TAB && key[SCAN_LSHIFT])
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
  } while (!done);
}

void LayersSubMenu(int dx, int dy)
{ char done=0;
  //int mxo, myo, cursorblink;

  do
  {
    if (dx<16) dx=16;
    if (dy<16) dy=16;
    if (dx+80>335) dx=336-80;
    if (dy+89>sy+16) dy=(sy+16)-89;

    RenderMap();
    RenderGUI();
    stdwindow(dx, dy, dx+80, dy+29);

    if (mx>dx && mx<dx+80)
    {
       if (my>dy+2 && my<dy+9) FilledBox(dx+2, dy+2, 76, 7, darkw);
       if (my>dy+8 && my<dy+15) FilledBox(dx+2, dy+8, 76, 7, darkw);
       if (my>dy+14 && my<dy+21) FilledBox(dx+2, dy+14, 76, 7, darkw);
       if (my>dy+20 && my<dy+26) FilledBox(dx+2, dy+20, 76, 7, darkw);
    }

    GotoXY(dx+3, dy+3); printstring("Add Layer");
    GotoXY(dx+3, dy+9); printstring("Edit Layer");
    GotoXY(dx+3, dy+15); printstring("Import Image");
    GotoXY(dx+3, dy+21); printstring("Destroyer");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    if (mb==1 && mx>dx && mx<dx+80)
    {
       if (my>dy+2 && my<dy+9) { AddLayer(); NoticeDialog(); done=1; WaitRelease(); break; }
       if (my>dy+8 && my<dy+15) { EditLayerProperties(); done=1; WaitRelease(); break; }
       if (my>dy+14 && my<dy+21) { ImageToLayer(); done=1; WaitRelease(); break; }
       if (my>dy+20 && my<dy+26) { Destroyer(); done=1; WaitRelease(); break; }
    }

    if (mb==1)
    {
      WaitRelease();
      done=1;
    }
    if (key[SCAN_ESC]==1) done=1;
  } while (!done);
  key[SCAN_ESC]=0;
}

void ExecuteSubMenu(int dx, int dy)
{ char done=0;
  //int mxo, myo, cursorblink;

  do
  {
    if (dx<16) dx=16;
    if (dy<16) dy=16;
    if (dx+80>335) dx=336-80;
    if (dy+89>sy+16) dy=(sy+16)-89;

    RenderMap();
    RenderGUI();
    stdwindow(dx, dy, dx+80, dy+23);

    if (mx>dx && mx<dx+80)
    {
       if (my>dy+2 && my<dy+9) FilledBox(dx+2, dy+2, 76, 7, darkw);
       if (my>dy+8 && my<dy+15) FilledBox(dx+2, dy+8, 76, 7, darkw);
       if (my>dy+14 && my<dy+21) FilledBox(dx+2, dy+14, 76, 7, darkw);
    }

    GotoXY(dx+3, dy+3); printstring("MAP");
    GotoXY(dx+3, dy+9); printstring("VERGE");
    GotoXY(dx+3, dy+15); printstring("DOS");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    if (mb==1 && mx>dx && mx<dx+80)
    {
       if (my>dy+2 && my<dy+9)   { ShellMAP();   done=1; WaitRelease(); break; }
       if (my>dy+8 && my<dy+15)  { ShellVERGE(); done=1; WaitRelease(); break; }
       if (my>dy+14 && my<dy+21) { ShellToDOS(); done=1; WaitRelease(); break; }
    }

    if (mb==1)
    {
      WaitRelease();
      done=1;
    }
    if (key[SCAN_ESC]==1) done=1;
  } while (!done);
  key[SCAN_ESC]=0;
}

void AddLayer()
{
  layers[numlayers]=(word *) valloc(layer[0].sizex * layer[0].sizey * 2,"layer data",numlayers);
  //memset(layers[numlayers], 0, layer[0].sizex * layer[0].sizey * 2);
  layer[numlayers].pmultx=1; layer[numlayers].pmulty=1;
  layer[numlayers].pdivx=1;  layer[numlayers].pdivy=1;
  layer[numlayers].sizex=layer[0].sizex;
  layer[numlayers].sizey=layer[0].sizey;
  layer[numlayers].trans=0;
  layer[numlayers].hline=0;
  numlayers++;
}

int lpxsize=190, lpysize=68;
int lpxofs=40, lpyofs=60;

void EditLayerProperties()
{ char done=0, moving=0;
  int mxo, myo, cursorblink, i;
  char ctf=0, cb=0, c, t, *str;
  char nstr1[80], nstr2[80], nstr3[80], nstr4[80], nstr5[80],nstr6[80];

  i=el;
  if (i>5) i=0;
  if (i>=numlayers) i=0;

  sprintf(nstr1, "%d", layer[i].pmultx);
  sprintf(nstr2, "%d", layer[i].pdivx);
  sprintf(nstr3, "%d", layer[i].pmulty);
  sprintf(nstr4, "%d", layer[i].pdivy);
  sprintf(nstr5, "%d", layer[i].trans);
  sprintf(nstr6, "%d", layer[i].hline);
  last_pressed=0;
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      lpxofs = (mx-mxo);
      lpyofs = (my-myo);
      if (!mb) moving=0;
      if (lpxofs<16) lpxofs = 16;
      if (lpyofs<16) lpyofs = 16;
      if (lpxofs+lpxsize>335) lpxofs = 336-lpxsize;
      if (lpyofs+lpysize>sy+16) lpyofs = (sy+16)-lpysize;
    }

    Window(lpxofs, lpyofs, lpxofs+lpxsize, lpyofs+lpysize, "Edit Layer Properties");
    GotoXY(lpxofs+3, lpyofs+10); printstring("Parallax controls:");
    GotoXY(lpxofs+4, lpyofs+21); printstring("MultX:");
    GotoXY(lpxofs+12, lpyofs+31); printstring("DivX:");
    GotoXY(lpxofs+93, lpyofs+21); printstring("MultY:");
    GotoXY(lpxofs+101, lpyofs+31); printstring("DivY:");
    TextField(lpxofs+29, lpyofs+19, 60, nstr1, ctf == 1 ? cb : 0);
    TextField(lpxofs+29, lpyofs+29, 60, nstr2, ctf == 2 ? cb : 0);
    TextField(lpxofs+118, lpyofs+19, 60, nstr3, ctf == 3 ? cb : 0);
    TextField(lpxofs+118, lpyofs+29, 60, nstr4, ctf == 4 ? cb : 0);
    HLine(lpxofs+1, lpyofs+43, 188, white);
    GotoXY(lpxofs+9, lpyofs+48); printstring("Transparent");
    GotoXY(lpxofs+36, lpyofs+58); printstring("HLine");
    TextField(lpxofs+56, lpyofs+46, 60, nstr5, ctf == 5 ? cb : 0);
    TextField(lpxofs+56, lpyofs+56, 60, nstr6, ctf == 6 ? cb : 0);
    Button(lpxofs+156, lpyofs+55, "OK");
    GotoXY(lpxofs+144, lpyofs+47); printstring("Layer");
    sprintf(strbuf,"%d",i); GotoXY(lpxofs+168, lpyofs+47); printstring(strbuf);

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mb==1 && mx>lpxofs+29 && mx<lpxofs+60+29 && my>lpyofs+19 && my<lpyofs+9+19 && !moving)
    {
      ctf=1; str=nstr1; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>lpxofs+29 && mx<lpxofs+60+29 && my>lpyofs+29 && my<lpyofs+9+29 && !moving)
    {
      ctf=2; str=nstr2; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>lpxofs+118 && mx<lpxofs+60+118 && my>lpyofs+19 && my<lpyofs+9+19 && !moving)
    {
      ctf=3; str=nstr3; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>lpxofs+118 && mx<lpxofs+60+118 && my>lpyofs+29 && my<lpyofs+9+29 && !moving)
    {
      ctf=4; str=nstr4; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>lpxofs+56 && mx<lpxofs+60+56 && my>lpyofs+46 && my<lpyofs+9+46 && !moving)
    {
      ctf=5; str=nstr5; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>lpxofs+56 && mx<lpxofs+60+56 && my>lpyofs+56 && my<lpyofs+9+56 && !moving)
    {
      ctf=6; str=nstr6; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if ((mb==1 && mx>lpxofs+156 && mx<lpxofs+30+156 && my>lpyofs+55 && my<lpyofs+10+55 && !moving) || (key[SCAN_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      ButtonPressed(lpxofs+156, lpyofs+55, "OK");
      DrawMouse(); done=1;
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mb==1 && mx>(lpxofs+lpxsize-9) && (mx<lpxofs+lpxsize-2)
    && my>(lpyofs+1) && (my<lpyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mb==1 && mx>lpxofs && mx<lpxofs+lpxsize && my>lpyofs && my<(lpyofs+9) && !moving)
    {
      moving=1;
      mxo=mx-lpxofs;
      myo=my-lpyofs;
      continue;
    }

    if (mb==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
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

      if (last_pressed==SCAN_UP)
      {
        layer[i].pmultx=atoi(nstr1);
        layer[i].pdivx=atoi(nstr2);
        layer[i].pmulty=atoi(nstr3);
        layer[i].pdivy=atoi(nstr4);
        layer[i].trans=atoi(nstr5);
        layer[i].hline=atoi(nstr6);
        last_pressed=0; key[SCAN_UP]=0;
        if (i) i--; else i=numlayers-1;
        sprintf(nstr1, "%d", layer[i].pmultx);
        sprintf(nstr2, "%d", layer[i].pdivx);
        sprintf(nstr3, "%d", layer[i].pmulty);
        sprintf(nstr4, "%d", layer[i].pdivy);
        sprintf(nstr5, "%d", layer[i].trans);
        sprintf(nstr6, "%d", layer[i].hline);
      }
      if (last_pressed==SCAN_DOWN)
      {
        layer[i].pmultx=atoi(nstr1);
        layer[i].pdivx=atoi(nstr2);
        layer[i].pmulty=atoi(nstr3);
        layer[i].pdivy=atoi(nstr4);
        layer[i].trans=atoi(nstr5);
        layer[i].hline=atoi(nstr6);
        last_pressed=0; key[SCAN_DOWN]=0;
        if (i<numlayers-1) i++; else i=0;
        sprintf(nstr1, "%d", layer[i].pmultx);
        sprintf(nstr2, "%d", layer[i].pdivx);
        sprintf(nstr3, "%d", layer[i].pmulty);
        sprintf(nstr4, "%d", layer[i].pdivy);
        sprintf(nstr5, "%d", layer[i].trans);
        sprintf(nstr6, "%d", layer[i].hline);
      }

      if (last_pressed==SCAN_ENTER)
      {
        key[SCAN_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if (last_pressed==SCAN_TAB && !key[SCAN_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=nstr1; break;
          case 1: ctf=2; str=nstr2; break;
          case 2: ctf=3; str=nstr3; break;
          case 3: ctf=4; str=nstr4; break;
          case 4: ctf=5; str=nstr5; break;
          case 5: ctf=6; str=nstr6; break;
          case 6: ctf=0; str=0; break;
        }
        key[SCAN_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (last_pressed==SCAN_TAB && key[SCAN_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=6; str=nstr6; break;
          case 1: ctf=0; str=0; break;
          case 2: ctf=1; str=nstr1; break;
          case 3: ctf=2; str=nstr2; break;
          case 4: ctf=3; str=nstr3; break;
          case 5: ctf=4; str=nstr4; break;
          case 6: ctf=5; str=nstr5; break;
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
  } while (!done);
  layer[i].pmultx=atoi(nstr1);
  layer[i].pdivx=atoi(nstr2);
  layer[i].pmulty=atoi(nstr3);
  layer[i].pdivy=atoi(nstr4);
  layer[i].trans=atoi(nstr5);
  layer[i].hline=atoi(nstr6);
}
/*
int lpxsize=190, lpysize=68;
int lpxofs=40, lpyofs=60;

void EditLayerProperties()
{ char done=0, moving=0;
  int mxo, myo, cursorblink, i;
  char ctf=0, cb=0, c, t, *str;
  char nstr1[80], nstr2[80], nstr3[80], nstr4[80];

  i=el;
  if (i>5) i=0;
  if (i>=numlayers) i=0;

  sprintf(nstr1, "%d", layer[i].pmultx);
  sprintf(nstr2, "%d", layer[i].pdivx);
  sprintf(nstr3, "%d", layer[i].pmulty);
  sprintf(nstr4, "%d", layer[i].pdivy);
  last_pressed=0;
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      lpxofs = (mx-mxo);
      lpyofs = (my-myo);
      if (!mb) moving=0;
      if (lpxofs<16) lpxofs = 16;
      if (lpyofs<16) lpyofs = 16;
      if (lpxofs+lpxsize>335) lpxofs = 336-lpxsize;
      if (lpyofs+lpysize>sy+16) lpyofs = (sy+16)-lpysize;
    }

    Window(lpxofs, lpyofs, lpxofs+lpxsize, lpyofs+lpysize, "Edit Layer Properties");
    GotoXY(lpxofs+3, lpyofs+10); printstring("Parallax controls:");
    GotoXY(lpxofs+4, lpyofs+21); printstring("MultX:");
    GotoXY(lpxofs+12, lpyofs+31); printstring("DivX:");
    GotoXY(lpxofs+93, lpyofs+21); printstring("MultY:");
    GotoXY(lpxofs+101, lpyofs+31); printstring("DivY:");
    TextField(lpxofs+29, lpyofs+19, 60, nstr1, ctf == 1 ? cb : 0);
    TextField(lpxofs+29, lpyofs+29, 60, nstr2, ctf == 2 ? cb : 0);
    TextField(lpxofs+118, lpyofs+19, 60, nstr3, ctf == 3 ? cb : 0);
    TextField(lpxofs+118, lpyofs+29, 60, nstr4, ctf == 4 ? cb : 0);
    HLine(lpxofs+1, lpyofs+43, 188, white);
    GotoXY(lpxofs+6, lpyofs+49); printstring("Transparent");
    GotoXY(lpxofs+32, lpyofs+56); printstring("HLine");
    CheckBox(lpxofs+56, lpyofs+48, layer[i].trans);
    CheckBox(lpxofs+56, lpyofs+56, layer[i].hline);
    Button(lpxofs+156, lpyofs+55, "OK");
    GotoXY(lpxofs+144, lpyofs+47); printstring("Layer");
    sprintf(strbuf,"%d",i); GotoXY(lpxofs+168, lpyofs+47); printstring(strbuf);

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mb==1 && mx>lpxofs+29 && mx<lpxofs+60+29 && my>lpyofs+19 && my<lpyofs+9+19 && !moving)
    {
      ctf=1; str=nstr1; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>lpxofs+29 && mx<lpxofs+60+29 && my>lpyofs+29 && my<lpyofs+9+29 && !moving)
    {
      ctf=2; str=nstr2; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>lpxofs+118 && mx<lpxofs+60+118 && my>lpyofs+19 && my<lpyofs+9+19 && !moving)
    {
      ctf=3; str=nstr3; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>lpxofs+118 && mx<lpxofs+60+118 && my>lpyofs+29 && my<lpyofs+9+29 && !moving)
    {
      ctf=4; str=nstr4; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>lpxofs+56 && mx<lpxofs+8+56 && my>lpyofs+48 && my<lpyofs+7+48 && !moving)
    {
      layer[i].trans ^= 1; WaitRelease(); continue;
    }

    if (mb==1 && mx>lpxofs+56 && mx<lpxofs+8+56 && my>lpyofs+56 && my<lpyofs+7+56 && !moving)
    {
      layer[i].hline ^= 1; WaitRelease(); continue;
    }

    if ((mb==1 && mx>lpxofs+156 && mx<lpxofs+30+156 && my>lpyofs+55 && my<lpyofs+10+55 && !moving) || (key[SCAN_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      done=1;
      ButtonPressed(lpxofs+156, lpyofs+55, "OK");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mb==1 && mx>(lpxofs+lpxsize-9) && (mx<lpxofs+lpxsize-2)
    && my>(lpyofs+1) && (my<lpyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mb==1 && mx>lpxofs && mx<lpxofs+lpxsize && my>lpyofs && my<(lpyofs+9) && !moving)
    {
      moving=1;
      mxo=mx-lpxofs;
      myo=my-lpyofs;
      continue;
    }

    if (mb==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
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

      if (last_pressed==SCAN_UP)
      {
        layer[i].pmultx=atoi(nstr1);
        layer[i].pdivx=atoi(nstr2);
        layer[i].pmulty=atoi(nstr3);
        layer[i].pdivy=atoi(nstr4);
        last_pressed=0; key[SCAN_UP]=0;
        if (i) i--; else i=numlayers-1;
        sprintf(nstr1, "%d", layer[i].pmultx);
        sprintf(nstr2, "%d", layer[i].pdivx);
        sprintf(nstr3, "%d", layer[i].pmulty);
        sprintf(nstr4, "%d", layer[i].pdivy);
      }
      if (last_pressed==SCAN_DOWN)
      {
        layer[i].pmultx=atoi(nstr1);
        layer[i].pdivx=atoi(nstr2);
        layer[i].pmulty=atoi(nstr3);
        layer[i].pdivy=atoi(nstr4);
        last_pressed=0; key[SCAN_DOWN]=0;
        if (i<numlayers-1) i++; else i=0;
        sprintf(nstr1, "%d", layer[i].pmultx);
        sprintf(nstr2, "%d", layer[i].pdivx);
        sprintf(nstr3, "%d", layer[i].pmulty);
        sprintf(nstr4, "%d", layer[i].pdivy);
      }

      if (last_pressed==SCAN_ENTER)
      {
        key[SCAN_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if (last_pressed==SCAN_TAB && !key[SCAN_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=nstr1; break;
          case 1: ctf=2; str=nstr2; break;
          case 2: ctf=3; str=nstr3; break;
          case 3: ctf=4; str=nstr4; break;
          case 4: ctf=0; str=0; break;
        }
        key[SCAN_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (last_pressed==SCAN_TAB && key[SCAN_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=4; str=nstr4; break;
          case 1: ctf=0; str=0; break;
          case 2: ctf=1; str=nstr1; break;
          case 3: ctf=2; str=nstr2; break;
          case 4: ctf=3; str=nstr3; break;
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
  } while (!done);

  layer[i].pmultx=atoi(nstr1);
  layer[i].pdivx=atoi(nstr2);
  layer[i].pmulty=atoi(nstr3);
  layer[i].pdivy=atoi(nstr4);
}
   */
void NewMAP()
{ int cursorblink;

    if (Confirm("Lose existing MAP?"))
    {
      for (cursorblink=0; cursorblink<numlayers; cursorblink++)
        vfree(layers[cursorblink]);
      memcpy(mapname,"UNTITLED.MAP",13);
      memcpy(vspname,"UNTITLED.VSP",13);
      memcpy(musname,"\0",1);
      memcpy(rstring,"1E",2);
      xstart=1; ystart=1;
      numlayers=1;
      wrap=0;

      // aen: default newmap dimensions set to 100x100
      layer[0].pmultx=1;  layer[0].pmulty=1;
      layer[0].pdivx=1;   layer[0].pdivy=1;
      layer[0].sizex=100; layer[0].sizey=100;
      layer[0].trans=0;   layer[0].hline=0;

      layers[0] = (unsigned short *)valloc((layer[0].sizex*layer[0].sizey*2)+4,"layer data",0);
      vfree(obstruct);
      obstruct=(char *) valloc((layer[0].sizex*layer[0].sizey)+4,"obstruct",0);
      vfree(zone);
      zone=(char *) valloc((layer[0].sizex*layer[0].sizey)+4,"zone",0);
      entities=0; // Mordred

      // aen: default number of tiles set to 100
      numtiles=100;
      vfree(vsp);
      vsp=(char *) valloc(256 * numtiles,"vsp data",0);

      // zero: was re-allocating same memory so gfx still visible in some places
      memset(vsp,0,256*numtiles);
      // zero: gotta clear that anim stuff man!
      memset(vspanim,0,800);
      InitTileIDX();



      layertoggle[0]=1; layertoggle[1]=0; layertoggle[2]=0; layertoggle[3]=0;
      layertoggle[4]=0; layertoggle[5]=0; layertoggle[6]=0; layertoggle[7]=0;
      xwin=0; ywin=0;
    }
}

int amxsize=150, amysize=88;
int amxofs=60, amyofs=60;

void About()
{
  char done=0, moving=0;
  int mxo, myo, cursorblink;
  char ctf=0, cb=0, c, t, *str;

  last_pressed=0;

  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      amxofs = (mx-mxo);
      amyofs = (my-myo);
      if (!mb) moving=0;
      if (amxofs<16) amxofs = 16;
      if (amyofs<16) amyofs = 16;
      if (amxofs+amxsize>335) amxofs = 336-amxsize;
      if (amyofs+amysize>sy+16) amyofs = (sy+16)-amysize;
    }

    sprintf(strbuf, "About MapEd %s", ME2_VERSION);
    Window(amxofs, amyofs, amxofs+amxsize, amyofs+amysize, strbuf);
    sprintf(strbuf, "MapEd v.%s", ME2_VERSION);
    GotoXY(amxofs+52, amyofs+16); printstring(strbuf);
    GotoXY(amxofs+31, amyofs+22); printstring("Copyright (C) 1998 vecna");
    GotoXY(amxofs+40, amyofs+28); printstring("All Rights Reserved");
    GotoXY(amxofs+(75-pixels("Additional code by Ric")/2), amyofs+41);
    printstring("Additional code by Ric");
    GotoXY(amxofs+(75-pixels("Additional code by aen")/2), amyofs+47);
    printstring("Additional code by aen");
    GotoXY(amxofs+(75-pixels("Additional code by zeromus")/2), amyofs+53);
    printstring("Additional code by zeromus");
    Button(amxofs+60, amyofs+68, "OK");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if ((mb==1 && mx>amxofs+66 && mx<amxofs+30+66 && my>amyofs+68 && my<amyofs+10+68 && !moving) || (key[SCAN_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      done=1;
      ButtonPressed(amxofs+60, amyofs+68, "OK");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mb==1 && mx>(amxofs+amxsize-9) && (mx<amxofs+amxsize-2) &&
        my>(amyofs+1) && (my<amyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mb==1 && mx>amxofs && mx<amxofs+amxsize && my>amyofs && my<(amyofs+9) && !moving)
    {
      moving=1;
      mxo=mx-amxofs;
      myo=my-amyofs;
      continue;
    }

    if (mb==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
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

      if (last_pressed==SCAN_ENTER)
      {
        key[SCAN_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
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

  } while (!done);
}

int cnxsize=125, cnysize=50;
int cnxofs=114, cnyofs=70;

int Confirm(char *message)
{ char done=0, moving=0;
  int mxo, myo, cursorblink;
  char ctf=0, cb=0, c, t, *str;

  last_pressed=0;
  WaitRelease();
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      cnxofs = (mx-mxo);
      cnyofs = (my-myo);
      if (!mb) moving=0;
      if (cnxofs<16) cnxofs = 16;
      if (cnyofs<16) cnyofs = 16;
      if (cnxofs+cnxsize>335) cnxofs = 336-cnxsize;
      if (cnyofs+cnysize>sy+16) cnyofs = (sy+16)-cnysize;
    }

    Window(cnxofs, cnyofs, cnxofs+cnxsize, cnyofs+cnysize, "Confirm? (Y/N)");
    GotoXY(cnxofs+125/2-(pixels(message)/2), cnyofs+15);  printstring(message);

    Button(cnxofs+24, cnyofs+30, "Yes");
    Button(cnxofs+69, cnyofs+30, "No");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mb==1 && mx>cnxofs+24 && mx<cnxofs+30+24 && my>cnyofs+30 && my<cnyofs+10+30 && !moving)
    {
      // insert code for button "Yes" here
      ButtonPressed(cnxofs+24, cnyofs+30, "Yes");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease();
      return 1;
    }

    if (mb==1 && mx>cnxofs+69 && mx<cnxofs+30+69 && my>cnyofs+30 && my<cnyofs+10+30 && !moving)
    {
      ButtonPressed(cnxofs+69, cnyofs+30, "No");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease();
      return 0;
    }

    if (mb==1 && mx>(cnxofs+cnxsize-9) && (mx<cnxofs+cnxsize-2)
        && my>(cnyofs+1) && (my<cnyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mb==1 && mx>cnxofs && mx<cnxofs+cnxsize && my>cnyofs && my<(cnyofs+9) && !moving)
    {
      moving=1;
      mxo=mx-cnxofs;
      myo=my-cnyofs;
      continue;
    }

    if (mb==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
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
      if (last_pressed==SCAN_N)
      {
        key[SCAN_N]=0;
        done=1;
        break;
      }
      if (last_pressed==SCAN_Y)
      {
        key[SCAN_Y]=0;
        return 1;
      }

      if (last_pressed==SCAN_ENTER)
      {
        key[SCAN_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
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

  } while (!done);
  return 0;
}

int lmxsize=145, lmysize=36;
int lmxofs=80, lmyofs=50;

void LoadMAPDialog()
{ char lmapname[80];
  char done=0, moving=0;
  int mxo, myo, cursorblink;
  char ctf=0, cb=0, c, t, *str;

  memset(lmapname, 0, 80);
  last_pressed=0;
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      lmxofs = (mx-mxo);
      lmyofs = (my-myo);
      if (!mb) moving=0;
      if (lmxofs<16) lmxofs = 16;
      if (lmyofs<16) lmyofs = 16;
      if (lmxofs+lmxsize>335) lmxofs = 336-lmxsize;
      if (lmyofs+lmysize>sy+16) lmyofs = (sy+16)-lmysize;
    }

    Window(lmxofs, lmyofs, lmxofs+lmxsize, lmyofs+lmysize, "Load MAP");
    GotoXY(lmxofs+2, lmyofs+13); printstring("Load MAP:");
    TextField(lmxofs+38, lmyofs+11, 70, lmapname, ctf == 1 ? cb : 0);
    Button(lmxofs+79, lmyofs+22, "OK");
    Button(lmxofs+111, lmyofs+22, "Cancel");
    Button(lmxofs+111, lmyofs+11, "Browse");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mb==1 && mx>lmxofs+38 && mx<lmxofs+70+38 && my>lmyofs+11 && my<lmyofs+9+11 && !moving)
    {
      ctf=1; str=lmapname; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if ((mb==1 && mx>lmxofs+79 && mx<lmxofs+30+79 && my>lmyofs+22 && my<lmyofs+10+22 && !moving) || (key[SCAN_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      if (modified)
         if (!Confirm("Lose unsaved changes?")) { done=1; continue; }
      for (cursorblink=0; cursorblink<numlayers; cursorblink++)
           vfree(layers[cursorblink]);
      vfree(obstruct);
      vfree(zone);
      vfree(vsp);

      memcpy(mapname, lmapname, strlen(lmapname)+1);
      LoadMAP(mapname);
      InitTileIDX();
      xwin=0; ywin=0; done=1;

      ButtonPressed(lmxofs+79, lmyofs+22, "OK");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mb==1 && mx>lmxofs+111 && mx<lmxofs+30+111 && my>lmyofs+22 && my<lmyofs+10+22 && !moving)
    {
      // insert code for button "Cancel" here
      ButtonPressed(lmxofs+111, lmyofs+22, "Cancel");
      DrawMouse(); done=1;
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if ((mb==1 && mx>lmxofs+111 && mx<lmxofs+30+111 && my>lmyofs+11 && my<lmyofs+10+11 && !moving) || (key[SCAN_ALT] && key[SCAN_B]))
    {
      // insert code for button "Browse" here
      ButtonPressed(lmxofs+111, lmyofs+11, "Browse");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease();
      Browse(1,"*.map","","","",lmapname);
      continue;
    }

    if (mb==1 && mx>(lmxofs+lmxsize-9) && (mx<lmxofs+lmxsize-2)
    && my>(lmyofs+1) && (my<lmyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mb==1 && mx>lmxofs && mx<lmxofs+lmxsize && my>lmyofs && my<(lmyofs+9) && !moving)
    {
      moving=1;
      mxo=mx-lmxofs;
      myo=my-lmyofs;
      continue;
    }

    if (mb==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
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

      if (last_pressed==SCAN_ENTER)
      {
        key[SCAN_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if (last_pressed==SCAN_TAB)
      {
        switch (ctf)
        {
          case 0: ctf=1; str=lmapname; break;
          case 1: ctf=0; str=0; break;
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

  } while (!done);
}

int lvxsize=145, lvysize=35;
int lvxofs=80, lvyofs=50;

void LoadVSPDialog()
{ char done=0, moving=0;
  int mxo, myo, cursorblink;
  char ctf=0, cb=0, c, t, *str;
  char lvspname[80];

  last_pressed=0;
  memset(lvspname,0,80);
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      lvxofs = (mx-mxo);
      lvyofs = (my-myo);
      if (!mb) moving=0;
      if (lvxofs<16) lvxofs = 16;
      if (lvyofs<16) lvyofs = 16;
      if (lvxofs+lvxsize>335) lvxofs = 336-lvxsize;
      if (lvyofs+lvysize>sy+16) lvyofs = (sy+16)-lvysize;
    }

    Window(lvxofs, lvyofs, lvxofs+lvxsize, lvyofs+lvysize, "Load VSP");
    GotoXY(lvxofs+2, lvyofs+13); printstring("Load VSP:");
    TextField(lvxofs+38, lvyofs+11, 70, lvspname, ctf == 1 ? cb : 0);
    Button(lvxofs+79, lvyofs+21, "OK");
    Button(lvxofs+111, lvyofs+21, "Cancel");
    Button(lvxofs+111, lvyofs+10, "Browse");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mb==1 && mx>lvxofs+38 && mx<lvxofs+70+38 && my>lvyofs+11 && my<lvyofs+9+11 && !moving)
    {
      ctf=1; str=lvspname; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if ((mb==1 && mx>lvxofs+79 && mx<lvxofs+30+79 && my>lvyofs+21 && my<lvyofs+10+21 && !moving) || (key[SCAN_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      vfree(vsp);
      memcpy(vspname, lvspname, strlen(lvspname)+1);
      LoadVSP(vspname);
      InitTileIDX();
      ButtonPressed(lvxofs+79, lvyofs+21, "OK");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      done=1;
      WaitRelease(); continue;
    }

    if (mb==1 && mx>lvxofs+111 && mx<lvxofs+30+111 && my>lvyofs+21 && my<lvyofs+10+21 && !moving)
    {
      // insert code for button "Cancel" here
      ButtonPressed(lvxofs+111, lvyofs+21, "Cancel");
      DrawMouse(); done=1;
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if ((mb==1 && mx>lvxofs+111 && mx<lvxofs+30+111 && my>lvyofs+10 && my<lvyofs+10+10 && !moving) || (key[SCAN_ALT] && key[SCAN_B]))
    {
      // insert code for button "Browse" here
      ButtonPressed(lvxofs+111, lvyofs+10, "Browse");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      Browse(1,"*.vsp","","","",lvspname);
      WaitRelease(); continue;
    }

    if (mb==1 && mx>(lvxofs+lvxsize-9) && (mx<lvxofs+lvxsize-2)
    && my>(lvyofs+1) && (my<lvyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mb==1 && mx>lvxofs && mx<lvxofs+lvxsize && my>lvyofs && my<(lvyofs+9) && !moving)
    {
      moving=1;
      mxo=mx-lvxofs;
      myo=my-lvyofs;
      continue;
    }

    if (mb==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
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

      if (last_pressed==SCAN_ENTER)
      {
        key[SCAN_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if (last_pressed==SCAN_TAB && !key[SCAN_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=lvspname; break;
          case 1: ctf=0; str=0; break;
        }
        key[SCAN_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (last_pressed==SCAN_TAB && key[SCAN_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=lvspname; break;
          case 1: ctf=0; str=lvspname; break;
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
  } while (!done);
}
int mpxsize=175, mpysize=37;
int mpxofs=60, mpyofs=60;

void MPDialog()
{ char done=0, moving=0, playmod[80];
  int mxo, myo, cursorblink;
  char ctf=0, cb=0, c, t, *str;

  last_pressed=0;
  memset(playmod, 0, 80);
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      mpxofs = (mx-mxo);
      mpyofs = (my-myo);
      if (!mb) moving=0;
      if (mpxofs<16) mpxofs = 16;
      if (mpyofs<16) mpyofs = 16;
      if (mpxofs+mpxsize>335) mpxofs = 336-mpxsize;
      if (mpyofs+mpysize>sy+16) mpyofs = (sy+16)-mpysize;
    }

    Window(mpxofs, mpyofs, mpxofs+mpxsize, mpyofs+mpysize, "MOD Player Controls");
    GotoXY(mpxofs+2, mpyofs+15); printstring("MOD Playing:");
    TextField(mpxofs+49, mpyofs+13, 90, playmod, ctf == 1 ? cb : 0);
    Button(mpxofs+110, mpyofs+24, "Play");
    Button(mpxofs+142, mpyofs+24, "Stop");
    Button(mpxofs+78, mpyofs+24, "OK");
    Button(mpxofs+142, mpyofs+12, "Browse");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mb==1 && mx>mpxofs+49 && mx<mpxofs+90+49 && my>mpyofs+13 && my<mpyofs+9+13 && !moving)
    {
      ctf=1; str=playmod; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>mpxofs+110 && mx<mpxofs+30+110 && my>mpyofs+24 && my<mpyofs+10+24 && !moving)
    {
      // insert code for button "Play" here
      ButtonPressed(mpxofs+110, mpyofs+24, "Play");
      PlayMusic(playmod);
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mb==1 && mx>mpxofs+142 && mx<mpxofs+30+142 && my>mpyofs+24 && my<mpyofs+10+24 && !moving)
    {
      // insert code for button "Stop" here
      memset(playingmod, 0, 80);
      memset(playmod, 0, 80);
      StopMusic();
      ButtonPressed(mpxofs+142, mpyofs+24, "Stop");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if ((mb==1 && mx>mpxofs+78 && mx<mpxofs+30+78 && my>mpyofs+24 && my<mpyofs+10+24 && !moving) || (key[SCAN_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      done=1;
      ButtonPressed(mpxofs+78, mpyofs+24, "OK");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if ((mb==1 && mx>mpxofs+142 && mx<mpxofs+30+142 && my>mpyofs+12 && my<mpyofs+10+12 && !moving) || (key[SCAN_ALT] && key[SCAN_B]))
    {
      // insert code for button "Browse" here
      ButtonPressed(mpxofs+142, mpyofs+12, "Browse");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      Browse(4,"*.s3m","*.mod","*.xm",".it",playmod);
      WaitRelease(); continue;
    }

    if (mb==1 && mx>(mpxofs+mpxsize-9) && (mx<mpxofs+mpxsize-2)
    && my>(mpyofs+1) && (my<mpyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mb==1 && mx>mpxofs && mx<mpxofs+mpxsize && my>mpyofs && my<(mpyofs+9) && !moving)
    {
      moving=1;
      mxo=mx-mpxofs;
      myo=my-mpyofs;
      continue;
    }

    if (mb==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
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

      if (last_pressed==SCAN_ENTER)
      {
        key[SCAN_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if (last_pressed==SCAN_TAB && !key[SCAN_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=playmod; break;
          case 1: ctf=0; str=0; break;
        }
        key[SCAN_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (last_pressed==SCAN_TAB && key[SCAN_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=playmod; break;
          case 1: ctf=0; str=playmod; break;
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
  } while (!done);
}

int zexsize=135, zeysize=86;
int zexofs=80, zeyofs=40;

void ZoneEdDialog()
{ char done=0, moving=0;
  int mxo, myo, cursorblink, i=0;
  char ctf=0, cb=0, c, t, *str;
  char nstr1[80], nstr2[80], nstr3[80], nstr4[80];

  i=curzone;
  sprintf(nstr1, "%d", zones[i].script);
  sprintf(nstr2, "%d", zones[i].percent);
  sprintf(nstr3, "%d", zones[i].delay);
  sprintf(nstr4, "%d", zones[i].entityscript);
  last_pressed=0;
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      zexofs = (mx-mxo);
      zeyofs = (my-myo);
      if (!mb) moving=0;
      if (zexofs<16) zexofs = 16;
      if (zeyofs<16) zeyofs = 16;
      if (zexofs+zexsize>335) zexofs = 336-zexsize;
      if (zeyofs+zeysize>sy+16) zeyofs = (sy+16)-zeysize;
    }

    Window(zexofs, zeyofs, zexofs+zexsize, zeyofs+zeysize, "Zone Editor");
    GotoXY(zexofs+8, zeyofs+12); printstring("Name:");
    TextField(zexofs+30, zeyofs+10, 100, zones[i].name, ctf == 1 ? cb : 0);
    GotoXY(zexofs+5, zeyofs+22); printstring("Script:");
    TextField(zexofs+30, zeyofs+20, 100, nstr1, ctf == 2 ? cb : 0);
    GotoXY(zexofs+2, zeyofs+32); printstring("Chance:");
    TextField(zexofs+30, zeyofs+30, 100, nstr2, ctf == 3 ? cb : 0);
    GotoXY(zexofs+7, zeyofs+42); printstring("Delay:");
    TextField(zexofs+30, zeyofs+40, 100, nstr3, ctf == 4 ? cb : 0);
    GotoXY(zexofs+4, zeyofs+52); printstring("Entity:");
    TextField(zexofs+30, zeyofs+50, 100, nstr4, ctf == 5 ? cb : 0);
    CheckBox(zexofs+20, zeyofs+61, zones[i].aaa);
    GotoXY(zexofs+31, zeyofs+62); printstring("Allow Adjacent Activation");
    Button(zexofs+99, zeyofs+71, "OK");
    GotoXY(zexofs+12, zeyofs+73); printstring("Zone");
    GotoXY(zexofs+30, zeyofs+73);
    sprintf(strbuf,"%d",i); printstring(strbuf);

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mb==1 && mx>zexofs+30 && mx<zexofs+100+30 && my>zeyofs+10 && my<zeyofs+9+10 && !moving)
    {
      ctf=1; str=zones[i].name; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>zexofs+30 && mx<zexofs+100+30 && my>zeyofs+20 && my<zeyofs+9+20 && !moving)
    {
      ctf=2; str=nstr1; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>zexofs+30 && mx<zexofs+100+30 && my>zeyofs+30 && my<zeyofs+9+30 && !moving)
    {
      ctf=3; str=nstr2; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>zexofs+30 && mx<zexofs+100+30 && my>zeyofs+40 && my<zeyofs+9+40 && !moving)
    {
      ctf=4; str=nstr3; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>zexofs+30 && mx<zexofs+100+30 && my>zeyofs+50 && my<zeyofs+9+50 && !moving)
    {
      ctf=5; str=nstr4; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>zexofs+20 && mx<zexofs+8+20 && my>zeyofs+61 && my<zeyofs+7+61 && !moving)
    {
      zones[i].aaa ^= 1; WaitRelease(); continue;
    }

    if ((mb==1 && mx>zexofs+99 && mx<zexofs+30+99 && my>zeyofs+71 && my<zeyofs+10+71 && !moving) || (key[SCAN_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      done=1;
      ButtonPressed(zexofs+99, zeyofs+71, "OK");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mb==1 && mx>(zexofs+zexsize-9) && (mx<zexofs+zexsize-2)
    && my>(zeyofs+1) && (my<zeyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mb==1 && mx>zexofs && mx<zexofs+zexsize && my>zeyofs && my<(zeyofs+9) && !moving)
    {
      moving=1;
      mxo=mx-zexofs;
      myo=my-zeyofs;
      continue;
    }

    if (mb==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
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
      if (last_pressed==SCAN_UP)
      {
        zones[i].script=atoi(nstr1);
        zones[i].percent=atoi(nstr2);
        zones[i].delay=atoi(nstr3);
        zones[i].entityscript=atoi(nstr4);
        if (i<255) i++; else i=0;
        last_pressed=0; key[SCAN_UP]=0;
        sprintf(nstr1, "%d", zones[i].script);
        sprintf(nstr2, "%d", zones[i].percent);
        sprintf(nstr3, "%d", zones[i].delay);
        sprintf(nstr4, "%d", zones[i].entityscript);
        continue;
      }
      if (last_pressed==SCAN_DOWN)
      {
        zones[i].script=atoi(nstr1);
        zones[i].percent=atoi(nstr2);
        zones[i].delay=atoi(nstr3);
        zones[i].entityscript=atoi(nstr4);
        if (i) i--; else i=255;
        last_pressed=0; key[SCAN_DOWN]=0;
        sprintf(nstr1, "%d", zones[i].script);
        sprintf(nstr2, "%d", zones[i].percent);
        sprintf(nstr3, "%d", zones[i].delay);
        sprintf(nstr4, "%d", zones[i].entityscript);
        continue;
      }
      if (last_pressed==SCAN_ENTER)
      {
        key[SCAN_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if (last_pressed==SCAN_TAB && !key[SCAN_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=zones[i].name; break;
          case 1: ctf=2; str=nstr1; break;
          case 2: ctf=3; str=nstr2; break;
          case 3: ctf=4; str=nstr3; break;
          case 4: ctf=5; str=nstr4; break;
          case 5: ctf=0; str=0; break;
        }
        key[SCAN_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (last_pressed==SCAN_TAB && key[SCAN_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=5; str=nstr4; break;
          case 1: ctf=0; str=0; break;
          case 2: ctf=1; str=zones[i].name; break;
          case 3: ctf=2; str=nstr1; break;
          case 4: ctf=3; str=nstr2; break;
          case 5: ctf=4; str=nstr3; break;
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

  } while (!done);
  curzone=i;
  zones[i].script=atoi(nstr1);
  zones[i].percent=atoi(nstr2);
  zones[i].delay=atoi(nstr3);
  zones[i].entityscript=atoi(nstr4);
}

int exsize=180, eysize=175;
int exofs=70, eyofs=30;

void EntityEditor(int i)
{ char done=0, moving=0;
  int mxo, myo, cursorblink;
  char ctf=0, cb=0, c, t, *str;
  char nstr1[80], nstr2[80], nstr3[80], nstr4[80], nstr5[80], nstr6[80], nstr7[80], nstr8[80], nstr9[80], nstr10[80], nstr11[80], nstr12[80], nstr13[80];

  sprintf(nstr1, "%d", entity[i].x);
  sprintf(nstr2, "%d", entity[i].y);
  sprintf(nstr3, "%d", entity[i].chrindex);
  sprintf(nstr4, "%d", entity[i].speed);
  sprintf(nstr5, "%d", entity[i].actscript);
  sprintf(nstr6, "%d", entity[i].movecode);
  sprintf(nstr7, "%d", entity[i].step);
  sprintf(nstr8, "%d", entity[i].data2);
  sprintf(nstr9, "%d", entity[i].data3);
  sprintf(nstr10, "%d", entity[i].delay);
  sprintf(nstr11, "%d", entity[i].data5);
  sprintf(nstr12, "%d", entity[i].data6);
  sprintf(nstr13, "%d", entity[i].movescript);
  last_pressed=0;
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      exofs = (mx-mxo);
      eyofs = (my-myo);
      if (!mb) moving=0;
      if (exofs<16) exofs = 16;
      if (eyofs<16) eyofs = 16;
      if (exofs+exsize>335) exofs = 336-exsize;
      if (eyofs+eysize>sy+16) eyofs = (sy+16)-eysize;
    }

    Window(exofs, eyofs, exofs+exsize, eyofs+eysize, "Entity Editor");
    GotoXY(exofs+7, eyofs+13); printstring("Desc:");
    TextField(exofs+26, eyofs+11, 80, entity[i].desc, ctf == 1 ? cb : 0);
    GotoXY(exofs+19, eyofs+23); printstring("X:");
    TextField(exofs+26, eyofs+21, 80, nstr1, ctf == 2 ? cb : 0);
    GotoXY(exofs+19, eyofs+33); printstring("Y:");
    TextField(exofs+26, eyofs+31, 80, nstr2, ctf == 3 ? cb : 0);
    GotoXY(exofs+11, eyofs+43); printstring("CHR:");
    TextField(exofs+26, eyofs+41, 80, nstr3, ctf == 4 ? cb : 0);

    // ***
    // entity image goes here (just above chr list button)
    // ***

    BigButton(exofs+116, eyofs+61, "CHR List");
    BigButton(exofs+115, eyofs+153, "Scripts");
    GotoXY(exofs+2, eyofs+155); printstring("Movescript:");
    HLine(exofs+1, eyofs+73, 178, white);
    GotoXY(exofs+3, eyofs+53); printstring("Speed:");
    TextField(exofs+26, eyofs+51, 80, nstr4, ctf == 5 ? cb : 0);
    TextField(exofs+26, eyofs+61, 80, nstr5, ctf == 6 ? cb : 0);
    GotoXY(exofs+107, eyofs+79); printstring("Is Obstruction");
    CheckBox(exofs+161, eyofs+87, entity[i].obsmode1);
    GotoXY(exofs+102, eyofs+88); printstring("Is Obstructable");
    CheckBox(exofs+161, eyofs+78, entity[i].obsmode2);
    GotoXY(exofs+24, eyofs+79); printstring("Auto Face");
    CheckBox(exofs+61, eyofs+78, entity[i].face);
    GotoXY(exofs+10, eyofs+88); printstring("Activ. Method");
    CheckBox(exofs+61, eyofs+87, entity[i].actm);
    HLine(exofs+1, eyofs+98, 178, white);
    GotoXY(exofs+126, eyofs+166); printstring("Entity");
    GotoXY(exofs+6, eyofs+104); printstring("Move Code:");
    TextField(exofs+47, eyofs+103, 80, nstr6, ctf == 7 ? cb : 0);
    TextField(exofs+30, eyofs+115, 40, nstr7, ctf == 8 ? cb : 0);
    TextField(exofs+30, eyofs+125, 40, nstr8, ctf == 9 ? cb : 0);
    TextField(exofs+30, eyofs+135, 40, nstr9, ctf == 10 ? cb : 0);
    TextField(exofs+113, eyofs+115, 40, nstr10, ctf == 11 ? cb : 0);
    TextField(exofs+113, eyofs+125, 40, nstr11, ctf == 12 ? cb : 0);
    TextField(exofs+113, eyofs+135, 40, nstr12, ctf == 13 ? cb : 0);
    HLine(exofs+1, eyofs+148, 178, white);
    TextField(exofs+46, eyofs+153, 60, nstr13, ctf == 14 ? cb : 0);
    GotoXY(exofs+5, eyofs+63); printstring("Activ:");
    GotoXY(exofs+152, eyofs+166);
    sprintf(strbuf, "%d/%d", i, entities-1); printstring(strbuf);
    entity[i].movecode=atoi(nstr6);
    if (ctf!=4) entity[i].chrindex=atoi(nstr3);

    switch (entity[i].movecode)
    {
      case 0: GotoXY(exofs+131, eyofs+104); printstring("Stopped"); break;
      case 1: GotoXY(exofs+131, eyofs+104); printstring("Wander");
              GotoXY(exofs+11, eyofs+116); printstring("Step:");
              GotoXY(exofs+90, eyofs+116); printstring("Delay:"); break;
      case 2: GotoXY(exofs+131, eyofs+104); printstring("Wander Zone");
              GotoXY(exofs+11, eyofs+116); printstring("Step:");
              GotoXY(exofs+90, eyofs+116); printstring("Delay:");
              GotoXY(exofs+10, eyofs+126); printstring("Zone:"); break;
      case 3: GotoXY(exofs+131, eyofs+104); printstring("Wander Box");
              GotoXY(exofs+11, eyofs+116); printstring("Step:");
              GotoXY(exofs+90, eyofs+116); printstring("Delay:");
              GotoXY(exofs+21, eyofs+126); printstring("X1:");
              GotoXY(exofs+21, eyofs+136); printstring("Y1:");
              GotoXY(exofs+102, eyofs+126); printstring("X2:");
              GotoXY(exofs+102, eyofs+136); printstring("Y2:"); break;
      case 4: GotoXY(exofs+131, eyofs+104); printstring("Scripted"); break;
    }
    switch (entity[i].actm)
    {
      case 0: GotoXY(exofs+72, eyofs+88); printstring("Actv"); break;
      case 1: GotoXY(exofs+72, eyofs+88); printstring("Adjc"); break;
    }

    if (chrs[entity[i].chrindex].frame)
      TCopySprite(exofs+143-(chrs[entity[i].chrindex].fx/2),
                  eyofs+31-(chrs[entity[i].chrindex].fy/2),
                  chrs[entity[i].chrindex].fx, chrs[entity[i].chrindex].fy,
                  chrs[entity[i].chrindex].frame);

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mb==1 && mx>exofs+26 && mx<exofs+80+26 && my>eyofs+11 && my<eyofs+9+11 && !moving)
    {
      ctf=1; str=entity[i].desc; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>exofs+26 && mx<exofs+80+26 && my>eyofs+21 && my<eyofs+9+21 && !moving)
    {
      ctf=2; str=nstr1; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>exofs+26 && mx<exofs+80+26 && my>eyofs+31 && my<eyofs+9+31 && !moving)
    {
      ctf=3; str=nstr2; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>exofs+26 && mx<exofs+80+26 && my>eyofs+41 && my<eyofs+9+41 && !moving)
    {
      ctf=4; str=nstr3; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>exofs+116 && mx<exofs+50+116 && my>eyofs+61 && my<eyofs+10+61 && !moving)
    {
      // insert code for button "CHR List" here
      BigButtonPressed(exofs+116, eyofs+61, "CHR List");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      CHRList();
//      DoCHRdealy();
      WaitRelease(); continue;
    }

    if (mb==1 && mx>exofs+26 && mx<exofs+80+26 && my>eyofs+51 && my<eyofs+9+51 && !moving)
    {
      ctf=5; str=nstr4; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>exofs+26 && mx<exofs+80+26 && my>eyofs+61 && my<eyofs+9+61 && !moving)
    {
      ctf=6; str=nstr5; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>exofs+115 && mx<exofs+50+115 && my>eyofs+153 && my<eyofs+10+153 && !moving)
    {
      // insert code for button "Scripts..." here
      BigButtonPressed(exofs+115, eyofs+153, "Scripts");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      MoveScriptDialog();
      WaitRelease(); continue;
    }

    if (mb==1 && mx>exofs+161 && mx<exofs+8+161 && my>eyofs+87 && my<eyofs+7+87 && !moving)
    {
      entity[i].obsmode1 ^= 1; WaitRelease(); continue;
    }

    if (mb==1 && mx>exofs+161 && mx<exofs+8+161 && my>eyofs+78 && my<eyofs+7+78 && !moving)
    {
      entity[i].obsmode2 ^= 1; WaitRelease(); continue;
    }

    if (mb==1 && mx>exofs+61 && mx<exofs+8+61 && my>eyofs+78 && my<eyofs+7+78 && !moving)
    {
      entity[i].face ^= 1; WaitRelease(); continue;
    }

    if (mb==1 && mx>exofs+61 && mx<exofs+8+61 && my>eyofs+87 && my<eyofs+7+87 && !moving)
    {
      entity[i].actm ^= 1; WaitRelease(); continue;
    }

    if (mb==1 && mx>exofs+47 && mx<exofs+80+47 && my>eyofs+103 && my<eyofs+9+103 && !moving)
    {
      ctf=7; str=nstr6; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>exofs+30 && mx<exofs+40+30 && my>eyofs+115 && my<eyofs+9+115 && !moving)
    {
      ctf=8; str=nstr7; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>exofs+30 && mx<exofs+40+30 && my>eyofs+125 && my<eyofs+9+125 && !moving)
    {
      ctf=9; str=nstr8; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>exofs+30 && mx<exofs+40+30 && my>eyofs+135 && my<eyofs+9+135 && !moving)
    {
      ctf=10; str=nstr9; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>exofs+113 && mx<exofs+40+113 && my>eyofs+115 && my<eyofs+9+115 && !moving)
    {
      ctf=11; str=nstr10; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>exofs+113 && mx<exofs+40+113 && my>eyofs+125 && my<eyofs+9+125 && !moving)
    {
      ctf=12; str=nstr11; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>exofs+113 && mx<exofs+40+113 && my>eyofs+135 && my<eyofs+9+135 && !moving)
    {
      ctf=13; str=nstr12; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>exofs+46 && mx<exofs+60+46 && my>eyofs+153 && my<eyofs+9+153 && !moving)
    {
      ctf=14; str=nstr13; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>(exofs+exsize-9) && (mx<exofs+exsize-2)
    && my>(eyofs+1) && (my<eyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mb==1 && mx>exofs && mx<exofs+exsize && my>eyofs && my<(eyofs+9) && !moving)
    {
      moving=1;
      mxo=mx-exofs;
      myo=my-eyofs;
      continue;
    }

    if (mb==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
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

      if (last_pressed==SCAN_UP)
      {
        entity[i].x=atoi(nstr1);
        entity[i].y=atoi(nstr2);
        entity[i].chrindex=atoi(nstr3);
        entity[i].speed=atoi(nstr4);
        entity[i].actscript=atoi(nstr5);
        entity[i].movecode=atoi(nstr6);
        entity[i].step=atoi(nstr7);
        entity[i].data2=atoi(nstr8);
        entity[i].data3=atoi(nstr9);
        entity[i].delay=atoi(nstr10);
        entity[i].data5=atoi(nstr11);
        entity[i].data6=atoi(nstr12);
        entity[i].movescript=atoi(nstr13);

        if (i<entities-1) i++; else i=0;
        last_pressed=0; key[SCAN_UP]=0;

        sprintf(nstr1, "%d", entity[i].x);
        sprintf(nstr2, "%d", entity[i].y);
        sprintf(nstr3, "%d", entity[i].chrindex);
        sprintf(nstr4, "%d", entity[i].speed);
        sprintf(nstr5, "%d", entity[i].actscript);
        sprintf(nstr6, "%d", entity[i].movecode);
        sprintf(nstr7, "%d", entity[i].step);
        sprintf(nstr8, "%d", entity[i].data2);
        sprintf(nstr9, "%d", entity[i].data3);
        sprintf(nstr10, "%d", entity[i].delay);
        sprintf(nstr11, "%d", entity[i].data5);
        sprintf(nstr12, "%d", entity[i].data6);
        sprintf(nstr13, "%d", entity[i].movescript);
        continue;
      }
      if (last_pressed==SCAN_DOWN)
      {
        entity[i].x=atoi(nstr1);
        entity[i].y=atoi(nstr2);
        entity[i].chrindex=atoi(nstr3);
        entity[i].speed=atoi(nstr4);
        entity[i].actscript=atoi(nstr5);
        entity[i].movecode=atoi(nstr6);
        entity[i].step=atoi(nstr7);
        entity[i].data2=atoi(nstr8);
        entity[i].data3=atoi(nstr9);
        entity[i].delay=atoi(nstr10);
        entity[i].data5=atoi(nstr11);
        entity[i].data6=atoi(nstr12);
        entity[i].movescript=atoi(nstr13);

        if (i) i--; else i=entities-1;
        last_pressed=0; key[SCAN_DOWN]=0;

        sprintf(nstr1, "%d", entity[i].x);
        sprintf(nstr2, "%d", entity[i].y);
        sprintf(nstr3, "%d", entity[i].chrindex);
        sprintf(nstr4, "%d", entity[i].speed);
        sprintf(nstr5, "%d", entity[i].actscript);
        sprintf(nstr6, "%d", entity[i].movecode);
        sprintf(nstr7, "%d", entity[i].step);
        sprintf(nstr8, "%d", entity[i].data2);
        sprintf(nstr9, "%d", entity[i].data3);
        sprintf(nstr10, "%d", entity[i].delay);
        sprintf(nstr11, "%d", entity[i].data5);
        sprintf(nstr12, "%d", entity[i].data6);
        sprintf(nstr13, "%d", entity[i].movescript);
        continue;
      }

      if (last_pressed==SCAN_ENTER)
      {
        key[SCAN_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if (last_pressed==SCAN_TAB && !key[SCAN_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=entity[i].desc; break;
          case 1: ctf=2; str=nstr1; break;
          case 2: ctf=3; str=nstr2; break;
          case 3: ctf=4; str=nstr3; break;
          case 4: ctf=5; str=nstr4; break;
          case 5: ctf=6; str=nstr5; break;
          case 6: ctf=7; str=nstr6; break;
          case 7: ctf=8; str=nstr7; break;
          case 8: ctf=9; str=nstr8; break;
          case 9: ctf=10; str=nstr9; break;
          case 10: ctf=11; str=nstr10; break;
          case 11: ctf=12; str=nstr11; break;
          case 12: ctf=13; str=nstr12; break;
          case 13: ctf=14; str=nstr13; break;
          case 14: ctf=0; str=0; break;
        }
        key[SCAN_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (last_pressed==SCAN_TAB && key[SCAN_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=13; str=nstr12; break;
          case 1: ctf=0; str=0; break;
          case 2: ctf=1; str=entity[i].desc; break;
          case 3: ctf=2; str=nstr1; break;
          case 4: ctf=3; str=nstr2; break;
          case 5: ctf=4; str=nstr3; break;
          case 6: ctf=5; str=nstr4; break;
          case 7: ctf=6; str=nstr5; break;
          case 8: ctf=7; str=nstr6; break;
          case 9: ctf=8; str=nstr7; break;
          case 10: ctf=9; str=nstr8; break;
          case 11: ctf=10; str=nstr9; break;
          case 12: ctf=11; str=nstr10; break;
          case 13: ctf=12; str=nstr11; break;
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
  } while (!done);
  entity[i].x=atoi(nstr1);
  entity[i].y=atoi(nstr2);
  entity[i].chrindex=atoi(nstr3);
  entity[i].speed=atoi(nstr4);
  entity[i].actscript=atoi(nstr5);
  entity[i].movecode=atoi(nstr6);
  entity[i].step=atoi(nstr7);
  entity[i].data2=atoi(nstr8);
  entity[i].data3=atoi(nstr9);
  entity[i].delay=atoi(nstr10);
  entity[i].data5=atoi(nstr11);
  entity[i].data6=atoi(nstr12);
  entity[i].movescript=atoi(nstr13);
}

int clxsize=170, clysize=78;
int clxofs=105, clyofs=80;
char base=0;

void CHRList()
{ char done=0, moving=0;
  int mxo, myo, cursorblink;
  char ctf=0, cb=0, c, t, *str;

  last_pressed=0;
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      clxofs = (mx-mxo);
      clyofs = (my-myo);
      if (!mb) moving=0;
      if (clxofs<16) clxofs = 16;
      if (clyofs<16) clyofs = 16;
      if (clxofs+clxsize>335) clxofs = 336-clxsize;
      if (clyofs+clysize>sy+16) clyofs = (sy+16)-clysize;
    }

    Window(clxofs, clyofs, clxofs+clxsize, clyofs+clysize, "Entity CHR Reference Index");
    TextField(clxofs+14, clyofs+11, 120, chrlist[base].t, ctf == 1 ? cb : 0);
    TextField(clxofs+14, clyofs+22, 120, chrlist[base+1].t, ctf == 2 ? cb : 0);
    TextField(clxofs+14, clyofs+33, 120, chrlist[base+2].t, ctf == 3 ? cb : 0);
    TextField(clxofs+14, clyofs+44, 120, chrlist[base+3].t, ctf == 4 ? cb : 0);
    TextField(clxofs+14, clyofs+55, 120, chrlist[base+4].t, ctf == 5 ? cb : 0);
    Button(clxofs+137, clyofs+65, "OK");
    Button(clxofs+137, clyofs+10, "Browse");
    Button(clxofs+137, clyofs+21, "Browse");
    Button(clxofs+137, clyofs+32, "Browse");
    Button(clxofs+137, clyofs+43, "Browse");
    Button(clxofs+137, clyofs+54, "Browse");
    GotoXY(clxofs+3, clyofs+11); sprintf(strbuf,"%d:", base); printstring(strbuf);
    GotoXY(clxofs+3, clyofs+22); sprintf(strbuf,"%d:", base+1); printstring(strbuf);
    GotoXY(clxofs+3, clyofs+33); sprintf(strbuf,"%d:", base+2); printstring(strbuf);
    GotoXY(clxofs+3, clyofs+44); sprintf(strbuf,"%d:", base+3); printstring(strbuf);
    GotoXY(clxofs+3, clyofs+55); sprintf(strbuf,"%d:", base+4); printstring(strbuf);

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mb==1 && mx>clxofs+14 && mx<clxofs+120+14 && my>clyofs+11 && my<clyofs+9+11 && !moving)
    {
      ctf=1; str=chrlist[base].t; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>clxofs+14 && mx<clxofs+120+14 && my>clyofs+22 && my<clyofs+9+22 && !moving)
    {
      ctf=2; str=chrlist[base+1].t; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>clxofs+14 && mx<clxofs+120+14 && my>clyofs+33 && my<clyofs+9+33 && !moving)
    {
      ctf=3; str=chrlist[base+2].t; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>clxofs+14 && mx<clxofs+120+14 && my>clyofs+44 && my<clyofs+9+44 && !moving)
    {
      ctf=4; str=chrlist[base+3].t; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>clxofs+14 && mx<clxofs+120+14 && my>clyofs+55 && my<clyofs+9+55 && !moving)
    {
      ctf=5; str=chrlist[base+4].t; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if ((mb==1 && mx>clxofs+137 && mx<clxofs+30+137 && my>clyofs+65 && my<clyofs+10+65 && !moving) || (key[SCAN_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      done=1;
      ButtonPressed(clxofs+137, clyofs+65, "OK");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mb==1 && mx>clxofs+137 && mx<clxofs+30+137 && my>clyofs+10 && my<clyofs+10+10 && !moving)
    {
      // insert code for button "Browse" here
      ButtonPressed(clxofs+137, clyofs+10, "Browse");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      Browse(1,"*.chr","","","",chrlist[base].t);
      WaitRelease(); continue;
    }

    if (mb==1 && mx>clxofs+137 && mx<clxofs+30+137 && my>clyofs+21 && my<clyofs+10+21 && !moving)
    {
      // insert code for button "Browse" here
      ButtonPressed(clxofs+137, clyofs+21, "Browse");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      Browse(1,"*.chr","","","",chrlist[base+1].t);
      WaitRelease(); continue;
    }

    if (mb==1 && mx>clxofs+137 && mx<clxofs+30+137 && my>clyofs+32 && my<clyofs+10+32 && !moving)
    {
      // insert code for button "Browse" here
      ButtonPressed(clxofs+137, clyofs+32, "Browse");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      Browse(1,"*.chr","","","",chrlist[base+2].t);
      WaitRelease(); continue;
    }

    if (mb==1 && mx>clxofs+137 && mx<clxofs+30+137 && my>clyofs+43 && my<clyofs+10+43 && !moving)
    {
      // insert code for button "Browse" here
      ButtonPressed(clxofs+137, clyofs+43, "Browse");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      Browse(1,"*.chr","","","",chrlist[base+3].t);
      WaitRelease(); continue;
    }

    if (mb==1 && mx>clxofs+137 && mx<clxofs+30+137 && my>clyofs+54 && my<clyofs+10+54 && !moving)
    {
      // insert code for button "Browse" here
      ButtonPressed(clxofs+137, clyofs+54, "Browse");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      Browse(1,"*.chr","","","",chrlist[base+4].t);
      WaitRelease(); continue;
    }

    if (mb==1 && mx>(clxofs+clxsize-9) && (mx<clxofs+clxsize-2)
    && my>(clyofs+1) && (my<clyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mb==1 && mx>clxofs && mx<clxofs+clxsize && my>clyofs && my<(clyofs+9) && !moving)
    {
      moving=1;
      mxo=mx-clxofs;
      myo=my-clyofs;
      continue;
    }

    if (mb==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
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
      if (key[SCAN_UP])
      {
        key[SCAN_UP]=0;
        if (base) base--;
        last_pressed=0;
        continue;
      }
      if (key[SCAN_DOWN])
      {
        key[SCAN_DOWN]=0;
        if (base<95) base++;
        last_pressed=0;
        continue;
      }
      if (last_pressed==SCAN_ENTER)
      {
        key[SCAN_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if (last_pressed==SCAN_TAB && !key[SCAN_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=chrlist[base].t; break;
          case 1: ctf=2; str=chrlist[base+1].t; break;
          case 2: ctf=3; str=chrlist[base+2].t; break;
          case 3: ctf=4; str=chrlist[base+3].t; break;
          case 4: ctf=5; str=chrlist[base+4].t; break;
          case 5: ctf=0; str=0; break;
        }
        key[SCAN_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (last_pressed==SCAN_TAB && key[SCAN_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=5; str=chrlist[base].t; break;
          case 1: ctf=0; str=chrlist[base].t; break;
          case 2: ctf=1; str=chrlist[base+1].t; break;
          case 3: ctf=2; str=chrlist[base+2].t; break;
          case 4: ctf=3; str=chrlist[base+3].t; break;
          case 5: ctf=4; str=chrlist[base+4].t; break;
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
  } while (!done);
}

int msxsize=181, msysize=77;
int msxofs=70, msyofs=50;
char base1=0;

void MoveScriptDialog()
{ char done=0, moving=0;
  int mxo, myo, cursorblink;
  char ctf=0, cb=0, c, t, *str;

  last_pressed=0;
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      msxofs = (mx-mxo);
      msyofs = (my-myo);
      if (!mb) moving=0;
      if (msxofs<16) msxofs = 16;
      if (msyofs<16) msyofs = 16;
      if (msxofs+msxsize>335) msxofs = 336-msxsize;
      if (msyofs+msysize>sy+16) msyofs = (sy+16)-msysize;
    }

    Window(msxofs, msyofs, msxofs+msxsize, msyofs+msysize, "Movement Script Editor");
    TextField(msxofs+15, msyofs+12, 160, ms[base1].t, ctf == 1 ? cb : 0);
    TextField(msxofs+15, msyofs+22, 160, ms[base1+1].t, ctf == 2 ? cb : 0);
    TextField(msxofs+15, msyofs+32, 160, ms[base1+2].t, ctf == 3 ? cb : 0);
    TextField(msxofs+15, msyofs+42, 160, ms[base1+3].t, ctf == 4 ? cb : 0);
    TextField(msxofs+15, msyofs+52, 160, ms[base1+4].t, ctf == 5 ? cb : 0);
    Button(msxofs+146, msyofs+63, "OK");
    GotoXY(msxofs+3, msyofs+11); sprintf(strbuf,"%d:", base1); printstring(strbuf);
    GotoXY(msxofs+3, msyofs+22); sprintf(strbuf,"%d:", base1+1); printstring(strbuf);
    GotoXY(msxofs+3, msyofs+33); sprintf(strbuf,"%d:", base1+2); printstring(strbuf);
    GotoXY(msxofs+3, msyofs+44); sprintf(strbuf,"%d:", base1+3); printstring(strbuf);
    GotoXY(msxofs+3, msyofs+55); sprintf(strbuf,"%d:", base1+4); printstring(strbuf);

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mb==1 && mx>msxofs+15 && mx<msxofs+160+15 && my>msyofs+12 && my<msyofs+10+12 && !moving)
    {
      ctf=1; str=ms[base1].t; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>msxofs+15 && mx<msxofs+160+15 && my>msyofs+22 && my<msyofs+9+22 && !moving)
    {
      ctf=2; str=ms[base1+1].t; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>msxofs+15 && mx<msxofs+160+15 && my>msyofs+32 && my<msyofs+9+32 && !moving)
    {
      ctf=3; str=ms[base1+2].t; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>msxofs+15 && mx<msxofs+160+15 && my>msyofs+42 && my<msyofs+9+42 && !moving)
    {
      ctf=4; str=ms[base1+3].t; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>msxofs+15 && mx<msxofs+160+15 && my>msyofs+52 && my<msyofs+9+52 && !moving)
    {
      ctf=5; str=ms[base1+4].t; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if ((mb==1 && mx>msxofs+146 && mx<msxofs+30+146 && my>msyofs+63 && my<msyofs+10+63 && !moving) || (key[SCAN_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      done=1;
      ButtonPressed(msxofs+146, msyofs+63, "OK");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mb==1 && mx>(msxofs+msxsize-9) && (mx<msxofs+msxsize-2)
    && my>(msyofs+1) && (my<msyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mb==1 && mx>msxofs && mx<msxofs+msxsize && my>msyofs && my<(msyofs+9) && !moving)
    {
      moving=1;
      mxo=mx-msxofs;
      myo=my-msyofs;
      continue;
    }

    if (mb==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
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
      if (key[SCAN_UP])
      {
        key[SCAN_UP]=0;
        if (base1) base1--;
        last_pressed=0;
        continue;
      }
      if (key[SCAN_DOWN])
      {
        key[SCAN_DOWN]=0;
        if (base1<95) base1++;
        last_pressed=0;
        continue;
      }

      if (last_pressed==SCAN_ENTER)
      {
        key[SCAN_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if (last_pressed==SCAN_TAB && !key[SCAN_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=ms[base1].t; break;
          case 1: ctf=2; str=ms[base1+1].t; break;
          case 2: ctf=3; str=ms[base1+2].t; break;
          case 3: ctf=4; str=ms[base1+3].t; break;
          case 4: ctf=5; str=ms[base1+4].t; break;
          case 5: ctf=0; str=0; break;
        }
        key[SCAN_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (last_pressed==SCAN_TAB && key[SCAN_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=5; str=ms[base1+4].t; break;
          case 1: ctf=0; str=0; break;
          case 2: ctf=1; str=ms[base1].t; break;
          case 3: ctf=2; str=ms[base1+1].t; break;
          case 4: ctf=3; str=ms[base1+2].t; break;
          case 5: ctf=4; str=ms[base1+3].t; break;
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
  } while (!done);
}

int prxsize=150, prysize=80;
int prxofs=50, pryofs=60;

void MapPropertiesDialog()
{ char done=0, moving=0;
  int mxo, myo, cursorblink;
  char ctf=0, cb=0, c, t, *str;

  last_pressed=0;
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      prxofs = (mx-mxo);
      pryofs = (my-myo);
      if (!mb) moving=0;
      if (prxofs<16) prxofs = 16;
      if (pryofs<16) pryofs = 16;
      if (prxofs+prxsize>335) prxofs = 336-prxsize;
      if (pryofs+prysize>sy+16) pryofs = (sy+16)-prysize;
    }

    Window(prxofs, pryofs, prxofs+prxsize, pryofs+prysize, "MAP Properties");
    GotoXY(prxofs+10, pryofs+23); printstring("Music:");
    TextField(prxofs+33, pryofs+11, 112, mapname, ctf == 1 ? cb : 0);
    TextField(prxofs+33, pryofs+21, 112, musname, ctf == 2 ? cb : 0);
    GotoXY(prxofs+18, pryofs+33); printstring("VSP:");
    TextField(prxofs+33, pryofs+31, 112, vspname, ctf == 3 ? cb : 0);
    GotoXY(prxofs+3, pryofs+43); printstring("rstring:");
    TextField(prxofs+33, pryofs+41, 112, rstring, ctf == 4 ? cb : 0);
    BigButton(prxofs+97, pryofs+56, "Resize MAP");
    HLine(prxofs+1, pryofs+53, 148, white);
    GotoXY(prxofs+3, pryofs+58); printstring("xsize:");
    sprintf(strbuf,"%d",layer[0].sizex); printstring(strbuf);
    GotoXY(prxofs+49, pryofs+58); printstring("ysize:");
    sprintf(strbuf,"%d",layer[0].sizey); printstring(strbuf);
    Button(prxofs+117, pryofs+67, "OK");
    GotoXY(prxofs+16, pryofs+13); printstring("MAP:");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mb==1 && mx>prxofs+33 && mx<prxofs+112+33 && my>pryofs+11 && my<pryofs+9+11 && !moving)
    {
      ctf=1; str=mapname; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>prxofs+33 && mx<prxofs+112+33 && my>pryofs+21 && my<pryofs+9+21 && !moving)
    {
      ctf=2; str=musname; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>prxofs+33 && mx<prxofs+112+33 && my>pryofs+31 && my<pryofs+9+31 && !moving)
    {
      ctf=3; str=vspname; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>prxofs+33 && mx<prxofs+112+33 && my>pryofs+41 && my<pryofs+9+41 && !moving)
    {
      ctf=4; str=rstring; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>prxofs+97 && mx<prxofs+30+97 && my>pryofs+56 && my<pryofs+10+56 && !moving)
    {
      // insert code for button "Resize MAP" here
      BigButtonPressed(prxofs+97, pryofs+56, "Resize MAP");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease();
      ResizeMapDialog();
      WaitRelease(); continue;
    }

    if ((mb==1 && mx>prxofs+117 && mx<prxofs+30+117 && my>pryofs+67 && my<pryofs+10+67 && !moving) || (key[SCAN_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      ButtonPressed(prxofs+117, pryofs+67, "OK");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      done=1;
      WaitRelease(); continue;
    }

    if (mb==1 && mx>(prxofs+prxsize-9) && (mx<prxofs+prxsize-2)
    && my>(pryofs+1) && (my<pryofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mb==1 && mx>prxofs && mx<prxofs+prxsize && my>pryofs && my<(pryofs+9) && !moving)
    {
      moving=1;
      mxo=mx-prxofs;
      myo=my-pryofs;
      continue;
    }

    if (mb==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
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

      if (last_pressed==SCAN_ENTER)
      {
        key[SCAN_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if (last_pressed==SCAN_TAB && !key[SCAN_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=mapname; break;
          case 1: ctf=2; str=musname; break;
          case 2: ctf=3; str=vspname; break;
          case 3: ctf=4; str=rstring; break;
          case 4: ctf=0; str=0; break;
        }
        key[SCAN_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (last_pressed==SCAN_TAB && key[SCAN_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=4; str=rstring; break;
          case 1: ctf=0; str=0; break;
          case 2: ctf=1; str=mapname; break;
          case 3: ctf=2; str=musname; break;
          case 4: ctf=3; str=vspname; break;
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

  } while (!done);

}

int rsxsize=134, rsysize=44;
int rsxofs=20, rsyofs=20;

static void PutByte(byte *to, byte v, int x, int y, int w, int h)
{
  if (x<0 || x>=w || y<0 || y>=h)
    return;

  to[(y*w)+x]=v;
}

static byte GetByte(byte *from, int x, int y, int w, int h)
{
  if (x<0 || x>=w || y<0 || y>=h)
    return 0;

  return from[(y*w)+x];
}

static void PutWord(word *to, word v, int x, int y, int w, int h)
{
  if (x<0 || x>=w || y<0 || y>=h)
    return;

  to[(y*w)+x]=v;
}

static word GetWord(word *from, int x, int y, int w, int h)
{
  if (x<0 || x>=w || y<0 || y>=h)
    return 0;

  return from[(y*w)+x];
}

void ResizeMapDialog()
{ char done=0, moving=0;
  int mxo, myo, cursorblink, basex, basey, i; //, xl, yl;
  char ctf=0, cb=0, c, t;
  char *str, *tb; //, *s1, *d1;
  char nstr1[80], nstr2[80];
  word *tbw; //, *s=NULL,*d=NULL;
  //int temp_width;
  int a,b;

  basex=layer[0].sizex;
  basey=layer[0].sizey;
  sprintf(nstr1, "%d", basex);
  sprintf(nstr2, "%d", basey);
  last_pressed=0;
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      rsxofs = (mx-mxo);
      rsyofs = (my-myo);
      if (!mb) moving=0;
      if (rsxofs<16) rsxofs = 16;
      if (rsyofs<16) rsyofs = 16;
      if (rsxofs+rsxsize>335) rsxofs = 336-rsxsize;
      if (rsyofs+rsysize>sy+16) rsyofs = (sy+16)-rsysize;
    }

    Window(rsxofs, rsyofs, rsxofs+rsxsize, rsyofs+rsysize, "Resize MAP");
    GotoXY(rsxofs+4, rsyofs+12); printstring("Base X:");
    GotoXY(rsxofs+4, rsyofs+22); printstring("Base Y:");
    TextField(rsxofs+30, rsyofs+10, 100, nstr1, ctf == 1 ? cb : 0);
    TextField(rsxofs+30, rsyofs+20, 100, nstr2, ctf == 2 ? cb : 0);
    Button(rsxofs+69, rsyofs+31, "Resize");
    Button(rsxofs+101, rsyofs+31, "Cancel");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mb==1 && mx>rsxofs+30 && mx<rsxofs+100+30 && my>rsyofs+10 && my<rsyofs+9+10 && !moving)
    {
      ctf=1; str=nstr1; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>rsxofs+30 && mx<rsxofs+100+30 && my>rsyofs+20 && my<rsyofs+9+20 && !moving)
    {
      ctf=2; str=nstr2; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>rsxofs+69 && mx<rsxofs+30+69 && my>rsyofs+31 && my<rsyofs+10+31 && !moving)
    {
      printf("&");
      fflush(stdout);
      // insert code for button "Resize" here
      ButtonPressed(rsxofs+69, rsyofs+31, "Resize");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      done=1;
      basex=atoi(nstr1);
      basey=atoi(nstr2);

      tb = (char *)valloc(basex*basey,"tilebuf",0);
      for (b=0; b<basey; ++b)
        for (a=0; a<basex; ++a)
          PutByte(tb,
            GetByte(obstruct,a,b,layer[0].sizex,layer[0].sizey),
            a,b,basex,basey);
      vfree(obstruct);
      obstruct=tb;

      tb = (char *)valloc(basex*basey,"tilebuf",0);
      for (b=0; b<basey; ++b)
        for (a=0; a<basex; ++a)
          PutByte(tb,
            GetByte(zone,a,b,layer[0].sizex,layer[0].sizey),
            a,b,basex,basey);
      vfree(zone);
      zone=tb;

      for (i=0; i<numlayers; i++)
      {
        tbw=(word *)valloc(basex*basey*2,"tilebuf",0);
        //memset(tbw, 0, basex*basey*2);
        for (b=0; b<basey; ++b)
          for (a=0; a<basex; ++a)
            PutWord(tbw,
              GetWord(layers[i],a,b,layer[i].sizex,layer[i].sizey),
              a,b,basex,basey);

        vfree(layers[i]);
        layers[i]=tbw;
      }
      for (i=0; i<numlayers; ++i)
      {
        layer[i].sizex=basex;
        layer[i].sizey=basey;
      }
      xwin=0; ywin=0;
      WaitRelease(); continue;
    }

    if (mb==1 && mx>rsxofs+101 && mx<rsxofs+30+101 && my>rsyofs+31 && my<rsyofs+10+31 && !moving)
    {
      // insert code for button "Cancel" here
      done=1;
      ButtonPressed(rsxofs+101, rsyofs+31, "Cancel");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mb==1 && mx>(rsxofs+rsxsize-9) && (mx<rsxofs+rsxsize-2)
    && my>(rsyofs+1) && (my<rsyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mb==1 && mx>rsxofs && mx<rsxofs+rsxsize && my>rsyofs && my<(rsyofs+9) && !moving)
    {
      moving=1;
      mxo=mx-rsxofs;
      myo=my-rsyofs;
      continue;
    }

    if (mb==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
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

      if (last_pressed==SCAN_ENTER)
      {
        key[SCAN_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if (last_pressed==SCAN_TAB && !key[SCAN_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=nstr1; break;
          case 1: ctf=2; str=nstr2; break;
          case 2: ctf=0; str=0; break;
        }
        key[SCAN_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (last_pressed==SCAN_TAB && key[SCAN_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=2; str=nstr2; break;
          case 1: ctf=0; str=0; break;
          case 2: ctf=1; str=nstr1; break;
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
  } while (!done);
  basex=atoi(nstr1);
  basey=atoi(nstr2);
}

int cdxsize=150, cdysize=45;
int cdxofs=110, cdyofs=70;
char curtrack=1;

void CDDialog()
{ char done=0, moving=0;
  int mxo, myo, cursorblink;
  char ctf=0, cb=0, c, t, *str;

  last_pressed=0;
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      cdxofs = (mx-mxo);
      cdyofs = (my-myo);
      if (!mb) moving=0;
      if (cdxofs<16) cdxofs = 16;
      if (cdyofs<16) cdyofs = 16;
      if (cdxofs+cdxsize>335) cdxofs = 336-cdxsize;
      if (cdyofs+cdysize>sy+16) cdyofs = (sy+16)-cdysize;
    }

    Window(cdxofs, cdyofs, cdxofs+cdxsize, cdyofs+cdysize, "CD Player");
    Button(cdxofs+53, cdyofs+10, "Play");
    Button(cdxofs+53, cdyofs+21, "Stop");
    Button(cdxofs+85, cdyofs+10, "Eject");
    Button(cdxofs+85, cdyofs+21, "Close");
    Button(cdxofs+117, cdyofs+10, "Next");
    Button(cdxofs+117, cdyofs+21, "Prev");
    Button(cdxofs+117, cdyofs+32, "OK");
    GotoXY(cdxofs+4, cdyofs+18); printstring("Track ");
    sprintf(strbuf,"%d",curtrack); printstring(strbuf);

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mb==1 && mx>cdxofs+53 && mx<cdxofs+30+53 && my>cdyofs+10 && my<cdyofs+10+10 && !moving)
    {
      // insert code for button "Play" here
      ButtonPressed(cdxofs+53, cdyofs+10, "Play");
      DrawMouse();
      ShowPage();
      //CD_Play(curtrack);
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mb==1 && mx>cdxofs+53 && mx<cdxofs+30+53 && my>cdyofs+21 && my<cdyofs+10+21 && !moving)
    {
      // insert code for button "Stop" here
      ButtonPressed(cdxofs+53, cdyofs+21, "Stop");
      DrawMouse();
      ShowPage();
      //CD_Stop();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mb==1 && mx>cdxofs+85 && mx<cdxofs+30+85 && my>cdyofs+10 && my<cdyofs+10+10 && !moving)
    {
      // insert code for button "Eject" here
      ButtonPressed(cdxofs+85, cdyofs+10, "Eject");
      DrawMouse();
      ShowPage();
      //CD_Open_Door();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mb==1 && mx>cdxofs+85 && mx<cdxofs+30+85 && my>cdyofs+21 && my<cdyofs+10+21 && !moving)
    {
      // insert code for button "Close" here
      ButtonPressed(cdxofs+85, cdyofs+21, "Close");
      DrawMouse();
      ShowPage();
      //CD_Close_Door();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mb==1 && mx>cdxofs+117 && mx<cdxofs+30+117 && my>cdyofs+10 && my<cdyofs+10+10 && !moving)
    {
      // insert code for button "Next" here
      ButtonPressed(cdxofs+117, cdyofs+10, "Next");
      curtrack++;
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mb==1 && mx>cdxofs+117 && mx<cdxofs+30+117 && my>cdyofs+21 && my<cdyofs+10+21 && !moving)
    {
      // insert code for button "Prev" here
      ButtonPressed(cdxofs+117, cdyofs+21, "Prev");
      DrawMouse();
      if (curtrack>1) curtrack--;
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if ((mb==1 && mx>cdxofs+117 && mx<cdxofs+30+117 && my>cdyofs+32 && my<cdyofs+10+32 && !moving) || (key[SCAN_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      ButtonPressed(cdxofs+117, cdyofs+32, "OK");
      DrawMouse();
      done=1;
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mb==1 && mx>(cdxofs+cdxsize-9) && (mx<cdxofs+cdxsize-2)
    && my>(cdyofs+1) && (my<cdyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mb==1 && mx>cdxofs && mx<cdxofs+cdxsize && my>cdyofs && my<(cdyofs+9) && !moving)
    {
      moving=1;
      mxo=mx-cdxofs;
      myo=my-cdyofs;
      continue;
    }

    if (mb==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
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

      if (last_pressed==SCAN_ENTER)
      {
        key[SCAN_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
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
  } while (!done);
}

int ntxsize=130, nytsize=51;
int ntxofs=110, ntyofs=70;

void NoticeDialog()
{ char done=0, moving=0;
  int mxo, myo, cursorblink;
  char ctf=0, cb=0, c, t, *str;

  last_pressed=0;
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      ntxofs = (mx-mxo);
      ntyofs = (my-myo);
      if (!mb) moving=0;
      if (ntxofs<16) ntxofs = 16;
      if (ntyofs<16) ntyofs = 16;
      if (ntxofs+ntxsize>335) ntxofs = 336-ntxsize;
      if (ntyofs+nytsize>sy+16) ntyofs = (sy+16)-nytsize;
    }

    Window(ntxofs, ntyofs, ntxofs+ntxsize, ntyofs+nytsize, "Notice!");
    GotoXY(ntxofs+20, ntyofs+20); printstring("Don't forget to add this");
    GotoXY(ntxofs+14, ntyofs+26); printstring("layer to the Render String!");
    Button(ntxofs+50, ntyofs+36, "OK");
    GotoXY(ntxofs+25, ntyofs+14); printstring("Layer has been added.");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if ((mb==1 && mx>ntxofs+50 && mx<ntxofs+30+50 && my>ntyofs+36 && my<ntyofs+10+36 && !moving) || (key[SCAN_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      ButtonPressed(ntxofs+50, ntyofs+36, "OK");
      done=1;
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mb==1 && mx>(ntxofs+ntxsize-9) && (mx<ntxofs+ntxsize-2)
    && my>(ntyofs+1) && (my<ntyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mb==1 && mx>ntxofs && mx<ntxofs+ntxsize && my>ntyofs && my<(ntyofs+9) && !moving)
    {
      moving=1;
      mxo=mx-ntxofs;
      myo=my-ntyofs;
      continue;
    }

    if (mb==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
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

      if (last_pressed==SCAN_ENTER)
      {
        key[SCAN_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
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
  } while (!done);
}

int oxsize=150, oysize=98;
int oxofs=120, oyofs=70;
extern char vcedprog[80];

void OptionsDialog()
{ char done=0, moving=0;
  int mxo, myo, cursorblink;
  char ctf=0, cb=0, c, t, *str;
  char nstr1[80], nstr2[80], nstr3[80], nstr4[80];

  sprintf(nstr1, "%d", winbg);
  sprintf(nstr2, "%d", titlebg);
  sprintf(nstr3, "%d", brightw);
  sprintf(nstr4, "%d", darkw);
  last_pressed=0;
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      oxofs = (mx-mxo);
      oyofs = (my-myo);
      if (!mb) moving=0;
      if (oxofs<16) oxofs = 16;
      if (oyofs<16) oyofs = 16;
      if (oxofs+oxsize>335) oxofs = 336-oxsize;
      if (oyofs+oysize>sy+16) oyofs = (sy+16)-oysize;
    }

    Window(oxofs, oyofs, oxofs+oxsize, oyofs+oysize, "MapEd Options");
    GotoXY(oxofs+16, oyofs+12); printstring("Smooth Scroll");
    CheckBox(oxofs+5, oyofs+11, scrollmode);
    GotoXY(oxofs+16, oyofs+20); printstring("Tile Highlight");
    CheckBox(oxofs+5, oyofs+19, mh);
    GotoXY(oxofs+38, oyofs+31); printstring("Window Background");
    TextField(oxofs+5, oyofs+29, 30, nstr1, ctf == 1 ? cb : 0);
    TextField(oxofs+5, oyofs+39, 30, nstr2, ctf == 2 ? cb : 0);
    GotoXY(oxofs+38, oyofs+41); printstring("Titlebar Background");
    CheckBox(oxofs+74, oyofs+11, vm);
    GotoXY(oxofs+84, oyofs+12); printstring("320x240 driver");
    CheckBox(oxofs+74, oyofs+19, mouse_scroll);
    GotoXY(oxofs+84, oyofs+20); printstring("Mouse Scrolling");
    TextField(oxofs+5, oyofs+49, 30, nstr3, ctf == 3 ? cb : 0);
    TextField(oxofs+5, oyofs+59, 30, nstr4, ctf == 4 ? cb : 0);
    GotoXY(oxofs+38, oyofs+51); printstring("White");
    GotoXY(oxofs+38, oyofs+61); printstring("Shadow");
    Button(oxofs+60, oyofs+85, "OK");
    HLine(oxofs+1, oyofs+71, 148, white);
    GotoXY(oxofs+3, oyofs+76); printstring("Editor:");
    TextField(oxofs+28, oyofs+74, 118, vcedprog, ctf == 5 ? cb : 0);

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mb==1 && mx>oxofs+5 && mx<oxofs+8+5 && my>oyofs+11 && my<oyofs+7+11 && !moving)
    {
      xwin=xwin>>4; xwin=xwin<<4;
      ywin=ywin>>4; ywin=ywin<<4;
      scrollmode ^= 1; WaitRelease(); continue;
    }

    if (mb==1 && mx>oxofs+5 && mx<oxofs+8+5 && my>oyofs+19 && my<oyofs+7+19 && !moving)
    {
      mh ^= 1; WaitRelease(); continue;
    }

    if (mb==1 && mx>oxofs+5 && mx<oxofs+30+5 && my>oyofs+29 && my<oyofs+9+29 && !moving)
    {
      ctf=1; str=nstr1; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>oxofs+5 && mx<oxofs+30+5 && my>oyofs+39 && my<oyofs+9+39 && !moving)
    {
      ctf=2; str=nstr2; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>oxofs+74 && mx<oxofs+8+74 && my>oyofs+11 && my<oyofs+7+11 && !moving)
    {
      vm ^= 1; WaitRelease();
      ShutdownVideo();
      InitVideo(vm);
      set_intensity(63);
      InitMouse();
      continue;
    }

    if (mb==1 && mx>oxofs+74 && mx<oxofs+8+74 && my>oyofs+19 && my<oyofs+7+19 && !moving)
    {
      mouse_scroll ^= 1; WaitRelease(); continue;
    }

    if (mb==1 && mx>oxofs+5 && mx<oxofs+30+5 && my>oyofs+49 && my<oyofs+9+49 && !moving)
    {
      ctf=3; str=nstr3; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>oxofs+5 && mx<oxofs+30+5 && my>oyofs+59 && my<oyofs+9+59 && !moving)
    {
      ctf=4; str=nstr4; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if ((mb==1 && mx>oxofs+60 && mx<oxofs+30+60 && my>oyofs+85 && my<oyofs+10+85 && !moving) || (key[SCAN_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      ButtonPressed(oxofs+60, oyofs+85, "OK");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      done=1;
      WaitRelease(); continue;
    }

    if (mb==1 && mx>oxofs+28 && mx<oxofs+118+28 && my>oyofs+74 && my<oyofs+9+74 && !moving)
    {
      ctf=5; str=vcedprog; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>(oxofs+oxsize-9) && (mx<oxofs+oxsize-2)
    && my>(oyofs+1) && (my<oyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mb==1 && mx>oxofs && mx<oxofs+oxsize && my>oyofs && my<(oyofs+9) && !moving)
    {
      moving=1;
      mxo=mx-oxofs;
      myo=my-oyofs;
      continue;
    }

    if (mb==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
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

      if (last_pressed==SCAN_ENTER)
      {
        key[SCAN_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if (last_pressed==SCAN_TAB && !key[SCAN_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=nstr1; break;
          case 1: ctf=2; str=nstr2; break;
          case 2: ctf=3; str=nstr3; break;
          case 3: ctf=4; str=nstr4; break;
          case 4: ctf=5; str=vcedprog; break;
          case 5: ctf=0; str=0; break;
        }
        key[SCAN_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (last_pressed==SCAN_TAB && key[SCAN_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=5; str=vcedprog; break;
          case 1: ctf=0; str=0; break;
          case 2: ctf=1; str=nstr1; break;
          case 3: ctf=2; str=nstr2; break;
          case 4: ctf=3; str=nstr3; break;
          case 5: ctf=4; str=nstr4; break;
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
  } while (!done);
  winbg=atoi(nstr1);
  titlebg=atoi(nstr2);
  brightw=atoi(nstr3);
  darkw=atoi(nstr4);
}

int vcexsize=200, vceysize=60;
int vcexofs=76, vceyofs=70;

void VCNotify(char *msg)
{ char done=0, moving=0;
  int mxo, myo, cursorblink;
  char ctf=0, cb=0, c, t, *str;

  last_pressed=0;
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      vcexofs = (mx-mxo);
      vceyofs = (my-myo);
      if (!mb) moving=0;
      if (vcexofs<16) vcexofs = 16;
      if (vceyofs<16) vceyofs = 16;
      if (vcexofs+vcexsize>335) vcexofs = 336-vcexsize;
      if (vceyofs+vceysize>sy+16) vceyofs = (sy+16)-vceysize;
    }

    Window(vcexofs, vceyofs, vcexofs+vcexsize, vceyofs+vceysize, "VergeC Error Report");
    Button(vcexofs+85, vceyofs+41, "OK");
    GotoXY(vcexofs+29, vceyofs+17); printstring("VCC has reported the following error:");
    GotoXY(vcexofs+100-(pixels(msg)/2), vceyofs+23); printstring(msg);

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if ((mb==1 && mx>vcexofs+85 && mx<vcexofs+30+85 && my>vceyofs+41 && my<vceyofs+10+41 && !moving) || (key[SCAN_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      ButtonPressed(vcexofs+85, vceyofs+41, "OK");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      done=1;
      WaitRelease(); continue;
    }

    if (mb==1 && mx>(vcexofs+vcexsize-9) && (mx<vcexofs+vcexsize-2)
    && my>(vceyofs+1) && (my<vceyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mb==1 && mx>vcexofs && mx<vcexofs+vcexsize && my>vceyofs && my<(vceyofs+9) && !moving)
    {
      moving=1;
      mxo=mx-vcexofs;
      myo=my-vceyofs;
      continue;
    }

    if (mb==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
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

      if (last_pressed==SCAN_ENTER)
      {
        key[SCAN_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
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
  } while (!done);
}
