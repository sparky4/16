#define JOYPORT 0x201
#define J1B1    0x10
#define J1B2    0x20
#define J2B1    0x40
#define J2B2    0x80
#define J1      0x01
#define J2      0x02
#define J1X     0x01
#define J1Y     0x02


unsigned char joybutton(unsigned char button)  {
    outp(JOYPORT,0);
    return ((unsigned char) (~inp(JOYPORT) &button));
}//end joybutton

int joy1b(int button);
    #pragma aux joy1b =                  \
    "mov dx, 0x201"                       \
    "out dx, al"                       \
    "in al, dx"                           \
    "and ax, bx"                        \
    "xor ax, bx"                           \
    parm [bx]                               \
    modify[dx ax]                            \
    value[ax];

extern unsigned int joystick( unsigned char _stick );
     #pragma aux joystick =                \
             "cli"                         \
             "mov ah,ch"                    \
             "xor al,al"                   \
             "xor cx,cx"                   \
             "mov dx,0x201"              \
             "out dx,al"                   \
             "charged:"                    \
             "in al,dx"                    \
             "test al,ah"                  \
             "loopne charged"              \
             "xor ax,ax"                   \
             "sub ax,cx"                   \
             "sti"                         \
             parm   [ch]                   \
             modify [ax];
                               

