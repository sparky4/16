/*
	RegEdit.HPP

*/

#include <conio.h>
#include "Screen.hpp"
#include "RegEdit.HPP"


RegisterEditor::RegisterEditor(istream &ins)
	: RegisterTable(ins)
	{
	prevSel = select = 0;
	}

void RegisterEditor::printCon(int r)
	{
	// This gotoxy divides the registers into two columns.
	gotoxy(40*(r / editHeight) +1, r % editHeight +1);
	// Optionally print the left cursor.
	textattr(CURSOR_COLOR);
	cprintf(r==select ? "\20" : " ");
	// Then put out the meat.
	reg[r].printCon();
	// And possibly the right cursor.
	textattr(CURSOR_COLOR);
	cprintf(r==select ? "\21" : " ");
	// This gotoxy just puts the hardware cursor where it won't distract you.
	gotoxy(40*(r / editHeight)+38, r % editHeight +1);
	}

void RegisterEditor::printAllCon()
	{
	for (int r = 0; r < registers; r++)
		printCon(r);
	}

int RegisterEditor::updateSelect()
	{
	if (select < 0)
		select = registers - 1;
	else
		if (select >= registers)
			select = 0;
	if (prevSel != select)
		{
		printCon(prevSel);
		prevSel = select;
		}
	printCon(select);
	return select;
	}


// RegisterEditor::showBitMask() updates the bit pattern display with
//	the value of selected register.  This is TWEAK specific.

void RegisterEditor::showBitMask()
	{
	gotoxy(42,editHeight-4);
	textattr(BITHEADER_COLOR);
	cprintf("Bit mask: 7 6 5 4 3 2 1 0");
	gotoxy(51,editHeight-3);
	textattr(BITPATTERN_COLOR);
	unsigned char v = reg[select].getValue();
	for (int e=7; e>=0; e--)
		cprintf( v&(1<<e) ? " 1" : " 0");
	}

