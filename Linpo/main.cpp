#include "SDL_ttf.h"

#include "constants.h"
#include "globals.h"
#include "grid.h"
#include "fps_timer.h"
#include "linpo.h"
#include "score_board.h"
#include "main_menu.h"


bool init()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
	{
		SDL_Log("Error with SDL_Init: %s", SDL_GetError());
		return false;
	}

	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	{
		SDL_Log("Warning: Linear texture filtering not enabled!");
	}

	main_window = SDL_CreateWindow("Linpo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (main_window == nullptr)
	{
		SDL_Log("Error creating main_window: %s", SDL_GetError());
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
			SDL_Log("Error creating main_renderer: %s", SDL_GetError());
			return false;
		}
		else
		{
			SDL_SetRenderDrawColor(main_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		}
	}

	if (TTF_Init() == -1) {
		SDL_Log("TTF_Init error: %s", TTF_GetError());
		return false;
	}
	else
	{
		global_font = TTF_OpenFont("resources/OpenSans-Regular.ttf", calculate_font_size());
		if (global_font == NULL)
		{
			SDL_Log("TTF_OpenFont error: %s", TTF_GetError());
			return false;
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


void handle_option(const MENU_OPTION &option, Linpo &linpo, ScoreBoard &score_board)
{
	switch (option)
	{
	case MENU_OPTION::NULL_OPTION:
		break;
	case MENU_OPTION::RESTART_GAME:
		SDL_Log("Restart game option clicked.");
		linpo.reset_game();
		score_board.clear();
		break;
	case MENU_OPTION::N_PLAYER_CHANGE:
		SDL_Log("n player change");
		break;
	case MENU_OPTION::AI_TOGGLE:
		SDL_Log("ai toggle");
		break;
	}
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
		MainMenu main_menu;

		while (!quit)
		{
			if (!VSYNC_ENABLED) fps_cap_timer.start();

			if (SDL_WaitEventTimeout(&e, TICKS_PER_FRAME) != 0)
			{
				do 
				{
					main_menu.handle_event(e);

					if (!main_menu.is_visible())
					{
						game_grid.handle_event(e);
						score_board.handle_event(e);
					}

					if (e.type == SDL_QUIT)
						quit = true;
				} while (SDL_PollEvent(&e) != 0);
			}

			SDL_SetRenderDrawColor(main_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_RenderClear(main_renderer);

			// Code below
			if (main_menu.is_visible())
			{
				// the viewport needs to be reset
				SDL_RenderSetViewport(main_renderer, NULL);
				main_menu.render();

				handle_option(main_menu.get_selected_option(), linpo_logic, score_board);
			}
			else {
				linpo_logic.update();
				game_grid.update(linpo_logic.get_current_player());
				game_grid.render();
				score_board.render();
			}

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

// OPTIONS-MENU:
//	ESC key or button on screen
//	Reset game
//	N Players
//	(Font size)


// |<--| N_PLAYERS |-->|

// this is a slider ^

// render pause button -> two vertical rects