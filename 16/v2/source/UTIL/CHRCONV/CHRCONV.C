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
#include <malloc.h>

char c[100];
FILE *f;
char *jalblkasd;
char byte, *t;
short word;
int quad;

int bufsize=0;
unsigned char *csrc, *cb;

void EmitC (unsigned char c)
{
  *csrc++=c;
  bufsize++;
}

void Compress(unsigned char *p, int len)
{
  int i;
  unsigned char byt,samect;
  unsigned char repcode;

  csrc=cb;
  i=0; bufsize=0;
  do
  {   byt=p[i++];
      samect=1;
      while (samect<254 && i<len && byt==p[i])
      {
         samect++;
         i++;
      }
      if (samect==2 && byt != 0xFF) { EmitC(byt); }
      if (samect==3 && byt != 0xFF) { EmitC(byt); EmitC(byt); }
      if (samect>3 || byt == 0xFF)
      {
         repcode=0xFF;
         EmitC(repcode);
         repcode=(char) samect;
         EmitC(repcode);
      }
      EmitC(byt);
  } while (i<len);
}

int main(int argc, char *argv[])
{
  int i;

  printf("CHRCONV v.1.00 Copyright (C)1998 BJ Eirich \n");
  if (argc==1)
  {
    printf("CHR filename: "); fflush(stdout);
    scanf("%s",c);
    f=fopen(c, "rb");
  }
  if (argc==2)
  {
    memcpy(c, argv[1], strlen(argv[1]));
    printf("Converting %s.\n",c);
    f=fopen(c, "rb");
  }
  if (!f)
  {
    printf("Invalid filename.\n");
    exit(-1);
  }
  jalblkasd=(char *) malloc(15360);
  fread(jalblkasd, 1, 15360, f);
  fclose(f);

  f=fopen(c,"wb");
  byte=2; fwrite(&byte, 1, 1, f);
  word=16; fwrite(&word, 1, 2, f);
  word=32; fwrite(&word, 1, 2, f);
  word=0;  fwrite(&word, 1, 2, f);
  word=16; fwrite(&word, 1, 2, f);
  word=16; fwrite(&word, 1, 2, f);
  word=16; fwrite(&word, 1, 2, f);
  word=30; fwrite(&word, 1, 2, f);

  cb=(char *) malloc(512*30);
  Compress(jalblkasd, 512*30);
  free(jalblkasd);
  fwrite(&bufsize, 1, 4, f);
  fwrite(cb, 1, bufsize, f);
  free(cb);

  quad=15; fwrite(&quad, 1, 4, f);
  quad=10; fwrite(&quad, 1, 4, f);
  quad=5; fwrite(&quad, 1, 4, f);
  quad=0; fwrite(&quad, 1, 4, f);

  t="F16W10F17W10F16W10F15W10F18W10F19W10F18W10F15W10";
  i=(strlen(t)+1); fwrite(&i, 1, 4, f); fwrite(t, 1, i, f);
  t="F11W10F12W10F11W10F10W10F13W10F14W10F13W10F10W10";
  i=(strlen(t)+1); fwrite(&i, 1, 4, f); fwrite(t, 1, i, f);
  t="F6W10F7W10F6W10F5W10F8W10F9W10F8W10F5W10";
  i=(strlen(t)+1); fwrite(&i, 1, 4, f); fwrite(t, 1, i, f);
  t="F1W10F2W10F1W10F0W10F3W10F4W10F3W10F0W10";
  i=(strlen(t)+1); fwrite(&i, 1, 4, f); fwrite(t, 1, i, f);

  fclose(f);

  return 0;
}
