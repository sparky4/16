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

#include "vdriver.h"

#define xx 31

int fontx=0;
int fonty=0;

char sbA[]={3,
            00,xx,00,
            xx,00,xx,
            xx,xx,xx,
            xx,00,xx,
            xx,00,xx};

char ssA[]={3,
            00,00,00,
            00,xx,00,
            xx,00,xx,
            xx,xx,xx,
            xx,00,xx};

char sbB[]={3,
            xx,xx,00,
            xx,00,xx,
            xx,xx,00,
            xx,00,xx,
            xx,xx,00};

char ssB[]={3,
            00,00,00,
            xx,xx,00,
            xx,xx,00,
            xx,00,xx,
            xx,xx,xx};

char sbC[]={3,
            00,xx,xx,
            xx,00,00,
            xx,00,00,
            xx,00,00,
            00,xx,xx};

char ssC[]={3,
            00,00,00,
            00,xx,xx,
            xx,00,00,
            xx,00,00,
            00,xx,xx};

char sbD[]={3,
            xx,xx,00,
            xx,00,xx,
            xx,00,xx,
            xx,00,xx,
            xx,xx,00};

char ssD[]={3,
            00,00,00,
            xx,xx,00,
            xx,00,xx,
            xx,00,xx,
            xx,xx,00};

char sbE[]={3,
            xx,xx,xx,
            xx,00,00,
            xx,xx,00,
            xx,00,00,
            xx,xx,xx};

char ssE[]={3,
            00,00,00,
            xx,xx,xx,
            xx,xx,00,
            xx,00,00,
            xx,xx,xx};

char sbF[]={3,
            xx,xx,xx,
            xx,00,00,
            xx,xx,00,
            xx,00,00,
            xx,00,00};

char ssF[]={3,
            00,00,00,
            xx,xx,xx,
            xx,00,00,
            xx,xx,00,
            xx,00,00};

char sbG[]={3,
            00,xx,xx,
            xx,00,00,
            xx,00,xx,
            xx,00,xx,
            00,xx,xx};

char ssG[]={3,
            00,00,00,
            00,xx,xx,
            xx,00,00,
            xx,00,xx,
            00,xx,xx};

char sbH[]={3,
            xx,00,xx,
            xx,00,xx,
            xx,xx,xx,
            xx,00,xx,
            xx,00,xx};

char ssH[]={3,
            00,00,00,
            xx,00,xx,
            xx,00,xx,
            xx,xx,xx,
            xx,00,xx};

char sbI[]={1,
            xx,
            xx,
            xx,
            xx,
            xx};

char ssI[]={1,
            00,
            xx,
            xx,
            xx,
            xx};

char sbJ[]={3,
            00,00,xx,
            00,00,xx,
            00,00,xx,
            xx,00,xx,
            00,xx,00};

char ssJ[]={3,
            00,00,00,
            00,00,xx,
            00,00,xx,
            xx,00,xx,
            00,xx,00};

char sbK[]={3,
            xx,00,xx,
            xx,00,xx,
            xx,xx,00,
            xx,00,xx,
            xx,00,xx};

char ssK[]={3,
            00,00,00,
            xx,00,xx,
            xx,xx,00,
            xx,00,xx,
            xx,00,xx};

char sbL[]={3,
            xx,00,00,
            xx,00,00,
            xx,00,00,
            xx,00,00,
            xx,xx,xx};

char ssL[]={3,
            00,00,00,
            xx,00,00,
            xx,00,00,
            xx,00,00,
            xx,xx,xx};

char sbM[]={5,
            xx,00,00,00,xx,
            xx,xx,00,xx,xx,
            xx,00,xx,00,xx,
            xx,00,00,00,xx,
            xx,00,00,00,xx};

char ssM[]={5,
            00,00,00,00,00,
            xx,00,00,00,xx,
            xx,xx,00,xx,xx,
            xx,00,xx,00,xx,
            xx,00,00,00,xx};

char sbN[]={4,
            xx,00,00,xx,
            xx,xx,00,xx,
            xx,00,xx,xx,
            xx,00,00,xx,
            xx,00,00,xx};

char ssN[]={4,
            00,00,00,00,
            xx,00,00,xx,
            xx,xx,00,xx,
            xx,00,xx,xx,
            xx,00,00,xx};

char sbO[]={3,
            00,xx,00,
            xx,00,xx,
            xx,00,xx,
            xx,00,xx,
            00,xx,00};

char ssO[]={3,
            00,00,00,
            00,xx,00,
            xx,00,xx,
            xx,00,xx,
            00,xx,00};

char sbP[]={3,
            xx,xx,00,
            xx,00,xx,
            xx,xx,00,
            xx,00,00,
            xx,00,00};

char ssP[]={3,
            00,00,00,
            xx,xx,00,
            xx,00,xx,
            xx,xx,00,
            xx,00,00};

char sbQ[]={4,
            00,xx,xx,00,
            xx,00,00,xx,
            xx,00,00,xx,
            xx,00,xx,00,
            00,xx,00,xx};

char ssQ[]={3,
            00,00,00,
            xx,xx,00,
            xx,00,xx,
            xx,xx,00,
            00,xx,xx};

char sbR[]={3,
            xx,xx,00,
            xx,00,xx,
            xx,xx,00,
            xx,00,xx,
            xx,00,xx};

char ssR[]={3,
            00,00,00,
            xx,xx,00,
            xx,00,xx,
            xx,xx,00,
            xx,00,xx};

char sbS[]={3,
            00,xx,xx,
            xx,00,00,
            00,xx,00,
            00,00,xx,
            xx,xx,00};

char ssS[]={3,
            00,00,00,
            00,xx,xx,
            xx,00,00,
            00,xx,xx,
            xx,xx,00};

char sbT[]={3,
            xx,xx,xx,
            00,xx,00,
            00,xx,00,
            00,xx,00,
            00,xx,00};

char ssT[]={3,
            00,00,00,
            xx,xx,xx,
            00,xx,00,
            00,xx,00,
            00,xx,00};

char sbU[]={3,
            xx,00,xx,
            xx,00,xx,
            xx,00,xx,
            xx,00,xx,
            xx,xx,xx};

char ssU[]={3,
            00,00,00,
            xx,00,xx,
            xx,00,xx,
            xx,00,xx,
            xx,xx,xx};

char sbV[]={3,
            xx,00,xx,
            xx,00,xx,
            xx,00,xx,
            xx,00,xx,
            00,xx,00};

char ssV[]={3,
            00,00,00,
            xx,00,xx,
            xx,00,xx,
            xx,00,xx,
            00,xx,00};

char sbW[]={5,
            xx,00,00,00,xx,
            xx,00,00,00,xx,
            xx,00,xx,00,xx,
            xx,xx,00,xx,xx,
            xx,00,00,00,xx};

char ssW[]={5,
            00,00,00,00,00,
            xx,00,00,00,xx,
            xx,00,xx,00,xx,
            xx,xx,00,xx,xx,
            xx,00,00,00,xx};

char sbX[]={3,
            xx,00,xx,
            xx,00,xx,
            00,xx,00,
            xx,00,xx,
            xx,00,xx};

char ssX[]={3,
            00,00,00,
            xx,00,xx,
            00,xx,00,
            xx,00,xx,
            xx,00,xx};

char sbY[]={3,
            xx,00,xx,
            xx,00,xx,
            00,xx,00,
            00,xx,00,
            00,xx,00};

char ssY[]={3,
            00,00,00,
            xx,00,xx,
            xx,00,xx,
            00,xx,00,
            00,xx,00};

char sbZ[]={3,
            xx,xx,xx,
            00,00,xx,
            00,xx,00,
            xx,00,00,
            xx,xx,xx};

char ssZ[]={2,
            00,00,
            xx,xx,
            00,xx,
            xx,00,
            xx,xx};

char s1[]={1,
           xx,
           xx,
           xx,
           xx,
           xx};

char s2[]={3,
           xx,xx,xx,
           00,00,xx,
           xx,xx,xx,
           xx,00,00,
           xx,xx,xx};

char s3[]={3,
           xx,xx,xx,
           00,00,xx,
           00,xx,xx,
           00,00,xx,
           xx,xx,xx};

char s4[]={3,
           xx,00,xx,
           xx,00,xx,
           xx,xx,xx,
           00,00,xx,
           00,00,xx};

char s5[]={3,
           xx,xx,xx,
           xx,00,00,
           xx,xx,xx,
           00,00,xx,
           xx,xx,xx};

char s6[]={3,
           xx,xx,xx,
           xx,00,00,
           xx,xx,xx,
           xx,00,xx,
           xx,xx,xx};

char s7[]={3,
           xx,xx,xx,
           00,00,xx,
           00,00,xx,
           00,00,xx,
           00,00,xx};

char s8[]={3,
           xx,xx,xx,
           xx,00,xx,
           xx,xx,xx,
           xx,00,xx,
           xx,xx,xx};

char s9[]={3,
           xx,xx,xx,
           xx,00,xx,
           xx,xx,xx,
           00,00,xx,
           xx,xx,xx};

char s0[]={3,
           xx,xx,xx,
           xx,00,xx,
           xx,00,xx,
           xx,00,xx,
           xx,xx,xx};

char sQuote[]={3,
               xx,00,xx,
               xx,00,xx,
               00,00,00,
               00,00,00,
               00,00,00};

char sYow[]={1,
             xx,
             xx,
             xx,
             00,
             xx};

char sQuotes[]={1,
                xx,
                xx,
                00,
                00,
                00};


char sComma[]={2,
               00,00,
               00,00,
               00,00,
               00,xx,
               xx,00};
char sPeriod[]={1,
                00,
                00,
                00,
                00,
                xx};

char sMinus[]={2,
               00,00,
               00,00,
               xx,xx,
               00,00,
               00,00};

char sQuest[]={3,
               xx,xx,xx,
               00,00,xx,
               00,xx,xx,
               00,00,00,
               00,xx,00};

char sColon[]={1,
               00,
               xx,
               00,
               xx,
               00};

char sch[]={3,
            00,xx,00,
            xx,xx,xx,
            xx,xx,xx,
            xx,xx,xx,
            00,xx,00};

char usc[]={2,
            00,00,
            00,00,
            00,00,
            00,00,
            xx,xx};

char star[]={4,
             00,00,00,xx,
             xx,00,xx,00,
             00,xx,00,00,
             00,00,00,00,
             00,00,00,00};

char ss[]={2,
           xx,xx,
           xx,xx,
           xx,xx,
           xx,xx,
           xx,xx};

char sra[]={3,
            xx,00,00,
            xx,xx,00,
            xx,xx,xx,
            xx,xx,00,
            xx,00,00};

char slParen[]={2,
                00,xx,
                xx,00,
                xx,00,
                xx,00,
                00,xx};

char srParen[]={2,
                xx,00,
                00,xx,
                00,xx,
                00,xx,
                xx,00};

char ssemic[]={2,
               xx,xx,
               00,00,
               xx,xx,
               00,xx,
               xx,00};

char sSlash[]={3,
               00,00,xx,
               00,00,xx,
               00,xx,00,
               xx,00,00,
               xx,00,00};

char sBlank[]={2,
               00,00,
               00,00,
               00,00,
               00,00,
               00,00};

// ...
char mousepic[] = { xx,00,00,00,
                    xx,xx,00,00,
                    xx,xx,xx,00,
                    xx,xx,xx,xx,
                    00,00,xx,00,
                    00,00,xx,00};

char *smal_tbl[]=
{  sBlank,
   sYow,  sQuote,      ss,      ss,     ss,     ss, sQuotes, slParen,  // 40
 srParen,    star,      ss,  sComma, sMinus,sPeriod,  sSlash,      s0,  // 48
      s1,      s2,      s3,      s4,     s5,     s6,      s7,      s8,  // 56
      s9,  sColon,  ssemic,      ss,     ss,    sra,  sQuest,      ss,  // 64
     sbA,     sbB,     sbC,     sbD,    sbE,    sbF,     sbG,     sbH,  // 72
     sbI,     sbJ,     sbK,     sbL,    sbM,    sbN,     sbO,     sbP,  // 80
     sbQ,     sbR,     sbS,     sbT,    sbU,    sbV,     sbW,     sbX,  // 88
     sbY,     sbZ,      ss,      ss,     ss,     ss,     usc,     sch,  // 96
     ssA,     ssB,     ssC,     ssD,    ssE,    ssF,     ssG,     ssH,  // 102
     ssI,     ssJ,     ssK,     ssL,    ssM,    ssN,     ssO,     ssP,  // 110
     ssQ,     ssR,     ssS,     ssT,    ssU,    ssV,     ssW,     ssX,  // 118
     ssY,     ssZ };                                                    // 120

unsigned char lastcol=31;

void GotoXY(int x1, int y1)
{
  fontx=x1;
  fonty=y1;
}

void print_char(char c)
{ char* img;

  c-=32;
  if (c>90) c=2;
  img=smal_tbl[c];
  TCopySprite(fontx,fonty,img[0],5,img+1);
  fontx+=img[0]+1;
}

void printstring(char *str)
{
  for (; *str; ++str)
    print_char(*str);
}

int pixels(char *str)
{ int pix;

  for (pix=0; *str; ++str)
    pix += *smal_tbl[*str-32]+1;
  return pix;
}

void dec_to_asciiz(int num, char *buf)
{
  sprintf(buf,"%d",num);
}

void TextColor(unsigned char newc)
{ char *src;

  src=sbA;
  while ((int)src < (int) smal_tbl)
  {
    if (*src==lastcol) *src=newc;
    src++;
  }
}
