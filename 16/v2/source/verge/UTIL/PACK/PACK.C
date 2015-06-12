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
#include <stdlib.h>
#include <string.h>

#define byte unsigned char

// ================================= Data ====================================

FILE *pack;
FILE *list;
FILE *src;
char *strbuf,*filebuf;
int numfiles,curofs;
char headertag[]="VRGPACK";

typedef struct
{
  unsigned char fname[84];            // pathname thingo
  int size;                           // size of the file
  int packofs;                        // where the file can be found in PACK
  int count;                          // internal (engine-runtime) use
  char extractable;                   // can UNPACK .. unpack it?
  char override;                      // should the engine override it w/
                                      // local files?
} filestruct;

filestruct filetbl[512];
byte *ptr;

// ================================= Code ====================================

void CreatePackHeader()
{ char a;

  fwrite(&headertag, 1, 7, pack);
  a=1; fwrite(&a, 1, 1, pack);              // pack version 1
  fwrite(&numfiles, 1, 4, pack);            // number of files in archive

  // Hmm.. that's it. What a boring header. :)
}

void EncryptHeader()
{ byte lastvalue; //, precodebyte;

  ptr=(byte *) filetbl;
  lastvalue=*ptr;
  ptr++;

  while (ptr < (byte *) (int) filetbl + (int) 5120)
  {
    (*ptr)+=lastvalue;
    lastvalue=*ptr;
    ptr++;
  }
}

void DecryptHeader()
{ byte lastvalue, precodebyte;

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

void BuildFileTable()
{ int i;

  curofs=12+(numfiles*100);
  memset(filetbl, 0, sizeof filetbl);

  for (i=0; i<numfiles; i++)
  {
    fscanf(list,"%s",strbuf);
    if (!(src=fopen(strbuf,"rb")))
    {
      printf("Could not open included file %s. \n",strbuf);
      exit(-1);
    }
    memcpy(&filetbl[i].fname,strbuf,strlen(strbuf));
    fseek(src,0,2);
    filetbl[i].packofs=curofs;
    filetbl[i].size=ftell(src);
    curofs+=filetbl[i].size;
    fscanf(list,"%s",strbuf); filetbl[i].extractable=atoi(strbuf);
    fscanf(list,"%s",strbuf); filetbl[i].override=atoi(strbuf);
    fclose(src);
  }
  EncryptHeader();
  fwrite(&filetbl, numfiles, 100, pack);
  DecryptHeader();
}

void DumpFiles()
{ int i;

  for (i=0; i<numfiles; i++)
  {
    filebuf=(char *) malloc(filetbl[i].size);
    src=fopen(filetbl[i].fname, "rb");
    printf("Packing file %s...\n", filetbl[i].fname);
    fread(filebuf, 1, filetbl[i].size, src);
    fwrite(filebuf, 1, filetbl[i].size, pack);
    fclose(src);
    free(filebuf);
  }
  printf("Done.\n");
}

int main(int argc, char *argv[])
{
  printf("PACK v.0.1 Copyright (C)1997 BJ Eirich \n");
  if (argc<2)
  {
    printf("USAGE: PACK <makefile> \n");
    exit(-1);
  }

  if (!(list=fopen(argv[1],"r")))
  {
    printf("Unable to open file %s. \n",argv[1]);
    exit(-1);
  }

  strbuf=(char *) malloc(100);
  fscanf(list,"%s",strbuf);

  if (!(pack=fopen(strbuf,"wb")))
  {
    printf("Unable to create file %s. \n",strbuf);
    exit(0);
  }
  fscanf(list,"%d",&numfiles);

  CreatePackHeader();
  BuildFileTable();
  DumpFiles();

  fclose(list);
  fclose(pack);

  return 0;
}
