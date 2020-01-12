/* Project 16 Source Code~
 * Copyright (C) 2012-2020 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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

#include "src/lib/16_dbg.h"

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
