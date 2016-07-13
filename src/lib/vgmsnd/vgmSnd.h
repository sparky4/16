#ifndef __VGMSNDDRV_H__
#define __VGMSNDDRV_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "src/lib/vgmsnd/stdtype.h"
#include "src/lib/16_head.h"

typedef struct _vgm_file_header
{
	dword/*32*/ fccVGM;
	dword/*32*/ lngEOFOffset;
	dword/*32*/ lngVersion;
	dword/*32*/ lngHzPSG;
	dword/*32*/ lngHzYM2413;
	dword/*32*/ lngGD3Offset;
	dword/*32*/ lngTotalSamples;
	dword/*32*/ lngLoopOffset;
	dword/*32*/ lngLoopSamples;
	dword/*32*/ lngRate;
	UINT8 bytPSGCfg[4];
	dword/*32*/ lngHzYM2612;
	dword/*32*/ lngHzYM2151;
	dword/*32*/ lngDataOffset;
	dword/*32*/ lngHzSPCM;
	dword/*32*/ lngSPCMIntf;
	dword/*32*/ lngHzRF5C68;
	dword/*32*/ lngHzYM2203;
	dword/*32*/ lngHzYM2608;
	dword/*32*/ lngHzYM2610;
	dword/*32*/ lngHzYM3812;
	dword/*32*/ lngHzYM3526;
	dword/*32*/ lngHzY8950;
	dword/*32*/ lngHzYMF262;
	dword/*32*/ lngHzYMF278B;
	dword/*32*/ lngHzYMF271;
	dword/*32*/ lngHzYMZ280B;
	dword/*32*/ lngHzRF5C164;
	dword/*32*/ lngHzPWM;
	dword/*32*/ lngHzAY8910;
	UINT8 bytAYCfg[4];
	UINT8 bytVolumeModifier;
	UINT8 bytReserved2;
	INT8 bytLoopBase;
	UINT8 bytLoopModifier;
} VGM_HEADER;

typedef struct _vgm_file
{
	dword/*32*/ dataLen;
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
extern void OPL2_Write(UINT8 reg, UINT8 data);
extern UINT8 OPL2_ReadStatus(void);


#ifdef __cplusplus
}
#endif

#endif // ! __VGMSNDDRV_H__
