#include "src/lib/16_us.h"

word px,py;
word PrintX,PrintY;
word WindowX,WindowY,WindowW,WindowH;

static word far* clockw= (word far*) 0x046C; // 18.2hz clock

//      Control panel data

#define CtlPanelSX	74
#define CtlPanelSY	48
#define CtlPanelEX	234
#define CtlPanelEY	150
#define CtlPanelW	(CtlPanelEX - CtlPanelSX)
#define CtlPanelH	(CtlPanelEY - CtlPanelSY)

#define	TickBase	70		// 70Hz per tick - used as a base for timer 0

#define PaddleMinX	(CtlPanelSX + 3)
#define PaddleMaxX	(CtlPanelEX - 15)
#define BallMinX		(CtlPanelSX + 2)
#define BallMinY		(CtlPanelSY + 12 + 2)
#define BallMaxX		(CtlPanelEX - 6)
#define BallMaxY		(CtlPanelEY - 13)
#define CPaddleY		(BallMinY + 4)
#define KPaddleY		(BallMaxY - 2)
void
USL_DrawPongScore(word k,word c, global_game_variables_t *gvar)
{
	//++++fontcolor = HiliteColor;
	PrintY = py = CtlPanelSY + 4;
	px = CtlPanelSX + 6;
	VLB_Bar(px,py,42,6,3/*BackColor*/, &gvar->video.ofs);
	//++++USL_DrawString("YOU:");
//	modexprint(page_t *page, sword x, sword y, word t, boolean tlsw, word col, word bgcol, boolean sw, const byte *str)
	modexprint(gvar->video.page, px, py, 1, 0, 4, 8, 1, "YOU:");
	PrintX = px;
	//++++US_PrintUnsigned(k);
	px = CtlPanelSX + 108;
	VLB_Bar(px,py,50,6,3/*BackColor*/, &gvar->video.ofs);
	//++++USL_DrawString("COMP:");
	modexprint(gvar->video.page, px, py, 1, 0, 4, 8, 1, "COMP:");
	PrintX = px;
	//++++US_PrintUnsigned(c);
}

void
USL_PlayPong(global_game_variables_t *gvar)
{
	boolean		ball,killball,revdir,done,lastscore;
	word			cycle,
				x,y,
				kx,cx,
				rx,
				bx,by,
				kscore,cscore,
				speedup;
	int			bdx,bdy;
	word			balltime,waittime;
	CursorInfo		cursorinfo;
	word TimeCount = *clockw;

	kx = cx = PaddleMinX + ((PaddleMaxX - PaddleMinX) / 2);
	bx = by = bdx = bdy = 0;
	kscore = cscore = 0;
	USL_DrawPongScore(0,0, gvar);
	cycle = 0;
	revdir = false;
	killball = true;
	done = false;
	lastscore = false;
	do
	{
		waittime = TimeCount;

		IN_ReadCursor(&cursorinfo, gvar);
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

//++++
//#if 1
		VLB_Bar(BallMinX,BallMinY - 1,
				BallMaxX - BallMinX + 5,BallMaxY - BallMinY + 7,
				3/*BackColor*/, &gvar->video.ofs);
#if 0
		VWB_DrawSprite(cx,CPaddleY,PADDLESPR);
		VWB_DrawSprite(kx,KPaddleY,PADDLESPR);

		modexputPixel(gvar->video.page, BallMinX, BallMinY, 3);
#endif
		modexputPixel(gvar->video.page, cx, CPaddleY, 3);
		modexputPixel(gvar->video.page, kx, KPaddleY, 3);
		if (ball)
		{
			if
			(
				(((bx + bdx) >> 2) > BallMaxX)
			||	(((bx + bdx) >> 2) < BallMinX)
			)
			{
//++++				SD__PlaySound(BALLBOUNCESND);
				bdx = -bdx;
			}
			bx += bdx;

			if (((by + bdy) >> 2) > BallMaxY)
			{
				killball = true;
				lastscore = false;
				cscore++;
//++++				SD__PlaySound(COMPSCOREDSND);
				USL_DrawPongScore(kscore,cscore, gvar);
				if (cscore == 21)
				{
//++++					USL_CtlDialog("You lost!","Press any key",nil);
					done = true;
					continue;
				}
			}
			else if (((by + bdy) >> 2) < BallMinY)
			{
				killball = true;
				lastscore = true;
				kscore++;
//++++				SD__PlaySound(KEENSCOREDSND);
				USL_DrawPongScore(kscore,cscore, gvar);
				if (kscore == 21)
				{
//++++					USL_CtlDialog("You won!","Press any key",nil);
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
				&&	((y >= CPaddleY) && (y < CPaddleY + 3))
				&&	((x >= (cx - 5)) && (x < (cx + 11)))
				)
				{
					rx = cx;
					revdir = true;
//++++				SD__PlaySound(COMPPADDLESND);
				}
				else if
				(
					(bdy > 0)
				&&	((y >= (KPaddleY - 3)) && (y < KPaddleY))
				&&	((x >= (kx - 5)) && (x < (kx + 11)))
				)
				{
					if (((bdy >> 2) < 3) && !(--speedup))
					{
						bdy++;
						speedup = 10;
					}
					rx = kx;
					revdir = true;
//++++					SD__PlaySound(KEENPADDLESND);
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
//++++			VWB_DrawSprite(x,y,(x & 1)? BALL1PIXELTOTHERIGHTSPR : BALLSPR);
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
//++++		VW_UpdateScreen();
		while (waittime == TimeCount){}
			//;       // DEBUG - do adaptiveness
	} while ((gvar->in.inst->LastScan != sc_Escape) && !done);
	IN_ClearKeysDown();
}
/*
#pragma argsused
static boolean
USL_PongCustom(UserCall call,struct UserItem far *item)
{
	if (call != uic_SetupCard)
		return(false);

	//++++VWB_DrawPic(0,0,CP_MENUSCREENPIC);
	//++++VWB_DrawPic(CtlPanelSX + 56,CtlPanelSY,CP_PADDLEWARPIC);
	//++++VWB_Hlin(CtlPanelSX + 3,CtlPanelEX - 3,CtlPanelSY + 12,HiliteColor ^ BackColor);
	//++++VWB_Hlin(CtlPanelSX + 3,CtlPanelEX - 3,CtlPanelEY - 7,HiliteColor ^ BackColor);
	USL_PlayPong();

	return(true);
}
*/
///////////////////////////////////////////////////////////////////////////
//
// US_RndT - Returns the next 8-bit pseudo random number
//
///////////////////////////////////////////////////////////////////////////
int US_RndT()
{
	int rndindex = 0;

	byte rndtable[] = {
	0,   8, 109, 220, 222, 241, 149, 107,  75, 248, 254, 140,  16,  66,
	74,  21, 211,  47,  80, 242, 154,  27, 205, 128, 161,  89,  77,  36,
	95, 110,  85,  48, 212, 140, 211, 249,  22,  79, 200,  50,  28, 188,
	52, 140, 202, 120,  68, 145,  62,  70, 184, 190,  91, 197, 152, 224,
	149, 104,  25, 178, 252, 182, 202, 182, 141, 197,   4,  81, 181, 242,
	145,  42,  39, 227, 156, 198, 225, 193, 219,  93, 122, 175, 249,   0,
	175, 143,  70, 239,  46, 246, 163,  53, 163, 109, 168, 135,   2, 235,
	25,  92,  20, 145, 138,  77,  69, 166,  78, 176, 173, 212, 166, 113,
	94, 161,  41,  50, 239,  49, 111, 164,  70,  60,   2,  37, 171,  75,
	136, 156,  11,  56,  42, 146, 138, 229,  73, 146,  77,  61,  98, 196,
	135, 106,  63, 197, 195,  86,  96, 203, 113, 101, 170, 247, 181, 113,
	80, 250, 108,   7, 255, 237, 129, 226,  79, 107, 112, 166, 103, 241,
	24, 223, 239, 120, 198,  58,  60,  82, 128,   3, 184,  66, 143, 224,
	145, 224,  81, 206, 163,  45,  63,  90, 168, 114,  59,  33, 159,  95,
	28, 139, 123,  98, 125, 196,  15,  70, 194, 253,  54,  14, 109, 226,
	71,  17, 161,  93, 186,  87, 244, 138,  20,  52, 123, 251,  26,  36,
	17,  46,  52, 231, 232,  76,  31, 221,  84,  37, 216, 165, 212, 106,
	197, 242,  98,  43,  39, 175, 254, 145, 190,  84, 118, 222, 187, 136,
	120, 163, 236, 249 };
	rndindex = (rndindex+1)&0xff;
	return rndtable[rndindex];
}
