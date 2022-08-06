#define PEL_READ_REG 	0x03C7   /* Color register, read address */
#define PEL_WRITE_REG 	0x03C8   /* Color register, write address */
#define PEL_DATA_REG 	0x03C9   /* Color register, data port */
#define PAL_SIZE	768	 /* Size of palette in bytes */
#define SC_INDEX 	0x03C4   /* Sequence Controller Index */
#define CRTC_INDEX 	0x03D4   /* CRT Controller Index */
#define MISC_OUTPUT 	0x03C2   /* Miscellaneous Output register */
#define SCREEN_SEG 	0x0a000  /* segment of display memory in mode X */
#define INPUT_STATUS_1 	0x03DA   /* Input Status 1 register */
#define ATC_INDEX 	0x03C0   /* Attribute Controller */
#define START_ADDRESS_HIGH 	0x0C     /* bitmap start address high byte */
#define START_ADDRESS_LOW 	0x0D     /* bitmap start address low byte */
#define GC_INDEX 	0x03CE
#define BIT_MASK 	0x08
#define MAP_MASK 	0x02

#define ALL_COPY_BITS 	0x00000+BIT_MASK
#define ALL_DRAW_BITS 	0x0FF00+BIT_MASK

#define SQUARE_WIDTH 16
#define SQUARE_HEIGHT 16
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define VIRTUAL_WIDTH 352
#define VIRTUAL_HEIGHT 240

#define PAGE_0 0
#define PAGE_1 0x05540  /* 05470h  ;5540h */
#define PAGE_2 0x0AA80  /* 0A8E0h  ;AA80h */

#define SCROLL_SPEED 1               /*  Don't let it go above 8! */
#define MAGIC_NUM 100

#define CPU8086 0
#define CPU80286 1
#define CPU80386 2
#define CPU80486 3

/* ====================================================================== */
/*                            Key Assignments */
/* ====================================================================== */
#define kESC 2
#define kONE 4
#define kTWO 6
#define kTHREE 8
#define kFOUR 10
#define kFIVE 12
#define kSIX 14
#define kSEVEN 16
#define kEIGHT 18
#define kNINE 20
#define kZERO 22
#define kMINUS 24
#define kEQUAL 26
#define kBACKSPACE 28
#define kTAB 30
#define kQ 32
#define kW 34
#define kE 36
#define kR 38
#define kT 40
#define kY 42
#define kU 44
#define kI 46
#define kO 48
#define kP 50
#define kL_BRACE 52
#define kR_BRACE 54
#define kENTER 56
#define kCTRL 58
#define kA 60
#define kS 62
#define kD 64
#define kF 66
#define kG 68
#define kH 70
#define kJ 72
#define kK 74
#define kL 76
#define kSEMICOLON 78
#define kQUOTE 80
#define kBACKQUOTE 82
#define kL_SHIFT 84
#define kBACKSLASH 86
#define kZ 88
#define kX 90
#define kC 92
#define kV 94
#define kB 96
#define kN 98
#define kM 100
#define kCOMMA 102
#define kPERIOD 104
#define kSLASH 106
#define kR_SHIFT 108
#define kGREY_STAR 110
#define kALT 112
#define kSPACE 114
#define kCAPSLOCK 116
#define kF1 118
#define kF2 120
#define kF3 122
#define kF4 124
#define kF5 126
#define kF6 128
#define kF7 130
#define kF8 132
#define kF9 134
#define kF10 136
#define kNUMLOCK 138
#define kSCRLLOCK 140
#define kHOME 142
#define kUP 144
#define kPAGE_UP 146
#define kGREY_MINUS 148
#define kLEFT 150
#define kPAD_FIVE 152
#define kRIGHT 154
#define kGREY_PLUS 156
#define kEND 158
#define kDOWN 160
#define kPAGE_DOWN 162
#define kINSERT 164
#define kDELETE 166

#define kF11 174
#define kF12 176


/* handy types */
typedef char byte;
typedef short word;
