/*
	Register.hpp version 1.0
	by Robert Schmidt of Ztiff Zox Softwear 1993

	Declares the Register class, members defined in Register.cpp.
*/

#ifndef _Register_HPP
#define _Register_HPP

#include <iostream.h>

/*
	xxxxADDR defines the base port number used to access VGA component xxxx,
	and is defined for xxxx =
		ATTRCON		-	Attribute Controller
		MISC		-	Miscellaneous Register
		VGAENABLE	-	VGA Enable Register
		SEQ			-	Sequencer
		GRACON		-	Graphics Controller
		CRTC		-	Cathode Ray Tube Controller
		STATUS		-	Status Register
*/

#define ATTRCON_ADDR	0x3c0
#define MISC_ADDR		0x3c2
#define VGAENABLE_ADDR	0x3c3
#define SEQ_ADDR		0x3c4
#define GRACON_ADDR		0x3ce
#define CRTC_ADDR		0x3d4
#define STATUS_ADDR		0x3da

// SVGA specific registers here:

#define CHIPSTECH               0x3d6


class	Register
	{
	unsigned		port;
	unsigned char	index;
	unsigned char	value;
public:
	Register(unsigned p=0, unsigned char i=0, unsigned char v=0)
		{ init(p,i,v); }
	unsigned char init(unsigned p, unsigned char i, unsigned char v)
		{ port = p; index = i; return value = v; }
	unsigned char init(Register& r)
		{ port = r.port; index = r.index; return value = r.value; }
	void setPort(unsigned p)		{ port = p; }
	void setIndex(unsigned char i)	{ index = i; }
	void setValue(unsigned char v)	{ value = v; }
	unsigned getPort(void) const	{ return port; }
	unsigned char getIndex(void) const { return index; }
	unsigned char getValue(void) const { return value; }
	unsigned char operator++(void)	{ return value++; }
	unsigned char operator--(void)	{ return value--; }
	unsigned char operator++(int)	{ return ++value; }
	unsigned char operator--(int)	{ return --value; }
	unsigned char &operator *()		{ return value; }
	unsigned char operator=(Register &r)
		{ return init(r); }
	unsigned operator() (int rbit, int len)
    	{ return (value >> rbit) & (1 << len)-1; }

	void out();
	unsigned char in();

	friend istream& operator>> (istream&, Register&);
	friend ostream& operator<< (ostream&, Register&);
	};


class NamedRegister : public Register
	{
	char 			*name;
	char			enableFlag;
public:
	NamedRegister()					{ doEnable(); }
	~NamedRegister()				{ delete name; }
	void doEnable()					{ enableFlag = 1; }
	void doDisable()				{ enableFlag = 0; }
	void toggleEnable()				{ enableFlag = !enableFlag; }
	char isEnabled()				{ return enableFlag; }
	char *getName()					{ return name; }
	void printCon();

	friend istream& operator>> (istream&, NamedRegister&);
	};

#endif
