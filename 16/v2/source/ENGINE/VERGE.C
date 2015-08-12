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

#include <stdarg.h> // va_*

#include "verge.h"

// ================================= Data ====================================

// declared in VDRIVER.C
extern byte* translucency_table;

char *strbuf=0;                       // Universal temporary string buffer. :)
char joyflag=0;
int vidxres=0,vidyres=0;              // Joystick on/off | Video mode to use
char logoutput=0;                     // Verbose debugging startup mode
char nocdaudio=0;                     // do not use CD audio
char startmap[80];                    // startup map
FILE *logf=0;                         // logfile file handle

// ================================= Code ====================================

void InitializeDefaults()
{
  kb1=28;  kb2=56;  kb3=1;  kb4=57;    // default keyboard controls
  jb1=1;   jb2=2;   jb3=3;  jb4=4;     // default joystick controls
  joyflag=0;                           // joystick defaults to disabled
  vidxres=320; vidyres=200;
  logoutput=0;                         // Don't be annoyingly verbose
  strbuf=(char *) valloc(2000, "strbuf", OID_TEMP);
  memcpy(startmap,"test.map\0",9);

  md_mixfreq=44100;
  md_dmabufsize=2000;
  md_mode=DMODE_16BITS|DMODE_STEREO;
  md_device=0;
}

static VFILE *user_cfg_file=0;
static char parse_str[256];

static char *parse_cfg_token()
{
  vscanf(user_cfg_file, "%s", parse_str);
  return parse_str;
}

// compares string against parse_str (grabbed by parse_cfg_token)
// 0=mismatch, 1=match
static int parse_match(char *str)
{
  return !strcmp(parse_str, str);
}

void ParseStartupFiles()
{
  user_cfg_file = vopen("user.cfg");
  if (!user_cfg_file)
  {
    printf("Could not open user.cfg. \n");
    exit(-1);
  }

  while (1)
  {
    parse_cfg_token();

    // mounts a pack file; up to 3? (perhaps gaurd against more?)
    if (parse_match("mount"))
      { MountVFile(parse_cfg_token()); continue; }
    // set video resolution
    else if (parse_match("vidmode"))
    {
      vidxres = atoi(parse_cfg_token());
      vidyres = atoi(parse_cfg_token());

      // check validity
      if (vidxres != 256 && vidxres != 320
      &&  vidxres != 360 && vidxres != 640)
      {
        Log("user.cfg: invalid resolution width %i passed to vidmode",
          vidxres);
      }
      if (vidyres != 200 && vidyres != 240
      &&  vidyres != 256 && vidyres != 480)
      {
        Log("user.cfg: invalid resolution height %i passed to vidmode",
          vidyres);
      }

      continue;
    }
    // log to VERGE.LOG
    else if (parse_match("log"))
    {
      logoutput=1;
      continue;
    }
    // disable CD playing
    else if (parse_match("nocdaudio"))
    {
      nocdaudio=1;
      continue;
    }
    // map VERGE.EXE will run first when executed
    else if (parse_match("startmap"))
    {
      strcpy(startmap, parse_cfg_token());
      continue;
    }
    // 0=auto detect, 1=???, 2=???, 3=nosound
    else if (parse_match("sound_device"))
    {
      md_device = atoi(parse_cfg_token());
      continue;
    }
    // sound lib setting
    else if (parse_match("mixrate"))
    {
      md_mixfreq = atoi(parse_cfg_token());
      continue;
    }
    // sound lib setting
    else if (parse_match("dmabufsize"))
    {
      md_dmabufsize = atoi(parse_cfg_token());
      continue;
    }
    // sound lib setting
    else if (parse_match("force8bit"))
    {
      continue;
    }
    // sound lib setting
    else if (parse_match("forcemono"))
    {
      continue;
    }

    // unknown command, assume end
    break;
  }

  // done parsing config
  vclose(user_cfg_file);
}

void ParseAutoCFG()
{ VFILE *f;
  int i;

  if (!(f=vopen("auto.cfg"))) return;

  while (1)
  {
    vgets((char *) cmd, 40, f);
    for (i=0; i<(signed) strlen((char *) cmd); i++)
      if (cmd[i]==10 || cmd[i]==13) cmd[i]=0;
    if (strlen((char*) cmd)<2) break;
    ProcessCommand();
  }
  vclose(f);
}

void InitLog()
{
  if (logoutput)
  {
    remove("verge.log");
  }
}

void Log(char *message, ...)
{
  char tempbuf[256];
  va_list lst;

  if (!logoutput) return;

  va_start(lst, message);
  vsprintf(tempbuf, message, lst);
  va_end(lst);

  logf = fopen("verge.log", "aw");
  if (!logf) err("unable to log to verge.log");

  fprintf(logf, "%s \n", tempbuf);
  fflush(logf);

  fclose(logf);
}

// used in conjunction with LogDone()
void Logp(char *message, ...)
{
  char tempbuf[256];
  va_list lst;

  if (!logoutput) return;

  va_start(lst, message);
  vsprintf(tempbuf, message, lst);
  va_end(lst);

  logf = fopen("verge.log", "aw");
  if (!logf) err("unable to log to verge.log");

  fprintf(logf, "%s", tempbuf);
  fflush(logf);

  fclose(logf);
}

void LogDone()
{
  if (!logoutput) return;

  logf = fopen("verge.log", "aw");
  if (!logf) err("unable to log to verge.log");

  fprintf(logf, "... OK \n");
  fflush(logf);

  fclose(logf);
}

void InitSystems()
{
  InitLog();
  Log("V2 startup. Logfile initialized.");
  Logp("Sys: Initializing keyboard handler.");
  InitKeyboard();
  LogDone();
#ifdef __DJGPP__
  keyboard_chain(0);
#endif
  Logp("Sys: Initializing timer. Set 100hz.");
  InitTimer();
  LogDone();

  /*
  Logp("Sys: Initializing CD Audio.");
  CD_Init();
  LogDone();
  */

  Logp("Sys: Initializing music system.");
  InitMusicSystem();
  LogDone();

  InitVideo(vidxres, vidyres);
}

void LoadTransTable()
{
  FILE *fp=0;

  if (translucency_table)
    { free(translucency_table); translucency_table=0; }
  translucency_table=(byte *)valloc(65536, "translucency_table", OID_MISC);

  fp = fopen("trans.tbl", "rb");
  if (!fp) err("Could not open trans.tbl.");

  fread(translucency_table, 65536, 1, fp);

  fclose(fp);
}

void vmain(int argc, char *argv[])
{
  char temp[256];

  InitializeDefaults();
  ParseStartupFiles();
  InitSystems();
  if (argc==2)
  {
    memcpy(startmap, argv[1], strlen(argv[1]));
  }

  InitConsole();
  sprintf(temp,"VERGE System Version %s", VERSION);
  Con_Printf(temp);
  Con_Printf("Copyright (C)1998 vecna");

  ParseAutoCFG();
  LoadTransTable();
  LoadSystemVC();
  LoadMAP(startmap);

  while (1)
  {
    while (timer_count!=0)
    {
       timer_count--;
       GameTick();
    }
    Render();

    ShowPage();
    if (kill)
    {
      FreeVSP();
      FreeMAP();
      FreeCHRList();
      vcsp=vcstack;
      kill=0;
      LoadMAP(startmap);
    }
  }
}
