#include "fps_timer.h"


Timer::Timer()
{
	start_ticks = 0;
	started = false;
}

void Timer::start()
{
	start_ticks = SDL_GetTicks();
	started = true;
}

void Timer::stop()
{
	started = false;
	start_ticks = 0;
}

Uint32 Timer::get_ticks()
{
	if (started)
	{
		return SDL_GetTicks() - start_ticks;
	}
	return 0;
}

bool Timer::is_started()
{
	return started;
}
