/*
	Example2.C version 1.0
	by Robert Schmidt of Ztiff Zox Softwear 1993

	Example of making use of a TWEAK file by linking its contents in
	with the program.  This program doesn't need any external files at
	run time.
*/

#include <mem.h>
#include <dos.h>
#include <alloc.h>
#include <conio.h>
#include "TwkUser.h"

/*	The following included file defines the Mode320x240 table of Registers.
	The file should be created by running
	TWEAK2C 320x240.256 320x240.c Mode320x240 */

#include "320x240.c"

main()
	{
	int y, lastMode;

	cprintf("This example program was compiled with the C version of a TWEAK\n\r");
	cprintf("register file, and so will not need the external file.\n\r");
	cprintf("The C array is now used to set Mode X (320x240x256).\n\rPress any key.");
	getch();

	/* Save the number of the current BIOS mode, so we can restore it
	   later. */

	_AH = 0x0f;
	geninterrupt(0x10);
	lastMode = _AL;

	/* Set mode 13h, to make sure the EGA palette set is correct for a 256
	   color mode */

	_AX = 0x13;
	geninterrupt(0x10);

	/* Note that no initialization is neccessary now.  The Register array
	   is linked in as global data, and is directly accessible.  Take
	   note of the way the number of Register elements in the array is
	   calculated: */

	outRegArray(Mode320x240, sizeof(Mode320x240)/sizeof(Register));

	outpw(0x3c4, 0x0f02);	/* Enable all 4 planes */

	/* Fill the screen with a blend of red and blue lines, defining the
	   palette on the fly. */

	outp(0x3c8, 0);					/* start with color 0 */
	for (y = 0; y<240; y++)
		{
		outp(0x3c9, y>>2);			/* red component */
		outp(0x3c9, 0);				/* green component */
		outp(0x3c9, (256-y) >> 2);	/* blue component */
		memset((char*)MK_FP(0xa000,0) + y*80, y, 80);
		}

	/* The picture is drawn, so wait for user to get tired of it. */

	getch();

	/* Restore the saved mode number.  Borland's textmode() won't work, as the
	   C library still thinks we're in the mode it detected at startup.
	   The palette will be set to the BIOS mode's default. */

	_AX = lastMode;
	geninterrupt(0x10);


	/* Also note that since the array is static, global data, there is
	   no pointer to free(), as was done in Example1.C */

	return 0;
	}
