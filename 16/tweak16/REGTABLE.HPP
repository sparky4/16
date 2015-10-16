/*
	RegTable.HPP version 1.0
	by Robert Schmidt of Ztiff Zox Softwear 1993

	Declares the RegisterTable class and its members, defined in
		RegTable.CPP.
*/

#ifndef _RegTable_HPP
#define _RegTable_HPP

#include "Register.HPP"


class RegisterTable
	{
protected:
	NamedRegister *reg;
	int registers;
public:
	RegisterTable(istream &);
	~RegisterTable()			{ delete[] reg; }

	int getMaxReg()				{ return registers-1; }
	void doEnable();
	void doDisable();
	void in();
	void out();

    Register *getRegister(unsigned, unsigned char); 

	friend istream& operator>> (istream&, RegisterTable&);
	friend ostream& operator<< (ostream&, RegisterTable&);
	};

#endif