#ifndef _REGEDIT_HPP
#define _REGEDIT_HPP

#include "RegTable.hpp"

class RegisterEditor : public RegisterTable
	{
	int select, prevSel;
public:
	RegisterEditor(istream &);
	int operator++()			{ return setSelect(select+1); }
	int operator--()			{ return setSelect(select-1); }
	NamedRegister& operator* (void) { return reg[select]; }
	int getSelect()				{ return select; }
	void printCon(int);
	void printAllCon();
	int updateSelect();
	int setSelect(int s)		{ select = s; return updateSelect(); }
	void showBitMask();
	};

#endif