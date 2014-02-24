#include "src\eng_gfx.h"
#include <iostream>

#ifdef __cplusplus      /* Functions must be declared C style */
extern "C" {
#endif
extern void interrupt far newkb(void);
extern int keyp(byte c);
#ifdef __cplusplus
}
#endif

namespace engine {

	void do_graphics()
	{
		//graphics here
//====		std::cout << "GRAPHICS HERE" << std::endl;
		static int xxxx = 0; //I am confused!!!
		static int yyyy = 0; //I am confused!!!
		//static int mode = 10; //I am confused!!!
		static int paldump = 0; //switch variable
		//if(12 >= kee && kee >= 2) mode = kee - 1;
		/*if(keyp(2)) mode = 1;
		if(keyp(3)) mode = 2;
		if(keyp(4)) mode = 3;
		if(keyp(5)) mode = 4;
		if(keyp(6)) mode = 5;
		if(keyp(7)) mode = 6;
		if(keyp(8)) mode = 7;
		if(keyp(9)) mode = 8;
		if(keyp(10)) mode = 9;
		if(keyp(11)) mode = 10;
		if(keyp(12)) mode = 11;*/
//____		if(keyp(47) && paldump == 8) paldump = 0;
//____			else if(keyp(47)) paldump++;
		/* working on code overall!
		if(keyp(57) && mode == 10) mode = 0;
			else if(keyp(57) && mode == 0) mode = 10;
		if(mode > 0){
			ding(mode);
		}else{*/
//____			if(keyp(KEY_C)) tulip();
/*____			if(keyp(KEY_UP) || keyp(KEY_DOWN) || keyp(KEY_LEFT) || keyp(KEY_RIGHT)) eraseplayer(xxxx, yyyy);
			if(keyp(KEY_UP)){
				if(--yyyy < 0) yyyy = 0;
			}else if(keyp(KEY_DOWN)){
				if(++yyyy >= SH) yyyy = SH - 1;
			}
			if(keyp(KEY_LEFT)){
				if(--xxxx < 0) xxxx = 0;
			}
			if(keyp(KEY_RIGHT)){
				if(++xxxx >= SW) xxxx = SW - 1;
			}
			drawplayer(xxxx, yyyy, 14);
		//}
		if(paldump) pdump(paldump);____*/
		/*clearscr();
		BlockMove();*/
	}

}
