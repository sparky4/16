/**********************************************
 to use keydrv must include global variables:
    int pressed;       //globals for key driver (# keys pressed)
    char keymap[128];  //map of all keys and their state
***********************************************/

#ifndef keydrv_h
#define keydrv_h


void irdisable();
#pragma aux irdisable = \
    "cli";
    
void irenable();
#pragma aux irenable = \
    "sti";

void hook_keys();    //installs keydrv,saves dos driver,0's out globals
void release_keys(); //reinstalls dos driver
void waitkey(int key); //waits for a key to be pressed then returns
void waitkeyup(int key);  //waits for key specified by parameter
                           //to be released

//scan code defines
#define ESC      1
#define KEY1     2
#define KEY2     3
#define KEY3     4
#define KEY4     5
#define KEY5     6
#define KEY6     7
#define KEY7     8
#define KEY8     9
#define KEY9    10
#define KEY0    11
#define KEYQ    16
#define KEYA    30
#define KEYJ    36
#define KEYL    38
#define LSHIFT  42
#define KEYZ    44
#define SPACE   57
#define UP      72
#define LEFT    75
#define RIGHT   77
#define DOWN    80

#endif


