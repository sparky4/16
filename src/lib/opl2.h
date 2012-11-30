#ifndef _DOSLIB_SND_H_
#define _DOSLIB_SND_H_
/* C Header File: ADLIB *****************************************************

Author:   Kevin A. Lee

Last Amended: 27th April, 1993

Description:    Low-level interface to the Adlib (or compatible)
    FM sound card. All information gleaned from
    Jeffrey S. Lee's "Programming the Adlib/Sound
    Blaster FM Music Chips". See Lee's document for
    further information.

Compiled succesfully under Turbo C, Borland C++,
and Microsoft Quick C (all latest versions).

****************************************************************************/
#include "src\lib\lib_com.h"

#define NUMVOICE 11
#define FACTOR  1193180
#define OCTAVE  4

#define MIN_REGISTER			0x01
#define MAX_REGISTER			0xF5
#define ADLIB_FM_ADDRESS	0x388	/* adlib address/status register */
#define ADLIB_FM_DATA		0x389	/* adlib data register           */

/*
static int percussiveMode = 0,currentBDContents = 0;
//static int opTable[18] = {0,1,2,3,4,5,8,9,10,11,12,13,16,17,18,19,20,21};
static int voiceModulator[11] = {0,1,2,6,7,8,12,16,14,17,13};
static int voiceCarrier[9] = {3,4,5,9,10,11,15,16,17};
*/

/*
* FM Instrument definition for .SBI files - SoundBlaster instrument
* - these are the important parts - we will skip the header, but since
*   I am not sure where it starts and ends so I have had to guess.
*   However it SEEMS! to work. Each array has two values, one for
*   each operator.
*/
typedef struct{
	byte SoundCharacteristic[2];    /* modulator frequency multiple...  */
	byte Level[2];                  /* modulator frequency level...     */
	byte AttackDecay[2];            /* modulator attack/decay...        */
	byte SustainRelease[2];         /* modulator sustain/release...     */
	byte WaveSelect[2];             /* output waveform distortion       */
	byte Feedback;                  /* feedback algorithm and strength  */
} FMInstrument;


/*
* Enumerated F-Numbers (in octave 4) for the chromatic scale.
*/
/*enum oct{
	B4 = 493.88,
	B4b = 466.16,
	A4 = 440,
	A4b = 415.3,
	G4 = 392,
	G4b = 369.99,
	F4 = 349.23,
	E4 = 329.63,
	E4b = 311.13,
	D4 = 293.66,
	D4b = 277.18,
	C4 = 261.63,
	A0 = 27.50
};*/
enum SCALE{
	B4  = 0x1EE,
	B4b = 0x1D2,
	A4  = 440,
	A4b = 0x19F,
	G4 = 0x188,
	G4b  = 0x172,
	F4  = 0x15D,
	E4  = 0x14A,
	E4b  = 0x137,
	D4  = 0x126,
	D4b = 0x115,
	C4  = 0x106,
	A0 = 0x7F2
};
unsigned short Notes[] = {
        19327 ,        /* C b            */
        18242 ,        /* C              */
        17218 ,        /* C #   ( D b )  */
        16252 ,        /* D              */
        15340 ,        /* D #   ( E b )  */
        14479 ,        /* E     ( F b )  */
        13666 ,        /* F     ( E # )  */
        12899 ,        /* F #   ( G b )  */
        12175 ,        /* G              */
        11492 ,        /* G #   ( A b )  */
        10847 ,        /* A              */
        10238 ,        /* A #   ( B b )  */
        9664 ,         /* B     ( C b )  */
        9121 ,         /* B #            */
        0
};

/* function prototyping */
void WriteFM(int reg, int value);
int ReadFM(void);
int AdlibExists(void);
void FMReset(/*int percusiveMode*/);	/* Resets the card, quiets all voices, sets the percussive mode state */
void FMKeyOff(int voice);typedef unsigned char byte;
void FMKeyOn(int voice, int freq, int octave);
void FMVoiceVolume(int voice, int vol);
void FMSetVoice(int voiceNum, FMInstrument *ins);
/* Remember: percussion instruments must be assigned only to the correct voice number. */
//void FMSetPercusiveMode(int state);
//int  LoadSBI(char filename[], FMInstrument *ins);
void fmtest(void);
int soundtest();
#endif/*_DOSLIB_SND_H_*/