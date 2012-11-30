#include "src\eng_core.h"
#include <list>
#include "src\eng_snd.h"
#include "src\eng_gfx.h"
#include "src\eng_comm.h"
#include "src\eng_ai.h"

#include <iostream>

#include "src\lib\dos_gfx.h"
#include "src\lib\opl2.h"
#include "src\lib\dos_comm.h"

namespace engine{

	Core::Core()
	{
		// Hardware Initiation
		this->init();
		
		//default constructor
		this->fp = new std::list<void(Core::*)()>;
		this->msg = ENGINE_RUN;
		this->timer.start_timer();
		this->frames_per_second = 60;

		this->fp->push_back(&Core::sound);
		this->fp->push_back(&Core::graphics);
		this->fp->push_back(&Core::comm);
		this->fp->push_back(&Core::ai);

		//this->fp.push_back(&core::sound);
		//this->fp.push_back(&core::graphics);
		//this->fp.push_back(&core::comm);
		//this->fp.push_back(&engine::core::ai);

		// Global Variables
		this->qing = 4; // test global variable
		this->x = 0; // X
		this->y = 0; // Y
	}

	void Core::update()
	{
		for (std::list<void(Core::*)()>::iterator i = fp->begin(); i != fp->end(); i++)
		//for (std::list<void(core::*)()>::iterator i = this->fp.begin(); i != this->fp.end(); i++)
		{
			
			(this->*(*i))();
		}

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
		this->msg = do_communication(scankey());
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
//tt			std::cout << this->qing << std::endl;
		}
	}

	bool Core::init(){
		bool xz = 0; // error switch...
		std::cout << "VIDEO INITIATION" << std::endl;
		setvideo(0x13, 1);
		setvbuff(1);
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
		std::cout << "Press press any key to continue!" << std::endl;
		getch();
		std::cout << "INPUT INITIATION" << std::endl;
		setkb(1);
		std::cout << "INITIATION" << std::endl;
		return xz;
	}

	void Core::release()
	{
		//release contexts and set engine to exit
		setvideo(0x03, 0);
		setvbuff(0);
		setkb(0);
		FMReset();
		this->msg = ENGINE_EXIT;
	}

	void Core::input(){
		//retrieve input device information
		//dummy 'w' and 'p' down, t up
//----		bool dummy_input_available = true;
//----		if(dummy_input_available)
		//if (scankey() <= 128)
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
			this->fp->push_back(&Core::graphics);
			this->timer.start_timer();
//====			std::cout << "GRAPHICS GRAPHICS GRAPHICS GRAPHICS GRAPHICS" << std::endl;
		}
		else
		{
			this->fp->remove(&Core::graphics);
		}
		if (ENGINE_INPUT == this->msg)
		{
			this->fp->push_back(&Core::comm);
		}
		else
		{
			this->fp->remove(&Core::comm);
		}
		if (ENGINE_QUIT == this->msg)
		{
			this->fp->push_back(&Core::release);
		}
	}

	Core::~Core()
	{
		//deconstructor
		delete(this->fp);
	}

}
