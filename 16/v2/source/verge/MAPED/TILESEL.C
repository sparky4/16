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
#include <malloc.h>
#include <stdlib.h>

#include "config.h"
#include "keyboard.h"
#include "maped.h"
#include "mouse.h"
#include "render.h"
#include "tileed.h"
#include "timer.h"
#include "vdriver.h"

#include "smalfont.h"
#include "guicomp.h"
#include "gui.h"

extern byte TileCmp(byte *one, byte *two); //zero

// ================================= Data ====================================

int selofs=0;
char ttile[256], ttile2[256];
char curastd=0;
char curmode=0;

// ================================= Code ====================================

int curtileptr(int row)
{
  int jumper,wide;
  int x,y;

  jumper=16+pad;
  wide=320/jumper;

  x=(mx-16)/jumper;
  y=((my-16)/jumper)*wide;

  return x+y+row;
}

void NormalButton(int x1, int y1, int x2, int y2, char *str)
{
  FilledBox(x1, y1, x2, y2, winbg);
  HLine(x1, y1, x2, black);
  VLine(x1, y1, y2, black);
  HLine(x1+1, y1+y2-1, x2-1, black);
  VLine(x1+x2-1, y1+1, y2-1, black);

  HLine(x1+1, y1+1, x2-2, brightw);
  VLine(x1+1, y1+1, y2-2, brightw);
  HLine(x1+2, y1+y2-2, x2-3, darkw);
  VLine(x1+x2-2, y1+2, y2-3, darkw);
  GotoXY(x1+((x2-pixels(str))/2), y1+3);
  printstring(str);
}

void PressdButton(int x1, int y1, int x2, int y2, char *str)
{
  FilledBox(x1, y1, x2, y2, winbg);
  HLine(x1, y1, x2, black);
  VLine(x1, y1, y2, black);
  HLine(x1+1, y1+y2-1, x2-1, black);
  VLine(x1+x2-1, y1+1, y2-1, black);

  HLine(x1+1, y1+1, x2-2, darkw);
  VLine(x1+1, y1+1, y2-2, darkw);
  GotoXY(x1+((x2-pixels(str))/2)+1, y1+4);
  printstring(str);
}

void RenderSelector()
{ unsigned char *img,i,j;
  int z;
  int tskip=16+pad;
  int twide=320/tskip;
  int tdeep=((200+15)&~15)/tskip;

  ClearScreen();

  for (i=0; i<tdeep; ++i)
    for (j=0; j<twide; ++j)
    {
      img=vsp+(256*((i*twide)+j+selofs));
      if (((i*twide)+j+selofs)>=numtiles) img=vsp;
      CopyTile((j*tskip)+16,(i*tskip)+16,img);
    }
  if (th)
  {
    if (my<(tsy-40))
    DrawHighlight( ((mx/tskip)*tskip)-(1+pad),
                   ((my/tskip)*tskip)-(1+pad));
  }
  z=tsy-40;
  sprintf(strbuf,"MapEd %s Tile Selector - Tile %d/%d",ME2_VERSION,curtileptr(selofs),numtiles);
  Window(16,z,336,z+40,strbuf);
  HLine(278, z+11, 53, darkw);
  VLine(278, z+11, 25, darkw);
  VLine(330, z+12, 24, brightw);
  HLine(279, z+35, 51, brightw);
  if (curmode==0) PressdButton(290, z+12, 40, 12, "Select");
             else NormalButton(290, z+12, 40, 12, "Select");
  if (curmode==1) PressdButton(290, z+23, 40, 12, "Edit");
             else NormalButton(290, z+23, 40, 12, "Edit");
  if (th)         PressdButton(279, z+12, 12, 12, "H");
             else NormalButton(279, z+12, 12, 12, "H");
  if (pad)        PressdButton(279, z+23, 12, 12, "P");
             else NormalButton(279, z+23, 12, 12, "P");

  HLine(219, z+11, 52, darkw);
  VLine(219, z+11, 25, darkw);
  VLine(270, z+12, 24, brightw);
  HLine(220, z+35, 50, brightw);
  NormalButton(220, z+12, 50, 12, "Import VSP");
  NormalButton(220, z+23, 50, 12, "Import PCX");

  HLine(160, z+11, 52, darkw);
  VLine(160, z+11, 25, darkw);
  VLine(211, z+12, 24, brightw);
  HLine(161, z+35, 50, brightw);
  NormalButton(161, z+12, 50, 12, "Add Tiles");
  NormalButton(161, z+23, 50, 12, "Animation");

  HLine(26, z+10, 20, darkw);
  VLine(26, z+10, 20, darkw);
  HLine(27, z+29, 19, brightw);
  VLine(45, z+11, 19, brightw);
  FilledBox(27, z+11, 18, 18, black);
  CopyTile(28, z+12, ttile2);
  GotoXY(20, z+32); printstring("Clipboard");
}

void FlipX(unsigned short edtile)
{ int i,j;

  key[SCAN_X]=0;
  if (edtile>=numtiles) return;
  memcpy(ttile,&vsp[edtile*256],256);
  modified=1;

  for (i=0;i<16;i++)
      for (j=0;j<16;j++)
          vsp[((edtile)*256)+(j*16)+i]=ttile[(j*16)+(15-i)];
}

void FlipY(unsigned short edtile)
{ int i,j;

  key[SCAN_Y]=0;
  if (edtile>=numtiles) return;
  memcpy(ttile,&vsp[edtile*256],256);
  modified=1;

  for (i=0;i<16;i++)
      for (j=0;j<16;j++)
          vsp[((edtile)*256)+(j*16)+i]=ttile[((15-j)*16)+i];
}

void FlipZ(unsigned short edtile)
{ int i,j;

  key[SCAN_Z]=0;
  if (edtile>=numtiles) return;
  memcpy(ttile,&vsp[edtile*256],256);
  modified=1;

  for (i=0;i<16;i++)
      for (j=0;j<16;j++)
          vsp[((edtile)*256)+(j*16)+i]=ttile[(i*16)+j];
}

void DrawHighlight(int x, int y)
{
  if (x<10 || y<10 || x>330 || y>sy+10) return;
  HLine(x, y, 18, white);
  HLine(x, y+17, 18, white);
  VLine(x, y, 18, white);
  VLine(x+17, y, 18, white);
}

void TileSelector()
{ int z, ct, k,j,m;
  char *s, *d;

  int ms_down=0; // aen
  int ms_up=0;

  while (!key[SCAN_ESC])
  {
     RenderSelector();
     RenderMessages();
     DrawMouse();
     ShowPage();

     ms_down=0;
     ms_up=0;
     if (mouse_scroll)
     {
       if (my>tsy-2) ms_down=1;
       if (my<18) ms_up=1;
     }

     if ((key[SCAN_DOWN] || (ms_down && (!(systemtime%10)))) && selofs<65319)
     {
       if (!pad) selofs=selofs+20;
       else selofs+=18;
       key[SCAN_DOWN]=0;
       systemtime++;
     }
     if ((key[SCAN_UP] || (ms_up && (!(systemtime%10)))) && selofs>17)
     {
       if (!pad) selofs-=20;
       else selofs-=18;
       key[SCAN_UP]=0;
       systemtime++;
     }
     if (key[SCAN_P])
     {
        key[SCAN_P]=0;
        pad=pad ^ 1;
        selofs=0;
        if (pad) Message("Tile padding enabled.",100);
            else Message("Tile padding disabled.",100);
     }
     if (key[SCAN_H])
     {
       key[SCAN_H]=0;
       th=th ^ 1;
       if (th) Message("Tile Highlight enabled.",100);
          else Message("Tile Highlight disabled.",100);
     }

     if (key[SCAN_X]) FlipX(curtileptr(selofs));
     if (key[SCAN_Y]) FlipY(curtileptr(selofs));
     if (key[SCAN_Z]) FlipZ(curtileptr(selofs));
     if (key[SCAN_V]) VSPAnimDialog();
     if (key[SCAN_CTRL] && key[SCAN_C])
     {
       memcpy(ttile2, (char *) (vsp+(256*curtileptr(selofs))), 256);
       key[SCAN_C]=0;
     }
     if (key[SCAN_CTRL] && key[SCAN_S])
     {
       if (curtileptr(selofs) >= numtiles) return;
       memcpy((char *) (vsp+(256*curtileptr(selofs))), ttile2, 256);
       modified=1;
       key[SCAN_S]=0;
     }
     if (key[SCAN_CTRL] && key[SCAN_T])
     {
       if (curtileptr(selofs) >= numtiles) return;
       modified=1;
       s=ttile2;
       d=(char *) (vsp+(256*curtileptr(selofs)));
       for (z=0; z<256; z++)
       {
         if (*s) *d=*s;
         d++;
         s++;
       }
       key[SCAN_T]=0;
     }
     if (key[SCAN_CTRL] && key[SCAN_D] && !key[SCAN_ALT])
     {
        key[SCAN_D]=0;
        modified=1;
        z=curtileptr(selofs);
        if (z<numtiles)
        {
          memcpy(vsp+(z*256),vsp+((z+1)*256),((numtiles-z-1)*256));
          numtiles--;
        }
     }
// zero: Super Delete Mode!
// wimpy delete irks me.
     if (key[SCAN_CTRL] && key[SCAN_D] && key[SCAN_ALT])
     {
       key[SCAN_D]=0;
       modified=1;
       z=curtileptr(selofs);
       if(ct<numtiles)
       {
         if(z<numtiles)
         {
           memcpy(vsp+(z*256),vsp+((z+1)*256),((numtiles-z-1)*256));
           numtiles--;
           for(k=0;k<numlayers;k++)
           {
             for(j=0; j<layer[k].sizey; j++)
             {
               for(m=0; m<layer[k].sizex; m++)
               {
                 if(layers[k][j*layer[k].sizex+m]==z) layers[k][j*layer[k].sizex+m]=0;
                 if(layers[k][j*layer[k].sizex+m]>z) layers[k][j*layer[k].sizex+m]--;
               }
             }
           }
         }
       }
     }
// zero: Super Insert Mode!
// wimpy insert irks me.
     if (key[SCAN_CTRL] && key[SCAN_I] && key[SCAN_ALT])
     {
       key[SCAN_I]=0;
       ct=curtileptr(selofs);
       if (ct <= numtiles)
       {
         modified=1;
         s=(char *) valloc((numtiles+1)*256, "s_ins",0);
         memcpy(s, vsp, (numtiles*256));
         vfree(vsp); vsp=s; s=0;
         for (z=numtiles-1; z>=ct; z--)
           memcpy(vsp+((z+1)*256),vsp+(z*256),256);
         memset(vsp+(ct*256), 0, 256);
         numtiles++;
       }
       for(k=0;k<numlayers;k++)
       {
         for(j=0; j<layer[k].sizey; j++)
         {
           for(m=0; m<layer[k].sizex; m++)
           {
             if(layers[k][j*layer[k].sizex+m]>=ct) layers[k][j*layer[k].sizex+m]++;
           }
         }
       }
     }

     if (key[SCAN_CTRL] && key[SCAN_I] && !key[SCAN_ALT])
     {
       key[SCAN_I]=0;
       ct=curtileptr(selofs);
       if (ct <= numtiles)
       {
         modified=1;
         s=(char *)valloc((numtiles+1)*256,"s_ins",0);
         memcpy(s, vsp, (numtiles*256));
         vfree(vsp); vsp=s; s=0;
         for (z=numtiles-1; z>=ct; z--)
           memcpy(vsp+((z+1)*256),vsp+(z*256),256);
         memset(vsp+(ct*256), 0, 256);
         numtiles++;
       }
     }
     if (key[SCAN_E])
     {
       modified=1;
       TEtile=curtileptr(selofs); // middle tile initially
       if (TEtile<numtiles)
       {
         TEtype=TETILE;
         TEsource=vsp+(256*TEtile);
         TExsize=16;
         TEysize=16;
         TileEdit();
       }
     }

     if (mb==1 && my<(tsy-40) && !curmode)
     {
        if (curtileptr(selofs)<numtiles)
        {
          lt=curtileptr(selofs);
          key[SCAN_ESC]=1;
        }
        WaitRelease();
      }
     if (mb==2 && my<(tsy-40) && !curmode)
     {
        if (curtileptr(selofs)<numtiles)
        {
          rt=curtileptr(selofs);
          key[SCAN_ESC]=1;
        }
        WaitRelease();
     }
     if (mb && my<(tsy-40) && curmode)
     {
       WaitRelease();
       TEtile=curtileptr(selofs); // middle tile initially
       if (TEtile<numtiles)
       {
         modified=1;
         TEtype=TETILE;
         TEsource=vsp+(256*TEtile);
         TExsize=16;
         TEysize=16;
         TileEdit();
       }
     }
     z=tsy-40;
     if (mb && mx>290 && mx<330 && my>z+12 && my<z+23)
     {
       curmode=0;
       WaitRelease();
       Message("Tile select mode.",100);
     }
     if (mb && mx>290 && mx<330 && my>z+23 && my<z+47)
     {
       curmode=1;
       WaitRelease();
       Message("Tile edit mode.",100);
     }
     if (mb && mx>278 && mx<289 && my>z+12 && my<z+23)
     {
       WaitRelease();
       th=th ^ 1;
       if (th) Message("Tile Highlight enabled.",100);
          else Message("Tile Highlight disabled.",100);
        mb=0;
     }
     if (mb && mx>278 && mx<289 && my>z+23 && my<z+47)
     {
       WaitRelease();
       selofs=0;
       pad=pad ^ 1;
       if (pad) Message("Tile padding enabled.",100);
           else Message("Tile padding disabled.",100);
        mb=0;
     }
     if (mb && mx>161 && mx<210 && my>z+12 && my<z+23)
     {
        modified=1;
        PressdButton(161, z+12, 50, 12, "Add Tiles");
        while (mb)
        {
          ReadMouse();
          ShowPage();
        }
        WaitRelease();
        AddTilesDialog();
        mb=0;
     }
     if (mb && mx>161 && mx<210 && my>z+23 && my<z+47)
     {
        PressdButton(161, z+23, 50, 12, "Animation");
        while (mb)
        {
          ReadMouse();
          ShowPage();
        }
        WaitRelease();
        VSPAnimDialog();
        mb=0;
     }
     if (mb && mx>220 && mx<270 && my>z+12 && my<z+24)
     {
        PressdButton(220, z+12, 50, 12, "Import VSP");
        while (mb)
        {
          ReadMouse();
          ShowPage();
        }
        WaitRelease();
        ImportVSP();
        modified=1;
        mb=0;
     }
     if (mb && mx>220 && mx<270 && my>z+23 && my<z+47)
     {
        PressdButton(220, z+23, 50, 12, "Import PCX");
        while (mb)
        {
          ReadMouse();
          ShowPage();
        }
        WaitRelease();
        ImportPCX();
        modified=1;
        mb=0;
     }
     if (key[SCAN_F10])
     {
      key[SCAN_F10]=0;
      SaveMAP(mapname);
      SaveVSP(vspname);
      Message("MAP/VSP saved.",100);
      modified=0;
    }
  }
  key[SCAN_ESC]=0;
}

int vaxsize=150, vaysize=66;
int vaxofs=20, vayofs=20;

void VSPAnimDialog()
{ char done=0, moving=0, i=0;
  int mxo, myo, cursorblink;
  char ctf=0, cb=0, c, t, *str, bleh;
  char nstr1[80], nstr2[80], nstr3[80], nstr4[80];

  bleh=th;
  th=0;
  i=curastd;
  sprintf(nstr1, "%d", vspanim[i].start);
  sprintf(nstr2, "%d", vspanim[i].finish);
  sprintf(nstr3, "%d", vspanim[i].delay);
  sprintf(nstr4, "%d", vspanim[i].mode);
  last_pressed=0;
  do
  {
    RenderSelector();
    RenderMessages();

    if (ctf && systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      vaxofs = (mx-mxo);
      vayofs = (my-myo);
      if (!mb) moving=0;
      if (vaxofs<16) vaxofs = 16;
      if (vayofs<16) vayofs = 16;
      if (vaxofs+vaxsize>335) vaxofs = 336-vaxsize;
      if (vayofs+vaysize>sy+16) vayofs = (sy+16)-vaysize;
    }

    Window(vaxofs, vayofs, vaxofs+vaxsize, vayofs+vaysize, "VSP Animation Editor");
    CopyTile(vaxofs+110, vayofs+16, (char *) (vsp+(256*tileidx[vspanim[i].start])));
    GotoXY(vaxofs+4, vayofs+12); printstring("Start");
    TextField(vaxofs+25, vayofs+11, 60, nstr1, ctf == 1 ? cb : 0);
    GotoXY(vaxofs+11, vayofs+22); printstring("End");
    TextField(vaxofs+25, vayofs+21, 60, nstr2, ctf == 2 ? cb : 0);
    GotoXY(vaxofs+4, vayofs+32); printstring("Delay");
    TextField(vaxofs+25, vayofs+31, 60, nstr3, ctf == 3 ? cb : 0);
    GotoXY(vaxofs+6, vayofs+42); printstring("Mode");
    TextField(vaxofs+25, vayofs+41, 60, nstr4, ctf == 4 ? cb : 0);
    GotoXY(vaxofs+2, vayofs+56); printstring("Strand");
    GotoXY(vaxofs+104, vayofs+33); printstring("Preview");
    HLine(vaxofs+1, vayofs+51, 148, white);
    Button(vaxofs+85, vayofs+54, "OK");
    Button(vaxofs+117, vayofs+54, "Update");
    GotoXY(vaxofs+30, vayofs+56);
    sprintf(strbuf, "%d", i); printstring(strbuf);

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mb==1 && mx>vaxofs+25 && mx<vaxofs+60+25 && my>vayofs+11 && my<vayofs+9+11 && !moving)
    {
      ctf=1; str=nstr1; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>vaxofs+25 && mx<vaxofs+60+25 && my>vayofs+21 && my<vayofs+9+21 && !moving)
    {
      ctf=2; str=nstr2; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>vaxofs+25 && mx<vaxofs+60+25 && my>vayofs+31 && my<vayofs+9+31 && !moving)
    {
      ctf=3; str=nstr3; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mb==1 && mx>vaxofs+25 && mx<vaxofs+60+25 && my>vayofs+41 && my<vayofs+9+41 && !moving)
    {
      ctf=4; str=nstr4; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if ((mb==1 && mx>vaxofs+85 && mx<vaxofs+30+85 && my>vayofs+54 && my<vayofs+10+54 && !moving) || (key[SCAN_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      done=1;
      ButtonPressed(vaxofs+85, vayofs+54, "OK");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mb==1 && mx>vaxofs+117 && mx<vaxofs+30+117 && my>vayofs+54 && my<vayofs+10+54 && !moving)
    {
      // insert code for button "Update" here
      vspanim[i].start=atoi(nstr1);
      vspanim[i].finish=atoi(nstr2);
      vspanim[i].delay=atoi(nstr3);
      vspanim[i].mode=atoi(nstr4);
      InitTileIDX();
      ButtonPressed(vaxofs+117, vayofs+54, "Update");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mb==1 && mx>(vaxofs+vaxsize-9) && (mx<vaxofs+vaxsize-2)
    && my>(vayofs+1) && (my<vayofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mb==1 && mx>vaxofs && mx<vaxofs+vaxsize && my>vayofs && my<(vayofs+9) && !moving)
    {
      moving=1;
      mxo=mx-vaxofs;
      myo=my-vayofs;
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

      if (last_pressed==SCAN_UP)
      {
        vspanim[i].start=atoi(nstr1);
        vspanim[i].finish=atoi(nstr2);
        vspanim[i].delay=atoi(nstr3);
        vspanim[i].mode=atoi(nstr4);
        if (i<99) i++; else i=0;
        last_pressed=0; key[SCAN_UP]=0;
        sprintf(nstr1, "%d", vspanim[i].start);
        sprintf(nstr2, "%d", vspanim[i].finish);
        sprintf(nstr3, "%d", vspanim[i].delay);
        sprintf(nstr4, "%d", vspanim[i].mode);
        continue;
      }
      if (last_pressed==SCAN_DOWN)
      {
        vspanim[i].start=atoi(nstr1);
        vspanim[i].finish=atoi(nstr2);
        vspanim[i].delay=atoi(nstr3);
        vspanim[i].mode=atoi(nstr4);
        if (i) i--; else i=99;
        last_pressed=0; key[SCAN_DOWN]=0;
        sprintf(nstr1, "%d", vspanim[i].start);
        sprintf(nstr2, "%d", vspanim[i].finish);
        sprintf(nstr3, "%d", vspanim[i].delay);
        sprintf(nstr4, "%d", vspanim[i].mode);
        continue;
      }
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
//  memset(&vadelay, 0, 100);
//  for (i=0; i<numtiles; i++)
//    tileidx[i]=i;
  vspanim[i].start=atoi(nstr1);
  vspanim[i].finish=atoi(nstr2);
  vspanim[i].delay=atoi(nstr3);
  vspanim[i].mode=atoi(nstr4);
  curastd=i;
  th=bleh;
}

int atxsize=150, atysize=39;
int atxofs=80, atyofs=50;

void AddTilesDialog()
{ char done=0, moving=0;
  int mxo, myo, cursorblink, at=0; //, i;
  char ctf=0, cb=0, c, t, *str, bleh;
  char nstr1[80];

  bleh=th;
  th=0;
  sprintf(nstr1, "%d", at);
  last_pressed=0;
  do
  {
    RenderSelector();
    RenderMessages();

    if (ctf && systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      atxofs = (mx-mxo);
      atyofs = (my-myo);
      if (!mb) moving=0;
      if (atxofs<16) atxofs = 16;
      if (atyofs<16) atyofs = 16;
      if (atxofs+atxsize>335) atxofs = 336-atxsize;
      if (atyofs+atysize>sy+16) atyofs = (sy+16)-atysize;
    }

    Window(atxofs, atyofs, atxofs+atxsize, atyofs+atysize, "Allocate Additional Tiles");
    GotoXY(atxofs+12, atyofs+15); printstring("Alloc Tiles:");
    TextField(atxofs+56, atyofs+13, 80, nstr1, ctf == 1 ? cb : 0);
    Button(atxofs+85, atyofs+26, "OK");
    Button(atxofs+117, atyofs+26, "Cancel");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mb==1 && mx>atxofs+56 && mx<atxofs+80+56 && my>atyofs+13 && my<atyofs+9+13 && !moving)
    {
      ctf=1; str=nstr1; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if ((mb==1 && mx>atxofs+85 && mx<atxofs+30+85 && my>atyofs+26 && my<atyofs+10+26 && !moving) || (key[SCAN_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      done=1;
      at=atoi(nstr1);
      str=(char *) valloc((numtiles+at)*256,"vsp data",0);
      memcpy(str,vsp,(numtiles*256));
      vfree(vsp); vsp=str; str=0; ctf=0;
      memset(vsp+(numtiles*256), 0, at*256);
      numtiles+=at;
      ButtonPressed(atxofs+85, atyofs+26, "OK");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mb==1 && mx>atxofs+117 && mx<atxofs+30+117 && my>atyofs+26 && my<atyofs+10+26 && !moving)
    {
      // insert code for button "Cancel" here
      done=1;
      ButtonPressed(atxofs+117, atyofs+26, "Cancel");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mb==1 && mx>(atxofs+atxsize-9) && (mx<atxofs+atxsize-2)
    && my>(atyofs+1) && (my<atyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mb==1 && mx>atxofs && mx<atxofs+atxsize && my>atyofs && my<(atyofs+9) && !moving)
    {
      moving=1;
      mxo=mx-atxofs;
      myo=my-atyofs;
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
          case 0: ctf=1; str=nstr1; break;
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
  at=atoi(nstr1);
  th=bleh;
//  memset(&vadelay, 0, 100);
//  for (i=0; i<numtiles; i++)
//    tileidx[i]=i;
}

// ========================= Import *** Data/Code ============================

char *ovsp, op, oh, openas;
word ontiles;
word sofs=0;
word tilesrippedthissession;
byte *implist; //zero

void RenderImport()
{ unsigned char *img,i,j;
  int z, jumper,wide,deep;
  int at;

  jumper=16+pad;
  wide=320/jumper;
  deep=((200+15)&~15)/jumper;

  ClearScreen();

  for (i=0; i<deep; i++)
    for (j=0; j<wide; j++)
    {
      at=(i*wide)+j+sofs;
      img=ovsp+(256*at);
      if (at>=ontiles) img=ovsp;
      CopyTile((j*jumper)+16,(i*jumper)+16,img);
      if(at<ontiles) if(implist[at]) ColorGrid((j*jumper)+16,(i*jumper)+16,7); //zero
    }
  if (th)
  {
    if (my<(tsy-40))
    DrawHighlight(((mx/jumper)*jumper)-(1+pad),
      ((my/jumper)*jumper)-(1+pad));
  }
  z=tsy-40;
  sprintf(strbuf,"Import Tiles %d/%d",curtileptr(sofs),ontiles);
  Window(16,z,336,z+40,strbuf);
  sprintf(strbuf,"Tiles imported this session: %d",tilesrippedthissession);
  GotoXY(176-(pixels(strbuf)/2),z+20); printstring(strbuf);
}

void ImportTiles()
{ //int tp;
  char *str;
  int bloop1,bloop2;

  op=pad; oh=th;
  pad=1; th=1;
  tilesrippedthissession=0;
  implist=(byte *)malloc(ontiles); //zero
  memset(implist,0,ontiles); //zero
  while (!key[SCAN_ESC])
  {
    RenderImport();
    RenderMessages();
    DrawMouse();
    ShowPage();

     if (key[SCAN_DOWN] && sofs<65319)
     {
       if (!pad) sofs=sofs+20;
       else sofs+=18;
       key[SCAN_DOWN]=0;
     }
     if (key[SCAN_UP] && sofs>17)
     {
       if (!pad) sofs-=20;
       else sofs-=18;
       key[SCAN_UP]=0;
     }
/*all by zero here in this scan_b thing*/
     if (key[SCAN_B])
     {
       key[SCAN_B]=0;
       Message("Building duplicate import list",200);
       for(bloop1=0; bloop1<ontiles; bloop1++)
       {
         for(bloop2=0; bloop2<numtiles; bloop2++)
         {
           if(TileCmp(vsp+bloop2*256,ovsp+bloop1*256))
           {
             implist[bloop1]=1;
             bloop2=numtiles;
           }
         }
       }
     }
//all by zero here in this scan_a thing
     if (key[SCAN_A])
     {
       Message("Importing all tiles",200);
       key[SCAN_A]=0;
       for(bloop1=0; bloop1<ontiles; bloop1++)
       {
         str=(char *) valloc((numtiles+1)*256,"vsp data",0);
         memcpy(str, vsp, (numtiles*256));
         vfree(vsp); vsp=str; str=0;
         memcpy(vsp+(numtiles*256), ovsp+(bloop1*256), 256);
         numtiles+=1;
       }
       goto DIE;
     }

//all by zero here in this scan_n thing
     if (key[SCAN_N])
     {
       Message("Importing all new tiles",200);
       key[SCAN_N]=0;
       for(bloop1=0; bloop1<ontiles; bloop1++)
       {
         for(bloop2=0; bloop2<numtiles; bloop2++)
         {
           if(TileCmp(vsp+bloop2*256,ovsp+bloop1*256))
           {
             implist[bloop1]=1;
             bloop2=numtiles;
           }
         }
       }
       for(bloop1=0; bloop1<ontiles; bloop1++)
       {
         if(!implist[bloop1])
         {
           str=(char *) valloc((numtiles+1)*256,"vsp data",0);
           memcpy(str, vsp, (numtiles*256));
           vfree(vsp); vsp=str; str=0;
           memcpy(vsp+(numtiles*256), ovsp+(bloop1*256), 256);
           numtiles+=1;
         }
       }
       goto DIE;
     }

     if (mb==1 && my<(tsy-40) && curtileptr(sofs)<ontiles)
     {
        lt=curtileptr(sofs);
        implist[lt]=1; //zero
        str=(char *) valloc((numtiles+1)*256,"vsp data",0);
        memcpy(str, vsp, (numtiles*256));
        vfree(vsp); vsp=str; str=0;
        memcpy(vsp+(numtiles*256), ovsp+(lt*256), 256);
        numtiles+=1;
        tilesrippedthissession++;
        WaitRelease();
        sprintf(strbuf,"Tile %d imported.",lt);
        Message(strbuf, 200);
     }
  }
  DIE: //zero
  pad=op; th=oh;
  vfree(implist); //zero
}

void ImportVSP()
{ FILE *f;
  word ver;
  char fname[60];
  int bsize;

  memset(fname, 0, 60);
  bmode=1; oh=th; th=0;
  Browse(1,"*.vsp","","","",&fname);
  if (strlen(fname)<3) return;
  if (!(f=fopen(fname,"rb")))
     errf("VSP file %s not found.",fname);
  WaitRelease(); th=oh; bmode=0;

  fread(&ver, 1, 2, f);
  if (ver!=2 && ver!=3)
     errf("VSP file %s is not the correct VSP format.",fname);
  fread(strbuf, 1, 768, f);
  fread(&ontiles, 1, 2, f);
  ovsp=(char *) valloc(256*ontiles,"ovsp",0);

  if (ver==2)
    fread(ovsp, 256, ontiles, f);
  if (ver==3)
  {
    fread(&bsize, 1, 4, f);
    ReadCompressedLayer1(ovsp, 256*ontiles, f);
  }

  fclose(f);

  ImportTiles();
  vfree(ovsp);
  key[SCAN_ESC]=0;

}

int OPxsize=100, OPysize=40;
int OPxofs=100, OPyofs=70;

void OpenPCXas()
{ char done=0, moving=0;
  int mxo, myo, cursorblink;
  char ctf=0, cb=0, c, t, *str;

  last_pressed=0;
  do
  {
    RenderSelector();
    RenderMessages();

    if (ctf && systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      OPxofs = (mx-mxo);
      OPyofs = (my-myo);
      if (!mb) moving=0;
      if (OPxofs<16) OPxofs = 16;
      if (OPyofs<16) OPyofs = 16;
      if (OPxofs+OPxsize>335) OPxofs = 336-OPxsize;
      if (OPyofs+OPysize>sy+16) OPyofs = (sy+16)-OPysize;
    }

    Window(OPxofs, OPyofs, OPxofs+OPxsize, OPyofs+OPysize, "Open PCX As");
    Button(OPxofs+21, OPyofs+18, "Tileset");
    Button(OPxofs+50, OPyofs+18, "Image");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mb==1 && mx>OPxofs+21 && mx<OPxofs+30+21 && my>OPyofs+18 && my<OPyofs+10+18 && !moving)
    {
      // insert code for button "Tileset" here
      ButtonPressed(OPxofs+21, OPyofs+18, "Tileset");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      openas=0; done=1;
      WaitRelease(); continue;
    }

    if (mb==1 && mx>OPxofs+50 && mx<OPxofs+30+50 && my>OPyofs+18 && my<OPyofs+10+18 && !moving)
    {
      // insert code for button "Image" here
      ButtonPressed(OPxofs+50, OPyofs+18, "Image");
      DrawMouse();
      while (mb)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      openas=1; done=1;
      WaitRelease(); continue;
    }

    if (mb==1 && mx>(OPxofs+OPxsize-9) && (mx<OPxofs+OPxsize-2)
    && my>(OPyofs+1) && (my<OPyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mb==1 && mx>OPxofs && mx<OPxofs+OPxsize && my>OPyofs && my<(OPyofs+9) && !moving)
    {
      moving=1;
      mxo=mx-OPxofs;
      myo=my-OPyofs;
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

char manufacturer;
char version;
char encoding;
char bits_per_pixel;
short xmin,ymin;
short xmax,ymax;
short hres;
short vres;
char palette[48];
char reserved;
char color_planes;
short bytes_per_line;
short palette_type;
char filler[58];
char *t;

int width,depth;
int bytes;
int vidoffset;
FILE *pcxf;

void ReadPCXLine(unsigned char *dest)
{ int i, n;
  char c, run;

  n=0;
  do {
    c=fgetc(pcxf) & 0xff;

    if ((c & 0xc0)==0xc0)
    { run=c & 0x3f;
      c=fgetc(pcxf);
      for (i=0; i<run; i++)
        dest[vidoffset+n+i]=c;
      n+=run;
    }
    else {
      dest[vidoffset+n]=c;
      n++;
    }
  } while (n<bytes);
}

void LoadPCXHeader(char *fname)
{
  int i;

  if (!(pcxf=fopen(fname,"rb"))) err("Could not open specified PCX file.");
  fread(&manufacturer,1,1,pcxf);
  fread(&version,1,1,pcxf);
  fread(&encoding,1,1,pcxf);
  fread(&bits_per_pixel,1,1,pcxf);
  fread(&xmin,1,2,pcxf);
  fread(&ymin,1,2,pcxf);
  fread(&xmax,1,2,pcxf);
  fread(&ymax,1,2,pcxf);
  fread(&hres,1,2,pcxf);
  fread(&vres,1,2,pcxf);
  fread(&palette,1,48,pcxf);
  fread(&reserved,1,1,pcxf);
  fread(&color_planes,1,1,pcxf);
  fread(&bytes_per_line,1,2,pcxf);
  fread(&palette_type,1,2,pcxf);
  fread(&filler,1,58,pcxf);
  fseek(pcxf,-768L,SEEK_END);
  fread(pal,1,768,pcxf);
  fseek(pcxf,128L,SEEK_SET);
  width=xmax-xmin+1;
  depth=ymax-ymin+1;
  bytes=bytes_per_line;

  for (i=0; i<768; i++)
    pal[i]=pal[i] >> 2;
  set_intensity(63);
}

void LoadPCXHeaderNP(char *fname)
{
  pcxf=fopen(fname,"rb");
  if (!pcxf) err("Could not open specified PCX file.");
  fread(&manufacturer,1,1,pcxf);
  fread(&version,1,1,pcxf);
  fread(&encoding,1,1,pcxf);
  fread(&bits_per_pixel,1,1,pcxf);
  fread(&xmin,1,2,pcxf);
  fread(&ymin,1,2,pcxf);
  fread(&xmax,1,2,pcxf);
  fread(&ymax,1,2,pcxf);
  fread(&hres,1,2,pcxf);
  fread(&vres,1,2,pcxf);
  fread(&palette,1,48,pcxf);
  fread(&reserved,1,1,pcxf);
  fread(&color_planes,1,1,pcxf);
  fread(&bytes_per_line,1,2,pcxf);
  fread(&palette_type,1,2,pcxf);
  fread(&filler,1,58,pcxf);
  width=xmax-xmin+1;
  depth=ymax-ymin+1;
  bytes=bytes_per_line;
}

// grabs a tile from the ovsp buffer (transit tile bay)
int GrabTileAt(int x, int y, int z, int check_dup)
{ int a;
  char tvsp[256];
  char *s;

  s=t+(y*width)+x;

  for (a=0; a<16; a++)
  {
    memcpy(tvsp+(a*16), s, 16);
    s+=width;
  }

  if (check_dup)
  {
  for (a=0; a<z; a++)
    if (!memcmp(tvsp, ovsp+(a*256), 256)) return 0;
  }

  memcpy(ovsp+(z*256), tvsp, 256);
  return 1;
}

void ImportPCX()
{ char fname[60];
  int i,j,z;
  int wide,deep; // grabbable x&y tiles
  int add,jumper; // related to grab coords

  bmode=1; oh=th; th=0;
  Browse(1,"*.pcx","","","",fname);
  WaitRelease();
  if (strlen(fname)<3) return;
  OpenPCXas();
  th=oh; bmode=0;

  add=!openas;
  jumper=16+add;

  LoadPCXHeader(fname);

  // temp pcx buffer for grabs
  t=(char *) valloc(width*depth,"pcx buf",0);

  for (i=0; i<depth; i++)
  {
    vidoffset=width*i;
    ReadPCXLine(t);
  }

  fclose(pcxf);

  deep=((depth&~15)+add)/jumper;
  wide=(width+add)/jumper;

  ontiles=wide*deep;
  ovsp=(char *) valloc(ontiles*256,"ovsp",0); // temp tilebay

  z=0;
  for (i=0; i<deep; i++) // grab all tiles
    for (j=0; j<wide; j++)
    {
      z+=GrabTileAt((j*jumper)+add, (i*jumper)+add, z, openas);
    }

  ontiles = z; // tiles successfully grabbed
  vfree(t);

  ImportTiles();
  vfree(ovsp);
  key[SCAN_ESC]=0;
}
