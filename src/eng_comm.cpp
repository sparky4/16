#include "src\eng_comm.h"
#include <iostream>
		int xxxx = 0;
		int yyyy = 0;
		int qing = 4;

namespace engine {

	/*engine_message do_communication()
	{
		//communication here
//====		std::cout << "COMMUNICATION HERE" << std::endl;
		int msg;
		std::cout << "Enter 1 for continue." << std::endl;
		std::cin >> msg;
		return msg == 1 ? ENGINE_RUN : ENGINE_QUIT;
	}*/
	/*engine_message do_communication(bool *input)
	{
		//communication here
//====		std::cout << "COMMUNICATION HERE" << std::endl;
		return input['e'] == true ? ENGINE_QUIT : ENGINE_RUN;
	}*/
	engine_message do_communication(byte input)
	{
		//communication here
//====		std::cout << "COMMUNICATION HERE" << std::endl;
		if(input == 44){
			FMKeyOn(0, 0x106, 4);
		}else if(input == 172) FMKeyOff(0);
		if(input == KEY_X){
			sound(440);
			delay(100);
			nosound();
			//delay(50);
		}else if(input == KEY_X+128) nosound();
		if(12 >= input && input >= 2) qing = input - 1;
		if(input == 57) qing = 0;
		
		if(qing > 0){
			ding(qing);
		}else{
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
		//if(input) printf("%03d\n", input);
		return input == 1 ? ENGINE_QUIT : ENGINE_RUN;
	}

}
