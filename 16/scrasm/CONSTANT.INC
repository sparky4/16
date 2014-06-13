PEL_READ_REG    EQU     03C7h   ;Color register, read address
PEL_WRITE_REG   EQU     03C8h   ;Color register, write address
PEL_DATA_REG    EQU     03C9h   ;Color register, data port
SC_INDEX        equ     03C4h   ;Sequence Controller Index
CRTC_INDEX      equ     03D4h   ;CRT Controller Index
MISC_OUTPUT     equ     03C2h   ;Miscellaneous Output register
SCREEN_SEG      equ     0a000h  ;segment of display memory in mode X
INPUT_STATUS_1  equ     03DAh   ;Input Status 1 register
ATC_INDEX       equ     03C0h   ;Attribute Controller
START_ADDRESS_HIGH equ  0Ch     ;bitmap start address high byte
START_ADDRESS_LOW equ   0Dh     ;bitmap start address low byte
GC_INDEX        EQU     03CEh
BIT_MASK        EQU     08h
MAP_MASK        EQU     02h

ALL_COPY_BITS   EQU     00000h+BIT_MASK
ALL_DRAW_BITS   EQU     0FF00h+BIT_MASK

SQUARE_WIDTH    EQU     16
SQUARE_HEIGHT   EQU     16
SCREEN_WIDTH    EQU     320
SCREEN_HEIGHT   EQU     240
VIRTUAL_WIDTH   EQU     352
VIRTUAL_HEIGHT  EQU     240

PAGE_0          EQU     0
PAGE_1          EQU     05540h  ;05470h  ;5540h
PAGE_2          EQU     0AA80h  ;0A8E0h  ;AA80h

SCROLL_SPEED    EQU     1               ; Don't let it go above 8!
MAGIC_NUM       EQU     100

CPU8086         EQU     0
CPU80286        EQU     1
CPU80386        EQU     2
CPU80486        EQU     3

;======================================================================
;                           Key Assignments
;======================================================================
kESC            EQU       2
kONE            EQU       4
kTWO            EQU       6
kTHREE          EQU       8
kFOUR           EQU      10
kFIVE           EQU      12
kSIX            EQU      14
kSEVEN          EQU      16
kEIGHT          EQU      18
kNINE           EQU      20
kZERO           EQU      22
kMINUS          EQU      24
kEQUAL          EQU      26
kBACKSPACE      EQU      28
kTAB            EQU      30
kQ              EQU      32
kW              EQU      34
kE              EQU      36
kR              EQU      38
kT              EQU      40
kY              EQU      42
kU              EQU      44
kI              EQU      46
kO              EQU      48
kP              EQU      50
kL_BRACE        EQU      52
kR_BRACE        EQU      54
kENTER          EQU      56
kCTRL           EQU      58
kA              EQU      60
kS              EQU      62
kD              EQU      64
kF              EQU      66
kG              EQU      68
kH              EQU      70
kJ              EQU      72
kK              EQU      74
kL              EQU      76
kSEMICOLON      EQU      78
kQUOTE          EQU      80
kBACKQUOTE      EQU      82
kL_SHIFT        EQU      84
kBACKSLASH      EQU      86
kZ              EQU      88
kX              EQU      90
kC              EQU      92
kV              EQU      94
kB              EQU      96
kN              EQU      98
kM              EQU     100
kCOMMA          EQU     102
kPERIOD         EQU     104
kSLASH          EQU     106
kR_SHIFT        EQU     108
kGREY_STAR      EQU     110
kALT            EQU     112
kSPACE          EQU     114
kCAPSLOCK       EQU     116
kF1             EQU     118
kF2             EQU     120
kF3             EQU     122
kF4             EQU     124
kF5             EQU     126
kF6             EQU     128
kF7             EQU     130
kF8             EQU     132
kF9             EQU     134
kF10            EQU     136
kNUMLOCK        EQU     138
kSCRLLOCK       EQU     140
kHOME           EQU     142
kUP             EQU     144
kPAGE_UP        EQU     146
kGREY_MINUS     EQU     148
kLEFT           EQU     150
kPAD_FIVE       EQU     152
kRIGHT          EQU     154
kGREY_PLUS      EQU     156
kEND            EQU     158
kDOWN           EQU     160
kPAGE_DOWN      EQU     162
kINSERT         EQU     164
kDELETE         EQU     166

kF11            EQU     174
kF12            EQU     176
