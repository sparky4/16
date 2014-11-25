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
#include "timer.h"
#include "vga.h"

char manufacturer;                     // pcx header
char version;
char encoding;
char bits_per_pixel;
short int xmin,ymin;
short int xmax,ymax;
short int hres;
short int vres;
char palette[48];
char reserved;
char color_planes;
short int bytes_per_line;
short int palette_type;
char filler[58];

unsigned short int width,depth;
unsigned short int bytes, i;
unsigned char c, run, ss=0;
unsigned int vidoffset, n=0;
FILE *pcxf;

void ReadPCXLine(unsigned char *dest)
{ int j;
  n=0;

  do {
  c=fgetc(pcxf) & 0xff;
    if ((c & 0xc0)==0xc0) {
       run=c & 0x3f;
       c=fgetc(pcxf);
       for (j=0; j<run; j++)
           dest[vidoffset+n+j]=c;
       n+=run; }
    else { dest[vidoffset+n]=c;
           n++; }
  } while (n<bytes);
  fread(strbuf, 1, bytes_per_line-width, pcxf);
}

void LoadPCXHeader(char *fname)
{
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
  width=xmax-xmin+1;
  depth=ymax-ymin+1;
  bytes=bytes_per_line;
}

void loadpcx(char *fname, char *dest)
{
  LoadPCXHeader(fname);

  for (i=0; i<depth; i++)
    { vidoffset=5648+(i*352);
      ReadPCXLine(dest); }

  fclose(pcxf);
}

void WritePCXLine(unsigned char *p)
{ int i;
  unsigned char byte,samect,repcode;

  i=0;
  do
  {   byte=p[i++];
      samect=1;
      while (samect<(unsigned) 63 && i<320 && byte==p[i])
      {
         samect++;
         i++;
      }
      if (samect>1 || (byte & 0xC0) != 0)
      {
         repcode=0xC0 | samect;
         fwrite(&repcode,1,1,pcxf);
      }
      fwrite(&byte,1,1,pcxf);
  } while (i<320);
}

void WritePalette()
{ char b;
  int i;

  for (i=0; i<768; i++)
      pal[i]=pal[i] << 2;

  b=12; fwrite(&b, 1, 1, pcxf);
  fwrite(pal, 1, 768, pcxf);

  for (i=0; i<768; i++)
      pal[i]=pal[i] >> 2;
}

void ScreenShot()
{ unsigned char b1;
  unsigned short int w1;
  char fnamestr[13];

  // Takes a snapshot of the current screen.

   dec_to_asciiz(ss,&fnamestr);
   b1=strlen(&fnamestr);
   fnamestr[b1++]='.';
   fnamestr[b1++]='P';
   fnamestr[b1++]='C';
   fnamestr[b1++]='X';
   fnamestr[b1++]=0;

   pcxf=fopen(&fnamestr,"wb");
   ss++;

// Write PCX header

   b1=10; fwrite(&b1, 1, 1, pcxf); // manufacturer always = 10
   b1=5; fwrite(&b1, 1, 1, pcxf);  // version = 3.0, >16 colors
   b1=1; fwrite(&b1, 1, 1, pcxf);  // encoding always = 1
   b1=8; fwrite(&b1, 1, 1, pcxf);  // 8 bits per pixel, for 256 colors
   w1=0; fwrite(&w1, 1, 2, pcxf);  // xmin = 0;
   w1=0; fwrite(&w1, 1, 2, pcxf);  // ymin = 0;
 w1=319; fwrite(&w1, 1, 2, pcxf);  // xmax = 319;
 w1=199; fwrite(&w1, 1, 2, pcxf);  // ymax = 199;
 w1=320; fwrite(&w1, 1, 2, pcxf);  // hres = 320;
 w1=200; fwrite(&w1, 1, 2, pcxf);  // vres = 200;

 fwrite(virscr,1,48,pcxf);     // 16-color palette data. Who knows what's
                               // actually in here. It doesn't matter since
                               // the 256-color palette is stored elsewhere.

 b1=0; fwrite(&b1, 1, 1, pcxf);   // reserved always = 0.
 b1=1; fwrite(&b1, 1, 1, pcxf);   // number of color planes. Just 1 for 8bit.
 w1=320; fwrite(&w1, 1, 2, pcxf); // number of bytes per line

 w1=0; fwrite(&w1, 1, 1, pcxf);
 fwrite(virscr, 1, 59, pcxf);          // filler

 for (w1=0; w1<200; w1++)
     WritePCXLine(screen+(w1*320));

 WritePalette();
 fclose(pcxf);
 timer_count=0;
}
