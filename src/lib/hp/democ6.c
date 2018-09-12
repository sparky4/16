/*
=====================
=
= DemoLoop
=
=====================
*/

static	char *ParmStrings[] = {"easy","normal","hard",""};

void	DemoLoop (void)
{

/////////////////////////////////////////////////////////////////////////////
// main game cycle
/////////////////////////////////////////////////////////////////////////////

	displayofs = bufferofs = 0;
	VW_Bar (0,0,320,200,0);
	VW_SetScreen(0,0);

//
// Read in all the graphic images needed for the title sequence
//
		VW_WaitVBL(1);
		IN_ReadControl(0,&control);

//	set EASYMODE
//
	if (stricmp(_argv[2], "1") == 0)
		EASYMODEON = true;
	else
		EASYMODEON = false;

// restore game
//
	if (stricmp(_argv[3], "1") == 0)
	{
		VW_FadeOut();
		bufferofs = displayofs = 0;
		VW_Bar(0,0,320,200,0);
		if (GE_LoadGame())
		{
			loadedgame = true;
			playstate = ex_loadedgame;
			Keyboard[sc_Enter] = true;
			VW_Bar(0,0,320,200,0);
			ColoredPalette();
		}
		VW_Bar(0,0,320,200,0);
		VW_FadeIn();
	}

	// Play a game
	//
		restartgame = gd_Normal;
		NewGame();
		GameLoop();
}
