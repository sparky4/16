
#ifndef __C_UTILS_H
#define __C_UTILS_H


	/* Misc Constants */

#define True     -1
#define False    0
#define nil      0

	/* Color Constants */

#define c_BLACK   0
#define c_BLUE    1
#define c_GREEN   2
#define c_CYAN    3
#define c_RED     4
#define c_PURPLE  5
#define c_BROWN   6
#define c_WHITE   7
#define c_GREY    8
#define c_bBLUE   9
#define c_bGREEN  10
#define c_bCYAN   11
#define c_bRED    12
#define c_bPURPLE 13
#define c_YELLOW  14
#define c_bWHITE  15
#define c_BRIGHT  16


#define Ky_F1     0x3B00
#define Ky_F2     0x3C00
#define Ky_F3     0x3D00
#define Ky_F4     0x3E00
#define Ky_F5     0x3F00
#define Ky_F6     0x4000
#define Ky_F7     0x4100
#define Ky_F8     0x4200
#define Ky_F9     0x4300
#define Ky_F10    0x4400

#define Ky_Up     0x4800
#define Ky_Left   0x4B00
#define Ky_Right  0x4D00
#define Ky_Down   0x5000
#define Ky_SUp    0xC800
#define Ky_SLeft  0xCB00
#define Ky_SRight 0xCD00
#define Ky_SDown  0xD000

#define Ky_Home   0x4700
#define Ky_End    0x4F00
#define Ky_PgUp   0x4900
#define Ky_PgDn   0x5100
#define Ky_SHome  0xC700
#define Ky_SEnd   0xCF00
#define Ky_SPgUp  0xC900
#define Ky_SPgDn  0xD100

#define Ky_Ins    0x5200
#define Ky_Del    0x5300
#define Ky_SIns   0xC200
#define Ky_SDel   0xC300

#define Ky_Tab    0x0009
#define Ky_RvsTab 0x8F00
#define Ky_STab   0x8F00

#define Ky_BS     0x0008
#define Ky_CR     0x000D
#define Ky_ESC    0x001B
#define Ky_Clr    0x007F

#define Ky_Plus   0x002D
#define Ky_Minus  0x002B

#define Ky_AltA   0x1E00
#define Ky_AltB   0x3000
#define Ky_AltC   0x2E00
#define Ky_AltD   0x2000
#define Ky_AltE   0x1200
#define Ky_AltF   0x2100
#define Ky_AltG   0x2200
#define Ky_AltH   0x2300
#define Ky_AltI   0x1700
#define Ky_AltJ   0x2400
#define Ky_AltK   0x2500
#define Ky_AltL   0x2600
#define Ky_AltM   0x3200
#define Ky_AltN   0x3100
#define Ky_AltO   0x1800
#define Ky_AltP   0x1900
#define Ky_AltQ   0x1000
#define Ky_AltR   0x1300
#define Ky_AltS   0x1F00
#define Ky_AltT   0x1400
#define Ky_AltU   0x1600
#define Ky_AltV   0x2F00
#define Ky_AltW   0x1100
#define Ky_AltX   0x2D00
#define Ky_AltY   0x1500
#define Ky_AltZ   0x2C00

	/* .ASM Functions From C_UTILS.ASM */

void far pascal dos_print (char far *Text);
void far pascal dos_prints (char far *Text);
void far pascal set_video_mode (int Mode);
int  far pascal scan_keyboard (void);
int  far pascal random_int (int MaxValue);
void far pascal init_random (void);
int  far pascal int_sqr (int X, int Round);
int  far pascal timer_count (void);

#endif
