#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src\lib\ems.c"
     /*
     Test function for the EMM routines
     */
     
     void main()
     {
          long emmhandle;
          long avail;
          char teststr[80];
          int i;
     
          if(!emmtest())
          {
               printf("Expanded memory is not present\n");
               exit(0);
          }
     
          if(!emmok())
          {
               printf("Expanded memory manager is not present\n");
               exit(0);
          }
     
          avail = emmavail();
          if (avail == -1)
          {
               printf("Expanded memory manager error\n");
               exit(0);
          }
          printf("There are %ld pages available\n",avail);
     
          /* Request 10 pages of expanded memory */
          if((emmhandle = emmalloc(avail)) < 0)
          {
               printf("Insufficient pages available\n");
               exit(0);
          }
     
          for (i = 0; i < avail; i++)
          {
               sprintf(teststr,"%02d This is a test string\n",i);
               emmmap(emmhandle,i,0);
               emmmove(0,teststr,strlen(teststr) + 1);
          }
     
          for (i = 0; i < avail; i++)
          {
               emmmap(emmhandle,i,0);
               emmget(0,teststr,strlen(teststr) + 1);
               printf("READING BLOCK %d: %s\n",i,teststr);
          }
     
          emmclose(emmhandle);
     }
