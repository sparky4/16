/* Project 16 Source Code~
 * Copyright (C) 2012-2017 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
 *
 * This file is part of Project 16.
 *
 * Project 16 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Project 16 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 * write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

//
//	ID Engine
//	ID_SD.c - Sound Manager for Wolfenstein 3D
//	v1.2w
//	By Jason Blochowiak
//	Open Watcom port by sparky4
//

//
//	This module handles dealing with generating sound on the appropriate
//		hardware
//
//	Depends on: User Mgr (for parm checking)
//
//	Globals:
//		For User Mgr:
//			SoundSourcePresent - Sound Source thingie present?
//			SoundBlasterPresent - SoundBlaster card present?
//			AdLibPresent - AdLib card present?
//			SoundMode - What device is used for sound effects
//				(Use SM_SetSoundMode() to set)
//			MusicMode - What device is used for music
//				(Use SM_SetMusicMode() to set)
//			DigiMode - What device is used for digitized sound effects
//				(Use SM_SetDigiDevice() to set)
//
//		For Cache Mgr:
//			NeedsDigitized - load digitized sounds?
//			NeedsMusic - load music?
//

#pragma hdrstop		// Wierdo thing with MUSE

#include <dos.h>

#ifndef	_MUSE_      // Will be defined in ID_Types.h
#include "src/lib/16_sd.h"
#else
#include "src/lib/16_head.h"
#endif
#pragma	hdrstop
#pragma	warn	-pia

#define	SDL_SoundFinished()	{SoundNumber = SoundPriority = 0;}

// Macros for AdLib stuff
#define	selreg(n)	outportb(alFMAddr,n)
#define	writereg(n)	outportb(alFMData,n)
#define	readstat()	inportb(alFMStatus)

//	Imports from ID_SD_A.ASM
/*extern*/	void			SDL_SetDS(void);
/*extern*/	void interrupt	SDL_t0ExtremeAsmService(void);//,
//					SDL_t0FastAsmService(void),
//					SDL_t0SlowAsmService(void);
	void			SDL_IndicatePC(boolean on),
					SDL_DigitizedDoneInIRQ();

//	Global variables
	boolean		AdLibPresent,
				NeedsDigitized,NeedsMusic,
				SoundPositioned;
	SDMode		SoundMode;
	SMMode		MusicMode;
	SDSMode		DigiMode;
	dword	TimeCount;
	word		HackCount;
	word		*SoundTable;	// Really * _seg *SoundTable, but that don't work
	boolean		ssIsTandy;
	int			DigiMap[LASTSOUND];

//	Internal variables
static	boolean			SD_Started;
		boolean			nextsoundpos;
		word/*boolean_+*/			TimerDone;
		word			TimerVal,TimerDelay10,TimerDelay25,TimerDelay100;
		dword		TimerDivisor,TimerCount;
static	char			*ParmStrings[] =
						{
							"noal",
							nil
						};
static	void			(*SoundUserHook)(void);
		soundnames		SoundNumber,DigiNumber;
		word			SoundPriority,DigiPriority;
		int				LeftPosition,RightPosition;
		void interrupt	(*t0OldService)(void);
		long			LocalTime;
		word			TimerRate;

		word			NumDigi,DigiLeft,DigiPage;
		word			_seg *DigiList;
		word			DigiLastStart,DigiLastEnd;
		boolean			DigiPlaying;
static	boolean			DigiMissed,DigiLastSegment;
static	memptr			DigiNextAddr;
static	word			DigiNextLen;

//	PC Sound variables
		volatile byte	pcLastSample,far *pcSound;
		dword		pcLengthLeft;
		word			pcSoundLookup[255];

//	AdLib variables
		boolean			alNoCheck;
		byte			far *alSound;
		word			alBlock;
		dword		alLengthLeft;
		dword		alTimeCount;
		Instrument		alZeroInst;

// This table maps channel numbers to carrier and modulator op cells
static	byte			carriers[9] =  { 3, 4, 5,11,12,13,19,20,21},
						modifiers[9] = { 0, 1, 2, 8, 9,10,16,17,18},
// This table maps percussive voice numbers to op cells
						pcarriers[5] = {19,0xff,0xff,0xff,0xff},
						pmodifiers[5] = {16,17,18,20,21};

//	Sequencer variables
		boolean			sqActive;
static	word			alFXReg;
static	ActiveTrack		*tracks[sqMaxTracks];//,
//--						mytracks[sqMaxTracks];
//--static	word			sqMode,sqFadeStep;
		word			far *sqHack,far *sqHackPtr,sqHackLen,sqHackSeqLen;
		long			sqHackTime;

//	Internal routines
		void			SDL_DigitizedDone(void);

//Assembly functions
boolean		alNoIRQ;

int count_time=0;
int count_fx=0;
int extreme=0;

void SDL_turnOnPCSpeaker(word timerval)
{
	__asm {
		mov	bx,timerval
		mov	al,0b6h
		out	43h,al
		mov	al,bl
		out	42h,al
		mov	al,bh
		out	42h,al
		in	al,61h
		or	al,3
		out	61h,al
	}
}

void SDL_turnOffPCSpeaker()
{
	__asm {
		in	al,61h
		and	al,0fch
		out	61h,al
	}
}

void SDL_setPCSpeaker(byte val)
{
	__asm {
		mov	al,val
		in	al,61h
		and	al,0fch
		or	al,ah
		out	61h,al
	}
}

void SDL_DoFX()
{
	if(pcSound)
	{
		if(*pcSound!=pcLastSample)
		{
			pcLastSample=*pcSound;

			if(pcLastSample)
				SDL_turnOnPCSpeaker(pcLastSample*60);
			else
				SDL_turnOffPCSpeaker();
		}
		pcSound++;
		pcLengthLeft--;
		if(!pcLengthLeft)
		{
			pcSound=0;
			SoundNumber=(soundnames)0;
			SoundPriority=0;
			SDL_turnOffPCSpeaker();
		}
	}

	if(alSound && !alNoIRQ)
	{
		if(*alSound)
		{
			alOut/*InIRQ*/(alFreqL,*alSound);
			alOut/*InIRQ*/(alFreqH,alBlock);
		}
		else alOut/*InIRQ*/(alFreqH,0);
		alSound++;
		alLengthLeft--;
		if(!alLengthLeft)
		{
			alSound=0;
			SoundNumber=(soundnames)0;
			SoundPriority=0;
			alOut/*InIRQ*/(alFreqH,0);
		}
	}

}

void SDL_DoFast()
{
	count_fx++;
	if(count_fx>=5)
	{
		count_fx=0;

		SDL_DoFX();

		count_time++;
		if(count_time>=2)
		{
			TimeCount++;
			count_time=0;
		}
	}

	if(sqActive && !alNoIRQ)
	{
		if(sqHackLen)
		{
			do
			{
				if(sqHackTime>alTimeCount) break;
				sqHackTime=alTimeCount+*(sqHackPtr+1);
				alOut/*InIRQ*/(*(byte *)sqHackPtr,*(((byte *)sqHackPtr)+1));
				sqHackPtr+=2;
				sqHackLen-=4;
			}
			while(sqHackLen);
		}
		alTimeCount++;
		if(!sqHackLen)
		{
			sqHackPtr=sqHack;
			sqHackLen=sqHackSeqLen;
			alTimeCount=0;
			sqHackTime=0;
		}
	}

//SS	if(ssSample)
//SS	{
//SS		if(!(inp(ssStatus)&0x40))
//SS		{
//SS			outp(ssData,*ssSample++);
//SS			outp(ssControl,ssOff);
//SS			__asm push ax
//SS			__asm pop ax
//SS			outp(ssControl,ssOn);
//SS			__asm push ax
//SS			__asm pop ax
//SS			ssLengthLeft--;
//SS			if(!ssLengthLeft)
//SS			{
//SS				ssSample=0;
//SS				SDL_DigitizedDoneInIRQ();
//SS			}
//SS		}
//SS	}

	TimerCount+=TimerDivisor;
	if(*((word *)&TimerCount+1))
	{
		*((word *)&TimerCount+1)=0;
		t0OldService();
	}
	else
	{
		outp(0x20,0x20);
	}
}

// Timer 0 ISR for 700Hz interrupts
void interrupt SDL_t0FastAsmService(void)
{
	SDL_DoFast();
}

// Timer 0 ISR for 140Hz interrupts
void interrupt SDL_t0SlowAsmService(void)
{
	count_time++;
	if(count_time>=2)
	{
		TimeCount++;
		count_time=0;
	}

	SDL_DoFX();

	TimerCount+=TimerDivisor;
	if(*((word *)&TimerCount+1))
	{
		*((word *)&TimerCount+1)=0;
		t0OldService();
	}
	else
		outp(0x20,0x20);
}


///////////////////////////////////////////////////////////////////////////
//
//	SDL_SetTimer0() - Sets system timer 0 to the specified speed
//
///////////////////////////////////////////////////////////////////////////
#pragma	argsused
static void
SDL_SetTimer0(word speed)
{
#ifndef TPROF	// If using Borland's profiling, don't screw with the timer
asm	pushf
asm	cli

	outportb(0x43,0x36);				// Change timer 0
	outportb(0x40,speed);
	outportb(0x40,speed >> 8);
	// Kludge to handle special case for digitized PC sounds
	if (TimerDivisor == (1192030 / (TickBase * 100)))
		TimerDivisor = (1192030 / (TickBase * 10));
	else
		TimerDivisor = speed;

asm	popf
#else
	TimerDivisor = 0x10000;
#endif
}

///////////////////////////////////////////////////////////////////////////
//
//	SDL_SetIntsPerSec() - Uses SDL_SetTimer0() to set the number of
//		interrupts generated by system timer 0 per second
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_SetIntsPerSec(word ints)
{
	TimerRate = ints;
	SDL_SetTimer0(1192030 / ints);
}

#ifndef SD_USECATA3DSETTIMERSPEED
static void
SDL_SetTimerSpeed(void)
{
	word	rate;
	void interrupt	(*isr)(void);

	if ((DigiMode == sds_PC) && DigiPlaying)
	{
		rate = TickBase * 100;
		isr = SDL_t0ExtremeAsmService;
	}
	else if
	(
		(MusicMode == smm_AdLib)
	||	((DigiMode == sds_SoundSource) && DigiPlaying)
	)
	{
		rate = TickBase * 10;
		isr = SDL_t0FastAsmService;
	}
	else
	{
		rate = TickBase * 2;
		isr = SDL_t0SlowAsmService;
	}

	if (rate != TimerRate)
	{
		setvect(8,isr);
		SDL_SetIntsPerSec(rate);
		TimerRate = rate;
	}
}
#else
static void
SDL_SetTimerSpeed(void)
{
	word	rate;

	if (MusicMode == smm_AdLib)
		rate = TickBase * 8;
	else
		rate = TickBase * 2;
	SDL_SetIntsPerSec(rate);
}
#endif

///////////////////////////////////////////////////////////////////////////
//
//	SDL_TimingService() - Used by SDL_InitDelay() to determine a timing
//		value for the current system that we're running on
//
///////////////////////////////////////////////////////////////////////////
//static void interrupt
void interrupt
SDL_TimingService(void)
{
	//TimerVal = _CX;
	__asm {
		mov TimerVal,cx
	}
	TimerDone = 1;

	outportb(0x20,0x20);				// Ack interrupt
}
#ifdef SD_USECATA3DSETTIMERSPEED
///////////////////////////////////////////////////////////////////////////
//
//	SDL_InitDelay() - Sets up TimerDelay's for SDL_Delay()
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_InitDelay(void)
{
	int		i;
	word	timer;

//++++	setvect(8,SDL_TimingService);		// Set to my timer 0 ISR

	SDL_SetIntsPerSec(1000);			// Time 1ms

	for (i = 0,timer = 0;i < 10;i++)	// Do timing test 10 times
	{
		__asm {
			xor		dx,dx					// Zero DX
			mov		cx,0xffff				// Put starting value in CX
			mov		[TimerDone],cx			// TimerDone = false - 1
#ifdef __BORLANDC__
		}
#endif
startloop:
#ifdef __BORLANDC__
		__asm {
#endif
			or		[TimerDone],0
			jnz		startloop				// Make sure we're at the start
#ifdef __BORLANDC__
		}
#endif
loop_:
#ifdef __BORLANDC__
		__asm {
#endif
			test	[TimerDone],1			// See if TimerDone flag got hit
			jnz		done					// Yep - drop out of the loop
			loop	loop_
#ifdef __BORLANDC__
		}
#endif
done:
#ifdef __WATCOMC__
	}
#endif

		if (0xffff - TimerVal > timer)
			timer = 0xffff - TimerVal;
	}
	timer += timer / 2;					// Use some slop
	TimerDelay10 =  timer / (1000 / 10);
	TimerDelay25 =  timer / (1000 / 25);
	TimerDelay100 = timer / (1000 / 100);

	SDL_SetTimer0(0);					// Reset timer 0

	setvect(8,t0OldService);			// Set back to old ISR
}
#endif
///////////////////////////////////////////////////////////////////////////
//
//	SDL_Delay() - Delays the specified amount of time
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_Delay(word delay)
{
	if (!delay)
		return;

	__asm {
		mov		cx,[delay]
#ifdef __BORLANDC__
	}
#endif
loop_:
#ifdef __BORLANDC__
	__asm {
#endif
		test	[TimerDone],0	// Useless code - just for timing equivilency
		jnz		done
		loop	loop_
#ifdef __BORLANDC__
	}
#endif
done:
#ifdef __WATCOMC__
	}
#endif
}

//
//	PC Sound code
//

///////////////////////////////////////////////////////////////////////////
//
//	SDL_PCPlaySample() - Plays the specified sample on the PC speaker
//
///////////////////////////////////////////////////////////////////////////
#ifdef	_MUSE_
void
#else
static void
#endif
SDL_PCPlaySample(byte huge *data,dword len)
{
asm	pushf
asm	cli

	SDL_IndicatePC(true);

	pcLengthLeft = len;
	pcSound = (volatile byte far *)data;

asm	popf
}

///////////////////////////////////////////////////////////////////////////
//
//	SDL_PCStopSample() - Stops a sample playing on the PC speaker
//
///////////////////////////////////////////////////////////////////////////
#ifdef	_MUSE_
void
#else
static void
#endif
SDL_PCStopSample(void)
{
asm	pushf
asm	cli

	/*(long)*/pcSound = 0;

	SDL_IndicatePC(false);

asm	in	al,0x61		  	// Turn the speaker off
asm	and	al,0xfd			// ~2
asm	out	0x61,al

asm	popf
}

///////////////////////////////////////////////////////////////////////////
//
//	SDL_PCPlaySound() - Plays the specified sound on the PC speaker
//
///////////////////////////////////////////////////////////////////////////
#ifdef	_MUSE_
void
#else
static void
#endif
SDL_PCPlaySound(PCSound far *sound)
{
asm	pushf
asm	cli

	pcLastSample = -1;
	pcLengthLeft = sound->common.length;
	pcSound = sound->data;

asm	popf
}

///////////////////////////////////////////////////////////////////////////
//
//	SDL_PCStopSound() - Stops the current sound playing on the PC Speaker
//
///////////////////////////////////////////////////////////////////////////
#ifdef	_MUSE_
void
#else
static void
#endif
SDL_PCStopSound(void)
{
asm	pushf
asm	cli

	/*(long)*/pcSound = 0;

asm	in	al,0x61		  	// Turn the speaker off
asm	and	al,0xfd			// ~2
asm	out	0x61,al

asm	popf
}

///////////////////////////////////////////////////////////////////////////
//
//	SDL_PCService() - Handles playing the next sample in a PC sound
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_PCService(void)
{
	byte	s;
	word	t;

	if (pcSound)
	{
		s = *pcSound++;
		if (s != pcLastSample)
		{
		asm	pushf
		asm	cli

			pcLastSample = s;
			if (s)					// We have a frequency!
			{
				t = pcSoundLookup[s];
			asm	mov	bx,[t]

			asm	mov	al,0xb6			// Write to channel 2 (speaker) timer
			asm	out	43h,al
			asm	mov	al,bl
			asm	out	42h,al			// Low byte
			asm	mov	al,bh
			asm	out	42h,al			// High byte

			asm	in	al,0x61			// Turn the speaker & gate on
			asm	or	al,3
			asm	out	0x61,al
			}
			else					// Time for some silence
			{
			asm	in	al,0x61		  	// Turn the speaker & gate off
			asm	and	al,0xfc			// ~3
			asm	out	0x61,al
			}

		asm	popf
		}

		if (!(--pcLengthLeft))
		{
			SDL_PCStopSound();
			SDL_SoundFinished();
		}
	}
}

///////////////////////////////////////////////////////////////////////////
//
//	SDL_ShutPC() - Turns off the pc speaker
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_ShutPC(void)
{
asm	pushf
asm	cli

	pcSound = 0;

asm	in	al,0x61		  	// Turn the speaker & gate off
asm	and	al,0xfc			// ~3
asm	out	0x61,al

asm	popf
}

//
//	Stuff for digitized sounds
//
memptr
SDL_LoadDigiSegment(word page, global_game_variables_t *gvar)
{
	memptr	addr;

#if 0	// for debugging
asm	mov	dx,STATUS_REGISTER_1
asm	in	al,dx
asm	mov	dx,ATR_INDEX
asm	mov	al,ATR_OVERSCAN
asm	out	dx,al
asm	mov	al,10	// bright green
asm	out	dx,al
#endif

	addr = PM_GetSoundPage(page);
	PM_SetPageLock(gvar->pm.fi.PMSoundStart + page,pml_Locked, gvar);

#if 0	// for debugging
asm	mov	dx,STATUS_REGISTER_1
asm	in	al,dx
asm	mov	dx,ATR_INDEX
asm	mov	al,ATR_OVERSCAN
asm	out	dx,al
asm	mov	al,3	// blue
asm	out	dx,al
asm	mov	al,0x20	// normal
asm	out	dx,al
#endif

	return(addr);
}

void
SDL_PlayDigiSegment(memptr addr,word len)
{
	switch (DigiMode)
	{
	case sds_PC:
    	SDL_PCPlaySample(addr,len);
		break;
//SS	case sds_SoundSource:
//SS		SDL_SSPlaySample(addr,len);
//SS		break;
//SB	case sds_SoundBlaster:
//SB		SDL_SBPlaySample(addr,len);
//SB		break;
	}
}

void
SD_StopDigitized(global_game_variables_t *gvar)
{
	int	i;

asm	pushf
asm	cli

	DigiLeft = 0;
	DigiNextAddr = nil;
	DigiNextLen = 0;
	DigiMissed = false;
	DigiPlaying = false;
	DigiNumber = DigiPriority = 0;
	SoundPositioned = false;
	if ((DigiMode == sds_PC) && (SoundMode == sdm_PC))
		SDL_SoundFinished();

	switch (DigiMode)
	{
	case sds_PC:
		SDL_PCStopSample();
		break;
//SS	case sds_SoundSource:
//SS		SDL_SSStopSample();
//SS		break;
//SB	case sds_SoundBlaster:
//SB		SDL_SBStopSample();
//SB		break;
	}

asm	popf

	for (i = DigiLastStart;i < DigiLastEnd;i++)
		PM_SetPageLock(i + gvar->pm.fi.PMSoundStart,pml_Unlocked, gvar);
	DigiLastStart = 1;
	DigiLastEnd = 0;
}

void
SD_Poll(global_game_variables_t *gvar)
{
	if (DigiLeft && !DigiNextAddr)
	{
		DigiNextLen = (DigiLeft >= PMPageSize)? PMPageSize : (DigiLeft % PMPageSize);
		DigiLeft -= DigiNextLen;
		if (!DigiLeft)
			DigiLastSegment = true;
		DigiNextAddr = SDL_LoadDigiSegment(DigiPage++, gvar);
	}
	if (DigiMissed && DigiNextAddr)
	{
		SDL_PlayDigiSegment(DigiNextAddr,DigiNextLen);
		DigiNextAddr = nil;
		DigiMissed = false;
		if (DigiLastSegment)
		{
			DigiPlaying = false;
			DigiLastSegment = false;
		}
	}
	SDL_SetTimerSpeed();
}

void
SD_SetPosition(int leftpos,int rightpos, global_game_variables_t *gvar)
{
	if
	(
		(leftpos < 0)
	||	(leftpos > 15)
	||	(rightpos < 0)
	||	(rightpos > 15)
	||	((leftpos == 15) && (rightpos == 15))
	)
		Quit(gvar, "SD_SetPosition: Illegal position");

	switch (DigiMode)
	{
//SB	case sds_SoundBlaster:
//SB		SDL_PositionSBP(leftpos,rightpos);
//SB		break;
	}
}

void
SD_PlayDigitized(word which,int leftpos,int rightpos, global_game_variables_t *gvar)
{
	word	len;
	memptr	addr;

	if (!DigiMode)
		return;

	SD_StopDigitized(gvar);
	if (which >= NumDigi)
		Quit(gvar, "SD_PlayDigitized: bad sound number");

	SD_SetPosition(leftpos,rightpos, gvar);

	DigiPage = DigiList[(which * 2) + 0];
	DigiLeft = DigiList[(which * 2) + 1];

	DigiLastStart = DigiPage;
	DigiLastEnd = DigiPage + ((DigiLeft + (PMPageSize - 1)) / PMPageSize);

	len = (DigiLeft >= PMPageSize)? PMPageSize : (DigiLeft % PMPageSize);
	addr = SDL_LoadDigiSegment(DigiPage++, gvar);

	DigiPlaying = true;
	DigiLastSegment = false;

	SDL_PlayDigiSegment(addr,len);
	DigiLeft -= len;
	if (!DigiLeft)
		DigiLastSegment = true;

	SD_Poll(gvar);
}

void
SDL_DigitizedDone(void)
{
	if (DigiNextAddr)
	{
		SDL_PlayDigiSegment(DigiNextAddr,DigiNextLen);
		DigiNextAddr = nil;
		DigiMissed = false;
	}
	else
	{
		if (DigiLastSegment)
		{
			DigiPlaying = false;
			DigiLastSegment = false;
			if ((DigiMode == sds_PC) && (SoundMode == sdm_PC))
			{
				SDL_SoundFinished();
			}
			else
				DigiNumber = DigiPriority = 0;
			SoundPositioned = false;
		}
		else
			DigiMissed = true;
	}
}

void
SD_SetDigiDevice(SDSMode mode, global_game_variables_t *gvar)
{
	boolean	devicenotpresent;

	if (mode == DigiMode)
		return;

	SD_StopDigitized(gvar);

	devicenotpresent = false;
//SBSS	switch (mode)
//SBSS	{
//SB	case sds_SoundBlaster:
//SB		if (!SoundBlasterPresent)
//SB		{
//SB			if (SoundSourcePresent)
//SB				mode = sds_SoundSource;
//SB			else
//SB				devicenotpresent = true;
//SB		}
//SB		break;
//SS	case sds_SoundSource:
//SS		if (!SoundSourcePresent)
//SS			devicenotpresent = true;
//SS		break;
//SBSS	}

	if (!devicenotpresent)
	{
//SS		if (DigiMode == sds_SoundSource)
//SS			SDL_ShutSS();

		DigiMode = mode;

//SS		if (mode == sds_SoundSource)
//SS			SDL_StartSS();

		SDL_SetTimerSpeed();
	}
}

void
SDL_SetupDigi(global_game_variables_t *gvar)
{
	memptr	list;
	word	far *p,
			pg;
	int		i;

	PM_UnlockMainMem(gvar);
	MM_GetPtr(&list,PMPageSize, gvar);
	PM_CheckMainMem(gvar);
	p = (word far *)MK_FP(PM_GetPage(gvar->pm.fi.ChunksInFile - 1, gvar),0);
	_fmemcpy((void far *)list,(void far *)p,PMPageSize);
	pg = gvar->pm.fi.PMSoundStart;
	for (i = 0;i < PMPageSize / (sizeof(word) * 2);i++,p += 2)
	{
		if (pg >= gvar->pm.fi.ChunksInFile - 1)
			break;
		pg += (p[1] + (PMPageSize - 1)) / PMPageSize;
	}
	PM_UnlockMainMem(gvar);
	MM_GetPtr(MEMPTRCONV DigiList,i * sizeof(word) * 2, gvar);
	_fmemcpy((void far *)DigiList,(void far *)list,i * sizeof(word) * 2);
	MM_FreePtr(&list, gvar);
	NumDigi = i;

	for (i = 0;i < LASTSOUND;i++)
		DigiMap[i] = -1;
}

// 	AdLib Code

///////////////////////////////////////////////////////////////////////////
//
//	alOut(n,b) - Puts b in AdLib card register n
//
///////////////////////////////////////////////////////////////////////////
void
alOut(byte n,byte b)
{
asm	pushf
asm	cli

asm	mov		dx,0x388
asm	mov		al,[n]
asm	out		dx,al
#if 0
	SDL_Delay(TimerDelay10);
#else
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
#endif

asm	mov		dx,0x389
asm	mov		al,[b]
asm	out		dx,al

asm	popf

#if 0
	SDL_Delay(TimerDelay25);
#else
asm	mov	dx,0x388
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx

asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx

asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx

asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
#endif
}

//#if 0
///////////////////////////////////////////////////////////////////////////
//
//	SDL_SetInstrument() - Puts an instrument into a generator
//
///////////////////////////////////////////////////////////////////////////
//static void
void
SDL_SetInstrument(int track,int which,Instrument far *inst,boolean percussive)
{
	byte		c,m;

	if (percussive)
	{
		c = pcarriers[which];
		m = pmodifiers[which];
	}
	else
	{
		c = carriers[which];
		m = modifiers[which];
	}

	tracks[track - 1]->inst = *inst;
	tracks[track - 1]->percussive = percussive;

	alOut(m + alChar,inst->mChar);
	alOut(m + alScale,inst->mScale);
	alOut(m + alAttack,inst->mAttack);
	alOut(m + alSus,inst->mSus);
	alOut(m + alWave,inst->mWave);

	// Most percussive instruments only use one cell
	if (c != 0xff)
	{
		alOut(c + alChar,inst->cChar);
		alOut(c + alScale,inst->cScale);
		alOut(c + alAttack,inst->cAttack);
		alOut(c + alSus,inst->cSus);
		alOut(c + alWave,inst->cWave);
	}

	alOut(which + alFeedCon,inst->nConn);	// DEBUG - I think this is right
}
//#endif

///////////////////////////////////////////////////////////////////////////
//
//	SDL_ALStopSound() - Turns off any sound effects playing through the
//		AdLib card
//
///////////////////////////////////////////////////////////////////////////
#ifdef	_MUSE_
void
#else
static void
#endif
SDL_ALStopSound(void)
{
asm	pushf
asm	cli

	/*(long)*/alSound = 0;
	alOut(alFreqH + 0,0);

asm	popf
}

static void
SDL_AlSetFXInst(Instrument far *inst)
{
	byte		c,m;

	m = modifiers[0];
	c = carriers[0];
	alOut(m + alChar,inst->mChar);
	alOut(m + alScale,inst->mScale);
	alOut(m + alAttack,inst->mAttack);
	alOut(m + alSus,inst->mSus);
	alOut(m + alWave,inst->mWave);
	alOut(c + alChar,inst->cChar);
	alOut(c + alScale,inst->cScale);
	alOut(c + alAttack,inst->cAttack);
	alOut(c + alSus,inst->cSus);
	alOut(c + alWave,inst->cWave);

	// Note: Switch commenting on these lines for old MUSE compatibility
//	alOut(alFeedCon,inst->nConn);
	alOut(alFeedCon,0);
}

///////////////////////////////////////////////////////////////////////////
//
//	SDL_ALPlaySound() - Plays the specified sound on the AdLib card
//
///////////////////////////////////////////////////////////////////////////
#ifdef	_MUSE_
void
#else
static void
#endif
SDL_ALPlaySound(AdLibSound far *sound, global_game_variables_t *gvar)
{
	Instrument	__far *inst;
	byte		huge *data;

	SDL_ALStopSound();

asm	pushf
asm	cli

	alLengthLeft = sound->common.length;
	data = sound->data;
	data++;
	data--;
	alSound = (byte far *)data;
	alBlock = ((sound->block & 7) << 2) | 0x20;
	inst = &sound->inst;

	if (!(inst->mSus | inst->cSus))
	{
	asm	popf
		Quit(gvar, "SDL_ALPlaySound() - Bad instrument");
	}

	SDL_AlSetFXInst(&alZeroInst);	// DEBUG
	SDL_AlSetFXInst(inst);

asm	popf
}

//#if 0
///////////////////////////////////////////////////////////////////////////
//
// 	SDL_ALSoundService() - Plays the next sample out through the AdLib card
//
///////////////////////////////////////////////////////////////////////////
//static void
void
SDL_ALSoundService(void)
{
	byte	s;

	if (alSound)
	{
		s = *alSound++;
		if (!s)
			alOut(alFreqH + 0,0);
		else
		{
			alOut(alFreqL + 0,s);
			alOut(alFreqH + 0,alBlock);
		}

		if (!(--alLengthLeft))
		{
			/*(long)*/alSound = 0;
			alOut(alFreqH + 0,0);
			SDL_SoundFinished();
		}
	}
}
//#endif

#if 0
///////////////////////////////////////////////////////////////////////////
//
//	SDL_SelectMeasure() - sets up sequencing variables for a given track
//
///////////////////////////////////////////////////////////////////////////
void
SDL_SelectMeasure(ActiveTrack *track)
{
	track->seq = track->moods[track->mood];
	track->nextevent = 0;
}
#endif

//#if 0
void
SDL_ALService(void)
{
	byte	a,v;
	word	w;

	a=v=0;

	if (!sqActive)
		return;

	while (sqHackLen && (sqHackTime <= alTimeCount))
	{
		w = *sqHackPtr++;
		sqHackTime = alTimeCount + *sqHackPtr++;
	asm	mov	dx,[w]
	asm	mov	[a],dl
	asm	mov	[v],dh
		alOut(a,v);
		sqHackLen -= 4;
	}
	alTimeCount++;
	if (!sqHackLen)
	{
		sqHackPtr = (word far *)sqHack;
		sqHackLen = sqHackSeqLen;
		alTimeCount = sqHackTime = 0;
	}
}
//#endif

///////////////////////////////////////////////////////////////////////////
//
//	SDL_ShutAL() - Shuts down the AdLib card for sound effects
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_ShutAL(void)
{
asm	pushf
asm	cli

	alOut(alEffects,0);
	alOut(alFreqH + 0,0);
	SDL_AlSetFXInst(&alZeroInst);
	alSound = 0;

asm	popf
}

///////////////////////////////////////////////////////////////////////////
//
//	SDL_CleanAL() - Totally shuts down the AdLib card
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_CleanAL(void)
{
	int	i;

asm	pushf
asm	cli

	alOut(alEffects,0);
	for (i = 1;i < 0xf5;i++)
		alOut(i,0);

asm	popf
}

///////////////////////////////////////////////////////////////////////////
//
//	SDL_StartAL() - Starts up the AdLib card for sound effects
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_StartAL(void)
{
	alFXReg = 0;
	alOut(alEffects,alFXReg);
	SDL_AlSetFXInst(&alZeroInst);
}

///////////////////////////////////////////////////////////////////////////
//
//	SDL_DetectAdLib() - Determines if there's an AdLib (or SoundBlaster
//		emulating an AdLib) present
//
///////////////////////////////////////////////////////////////////////////
static boolean
SDL_DetectAdLib(void)
{
	byte	status1,status2;
	int		i;

	alOut(4,0x60);	// Reset T1 & T2
	alOut(4,0x80);	// Reset IRQ
	status1 = readstat();
	alOut(2,0xff);	// Set timer 1
	alOut(4,0x21);	// Start timer 1
	SDL_Delay(TimerDelay100);

#if 0
	__asm {
		mov	dx,0x388
		mov	cx,100
#ifdef __BORLANDC__
	}
#endif
usecloop:
#ifdef __BORLANDC__
	__asm {
#endif
	in	al,dx
	loop usecloop
	}
#endif

	status2 = readstat();
	alOut(4,0x60);
	alOut(4,0x80);

	if (((status1 & 0xe0) == 0x00) && ((status2 & 0xe0) == 0xc0))
	{
		for (i = 1;i <= 0xf5;i++)	// Zero all the registers
			alOut(i,0);

		alOut(1,0x20);	// Set WSE=1
		alOut(8,0);		// Set CSM=0 & SEL=0

		return(true);
	}
	else
		return(false);
}

///////////////////////////////////////////////////////////////////////////
//
//	SDL_t0Service() - My timer 0 ISR which handles the different timings and
//		dispatches to whatever other routines are appropriate
//
///////////////////////////////////////////////////////////////////////////
//static void interrupt
void interrupt
SDL_t0Service(void)
{
static	word	count = 1;
	boolean myackflag = 0;

//00#if 0	// for debugging
asm	mov	dx,STATUS_REGISTER_1
asm	in	al,dx
asm	mov	dx,ATR_INDEX
asm	mov	al,ATR_OVERSCAN
asm	out	dx,al
asm	mov	al,4	// red
asm	out	dx,al
//00#endif

	HackCount++;

	if ((MusicMode == smm_AdLib) || (DigiMode == sds_SoundSource))
	{
		SDL_ALService();
//SS		SDL_SSService();
//		if (!(++count & 7))
		if (!(++count % 10))
		{
			LocalTime++;
			TimeCount++;
			if (SoundUserHook)
				SoundUserHook();
		}
//		if (!(count & 3))
		if (!(count % 5))
		{
			switch (SoundMode)
			{
			case sdm_PC:
				SDL_PCService();
				break;
			case sdm_AdLib:
				SDL_ALSoundService();
				break;
			}
		}
	}
	else
	{
		if (!(++count & 1))
		{
			LocalTime++;
			TimeCount++;
			if (SoundUserHook)
				SoundUserHook();
		}
		switch (SoundMode)
		{
		case sdm_PC:
			SDL_PCService();
			break;
		case sdm_AdLib:
			SDL_ALSoundService();
			break;
		}
	}

	__asm {
		mov	ax,[WORD PTR TimerCount]
		add	ax,[WORD PTR TimerDivisor]
		mov	[WORD PTR TimerCount],ax
		jnc	myack
		jmp end1
#ifdef __BORLANDC__
	}
#endif
myack:
#ifdef __BORLANDC__
	__asm {
#endif
		mov	myackflag,1
#ifdef __BORLANDC__
	}
#endif
end1:
#ifdef __WATCOMC__
	}
#endif
	if(!myackflag)
		t0OldService();			// If we overflow a word, time to call old int handler
	else
		outportb(0x20,0x20);	// Ack the interrupt

//00#if 0	// for debugging
asm	mov	dx,STATUS_REGISTER_1
asm	in	al,dx
asm	mov	dx,ATR_INDEX
asm	mov	al,ATR_OVERSCAN
asm	out	dx,al
asm	mov	al,3	// blue
asm	out	dx,al
asm	mov	al,0x20	// normal
asm	out	dx,al
//00#endif
}

////////////////////////////////////////////////////////////////////////////
//
//	SDL_ShutDevice() - turns off whatever device was being used for sound fx
//
////////////////////////////////////////////////////////////////////////////
static void
SDL_ShutDevice(void)
{
	switch (SoundMode)
	{
	case sdm_PC:
		SDL_ShutPC();
		break;
	case sdm_AdLib:
		SDL_ShutAL();
		break;
	}
	SoundMode = sdm_Off;
}

///////////////////////////////////////////////////////////////////////////
//
//	SDL_CleanDevice() - totally shuts down all sound devices
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_CleanDevice(void)
{
	if ((SoundMode == sdm_AdLib) || (MusicMode == smm_AdLib))
		SDL_CleanAL();
}

///////////////////////////////////////////////////////////////////////////
//
//	SDL_StartDevice() - turns on whatever device is to be used for sound fx
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_StartDevice(void)
{
	switch (SoundMode)
	{
	case sdm_AdLib:
		SDL_StartAL();
		break;
	}
	SoundNumber = SoundPriority = 0;
}
#if 0
static void
SDL_SetTimerSpeed(void)
{
	word	rate;

	if (MusicMode == smm_AdLib)
		rate = TickBase * 8;
	else
		rate = TickBase * 2;
	SDL_SetIntsPerSec(rate);
}
#endif
//	Public routines

///////////////////////////////////////////////////////////////////////////
//
//	SD_SetSoundMode() - Sets which sound hardware to use for sound effects
//
///////////////////////////////////////////////////////////////////////////
boolean
SD_SetSoundMode(SDMode mode, global_game_variables_t *gvar)
{
	boolean	result = false;
	word	tableoffset;

	SD_StopSound(gvar);

#ifndef	_MUSE_
	if ((mode == sdm_AdLib) && !AdLibPresent)
		mode = sdm_PC;

	switch (mode)
	{
	case sdm_Off:
		NeedsDigitized = false;
		result = true;
		break;
	case sdm_PC:
		tableoffset = STARTPCSOUNDS;
		NeedsDigitized = false;
		result = true;
		break;
	case sdm_AdLib:
		if (AdLibPresent)
		{
			tableoffset = STARTADLIBSOUNDS;
			NeedsDigitized = false;
			result = true;
		}
		break;
	}
#else
	result = true;
#endif

	if (result && (mode != SoundMode))
	{
		SDL_ShutDevice();
		SoundMode = mode;
#ifndef	_MUSE_
		SoundTable = (word *)(&gvar->ca.audiosegs[tableoffset]);
#endif
		SDL_StartDevice();
	}

	SDL_SetTimerSpeed();

	return(result);
}

///////////////////////////////////////////////////////////////////////////
//
//	SD_SetMusicMode() - sets the device to use for background music
//
///////////////////////////////////////////////////////////////////////////
boolean
SD_SetMusicMode(SMMode mode)
{
	boolean	result = false;

	SD_FadeOutMusic();
	while (SD_MusicPlaying())
		;

	switch (mode)
	{
	case smm_Off:
		NeedsMusic = false;
		result = true;
		break;
	case smm_AdLib:
		if (AdLibPresent)
		{
			NeedsMusic = true;
			result = true;
		}
		break;
	}

	if (result)
		MusicMode = mode;

	SDL_SetTimerSpeed();

	return(result);
}

///////////////////////////////////////////////////////////////////////////
//
//	SD_Startup() - starts up the Sound Mgr
//		Detects all additional sound hardware and installs my ISR
//
///////////////////////////////////////////////////////////////////////////
void
SD_Startup(global_game_variables_t *gvar)
{
	int	i;

	if (SD_Started)
		return;
#ifndef SD_USECATA3DSETTIMERSPEED
//	SDL_SetDS();
#endif
	ssIsTandy = false;
//SS	ssNoCheck = false;
	alNoCheck = false;
//SB	sbNoCheck = false;
//SB	sbNoProCheck = false;
#ifndef	_MUSE_
	for (i = 1;i < _argc;i++)
	{
		switch (US_CheckParm(_argv[i],ParmStrings))
		{
		case 0:						// No AdLib detection
			alNoCheck = true;
			break;
//SB		case 1:						// No SoundBlaster detection
//SB			sbNoCheck = true;
//SB			break;
//SB		case 2:						// No SoundBlaster Pro detection
//SB			sbNoProCheck = true;
//SB			break;
//SS		case 3:
//SS			ssNoCheck = true;		// No Sound Source detection
//SS			break;
		case 4:						// Tandy Sound Source handling
			ssIsTandy = true;
			break;
//SS		case 5:						// Sound Source present at LPT1
//SS			ssPort = 1;
//SS			ssNoCheck = SoundSourcePresent = true;
//SS			break;
//SS		case 6:                     // Sound Source present at LPT2
//SS			ssPort = 2;
//SS			ssNoCheck = SoundSourcePresent = true;
//SS			break;
//SS		case 7:                     // Sound Source present at LPT3
//SS			ssPort = 3;
//SS			ssNoCheck = SoundSourcePresent = true;
//SS			break;
		}
	}
#endif

	SoundUserHook = 0;

	t0OldService = getvect(8);	// Get old timer 0 ISR
#ifdef SD_USECATA3DSETTIMERSPEED
	SDL_InitDelay();			// SDL_InitDelay() uses t0OldService

	setvect(8,SDL_t0Service);	// Set to my timer 0 ISR
#endif
	LocalTime = TimeCount = alTimeCount = 0;

	SD_SetSoundMode(sdm_Off, gvar);
	SD_SetMusicMode(smm_Off);

//SS	if (!ssNoCheck)
//SS		SoundSourcePresent = SDL_DetectSoundSource();

	if (!alNoCheck)
	{
		AdLibPresent = SDL_DetectAdLib();
//SB		if (AdLibPresent) && !sbNoCheck)
//SB		{
//SB			int port = -1;
//SB			char *env = getenv("BLASTER");
//SB			if (env)
//SB			{
//SB				long temp;
//SB				while (*env)
//SB				{
//SB					while (isspace(*env))
//SB						env++;
//SB
//SB					switch (toupper(*env))
//SB					{
//SB					case 'A':
//SB						temp = strtol(env + 1,&env,16);
//SB						if
//SB						(
//SB							(temp >= 0x210)
//SB						&&	(temp <= 0x260)
//SB						&&	(!(temp & 0x00f))
//SB						)
//SB							port = (temp - 0x200) >> 4;
//SB						else
//SB							Quit(gvar, "SD_Startup: Unsupported address value in BLASTER");
//SB						break;
//SB					case 'I':
//SB						temp = strtol(env + 1,&env,10);
//SB						if
//SB						(
//SB							(temp >= 0)
//SB						&&	(temp <= 10)
//SB						&&	(sbIntVectors[temp] != -1)
//SB						)
//SB						{
//SB							sbInterrupt = temp;
//SB							sbIntVec = sbIntVectors[sbInterrupt];
//SB						}
//SB						else
//SB							Quit(gvar, "SD_Startup: Unsupported interrupt value in BLASTER");
//SB						break;
//SB					case 'D':
//SB						temp = strtol(env + 1,&env,10);
//SB						if ((temp == 0) || (temp == 1) || (temp == 3))
//SB							SDL_SBSetDMA(temp);
//SB						else
//SB							Quit(gvar, "SD_Startup: Unsupported DMA value in BLASTER");
//SB						break;
//SB					default:
//SB						while (isspace(*env))
//SB							env++;
//SB						while (*env && !isspace(*env))
//SB							env++;
//SB						break;
//SB					}
//SB				}
//SB			}
//SB			SoundBlasterPresent = SDL_DetectSoundBlaster(port);
//SB		}
	}

	for (i = 0;i < 255;i++)
		pcSoundLookup[i] = i * 60;

//SB	if (SoundBlasterPresent)
//SB		SDL_StartSB();

//++++	SDL_SetupDigi(gvar);

	SD_Started = true;
}

///////////////////////////////////////////////////////////////////////////
//
//	SD_Default() - Sets up the default behaviour for the Sound Mgr whether
//		the config file was present or not.
//
///////////////////////////////////////////////////////////////////////////
void
SD_Default(boolean gotit,SDMode sd,SMMode sm, global_game_variables_t *gvar)
{
	boolean	gotsd,gotsm;

	gotsd = gotsm = gotit;

	if (gotsd)	// Make sure requested sound hardware is available
	{
		switch (sd)
		{
		case sdm_AdLib:
			gotsd = AdLibPresent;
			break;
		}
	}
	if (!gotsd)
	{
		if (AdLibPresent)
			sd = sdm_AdLib;
		else
			sd = sdm_PC;
	}
	if (sd != SoundMode)
		SD_SetSoundMode(sd, gvar);


	if (gotsm)	// Make sure requested music hardware is available
	{
		switch (sm)
		{
		case sdm_AdLib:
			gotsm = AdLibPresent;
			break;
		}
	}
	if (!gotsm)
	{
		if (AdLibPresent)
			sm = smm_AdLib;
	}
	if (sm != MusicMode)
		SD_SetMusicMode(sm);
}

///////////////////////////////////////////////////////////////////////////
//
//	SD_Shutdown() - shuts down the Sound Mgr
//		Removes sound ISR and turns off whatever sound hardware was active
//
///////////////////////////////////////////////////////////////////////////
void
SD_Shutdown(global_game_variables_t *gvar)
{
	if (!SD_Started)
		return;

	SD_MusicOff();
	SD_StopSound(gvar);
	SDL_ShutDevice();
	SDL_CleanDevice();

//SB	if (SoundBlasterPresent)
//SB		SDL_ShutSB();

//SS	if (SoundSourcePresent)
//SS		SDL_ShutSS();

	asm	pushf
	asm	cli

	SDL_SetTimer0(0);

	setvect(8,t0OldService);

	asm	popf

	SD_Started = false;
}

///////////////////////////////////////////////////////////////////////////
//
//	SD_SetUserHook() - sets the routine that the Sound Mgr calls every 1/70th
//		of a second from its timer 0 ISR
//
///////////////////////////////////////////////////////////////////////////
void
SD_SetUserHook(void (* hook)(void))
{
	SoundUserHook = hook;
}

///////////////////////////////////////////////////////////////////////////
//
//	SD_PositionSound() - Sets up a stereo imaging location for the next
//		sound to be played. Each channel ranges from 0 to 15.
//
///////////////////////////////////////////////////////////////////////////
void
SD_PositionSound(int leftvol,int rightvol)
{
	LeftPosition = leftvol;
	RightPosition = rightvol;
	nextsoundpos = true;
}

///////////////////////////////////////////////////////////////////////////
//
//	SD_PlaySound() - plays the specified sound on the appropriate hardware
//
///////////////////////////////////////////////////////////////////////////
boolean
SD_PlaySound(soundnames sound, global_game_variables_t *gvar)
{
	boolean		ispos;
	SoundCommon	far *s;
	int	lp,rp;

	lp = LeftPosition;
	rp = RightPosition;
	LeftPosition = 0;
	RightPosition = 0;

	ispos = nextsoundpos;
	nextsoundpos = false;

	if (sound == -1)
		return(false);

	s = MK_FP(SoundTable[sound],0);
	if ((SoundMode != sdm_Off) && !s)
		Quit(gvar, "SD_PlaySound() - Uncached sound");

	if ((DigiMode != sds_Off) && (DigiMap[sound] != -1))
	{
		if ((DigiMode == sds_PC) && (SoundMode == sdm_PC))
		{
			if (s->priority < SoundPriority)
				return(false);

			SDL_PCStopSound();

			SD_PlayDigitized(DigiMap[sound],lp,rp, gvar);
			SoundPositioned = ispos;
			SoundNumber = sound;
			SoundPriority = s->priority;
		}
		else
		{
		asm	pushf
		asm	cli
			if (DigiPriority && !DigiNumber)
			{
			asm	popf
				Quit(gvar, "SD_PlaySound: Priority without a sound");
			}
		asm	popf

			if (s->priority < DigiPriority)
				return(false);

			SD_PlayDigitized(DigiMap[sound],lp,rp, gvar);
			SoundPositioned = ispos;
			DigiNumber = sound;
			DigiPriority = s->priority;
		}

		return(true);
	}

	if (SoundMode == sdm_Off)
		return(false);
	if (!s->length)
		Quit(gvar, "SD_PlaySound() - Zero length sound");
	if (s->priority < SoundPriority)
		return(false);

	switch (SoundMode)
	{
	case sdm_PC:
		SDL_PCPlaySound((void far *)s);
		break;
	case sdm_AdLib:
		SDL_ALPlaySound((void far *)s, gvar);
		break;
	}

	SoundNumber = sound;
	SoundPriority = s->priority;

	return(false);
}

///////////////////////////////////////////////////////////////////////////
//
//	SD_SoundPlaying() - returns the sound number that's playing, or 0 if
//		no sound is playing
//
///////////////////////////////////////////////////////////////////////////
word
SD_SoundPlaying(void)
{
	boolean	result = false;

	switch (SoundMode)
	{
	case sdm_PC:
		result = pcSound? true : false;
		break;
	case sdm_AdLib:
		result = alSound? true : false;
		break;
	}

	if (result)
		return(SoundNumber);
	else
		return(false);
}

///////////////////////////////////////////////////////////////////////////
//
//	SD_StopSound() - if a sound is playing, stops it
//
///////////////////////////////////////////////////////////////////////////
void
SD_StopSound(global_game_variables_t *gvar)
{
	if (DigiPlaying)
		SD_StopDigitized(gvar);

	switch (SoundMode)
	{
	case sdm_PC:
		SDL_PCStopSound();
		break;
	case sdm_AdLib:
		SDL_ALStopSound();
		break;
	}

	SoundPositioned = false;

	SDL_SoundFinished();
}

///////////////////////////////////////////////////////////////////////////
//
//	SD_WaitSoundDone() - waits until the current sound is done playing
//
///////////////////////////////////////////////////////////////////////////
void
SD_WaitSoundDone(void)
{
	while (SD_SoundPlaying())
		;
}

///////////////////////////////////////////////////////////////////////////
//
//	SD_MusicOn() - turns on the sequencer
//
///////////////////////////////////////////////////////////////////////////
void
SD_MusicOn(void)
{
	sqActive = true;
}

///////////////////////////////////////////////////////////////////////////
//
//	SD_MusicOff() - turns off the sequencer and any playing notes
//
///////////////////////////////////////////////////////////////////////////
void
SD_MusicOff(void)
{
	word	i;


	switch (MusicMode)
	{
	case smm_AdLib:
		alFXReg = 0;
		alOut(alEffects,0);
		for (i = 0;i < sqMaxTracks;i++)
			alOut(alFreqH + i + 1,0);
		break;
	}
	sqActive = false;
}

///////////////////////////////////////////////////////////////////////////
//
//	SD_StartMusic() - starts playing the music pointed to
//
///////////////////////////////////////////////////////////////////////////
void
SD_StartMusic(MusicGroup far *music)
{
	SD_MusicOff();
asm	pushf
asm	cli

	if (MusicMode == smm_AdLib)
	{
		sqHackPtr = sqHack = music->values;
		sqHackSeqLen = sqHackLen = music->length;
		sqHackTime = 0;
		alTimeCount = 0;
		SD_MusicOn();
	}

asm	popf
}

///////////////////////////////////////////////////////////////////////////
//
//	SD_FadeOutMusic() - starts fading out the music. Call SD_MusicPlaying()
//		to see if the fadeout is complete
//
///////////////////////////////////////////////////////////////////////////
void
SD_FadeOutMusic(void)
{
	switch (MusicMode)
	{
	case smm_AdLib:
		// DEBUG - quick hack to turn the music off
		SD_MusicOff();
		break;
	}
}

///////////////////////////////////////////////////////////////////////////
//
//	SD_MusicPlaying() - returns true if music is currently playing, false if
//		not
//
///////////////////////////////////////////////////////////////////////////
boolean
SD_MusicPlaying(void)
{
	boolean	result;

	switch (MusicMode)
	{
	case smm_AdLib:
		result = false;
		// DEBUG - not written
		break;
	default:
		result = false;
	}

	return(result);
}

//#if 0
//
// SD ASS!
//

//variables for these assembly functions
volatile boolean pcindicate;
volatile unsigned/*boolean*/ MyDS;

void SDL_SetDS()
{
	__asm {
		mov	ax,ds
		mov	[cs:MyDS],ds
		ret
	}
}

//

// Timer 0 ISR for 7000Hz interrupts
void interrupt SDL_t0ExtremeAsmService(void)
{
	if(pcindicate)
	{
		if(pcSound)
		{
			SDL_setPCSpeaker(((*pcSound++)&0x80)>>6);
			pcLengthLeft--;
			if(!pcLengthLeft)
			{
				pcSound=0;
				SDL_turnOffPCSpeaker();
				SDL_DigitizedDoneInIRQ();
			}
		}
	}
	extreme++;
	if(extreme>=10)
	{
		extreme=0;
		SDL_DoFast();
	}
	else
		outp(0x20,0x20);
}

//

void SDL_IndicatePC(boolean ind)
{
	pcindicate=ind;
}

void
SDL_DigitizedDoneInIRQ(void)
{
	if (DigiNextAddr)
	{
		SDL_PlayDigiSegment(DigiNextAddr,DigiNextLen/*,true*/);
		DigiNextAddr = nil;
		DigiMissed = false;
	}
	else
	{
		if (DigiLastSegment)
		{
			DigiPlaying = false;
			DigiLastSegment = false;
			if ((DigiMode == sds_PC) && (SoundMode == sdm_PC))
			{
				SDL_SoundFinished();
			}
			else
			{
				DigiNumber = (soundnames) 0;
				DigiPriority = 0;
			}
			SoundPositioned = false;
		}
		else
			DigiMissed = true;
	}
}

#if 0
// Inside an interrupt handler interrupts should already be disabled
// so don't disable them again and cause V86 exceptions which cost
// aprox. 300 processor tics!

//static
void alOutInIRQ(byte n,byte b)
{
	__asm {
		mov     dx,0x388
		mov     al,[n]
		out     dx,al
		in      al,dx
		in      al,dx
		in      al,dx
		in      al,dx
		in      al,dx
		in      al,dx
		inc     dx
		mov     al,[b]
		out     dx,al

		dec     dx
		in      al,dx
		in      al,dx
		in      al,dx
		in      al,dx
		in      al,dx
		in      al,dx
		in      al,dx
		in      al,dx
		in      al,dx
		in      al,dx

		in      al,dx
		in      al,dx
		in      al,dx
		in      al,dx
		in      al,dx
		in      al,dx
		in      al,dx
		in      al,dx
		in      al,dx
		in      al,dx

		in      al,dx
		in      al,dx
		in      al,dx
		in      al,dx
		in      al,dx
		in      al,dx
		in      al,dx
		in      al,dx
		in      al,dx
		in      al,dx

		in      al,dx
		in      al,dx
		in      al,dx
		in      al,dx
		in      al,dx
	}
}
#endif
