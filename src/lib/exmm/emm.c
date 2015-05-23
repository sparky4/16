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
#include <conio.h>

#include "memory.h"
#include "../types.h"

#define PEMMDATAPAGENUM	4

void TransformData(char *pEmmData, unsigned int len)
{
	while(len)
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
	unsigned long advi;
	unsigned long i=0;
	char huge *p =(char huge *)(GetEMMSeg0()*0x10000);

	if(OpenEMM() != SUCCESS)
	{		// make sure we got EMM
		printf("EMM unavailable.\n");
		exit(1);
	}
	else
	{
		printf("Emm %x available\n", EMVer());
		printf("EMS pages available are %lu\n", EMMCoreLeft());
		//printf("EMM Hardware Information %lu\n", EMInfo());
	}

	printf("b4 EMS		pEmmData=%p\n", pEmmData);
	//printf("b4 EMS	*pEmmData=%s\n", *pEmmData);
	advi=EMMCoreLeft()*EMMPAGESIZE;
	printf("advi=%lu\n", advi);

	pEmmData = (byte *)EMMalloc(&hEData, EMMCoreLeft());  // get 6 * 16K bytes - 96K
	if(pEmmData == NULL/* ||  pEmmData0 == NULL*/)
	{
		printf("Not enough EMM or out of handles.\n");
		exit(2);
	}
	else
		printf("EMM allocate OK\n");

	printf("EMS pages available are %lu\n", EMMCoreLeft());



	printf("Map 1st 4 pages\n");
	MapEMM(hEData, 0, PEMMDATAPAGENUM);   // load 1st 4 pages into page frame: 0-3
	//memset(pEmmData, 0x0e, 64000u);
//0000	printf("(advi*EMMPAGESIZE)=%lu\n", advi);
	memset(pEmmData, atoi(argv[1]), 0xffff);//sizeof(atoi(argv[1])));//((EMMCoreLeft())*EMMPAGESIZE));
//qqqq	memset(pEmmData+0x10000, atoi(argv[1]), 0xffff);//sizeof(atoi(argv[1])));//((EMMCoreLeft())*EMMPAGESIZE));
//----	UnmapEMM(hEData, 0, 4);          // not absolutely necessary
	printf("%p= %c %c %c %c %c %c %c %c\n", pEmmData,
*pEmmData, *(pEmmData+1), *(pEmmData+2), *(pEmmData+3), *(pEmmData+4), *(pEmmData+5), *(pEmmData+6), *(pEmmData+7));

	/*printf("Map next 2 pages\n");
	MapEMM(hEData, 4, 2);            // map last 2 pages: 4-5
	memset(pEmmData, 0x04, 32768u);
//	memset(pEmmData, atoi(argv[0]), 32768u);
	printf("*pEmmData=%c\n", *pEmmData);
	printf(" pEmmData=%p\n", pEmmData);*/

   /*MapEMM(hEData, 0, 4);
   // do some stuff with the first 64K of file data.
   printf("Transform data\n");
   TransformData(pEmmData, 64000UL);
	printf("*pEmmData=%lu\n", *pEmmData);
   MapEMM(hEData, 4, 2);  // only unmaps 1st two pages of prior 64k mapping
   // do stuff with remaining 32K of data
   TransformData(pEmmData, 32768UL);
	printf("*pEmmData=%lu\n", *pEmmData);*/
	if(atoi(argv[2]))
	{
		while(p<(char huge *)((GetEMMSeg0()*0x10000)+0xffff) && !kbhit())
		{
			if((i%16)==0) printf("%p= %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c\n", p,
*p, *(p+1), *(p+2), *(p+3), *(p+4), *(p+5), *(p+6), *(p+7),
*(p+8), *(p+9), *(p+10), *(p+11), *(p+12), *(p+13), *(p+14), *(p+15));
	//		(*p)++;
			i++;
			p++;
		}
	}
	/*if(atoi(argv[3]))
		p =(char huge *)(GetEMMSeg0()*0x20000);
		while(p<(char huge *)((GetEMMSeg0()*0x20000)+0xffff) && !kbhit())
		{
			if((i%16)==0) printf("%p= %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c\n", p,
*p, *(p+1), *(p+2), *(p+3), *(p+4), *(p+5), *(p+6), *(p+7),
*(p+8), *(p+9), *(p+10), *(p+11), *(p+12), *(p+13), *(p+14), *(p+15));
	//		(*p)++;
			i++;
			p++;
		}
		//free(p);
	}*/
	if(!atoi(argv[3]))
	{
		UnmapEMM(hEData, 0, PEMMDATAPAGENUM);  // should unmap before freeing
		//printf("after EMS	*pEmmData=%c\n", *pEmmData);
		printf("Close emm\n");
		EMMFree(hEData);     // finished with the file data
		CloseEMM();
	}
	printf("after EMS	pEmmData=%p\n", pEmmData);
	printf("EMS pages available are %lu\n", EMMCoreLeft());
	printf("GetEMMSeg0=%p\n", GetEMMSeg0()*0x10000);
	//printf("EMMPAGESIZE=%d\n", EMMPAGESIZE);
	return 0;
}
