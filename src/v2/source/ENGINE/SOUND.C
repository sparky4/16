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

#define SOUND_H
#include "verge.h"

// ================================= Data ====================================

UNIMOD *mf;
SAMPLE *sfx[100];
int nsfx=0;
char playing[60];

// ================================= Code ====================================

void PlaySound(int, int, int);

void tickhandler()
{
  MP_HandleTick();
  MD_SetBPM(mp_bpm);
}

void InitMusicSystem()
{
  memset(playing, 0, 60);
  ML_RegisterLoader(&load_mod);
  ML_RegisterLoader(&load_s3m);
  ML_RegisterLoader(&load_uni);
  ML_RegisterLoader(&load_xm);

  MD_RegisterDriver(&drv_nos);
//  MD_RegisterDriver(&drv_ss);
  MD_RegisterDriver(&drv_sb);
  MD_RegisterDriver(&drv_gus);

  MD_RegisterPlayer(tickhandler);
  if (!MD_Init())
  {
    printf("Driver error: %s.\n",myerr);
    exit(-1);
  }
}

void PlayMusic(char *fname)
{
  strlwr(fname);
  if (!strcmp(fname,playing)) return;
  if (!MP_Ready())
  {
    MD_PlayStop();
    ML_Free(mf);
  }
  memcpy(playing, fname, strlen(fname));
  if (!(mf=ML_LoadFN(fname))) err(myerr);
  MP_Init(mf);
  md_numchn=mf->numchn+2;
  mp_loop=1; mp_volume=100;
  MD_PlayStart();
  PlaySound(0, 0, 0);
  PlaySound(0, 0, 0);
}

void StopMusic()
{
  if (MP_Ready()) return;
  MD_PlayStop();
  ML_Free(mf);
}

int CacheSound(char *fname)
{
  if (!(sfx[nsfx]=MW_LoadWavFN(fname)))
    err("WAV [%s] load error: %s", fname, myerr);
  return nsfx++;
}

void FreeAllSounds()
{
  int i;

  for (i=0; i<nsfx; i++)
      MW_FreeWav(sfx[i]);
  nsfx=0;
}

void PlaySound(int index, int vol, int pan)
{
  static int switcher=1;
  int chanl;

  if (index >= nsfx) return;
  switcher ^= 1;
  chanl=md_numchn-1-switcher;
  MD_VoiceSetVolume(chanl, vol);
  MD_VoiceSetPanning(chanl, pan);
  MD_VoiceSetFrequency(chanl, sfx[index]->c2spd);
  MD_VoicePlay(chanl, sfx[index]->handle, 0, sfx[index]->length,
               0, 0, sfx[index]->flags);
}
