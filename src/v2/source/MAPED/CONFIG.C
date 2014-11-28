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
#include <stdlib.h>

#include "gui.h"
#include "mikmod.h"
#include "maped.h"

#include "smalfont.h"

// ================================= Data ====================================

FILE *cfg;
char *strbuf;

char vm=0;                             // Video mode
char white=0, black=0;                   // Color definitions
char pad=0;                            // Padding mode in tile selector.
char th=0, mh=0;                         // tile-highlight, map highlight

// GUI default colors

char winbg=23;
char titlebg=125;
char brightw=31;
char darkw=13;
char darkred=44;

// ------------------

extern char scrollmode, vcedprog[80];
extern int difficulty;

// ================================= Code ====================================

void InitializeDefaults()
{
  vm=0;
  white=31;
  black=1;
  pad=1;
  th=1;
  mh=1;
  mouse_scroll=1;
  scrollmode=1;
  memcpy(vcedprog,"edit",5);
}

void GetConfig(char *fname)
{
  strbuf=(char *) valloc(2000,"strbuf!",0);
  InitializeDefaults();
  if (!(cfg=fopen(fname,"r")))
  {
    printf("Unable to open config file. Using defaults... \n");
    return;
  }

  while (1)
  {
    fscanf(cfg,"%s",strbuf);

    if (!strcmp(strbuf,"difficulty"))
    { fscanf(cfg,"%s",strbuf);
      difficulty=atoi(strbuf); continue; }
    if (!strcmp(strbuf,"nocdaudio"))
    {
      //nocdaudio=1;
      continue;
    }
    if (!strcmp(strbuf,"vidmode"))
    { fscanf(cfg,"%s",strbuf);
      vm=atoi(strbuf); continue; }
    if (!strcmp(strbuf,"white"))
    { fscanf(cfg,"%s",strbuf);
      white=atoi(strbuf); brightw=atoi(strbuf);
      TextColor(atoi(strbuf)); continue; }
    if (!strcmp(strbuf,"black"))
    { fscanf(cfg,"%s",strbuf);
      black=atoi(strbuf); continue; }
    if (!strcmp(strbuf,"winbg"))
    { fscanf(cfg,"%s",strbuf);
      winbg=atoi(strbuf); continue; }
    if (!strcmp(strbuf,"shadow"))
    { fscanf(cfg,"%s",strbuf);
      darkw=atoi(strbuf); continue; }
    if (!strcmp(strbuf,"darkred"))
    { fscanf(cfg,"%s",strbuf);
      darkred=atoi(strbuf); continue; }
    if (!strcmp(strbuf,"titlebg"))
    { fscanf(cfg,"%s",strbuf);
      titlebg=atoi(strbuf); continue; }
    if (!strcmp(strbuf,"pad"))
    { fscanf(cfg,"%s",strbuf);
      pad=atoi(strbuf); continue; }
    if (!strcmp(strbuf, "scrollmode"))
    { fscanf(cfg,"%s",strbuf);
      scrollmode=atoi(strbuf); continue; }
    // aen
    if (!strcmp(strbuf, "mouse_scroll"))
    { fscanf(cfg,"%s",strbuf);
      mouse_scroll=atoi(strbuf)&1; continue; }
    if (!strcmp(strbuf, "md_device"))
    { fscanf(cfg, "%s", strbuf);
      md_device=atoi(strbuf); continue; }
    if (!strcmp(strbuf, "th"))
    { fscanf(cfg,"%s",strbuf);
      th=atoi(strbuf); continue; }
    if (!strcmp(strbuf, "mh"))
    { fscanf(cfg,"%s",strbuf);
      mh=atoi(strbuf); continue; }
    if (!strcmp(strbuf, "amxofs"))
    { fscanf(cfg,"%s",strbuf);
      amxofs=atoi(strbuf); continue; }
    if (!strcmp(strbuf, "amyofs"))
    { fscanf(cfg,"%s",strbuf);
      amyofs=atoi(strbuf); continue; }
    if (!strcmp(strbuf, "mmxofs"))
    { fscanf(cfg,"%s",strbuf);
      mmxofs=atoi(strbuf); continue; }
    if (!strcmp(strbuf, "mmyofs"))
    { fscanf(cfg,"%s",strbuf);
      mmyofs=atoi(strbuf); continue; }
    if (!strcmp(strbuf, "cnxofs"))
    { fscanf(cfg,"%s",strbuf);
      cnxofs=atoi(strbuf); continue; }
    if (!strcmp(strbuf, "cnyofs"))
    { fscanf(cfg,"%s",strbuf);
      cnyofs=atoi(strbuf); continue; }
    if (!strcmp(strbuf, "lmxofs"))
    { fscanf(cfg,"%s",strbuf);
      lmyofs=atoi(strbuf); continue; }
    if (!strcmp(strbuf, "lmyofs"))
    { fscanf(cfg,"%s",strbuf);
      lmyofs=atoi(strbuf); continue; }
    if (!strcmp(strbuf, "lvxofs"))
    { fscanf(cfg,"%s",strbuf);
      lvxofs=atoi(strbuf); continue; }
    if (!strcmp(strbuf, "lvyofs"))
    { fscanf(cfg,"%s",strbuf);
      lvyofs=atoi(strbuf); continue; }
    if (!strcmp(strbuf, "mpxofs"))
    { fscanf(cfg,"%s",strbuf);
      mpxofs=atoi(strbuf); continue; }
    if (!strcmp(strbuf, "mpyofs"))
    { fscanf(cfg,"%s",strbuf);
      mpyofs=atoi(strbuf); continue; }
    if (!strcmp(strbuf, "vaxofs"))
    { fscanf(cfg,"%s",strbuf);
      vaxofs=atoi(strbuf); continue; }
    if (!strcmp(strbuf, "vayofs"))
    { fscanf(cfg,"%s",strbuf);
      vayofs=atoi(strbuf); continue; }
    if (!strcmp(strbuf, "zexofs"))
    { fscanf(cfg,"%s",strbuf);
      zexofs=atoi(strbuf); continue; }
    if (!strcmp(strbuf, "zeyofs"))
    { fscanf(cfg,"%s",strbuf);
      zeyofs=atoi(strbuf); continue; }
    if (!strcmp(strbuf, "exofs"))
    { fscanf(cfg,"%s",strbuf);
      exofs=atoi(strbuf); continue; }
    if (!strcmp(strbuf, "eyofs"))
    { fscanf(cfg,"%s",strbuf);
      eyofs=atoi(strbuf); continue; }
    if (!strcmp(strbuf, "prxofs"))
    { fscanf(cfg,"%s",strbuf);
      prxofs=atoi(strbuf); continue; }
    if (!strcmp(strbuf, "pryofs"))
    { fscanf(cfg,"%s",strbuf);
      pryofs=atoi(strbuf); continue; }
    if (!strcmp(strbuf, "rsxofs"))
    { fscanf(cfg,"%s",strbuf);
      rsxofs=atoi(strbuf); continue; }
    if (!strcmp(strbuf, "rsyofs"))
    { fscanf(cfg,"%s",strbuf);
      rsyofs=atoi(strbuf); continue; }
    if (!strcmp(strbuf,"vcedprog"))
    { fscanf(cfg,"%s",vcedprog);
      continue; }
    break;
  }
  fclose(cfg);
}
