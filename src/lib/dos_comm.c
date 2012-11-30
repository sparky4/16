/* Thanks to Alex Russell for example code */
/* Thanks to Gary Neal for example code */
#include "src\lib\dos_comm.h"

// Q code
byte kee;
byte keer[128];	/* key pressed */
byte keep[128];	/* key released */

#ifdef __cplusplus		/* Functions must be declared C style */
extern "C" {
#endif
extern void interrupt (far *oldkb)(void) = NULL; /* BIOS keyboard handler */
#ifdef __cplusplus
}
#endif
/*****************NEW KEYBOARD 09h ISR***********************/
void interrupt newkb(void){
	register char qx;

	kee = inp(0x60);	/* Read the keyboard scan code */

	/* Clear keyboard controller on XT machines */
	qx = inp(0x61);
	qx |= 0x82;
	outp(0x61, qx);
	qx &= 0x7F;
	outp(0x61, qx);

	/* Interpret the scan code and set our flags */
	if(kee & 0x80)
		keep[kee & 0x7F] = 0;
	else
		keep[kee] = keer[kee] = 1;

	outp(0x20, 0x20);
}

/* ---------------------- init_keyboard() ---------------- April 17,1993 */
/* restore the bios keyboard handler */
/* ---------------------- deinit_keyboard() -------------- April 17,1993 */
void setkb(int vq){
	if(!vq){ // deinitiation
		/* Abort if our function pointer has no valid address */
		if(oldkb == NULL) return;
		/* Set address in our function pointer in interrupt vector table */
		_dos_setvect(9, oldkb);
		/* Reset our function pointer to contain no valid address */
		oldkb = NULL;
	}else if(vq == 1){ // initiation
		int i;	/* Index variable */
		byte far *lock_key;

		/* Abort if our function pointer has a valid address. */
		if(oldkb != NULL) return;

		/* Clear the keyboard buttons state arrays */
		for(i = 0; i < 128; i++)
			keep[i] = keer[i] = 0;

		/* save old BIOS key board handler */
		oldkb = _dos_getvect(9);

		// turn off num-lock via BIOS
		lock_key = MK_FP(0x040, 0x017);	//wtf is going on here?
		*lock_key&=(~(16 | 32 | 64));	// toggle off the lock keys
		oldkb();	// call BIOS keyhandler to change keyboard lights

		/* setup our own handler */
		_dos_setvect(9, newkb);
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * keyp                                                              *
 *                                                                         *
 * Returns the status of the key requested.                                *
 * The status is 1 if the key is pressed or has been pressed since the     *
 * last call to this function for that particular key.                     *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
char keyp(byte c){
	register char retVal;

	/* Key value in range of keyboard keys available */
	c &= 0x7F;

	/* Get the status of the key requested */
	retVal = keep[c] | keer[c];

	/* Reset the was pressed status for the requested key */
	keer[c] = 0;

	/* Return the requested key's state */
	return retVal;
}

// tesuto
byte scankey(){
	//if(keyp(kee)) printf("%c %03d %03x\n", kee, kee, kee);
	return kee;
}
