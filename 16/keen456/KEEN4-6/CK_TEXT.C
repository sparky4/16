/* Reconstructed Commander Keen 4-6 Source Code
 * Copyright (C) 2021 K1n9_Duk3
 *
 * This file is primarily based on:
 * Wolfenstein 3-D Source Code
 * Copyright (C) 1992 id Software
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "CK_DEF.H"

/*
=============================================================================

TEXT FORMATTING COMMANDS
------------------------
^C<hex digit>  			Change text color
^E[enter]				End of layout (all pages)
^G<y>,<x>,<pic>[enter]	Draw a graphic and push margins
^P[enter]				start new page, must be the first chars in a layout
^L<x>,<y>[ENTER]		Locate to a specific spot, x in pixels, y in lines

=============================================================================
*/

/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/

#if GRMODE == CGAGR
#ifdef KEEN5
#define BACKCOLOR 2	// CGA magenta
#else
#define BACKCOLOR WHITE
#endif
#else
#define BACKCOLOR RED
#endif

#define WORDLIMIT 80
#define FONTHEIGHT 10
#define TOPMARGIN 10
#define BOTTOMMARGIN 10
#define LEFTMARGIN 10
#define RIGHTMARGIN 10
#define PICMARGIN 8
#define SPACEWIDTH 7
#define TEXTROWS ((200-TOPMARGIN-BOTTOMMARGIN)/FONTHEIGHT)
#define SCREENPIXWIDTH 320
#define SCREENMID (SCREENPIXWIDTH/2)

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

Sint16 pagenum,numpages;
Uint16 leftmargin[TEXTROWS],rightmargin[TEXTROWS];
char far *text;
Uint16 rowon;
Sint16 picx,picy,picnum,picdelay;
boolean layoutdone;

Sint16 helpmenupos;

//===========================================================================

/*
=====================
=
= RipToEOL
=
=====================
*/

void RipToEOL(void)
{
	while (*text++ != '\n');
}


/*
=====================
=
= ParseNumber
=
=====================
*/

Sint16 ParseNumber(void)
{
	char c, buffer[80];
	char *bufptr;

//
// scan until a number is found
//
	c = *text;
	while (c < '0' || c > '9')
		c = *++text;

//
// copy the number out
//
	bufptr = buffer;
	do
	{
		*bufptr = c;
		bufptr++;
		text++;
		c = *text;
	} while (c >= '0' && c <= '9');
	*bufptr = 0;

	return atoi(buffer);
}


/*
=====================
=
= ParsePicCommand
=
= Call with text pointing just after a ^P
= Upon exit text points to the start of next line
=
=====================
*/

void ParsePicCommand(void)
{
	picy = ParseNumber();
	picx = ParseNumber();
	picnum = ParseNumber();
	RipToEOL();
}

void ParseTimedCommand(void)
{
	picy = ParseNumber();
	picx = ParseNumber();
	picnum = ParseNumber();
	picdelay = ParseNumber();
	RipToEOL();
}

/*
=====================
=
= TimedPicCommand
=
= Call with text pointing just after a ^P
= Upon exit text points to the start of next line
=
=====================
*/

void TimedPicCommand(void)
{
	ParseTimedCommand();

//
// update the screen, and wait for time delay
//
#if GRMODE == CGAGR
	VW_UpdateScreen();
#else
	VW_WaitVBL(1);
	VW_ScreenToScreen(bufferofs, displayofs, 40, 200);
#endif

//
// wait for time
//
	TimeCount = 0;
	while (picdelay > TimeCount)
	;

//
// draw pic
//
	VWB_DrawPic(picx & ~7, picy, picnum);
}


/*
=====================
=
= HandleCommand
=
=====================
*/

void HandleCommand(void)
{
	Sint16 i,margin,top,bottom;
	Sint16 picwidth,picheight,picmid;

	switch (toupper(*(++text)))
	{
	case 'B':
		picy = ParseNumber();
		picx = ParseNumber();
		picwidth = ParseNumber();
		picheight = ParseNumber();
		VWB_Bar(picx, picy, picwidth, picheight, BACKCOLOR);
		RipToEOL();
		break;

	case 'P':		// ^P is start of next page, ^E is end of file
	case 'E':
		layoutdone = true;
		text--;
		break;

	case 'C':		// ^c<hex digit> changes text color
		i = toupper(*(++text));
		if (i >= '0' && i <= '9')
		{
			fontcolor = i + 0 - '0';
		}
		else if (i >= 'A' && i <= 'F')
		{
			fontcolor = i + 10 - 'A';
		}
#if GRMODE == CGAGR
		{
			static Sint16 colormap[16] = {2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0};
			// Note: This mapping is a bit problematic for Keen 5 CGA,
			// since some colors get mapped to CGA magenta, which is
			// used as the background color in that version. Luckily
			// those colors aren't used in the Keen 5 texts anyway.

			fontcolor = colormap[fontcolor];
		}
#endif
		fontcolor ^= BACKCOLOR;
		text++;
		break;

	case 'L':
		py = ParseNumber();
		rowon = (py - 10)/10;
		py = rowon * 10 + 10;
		px = ParseNumber();
		while (*(text++) != '\n')	// scan to end of line
		;
		break;

	case 'T':		// ^Tyyy,xxx,ppp,ttt waits ttt tics, then draws pic
		TimedPicCommand();
		break;

	case 'G':		// ^Gyyy,xxx,ppp draws graphic
		ParsePicCommand();
		VWB_DrawPic(picx & ~7, picy, picnum);
		picwidth = pictable[picnum-STARTPICS].width * BYTEPIXELS;
		picheight = pictable[picnum-STARTPICS].height;
		picmid = picx + picwidth/2;
		//
		// adjust margins
		//
		if (picmid > SCREENMID)
		{
			margin = picx-PICMARGIN;			// new right margin
		}
		else
		{
			margin = picx+picwidth+PICMARGIN;	// new left margin
		}
		top = (picy-TOPMARGIN)/FONTHEIGHT;
		if (top < 0)
		{
			top = 0;
		}
		bottom = (picy+picheight-TOPMARGIN)/FONTHEIGHT;
		if (bottom >= TEXTROWS)
		{
			bottom = TEXTROWS-1;
		}

		for (i=top; i<=bottom; i++)
		{
			if (picmid > SCREENMID)
			{
				rightmargin[i] = margin;
			}
			else
			{
				leftmargin[i] = margin;
			}
		}

		//
		// adjust this line if needed
		//
		if (leftmargin[rowon] > px)
		{
			px = leftmargin[rowon];
		}
		break;
	}
}


/*
=====================
=
= NewLine
=
=====================
*/

void NewLine(void)
{
	char c;

	if (++rowon == TEXTROWS)
	{
	//
	// overflowed the page, so skip until next page break
	//
		layoutdone = true;
		do
		{
			if (*text == '^')
			{
				c = toupper(text[1]);
				if (c == 'E' || c == 'P')
				{
					layoutdone = true;
					return;
				}
			}
			text++;
		} while (1);
	}
	px = leftmargin[rowon];
	py += FONTHEIGHT;
}


/*
=====================
=
= HandleCtrls
=
=====================
*/

void HandleCtrls(void)
{
	char c;

	c = *(text++);			// get the character and advance

	if (c == '\n')
	{
		NewLine();
		return;
	}
}

/*
=====================
=
= HandleWord
=
=====================
*/

void HandleWord(void)
{
	Uint16 wwidth, wheight, newpos, wordindex;
	char word[WORDLIMIT];

	//
	// copy the next word into [word]
	//
	word[0] = *(text++);
	wordindex = 1;
	while (*text > ' ')
	{
		word[wordindex] = *(text++);
		if (++wordindex == WORDLIMIT)
		{
			Quit("PageLayout: Word limit exceeded");
		}
	}
	word[wordindex] = 0;		// stick a null at end for C

	//
	// see if it fits on this line
	//
	VW_MeasurePropString(word, &wwidth, &wheight);
	
	while (rightmargin[rowon] < px+wwidth)
	{
		NewLine();
		if (layoutdone)
		{
			return;		// overflowed page
		}
	}

	//
	// print it
	//
	newpos = px+wwidth;
	VWB_DrawPropString(word);
	px = newpos;

	//
	// suck up any extra spaces
	//
	while (*text == ' ')
	{
		px += SPACEWIDTH;
		text++;
	}
}

/*
=====================
=
= PageLayout
=
= Clears the screen, draws the pics on the page, and word wraps the text.
= Returns a pointer to the terminating command
=
=====================
*/

void PageLayout(boolean shownumber)
{
	Sint16 oldcolor, i;
	char c;

	oldcolor = fontcolor;

#if GRMODE == CGAGR
	fontcolor = BLACK^BACKCOLOR;
#else
	fontcolor = YELLOW^BACKCOLOR;
#endif

//
// clear the screen
//
	VWB_Bar(0, 0, 320, 200, BACKCOLOR);
#ifndef KEEN6
	VWB_DrawPic(  0, 0, H_TOPWINDOWPIC);
	VWB_DrawPic(  0, 8, H_LEFTWINDOWPIC);
	VWB_DrawPic(312, 8, H_RIGHTWINDOWPIC);
	if (shownumber)
	{
		VWB_DrawPic(8, 176, H_BOTTOMINFOPIC);
	}
	else
	{
		VWB_DrawPic(8, 192, H_BOTTOMWINDOWPIC);
	}
#endif

	for (i=0; i<TEXTROWS; i++)
	{
		leftmargin[i] = LEFTMARGIN;
		rightmargin[i] = SCREENPIXWIDTH-RIGHTMARGIN;
	}

	px = LEFTMARGIN;
	py = TOPMARGIN;
	rowon = 0;
	layoutdone = false;

//
// make sure we are starting layout text (^P first command)
//
	while (*text <= ' ')
	{
		text++;
	}
	if (*text != '^' || toupper(*(++text)) != 'P')
	{
		Quit("PageLayout: Text not headed with ^P");
	}
	while (*(text++) != '\n')
	;

//
// process text stream
//
	do
	{
		c = *text;
		if (c == '^')
		{
			HandleCommand();
		}
		else if (c <= ' ')
		{
			HandleCtrls();
		}
		else
		{
			HandleWord();
		}
	} while (!layoutdone);

	pagenum++;

	if (shownumber)
	{
		strcpy(str, "pg ");
		itoa(pagenum, str2, 10);
		strcat(str, str2);
		strcat(str, " of ");
		itoa(numpages, str2, 10);
		strcat(str, str2);
#if GRMODE == CGAGR
		fontcolor = BLACK^BACKCOLOR;
#else
		fontcolor = LIGHTRED^BACKCOLOR;
#endif
		py = 186;
		px = 218;
		VWB_DrawPropString(str);
	}

	fontcolor = oldcolor;
}

//===========================================================================

/*
=====================
=
= BackPage
=
= Scans for a previous ^P
=
=====================
*/

void BackPage(void)
{
	pagenum--;
	do
	{
		text--;
		if (text[0] == '^' && toupper(text[1]) == 'P')
		{
			return;
		}
	} while (1);
}

//===========================================================================


/*
=====================
=
= CacheLayoutGraphics
=
= Scans an entire layout file (until a ^E) marking all graphics used, and
= counting pages, then caches the graphics in
=
=====================
*/
void CacheLayoutGraphics(void)
{
	char	far *bombpoint, far *textstart;
	char	ch;

	textstart = text;
	bombpoint = text+30000;
	numpages = pagenum = 0;

#ifndef KEEN6
	CA_MarkGrChunk(H_TOPWINDOWPIC);
	CA_MarkGrChunk(H_LEFTWINDOWPIC);
	CA_MarkGrChunk(H_RIGHTWINDOWPIC);
	CA_MarkGrChunk(H_BOTTOMINFOPIC);
	CA_MarkGrChunk(H_BOTTOMWINDOWPIC);
#endif

	do
	{
		if (*text == '^')
		{
			ch = toupper(*(++text));
			if (ch == 'P')		// start of a page
			{
				numpages++;
			}
			if (ch == 'E')		// end of file, so load graphics and return
			{
				CA_CacheMarks(NULL);
				text = textstart;
				return;
			}
			if (ch == 'G')		// draw graphic command, so mark graphics
			{
				ParsePicCommand();
				CA_MarkGrChunk(picnum);
			}
			if (ch == 'T')		// timed draw graphic command, so mark graphics
			{
				ParseTimedCommand();
				CA_MarkGrChunk(picnum);
			}
		}
		else
		{
			text++;
		}

	} while (text < bombpoint);

	Quit("CacheLayoutGraphics: No ^E to terminate file!");
}

//===========================================================================

#ifndef KEEN6
/*
=================
=
= HelpMenu
=
=================
*/
Sint16 HelpMenu(void)
{
	CursorInfo cursor;
	ControlInfo control;
	Sint16 ydelta;
	Uint16 key;

	VWB_Bar(0, 0, 320, 200, BACKCOLOR);

	CA_CacheGrChunk(H_HELPPIC);
	CA_CacheGrChunk(H_HANDPIC);
	CA_CacheGrChunk(H_TOPWINDOWPIC);
	CA_CacheGrChunk(H_LEFTWINDOWPIC);
	CA_CacheGrChunk(H_RIGHTWINDOWPIC);
	CA_CacheGrChunk(H_BOTTOMWINDOWPIC);

	VWB_DrawPic(  0,   0, H_TOPWINDOWPIC);
	VWB_DrawPic(  0,   8, H_LEFTWINDOWPIC);
	VWB_DrawPic(312,   8, H_RIGHTWINDOWPIC);
	VWB_DrawPic(  8, 192, H_BOTTOMWINDOWPIC);
	VWB_DrawPic( 96,   8, H_HELPPIC);

	ydelta = 0;
	IN_ClearKeysDown();
	do
	{
		if (helpmenupos < 0)
		{
			helpmenupos = 0;
		}
#ifdef GOODTIMES
		else if (helpmenupos > 3)
		{
			helpmenupos = 3;
		}
#else
		else if (helpmenupos > 4)
		{
			helpmenupos = 4;
		}
#endif
		VWB_DrawPic(48, 24*helpmenupos+48, H_HANDPIC);
		VW_UpdateScreen();
		VWB_Bar(48, 24*helpmenupos+48, 39, 24, BACKCOLOR);
		IN_ReadControl(0, &control);
		IN_ReadCursor(&cursor);
		if (LastScan)
		{
			key = LastScan;
			IN_ClearKeysDown();
			switch (key)
			{
			case sc_UpArrow:
				helpmenupos--;
				break;
			case sc_DownArrow:
				helpmenupos++;
				break;
			case sc_Enter:
				VW_ClearVideo(BACKCOLOR);
				return helpmenupos;
			case sc_Escape:
				VW_ClearVideo(BACKCOLOR);
				return -1;
			}
		}
		ydelta += cursor.y;
		if (cursor.button0 || cursor.button1 || control.button0 || control.button1)
		{
			VW_ClearVideo(BACKCOLOR);
			return helpmenupos;
		}
		if (ydelta < -40)
		{
			ydelta += 40;
			helpmenupos--;
		}
		else if (ydelta > 40)
		{
			ydelta -= 40;
			helpmenupos++;
		}
	} while (1);
}

/*
=================
=
= HelpScreens
=
=================
*/
void HelpScreens(void)
{
	static Uint16 layouttable[5] =
	{
		T_HELPART,
		T_CONTRART,
		T_STORYART,
#ifndef GOODTIMES
		T_ORDERART,
#endif
		T_IDART
	};

	Uint16 olddisplayofs, oldbufferofs, oldfontnumber, temp;
	Sint16 pos;
	boolean newpage;

	oldfontnumber = fontnumber;
	olddisplayofs = displayofs;
	oldbufferofs = bufferofs;
	fontnumber = 0;

#if GRMODE == EGAGR
	EGAMAPMASK(15);
#endif

	CA_UpLevel();
	CA_SetGrPurge();
	VW_ClearVideo(BACKCOLOR);

#if GRMODE == EGAGR
	RF_FixOfs();
	bufferofs = 0;
	displayofs = 0x8000;
	VW_SetScreen(displayofs, 0);
#endif

#ifdef KEEN5
	StartMusic(19);
#endif

	do
	{
		pos = HelpMenu();

		VW_ClearVideo(BACKCOLOR);

		if (pos == -1)
		{
			CA_DownLevel();
			IN_ClearKeysDown();
			bufferofs = oldbufferofs;
			displayofs = olddisplayofs;
			fontnumber = oldfontnumber;
			VW_ClearVideo(BACKCOLOR);
			RF_FixOfs();
#ifdef KEEN5
			StopMusic();	// Note: it's safer to call StopMusic BEFORE CA_DownLevel
#endif
			return;
		}

		pos = layouttable[pos];
		CA_CacheGrChunk(pos);
		text = grsegs[pos];
		CacheLayoutGraphics();

		newpage = true;
		do
		{
			if (newpage)
			{
				newpage = false;
				PageLayout(true);
#if GRMODE == CGAGR
				VW_UpdateScreen();
#else
				VW_SetScreen(bufferofs, 0);
				temp = displayofs;
				displayofs = bufferofs;
				bufferofs = temp;
#endif
			}

			LastScan = 0;
			while (!LastScan);

			switch (LastScan)
			{
			case sc_UpArrow:
			case sc_LeftArrow:
			case sc_PgUp:
				if (pagenum > 1)
				{
					BackPage();
					BackPage();
					newpage = true;
				}
				break;
			case sc_DownArrow:
			case sc_RightArrow:
			case sc_PgDn:
				if (pagenum < numpages)
				{
					newpage = true;
				}
				break;
			}
		} while (LastScan != sc_Escape);

		MM_FreePtr(&grsegs[pos]);
		IN_ClearKeysDown();
	} while (true);
}

#endif

//===========================================================================

/*
=================
=
= FinaleLayout
=
=================
*/
void FinaleLayout(void)
{
	char _seg *textseg;
	Sint16 i;

	VW_ClearVideo(BACKCOLOR);
	RF_FixOfs();
	CA_UpLevel();
	CA_SetGrPurge();
	CA_CacheGrChunk(H_FLASHARROW2PIC);
	CA_CacheGrChunk(H_FLASHARROW1PIC);

#ifdef KEEN5
	if (gamestate.leveldone[13] == ex_fusebroke)
	{
		CA_CacheGrChunk(T_ENDART2);
		textseg = grsegs[T_ENDART2];
	}
	else
	{
		CA_CacheGrChunk(T_ENDART);
		textseg = grsegs[T_ENDART];
	}
#else
	CA_CacheGrChunk(T_ENDART);
	textseg = grsegs[T_ENDART];
#endif

	text = textseg;
	CacheLayoutGraphics();

	StartMusic(ENDINGMUSIC);

	while (pagenum < numpages)
	{
		PageLayout(false);
		IN_ClearKeysDown();
#if GRMODE == CGAGR
		VW_UpdateScreen();
#else
		VW_SetScreen(bufferofs, 0);
#endif

		do
		{
			VWB_DrawPic(298, 184, H_FLASHARROW1PIC);
#if GRMODE == CGAGR
			VW_UpdateScreen();
#endif
			for (i=0; i<TickBase; i++)
			{
				if (IN_IsUserInput())
				{
					goto nextpage;
				}
				VW_WaitVBL(1);
			}

			VWB_DrawPic(298, 184, H_FLASHARROW2PIC);
#if GRMODE == CGAGR
			VW_UpdateScreen();
#endif
			for (i=0; i<TickBase; i++)
			{
				if (IN_IsUserInput())
				{
					goto nextpage;
				}
				VW_WaitVBL(1);
			}
		} while (1);

nextpage:
		;	// Borland C++ 2.0 needs a semicolon here...
	}

	StopMusic();

#ifdef KEEN5
	if (gamestate.leveldone[13] == ex_fusebroke)
	{
		MM_FreePtr(&grsegs[T_ENDART2]);
	}
	else
	{
		MM_FreePtr(&grsegs[H_FLASHARROW1PIC]);	// BUG! this should free T_ENDART, the arrow should be freed after the else branch!
	}
#else
	MM_FreePtr(&grsegs[T_ENDART]);
	MM_FreePtr(&grsegs[H_FLASHARROW1PIC]);
#endif
	MM_FreePtr(&grsegs[H_FLASHARROW2PIC]);
	CA_DownLevel();
	IN_ClearKeysDown();
#if GRMODE != CGAGR
	VW_ClearVideo(BACKCOLOR);
	RF_FixOfs();
#endif
	CA_FreeGraphics();
}