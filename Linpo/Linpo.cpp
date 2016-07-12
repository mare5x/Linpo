#include "sharedfunctions.h"
#include "constants.h"
#include "globals.h"
#include "grid.h"


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
	if (mainWindow == NULL)
	{
		log("Error creating mainWindow: ", SDL_GetError());
		return false;
	}
	else
	{
		mainRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (mainRenderer == NULL)
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
	mainRenderer = NULL;
	SDL_DestroyRenderer(mainRenderer);

	mainWindow = NULL;
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

		while (!quit)
		{
			while (SDL_PollEvent(&e))
			{
				game_grid.handle_event(e);

				if (e.type == SDL_QUIT)
					quit = true;
			}

			SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_RenderClear(mainRenderer);

			// Code below
			game_grid.update();
			game_grid.render();

			SDL_RenderPresent(mainRenderer);
		}
	}

	close();
	return 0;
}