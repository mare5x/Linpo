#pragma once

#include <SDL.h>


class Timer
{
public:
	Timer();

	void start();
	void stop();

	Uint32 get_ticks();

	bool is_started();
private:
	Uint32 start_ticks;
	bool started;
};
