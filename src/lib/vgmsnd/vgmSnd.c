// vgmSndDrv.c - VGM Sound Driver for OPL2
// Valley Bell, 2015-07-27

// Note: This uses quite a few optimizations that assume that your
//       machine is Little Endian.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/lib/vgmsnd/common.h"
#include "src/lib/vgmsnd/vgmSnd.h"


#define QUICK_READ



#define FCC_VGM	0x206D6756	// 'Vgm '
#define FCC_GD3	0x20336447	// 'Gd3 '

typedef struct _vgm_file_header_base
{
	dword fccVGM;			// 00
	dword/*32*/ lngEOFOffset;	// 04
	dword/*32*/ lngVersion;		// 08
	dword/*32*/ lngSkip1[2];		// 0C
	dword/*32*/ lngGD3Offset;	// 14
	dword/*32*/ lngTotalSamples;	// 18
	dword/*32*/ lngLoopOffset;	// 1C
	dword/*32*/ lngLoopSamples;	// 20
	dword/*32*/ lngRate;			// 24
	dword/*32*/ lngSkip2[3];		// 28
	dword/*32*/ lngDataOffset;	// 34
	dword/*32*/ lngSkip3[2];		// 38
} VGM_BASE_HDR;

#define PBMODE_MUSIC	0x00
#define PBMODE_SFX		0x01
typedef struct _vgm_playback
{
	UINT8 pbMode;
	UINT8 vgmEnd;	// 00 - running, 01 - finished, FF - not loaded
	UINT16 curLoopCnt;
	dword/*32*/ vgmPos;
	dword/*32*/ vgmSmplPos;
	dword/*32*/ pbSmplPos;
	VGM_FILE* file;

	// oplChnMask:
	//	Music: mask of channels used/overridden by SFX
	//	SFX:   ID of channel used by SFX (all commands are forces to it)
	UINT16 oplChnMask;
	UINT8* oplRegCache;
	UINT8 workRAM[0x04];
} VGM_PBK;



INLINE UINT16 ReadLE16(const UINT8* buffer)
{
#ifdef QUICK_READ
	return *(UINT16*)buffer;
#else
	return (buffer[0x00] << 0) | (buffer[0x01] << 8);
#endif
}

INLINE dword/*32*/ ReadLE32(const UINT8* buffer)
{
#ifdef QUICK_READ
	return *(dword/*32*/*)buffer;
#else
	return	(buffer[0x00] <<  0) | (buffer[0x01] <<  8) |
			(buffer[0x02] << 16) | (buffer[0x03] << 24);
#endif
}


// Function Prototypes
//UINT8 OpenVGMFile(const char* FileName, VGM_FILE* vgmFile);
//void FreeVGMFile(VGM_FILE* vgmFile);

static boolean DoVgmLoop(VGM_PBK* vgmPlay);
static void UpdateVGM(VGM_PBK* vgmPlay, UINT16 Samples);

//void InitEngine(void);
//void DeinitEngine(void);

//UINT8 PlayMusic(VGM_FILE* vgmFile);
//UINT8 PlaySFX(VGM_FILE* vgmFile, UINT8 sfxChnID);
//UINT8 StopMusic(void);
//UINT8 StopSFX(UINT8 sfxChnID);	// Note: sfxChnID == 0xFF -> stop all SFX
//UINT8 PauseMusic(void);
//UINT8 ResumeMusic(void);
static void StartPlayback(VGM_PBK* vgmPb);
static void StopPlayback(VGM_PBK* vgmPb);

static void ym2413_write(VGM_PBK* vgmPb, UINT8 reg, UINT8 data);
static void ym3812_write(VGM_PBK* vgmPb, UINT8 reg, UINT8 data);
static void ym3512_write(VGM_PBK* vgmPb, UINT8 reg, UINT8 data);
static void ymf262_write(VGM_PBK* vgmPb, UINT8 port, UINT8 reg, UINT8 data);

//void UpdateSoundEngine(void);


// Functions that must be supplied by external library
extern void OPL2_Write(UINT8 reg, UINT8 data);
extern UINT8 OPL2_ReadStatus(void);




#define SFX_CHN_COUNT	6

#define TIMER1_RATE		7	// 256-7 = 248; (3579545/72/4) / 248 = 50.12 Hz ~ 50 Hz
#define VGM_UPD_RATE	880	// 880 samples (44.1 KHz) = 50.11 Hz

static VGM_PBK vgmPbMusic;
static VGM_PBK vgmPbSFX[SFX_CHN_COUNT];

static UINT8 oplRegs_Music[0x100];
static UINT8 oplRegs_SFX[SFX_CHN_COUNT][0x0D];	// 20 23 40 43 60 63 80 83 E0 E3 C0 A0 B0

static const UINT8 SFX_REGS[0x0D] =
{	0x20, 0x23, 0x40, 0x43, 0x60, 0x63, 0x80, 0x83,
	0xE0, 0xE3, 0xC0, 0xA0, 0xB0};
static const UINT8 SFX_REGS_REV[0x10] =	// 20/30 -> 0, 40/50 -> 2, ...
{	0xFF, 0xFF, 0x00, 0x00, 0x02, 0x02, 0x04, 0x04,
	0x06, 0x06, 0x0B, 0x0C, 0x0A, 0xFF, 0x08, 0x08};
static const UINT8 CHN_OPMASK[0x09] =
{	0x00, 0x01, 0x02, 0x08, 0x09, 0x0A, 0x10, 0x11, 0x12};
static const UINT8 CHN_OPMASK_REV[0x20] =
{	0x00, 0x01, 0x02, 0x80, 0x81, 0x82, 0xFF, 0xFF,
	0x03, 0x04, 0x05, 0x83, 0x84, 0x85, 0xFF, 0xFF,
	0x06, 0x07, 0x08, 0x86, 0x87, 0x88, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};


UINT8 OpenVGMFile(const char* FileName, VGM_FILE* vgmFile, global_game_variables_t *gvar)
{
	size_t hdrSize;
	size_t readEl;	// 'elements' read from file
	size_t bytesToRead;
	VGM_BASE_HDR vgmBaseHdr;
	FILE* hFile;
	dword/*32*/ CurPos;

	memset(vgmFile, 0x00, sizeof(VGM_FILE));

	hFile = fopen(FileName, "rb");
	if (hFile == NULL)
		return 0xFF;

	hdrSize = sizeof(VGM_BASE_HDR);
	readEl = fread(&vgmBaseHdr, hdrSize, 0x01, hFile);
	if (readEl == 0)
	{
		fclose(hFile);
		return 0xFE;	// read error
	}
	if (vgmBaseHdr.fccVGM != FCC_VGM)
	{
		fclose(hFile);
		return 0x80;	// bad signature
	}
	if (vgmBaseHdr.lngVersion < 0x0150)
	{
		fclose(hFile);
		return 0x81;	// We don't support VGM v1.10 and earlier
	}

	vgmFile->dataLen = vgmBaseHdr.lngEOFOffset + 0x04;
	//vgmFile->data = (UINT8*)malloc(vgmFile->dataLen);
	MM_GetPtr(MEMPTRCONV gvar->ca.audiosegs[0], vgmFile->dataLen, gvar);
	vgmFile->data = (UINT8*)gvar->ca.audiosegs[0];
	if (vgmFile->data == NULL)
	{
		fclose(hFile);
		return 0xF0;	// malloc error
	}
	memcpy(vgmFile->data, &vgmBaseHdr, hdrSize);
	bytesToRead = vgmFile->dataLen - hdrSize;
	readEl = fread(vgmFile->data + hdrSize, 0x01, bytesToRead, hFile);
	if (readEl < bytesToRead)
	{
		//fclose(hFile);
		//return 0xFE;	// read error
		vgmFile->dataLen = hdrSize + readEl;
	}

	fclose(hFile);

	memcpy(&vgmFile->header, vgmFile->data, sizeof(VGM_HEADER));

	// relative -> absolute addresses
	vgmFile->header.lngEOFOffset += 0x04;
	if (vgmFile->header.lngGD3Offset)
		vgmFile->header.lngGD3Offset += 0x14;
	if (vgmFile->header.lngLoopOffset)
		vgmFile->header.lngLoopOffset += 0x1C;
	if (! vgmFile->header.lngDataOffset)
		vgmFile->header.lngDataOffset = 0x0C;
	vgmFile->header.lngDataOffset += 0x34;

	CurPos = vgmFile->header.lngDataOffset;
	if (vgmFile->header.lngVersion < 0x0150)
		CurPos = 0x40;
	hdrSize = sizeof(VGM_HEADER);
	if (hdrSize > CurPos)
		memset((UINT8*)&vgmFile->header + CurPos, 0x00, hdrSize - CurPos);

	fclose(hFile);
	return 0x00;
}

void FreeVGMFile(VGM_FILE* vgmFile, global_game_variables_t *gvar)
{
	//if(vgmFile->data){ free(vgmFile->data);	vgmFile->data = NULL; }
	MM_FreePtr(MEMPTRCONV gvar->ca.audiosegs[0], gvar);
	if(vgmFile->data) free(vgmFile->data);
	vgmFile->dataLen = 0;

//	return;
}


static boolean DoVgmLoop(VGM_PBK* vgmPlay)
{
	const VGM_HEADER* vgmHdr = &vgmPlay->file->header;

	if (! vgmHdr->lngLoopOffset)
		return false;

	vgmPlay->curLoopCnt ++;

	vgmPlay->vgmPos = vgmHdr->lngLoopOffset;
	vgmPlay->vgmSmplPos -= vgmHdr->lngLoopSamples;
	vgmPlay->pbSmplPos -= vgmHdr->lngLoopSamples;

	return true;
}

static void UpdateVGM(VGM_PBK* vgmPlay, UINT16 Samples)
{
	const dword/*32*/ vgmLen = vgmPlay->file->dataLen;
	const UINT8* vgmData = vgmPlay->file->data;
	const UINT8* VGMPnt;
	dword/*32*/ VGMPos;
	dword/*32*/ VGMSmplPos;
	UINT8 Command;
	UINT8 blockType;
	dword/*32*/ blockLen;

	vgmPlay->pbSmplPos += Samples;
	VGMPos = vgmPlay->vgmPos;
	VGMSmplPos = vgmPlay->vgmSmplPos;
	while(VGMSmplPos < vgmPlay->pbSmplPos && ! vgmPlay->vgmEnd)
	{
		VGMPnt = &vgmData[VGMPos];
		Command = VGMPnt[0x00];
		switch(Command & 0xF0)
		{
		case 0x70:	// small delay (1-16 samples)
			VGMSmplPos += (Command & 0x0F) + 0x01;
			VGMPos += 0x01;
			break;
		case 0x80:	// DAC write + small delay (0-15 samples)
			VGMSmplPos += (Command & 0x0F);
			VGMPos += 0x01;
			break;
		case 0x60:
			switch(Command)
			{
			case 0x66:	// End Of File
				vgmPlay->vgmPos = VGMPos;
				vgmPlay->vgmSmplPos = VGMSmplPos;
				if (! DoVgmLoop(vgmPlay))
					vgmPlay->vgmEnd = 0x01;
				VGMPos = vgmPlay->vgmPos;
				VGMSmplPos = vgmPlay->vgmSmplPos;
				break;
			case 0x62:	// 1/60s delay
				VGMSmplPos += 735;
				VGMPos += 0x01;
				break;
			case 0x63:	// 1/50s delay
				VGMSmplPos += 882;
				VGMPos += 0x01;
				break;
			case 0x61:	// xx Sample Delay
				VGMSmplPos += ReadLE16(&VGMPnt[0x01]);
				VGMPos += 0x03;
				break;
			case 0x67:	// Data Block (PCM Data Stream)
				blockType = VGMPnt[0x02];
				blockLen = ReadLE32(&VGMPnt[0x03]);
				blockLen &= 0x7FFFFFFF;
				VGMPos += 0x07 + blockLen;
				break;
			case 0x68:	// PCM RAM write
				VGMPos += 0x0C;
				break;
			default:
				vgmPlay->vgmEnd = 0x01;
				break;
			}
			break;
		case 0x50:
			if (Command == 0x50)
			{
				VGMPos += 0x02;	// SN76496 write
				break;
			}
			switch(Command)
			{
			case 0x51:	// YM2413 write
				ym2413_write(vgmPlay, VGMPnt[0x01], VGMPnt[0x02]);
				break;
			case 0x5A:	// YM3812 write
				ym3812_write(vgmPlay, VGMPnt[0x01], VGMPnt[0x02]);
				break;
			case 0x5B:	// YM3526 write
			case 0x5C:	// Y8950 write
				ym3512_write(vgmPlay, VGMPnt[0x01], VGMPnt[0x02]);
				break;
			case 0x5E:	// YMF262 write, port 0
			case 0x5F:	// YMF262 write, port 1
				ymf262_write(vgmPlay, Command & 0x01, VGMPnt[0x01], VGMPnt[0x02]);
				break;
			}
			VGMPos += 0x03;
			break;
		case 0x30:
			VGMPos += 0x02;
			break;
		case 0x40:
		case 0xA0:
		case 0xB0:
			VGMPos += 0x03;
			break;
		case 0xC0:
		case 0xD0:
			VGMPos += 0x04;
			break;
		case 0xE0:
		case 0xF0:
			VGMPos += 0x05;
			break;
		case 0x90:
			switch(Command)
			{
			case 0x90:	// DAC Ctrl: Setup Chip
				VGMPos += 0x05;
				break;
			case 0x91:	// DAC Ctrl: Set Data
				VGMPos += 0x05;
				break;
			case 0x92:	// DAC Ctrl: Set Freq
				VGMPos += 0x06;
				break;
			case 0x93:	// DAC Ctrl: Play from Start Pos
				VGMPos += 0x0B;
				break;
			case 0x94:	// DAC Ctrl: Stop immediately
				VGMPos += 0x02;
				break;
			case 0x95:	// DAC Ctrl: Play Block (small)
				VGMPos += 0x05;
				break;
			default:
				vgmPlay->vgmEnd = 0x01;
				break;
			}
			break;
		default:
			vgmPlay->vgmEnd = 0x01;
			return;
		}

		if (VGMPos >= vgmLen)
			vgmPlay->vgmEnd = 0x01;
	}
	vgmPlay->vgmPos = VGMPos;
	vgmPlay->vgmSmplPos = VGMSmplPos;
	if (vgmPlay->vgmEnd)
		StopPlayback(vgmPlay);

	return;
}




void InitEngine(void)
{
	UINT8 curSFX;
	UINT8 curReg;

	memset(oplRegs_Music, 0x00, 0x100);
	memset(&vgmPbMusic, 0x00, sizeof(VGM_PBK));
	vgmPbMusic.pbMode = PBMODE_MUSIC;
	vgmPbMusic.vgmEnd = 0xFF;
	vgmPbMusic.oplChnMask = 0x0000;
	vgmPbMusic.oplRegCache = oplRegs_Music;

	for (curSFX = 0; curSFX < SFX_CHN_COUNT; curSFX ++)
	{
		memset(&oplRegs_SFX[curSFX], 0x00, sizeof(VGM_PBK));
		memset(&vgmPbSFX[curSFX], 0x00, sizeof(VGM_PBK));
		vgmPbSFX[curSFX].pbMode = PBMODE_SFX;
		vgmPbSFX[curSFX].vgmEnd = 0xFF;
		vgmPbSFX[curSFX].oplChnMask = curSFX;
		vgmPbSFX[curSFX].oplRegCache = oplRegs_SFX[curSFX];
	}

	// reset OPL2 chip
	curReg = 0x00;
	do
	{
		curReg --;
		OPL2_Write(curReg, 0x00);
	} while(curReg > 0x20);

	OPL2_Write(0x02, TIMER1_RATE);	// set Timer 1 Period
	OPL2_Write(0x04, 0x01);	// Timer 1 on/unmasked, Timer 2 off
	OPL2_Write(0x04, 0x80);	// Reset Timer/IRQ Status Flags

	OPL2_Write(0x01, 0x20);	// Waveform Select: Enable

	return;
}

void DeinitEngine(void)
{
	UINT8 curSFX;

	StopPlayback(&vgmPbMusic);
	for (curSFX = 0; curSFX < SFX_CHN_COUNT; curSFX ++)
		StopPlayback(&vgmPbSFX[curSFX]);

	OPL2_Write(0x04, 0x00);	// disable all timers

	return;
}


UINT8 PlayMusic(VGM_FILE* vgmFile)
{
	VGM_PBK* vgmPb = &vgmPbMusic;

	if (! vgmPb->vgmEnd)
		StopPlayback(vgmPb);

	vgmPb->file = vgmFile;

	StartPlayback(vgmPb);

	return 0x00;
}

UINT8 PlaySFX(VGM_FILE* vgmFile, UINT8 sfxChnID)
{
	VGM_PBK* vgmPb;

	if (sfxChnID >= SFX_CHN_COUNT)
		return 0xFF;

	vgmPb = &vgmPbSFX[sfxChnID];

	if (! vgmPb->vgmEnd)
		StopPlayback(vgmPb);

	vgmPb->file = vgmFile;

	StartPlayback(vgmPb);

	return 0x00;
}

UINT8 StopMusic(void)
{
	StopPlayback(&vgmPbMusic);
	return 0x00;
}

UINT8 StopSFX(UINT8 sfxChnID)
{
	if (sfxChnID == 0xFF)
	{
		for (sfxChnID = 0; sfxChnID < SFX_CHN_COUNT; sfxChnID ++)
			StopPlayback(&vgmPbSFX[sfxChnID]);
		return 0x00;
	}

	if (sfxChnID >= SFX_CHN_COUNT)
		return 0xFF;

	StopPlayback(&vgmPbSFX[sfxChnID]);
	return 0x00;
}

UINT8 PauseMusic(void)
{
	if (vgmPbMusic.vgmEnd == 0xFF)
		return 0xFF;	// not playing
	if (vgmPbMusic.vgmEnd == 0x01)
		return 0x80;	// finished playing already
	if (vgmPbMusic.vgmEnd == 0x02)
		return 0x01;	// is already paused

	StopPlayback(&vgmPbMusic);
	vgmPbMusic.vgmEnd = 0x02;

	return 0x00;
}

UINT8 ResumeMusic(void)
{
	if (vgmPbMusic.vgmEnd == 0xFF)
		return 0xFF;	// not playing
	if (vgmPbMusic.vgmEnd == 0x01)
		return 0x80;	// finished playing already
	if (! (vgmPbMusic.vgmEnd & 0x02))
		return 0x01;	// is not paused

	vgmPbMusic.vgmEnd &= ~0x02;

	return 0x00;
}

static void StartPlayback(VGM_PBK* vgmPb)
{
	if (vgmPb->file == NULL || vgmPb->file->data == NULL ||
		vgmPb->file->header.fccVGM != FCC_VGM
)
	{
		vgmPb->vgmEnd = 0xFF;
		return;
	}

	vgmPb->vgmEnd = 0x00;	// set to 'running'
	vgmPb->vgmPos = vgmPb->file->header.lngDataOffset;
	vgmPb->vgmSmplPos = 0;
	vgmPb->pbSmplPos = 0;
	vgmPb->curLoopCnt = 0;
	memset(vgmPb->workRAM, 0x00, 0x04);

	if (vgmPb->pbMode == PBMODE_SFX)
	{
		UINT8 curReg;

		curReg = 0xB0 | vgmPb->oplChnMask;
		if (oplRegs_Music[curReg] & 0x20)
			OPL2_Write(curReg, oplRegs_Music[curReg] & ~0x20);	// send Key Off

		vgmPbMusic.oplChnMask |= (1 << vgmPb->oplChnMask);	// mask out music channel
	}

	return;
}

static void StopPlayback(VGM_PBK* vgmPb)
{
	if (vgmPb->vgmEnd & 0x80)
		return;

	if (vgmPb->pbMode == PBMODE_MUSIC)
	{
		UINT8 curReg;
		UINT16 chnMask;

		chnMask = 0x0001;
		for (curReg = 0xB0; curReg < 0xB9; curReg ++, chnMask <<= 1)
		{
			if (vgmPb->oplChnMask & chnMask)
				continue;	// keep channels used by SFX untouched
			if (vgmPb->oplRegCache[curReg] & 0x20)
			{
				vgmPb->oplRegCache[curReg] &= ~0x20;
				OPL2_Write(curReg, vgmPb->oplRegCache[curReg]);	// send Key Off
			}
		}
		curReg = 0xBD;	// rhythm register
		if (vgmPb->oplRegCache[curReg] & 0x1F)
		{
			vgmPb->oplRegCache[curReg] &= ~0x1F;
			OPL2_Write(curReg, vgmPb->oplRegCache[curReg]);	// send Key Off
		}

		vgmPb->vgmEnd = 0x01;
	}
	else //if (vgmPb->pbMode == PBMODE_SFX)
	{
		UINT8 regID;
		UINT8 curReg;
		UINT8 opMask;

		curReg = 0xB0 | vgmPb->oplChnMask;
		if (vgmPb->oplRegCache[0x0C] & 0x20)
		{
			vgmPb->oplRegCache[0x0C] &= ~0x20;
			OPL2_Write(curReg, vgmPb->oplRegCache[0x0C]);	// send Key Off
		}

		vgmPb->vgmEnd = 0x01;

		if (! vgmPbMusic.vgmEnd)	// if (music is playing)
		{
			opMask = CHN_OPMASK[vgmPb->oplChnMask];
			for (regID = 0x00; regID < 0x0A; regID ++)
			{
				curReg = SFX_REGS[regID] + opMask;
				OPL2_Write(curReg, oplRegs_Music[curReg]);	// restore Music register
			}
			for (; regID < 0x0D; regID ++)
			{
				curReg = SFX_REGS[regID] | vgmPb->oplChnMask;
				OPL2_Write(curReg, oplRegs_Music[curReg]);	// restore Music register
			}

			vgmPbMusic.oplChnMask &= ~(1 << vgmPb->oplChnMask);
		}
	}

	return;
}



static void OPL_CachedWrite(VGM_PBK* vgmPb, UINT8 reg, UINT8 data)
{
	UINT8 regChn;
	UINT8 ramOfs;

	if (vgmPb->pbMode == PBMODE_MUSIC)
	{
		if (reg == 0x01)
			data |= 0x20;	// enforce "Waveform Select Enable" bit
		vgmPb->oplRegCache[reg] = data;

		ramOfs = SFX_REGS_REV[reg >> 4];
		if (ramOfs < 0x0A)	// Operator 20/40/60/80/E0
		{
			regChn = CHN_OPMASK_REV[reg & 0x1F] & 0x7F;
			if (vgmPb->oplChnMask & (1 << regChn))
				return;	// channel overridden by SFX - return
		}
		else if (ramOfs < 0x0D)	// Operator C0/A0/B0
		{
			regChn = reg & 0x0F;
			if (vgmPb->oplChnMask & (1 << regChn))
				return;	// channel overridden by SFX - return
		}
	}
	else //if (vgmPb->pbMode == PBMODE_SFX)
	{
		if (reg == 0xBD)
			return;	// no rhythm register for SFX

		ramOfs = SFX_REGS_REV[reg >> 4];
		if (ramOfs == 0xFF)
			return;

		if (ramOfs < 0x0A)	// Operator 20/40/60/80/E0
		{
			regChn = CHN_OPMASK_REV[reg & 0x1F];
			if (regChn == 0xFF)
				return;	// ignore writes to invalid channels/operators
			ramOfs += (regChn & 0x80) >> 7;
			regChn &= 0x7F;
			vgmPb->oplRegCache[ramOfs] = data;

			if (regChn != vgmPb->oplChnMask)
			{
				// force command to current channel
				reg = SFX_REGS[ramOfs] + CHN_OPMASK[vgmPb->oplChnMask];
			}
		}
		else	// Operator C0/A0/B0
		{
			regChn = CHN_OPMASK_REV[reg & 0x0F];
			if (regChn >= 0x09)
				return;	// ignore writes to invalid channels
			vgmPb->oplRegCache[ramOfs] = data;

			reg = (reg & 0xF0) | vgmPb->oplChnMask;
		}
	}

	OPL2_Write(reg, data);
	return;
}


static void ym2413_write(VGM_PBK* vgmPb, UINT8 reg, UINT8 data)
{
	return;	// unsupported for now
}

static void ym3812_write(VGM_PBK* vgmPb, UINT8 reg, UINT8 data)
{
	if (reg == 0x01)
	{
		vgmPb->workRAM[0x00] = data & 0x20;	// store "Wave Select Enable" bit
	}
	else if ((reg & 0xE0) == 0xE0)
	{
		if (! vgmPb->workRAM[0x00])	// "Wave Select Enable" off?
			data = 0x00;	// disable waveforms
	}

	OPL_CachedWrite(vgmPb, reg, data);
	return;
}

static void ym3512_write(VGM_PBK* vgmPb, UINT8 reg, UINT8 data)
{
	if ((reg & 0xE0) == 0xE0)
	{
		data = 0x00;	// OPL1 has no waveforms
	}
	if (reg >= 0x07 && reg < 0x20)
	{
		if (reg == 0x08)
			data &= ~0x0F;	// mask out Y8950 DeltaT data
		else
			return;	// ignore Y8950 DeltaT writes
	}

	OPL_CachedWrite(vgmPb, reg, data);
	return;
}

static void ymf262_write(VGM_PBK* vgmPb, UINT8 port, UINT8 reg, UINT8 data)
{
	return;	// unsupported for now
}



void UpdateSoundEngine(void)
{
	UINT8 tmrMask;
	UINT8 curSFX;

	tmrMask = OPL2_ReadStatus();
	if (! (tmrMask & 0x40))
		return;	// wait for overflow
	OPL2_Write(0x04, 0x80);	// Reset Timer/IRQ Status Flags

	if (! vgmPbMusic.vgmEnd)
		UpdateVGM(&vgmPbMusic, VGM_UPD_RATE);
	for (curSFX = 0; curSFX < SFX_CHN_COUNT; curSFX ++)
	{
		if (! vgmPbSFX[curSFX].vgmEnd)
			UpdateVGM(&vgmPbSFX[curSFX], VGM_UPD_RATE);
	}

	return;
}
