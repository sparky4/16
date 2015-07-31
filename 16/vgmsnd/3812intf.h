#pragma once

void ym3812_stream_update(UINT8 ChipID, INT32 **outputs, int samples);
int device_start_ym3812(UINT8 ChipID, int clock);
void device_stop_ym3812(UINT8 ChipID);
void device_reset_ym3812(UINT8 ChipID);

UINT8 ym3812_r(UINT8 ChipID, UINT32 offset);
void ym3812_w(UINT8 ChipID, UINT32 offset, UINT8 data);
