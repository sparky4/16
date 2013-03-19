#ifndef _ENGINE_CORE_H_
#define _ENGINE_CORE_H_

#include "src\engine.h"
#include "src\timer.h"

namespace engine {
	class Core
	{
	public:
		Core();
		void update();
		void sound();
		void graphics();
		void comm();
		void ai();
		engine_message _msg();
		void run();
		bool init();
		void release();
		void input();
		void sync();
		~Core();

	protected:
	private:
		engine_message msg;
		Timer timer;
//		unsigned char key[256];
		int frames_per_second;
		//std::list<void(core::*)()> fp;
/*		int xxxx = 0;
		int yyyy = 0;*/
		int mode;
		int x;
		int y;

		// Because we removed the function pointer STL list:
		bool graphics_flag, comm_flag, release_flag;
	};
}

#endif/*_ENGINE_CORE_H_*/
