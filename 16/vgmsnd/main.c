#ifdef _WIN32
#ifdef _DEBUG
#include <crtdbg.h>
#endif
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>	// for toupper()


#ifdef _WIN32
int __cdecl _getch(void);	// from conio.h
#else
#define _getch	getchar
#endif

#include <common.h>
#include <audio/AudioStream.h>
#include <audio/AudioStream_SpcDrvFuns.h>
#include "vgmSndDrv.h"
#include "3812intf.h"


int main(int argc, char* argv[]);
static UINT32 FillBuffer(void* Params, UINT32 bufSize, void* Data);
void OPL2_Write(UINT8 reg, UINT8 data);
UINT8 OPL2_ReadStatus(void);
void EngineControlCUI(void);


#define VGM_SLOTS	4

static void* audDrv;
static void* audDrvLog;
INT32 CHIP_SAMPLE_RATE;
VGM_FILE vgmFiles[VGM_SLOTS];

int main(int argc, char* argv[])
{
	UINT8 retVal;
	UINT32 drvCount;
	UINT32 idWavOut;
	UINT32 idWavOutDev;
	AUDDRV_INFO* drvInfo;
	AUDIO_OPTS* opts;
	
	Audio_Init();
	drvCount = Audio_GetDriverCount();
	if (! drvCount)
		goto Exit_Deinit;
	
	idWavOut = 0;
	idWavOutDev = 0;
	Audio_GetDriverInfo(idWavOut, &drvInfo);
	printf("Using driver %s.\n", drvInfo->drvName);
	retVal = AudioDrv_Init(idWavOut, &audDrv);
	if (retVal)
	{
		printf("WaveOut: Drv Init Error: %02X\n", retVal);
		goto Exit_Deinit;
	}
	
	opts = AudioDrv_GetOptions(audDrv);
	opts->numChannels = 1;
	opts->numBitsPerSmpl = 16;
	CHIP_SAMPLE_RATE = opts->sampleRate;
	
	device_start_ym3812(0, 3579545);
	device_reset_ym3812(0);
	InitEngine();
	
	AudioDrv_SetCallback(audDrv, FillBuffer);
	printf("Opening Device %u ...\n", idWavOutDev);
	retVal = AudioDrv_Start(audDrv, idWavOutDev);
	if (retVal)
	{
		printf("Dev Init Error: %02X\n", retVal);
		goto Exit_DrvDeinit;
	}
	
	/*getchar();
	printf("Current Latency: %u ms\n", AudioDrv_GetLatency(audDrv));*/
	EngineControlCUI();
	
	retVal = AudioDrv_Stop(audDrv);
	
Exit_DrvDeinit:
	DeinitEngine();
	device_stop_ym3812(0);
	AudioDrv_Deinit(&audDrv);
Exit_Deinit:
	Audio_Deinit();
	printf("Done.\n");
	
#if _DEBUG
	if (_CrtDumpMemoryLeaks())
		_getch();
#endif
	
	return 0;
}

INT32 chipSmplL[1] = {0};
INT32 chipSmplR[1] = {0};
INT32* chipSmpls[2] = {chipSmplL, chipSmplR};
static UINT32 smplLastIrq = 0;
static UINT32 FillBuffer(void* Params, UINT32 bufSize, void* data)
{
	UINT32 smplCount;
	INT16* SmplPtr16;
	UINT32 curSmpl;
	INT32 chipSmplsFin;
	
	smplCount = bufSize / 2;
	SmplPtr16 = (INT16*)data;
	for (curSmpl = 0; curSmpl < smplCount; curSmpl ++)
	{
		ym3812_stream_update(0, chipSmpls, 1);
		UpdateSoundEngine();
		smplLastIrq ++;
		
		chipSmplsFin = chipSmplL[0];
		
#if 0
		if ((curSmpl / (smplCount / 16)) < 15)
			chipSmplsFin += +0x0100;
		else
			chipSmplsFin += -0x0100;
#endif
		
		if (chipSmplsFin < -0x7FFF)
			chipSmplsFin = -0x7FFF;
		else if (chipSmplsFin > 0x7FFF)
			chipSmplsFin = 0x7FFF;
		SmplPtr16[curSmpl] = (INT16)chipSmplsFin;
	}
	return curSmpl * 2;
}


void OPL2_Write(UINT8 reg, UINT8 data)
{
	ym3812_w(0, 0, reg);
	ym3812_w(0, 1, data);
	return;
}

UINT8 OPL2_ReadStatus(void)
{
	return ym3812_r(0, 0);
}



INLINE UINT8 getVgmID(char vgmIdChar)
{
	vgmIdChar = toupper(vgmIdChar);
	if (vgmIdChar == 'M')
		return 0x00;
	else if (vgmIdChar >= '0' && vgmIdChar < '0'+VGM_SLOTS)
		return (UINT8)(vgmIdChar - '0');
	else
		return 0xFF;
}

INLINE UINT8 getVgmChn(char vgmChnChar)
{
	vgmChnChar = toupper(vgmChnChar);
	if (vgmChnChar == 'M')
		return 0x7F;
	else if (vgmChnChar >= '0' && vgmChnChar <= '9')
		return vgmChnChar - '0';
	else
		return 0xFF;
}

void EngineControlCUI(void)
{
	VGM_FILE* tempVgmFile;
	UINT8 curSFX;
	char inLine[0x100];
	char* tempStr;
	int tempPos;
	UINT8 vgmId;
	UINT8 vgmChn;
	UINT8 retVal;
	
	printf("Commands:\n");
	printf("Ls File.vgm - Load File.vgm into Slot s\n");
	printf("Pcs - Play Slot s on Channel c\n");
	printf("Sc - Stop Channel c\n");
	printf("S - Stop all SFX\n");
	printf("E - Pause Music\n");
	printf("R - Resume Music\n");
	printf("Channels: M = music, 0-5 = SFX\n");
	printf("Slots: M, 0-3 (M equals slot 0)\n");
	
	for (curSFX = 0; curSFX < VGM_SLOTS; curSFX ++)
		memset(&vgmFiles[curSFX], 0x00, sizeof(VGM_FILE));
	
	while(1)
	{
		tempStr = fgets(inLine, 0x100, stdin);
		if (tempStr == NULL)
			break;
		tempPos = strlen(inLine);
		if (tempPos <= 1)
			break;
		inLine[tempPos-1] = '\0';
		
		switch(toupper(inLine[0]))
		{
		case 'L':	// load
			vgmId = getVgmID(inLine[1]);
			if (vgmId == 0xFF)
			{
				printf("Wrong File Slot!\n");
				break;
			}
			tempVgmFile = &vgmFiles[vgmId];
			FreeVGMFile(tempVgmFile);
			retVal = OpenVGMFile(inLine + 3, tempVgmFile);
			if (retVal > 0)
				printf("Load Error 0x%02X\n", retVal);
			else
				printf("VGM loaded into slot %d.\n", vgmId);
			break;
		case 'P':	// play
			vgmChn = getVgmChn(inLine[1]);
			vgmId = getVgmID(inLine[2]);
			if (vgmChn == 0xFF)
			{
				printf("Wrong Channel!\n");
				break;
			}
			if (vgmId == 0xFF)
			{
				printf("Wrong File Slot!\n");
				break;
			}
			tempVgmFile = &vgmFiles[vgmId];
			if (vgmChn == 0x7F)
				PlayMusic(tempVgmFile);
			else
				PlaySFX(tempVgmFile, vgmChn);
			break;
		case 'S':	// stop
			vgmChn = getVgmChn(inLine[1]);
			if (vgmChn == 0x7F)
				StopMusic();
			else
				StopSFX(vgmChn);
			break;
		case 'E':	// pause
			PauseMusic();
			break;
		case 'R':	// resume
			ResumeMusic();
			break;
		default:
			printf("Unknown Command!\n");
			break;
		}
	}
	
	StopMusic();
	StopSFX(0xFF);
	for (curSFX = 0; curSFX < VGM_SLOTS; curSFX ++)
		FreeVGMFile(&vgmFiles[curSFX]);
	
	printf("Quit.\n");
	_getch();
	
	return;
}
