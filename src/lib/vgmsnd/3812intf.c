/******************************************************************************
* FILE
*   Yamaha 3812 emulator interface - MAME VERSION
*
* CREATED BY
*   Ernesto Corvi
*
* UPDATE LOG
*   JB  28-04-2002  Fixed simultaneous usage of all three different chip types.
*                       Used real sample rate when resample filter is active.
*       AAT 12-28-2001  Protected Y8950 from accessing unmapped port and keyboard handlers.
*   CHS 1999-01-09  Fixes new ym3812 emulation interface.
*   CHS 1998-10-23  Mame streaming sound chip update
*   EC  1998        Created Interface
*
* NOTES
*
******************************************************************************/
#include <stddef.h>
//#include "src/lib/vgmsnd/mamedef.h"
#include "src/lib/vgmsnd/3812intf.h"
//#include "fmopl.h"


typedef struct _ym3812_state ym3812_state;
struct _ym3812_state
{
	void *			chip;
};


extern INT32 CHIP_SAMPLE_RATE;

#define MAX_CHIPS	0x02
static ym3812_state YM3812Data[MAX_CHIPS];
static stream_sample_t* DUMMYBUF[0x02] = {NULL, NULL};

void ym3812_stream_update(UINT8 ChipID, stream_sample_t **outputs, int samples)
{
	ym3812_state *info = &YM3812Data[ChipID];
	ym3812_update_one(info->chip, outputs, samples);
}

static void _stream_update(void * param/*, int interval*/)
{
	ym3812_state *info = (ym3812_state *)param;
	ym3812_update_one(info->chip, DUMMYBUF, 0);
}


//static DEVICE_START( ym3812 )
int device_start_ym3812(UINT8 ChipID, int clock)
{
	ym3812_state *info;
	int rate;

	if (ChipID >= MAX_CHIPS)
		return 0;

	info = &YM3812Data[ChipID];
	//rate = clock /72;
	rate = CHIP_SAMPLE_RATE;

	info->chip = ym3812_init(clock, rate);

	/* YM3812 setup */
	ym3812_set_timer_handler (info->chip, NULL, info);
	ym3812_set_irq_handler   (info->chip, NULL, info);
	ym3812_set_update_handler(info->chip, _stream_update, info);

	return rate;
}

void device_stop_ym3812(UINT8 ChipID)
{
	ym3812_state *info = &YM3812Data[ChipID];
	ym3812_shutdown(info->chip);
}

void device_reset_ym3812(UINT8 ChipID)
{
	ym3812_state *info = &YM3812Data[ChipID];
	ym3812_reset_chip(info->chip);
}


UINT8 ym3812_r(UINT8 ChipID, offs_t offset)
{
	ym3812_state *info = &YM3812Data[ChipID];
	return ym3812_read(info->chip, offset & 1);
}

void ym3812_w(UINT8 ChipID, offs_t offset, UINT8 data)
{
	ym3812_state *info = &YM3812Data[ChipID];
	ym3812_write(info->chip, offset & 1, data);
}
