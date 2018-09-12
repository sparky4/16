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
	int	i,level;

//
// check for launch from ted
//
	if (tedlevel)
	{
		NewGame();
		gamestate.mapon = tedlevelnum;
		restartgame = gd_Normal;
		for (i = 1;i < _argc;i++)
		{
			if ( (level = US_CheckParm(_argv[i],ParmStrings)) == -1)
				continue;

			restartgame = gd_Easy+level;
			break;
		}
		GameLoop();
		TEDDeath();
	}


//
// main game cycle
//
	displayofs = bufferofs = 0;
	VW_Bar (0,0,320,200,0);

	while (1)
	{
		CA_CacheGrChunk (TITLEPIC);
		bufferofs = SCREEN2START;
		displayofs = SCREEN1START;
		VWB_DrawPic (0,0,TITLEPIC);
		MM_SetPurge (&grsegs[TITLEPIC],3);
		UNMARKGRCHUNK(TITLEPIC);
		FizzleFade (bufferofs,displayofs,320,200,true);

		if (!IN_UserInput(TickBase*3,false))
		{
			CA_CacheGrChunk (CREDITSPIC);
			VWB_DrawPic (0,0,CREDITSPIC);
			MM_SetPurge (&grsegs[CREDITSPIC],3);
			UNMARKGRCHUNK(CREDITSPIC);
			FizzleFade (bufferofs,displayofs,320,200,true);

		}

		if (!IN_UserInput(TickBase*3,false))
		{
highscores:
			DrawHighScores ();
			FizzleFade (bufferofs,displayofs,320,200,true);
			IN_UserInput(TickBase*3,false);
		}

		if (IN_IsUserInput())
		{
			US_ControlPanel ();

			if (restartgame || loadedgame)
			{
				GameLoop ();
				goto highscores;
			}
		}

	}
}
