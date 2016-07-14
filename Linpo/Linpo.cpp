#include "sharedfunctions.h"
#include "constants.h"
#include "globals.h"
#include "grid.h"
#include "fpstimer.h"


bool init()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
	{
		log("Error with SDL_Init: ", SDL_GetError());
		return false;
	}

	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	{
		log("Warning: Linear texture filtering not enabled!");
	}

	mainWindow = SDL_CreateWindow("Linpo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (mainWindow == nullptr)
	{
		log("Error creating mainWindow: ", SDL_GetError());
		return false;
	}
	else
	{
		if (VSYNC_ENABLED)
			mainRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		else
			mainRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED);

		if (mainRenderer == nullptr)
		{
			log("Error creating mainRenderer: ", SDL_GetError());
			return false;
		}
		else
		{
			SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		}
	}

	return true;
}


void close()
{
	mainRenderer = nullptr;
	SDL_DestroyRenderer(mainRenderer);

	mainWindow = nullptr;
	SDL_DestroyWindow(mainWindow);
	SDL_Quit();
}


int main(int argc, char* argv[])
{
	if (init())
	{
		bool quit = false;
		SDL_Event e;

		Grid game_grid(10, 10);
		Timer fps_cap_timer;

		while (!quit)
		{
			if (!VSYNC_ENABLED) fps_cap_timer.start();

			if (SDL_WaitEvent(&e))
			{
				do {
					game_grid.handle_event(e);

					if (e.type == SDL_QUIT)
						quit = true;

				} while (SDL_PollEvent(&e) != 0);
			}

			SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_RenderClear(mainRenderer);

			// Code below
			game_grid.update();
			game_grid.render();

			SDL_RenderPresent(mainRenderer);

			if (!VSYNC_ENABLED)
			{
				auto frame_ticks = fps_cap_timer.get_ticks();
				if (frame_ticks < TICKS_PER_FRAME)
					SDL_Delay(TICKS_PER_FRAME - frame_ticks);
			}
		}
	}

	close();
	return 0;
}