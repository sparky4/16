/*
	TWEAK 1.6á - Mold your own VGA modes

	by Robert Schmidt of Ztiff Zox Softwear, (C) Copyright 1992-93

	For documentation, see TWEAK.DOC.


	History:

	0.9
		First version available to the public.
		Aw, what the heck, I'll admit this is the first version ever...
		I know the usage of C++ classes may look a little stupid,
			but it works.
		Another stupid, *stupid* thing is the mixed use of conio and
			iostream, but hey, it works too!
		This version is not commented whatsoever.
		Pretty lame interface - no help.
		Test screens assume text start at segment 0xb800, and graphics
		at 0xa000.
	1.0
		Changed from small to large memory model.  Not that I need it, it
			just makes the code more readable.
		Commented heavily, and beautified the code.
		Nearly rewrote the program completely, by using another OO strategy.
		Changed use of abort() to exit(1).
		Text test pattern now fills entire text buffer from 0xb800:0x0000
			to 0xb800:0xffff.
		TWEAK now captures whatever screen mode is active at startup,
			instead of defaulting to BIOS mode 3 (80x25).
		The screen is reset to the startup BIOS mode at exit, too.
		Added 'S' and 'L' as synonyms for F9 and F10.
		Added Shift+TAB as a companion to TAB.
		Editing screen now uses 80x50 instead of 80x25.
		Added one text test screen which uses the 8x8 font.
		Added online help, not context sensitive as promised.  Maybe later.
		Edititing screen is cleared when a test is initiated, so that it
			is easier to see that something happened if the wrong test
			screen is selected.
		Removed startup banner.
		Removed use of cout and cin for screen/keyboard I/O, in favour of
			the colors and windows provided by conio.h.
		Instead, I use fstreams for *file* I/O... much more convenient than
			FILEs.
		Made the interface more cheerful by using lots of colors.
	1.1
		Nuked a serious bug in the text test pattern.  At the end, it
			wrote characters far outside of the video buffer, overwriting
			0x800 bytes at the start of the 0xC000 segment.
			Most PCs have ROM in this area, so I didn't notice the
			bug until I ran TWEAK on some 486s with a SCSI harddisk.
			I think those map some RAM to that segment, in any case
			TWEAK crashed whenever the text pattern was used on those
			machines.
		I Decided not to read back the registers when returning from the
			test screen.  It was not very usable, and imposed some
			problems when using different test screens.
		Never released.
	1.5
		Added the AutoDetect testing pattern, which makes use of my new,
			mode independant VGA graphics library to make the testing
			much more interesting and informative.  Might add simple
			menus to this later.
		Fixed a bug in the VGA library for Chained 256-color modes, which
			caused scrolling to go four times as far as intended.
		Made the help window larger, more readable.
		Added question of confirmation to quit when ESC is pressed.
		Added lots more sample modes (from xlib06).
	1.6
		Added the mode heuristics seen in the autodetect test screen to
			the editor screen.  This gives the user instant feedback on
			his work.
		Fixed the RegTable::out() method to correctly reset the sequencer.
			Result: no flickering when testing modes.
		The autodetect screen sets the VGA palettes to the BIOS default.
*/

#ifndef __LARGE__
# ifndef __COMPACT__
#  ifndef __HUGE__
#   error A large data model is required!
#  endif
# endif
#endif


#include <stdio.h>
#include <dos.h>
#include <stdlib.h>
#include <conio.h>
#include <fstream.h>
#include <ctype.h>
#include <string.h>

#include "Screen.HPP"
#include "RegEdit.HPP"
#include "TestPat.HPP"
#include "detect.hpp"


void helpWindow()
	{
	tempBuffer swap(textScr);
	swap.save();
	window(5,5,editWidth-5,33);
	textattr(HELP_COLOR);
	clrscr();
	window(6,6,editWidth-6,32);
	gotoxy(1,1);
	cprintf("TWEAK 1.6á by Robert Schmidt - Ztiff Zox Softwear\n\r"
			"Quick reference help screen\n\r"
			"\n\r"
			"Up/Down/Home/End: select register\n\r"
			"       Backspace: enable/disable register\n\r"
			"    2 hex digits: set register value\n\r"
			"             -/+: decrease/increase register value\n\r"
			"           F1-F8: toggle register bits 7-0\n\r"
			"       F9 or 'S': save register set to file\n\r"
			"      F10 or 'L': load set from file\n\r"
			"             'M': select BIOS mode to work from\n\r"
			"   TAB/Shift-TAB: select test screen\n\r"
			"           ENTER: show test screen\n\r"
			"             'H': show this help window\n\r"
			"             ESC: Quit TWEAK immediately\n\r"
			"\n\r"
			"The mode heuristics shown are NOT perfect.  It is practically\n\r"
			"impossible to foresee what your screen will show.  Use the\n\r"
			"values given as guidance, or hints.\n\r"
			"\n\r"
			"When viewing the autodetect test screen, you may use the\n\r"
			"arrows to scroll over the virtual screen.  Press ESC or ENTER\n\r"
			"to leave.\n\r"
			"\n\r"
			"For more information, see the TWEAK.DOC file.\n\r"
			"Now press the 'any' key to return to the editor.\n\r"
			);
	getch();
	window(1,1,editWidth,editHeight);
	swap.restore();
	}


void prompt(char *p = 0, int attr = PROMPT_COLOR)
	{
	textattr(7);
	gotoxy(42,editHeight-1);
	clreol();
	textattr(attr);
	if (p)
		cprintf(p);
	}


int main(int argc, char **argv)
	{
	// Initialize the register table with descriptions and all.
	RegisterEditor regEditor(ifstream("TWEAK.DAT"));
	TestPatterns test;

	int parentBiosMode = getBiosMode();
	preparePalettes();
	// Set our default editing screen, and get its dimensions
	setBiosMode(3);
	textmode(C4350);
	clrscr();
	text_info *ti = new text_info;
	gettextinfo(ti);
	editMode = ti->currmode;
	editHeight = ti->screenheight;
	editWidth = ti->screenwidth;
	editSize = editHeight * editWidth;
	delete ti;

	tempBuffer swap(textScr);	// Establish the temporary screen buffer

	unsigned char quit = 0;			// Non-zero when a quit command is
									//	initiated.
	int key;						// The last key pressed.

	// Build the editing screen:

	regEditor.printAllCon();
	gotoxy(42, editHeight-20);
	textattr(HELP_COLOR);
	cprintf("(Press H if you need help)");

	ModeInfo minfo(regEditor);

	// Start the main keyboard polling loop:

	while (!quit)
		{
		test.tellTest();
		regEditor.showBitMask();
		minfo.show();
		regEditor.updateSelect();

		int key = getch();
		if (!key)
			key = getch() << 8;
		else
			key = tolower(key);

keyentry:
		switch (key)
			{
			case 0x4700:			//HOME
				regEditor.setSelect(0);
				break;
			case 0x4800:			//UP
				--regEditor;
				break;
			case 0x4F00:			//END
				regEditor.setSelect(regEditor.getMaxReg());
				break;
			case 0x5000:			//DOWN
				++regEditor;
				break;

			// Function keys - toggle single bit in selected reg.
			case 0x3B00:			//F1
			case 0x3C00:
			case 0x3D00:
			case 0x3E00:
			case 0x3F00:
			case 0x4000:
			case 0x4100:
			case 0x4200:			//F8
				**regEditor ^= (1<<7-((key>>8)-0x3B));
				break;

			case 0x4300:			//F9
			case 0x4400:			//F10
				// Handle file operations (save/load):
				char fname[63];
				int save=(key==0x4300);

				// Prompt for filename.
				if (save)
					prompt("Save file name: ");
				else
					prompt("Load file name: ");
				gets(fname);
				if (strlen(fname) == 0)
					{
					prompt("File operation canceled.");
					break;
					}

				prompt(0, ERROR_COLOR);	// prepare for error
				if (save)
					{
					ofstream out(fname, ios::trunc|ios::binary|ios::out);
					if (out.bad())
						cprintf("Error creating '%s'!", fname);
					else
						{
						out << regEditor;
						if (out.bad())
							cprintf("Error writing '%s'!", fname);
						else
							{
							prompt(fname);
							cprintf(" written.");
							}
						}
					}
				else
					{
					ifstream in(fname, ios::binary|ios::in|ios::nocreate);
					if (in.bad())
						cprintf("Error opening '%s'!", fname);
					else
						{
						in >> regEditor;
						if (in.bad())
							cprintf("Error reading '%s'!", fname);
						else
							{
							prompt(fname);
							cprintf(" read.");
							}
						}
					}

				// Update screen to reflect changes (if any).
				regEditor.printAllCon();
				test.tellTest();
				break;

			case '0': case '1': case '2': case '3': case '4': case '5':
			case '6': case '7': case '8': case '9': case 'a': case 'b':
			case 'c': case 'd': case 'e': case 'f':
				// Input hexadecimal number:
				gotoxy(40*(regEditor.getSelect()/editHeight)+38,
					regEditor.getSelect()%editHeight+1);
				ungetch(key);
				cprintf("%c \b", key);
				unsigned char newValue;
				cscanf("%2hx", &newValue);
				(*regEditor).setValue(newValue);
				break;

			case 'h':
				// Help:
				helpWindow();
				break;

			// Alternate file I/O keys:
			case 's':
				key = 0x4300;
				goto keyentry;

			case 'l':
				key = 0x4400;
				goto keyentry;

			case 'm':

				// Select a BIOS mode to work from.
				int baseMode;
				prompt("Base BIOS screen mode: 0x");
				cscanf("%2hx",&baseMode);
				swap.save();				//save edit buffer

				// Set the selected mode, and grab register values.
				setBiosMode(baseMode);
				regEditor.in();
				swap.restore();				//reset edit mode and buffer
				regEditor.printAllCon();
				prompt();
				cprintf("Got registers from mode 0x%02x", baseMode);
				break;

			case 8:
				(*regEditor).toggleEnable();
				++regEditor;
				break;

			case '-':
				// Decrease register value:
				// Note that * (dereferencing) is overloaded for both
				//	RegisterTable and Register!  Pretty fancy, but hard
				//	to read!
				-- **regEditor;
				break;

			case '+':
				// Increase register value
				++ **regEditor;
				break;

			case 13:	//ENTER
				// Test the screen mode.
				swap.save();
				// Clear the text screen, so the user can see something is
				//	happening even if he chose the wrong test screen.
				clrscr();
				test.run(regEditor);
				swap.restore();
				break;

			case 9:		//TAB
				// Select next test pattern:
				++test;
				break;

			case 0x0F00: // shift-TAB
				// Select previous test pattern:
				--test;
				break;

			case 27:	//ESC
				// Quit TWEAK:
				prompt("Really quit [n]?");
				if (toupper(getch()) == 'Y')
					quit = 1;
				prompt();
				break;
			}
		minfo.detectFrom(regEditor);
		}

	// Reset BIOS mode detected at startup.
	setBiosMode(parentBiosMode);

	return 0;
	}