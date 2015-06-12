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

#define CONSOLE_H
#include "verge.h"
#define conwidth 40

// ================================= Data ====================================

byte *consolebg;                           // Console background image
char *consoletext;                         // Console text buffer
byte *cmd, *cmd2, cmdlen=0;                // command line buffer
char *lastcmds, numcmds=0, cmdpos=0;       // last-command memory
byte *args[10], numargs;                   // command argument pointers
char cursor=1;                             // flag on/off cursor visible
int cswtime=0;                             // cursor switch time.
int conlines=1;                            // Number of visible lines
int lines=0;                               // number of lines entered since last draw
char startln=36;                           // Start display ofs (for scrollback)
char allowconsole=1;
char consoleoverride=0;

byte key_ascii_tbl[128] =
{
  0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 8,   9,
  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 13,  0,   'a', 's',
  'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', 39,  0,   0,   92,  'z', 'x', 'c', 'v',
  'b', 'n', 'm', ',', '.', '/', 0,   '*', 0,   ' ', 0,   3,   3,   3,   3,   8,
  3,   3,   3,   3,   3,   0,   0,   0,   0,   0,   '-', 0,   0,   0,   '+', 0,
  0,   0,   0,   127, 0,   0,   92,  3,   3,   0,   0,   0,   0,   0,   0,   0,
  13,  0,   '/', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   127,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   '/', 0,   0,   0,   0,   0
};

byte key_shift_tbl[128] =
{
   0,   0,   '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 126, 126,
   'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 126, 0,   'A', 'S',
   'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', 34,  0,   0,   '|', 'Z', 'X', 'C', 'V',
   'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   1,   0,   1,   1,   1,   1,   1,
   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   '-', 0,   0,   0,   '+', 0,
   0,   0,   1,   127, 0,   0,   0,   1,   1,   0,   0,   0,   0,   0,   0,   0,
   13,  0,   '/', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   127,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   '/', 0,   0,   0,   0,   0
};

#define NUMCMDS 31
char *concmds[] = {
  "CONSOLEBG","LISTMOUNTS","PACKINFO","LISTCMDS","CD_PLAY","CD_STOP",
  "CD_OPEN","CD_CLOSE","EXIT","VID_MODE","CPU_USAGE","MOUNT","MAP",
  "VER","BROWSETILES","WARP","CAMERATRACKING","RSTRING","SHOWOBS",
  "PHANTOM","ENTITYSTAT","ACTIVEENTS","ENTITY","CURPOS","PLAYERSPEED",
  "SPEEDDEMON","RV","SV","PLAYER","SPAWNENTITY","SHOWZONES"
  };
byte sortedcmds[NUMCMDS];

// ================================= Code ====================================

void SortConCmds(void)
/* -- ric: 03/Jun/98 --
 * creates the sorted index into concmds
 */
{
  int i,j;
  int temp;

  for (i=0; i<NUMCMDS; i++)
    sortedcmds[i]=i;
  for (i=1; i<NUMCMDS; i++)
  {
    for (j=NUMCMDS-1; j>=i; j--)
      if (strcmp(concmds[sortedcmds[j-1]],concmds[sortedcmds[j]])>0)
      {
        // swap the indices
        temp=sortedcmds[j-1];
        sortedcmds[j-1]=sortedcmds[j];
        sortedcmds[j]=temp;
      }
  }
}

void InitConsole(void)
{
  Logp("Initialize console.");
  LoadFont("system.fnt");
  consolebg=VLoadImageBuf("console.gif");

  consoletext=(char *) valloc(45*50, "consoletext", OID_MISC);
  cmd=(byte *) valloc(40, "InitConsole:cmd", OID_MISC);
  cmd2=(byte *) valloc(40, "InitConsole:cmd2", OID_MISC);
  memset(cmd, 0, 40);
  memset(cmd2, 0, 40);
  memset(consoletext, 0, 2250);

  lastcmds=(char *) valloc(400, "InitConsole:lastcmds", OID_MISC);
  memset(lastcmds, 0, 400);

  SortConCmds();
  LogDone();
}

void DrawConsole();

void Con_Printf(char *str)
{ char tbuf[2250];

  // move buffer up a line
  memcpy(tbuf, consoletext+50, 2200);
  memcpy(consoletext, tbuf, 2200);

  memcpy(consoletext+2200, str, strlen(str)+1);
  lines++;
}

void Con_NextLine(void)
{
  Con_Printf("");
  lines=0;
}

int ShowConsole(void)
{
  conlines+=2;
  if (conlines > 120)
  {
    callback=0;
    conlines=120;
  }
  return 0;
}

int HideConsole(void)
{
  if (conlines > 3)
  {
    conlines-=2;
  }
  else
  {
    conlines=1;
    callback=0;
  }
  return 0;
}

void DrawConsole(void)
{ int i;
  int tagline=0;

  lines=0;
  tagline=conlines;

  Render();
  CopySpriteZoomClip(0,-120+tagline, 320,120, sx,120, consolebg);

  // write console text

  for (i=0; i<9; i++)
  {
    GotoXY(1, 1+(i*10)-(120-tagline));
    printstring(0,consoletext+((startln+i)*50));
  }

  GotoXY(1,101-(120-tagline));
  printstring(0,"]");
  printstring(0,(char *) cmd);

  if (systemtime>=cswtime)
  {
    cursor ^= 1;
    cswtime = systemtime+40;
  }

  if (cursor) printstring(0, "&");

  if (startln<36)
  {
    GotoXY(1, 91-(120-tagline));
    printstring(0, "^   ^   ^   ^   ^   ^   ^   ^   ^   ^");
  }
}

int Args(int num)
{
  if (numargs>=num) return 1;
  sprintf(strbuf,"This function needs Å%d~ arguments.",num-1);
  Con_Printf(strbuf);
  return 0;
}

void ListCmds(void)
{ int i;

  Con_NextLine();
  for (i=0; i<NUMCMDS; i++)
    Con_Printf(concmds[sortedcmds[i]]);
}

void ConsoleBackground()
{
  vfree(consolebg);
  consolebg=VLoadImageBuf((char *) args[1]);
}

void ExecuteCommand(int i)
{
  switch(i)
  {
    case 0: if (Args(2)) ConsoleBackground(); break;
    case 1: ListMounts(); break;
    case 2: if (Args(2)) PackInfo(); break;
    case 3: ListCmds(); break;
    case 4: break; //if (Args(2)) CD_Play(atoi((char *) args[1])); break;
    case 5: break; //CD_Stop(); break;
    case 6: break; //CD_Open_Door(); break;
    case 7: break; //CD_Close_Door(); break;
    case 8: err(""); break;
    case 9: if (Args(3)) vid_mode(); break;
    case 10: CPU_Usage(); break;
    case 11: if (Args(2)) Mount(); break;
    case 12: if (numargs<2) MAPstats(); else MAPswitch(); break;
    case 13: ver(); break;
    case 14: ZeroConBrowseTiles(); break;
    case 15: if (numargs==3) ZeroConWarp((char**)args); else { Con_Printf("Çsyntax:"); Con_Printf("Warp <x> <y>~"); } break;
    case 16: CameraTracking(); break;
    case 17: if (numargs==1) ZeroGetRString();
             if (numargs==2) ZeroSetRString((char**)args); break;
    case 18: Obstructions(); break;
    case 19: Phantom(); break;
    case 20: EntityStat(); break;
    case 21: ListActiveEnts(); break;
    case 22: if (Args(2)) EntityS(); break;
    case 23: CurPos(); break;
    case 24: PlayerSpeed(); break;
    case 25: speeddemon=1; break;
    case 26: ReadVCVar(); break;
    case 27: WriteVCVar(); break;
    case 28: if (Args(2)) Player(); break;
    case 29: if (Args(4)) SpawnEntity(); break;
    case 30: ShowZones(); break;
    case NUMCMDS: Con_Printf((char *) cmd); break;
  }
}

void ParseCommand(void)
{ byte *src;
  // breaks the command string into arguements and stuff. (in cmd2)

  numargs=0;
  src=(byte *) cmd2;
  args[0]=src;

  while (*src==' ') src++;

  if (!*src) return;

  while (1)
  {
    if (*src==' ')
    {
      while (*src== ' ')
      {
        *src=0;
        src++;
      }
      if (*src)
      {
        numargs++;
        args[numargs]=src;
      }
      src--;
    }
    src++;
    if (!*src)
    {
      numargs++;
      return;
    }
  }
}

void ProcessCommand(void)
{ byte i;

  memcpy(cmd2, cmd, 40);
  strupr((char *) cmd2);
  ParseCommand();

  i=0;
  while (i<NUMCMDS)
  {
    if (!strcmp(concmds[i], (char *) args[0])) break;
    i++;
  }
  ExecuteCommand(i);
  memcpy(lastcmds, lastcmds+40, 360);
  memcpy(lastcmds+360, cmd, 40);
  if (numcmds<10) numcmds++;
  cmdpos=0;
  memset(cmd, 0, 40);
  cmdlen=0;
}

void CommandInput(void)
{ byte c, len, cc;

  UpdateControls();
  if (!last_pressed) return;

  // Handle the Shift key

  if (key[SCAN_LSHIFT] || key[SCAN_RSHIFT])
  {
    c=key_shift_tbl[last_pressed];
  }
  else
  {
    c=key_ascii_tbl[last_pressed];
  }

  // Handle special cases first
  if (last_pressed == SCAN_PGUP)
  {
    if (startln) startln--;
    last_pressed=0;
    return;
  }

  if (last_pressed == SCAN_PGDN)
  {
    if (startln < 36) startln++;
    last_pressed=0;
    return;
  }

  if (last_pressed == SCAN_UP)
  {
    if (cmdpos<numcmds)
    {
      cmdpos++;
      memcpy(cmd, lastcmds+(400-(cmdpos*40)), 40);
      cmdlen=strlen((char *) cmd);
    }
    last_pressed=0;
    return;
  }

  if (last_pressed == SCAN_DOWN)
  {
    if (cmdpos)
    {
      cmdpos--;
      if (!cmdpos)
      {
        memset(cmd, 0, 40);
        cmdlen=0;
      }
      else
      {
        memcpy(cmd, lastcmds+(400-(cmdpos*40)), 40);
        cmdlen=strlen((char *) cmd);
      }
    }
    last_pressed=0;
    return;
  }

  if (!c)
  {
    last_pressed=0;
    return;
  }

  if (last_pressed == SCAN_BACKSP)
  {
    if (cmdlen)
    {
      cmdlen--;
      cmd[cmdlen]=0;
    }
    last_pressed=0;
    return;
  }

  if (last_pressed == SCAN_TAB)
  {
    last_pressed=0;
    len=strlen((char *) cmd);
    if (!len) return;
    for (cc=0; cc<NUMCMDS; cc++)
        if (!strncasecmp((char *) cmd, concmds[sortedcmds[cc]], len))
        {
          memcpy((char *) cmd, concmds[sortedcmds[cc]], strlen(concmds[sortedcmds[cc]])+1);
          strlwr((char *) cmd);
          cmdlen=strlen((char *) cmd);
          return;
        }
    return;
  }

  if (last_pressed == SCAN_ENTER)
  {
    ProcessCommand();
    last_pressed=0;
    return;
  }

  if (cmdlen<38)
  {
    cmd[cmdlen++]=c;
    cmd[cmdlen]=0;
  }

  last_pressed=0;
}

void ActivateConsole(void)
{
  if (!allowconsole && !consoleoverride) return;
  conlines=1;
  callback=ShowConsole;
  cswtime=systemtime+40;

  while (!key[SCAN_RQUOTA])
  {
    DrawConsole();
    ShowPage();
    CheckMessages();
    CommandInput();
  }

  callback=HideConsole;
  while (conlines > 1)
  {
    CheckMessages();
    DrawConsole();
    ShowPage();
  }
  conlines=0;
  key[SCAN_RQUOTA]=0;
  timer_count=0;
}
