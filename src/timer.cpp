#include "src\timer.h"
#include <time.h>

namespace engine {
	Timer::Timer()
	{
		//default constructor
	}

	clock_t Timer::start_timer()
	{
		//time(&(this->t));
		this->t = clock();
		return this->t;
	}

	double Timer::elapsed_timer()
	{
		return static_cast<double>(clock() - this->t) / CLOCKS_PER_SEC;//difftime(time(NULL), this->t);
	}

	double Timer::time_in_seconds()
	{
		return static_cast<double>(this->t) / CLOCKS_PER_SEC;
	}

	double Timer::time_in_seconds(time_t in_t)
	{
		return static_cast<double>(in_t) / CLOCKS_PER_SEC;
	}

	clock_t Timer::_time()
	{
		return this->t;
	}

	Timer::~Timer()
	{
		//deconstructor
	}
}