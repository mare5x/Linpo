#include "SDL_ttf.h"

#include "shared_functions.h"
#include "constants.h"
#include "globals.h"
#include "grid.h"
#include "fps_timer.h"
#include "linpo.h"
#include "score_board.h"


bool init()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
	{
		mlog("Error with SDL_Init: ", SDL_GetError());
		return false;
	}

	if (TTF_Init() == -1) {
		mlog("TTF_Init error: ", TTF_GetError());
		return false;
	}
	else
	{
		global_font = TTF_OpenFont("resources/OpenSans-Regular.ttf", 18);
		if (global_font == NULL)
		{
			mlog("TTF_OpenFont error: ", TTF_GetError());
			return false;
		}
	}

	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	{
		mlog("Warning: Linear texture filtering not enabled!");
	}

	main_window = SDL_CreateWindow("Linpo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (main_window == nullptr)
	{
		mlog("Error creating main_window: ", SDL_GetError());
		return false;
	}
	else
	{
		if (VSYNC_ENABLED)
			main_renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
		else
			main_renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

		if (main_renderer == nullptr)
		{
			mlog("Error creating main_renderer: ", SDL_GetError());
			return false;
		}
		else
		{
			SDL_SetRenderDrawColor(main_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		}
	}

	return true;
}


void close()
{
	SDL_DestroyRenderer(main_renderer);
	SDL_DestroyWindow(main_window);
	main_renderer = nullptr;
	main_window = nullptr;

	TTF_CloseFont(global_font);
	global_font = nullptr;

	TTF_Quit();
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
		Linpo linpo_logic(game_grid);
		ScoreBoard score_board(linpo_logic.get_players(), game_grid);

		while (!quit)
		{
			if (!VSYNC_ENABLED) fps_cap_timer.start();

			if (SDL_WaitEventTimeout(&e, TICKS_PER_FRAME) != 0)
			{
				do 
				{
					game_grid.handle_event(e);
					score_board.handle_event(e);

					if (e.type == SDL_QUIT)
						quit = true;
				} while (SDL_PollEvent(&e) != 0);
			}

			SDL_SetRenderDrawColor(main_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_RenderClear(main_renderer);

			// Code below
			linpo_logic.update();
			game_grid.update(linpo_logic.get_current_player());
			game_grid.render();
			score_board.render();

			SDL_RenderPresent(main_renderer);

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

// TODO: options screen
// TODO: ANDROID!
//		-> pinch to zoom

// TODO: use std::set for grid_lines
// TODO: make textures be only of necessary size
// TODO: textures for everything (? maybe)