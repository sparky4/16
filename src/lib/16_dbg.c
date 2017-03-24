#include "src/lib/16_dbg.h"

#ifdef __DEBUG__
#ifdef __DEBUG_MM__
boolean dbg_debugmm=0;
#endif
#ifdef __DEBUG_PM__
boolean dbg_debugpm=0;
#endif
#ifdef __DEBUG_CA__
boolean dbg_debugca=0;
#endif
#ifdef __DEBUG_InputMgr__
boolean dbg_testkeyin=0,dbg_testcontrolnoisy=0,dbg_nointest=0;
#endif
#ifdef __DEBUG_MAP__
boolean dbg_maptext=0;
byte *dbg_mapdata;
#endif
#endif	//debug

#ifdef __WATCOMC__
// TODO: Could we also provide a build mode to emit debug to the "Bochs E9 hack?"
#ifdef DEBUGSERIAL
# include <stdarg.h>
# include <stdlib.h>
# include <stdio.h>

unsigned char _DEBUG_INITed = 0;
struct info_8250 *_DEBUG_uart = NULL;

int _DEBUG_INIT() {
	if (!_DEBUG_INITed) {
		unsigned int i;
		uint16_t port;

		if (!init_8250()) return 0;

		// what does the BIOS say the serial ports are?
		probe_8250_bios_ports();
		for (i=0;i < bios_8250_ports;i++) {
			port = get_8250_bios_port(i);
			if (port == 0) continue;
			probe_8250(port);
		}

		// what about the standard serial ports?
		for (i=0;i < (sizeof(standard_8250_ports)/sizeof(standard_8250_ports[0]));i++) {
			port = standard_8250_ports[i];
			if (port == 0) continue;
			probe_8250(port);
		}

		// pick the first port, which is probably COM1
		if (base_8250_ports == 0) return 0; // FIXME: You know "base_8250_ports" is probably a bad variable name for the max entries in info_8250_port[]
		_DEBUG_uart = &info_8250_port[0];
		_DEBUG_INITed = 1;

		// init the COM port.
		// in DOSBox-X, the "log" mode will receive our text and print it into the log file
		// on real hardware, our text will likely go over a null modem cable to another PC running a serial terminal program like PuTTY or minicom.
		// if nothing is connected, then the bytes go off into the ether to get lost and life goes on.
		uart_8250_enable_interrupt(_DEBUG_uart,0);	// disable interrupts
		uart_8250_set_FIFO(_DEBUG_uart,0x07);		// enable FIFO (why not?), also clear xmit/recv FIFO buffers, set threshhold to 1 byte
		uart_8250_set_MCR(_DEBUG_uart,3);		// RTS and DTS on
		uart_8250_set_line_control(_DEBUG_uart,UART_8250_LCR_8BIT | UART_8250_LCR_PARITY); // 8 bit 1 stop bit odd parity
		uart_8250_set_baudrate(_DEBUG_uart,uart_8250_baud_to_divisor(_DEBUG_uart,9600)); // 9600 baud
	}

	return _DEBUG_INITed;
}

void _DEBUG(const char *msg) {
	if (_DEBUG_uart != NULL) {
		char c;

		while ((c=(*msg++)) != 0/*NUL*/) {
			while (!uart_8250_can_write(_DEBUG_uart)); // wait for the UART to indicate readiness for our output
			uart_8250_write(_DEBUG_uart,(uint8_t)c); // then write it
		}
	}
}

static char _DEBUGF_TMP[256];

void _DEBUGF(const char *fmt,...) {
	va_list va;

	va_start(va,fmt);
	vsnprintf(_DEBUGF_TMP,sizeof(_DEBUGF_TMP),fmt,va);
	_DEBUG(_DEBUGF_TMP);
	va_end(va);
}
#endif	//serial
#endif	//watcomc
/*
================
=
= ShapeTest
=
================
*/

#pragma warn -pia
void ShapeTest (global_game_variables_t *gvar)
{
extern	word	NumDigi;
extern	word	_seg *DigiList;
static	char	buf[10];

	boolean			done;
	ScanCode		scan;
	int				i,j;
//	dword		l; word	k,x;
	memptr			addr;
	PageListStruct	far *page;

//	CenterWindow(20,16);
//	VW_UpdateScreen();
	for (i = 0,done = false;!done;)
	{
//		US_ClearWindow();
//		sound = -1;

		page = &(gvar->pm.PMPages[i]);
//		US_Print(" Page #");
//		US_PrintUnsigned(i);
		printf(" Page #%u", i);
//++		if (i < (gvar->pm.fi.PMSpriteStart))
//			US_Print(" (Wall)");
//++			printf(" (Wall)");
/*		else if (i < (gvar->pm.fi.PMSoundStart))
//			US_Print(" (Sprite)");
			printf(" (Sprite)");
		else if (i == (gvar->pm.fi.ChunksInFile - 1))
//			US_Print(" (Sound Info)");
			printf(" (Sound Info)");
		else
//			US_Print(" (Sound)");
			printf(" (Sound)");*/

//		US_Print("\n XMS: ");
		printf("\n XMS: ");
		if (page->xmsPage != -1)
//			US_PrintUnsigned(page->xmsPage);
			printf("%u", page->xmsPage);
		else
//			US_Print("No");
			printf("No");

//		US_Print("\n Main: ");
		if (page->mainPage != -1)
//			US_PrintUnsigned(page->mainPage);
			printf("\n Main: %u", page->mainPage);
		else if (page->emsPage != -1)
		{
//			US_Print("EMS ");
//			US_PrintUnsigned(page->emsPage);
			printf("EMS %u", page->emsPage);
		}
		else
//			US_Print("No");
			printf("No");

//		US_Print("\n Last hit: ");
//		US_PrintUnsigned(page->lastHit);
		printf("\n Last hit: %u", page->lastHit);

//		US_Print("\n Address: ");
		printf("\n Address: ");
		addr = PM_GetPageAddress(i, gvar);
		sprintf(buf,"0x%04x",(word)addr);
//		US_Print(buf);
		printf("%s", buf);

		if (addr)
		{
/*			if (i < PMSpriteStart)
			{
			//
			// draw the wall
			//
				bufferofs += 32*SCREENWIDTH;
				postx = 128;
				postwidth = 1;
				postsource = ((long)((unsigned)addr))<<16;
				for (x=0;x<64;x++,postx++,postsource+=64)
				{
					wallheight[postx] = 256;
					FarScalePost ();
				}
				bufferofs -= 32*SCREENWIDTH;
			}
			else if (i < PMSoundStart)
			{
			//
			// draw the sprite
			//
				bufferofs += 32*SCREENWIDTH;
				SimpleScaleShape (160, i-PMSpriteStart, 64);
				bufferofs -= 32*SCREENWIDTH;
			}
			else if (i == ChunksInFile - 1)
			{
				US_Print("\n\n Number of sounds: ");
				US_PrintUnsigned(NumDigi);
				for (l = j = k = 0;j < NumDigi;j++)
				{
					l += DigiList[(j * 2) + 1];
					k += (DigiList[(j * 2) + 1] + (PMPageSize - 1)) / PMPageSize;
				}
				US_Print("\n Total bytes: ");
				US_PrintUnsigned(l);
				US_Print("\n Total pages: ");
				US_PrintUnsigned(k);
			}
			else
			{
				byte far *dp = (byte far *)MK_FP(addr,0);
				for (j = 0;j < NumDigi;j++)
				{
					k = (DigiList[(j * 2) + 1] + (PMPageSize - 1)) / PMPageSize;
					if
					(
						(i >= PMSoundStart + DigiList[j * 2])
					&&	(i < PMSoundStart + DigiList[j * 2] + k)
					)
						break;
				}
				if (j < NumDigi)
				{
//					sound = j;
					US_Print("\n Sound #");
					US_PrintUnsigned(j);
					US_Print("\n Segment #");
					US_PrintUnsigned(i - PMSoundStart - DigiList[j * 2]);
				}
				for (j = 0;j < page->length;j += 32)
				{
					byte v = dp[j];
					int v2 = (unsigned)v;
					v2 -= 128;
					v2 /= 4;
					if (v2 < 0)
						VWB_Vlin(WindowY + WindowH - 32 + v2,
								WindowY + WindowH - 32,
								WindowX + 8 + (j / 32),BLACK);
					else
						VWB_Vlin(WindowY + WindowH - 32,
								WindowY + WindowH - 32 + v2,
								WindowX + 8 + (j / 32),BLACK);
				}
			}*/
			printf("\naddr ok\n");
		}

//		VW_UpdateScreen();

		while (!(scan = gvar->in.inst->LastScan))
		{}
//			SD_Poll();

//if(IN_KeyDown(sc_Escape)) break;

		IN_ClearKey(scan);
		switch (scan)
		{
		case sc_LeftArrow:
			if (i)
				i--;
			break;
		case sc_RightArrow:
			if (++i >= (gvar->pm.fi.ChunksInFile))
				i--;
			break;
		case sc_W:	// Walls
			i = 0;
			break;
		case sc_S:	// Sprites
//			i = (gvar->pm.fi.PMSpriteStart);
			break;
		case sc_D:	// Digitized
//			i = (gvar->pm.fi.PMSoundStart);
			break;
		case sc_I:	// Digitized info
			i = (gvar->pm.fi.ChunksInFile - 1);
			break;
		case sc_L:	// Load all pages
			for (j = 0;j < (gvar->pm.fi.ChunksInFile);j++)
				PM_GetPage(j, gvar);
			break;
		case sc_P:
//			if (sound != -1)
//				SD_PlayDigitized(sound);
			break;
		case sc_Escape:
			done = true;
			break;
		case sc_Enter:
			PM_GetPage(i, gvar);
			break;
		}
	}
	//SD_StopDigitized();
}
#pragma warn +pia
