/* Project 16 Source Code~
 * Copyright (C) 2012-2017 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
 *
 * This file is part of Project 16.
 *
 * Project 16 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Project 16 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 * write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA 02110-1301 USA.
 *
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
#include "src/lib/16_head.h"
#include "src/lib/16_timer.h"
#ifdef __WATCOMC__	//borland C BCEXMM.EXE
#include "src/lib/16_dbg.h"
#include "src/lib/16_spri.h"
#include "src/lib/16_enti.h"
#endif

#define	KeyInt	9	// The keyboard ISR number

// Stuff for the joystick
#define	JoyScaleMax		32768
#define	JoyScaleShift	8
#define	MaxJoyValue		5000

//#define	MaxPlayers		2//future plans for multiple playable charaters and being able to alternate
#define	MaxKbds		2
#define	MaxJoys		2
#define	NumCodes	128

//typedef	byte		ScanCode;
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
//moved to 16_tdef.h
/*typedef	enum		{
						//ctrl_None,				// MDM (GAMERS EDGE) - added
						ctrl_Keyboard,
							ctrl_Keyboard1 = ctrl_Keyboard,ctrl_Keyboard2,
						ctrl_Joystick,
							ctrl_Joystick1 = ctrl_Joystick,ctrl_Joystick2,
						ctrl_Mouse,
					} ControlType;
typedef	enum		{
						motion_Left = -1,motion_Up = -1,
						motion_None = 0,
						motion_Right = 1,motion_Down = 1
					} Motion;
typedef	enum		{
						dir_North,//dir_NorthEast,
						dir_West,//dir_Nortinest,
						dir_None,
						dir_East,//,dir_SouthEast,
						dir_South,//dir_Soutinest,
					} Direction;
typedef	struct		{
						boolean	near	button0,button1,button2,button3;
						int	near		x,y;
						Motion	near	xaxis,yaxis;
						Direction near	dir;
					} CursorInfo;

typedef	struct		{
						ScanCode near	button0,button1,
									//upleft,
									up,
									down,
									left,
									right
									//upright,
									//downleft,
									//,downright
									;
					} KeyboardDef;
typedef	struct		{
						word	near	joyMinX,joyMinY,
									threshMinX,threshMinY,
									threshMaxX,threshMaxY,
									joyMaxX,joyMaxY,
									joyMultXL,joyMultYL,
									joyMultXH,joyMultYH;
					} JoystickDef;*/

/*
=============================================================================

					GLOBAL VARIABLES

=============================================================================
*/
#ifdef DEMO0
		static Demo		DemoMode = demo_Off;
		static byte /*_seg*/	*DemoBuffer;
		static word		DemoOffset,DemoSize;
#endif

//	Internal routines
extern	void		IN_Startup(void),IN_Shutdown(void),
					IN_Default(boolean gotit,player_t *player,ControlType nt),
					IN_SetKeyHook(void (*)()),
					IN_ClearKeysDown(void),
					IN_ReadCursor(CursorInfo *),
					IN_ReadControl(player_t *player),
					IN_SetControlType(player_t *player,ControlType type),
					IN_GetJoyAbs(word joy,word *xp,word *yp),
					IN_SetupJoy(word joy,word minx,word maxx,
								word miny,word maxy),
#if DEMO0
					IN_StopDemo(void),IN_FreeDemoBuffer(void),
#endif
					IN_Ack(void),IN_AckBack(void);
extern	boolean		IN_UserInput(word delay);
extern	char		IN_WaitForASCII(void);
extern	ScanCode	IN_WaitForKey(void);
extern	word		IN_GetJoyButtonsDB(word joy);
extern	byte		*IN_GetScanName(ScanCode);


byte	IN_MouseButtons (void);
byte	IN_JoyButtons (void);

void INL_GetJoyDelta(word joy,int *dx,int *dy/*,boolean adaptive*/);
void IN_StartAck(void);
boolean IN_CheckAck (void);
boolean IN_IsUserInput();
#define Mouse(x)         INL_Mouse(x)
//void IN_SetKeyHook(void (*hook)());
#if DEMO0
boolean IN_StartDemoRecord(word bufsize);
void IN_StartDemoPlayback(byte *buffer,word bufsize);
void IN_StopDemo(void);
void IN_FreeDemoBuffer(void);
#endif

boolean	IN_KeyDown(byte code),
		IN_qb(byte kee);
void		IN_ClearKey(byte code),
		IN_KbdLED();
ScanCode	IN_GetLastScan(),
		IN_GetCurCode();

#endif
