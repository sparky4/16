////////////////////////////////////////////////////
//
// XMS routines - uses the MENU.C & LIB.C routines for output
//
////////////////////////////////////////////////////
#include "ted5.h"
#pragma hdrstop


long TTLxms;
unsigned XMSavail;
void far *XMSdriver;

////////////////////////////////////////////////////
//
// CALL the XMSdriver with a function #
//
////////////////////////////////////////////////////
void CallXMS(char func)
{
 asm	mov	ah,[func]
 asm	call	[DWORD PTR XMSdriver]
}

////////////////////////////////////////////////////
//
// Initialize the XMS memory
//
////////////////////////////////////////////////////
void InitXMS(void)
{
 //
 // See if XMS driver is present...
 //
 asm	mov	ax,0x4300
 asm	int	0x2f
 asm	cmp	al,0x80		// installed?
 asm	je	Present

 ErrDialog("You poor bastard! I refuse\n"
	   "to work without Extended\n"
	   "Memory! Goodbye!"," I Suck ");
 Quit("No Extended Memory Driver found!");

 //
 // YES! We found an XMS driver! Now we
 // need to get the XMS driver's entry address...
 //
 Present:
 asm	mov	ax,0x4310
 asm	int	0x2f
 asm	mov	WORD PTR XMSdriver,bx
 asm	mov	WORD PTR XMSdriver+2,es

 XMSTotalFree();
}


////////////////////////////////////////////////////
//
// Report an XMS error, if any
//
////////////////////////////////////////////////////
void XMSerror(void)
{
 if (_AX==0)
   switch(_BL)
   {
    case 0x80: Quit("Function not implemented!");
    case 0x81: Quit("VDISK device driver was detected!");
    case 0x82: Quit("A20 error occurred!");
    case 0x8e: Quit("General driver error!");
    case 0x8f: Quit("Unrecoverable driver error!");

    case 0x90: Quit("High memory area does not exist!");
    case 0x91: Quit("High memory area already in use!");
    case 0x92: Quit("DX is less than /HMAMIN= parameter!");
    case 0x93: Quit("High memory area not allocated!");
    case 0x94: Quit("A20 line still enabled!");

    case 0xa0: Quit("Not enough Extended Memory available!");
    case 0xa1: Quit("Extended memory handles exhausted!");
    case 0xa2: Quit("Invalid handle!");
    case 0xa3: Quit("Invalid source handle!");
    case 0xa4: Quit("Invalid source offset!");
    case 0xa5: Quit("Invalid destination handle!");
    case 0xa6: Quit("Invalid destination offset!");
    case 0xa7: Quit("Invalid length!");
    case 0xa8: Quit("Invalid overlap in move request!");
    case 0xa9: Quit("Parity error detected!");
    case 0xaa: Quit("Block is not locked!");
    case 0xab: Quit("Block is locked!");
    case 0xac: Quit("Lock count overflowed!");
    case 0xad: Quit("Lock failed!");

    case 0xb0: Quit("Smaller UMB is available!");
    case 0xb1: Quit("No UMBs are available!");
    case 0xb2: Quit("Invalid UMB segment number!");

    default: Quit("Unknown XMS Memory Error! I'm totally stumped!");
   }
}

////////////////////////////////////////////////////
//
// Allocate XMS memory
//
////////////////////////////////////////////////////
int XMSAllocate(long size)
{
 #if 0
 static int allocnum=0;

 _MouseHide();
 sx=0;
 sy=1;
 print("XMS allocation #");
 printint(allocnum++);
 print(":");
 printint((1023L+size)/1024L);
 print("K");
 while(!keydown[0x1c]);
 clearkeys();
 _MouseShow();
 #endif


 _DX=(size+1023)/1024;
 CallXMS(9);
 XMSerror();
 asm push dx
 TTLxms=1024L*XMSTotalFree();
 asm pop dx
 return _DX;
}

////////////////////////////////////////////////////
//
// Free XMS memory
//
////////////////////////////////////////////////////
void XMSFreeMem(int handle)
{
 _DX=handle;
 CallXMS(10);
 XMSerror();
 TTLxms=1024L*XMSTotalFree();
}


////////////////////////////////////////////////////
//
// Return XMS memory available
//
////////////////////////////////////////////////////
unsigned XMSTotalFree(void)
{
 CallXMS(8);
 XMSerror();
 XMSavail=_DX;
 return XMSavail;
}

////////////////////////////////////////////////////
//
// Move XMS memory
//
////////////////////////////////////////////////////
void XMSmove(int srchandle,long srcoff,int desthandle,long destoff,long size)
{
 struct { long bsize;
	  int shandle;
	  long soff;
	  int dhandle;
	  long doff;
	} XMSparms;

 unsigned DSreg,SIreg;

 XMSparms.bsize=size+(size&1);
 XMSparms.shandle=srchandle;
 XMSparms.dhandle=desthandle;
 XMSparms.soff=srcoff;
 XMSparms.doff=destoff;

 DSreg=FP_SEG(&XMSparms);
 SIreg=FP_OFF(&XMSparms);

 asm	push	ds
 asm	push	si
 asm	mov	si,SIreg
 asm	mov	ds,DSreg

 CallXMS(11);

 asm	pop	si
 asm	pop	ds

 XMSerror();
}