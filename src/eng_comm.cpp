#include "src\eng_comm.h"
#include <iostream>

namespace engine {

	engine_message do_communication()
	/*{
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
	//engine_message do_communication(byte input)
	{
		//communication here
//====		std::cout << "COMMUNICATION HERE" << std::endl;
		//if(input) printf("%03d\n", input);
		return keyp(1) == 1 ? ENGINE_QUIT : ENGINE_RUN;
	}

}