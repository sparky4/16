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
#include <strings.h>
#include <stdlib.h>

// ================================= Data ====================================

struct span
{
  int begin, end;                     // begin & end location of span
  int len, inverse;                   // lenth of span and inverse on/off
};

struct span spans[32];                // maximum of 32 spans.
char numspans;                        // number of active spans in PDF

unsigned char *transparencytbl;       // full transparency table (64k)
char outname[100];                    // output filename
char paldefname[100];                 // palette definition filename
char *strbuf;                         // generic string buffer

char spanidx[256];                    // Color -> Span xlat table

// ================================= Code ====================================

void ParsePaletteDefinition(FILE *f)
{ int i;

  fscanf(f,"%s",strbuf);
  if (stricmp("gradients",strbuf))
  {
    printf("Not a valid PDF file. \n");
    exit(-1);
  }
  fscanf(f,"%s",strbuf);
  numspans=atoi(strbuf);

  for (i=0; i<numspans; i++)
  {
     spans[i].inverse=0;

     while (1)
     {
        fscanf(f,"%s",strbuf);
        if (!stricmp("starts",strbuf))
           { fscanf(f,"%s", strbuf);
             spans[i].begin=atoi(strbuf);
             continue; }
        if (!stricmp("ends",strbuf))
           { fscanf(f,"%s", strbuf);
             spans[i].end=atoi(strbuf);
             continue; }
       if (!stricmp("inverse",strbuf))
           { spans[i].inverse=1;
             continue; }
       if (!stricmp("endspan",strbuf))
           { spans[i].len=abs(spans[i].end-spans[i].begin);
             break; }
     }
  }
  fclose(f);
}

void GenerateSpanIndex()
{ int i,j;

  for (i=0; i<numspans; i++)
  {
    for (j=spans[i].begin; j<=spans[i].end; j++)
        spanidx[j]=i;
  }
}

unsigned char MixColors(int i, int j)
{ unsigned char in_i, in_j, in_m, f;

  in_i=i-spans[(int)spanidx[i]].begin;      // Get a intensity number from 0 to len
  in_i=in_i*63/spans[(int)spanidx[i]].len;  // Translate intensity to 0..63 range
  if (spans[(int)spanidx[i]].inverse)
     in_i=63-in_i;                     // If inverse, flip it.

  // Now, repeat for j.

  in_j=j-spans[(int)spanidx[j]].begin;      // Get a intensity number from 0 to len
  in_j=in_j*63/spans[(int)spanidx[j]].len;  // Translate intensity to 0..63 range
  if (spans[(int)spanidx[j]].inverse)
     in_j=63-in_j;                     // If inverse, flip it.

  in_m=(in_i+in_j)/2;                  // Average intensities.

  // now find final color.

  if (spans[(int)spanidx[i]].inverse)       // If dest span is inverse,
     in_m=63-in_m;                     // flip intensity_mixed
  in_m=in_m*spans[(int)spanidx[i]].len/63;  // Convert intensity to 0 .. len range

  f=spans[(int)spanidx[i]].begin+in_m;      // done!

  return f;
}

void WriteTable(FILE *f)
{ int i, j;

  transparencytbl=(char *) malloc(65536);
  memset(transparencytbl, 0, 65535);

  for (i=0; i<256; i++)
      for (j=0; j<256; j++)
        transparencytbl[(i*256)+j]=MixColors(i, j);

  fwrite(transparencytbl, 256, 256, f);
  fclose(f);
}

int main ()
{ FILE *f;

  strbuf=(char *) malloc(2000);
  printf("TRANS v.1.00 Transparency-table generator \n");
  printf("Copyright (C)1998 vecna \n\n");

  printf("Output table filename: ");
  fflush(stdout); gets(outname);
  printf("Palette definition filename: ");
  fflush(stdout); gets(paldefname);

  if (!(f=fopen(paldefname,"r")))
  {
    printf("Could not open palette definition file.\n");
    exit(-1);
  }

  ParsePaletteDefinition(f);
  GenerateSpanIndex();

  f=fopen(outname,"wb");
  WriteTable(f);

  return 0;
}
