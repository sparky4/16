/*
=====================
=
= DemoLoop
=
=====================
*/

void
DemoLoop (void)
{
	char		*s;
	word		move;
	longword	lasttime;
	char *FileName1;
	struct Shape FileShape1;
#if CREDITS
	char *FileName2;
	struct Shape FileShape2;
#endif
	struct ffblk ffblk;
	WindowRec	mywin;
	int bufsave	= bufferofs;
	int dissave	= displayofs;


#if FRILLS
//
// check for launch from ted
//
	if (tedlevel)
	{
		NewGame();
		gamestate.mapon = tedlevelnum;
		GameLoop();
		TEDDeath();
	}
#endif

//
// demo loop
//
	US_SetLoadSaveHooks(LoadGame,SaveGame,ResetGame);
	restartgame = gd_Continue;

	if (findfirst("KDREAMS.CMP", &ffblk, 0) == -1)
		Quit("Couldn't find KDREAMS.CMP");

	while (true)
	{

		loadedgame = false;

		FileName1 = "TITLESCR.LBM";
		if (LoadLIBShape("KDREAMS.CMP", FileName1, &FileShape1))
			Quit("Can't load TITLE SCREEN");
#if CREDITS
		FileName2 = "CREDITS.LBM";
		if (LoadLIBShape("KDREAMS.CMP", FileName2, &FileShape2))
			Quit("Can't load CREDITS SCREEN");
#endif

		while (!restartgame && !loadedgame)
		{

			VW_InitDoubleBuffer();
			IN_ClearKeysDown();

			while (true)
			{

				VW_SetScreen(0, 0);
				MoveGfxDst(0, 200);
				UnpackEGAShapeToScreen(&FileShape1, 0, 0);
				VW_ScreenToScreen (64*200,0,40,200);

#if CREDITS
				if (IN_UserInput(TickBase * 8, false))
					break;
#else
				if (IN_UserInput(TickBase * 4, false))
					break;
#endif

#if CREDITS
				MoveGfxDst(0, 200);
				UnpackEGAShapeToScreen(&FileShape2, 0, 0);
				VW_ScreenToScreen (64*200,0,40,200);

				if (IN_UserInput(TickBase * 7, false))
					break;
#else
				MoveGfxDst(0, 200);
				UnpackEGAShapeToScreen(&FileShape1, 0, 0);
				VW_ScreenToScreen (64*200,0,40,200);

				if (IN_UserInput(TickBase * 3, false))
					break;
#endif

				displayofs = 0;
				VWB_Bar(0,0,320,200,FIRSTCOLOR);
				US_DisplayHighScores(-1);

				if (IN_UserInput(TickBase * 6, false))
					break;

			}

			bufferofs = bufsave;
			displayofs = dissave;

			VW_FixRefreshBuffer();
			US_ControlPanel ();
		}

		if (!loadedgame)
			NewGame();

		FreeShape(&FileShape1);
#if CREDITS
		FreeShape(&FileShape2);
#endif
		GameLoop();
	}
}
