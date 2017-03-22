#if 0

#define PaddleMinX      (CtlPanelSX + 3)
#define PaddleMaxX      (CtlPanelEX - 15)
#define BallMinX        (CtlPanelSX + 2)
#define BallMinY        (CtlPanelSY + 12 + 2)
#define BallMaxX        (CtlPanelEX - 6)
#define BallMaxY        (CtlPanelEY - 13)
#define CPaddleY        (BallMinY + 4)
#define KPaddleY        (BallMaxY - 2)
void
USL_DrawPongScore(word k,word c)
{
	fontcolor = HiliteColor;
	PrintY = py = CtlPanelSY + 4;
	px = CtlPanelSX + 6;
	VWB_Bar(px,py,42,6,BackColor);
	USL_DrawString("YOU:");
	PrintX = px;
	US_PrintUnsigned(k);
	px = CtlPanelSX + 108;
	VWB_Bar(px,py,50,6,BackColor);
	USL_DrawString("COMP:");
	PrintX = px;
	US_PrintUnsigned(c);
}

void
USL_PlayPong(void)
{
	boolean         ball,killball,revdir,done,lastscore;
	word            cycle,
				x,y,
				kx,cx,
				rx,
				bx,by,
				kscore,cscore,
				speedup;
	int                     bdx,bdy;
	longword        balltime,waittime;
	CursorInfo      cursorinfo;

	kx = cx = PaddleMinX + ((PaddleMaxX - PaddleMinX) / 2);
	bx = by = bdx = bdy = 0;
	kscore = cscore = 0;
	USL_DrawPongScore(0,0);
	cycle = 0;
	revdir = false;
	killball = true;
	done = false;
	lastscore = false;
	do
	{
		waittime = TimeCount;

		IN_ReadCursor(&cursorinfo);
		if (((cursorinfo.x < 0) || IN_KeyDown(sc_LeftArrow)) && (kx > PaddleMinX))
			kx -= 2;
		else if (((cursorinfo.x > 0) || IN_KeyDown(sc_RightArrow)) && (kx < PaddleMaxX))
			kx += 2;

		if (killball)
		{
			ball = false;
			balltime = TimeCount + TickBase;
			speedup = 10;
			killball = false;
		}

		if (ball && (cycle++ % 3))
		{
			x = (bx >> 2);
			if (!(x & 1))
				x += (US_RndT() & 1);

			if ((cx + 6 < x) && (cx < PaddleMaxX))
				cx += 1;
			else if ((cx + 6 > x) && (cx > PaddleMinX))
				cx -= 1;
		}

		VWB_Bar(BallMinX,BallMinY - 1,
				BallMaxX - BallMinX + 5,BallMaxY - BallMinY + 7,
				BackColor);
		VWB_DrawSprite(cx,CPaddleY,PADDLESPR);
		VWB_DrawSprite(kx,KPaddleY,PADDLESPR);
		if (ball)
		{
			if
			(
				(((bx + bdx) >> 2) > BallMaxX)
			||      (((bx + bdx) >> 2) < BallMinX)
			)
			{
				SD_PlaySound(BALLBOUNCESND);
				bdx = -bdx;
			}
			bx += bdx;

			if (((by + bdy) >> 2) > BallMaxY)
			{
				killball = true;
				lastscore = false;
				cscore++;
				SD_PlaySound(COMPSCOREDSND);
				USL_DrawPongScore(kscore,cscore);
				if (cscore == 21)
				{
					USL_CtlDialog("You lost!","Press any key",nil);
					done = true;
					continue;
				}
			}
			else if (((by + bdy) >> 2) < BallMinY)
			{
				killball = true;
				lastscore = true;
				kscore++;
				SD_PlaySound(KEENSCOREDSND);
				USL_DrawPongScore(kscore,cscore);
				if (kscore == 21)
				{
					USL_CtlDialog("You won!","Press any key",nil);
					done = true;
					continue;
				}
			}
			by += bdy;

			x = bx >> 2;
			y = by >> 2;
			if (!killball)
			{
				if
				(
					(bdy < 0)
				&&      ((y >= CPaddleY) && (y < CPaddleY + 3))
				&&      ((x >= (cx - 5)) && (x < (cx + 11)))
				)
				{
					rx = cx;
					revdir = true;
					SD_PlaySound(COMPPADDLESND);
				}
				else if
				(
					(bdy > 0)
				&&      ((y >= (KPaddleY - 3)) && (y < KPaddleY))
				&&      ((x >= (kx - 5)) && (x < (kx + 11)))
				)
				{
					if (((bdy >> 2) < 3) && !(--speedup))
					{
						bdy++;
						speedup = 10;
					}
					rx = kx;
					revdir = true;
					SD_PlaySound(KEENPADDLESND);
				}
				if (revdir)
				{
					bdy = -bdy;
					bdx = ((x + 5 - rx) >> 1) - (1 << 2);
					if (!bdx)
						bdx--;
					revdir = false;
				}
			}
			VWB_DrawSprite(x,y,(x & 1)? BALL1PIXELTOTHERIGHTSPR : BALLSPR);
		}
		else if (TimeCount >= balltime)
		{
			ball = true;
			bdx = 1 - (US_RndT() % 3);
			bdy = 2;
			if (lastscore)
				bdy = -bdy;
			bx = (BallMinX + ((BallMaxX - BallMinX) / 2)) << 2;
			by = (BallMinY + ((BallMaxY - BallMinY) / 2)) << 2;
		}
		VW_UpdateScreen();
		while (waittime == TimeCount)
			;       // DEBUG - do adaptiveness
	} while ((LastScan != sc_Escape) && !done);
	IN_ClearKeysDown();
}

#pragma argsused
static boolean
USL_PongCustom(UserCall call,struct UserItem far *item)
{
	if (call != uic_SetupCard)
		return(false);

	VWB_DrawPic(0,0,CP_MENUSCREENPIC);
	VWB_DrawPic(CtlPanelSX + 56,CtlPanelSY,CP_PADDLEWARPIC);
	VWB_Hlin(CtlPanelSX + 3,CtlPanelEX - 3,CtlPanelSY + 12,HiliteColor ^ BackColor);
	VWB_Hlin(CtlPanelSX + 3,CtlPanelEX - 3,CtlPanelEY - 7,HiliteColor ^ BackColor);
	USL_PlayPong();

	return(true);
}

#endif
