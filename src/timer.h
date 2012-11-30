#ifndef _TIMER_H_
#define _TIMER_H_
#include <time.h>

namespace engine {
	class Timer
	{
	public:
		Timer();
		clock_t start_timer();
		double elapsed_timer();
		double time_in_seconds();
		double time_in_seconds(time_t in_t);
		clock_t _time();
		~Timer();
	protected:
	private:
		clock_t t;
	};
}

#endif