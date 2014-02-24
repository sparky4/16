#include "src\eng_core.h"

#include "src\eng_snd.h"
#include "src\eng_gfx.h"
#include "src\eng_comm.h"
#include "src\eng_ai.h"

#include <iostream>

//#include "src\lib\dos_gfx.h"
//#include "src\lib\opl2.h"
//#include "src\lib\dos_comm.h"

namespace engine{

	Core::Core()
	{
		// Hardware Initiation
		this->init();
		
		//default constructor
		this->msg = ENGINE_RUN;
		this->timer.start_timer();
		this->frames_per_second = 60;

		// Global Variables
		this->mode = 4; // test global variable
		this->x = 0; // X
		this->y = 0; // Y
		
		// Start setttings of flags.  I hope I got the same
		// logic as it was before, with the STL list.
		this->graphics_flag = true;
		this->comm_flag = true;
		this->release_flag = false;
	}

	void Core::update()
	{
		/*for (std::list<void(Core::*)()>::iterator i = fp->begin(); i != fp->end(); i++)
		//for (std::list<void(core::*)()>::iterator i = this->fp.begin(); i != this->fp.end(); i++)
		{
			(this->*(*i))();
		}*/
	  // call update functions, instead of traversing a function pointer
	  // list.  Remove this comment!
	  sound();
	  if ( graphics_flag )
		  graphics();
	  if ( comm_flag )
		  comm();
	  ai();
	  if ( release_flag )
		  release();
	}

	void Core::sound()
	{
		//wrap sound lib
		do_sound();
	}
	void Core::graphics()
	{
		//int num_frames = 60;
		//double fps = 1.0 / num_frames;
		//double now = this->timer.elapsed_timer();
		//std::cout << "fps: " << fps << "      now: " << now << std::endl;
		//if (this->timer.elapsed_timer() >= fps)
		//{
		//wrap graphics lib
		do_graphics();
		//this->timer.start_timer();
		//}
	}
	void Core::comm()
	{
		//int num_frames = 60;
		//double fps = 1.0 / num_frames;
		//double now = this->timer.elapsed_timer();
		//std::cout << "fps: " << fps << "      now: " << now << std::endl;
		//if (this->timer.elapsed_timer() >= fps)
		//{
		//wrap comm lib
		this->msg = do_communication();
		//this->timer.start_timer();
		//}
	}
	void Core::ai()
	{
		//wrap A.I. lib
		do_artificial_intelligence();
	}

	engine_message Core::_msg()
	{
		return this->msg;
	}

	void Core::run()
	{
//----		int i = 0;
		while (ENGINE_EXIT != this->msg)
		{
			//next line for purely testing purposes
//----			i++;if(i==600){char a;std::cin >> a;this->keeq[a] = true;i=0;}
			this->sync();
			this->input();
			this->update();
		}
	}

	bool Core::init(){
		bool xz = 0; // error switch...
		std::cout << "VIDEO INITIATION" << std::endl;
//____working on mode x		setvideo(0x13, 1);
//____working on mode x		setvbuff(1);
		std::cout << "Checking for Adlib sound card...." << std::endl;
		if(!AdlibExists()){
			std::cout << "not found." << std::endl;
			xz = 1;
		}
		std::cout << "found." << std::endl;
		std::cout << "OPL2 INITIATION" << std::endl;
		fmtest();
		std::cout << "\nPress ESC to quit the game engine core test." << std::endl;
		std::cout << "1 - 9 for graphical tests!" << std::endl;
		std::cout << "space bar for control and sprite test!" << std::endl;
		std::cout << "z key for noise!" << std::endl;
		//std::cout << "Press press any key to continue!" << std::endl;
		//getch();
		std::cout << "INPUT INITIATION" << std::endl;
		setkb(1);
		std::cout << "INITIATION" << std::endl;
		return xz;
	}

	void Core::release()
	{
		//release contexts and set engine to exit
//____working on mode x		setvideo(0x03, 0);
//____working on mode x		setvbuff(0);
		setkb(0);
		FMReset();
		this->msg = ENGINE_EXIT;
	}

	void Core::input(){
		//retrieve input device information
		//dummy 'w' and 'p' down, t up
//----		bool dummy_input_available = true;
//----		if(dummy_input_available)
//----		{
//----			std::cout << "key down" << std::endl;
//----			this->keeq['w'] = true;
//----			this->keeq['p'] = true;
//----			this->keeq['t'] = false;
			//notify engine that input occured
			this->msg = ENGINE_INPUT;
//----		}
//----		else
//----		{
//----			std::cout << "key up" << std::endl;
//----			this->msg = ENGINE_RUN;
//----		}
	}

	void Core::sync()
	{
		if (this->timer.elapsed_timer() >= (1.0 / this->frames_per_second))
		{
			//int a;std::cin >> a;
			graphics_flag = true;
			this->timer.start_timer();
//====			std::cout << "GRAPHICS GRAPHICS GRAPHICS GRAPHICS GRAPHICS" << std::endl;
//____			updatevbuff();
		}
		else
		{
			graphics_flag = false;
		}
		if (ENGINE_INPUT == this->msg)
		{
			comm_flag = true;
		}
		else
		{
			comm_flag = false;
		}
		if (ENGINE_QUIT == this->msg)
		{
			release_flag = true;
		}
	}

	Core::~Core()
	{
		//deconstructor
	}

}
