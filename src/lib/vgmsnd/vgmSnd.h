#ifndef __VGMSNDDRV_H__
#define __VGMSNDDRV_H__

#ifdef __cplusplus
extern "C"
{
#endif

//#include "src/lib/vgmsnd/stdtype.h"
#include "src/lib/16_head.h"

typedef struct _vgm_file_header
{
	sdword fccVGM;
	sdword lngEOFOffset;
	sdword lngVersion;
	sdword lngHzPSG;
	sdword lngHzYM2413;
	sdword lngGD3Offset;
	sdword lngTotalSamples;
	sdword lngLoopOffset;
	sdword lngLoopSamples;
	sdword lngRate;
	byte bytPSGCfg[4];
	sdword lngHzYM2612;
	sdword lngHzYM2151;
	sdword lngDataOffset;
	sdword lngHzSPCM;
	sdword lngSPCMIntf;
	sdword lngHzRF5C68;
	sdword lngHzYM2203;
	sdword lngHzYM2608;
	sdword lngHzYM2610;
	sdword lngHzYM3812;
	sdword lngHzYM3526;
	sdword lngHzY8950;
	sdword lngHzYMF262;
	sdword lngHzYMF278B;
	sdword lngHzYMF271;
	sdword lngHzYMZ280B;
	sdword lngHzRF5C164;
	sdword lngHzPWM;
	sdword lngHzAY8910;
	byte bytAYCfg[4];
	byte bytVolumeModifier;
	byte bytReserved2;
	char bytLoopBase;
	byte bytLoopModifier;
} VGM_HEADER;

typedef struct _vgm_file
{
	sdword dataLen;
	byte* data;
	VGM_HEADER header;
} VGM_FILE;


byte OpenVGMFile(const char* FileName, VGM_FILE* vgmFile);
void FreeVGMFile(VGM_FILE* vgmFile);

void InitEngine(void);
void DeinitEngine(void);

byte PlayMusic(VGM_FILE* vgmFile);
byte PlaySFX(VGM_FILE* vgmFile, byte sfxChnID);
byte StopMusic(void);
byte StopSFX(byte sfxChnID);
byte PauseMusic(void);
byte ResumeMusic(void);

void UpdateSoundEngine(void);


// Functions that must be supplied by external library:
extern void OPL2_Write(byte reg, byte data);
extern byte OPL2_ReadStatus(void);


#ifdef __cplusplus
}
#endif

#endif // ! __VGMSNDDRV_H__
