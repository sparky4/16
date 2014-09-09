/* Thanks to Alex Russell for example code */
/* Thanks to Gary Neal for example code */
#include "dos_kb.h"

// keyboard buffer
static byte key[NUM_SCANCODES]; // pressed
static byte kea[NUM_SCANCODES]; // released

#ifdef __cplusplus		/* Function must be declared C style */
extern "C" {
#endif
static void interrupt (far *oldkb)(void) = NULL; /* BIOS keyboard handler */
#ifdef __cplusplus
}
#endif

/*
 * Comment out the following #define if you don't want the testing main()
 * to be included.
 */
//#define TESTING
#define TESTING2

/*****************NEW KEYBOARD 09h ISR***********************/
void interrupt newkb(void){
	byte kee;
	register char qx;

	kee = inp(0x60);	/* Read the keyboard scan code */

	/* Clear keyboard controller on XT machines */
	qx = inp(0x61);	   /* Get keyboard control register */
	qx |= 0x82;
	outp(0x61, qx);	   /* Toggle acknowledge bit high */
	qx &= 0x7F;
	outp(0x61, qx);	   /* Toggle acknowledge bit low */

	/* Interpret the scan code and set our flags */
	#ifdef TESTING2
	//printf("%d[%d]\n",kee,key[kee]);
	printf("\0"); // bug
	#endif
	if(kee & 0x80)
		key[kee & 0x7F] = 0; // a key is released
	else
		key[kee] = kea[kee] = 1; // a key is pressed

	/* Acknowledge the interrupt to the programmable interrupt controller */
	outp(0x20, 0x20);      /* Signal non specific end of interrupt */
}

/* ---------------------- init_keyboard() ---------------- April 17,1993 */
/* restore the bios keyboard handler */
/* ---------------------- deinit_keyboard() -------------- April 17,1993 */
void setkb(int vq){
	int i;	/* Index variable */
	if(!vq){ // deinitiation
		/* Abort if our function pointer has no valid address */
		if(oldkb == NULL) return;
		/* Set address in our function pointer in interrupt vector table */
		_dos_setvect(9, oldkb);
		/* Reset our function pointer to contain no valid address */
		oldkb = NULL;
		#ifdef TESTING
		/* Print the key heap */
		printf("\n");
		for(i=0; i<NUM_SCANCODES; i++){
			if(i==NUM_SCANCODES/2) printf("================================\n");
			printf("%03d[%d][%d]",i+1,key[i],kea[i]);
			if(key[i]==1)printf("====");
			printf(",\n");
		}
		#endif
	}else if(vq == 1){ // initiation
		byte far *lock_key;

		/* Abort if our function pointer has a valid address. */
		if(oldkb != NULL) return;

		/* Clear the keyboard buttons state arrays */
		for(i = 0; i < NUM_SCANCODES; i++)
			key[i] = kea[i] = 0;

		/* save old BIOS key board handler */
		oldkb = _dos_getvect(9);

		// turn off num-lock via BIOS
		lock_key = MK_FP(0x040, 0x017); // Pointing to the address of the bios shift state keys
		*lock_key&=(~(16 | 32 | 64)); // toggle off the locks by changing the values of the 4th, 5th, and 6th bits of the address byte of 0040:0017
		oldkb();	// call BIOS keyhandler to change keyboard lights

		/* setup our own handler */
		_dos_setvect(9, newkb);
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * keyp							      *
 *									 *
 * Returns the status of the key requested.				*
 * The status is 1 if the key is pressed or has been pressed since the     *
 * last call to this function for that particular key.		     *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int keyp(byte c){
	register char retVal;

	/* Key value in range of keyboard keys available */
	c &= 0x7F;

	/* Get the status of the key requested */
	retVal = key[c] | kea[c];

	/* Reset the was pressed status for the requested key */
	kea[c] = 0;

	/* Return the requested key's state */
	return retVal;
}


/*
 * The library testing routines follows below.
 */

#ifdef TESTING

/*
 * Library test (program) entry point.
 */

void main(void)
{
	byte q;
	setkb(1);
	while(!keyp(1))
	{
		keyp(q);
	}
	setkb(0);
}

#endif
