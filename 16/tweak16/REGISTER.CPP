/*
	Register.CPP version 1.0
	by Robert Schmidt of Ztiff Zox Softwear 1993

	Defines the member functions of the Register class declared in
		Register.hpp.
	Defines the stream operators >> and << to read and write register
		info from istreams/to ostreams in *binary* format.
*/

#include <dos.h>
#include <iostream.h>
#include "Register.hpp"


/*
	Register::out()
		Outputs the Register.value to the correct port/index.
		It takes care of handling recognized special cases, like some
		VGA ports, correctly.
*/

void Register::out()
	{
	switch (port)
		{
		// First handle special cases:

		case ATTRCON_ADDR:
			inportb(STATUS_ADDR);  		// reset read/write flip-flop
			outportb(ATTRCON_ADDR, index);
			outportb(ATTRCON_ADDR, value);
			break;

		case SEQ_ADDR:
			if (index == 1)
				{
				// Reset the sequencer if the clock polarity settings
				//  are being accessed.
				outport(SEQ_ADDR, 0x0100);
				outport(SEQ_ADDR, value<<8 | 1);
				outport(SEQ_ADDR, 0x0300);
				break;
				}
		case GRACON_ADDR:
		case CRTC_ADDR:
		case CHIPSTECH:
			outport(port, index | value<<8);
			break;

		case MISC_ADDR:
		case VGAENABLE_ADDR:
		default:						// Default is to write the byte
			outportb(port, value);		//	directly to the port
			break;
		}
	}

/*
	Register::in()
		Inputs Register.value to the associated hardware register.
		It takes care of handling recognized special cases,
		like some VGA ports, correctly.
*/

unsigned char Register::in()
	{
	switch (port)
		{
		// First handle special cases:

		case MISC_ADDR:
			value = inportb(0x3cc);		// 0x3c2 is write-only, reading
										// must be mapped to 0x3cc
			break;

		case ATTRCON_ADDR:				// This 1 is odd.  First do a read to
			inportb(STATUS_ADDR);		//	reset the index/data flip-flop.
										//	Then give it the index, but:
										//	set bit 5!  If cleared, VGA
										//	output is disabled!
			outportb(ATTRCON_ADDR, index);
			value = inportb(ATTRCON_ADDR+1);
			break;

		case SEQ_ADDR:					// These 3 are similar.  Give it the
		case GRACON_ADDR:				//	register index, then read the
		case CRTC_ADDR:					//	byte from port+1.
		case CHIPSTECH:
			outportb(port, index);
			value = inportb(port+1);
			break;

		case VGAENABLE_ADDR:
		default:                        // Default is to read the byte
			value = inportb(port);		//	directly from the port
			break;
		}

	return value;							// Return value of first reg.
	}

/*
	The following stream operators operate on Registers in *binary*
	format, i.e. they are not suitable for communicating with text streams
	like the keyboard or the screen (cin/cout).
*/

istream& operator>> (istream &in, Register &r)
	{
	r.port = unsigned(in.get()) | (unsigned(in.get()) << 8);
	r.index = in.get();
	r.value = in.get();

	return in;
	}

ostream& operator<< (ostream &out, Register &r)
	{
	out.put(char(r.port));
	out.put(char(r.port>>8));
	out.put(r.index);
	out.put(r.value);

	return out;
	}



