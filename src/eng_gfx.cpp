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
		static int bing = 10;
		static int paldump = 0;
		//if(12 >= kee && kee >= 2) bing = kee - 1;
		/*if(keyp(2)) bing = 1;
		if(keyp(3)) bing = 2;
		if(keyp(4)) bing = 3;
		if(keyp(5)) bing = 4;
		if(keyp(6)) bing = 5;
		if(keyp(7)) bing = 6;
		if(keyp(8)) bing = 7;
		if(keyp(9)) bing = 8;
		if(keyp(10)) bing = 9;
		if(keyp(11)) bing = 10;
		if(keyp(12)) bing = 11;*/
		if(keyp(47) && paldump == 8) paldump = 0;
			else if(keyp(47)) paldump++;
		if(keyp(57) && bing == 10) bing = 0;
			else if(keyp(57) && bing == 0) bing = 10;
		if(bing > 0){
			ding(bing);
		}else{
			if(keyp(KEY_C)) tulip();
			if(keyp(KEY_UP) || keyp(KEY_DOWN) || keyp(KEY_LEFT) || keyp(KEY_RIGHT)) etesuto(xxxx, yyyy);
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
			qtesuto(xxxx, yyyy, 14);
		}
		if(paldump) pdump(paldump);
		/*clearscr();
		BlockMove();*/
	}

}
