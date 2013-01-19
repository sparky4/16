#ifndef _ENGINE_CORE_H_
#define _ENGINE_CORE_H_

#include <list>
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
		std::list<void(Core::*)()> *fp;
		Timer timer;
//		unsigned char key[256];
		int frames_per_second;
		//std::list<void(core::*)()> fp;
/*		int xxxx = 0;
		int yyyy = 0;*/
		int bing;
		int x;
		int y;
	};
}

#endif/*_ENGINE_CORE_H_*/