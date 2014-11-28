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

struct message
{
  char text[44];                     // message text
  int exptime;                       // message expire time
};

struct message messages[6];          // 5-message buffer
byte num_msgs=0;                     // number of active messages

// -- cpu usage --

int cputimer=0, frames=0;
char runprf[3];
// -- final numbers --
int fps=0;
char profile[3];

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

void RenderGUI()
{ int i;

  CheckMessageExpirations();
  for (i=0; i<num_msgs; i++)
  {
    GotoXY(1,1+(i*10));
    printstring(0,messages[i].text);
  }

  if (!cpu_watch) return;
  frames++;

  i=sx-76;
  GotoXY(i,sy-39);
  sprintf(strbuf,"etc:%d",profile[0]);
  printstring(0,strbuf);
  GotoXY(i,sy-29);
  sprintf(strbuf,"Render:%d",profile[1]);
  printstring(0,strbuf);
  GotoXY(i,sy-19);
  sprintf(strbuf,"PFlip:%d",profile[2]);
  printstring(0,strbuf);
  GotoXY(i,sy-9);
  sprintf(strbuf,"FPS:%d",fps);
  printstring(0,strbuf);
}

void CPUTick()
{
  cputimer++;
  runprf[cpubyte]++;
  if (cputimer==100)
  {
     fps=frames;
     frames=0;
     cputimer=0;

     profile[0]=runprf[0]; runprf[0]=0;
     profile[1]=runprf[1]; runprf[1]=0;
     profile[2]=runprf[2]; runprf[2]=0;
  }
}

void Message(char *text, int duration)
{
  Log("Message: %s", text);

  if (num_msgs<5)
  {
    memcpy(messages[num_msgs].text, text, strlen(text));
    messages[num_msgs].exptime=systemtime+duration;
    num_msgs++;
    return;
  }

  memcpy(&messages[0].text, &messages[1].text, 192);
  memcpy(messages[4].text, text, strlen(text));
  messages[num_msgs].exptime=systemtime+duration;
}
