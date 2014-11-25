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

#include "verge.h"

// ================================= Data ====================================

byte cpu_watch, cpubyte=0;

// ================================= Code ====================================

void ListMounts()
{ int i;

  Con_NextLine();
  sprintf(strbuf,"There are Å%d~ files mounted.",filesmounted);
  Con_Printf(strbuf);
  for (i=0; i<filesmounted; i++)
  {
    sprintf(strbuf,"File Ä%s~ contains Å%d~ files.",pack[i].mountname, pack[i].numfiles);
    Con_Printf(strbuf);
  }
}

void PackInfo()
{ int i,j;

  Con_NextLine();
  i=atoi((char *) args[1]);
  sprintf(strbuf,"Files in %s:",pack[i].mountname);
  Con_Printf(strbuf);

  for (j=0; j<pack[i].numfiles; j++)
  {
    sprintf(strbuf,"Ä%s~              ",pack[i].files[j].fname);
    sprintf(&strbuf[20],"Å%d~ bytes",pack[i].files[j].size);
    Con_Printf(strbuf);
  }
}

void FileInfo()
{ VFILE *f;

  Con_NextLine();
  sprintf(strbuf,"File stats for Ä%s~:",args[1]);
  Con_Printf(strbuf);

  f=vopen((char *) args[1]);
  if (!f)
  {
    Con_Printf("File not found.");
    return;
  }

  if (f->s)
  {
    sprintf(strbuf,"File is in Ä%s~, index Å%d~",pack[f->v].mountname, f->i);
    Con_Printf(strbuf);
    sprintf(strbuf,"Packofs: Å%d~ current ofs: Å%d~",pack[f->v].files[f->i].packofs,pack[f->v].files[f->i].curofs);
    Con_Printf(strbuf);
    if (pack[f->v].files[f->i].extractable)
       Con_Printf("File is extractable.");
    else Con_Printf("File is not extractable.");
    if (pack[f->v].files[f->i].override)
       Con_Printf("Override allowed.");
    else Con_Printf("Override not allowed.");
  }
  else Con_Printf("File is external.");
  sprintf(strbuf,"File is Å%d~ bytes.",filesize(f));
  Con_Printf(strbuf);
  vclose(f);
}

void vid_mode()
{ int xres,yres;

  xres=atoi((char *)args[1]);
  yres=atoi((char *)args[2]);
  Con_NextLine();

  ShutdownVideo(0);
  if (InitVideo(xres, yres))
  {
    Con_Printf("{||||||||||||}");
    Con_Printf("Loading new video driver...");
    Con_Printf(DriverDesc);
    Con_Printf("{||||||||||||}");
  }
  else Con_Printf("Unsupported/unknown video mode.");
  set_intensity(63);
}

void CPU_Usage()
{
  if (numargs==1)
  {
    sprintf(strbuf,"cpu_usage is Å%d~", cpu_watch);
    Con_Printf(strbuf);
  }
  else cpu_watch=atoi((char *) args[1]);
}

void Mount()
{
  MountVFile((char *) args[1]);
  sprintf(strbuf,"%s mounted.",args[1]);
  Con_Printf(strbuf);
}

void ZeroConWarp(char **args)
{
  player->x=atoi(args[1])*16;
  player->y=atoi(args[2])*16;
  player->tx=atoi(args[1]);
  player->ty=atoi(args[2]);
}

void ZeroConBrowseTiles(void)
{
  int x,y,n,k=0,a=0;

  while(last_pressed!=SCAN_Q)
  {
    ClearScreen();
    UpdateControls();
    CheckMessages();
    if(last_pressed==SCAN_A) { if(a) a=0; else a=1; last_pressed=0; }
    if(last_pressed==SCAN_DOWN&&(k+(ty-3)*(tx-3)-tx+4)<numtiles)
    {
      k+=tx-4;
      last_pressed=0;
    }
    if(last_pressed==SCAN_UP&&k>0)
    {
      k-=tx-4;
      last_pressed=0;
    }
    for(y=1; y<ty-2; y++)
    {
      for(x=1; x<tx-3; x++)
      {
        n=((y-1)*(tx-4)+x-1+k);
        if (n<numtiles)
        {
          if (!a) CopyTileClip(x*16,y*16,(unsigned char*)((unsigned int)vsp+((y-1)*(tx-4)+x-1+k)*256));
          else CopyTileClip(x*16,y*16,(unsigned char*)((unsigned int)vsp+tileidx[(y-1)*(tx-4)+x-1+k]*256));
        }

      }
      GotoXY((tx-2)*16-8,y*16+5);
      sprintf(strbuf,"%i",(y-1)*(tx-4)+k);
      printstring(0,strbuf);
    }
  GotoXY(16,(ty-1)*16-8);
  printstring(0,"Hit Q to quit, A to toggle anim,");
  GotoXY(16,(ty-1)*16);
  printstring(0,"up/down to change tiles");
  ShowPage();
  }
}

void CameraTracking()
{
  if (numargs==1)
  {
    sprintf(strbuf,"cameratracking is Å%d~", cameratracking);
    Con_Printf(strbuf);
  }
  else cameratracking=atoi((char *) args[1]);
}

void ZeroSetRString(char **args)
{
  memcpy(rstring,args[1],strlen(args[1])+1);
}

void ZeroGetRString(void)
{
  sprintf(strbuf,"Renderstring: Ä%s~",rstring);
  Con_Printf(strbuf);
}

void Obstructions()
{
  if (numargs==1)
  {
    sprintf(strbuf,"showobs is Å%d~", showobs);
    Con_Printf(strbuf);
  }
  else showobs=atoi((char *) args[1]);
}

void MoveGranularity()
{
  if (numargs==1)
  {
    sprintf(strbuf,"movegranularity is Å%d~", movegranularity);
    Con_Printf(strbuf);
  }
  else movegranularity=atoi((char *) args[1]);
}

void Phantom()
{
  if (numargs==1)
  {
    sprintf(strbuf,"phantom is Å%d~", phantom);
    Con_Printf(strbuf);
  }
  else phantom=atoi((char *) args[1]);
}

void CurPos()
{
  Con_NextLine();
  sprintf(strbuf,"xwc: Ç%d~ ywc: Ç%d~", player->x, player->y); Con_Printf(strbuf);
  sprintf(strbuf,"xtc: Ç%d~ ytc: Ç%d~", player->x>>4, player->y>>4); Con_Printf(strbuf);
}

void PlayerSpeed()
{
  if (!player)
  {
    Con_Printf("No player.");
    return;
  }
  if (numargs==1)
  {
    Con_NextLine();
    sprintf(strbuf,"speed is is Å%d~", player->speed);
    Con_Printf(strbuf);
  }
  else
  {
    player->speed=atoi((char *) args[1]);
    player->speedct=0;
  }
}

void Player()
{ int i;

  i=atoi((char *) args[1]);
  if (i<entities)
  {
    player=&entity[i];
    playernum=i;
//    entity[i].movecode=0;
    entity[i].moving=0;
    SiftEntities();
    Con_Printf("Player updated.");
  }
  else Con_Printf("No such entity.");
}

void SpawnEntity()
{ int i;

  i=AllocateEntity(atoi((char *) args[1]), atoi((char *) args[2]), (char *) args[3]);
  sprintf(strbuf,"Entity %d allocated.",i);
  Con_Printf(strbuf);
}

void ShowZones()
{
  if (numargs==1)
  {
    sprintf(strbuf,"showzones is Å%d~", showzone);
    Con_Printf(strbuf);
  }
  else showzone=atoi((char *) args[1]);
}
