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

#define byte unsigned char
// ================================= Data ====================================

FILE *pack;
FILE *src;
char strbuf[2048],*filebuf;
int numfiles;
char headertag[]={ 'V','R','G','P','A','C','K',0 };

struct filestruct
{
  unsigned char fname[84];            // pathname thingo
  int size;                           // size of the file
  int packofs;                        // where the file can be found in PACK
  int count;                          // internal (engine-runtime) use
  char extractable;                   // can UNPACK .. unpack it?
  char override;                      // should the engine override it w/
                                      // local files?
};

struct filestruct filetbl[512];

// ================================= Code ====================================

void DecryptHeader()
{ byte lastvalue, precodebyte, *ptr;

  ptr=(byte *) filetbl;
  lastvalue=*ptr;
  ptr++;

  while (ptr < (byte *) (int) filetbl + (int) 5120)
  {
    precodebyte=*ptr;
    (*ptr)-=lastvalue;
    lastvalue=precodebyte;
    ptr++;
  }
}

void CheckPackHeader()
{ char a;

  fread(strbuf, 1, 7, pack);
  strbuf[8]=0;
  if (strcmp(strbuf,headertag))
  {
    printf("*error* Not a valid VRG packfile. \n");
    exit(-1);
  }
  fread(&a, 1, 1, pack);
  if (a!=1)
  {
    printf("*error* Incorrect VRG packfile version. \n");
    exit(-1);
  }
  fread(&numfiles, 1, 4, pack);
  fread(&filetbl, 100, numfiles, pack);
  DecryptHeader();
}

void DumpFiles()
{ int i;

  printf("UNPACK v.0.1 Copyright (C)1997 BJ Eirich\n");
  for (i=0; i<numfiles; i++)
  {
    filebuf=(char *) malloc(filetbl[i].size);
    fread(filebuf, 1, filetbl[i].size, pack);
    if (filetbl[i].extractable)
    {
      src=fopen(filetbl[i].fname, "wb");
      printf("File: %s \n",filetbl[i].fname);
      fwrite(filebuf, 1, filetbl[i].size, src);
      fclose(src);
    }
    else printf("File: %s unexctractable.\n",filetbl[i].fname);
    free(filebuf);
  }
}

int main(int argc, char *argv[])
{
  if (argc<2)
  {
    printf("UNPACK v.0.1 Copyright (C)1997 BJ Eirich \n");
    printf("USAGE: UNPACK <VRG packfile> \n");
    exit(-1);
  }

  if (!(pack=fopen(argv[1],"rb")))
  {
    printf("Unable to open file %s. \n",argv[1]);
    exit(-1);
  }

  CheckPackHeader();
  DumpFiles();

  fclose(pack);

  return 0;
}
