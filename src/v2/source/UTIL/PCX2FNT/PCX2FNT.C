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

char fname1[100], fname2[100];
int xs, ys, ns;
FILE *fnt,*pcxf;
char *virscr;

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
unsigned char pal[768];

unsigned short int width,depth;
unsigned short int bytes;
unsigned char c, run, ss=0;
unsigned int vidoffset, n=0;

void ReadPCXLine(unsigned char *dest)
{
  int j;
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
}

void LoadPCXHeader()
{
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
  fread(&pal,1,768,pcxf);
  fseek(pcxf,128L,SEEK_SET);
  width=xmax-xmin+1;
  depth=ymax-ymin+1;
  bytes=bytes_per_line;
}

void dumpframe(int x, int y)
{ int i,j;
  unsigned char c;

  for (i=y; i<y+ys; i++)
   for (j=x; j<x+xs; j++)
     {
       c=virscr[(i*width)+j];
       fwrite(&c, 1, 1, fnt);
     }
}

int main()
{
  int i,j,z;

  printf("PCX2FNT v.3.00 Copyright (C)1998 BJ Eirich \n");
  printf("PCX filename: "); fflush(stdout);
  scanf("%s",fname1);
  pcxf=fopen(fname1,"rb");
  printf("FNT filename: "); fflush(stdout);
  scanf("%s",fname2);
  fnt=fopen(fname2, "wb");
  printf("Width of font: "); fflush(stdout);
  scanf("%d", &xs);
  printf("Height of font: "); fflush(stdout);
  scanf("%d", &ys);
  printf("Number of subsets: "); fflush(stdout);
  scanf("%d", &ns);

  version=1;
  fwrite(&version, 1, 1, fnt);
  fwrite(&xs, 1, 2, fnt);
  fwrite(&ys, 1, 2, fnt);
  fwrite(&ns, 1, 2, fnt);

  LoadPCXHeader();
  virscr=(char *) malloc(width*depth);
  for (i=0; i<depth; i++)
  {
    vidoffset=i*width;
    ReadPCXLine(virscr);
  }

  for (z=0; z<ns; z++)
    for (i=0; i<5; i++)
      for (j=0; j<20; j++)
        if ((i*20)+j<96) dumpframe((j*(xs+1))+1,(i*(ys+1))+(1+(ys+1)*(z*5)));

//  for (i=0; i<5; i++)
//   for (j=0; j<20; j++)
//      if ((i*20)+j<96) dumpframe((j*(xs+1))+1,(i*(ys+1))+(1+(ys+1)*5));

  fclose(pcxf);
  fclose(fnt);

  return 0;
}
