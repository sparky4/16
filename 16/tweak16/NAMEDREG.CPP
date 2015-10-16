/*
	NamedReg.CPP version 1.0
	by Robert Schmidt of Ztiff Zox Softwear 1993

	Defines the member functions of the NamedRegister class declared in
		Register.hpp.
	Defines the stream operator >> to read named register info from
		an istream.  (Text mode)
*/

#include <conio.h>
#include <iostream.h>
#include <string.h>
#include "Screen.hpp"
#include "Register.hpp"


// NamedRegister::printCon() prints the register state to the console.
//	If enableFlag is zero, the value is omitted from the display, and
//	its place is filled with spaces.

void NamedRegister::printCon()
	{
	textattr(enableFlag?REGENABLE_COLOR:REGDISABLE_COLOR);
	cprintf("%03hx (%02hx) %24s : %02hx", getPort(), getIndex(),
		name, getValue());
	}

/*
	This operator reads the register port number, index and name from the
	input stream.  (*Not* the value!)  Used for initializing each element
	in the register table used by TWEAK.
*/

istream& operator>> (istream &in, NamedRegister &r)
	{
	int i;
	char *n = new char[128];

	in >> hex >> i;
	r.setPort(i);
	in >> i >> ws;
	r.setIndex((unsigned char)(i));
	in.getline(n, 128);
	n[in.gcount()-1] = '\0';
	r.name = new char[strlen(n)+1];
	strcpy(r.name, n);
	delete[] n;

	return in;
	}

