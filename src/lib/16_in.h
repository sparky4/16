/* Catacomb Apocalypse Source Code
 * Copyright (C) 1993-2014 Flat Rock Software
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

//
//	ID Engine
//	ID_IN.h - Header file for Input Manager
//	v1.0d1w
//	By Jason Blochowiak
//	Open Watcom port by sparky4
//

#ifndef	__16_IN__
#define	__16_IN__

#include <string.h>
#include "src/lib/lib_head.h"

//++++#ifdef	__DEBUG__
#define	__DEBUG_InputMgr__
//++++#endif

#define TESTKEYIN
#define TESTCONTROLNOISY

#define	KeyInt	9	// The keyboard ISR number

// Stuff for the joystick
#define	JoyScaleMax		32768
#define	JoyScaleShift	8
#define	MaxJoyValue		5000

#define	MaxPlayers	4
#define	MaxKbds		2
#define	MaxJoys		2
#define	MaxPads		2
#define	NumCodes	128

typedef	byte		ScanCode;
#define	sc_None			0
#define	sc_Bad			0xff
#define	sc_Return		0x1c
#define	sc_Enter		sc_Return
#define	sc_Escape		0x01
#define	sc_Space		0x39
#define	sc_BackSpace	0x0e
#define	sc_Tab			0x0f
#define	sc_Alt			0x38
#define	sc_Control		0x1d
#define	sc_CapsLock		0x3a
#define	sc_LShift		0x2a
#define	sc_RShift		0x36
#define	sc_UpArrow		0x48
#define	sc_DownArrow	0x50
#define	sc_LeftArrow	0x4b
#define	sc_RightArrow	0x4d
#define	sc_Insert		0x52
#define	sc_Delete		0x53
#define	sc_Home			0x47
#define	sc_End			0x4f
#define	sc_PgUp			0x49
#define	sc_PgDn			0x51
#define	sc_F1			0x3b
#define	sc_F2			0x3c
#define	sc_F3			0x3d
#define	sc_F4			0x3e
#define	sc_F5			0x3f
#define	sc_F6			0x40
#define	sc_F7			0x41
#define	sc_F8			0x42
#define	sc_F9			0x43
#define	sc_F10			0x44
#define	sc_F11			0x57
#define	sc_F12			0x59

#define	sc_1			0x02
#define	sc_2			0x03
#define	sc_3			0x04
#define	sc_4			0x05
#define	sc_5			0x06
#define	sc_6			0x07
#define	sc_7			0x08
#define	sc_8			0x09
#define	sc_9			0x0a
#define	sc_0			0x0b

#define	sc_A			0x1e
#define	sc_B			0x30
#define	sc_C			0x2e
#define	sc_D			0x20
#define	sc_E			0x12
#define	sc_F			0x21
#define	sc_G			0x22
#define	sc_H			0x23
#define	sc_I			0x17
#define	sc_J			0x24
#define	sc_K			0x25
#define	sc_L			0x26
#define	sc_M			0x32
#define	sc_N			0x31
#define	sc_O			0x18
#define	sc_P			0x19
#define	sc_Q			0x10
#define	sc_R			0x13
#define	sc_S			0x1f
#define	sc_T			0x14
#define	sc_U			0x16
#define	sc_V			0x2f
#define	sc_W			0x11
#define	sc_X			0x2d
#define	sc_Y			0x15
#define	sc_Z			0x2c

#define	key_None		0
#define	key_Return		0x0d
#define	key_Enter		key_Return
#define	key_Escape		0x1b
#define	key_Space		0x20
#define	key_BackSpace	0x08
#define	key_Tab			0x09
#define	key_Delete		0x7f

#define	key_LSuper			0x5b
#define	key_RSuper			0x5c
#define	key_Menu			0x5d

// 	Stuff for the mouse
#define	MReset		0
#define	MButtons	3
#define	MDelta		11

#define	MouseInt	0x33
#ifdef DEMO0
typedef	enum		{
						demo_Off,demo_Record,demo_Playback,demo_PlayDone
					} Demo;
#endif
typedef	enum		{
						//ctrl_None,				// MDM (GAMERS EDGE) - added
						ctrl_Keyboard,
							ctrl_Keyboard1 = ctrl_Keyboard,ctrl_Keyboard2,
						ctrl_Joystick,
							ctrl_Joystick1 = ctrl_Joystick,ctrl_Joystick2,
						ctrl_Mouse,
						ctrl_Joypad,
							ctrl_Joypad1 = ctrl_Joypad,ctrl_Joypad2
					} ControlType;
typedef	enum		{
						motion_Left = -1,motion_Up = -1,
						motion_None = 0,
						motion_Right = 1,motion_Down = 1
					} Motion;
typedef	enum		{
						dir_North,//dir_NorthEast,
						dir_East,//dir_SouthEast,
						dir_South,//dir_Soutinest,
						dir_West,//dir_Nortinest,
						dir_None
					} Direction;
typedef	struct		{
						boolean		button0,button1,button2,button3;
						int			x,y;
						Motion		xaxis,yaxis;
						Direction	dir;
					} CursorInfo;

typedef	struct		{
						ScanCode	button0,button1,
									//upleft,
									up,
									//upright,
									left,				right,
									//downleft,
									down
									//,downright
									;
					} KeyboardDef;
typedef	struct		{
						word		joyMinX,joyMinY,
									threshMinX,threshMinY,
									threshMaxX,threshMaxY,
									joyMaxX,joyMaxY,
									joyMultXL,joyMultYL,
									joyMultXH,joyMultYH;
					} JoystickDef;
typedef	struct
{
	boolean w;
} JoypadDef;

typedef	struct
{
	CursorInfo	info;
	ControlType	Controls;
} player_t;

typedef	struct
{
	boolean			MousePresent;
	boolean			JoysPresent[MaxJoys];
	boolean			JoyPadPresent[MaxPads];
	boolean		Keyboard[NumCodes];
	boolean		Paused;
	char		LastASCII;
	ScanCode	LastScan;

	boolean		IN_Started;
	boolean		CapsLock;
	ScanCode	CurCode,LastCode;

	KeyboardDef	KbdDefs[MaxKbds];
	JoystickDef	JoyDefs[MaxJoys];
	JoypadDef	JoypadDefs[MaxPads];
} inconfig;

/*
=============================================================================

					GLOBAL VARIABLES

=============================================================================
*/

//
// configuration variables
//
//static boolean			MousePresent;
//static boolean			JoysPresent[MaxJoys];
//static boolean			JoyPadPresent[MaxPads];

// 	Global variables
//		extern boolean JoystickCalibrated;		// MDM (GAMERS EDGE) - added
//		extern ControlType ControlTypeUsed;				// MDM (GAMERS EDGE) - added

		//extern boolean		Keyboard[NumCodes];
		//extern boolean		Paused;
		//extern char		LastASCII;
		//extern ScanCode	LastScan;

		//extern KeyboardDef	KbdDefs[];
		//static KeyboardDef	KbdDefs[MaxKbds] = {0x1d,0x38,0x47,0x48,0x49,0x4b,0x4d,0x4f,0x50,0x51};
		//extern JoystickDef	JoyDefs[MaxJoys];
		//extern ControlType	Controls[MaxPlayers];

		//extern dword	MouseDownCount;

#ifdef DEMO0
		static Demo		DemoMode = demo_Off;
		static byte /*_seg*/	*DemoBuffer;
		static word		DemoOffset,DemoSize;
#endif

extern dword far* clockdw;

/*
=============================================================================

					LOCAL VARIABLES

=============================================================================
*/
static	byte        far ASCIINames[] =		// Unshifted ASCII for scan codes
					{
//	 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
	0  ,27 ,'1','2','3','4','5','6','7','8','9','0','-','=',8  ,9  ,	// 0
	'q','w','e','r','t','y','u','i','o','p','[',']',13 ,0  ,'a','s',	// 1
	'd','f','g','h','j','k','l',';',39 ,'`',0  ,92 ,'z','x','c','v',	// 2
	'b','n','m',',','.','/',0  ,'*',0  ,' ',0  ,0  ,0  ,0  ,0  ,0  ,	// 3
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,'7','8','9','-','4','5','6','+','1',	// 4
	'2','3','0',127,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 5
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 6
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0		// 7
					},
					far ShiftNames[] =		// Shifted ASCII for scan codes
					{
//	 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
	0  ,27 ,'!','@','#','$','%','^','&','*','(',')','_','+',8  ,9  ,	// 0
	'Q','W','E','R','T','Y','U','I','O','P','{','}',13 ,0  ,'A','S',	// 1
	'D','F','G','H','J','K','L',':',34 ,'~',0  ,'|','Z','X','C','V',	// 2
	'B','N','M','<','>','?',0  ,'*',0  ,' ',0  ,0  ,0  ,0  ,0  ,0  ,	// 3
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,'7','8','9','-','4','5','6','+','1',	// 4
	'2','3','0',127,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 5
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 6
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0   	// 7
					},
					far SpecialNames[] =	// ASCII for 0xe0 prefixed codes
					{
//	 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 0
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,13 ,0  ,0  ,0  ,	// 1
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 2
	0  ,0  ,0  ,0  ,0  ,'/',0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 3
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 4
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 5
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 6
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0   	// 7
					},
					*ScanNames[] =		// Scan code names with single chars
					{
	"?","?","1","2","3","4","5","6","7","8","9","0","-","+","?","?",
	"Q","W","E","R","T","Y","U","I","O","P","[","]","|","?","A","S",
	"D","F","G","H","J","K","L",";","\"","?","?","?","Z","X","C","V",
	"B","N","M",",",".","/","?","?","?","?","?","?","?","?","?","?",
	"?","?","?","?","?","?","?","?","\xf","?","-","\x15","5","\x11","+","?",
	"\x13","?","?","?","?","?","?","?","?","?","?","?","?","?","?","?",
	"?","?","?","?","?","?","?","?","?","?","?","?","?","?","?","?",
	"?","?","?","?","?","?","?","?","?","?","?","?","?","?","?","?"
					},	// DEBUG - consolidate these
					far ExtScanCodes[] =	// Scan codes with >1 char names
					{
	1,0xe,0xf,0x1d,0x2a,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,
	0x3f,0x40,0x41,0x42,0x43,0x44,0x57,0x59,0x46,0x1c,0x36,
	0x37,0x38,0x47,0x49,0x4f,0x51,0x52,0x53,0x45,0x48,
	0x50,0x4b,0x4d,0x00
					},
					*ExtScanNames[] =	// Names corresponding to ExtScanCodes
					{
	"Esc","BkSp","Tab","Ctrl","LShft","Space","CapsLk","F1","F2","F3","F4",
	"F5","F6","F7","F8","F9","F10","F11","F12","ScrlLk","Enter","RShft",
	"PrtSc","Alt","Home","PgUp","End","PgDn","Ins","Del","NumLk","Up",
	"Down","Left","Right",""
					};

static	Direction	DirTable[] =		// Quick lookup for total direction
					{
						//dir_Nortinest,
						dir_North,
						//dir_NorthEast,
						dir_West,		dir_None,	dir_East,
						//dir_Soutinest,
						dir_South//,dir_SouthEast
					};

static	void			(*INL_KeyHook)(void);
static	void interrupt	(*OldKeyVect)(void);

static	char			*ParmStringsIN[] = {"nojoys","nomouse",nil};

// Function prototypes
#define	IN_KeyDown(code)	(Keyboard[(code)])
#define	IN_ClearKey(code)	{Keyboard[code] = false; if (code == LastScan) LastScan = sc_None;}

//	Internal routines
void interrupt INL_KeyService(inconfig *in);
void Mouse(int x);
//static void INL_GetMouseDelta(int *x,int *y);
//static word INL_GetMouseButtons(void);
void IN_GetJoyAbs(word joy,word *xp,word *yp);
//static void INL_GetJoyDelta(word joy,int *dx,int *dy,boolean adaptive);
//static word INL_GetJoyButtons(word joy);
word IN_GetJoyButtonsDB(word joy);
//static void INL_StartKbd(void);
//static void INL_ShutKbd(void);
//static boolean INL_StartMouse(void);
//static void INL_ShutMouse(void);
//static void INL_SetJoyScale(word joy);
void IN_SetupJoy(word joy,word minx,word maxx,word miny,word maxy, inconfig *in);
//static boolean INL_StartJoy(word joy);
//static void INL_ShutJoy(word joy);
void IN_Startup(inconfig *in);
void IN_Default(boolean gotit,player_t *player,ControlType nt, inconfig *in);
void IN_Shutdown(inconfig *in);
void IN_SetKeyHook(void (*hook)());
void IN_ClearKeysDown(inconfig *in);
//static void INL_AdjustCursor(CursorInfo *info,word buttons,int dx,int dy);
void IN_ReadCursor(CursorInfo *info);
void IN_ReadControl(int playnum,player_t *player);
void IN_SetControlType(word playnum,player_t *player,ControlType type);
#if DEMO0
boolean IN_StartDemoRecord(word bufsize);
void IN_StartDemoPlayback(byte /*__segment*/ *buffer,word bufsize);
void IN_StopDemo(void);
void IN_FreeDemoBuffer(void);
#endif
byte *IN_GetScanName(ScanCode scan);
ScanCode IN_WaitForKey(void);
char IN_WaitForASCII(void);
void IN_AckBack(void);
void IN_Ack(void);
boolean IN_IsUserInput(void);
boolean IN_UserInput(dword delay,boolean clear);

/*extern	void		IN_Startup(void),IN_Shutdown(void),
					IN_Default(boolean gotit,ControlType in),
					IN_SetKeyHook(void (*)()),
					IN_ClearKeysDown(void),
					IN_ReadCursor(CursorInfo *),
					IN_ReadControl(int,ControlInfo *),
					IN_SetControlType(int,ControlType),
					IN_GetJoyAbs(word joy,word *xp,word *yp),
					IN_SetupJoy(word joy,word minx,word maxx,
								word miny,word maxy),
					Mouse(int x),
#ifdef DEMO0
					IN_StartDemoPlayback(byte __segment *buffer,word bufsize),
					IN_StopDemo(void),IN_FreeDemoBuffer(void),
#endif
					IN_Ack(void),IN_AckBack(void);
extern	boolean		IN_UserInput(dword delay,boolean clear),
					IN_IsUserInput(void)
#ifdef DEMO0
					, IN_StartDemoRecord(word bufsize)
#endif
;
extern	byte		*IN_GetScanName(ScanCode);
extern	char		IN_WaitForASCII(void);
extern	ScanCode	IN_WaitForKey(void);
extern	word		IN_GetJoyButtonsDB(word joy);*/

boolean IN_qb(byte kee);
#endif
