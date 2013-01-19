#include "src\eng_snd.h"
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

	void do_sound()
	{
		//sound here
//====		std::cout << "SOUND HERE" << std::endl;
		if(keyp(44)){
			FMKeyOn(0, 0x106, 4);
		}//else if(!keyp(44)) FMKeyOff(0);
		if(keyp(KEY_A)) FMKeyOff(0);
		if(keyp(KEY_X)){
			sound(440);
			delay(100);
			nosound();
		}else if(keyp(KEY_X+0x80)) nosound();
	}

}
