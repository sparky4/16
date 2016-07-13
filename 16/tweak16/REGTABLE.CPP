/*
	RegTable.CPP version 1.0
	by Robert Schmidt of Ztiff Zox Softwear 1993
	
	Defines the member and friend functions of the RegisterTable class
		declared in RegTable.HPP.
*/

#include <dos.h>
//#include <conio.h>
#include "RegTable.hpp"


RegisterTable::RegisterTable(istream &ins)
	{
	// Read number of registers (first number in file)
	ins >> registers;
	// Allocate enough space
	reg = new NamedRegister[registers];
	// Read reg. descriptions
	for (int i=0; i<registers; i++)
		ins >> reg[i];
	// Get current register configuration from VGA, and use as default
	in();
	}


void RegisterTable::in()
	{
	for (int r = 0; r < registers; r++)
		reg[r].in();
	}

void RegisterTable::out()
	{
	outportb(0x3d4,0x11);				// Ensure CRT regs. 0-7 are writable!
	int v = inportb(0x3d5);				//	That is, clear bit 7 of port
	v &= 0x7f;							//	0x3D4, index 0x11.
	outport(0x3d4,0x11 | (v << 8));

//	outport(0x3c4, 0x0100);				// reset sequencer

	for (int r = 0; r < registers; r++)
		if (reg[r].isEnabled())
			reg[r].out();

//	outport(0x3c4, 0x0300);				// re-reset sequencer
	outportb(0x3c0, 0x20);				// Reenable display data
	}

/*
	This istream operator >> reads an entire table of Register values
	into 'this' table.
	Notes:
		The stream is read until good() is no longer true.  Not good
			practice, but hey!
		If the read Register's port and index pair is not found in 'this'
			table, it is ignored.
		In effect, only the *values* in the table may change after >>,
			not port or index numbers.
*/

istream& operator>> (istream &in, RegisterTable &t)
	{
	int r = 0;
	t.doDisable();							// first disable all registers
	while (in.good())
		{
		Register temp;
		in >> temp;
	
		int prevr = r;
	
		//Search for the correct register position:
		while (temp.getPort() != t.reg[r].getPort() ||
			temp.getIndex() != t.reg[r].getIndex())
			{
			if (++r >= t.registers)
				r = 0;
			if (r == prevr)					// Have we looped around once?
				goto skip;					//	Register not supported!
			}
		// Correct register found, now store the value and enable it.
		t.reg[r].setValue(temp.getValue());
		t.reg[r].doEnable();				// enable this single register
	skip:
		}
	return in;
	}


//	This operator << sends all enabled registers in t to the out stream.

ostream& operator<< (ostream &out, RegisterTable &t)
	{
	for (int r = 0; r < t.registers; r++)
		if (t.reg[r].isEnabled())
			out << Register(t.reg[r]);
	return out;
	}

void RegisterTable::doEnable()
	{
	for (int r=0; r<registers; r++)
		reg[r].doEnable();
	}

void RegisterTable::doDisable()
	{
	for (int r=0; r<registers; r++)
		reg[r].doDisable();
	}


Register *RegisterTable::getRegister(unsigned p, unsigned char i)
	{
	for (int r = 0; r < registers; ++r)
		if (reg[r].getPort() == p && reg[r].getIndex() == i)
			return &reg[r];
	return 0;
	}


