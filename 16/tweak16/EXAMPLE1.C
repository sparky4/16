/*
	Example1.C version 1.5
	by Robert Schmidt of Ztiff Zox Softwear 1993

	Example of making use of a TWEAK file by loading it directly at
		run-time.  This program needs the file(s) created by TWEAK
		available at run-time.
*/

#include <dos.h>
#include <mem.h>
#include <alloc.h>
#include <conio.h>
#include "TwkUser.h"	/* declares the neccessary functions and types */

main()
	{
	/* Define the pointer that should point to the loaded array of
	   Registers, and the int to hold its size */

    RegisterPtr rarray;
	int rsize;

	int y, lastMode;

	cprintf("This example program loads a TWEAK register file at run-time\n\r");
	cprintf("and uses this to set Mode X (320x240x256).\n\rPress any key.");
	getch();

	/* Now use the loadRegArray function declared in TwkUser.h to load the
	   TWEAK file.  Memory is automatically allocated.  Note that the
	   second argument is the address of the pointer defined above!
	   The returned value is the number of Register elements loaded. */

    rsize = loadRegArray("320x240.256", &rarray);

	/* Save the number of the current BIOS mode, so we can restore it
	   later. */

	_AH = 0x0f;
	geninterrupt(0x10);
	lastMode = _AL;

	/* Set mode 13h, to make sure the EGA palette set is correct for a 256
	   color mode */

	_AX = 0x13;
	geninterrupt(0x10);

	/* rarray now points to the loaded array.  The second argument to
	   outRegArray is the number if Register elements in the array. */

    outRegArray(rarray, rsize);

	outpw(0x3c4, 0x0f02);			/* Enable all 4 planes */

	/* Fill the screen with a blend of pink and green lines, defining the
	   palette on the fly. */

	outp(0x3c8, 0);					/* start with color 0 */
	for (y = 0; y<240; y++)
		{
		outp(0x3c9, y>>2);			/* red component */
		outp(0x3c9, (256-y) >> 2);	/* green component */
		outp(0x3c9, y>>2);			/* blue component */
		memset((char*)MK_FP(0xa000,0) + y*80, y, 80);
		}

	/* The mode is now set, so wait for the user to press a key...
	   Nothing much interesting to look at, I'm afraid. */

    getch();

	/* Restore the saved mode number.  Borland's textmode() won't work, as the
	   C library still thinks we're in the mode it detected at startup.
	   The palette will be set to the BIOS mode's default. */

	_AX = lastMode;
	geninterrupt(0x10);

	/* Free the Register array allocated in loadRegArray.  *You* are
	   responsible for doing this! */

    free(rarray);

	return 0;
	}
