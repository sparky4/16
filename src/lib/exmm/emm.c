// Cut this out as emm.c

/*      File:       emm.c
 *      Module:     All Modules
 *      Author(s):  Chris Somers
 *      Date:       December 1, 1992
 *      Version:    V.1.1w

        minor mods by Alex Russell to simplify

        Must use memory model with FAR code

		Open Watcom patch by sparky4~

 */


#include <stdio.h>
#include <stdlib.h>
#include <mem.h>

#include "memory.h"

void TransformData(char *pEmmData, unsigned int len)
{
   while ( len )
      {
      (*pEmmData)++;
      pEmmData++;

      len--;
      }
}

int main(int argc, char *argv[])
{
	byte *pEmmData;
	int hEData;

	if(OpenEMM() != SUCCESS)
	{     // make sure we got EMM
		printf("EMM unavailable.\n");
		exit(1);
	}
	else
		{
			printf("Emm %x available\n", EMVer());
			printf("EMS pages available are %lu\n", EMMCoreLeft());
			//printf("EMM Hardware Information %lu\n", EMInfo());
		}

	printf("b4 EMS		*pEmmData=%x\n", *pEmmData);
	//printf("b4 EMS	*pEmmData=%s\n", *pEmmData);

	pEmmData = (byte *)EMMalloc(&hEData, EMMCoreLeft()-1);  // get 6 * 16K bytes - 96K
	if(pEmmData == NULL/* ||  pEmmData0 == NULL*/)
	{
		printf("Not enough EMM or out of handles.\n");
		exit(2);
	}
	else
		printf("EMM allocate OK\n");

	printf("EMS pages available are %lu\n", EMMCoreLeft());

	printf("Map 1st 4 pages\n");
	MapEMM(hEData, 0, 4);   // load 1st 4 pages into page frame: 0-3
	//memset(pEmmData, 0x0e, 64000u);
	memset(pEmmData, atoi(argv[1]), sizeof(atoi(argv[1])));//((EMMCoreLeft())*EMMPAGESIZE));
//----	UnmapEMM(hEData, 0, 4);          // not absolutely necessary
	printf("*pEmmData=%c\n", *pEmmData);

	printf("Map next 2 pages\n");
	MapEMM(hEData, 4, 2);            // map last 2 pages: 4-5
	memset(pEmmData, 0x04, 32768u);
//	memset(pEmmData, atoi(argv[0]), 32768u);
	printf("*pEmmData=%c\n", *pEmmData);
//	printf("*pEmmData++=%c\n", *(pEmmData++));

	/*MapEMM(hEData0, 0, 1);
	memset(pEmmData0, (short)4, sizeof(short));
	printf("*pEmmData0=%d\n", *pEmmData0);*/

   /*MapEMM(hEData, 0, 4);
   // do some stuff with the first 64K of file data.
   printf("Transform data\n");
   TransformData(pEmmData, 64000UL);
	printf("*pEmmData=%lu\n", *pEmmData);
   MapEMM(hEData, 4, 2);  // only unmaps 1st two pages of prior 64k mapping
   // do stuff with remaining 32K of data
   TransformData(pEmmData, 32768UL);
	printf("*pEmmData=%lu\n", *pEmmData);*/

	UnmapEMM(hEData, 0, EMMCoreLeft(/*4*/));  // should unmap before freeing
	//UnmapEMM(hEData0, 0, 1);  // should unmap before freeing
	//printf("after EMS	*pEmmData=%c\n", *pEmmData);

	printf("Close emm\n");
	EMMFree(hEData);     // finished with the file data
	CloseEMM();
	printf("after EMS	*pEmmData=%x\n", *pEmmData);
	printf("EMS pages available are %lu\n", EMMCoreLeft());
	//printf("EMMPAGESIZE=%d\n", EMMPAGESIZE);
	return 0;
}
