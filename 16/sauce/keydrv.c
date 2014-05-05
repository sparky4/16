#include<conio.h>
#include<dos.h>

#include "keydrv.h"

extern int pressed;
extern char keymap[];

static void (__interrupt __far  * old9h ) () ;  //holds dos key int



void __interrupt __far Key_handler()  {
    int scan,cv;

    irenable();
    scan = inp(0x60);                //read scan code
    cv = inp(0x61);
    outp( 0x61, cv | 0x80 );
    outp( 0x61, cv );
    outp(0x20, 0x20);  //reset key int
    
    if(scan > 127)  {               //on break set keymap 0, dec pressed
        keymap[(scan & 127)] = 0;
        pressed -= 1;
    }
    else if(keymap[scan] == 0)  {  //on make set keymap 1, inc pressed
            pressed += 1;           //if not already set
        keymap[scan] = 1;
    }
    
}

void hook_keys()  {
    int i;
    
    for(i=0; i<128; i++)  //set all keys to off
        keymap[i] = 0;
    pressed = 0;  //set number keys pressed to 0
        
    old9h = _dos_getvect ( 0x9 ); //save old key int
    _dos_setvect ( 0x9 , Key_handler );  //set key int to new handler
}

void release_keys()  {
    _dos_setvect ( 0x9 , old9h ) ;  // Return to DOS keyboard driver
}

void waitkey(int key)  {
    int temp = 0;
    while(!temp)
        if(keymap[key])
            temp = 1;
}

void waitkeyup(int key)  {
    int temp = 0;
    while(!temp)
        if(!keymap[key])
            temp = 1;
}//end waitkeyup



