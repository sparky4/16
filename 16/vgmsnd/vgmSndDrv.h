#ifndef __VGMSNDDRV_H__
#define __VGMSNDDRV_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdtype.h>

typedef struct _vgm_file_header
{
	UINT32 fccVGM;
	UINT32 lngEOFOffset;
	UINT32 lngVersion;
	UINT32 lngHzPSG;
	UINT32 lngHzYM2413;
	UINT32 lngGD3Offset;
	UINT32 lngTotalSamples;
	UINT32 lngLoopOffset;
	UINT32 lngLoopSamples;
	UINT32 lngRate;
	UINT8 bytPSGCfg[4];
	UINT32 lngHzYM2612;
	UINT32 lngHzYM2151;
	UINT32 lngDataOffset;
	UINT32 lngHzSPCM;
	UINT32 lngSPCMIntf;
	UINT32 lngHzRF5C68;
	UINT32 lngHzYM2203;
	UINT32 lngHzYM2608;
	UINT32 lngHzYM2610;
	UINT32 lngHzYM3812;
	UINT32 lngHzYM3526;
	UINT32 lngHzY8950;
	UINT32 lngHzYMF262;
	UINT32 lngHzYMF278B;
	UINT32 lngHzYMF271;
	UINT32 lngHzYMZ280B;
	UINT32 lngHzRF5C164;
	UINT32 lngHzPWM;
	UINT32 lngHzAY8910;
	UINT8 bytAYCfg[4];
	UINT8 bytVolumeModifier;
	UINT8 bytReserved2;
	INT8 bytLoopBase;
	UINT8 bytLoopModifier;
} VGM_HEADER;

typedef struct _vgm_file
{
	UINT32 dataLen;
	UINT8* data;
	VGM_HEADER header;
} VGM_FILE;


UINT8 OpenVGMFile(const char* FileName, VGM_FILE* vgmFile);
void FreeVGMFile(VGM_FILE* vgmFile);

void InitEngine(void);
void DeinitEngine(void);

UINT8 PlayMusic(VGM_FILE* vgmFile);
UINT8 PlaySFX(VGM_FILE* vgmFile, UINT8 sfxChnID);
UINT8 StopMusic(void);
UINT8 StopSFX(UINT8 sfxChnID);
UINT8 PauseMusic(void);
UINT8 ResumeMusic(void);

void UpdateSoundEngine(void);


// Functions that must be supplied by external library:
//extern void OPL2_Write(UINT8 reg, UINT8 data);
//extern UINT8 OPL2_ReadStatus(void);


#ifdef __cplusplus
}
#endif

#endif // ! __VGMSNDDRV_H__
