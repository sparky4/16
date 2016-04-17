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
//	ID_IN.c - Input Manager
//	v1.0d1w
//	By Jason Blochowiak
//	Open Watcom port by sparky4
//

//
//	This module handles dealing with the various input devices
//
//	Depends on: Memory Mgr (for demo recording), Sound Mgr (for timing stuff),
//				User Mgr (for command line parms)
//
//	Globals:
//		LastScan - The keyboard scan code of the last key pressed
//		LastASCII - The ASCII value of the last key pressed
//	DEBUG - there are more globals
//

#include "src/lib/16_in.h"

boolean testkeyin=0,testcontrolnoisy=0,testctrltype=0;

/*
=============================================================================

					GLOBAL VARIABLES

=============================================================================
*/
struct inconfig
{
	boolean			MousePresent;
	boolean			JoysPresent[MaxJoys];
	boolean		Keyboard[NumCodes];
	boolean		Paused;
	char		LastASCII;
	ScanCode	LastScan;

	KeyboardDef	KbdDefs[MaxKbds];
	JoystickDef	JoyDefs[MaxJoys];
} inpu;

//extern inconfig inpu;
//inpu.KbdDefs = {0x1d,0x38,/*0x47,*/0x48,/*0x49,*/0x4b,0x4d,/*0x4f,*/0x50/*,0x51*/};

/*
=============================================================================

					LOCAL VARIABLES

=============================================================================
*/

#ifdef __cplusplus		/* Function must be declared C style */
extern "C" {
#endif

static struct instat {
	boolean		IN_Started;
	boolean		CapsLock;
	ScanCode	CurCode,LastCode;
} inst;

static	void			(*INL_KeyHook)(void);
static	void interrupt	(*OldKeyVect)(void);
static	char			*ParmStringsIN[] = {"nojoys","nomouse",nil};

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
						dir_West,
						dir_None,
						dir_East,
						dir_South
						//dir_NorthEast,
						//dir_Soutinest,
						//,dir_SouthEast
					};
#ifdef __cplusplus
}
#endif

//	Internal routines
///////////////////////////////////////////////////////////////////////////
//
//	INL_KeyService() - Handles a keyboard interrupt (key up/down)
//
///////////////////////////////////////////////////////////////////////////
void interrupt
INL_KeyService()
{
static	boolean	special;
		byte	k,c;
		register byte temp;

	k = inp(0x60);	// Get the scan code

	// Tell the XT keyboard controller to clear the key
	outp(0x61,(temp = inp(0x61)) | 0x80);
	outp(0x61,temp);

	if (k == 0xe0)		// Special key prefix
		special = true;
	else if (k == 0xe1)	// Handle Pause key
		inpu.Paused = true;
	else
	{
		if (k & 0x80)	// Break code
		{
			k &= 0x7f;

// DEBUG - handle special keys: ctl-alt-delete, print scrn

			inpu.Keyboard[k] = false;
		}
		else			// Make code
		{
			inst.LastCode = inst.CurCode;
			inst.CurCode = inpu.LastScan = k;
			inpu.Keyboard[k] = true;

			if (special)
				c = SpecialNames[k];
			else
			{
				if (k == sc_CapsLock)
				{
					inst.CapsLock ^= true;
					// DEBUG - make caps lock light work
				}

				if (inpu.Keyboard[sc_LShift] || inpu.Keyboard[sc_RShift])	// If shifted
				{
					c = ShiftNames[k];
					if ((c >= 'A') && (c <= 'Z') && inst.CapsLock)
						c += 'a' - 'A';
				}
				else
				{
					c = ASCIINames[k];
					if ((c >= 'a') && (c <= 'z') && inst.CapsLock)
						c -= 'a' - 'A';
				}
			}
			if (c)
				inpu.LastASCII = c;
		}

		special = false;
	}

	if (INL_KeyHook && !special)
		INL_KeyHook();
//#ifdef TESTKEYIN
	if(testkeyin > 0) printf("%c	%u	[0x%x %u]	%u\n", c, c, k, k, inpu.Keyboard[k]);
//endif
	outp(0x20,0x20);
}

void
Mouse(int x)
{
	union REGS CPURegs;
	x = CPURegs.x.ax;
	int86(MouseInt,&CPURegs,&CPURegs);
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_GetMouseDelta() - Gets the amount that the mouse has moved from the
//		mouse driver
//
///////////////////////////////////////////////////////////////////////////
static void
INL_GetMouseDelta(int *x,int *y)
{
	union REGS CPURegs;
	Mouse(MDelta);
	*x = CPURegs.x.cx;
	*y = CPURegs.x.dx;
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_GetMouseButtons() - Gets the status of the mouse buttons from the
//		mouse driver
//
///////////////////////////////////////////////////////////////////////////
static word
INL_GetMouseButtons(void)
{
	union REGS CPURegs;
	word	buttons;

	Mouse(MButtons);
	buttons = CPURegs.x.bx;
	return(buttons);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_GetJoyAbs() - Reads the absolute position of the specified joystick
//
///////////////////////////////////////////////////////////////////////////
void
IN_GetJoyAbs(word joy,word *xp,word *yp)
{
	byte	xb,yb,
			xs,ys;
	word	x,y;

	x = y = 0;
	xs = joy? 2 : 0;		// Find shift value for x axis
	xb = 1 << xs;			// Use shift value to get x bit mask
	ys = joy? 3 : 1;		// Do the same for y axis
	yb = 1 << ys;

// Read the absolute joystick values
	__asm
	{
		pushf				// Save some registers
		push	si
		push	di
		cli					// Make sure an interrupt doesn't screw the timings


		mov		dx,0x201
		in		al,dx
		out		dx,al		// Clear the resistors

		mov		ah,[xb]		// Get masks into registers
		mov		ch,[yb]

		xor		si,si		// Clear count registers
		xor		di,di
		xor		bh,bh		// Clear high byte of bx for later

		push	bp			// Don't mess up stack frame
		mov		bp,MaxJoyValue

loo:
		in		al,dx		// Get bits indicating whether all are finished

		dec		bp			// Check bounding register
		jz		done		// We have a silly value - abort

		mov		bl,al		// Duplicate the bits
		and		bl,ah		// Mask off useless bits (in [xb])
		add		si,bx		// Possibly increment count register
		mov		cl,bl		// Save for testing later

		mov		bl,al
		and		bl,ch		// [yb]
		add		di,bx

		add		cl,bl
		jnz		loo		// If both bits were 0, drop out

done:
		pop		bp

		mov		cl,[xs]		// Get the number of bits to shift
		shr		si,cl		//  and shift the count that many times

		mov		cl,[ys]
		shr		di,cl

		mov		[x],si		// Store the values into the variables
		mov		[y],di

		pop		di
		pop		si
		popf				// Restore the registers
	}

	*xp = x;
	*yp = y;
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_GetJoyDelta() - Returns the relative movement of the specified
//		joystick (from +/-127, scaled adaptively)
//
///////////////////////////////////////////////////////////////////////////
static void
INL_GetJoyDelta(word joy,int *dx,int *dy,boolean adaptive)
{
	word		x,y;
	word	time;
	word TimeCount = *clockw;
	JoystickDef	*def;
static	word	lasttime;

	IN_GetJoyAbs(joy,&x,&y);
	def = inpu.JoyDefs + joy;

	//TODO: inject p16 input controls!
	//which is this
	/*			if(DIRECTIONIFELSE)
			{
			if(!inpu.Keyboard[def->left] && !inpu.Keyboard[def->right]){
				if((inpu.Keyboard[def->up] && !inpu.Keyboard[def->down]))
					my = motion_Up;
				if((inpu.Keyboard[def->down] && !inpu.Keyboard[def->up]))
					my = motion_Down;
			}else if(!inpu.Keyboard[def->up] && !inpu.Keyboard[def->down]){
				if((inpu.Keyboard[def->left] && !inpu.Keyboard[def->right]))
					mx = motion_Left;
				if((inpu.Keyboard[def->right] && !inpu.Keyboard[def->left]))// || player[pn].pdir != 1)
					mx = motion_Right;
			}else
				//if(mx+my!=1 && mx+my!=-1 && mx!=my!=0)
				{	//2 keys pressed
					switch (player[pn].pdir)
					{
						case 0:
						case 4:
							if((inpu.Keyboard[def->left] && !inpu.Keyboard[def->right])) dir = DirTable[1];
							else if((inpu.Keyboard[def->right] && !inpu.Keyboard[def->left])) dir = DirTable[3];
						break;
						case 1:
						case 3:
							if((inpu.Keyboard[def->up] && !inpu.Keyboard[def->down])) dir = DirTable[0];
							else if((inpu.Keyboard[def->down] && !inpu.Keyboard[def->up])) dir = DirTable[4];
						break;
						default:
						break;
					}
					if(testcontrolnoisy > 0){ printf("dir=%c ", dirchar(dir)); printf("pdir=%c	", dirchar(player[pn].pdir)); }
				}//else printf("				");
			}*/
	if (x < def->threshMinX)
	{
		if (x < def->joyMinX)
			x = def->joyMinX;

		x = -(x - def->threshMinX);
		x *= def->joyMultXL;
		x >>= JoyScaleShift;
		*dx = (x > 127)? -127 : -x;
	}
	else if (x > def->threshMaxX)
	{
		if (x > def->joyMaxX)
			x = def->joyMaxX;

		x = x - def->threshMaxX;
		x *= def->joyMultXH;
		x >>= JoyScaleShift;
		*dx = (x > 127)? 127 : x;
	}
	else
		*dx = 0;

	if (y < def->threshMinY)
	{
		if (y < def->joyMinY)
			y = def->joyMinY;

		y = -(y - def->threshMinY);
		y *= def->joyMultYL;
		y >>= JoyScaleShift;
		*dy = (y > 127)? -127 : -y;
	}
	else if (y > def->threshMaxY)
	{
		if (y > def->joyMaxY)
			y = def->joyMaxY;

		y = y - def->threshMaxY;
		y *= def->joyMultYH;
		y >>= JoyScaleShift;
		*dy = (y > 127)? 127 : y;
	}
	else
		*dy = 0;

	if (adaptive)
	{
		time = (TimeCount - lasttime) / 2;
		if (time)
		{
			if (time > 8)
				time = 8;
			*dx *= time;
			*dy *= time;
		}
	}
	lasttime = TimeCount;
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_GetJoyButtons() - Returns the button status of the specified
//		joystick
//
///////////////////////////////////////////////////////////////////////////
static word
INL_GetJoyButtons(word joy)
{
register	word	result;

	result = inp(0x201);	// Get all the joystick buttons
	result >>= joy? 6 : 4;	// Shift into bits 0-1
	result &= 3;				// Mask off the useless bits
	result ^= 3;
	return(result);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_GetJoyButtonsDB() - Returns the de-bounced button status of the
//		specified joystick
//
///////////////////////////////////////////////////////////////////////////
word
IN_GetJoyButtonsDB(word joy)
{
	word TimeCount = *clockw;
	word	lasttime;
	word		result1,result2;

	do
	{
		result1 = INL_GetJoyButtons(joy);
		lasttime = TimeCount;
		while(TimeCount == lasttime)
		result2 = INL_GetJoyButtons(joy);
	} while(result1 != result2);
	return(result1);
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_StartKbd() - Sets up my keyboard stuff for use
//
///////////////////////////////////////////////////////////////////////////
static void
INL_StartKbd()
{
	INL_KeyHook = 0;	// Clear key hook

	IN_ClearKeysDown();

	OldKeyVect = _dos_getvect(KeyInt);
	_dos_setvect(KeyInt,INL_KeyService);
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_ShutKbd() - Restores keyboard control to the BIOS
//
///////////////////////////////////////////////////////////////////////////
static void
INL_ShutKbd(void)
{
	pokeb(0x40,0x17,peekb(0x40,0x17) & 0xfaf0);	// Clear ctrl/alt/shift flags

	_dos_setvect(KeyInt,OldKeyVect);
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_StartMouse() - Detects and sets up the mouse
//
///////////////////////////////////////////////////////////////////////////
static boolean
INL_StartMouse(void)
{
	union REGS CPURegs;
	if(_dos_getvect(MouseInt))
	{
		Mouse(MReset);
		if(CPURegs.x.ax == 0xffff)
			return(true);
	}
	return(false);
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_ShutMouse() - Cleans up after the mouse
//
///////////////////////////////////////////////////////////////////////////
static void
INL_ShutMouse(void)
{
}

//
//	INL_SetJoyScale() - Sets up scaling values for the specified joystick
//
static void
INL_SetJoyScale(word joy)
{
	JoystickDef	*def;

	def = &(inpu.JoyDefs[joy]);
	def->joyMultXL = JoyScaleMax / (def->threshMinX - def->joyMinX);
	def->joyMultXH = JoyScaleMax / (def->joyMaxX - def->threshMaxX);
	def->joyMultYL = JoyScaleMax / (def->threshMinY - def->joyMinY);
	def->joyMultYH = JoyScaleMax / (def->joyMaxY - def->threshMaxY);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_SetupJoy() - Sets up thresholding values and calls INL_SetJoyScale()
//		to set up scaling values
//
///////////////////////////////////////////////////////////////////////////
void
IN_SetupJoy(word joy,word minx,word maxx,word miny,word maxy)
{
	word		d,r;
	JoystickDef	*def;

	def = &(inpu.JoyDefs[joy]);

	def->joyMinX = minx;
	def->joyMaxX = maxx;
	r = maxx - minx;
	d = r / 3;
	def->threshMinX = ((r / 2) - d) + minx;
	def->threshMaxX = ((r / 2) + d) + minx;

	def->joyMinY = miny;
	def->joyMaxY = maxy;
	r = maxy - miny;
	d = r / 3;
	def->threshMinY = ((r / 2) - d) + miny;
	def->threshMaxY = ((r / 2) + d) + miny;

	INL_SetJoyScale(joy);
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_StartJoy() - Detects & auto-configures the specified joystick
//					The auto-config assumes the joystick is centered
//
///////////////////////////////////////////////////////////////////////////
static boolean
INL_StartJoy(word joy)
{
	word		x,y;

	IN_GetJoyAbs(joy,&x,&y);

	if
	(
		((x == 0) || (x > MaxJoyValue - 10))
	||	((y == 0) || (y > MaxJoyValue - 10))
	)
		return(false);
	else
	{
		IN_SetupJoy(joy,0,x * 2,0,y * 2);
		return(true);
	}
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_ShutJoy() - Cleans up the joystick stuff
//
///////////////////////////////////////////////////////////////////////////
static void
INL_ShutJoy(word joy)
{
	inpu.JoysPresent[joy] = false;
}

//	Public routines

///////////////////////////////////////////////////////////////////////////
//
//	IN_Startup() - Starts up the Input Mgr
//
///////////////////////////////////////////////////////////////////////////
void
IN_Startup()
{
	boolean	checkjoys,checkmouse;
	word	i;

	if (inst.IN_Started)
		return;

	checkjoys = true;
	checkmouse = true;
	for (i = 1;i < __argc;i++)
	{
		switch (US_CheckParm(__argv[i],ParmStringsIN))
		{
		case 0:
			checkjoys = false;
			break;
		case 1:
			checkmouse = false;
			break;
		}
	}

	INL_StartKbd();
	inpu.MousePresent = checkmouse? INL_StartMouse() : false;

	for (i = 0;i < MaxJoys;i++)
		inpu.JoysPresent[i] = checkjoys? INL_StartJoy(i) : false;

	inst.IN_Started = true;

}

///////////////////////////////////////////////////////////////////////////
//
//	IN_Default() - Sets up default conditions for the Input Mgr
//
///////////////////////////////////////////////////////////////////////////
void
IN_Default(boolean gotit,player_t *player,ControlType nt)
{
	int i;
	if(testctrltype==0)
	if
	(
		(!gotit)
	|| 	((nt == ctrl_Joystick1) && !inpu.JoysPresent[0])
	|| 	((nt == ctrl_Joystick2) && !inpu.JoysPresent[1])
	|| 	((nt == ctrl_Mouse) && !inpu.MousePresent)
	)
		nt = ctrl_Keyboard1;
	inpu.KbdDefs[0].button0 = 0x1c;
	inpu.KbdDefs[0].button1 = 0x38;
	//in.KbdDefs[0].upleft = 0x47;
	inpu.KbdDefs[0].up = 0x48;
	//in.KbdDefs[0].upright = 0x49;
	inpu.KbdDefs[0].left = 0x4b;
	inpu.KbdDefs[0].right = 0x4d;
	//in.KbdDefs[0].downleft = 0x4f;
	inpu.KbdDefs[0].down = 0x50;
	//in.KbdDefs[0].downright = 0x51;
	IN_SetControlType(0,player,nt);
	for(i=0; i>MaxPlayers;i++)
		player[i].d=2;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_Shutdown() - Shuts down the Input Mgr
//
///////////////////////////////////////////////////////////////////////////
void
IN_Shutdown()
{
	word	i;

	if (!inst.IN_Started)
		return;

	INL_ShutMouse();
	for (i = 0;i < MaxJoys;i++)
		INL_ShutJoy(i);
	INL_ShutKbd();

	inst.IN_Started = false;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_SetKeyHook() - Sets the routine that gets called by INL_KeyService()
//			everytime a real make/break code gets hit
//
///////////////////////////////////////////////////////////////////////////
void
IN_SetKeyHook(void (*hook)())
{
	INL_KeyHook = hook;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_ClearKeyDown() - Clears the keyboard array
//
///////////////////////////////////////////////////////////////////////////
void
IN_ClearKeysDown()
{
	int	i;

	inpu.LastScan = sc_None;
	inpu.LastASCII = key_None;
	memset (inpu.Keyboard,0,sizeof(inpu.Keyboard));
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_AdjustCursor() - Internal routine of common code from IN_ReadCursor()
//
///////////////////////////////////////////////////////////////////////////
static void
INL_AdjustCursor(CursorInfo *info,word buttons,int dx,int dy)
{
	if (buttons & (1 << 0))
		info->button0 = true;
	if (buttons & (1 << 1))
		info->button1 = true;

	info->x += dx;
	info->y += dy;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_ReadCursor() - Reads the input devices and fills in the cursor info
//		struct
//
///////////////////////////////////////////////////////////////////////////
void
IN_ReadCursor(CursorInfo *info)
{
	word	i,
			buttons;
	int		dx,dy;

	info->x = info->y = 0;
	info->button0 = info->button1 = false;

	if (inpu.MousePresent)
	{
		buttons = INL_GetMouseButtons();
		INL_GetMouseDelta(&dx,&dy);
		INL_AdjustCursor(info,buttons,dx,dy);
	}

	for (i = 0;i < MaxJoys;i++)
	{
		if (!inpu.JoysPresent[i])
			continue;

		buttons = INL_GetJoyButtons(i);
		INL_GetJoyDelta(i,&dx,&dy,true);
		dx /= 64;
		dy /= 64;
		INL_AdjustCursor(info,buttons,dx,dy);
	}
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_ReadControl() - Reads the device associated with the specified
//		player and fills in the control info struct
//
///////////////////////////////////////////////////////////////////////////
void near
IN_ReadControl(int pn,player_t *player)
{
			boolean		realdelta;
			byte		dbyte;
			word		buttons;
			int			dx,dy;
			Motion		mx,my;
			ControlType	type;
			sword conpee;
			byte dir=DirTable[2];
register	KeyboardDef	*def;

	dx = dy = 0;
	mx = my = motion_None;
	buttons = 0;

#if DEMO0
	if (DemoMode == demo_Playback)
	{
		dbyte = DemoBuffer[DemoOffset + 1];
		my = (dbyte & 3) - 1;
		mx = ((dbyte >> 2) & 3) - 1;
		buttons = (dbyte >> 4) & 3;

		if (!(--DemoBuffer[DemoOffset]))
		{
			DemoOffset += 2;
			if (DemoOffset >= DemoSize)
				DemoMode = demo_PlayDone;
		}

		realdelta = false;
	}
	else if (DemoMode == demo_PlayDone)
		Quit("Demo playback exceeded");
	else
	{
#endif
		if(testctrltype>0) printf("player[pn].Controls=%c\n", player[pn].Controls);
		switch (type = player[pn].Controls)
		{
		case ctrl_Keyboard1:
		case ctrl_Keyboard2:
			def = &(inpu.KbdDefs[type - ctrl_Keyboard]);

/*			if (Keyboard[def->upleft])
				mx = motion_Left,my = motion_Up;
			else if (Keyboard[def->upright])
				mx = motion_Right,my = motion_Up;
			else if (Keyboard[def->downleft])
				mx = motion_Left,my = motion_Down;
			else if (Keyboard[def->downright])
				mx = motion_Right,my = motion_Down;*/
			if(DIRECTIONIFELSE)
			{
			if(!inpu.Keyboard[def->left] && !inpu.Keyboard[def->right]){
				if((inpu.Keyboard[def->up] && !inpu.Keyboard[def->down]))
					my = motion_Up;
				if((inpu.Keyboard[def->down] && !inpu.Keyboard[def->up]))
					my = motion_Down;
			}else if(!inpu.Keyboard[def->up] && !inpu.Keyboard[def->down]){
				if((inpu.Keyboard[def->left] && !inpu.Keyboard[def->right]))
					mx = motion_Left;
				if((inpu.Keyboard[def->right] && !inpu.Keyboard[def->left]))// || player[pn].pdir != 1)
					mx = motion_Right;
			}else
				//if(mx+my!=1 && mx+my!=-1 && mx!=my!=0)
				{	//2 keys pressed
					switch (player[pn].pdir)
					{
						case 0:
						case 4:
							if((inpu.Keyboard[def->left] && !inpu.Keyboard[def->right])) dir = DirTable[1];
							else if((inpu.Keyboard[def->right] && !inpu.Keyboard[def->left])) dir = DirTable[3];
						break;
						case 1:
						case 3:
							if((inpu.Keyboard[def->up] && !inpu.Keyboard[def->down])) dir = DirTable[0];
							else if((inpu.Keyboard[def->down] && !inpu.Keyboard[def->up])) dir = DirTable[4];
						break;
						default:
						break;
					}
					if(testcontrolnoisy > 0){ printf("dir=%c ", dirchar(dir)); printf("pdir=%c	", dirchar(player[pn].pdir)); }
				}//else printf("				");
			}
			//input from player
			if (inpu.Keyboard[def->button0])
				buttons += 1 << 0;
			if (inpu.Keyboard[def->button1])
				buttons += 1 << 1;
			realdelta = false;
			break;
		case ctrl_Joystick1:
		case ctrl_Joystick2:
			INL_GetJoyDelta(type - ctrl_Joystick,&dx,&dy,false);
			buttons = INL_GetJoyButtons(type - ctrl_Joystick);
			realdelta = true;
			break;
		case ctrl_Mouse:
			INL_GetMouseDelta(&dx,&dy);
			buttons = INL_GetMouseButtons();
			realdelta = true;
			break;
		}
#ifdef DEMO0
	}
#endif

	if (realdelta)
	{
		mx = (dx < 0)? motion_Left : ((dx > 0)? motion_Right : motion_None);
		my = (dy < 0)? motion_Up : ((dy > 0)? motion_Down : motion_None);
	}
	else
	{
		dx = mx;// * 127;
		dy = my;// * 127;
	}

	player[pn].info.x = dx;
	player[pn].info.xaxis = mx;
	player[pn].info.y = dy;
	player[pn].info.yaxis = my;
	player[pn].info.button0 = buttons & (1 << 0);
	player[pn].info.button1 = buttons & (1 << 1);
	player[pn].info.button2 = buttons & (1 << 2);
	player[pn].info.button3 = buttons & (1 << 3);
//	player[pn].info.dir = DirTable[((my + 1) * 3) + (mx + 1)];
	conpee=(((my + 1) * 2) + (mx + 1))-1;
	player[pn].info.dir = DirTable[conpee];

	if(DirTable[conpee]!=2)	player[pn].pdir=DirTable[conpee];
	if(player[pn].q==1 &&( dir!=2 || (mx!=motion_None || my!=motion_None)))
	{
		if(dir==2) player[pn].d = player[pn].info.dir;
		else player[pn].d = DirTable[dir];
	}

#if DEMO0
	if (DemoMode == demo_Record)
	{
		// Pack the control info into a byte
		dbyte = (buttons << 4) | ((mx + 1) << 2) | (my + 1);

		if
		(
			(DemoBuffer[DemoOffset + 1] == dbyte)
		&&	(DemoBuffer[DemoOffset] < 255)
		)
			(DemoBuffer[DemoOffset])++;
		else
		{
			if (DemoOffset || DemoBuffer[DemoOffset])
				DemoOffset += 2;

			if (DemoOffset >= DemoSize)
				Quit("Demo buffer overflow");

			DemoBuffer[DemoOffset] = 1;
			DemoBuffer[DemoOffset + 1] = dbyte;
		}
	}
#endif
//#ifdef TESTCONTROLNOISY
if(testcontrolnoisy > 0)
if((inpu.Keyboard[def->up] || inpu.Keyboard[def->down] || inpu.Keyboard[def->left] || inpu.Keyboard[def->right]) || player[pn].q>1)
{
	printf("q=%d ", player[pn].q);
	printf("cpee=%c ", dirchar(conpee));
	//printf("(mx)=%d	", mx);
	//printf("(my)=%d	", my);
	//printf("[%d]	", mx+my);
	printf("pdir=%c d=%c dir=%c ", dirchar(player[pn].pdir), dirchar(player[pn].d), dirchar(player[pn].info.dir));
	printf("%c%d %c%d %c%d %c%d\n", dirchar(0), inpu.Keyboard[def->up], dirchar(4), inpu.Keyboard[def->down], dirchar(1), inpu.Keyboard[def->left], dirchar(3), inpu.Keyboard[def->right]);
}
//#endif
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_SetControlType() - Sets the control type to be used by the specified
//		player
//
///////////////////////////////////////////////////////////////////////////
void
IN_SetControlType(word pn,player_t *player,ControlType type)
{
	// DEBUG - check that requested type is present?
	player[pn].Controls = type;
}

#if DEMO0
///////////////////////////////////////////////////////////////////////////
//
//	IN_StartDemoRecord() - Starts the demo recording, using a buffer the
//		size passed. Returns if the buffer allocation was successful
//
///////////////////////////////////////////////////////////////////////////
boolean
IN_StartDemoRecord(word bufsize)
{
	if (!bufsize)
		return(false);

	MM_GetPtr((memptr *)&DemoBuffer,bufsize);
	DemoMode = demo_Record;
	DemoSize = bufsize & ~1;
	DemoOffset = 0;
	DemoBuffer[0] = DemoBuffer[1] = 0;

	return(true);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_StartDemoPlayback() - Plays back the demo pointed to of the given size
//
///////////////////////////////////////////////////////////////////////////
void
IN_StartDemoPlayback(byte /*__segment*/ *buffer,word bufsize)
{
	DemoBuffer = buffer;
	DemoMode = demo_Playback;
	DemoSize = bufsize & ~1;
	DemoOffset = 0;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_StopDemo() - Turns off demo mode
//
///////////////////////////////////////////////////////////////////////////
void
IN_StopDemo(void)
{
	if ((DemoMode == demo_Record) && DemoOffset)
		DemoOffset += 2;

	DemoMode = demo_Off;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_FreeDemoBuffer() - Frees the demo buffer, if it's been allocated
//
///////////////////////////////////////////////////////////////////////////
void
IN_FreeDemoBuffer(void)
{
	if (DemoBuffer)
		MM_FreePtr((memptr *)&DemoBuffer);
}
#endif


///////////////////////////////////////////////////////////////////////////
//
//	IN_GetScanName() - Returns a string containing the name of the
//		specified scan code
//
///////////////////////////////////////////////////////////////////////////
byte *
IN_GetScanName(ScanCode scan)
{
	byte		**p;
	ScanCode	far *s;

	for (s = ExtScanCodes,p = ExtScanNames;*s;p++,s++)
		if (*s == scan)
			return(*p);

	return(ScanNames[scan]);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_WaitForKey() - Waits for a scan code, then clears LastScan and
//		returns the scan code
//
///////////////////////////////////////////////////////////////////////////
ScanCode
IN_WaitForKey()
{
	ScanCode	result;

	while (!(result = inpu.LastScan))
		;
	inpu.LastScan = 0;
	return(result);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_WaitForASCII() - Waits for an ASCII char, then clears LastASCII and
//		returns the ASCII value
//
///////////////////////////////////////////////////////////////////////////
char
IN_WaitForASCII()
{
	char		result;

	while (!(result = inpu.LastASCII))
		;
	inpu.LastASCII = '\0';
	return(result);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_AckBack() - Waits for either an ASCII keypress or a button press
//
///////////////////////////////////////////////////////////////////////////
void
IN_AckBack()
{
	word	i;

	while (!inpu.LastScan)
	{
		if (inpu.MousePresent)
		{
			if (INL_GetMouseButtons())
			{
				while (INL_GetMouseButtons())
					;
				return;
			}
		}

		for (i = 0;i < MaxJoys;i++)
		{
			if (inpu.JoysPresent[i])
			{
				if (IN_GetJoyButtonsDB(i))
				{
					while (IN_GetJoyButtonsDB(i))
						;
					return;
				}
			}
		}
	}

	IN_ClearKey(inpu.LastScan);
	inpu.LastScan = sc_None;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_Ack() - Clears user input & then calls IN_AckBack()
//
///////////////////////////////////////////////////////////////////////////
void
IN_Ack()
{
	word	i;

	IN_ClearKey(inpu.LastScan);
	inpu.LastScan = sc_None;

	if (inpu.MousePresent)
		while (INL_GetMouseButtons())
					;
	for (i = 0;i < MaxJoys;i++)
		if (inpu.JoysPresent[i])
			while (IN_GetJoyButtonsDB(i))
				;

	IN_AckBack();
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_IsUserInput() - Returns true if a key has been pressed or a button
//		is down
//
///////////////////////////////////////////////////////////////////////////
boolean
IN_IsUserInput()
{
	boolean	result;
	word	i;

	result = inpu.LastScan;

	if (inpu.MousePresent)
		if (INL_GetMouseButtons())
			result = true;

	for (i = 0;i < MaxJoys;i++)
		if (inpu.JoysPresent[i])
			if (INL_GetJoyButtons(i))
				result = true;

	return(result);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_UserInput() - Waits for the specified delay time (in ticks) or the
//		user pressing a key or a mouse button. If the clear flag is set, it
//		then either clears the key or waits for the user to let the mouse
//		button up.
//
///////////////////////////////////////////////////////////////////////////
boolean
IN_UserInput(dword delay,boolean clear)
{
	word TimeCount = *clockw;
	word	lasttime;

	lasttime = TimeCount;
	do
	{
		if (IN_IsUserInput())
		{
			if (clear)
				IN_AckBack();
			return(true);
		}
	} while (TimeCount - lasttime < delay);
	return(false);
}

boolean IN_KeyDown(byte code)
{
	return inpu.Keyboard[code];
}

void IN_ClearKey(byte code)
{
	inpu.Keyboard[code] = false;
	if(code == inpu.LastScan)
		inpu.LastScan = sc_None;
	}

boolean IN_qb(byte kee)
{
//#ifdef TESTKEYIN
//	if(testkeyin > 0) printf("%u\n", inpu.Keyboard[kee]);
//#endif
	if(inpu.Keyboard[kee]==true) return 1;
	else return 0;
}

//init player!
void IN_initplayer(player_t *player, word pn)
{
	player[pn].x = player[pn].tx*TILEWH;
	player[pn].y = player[pn].ty*TILEWH;
	player[pn].triggerx = player[pn].tx;
	player[pn].triggery = player[pn].ty+1;
/*	player[0].info.x = player[0].tx;
	player[0].info.xaxis = player[0].tx*TILEWH;
	player[0].info.y = player[0].ty;
	player[0].info.yaxis = player[0].ty*TILEWH;*/
	player[pn].q=1;
	player[pn].d=2;
	player[pn].hp=4;
}
