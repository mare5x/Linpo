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
		FONT_SIZE = calculate_font_size();
		SDL_Log("Font size: %d\n", FONT_SIZE);
		global_font = TTF_OpenFont("resources/OpenSans-Regular.ttf", FONT_SIZE);
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


void handle_option(const MENU_OPTION &option, Linpo &linpo, ScoreBoardWPauseItem &score_board, MainMenu &main_menu)
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
	{
		SDL_Log("n player change");
		const IncrementerMenuItem& menu_item = static_cast<const IncrementerMenuItem&>(main_menu.get_option_item(MENU_OPTION::N_PLAYER_CHANGE));
		linpo.set_player_number(menu_item.get_cur_val());
		score_board.clear();
		break;
	}
	case MENU_OPTION::GRID_SIZE:
	{
		SDL_Log("grid size change");
		const GridSizeMenuItem& menu_item = static_cast<const GridSizeMenuItem&>(main_menu.get_option_item(MENU_OPTION::GRID_SIZE));
		linpo.set_grid_size(menu_item.get_cur_val());
		score_board.clear();
		break;
	}
	case MENU_OPTION::AI_TOGGLE:
	{
		SDL_Log("ai toggle");
		const BoolMenuItem& menu_item = static_cast<const BoolMenuItem&>(main_menu.get_option_item(MENU_OPTION::AI_TOGGLE));
		linpo.set_ai_enabled(menu_item.get_cur_val());
		break;
	}
	case MENU_OPTION::COLOR_THEME:
	{
		SDL_Log("color theme change");
		const ThemeMenuItem& menu_item = static_cast<const ThemeMenuItem&>(main_menu.get_option_item(MENU_OPTION::COLOR_THEME));
		GLOBAL_COLOR_THEME = menu_item.get_cur_val();
		main_menu.set_color_theme(GLOBAL_COLOR_THEME);
		linpo.set_color_theme(GLOBAL_COLOR_THEME);
		//score_board.set_color_theme(menu_item.get_cur_val());
		break;
	}
	case MENU_OPTION::PAUSE:
		SDL_Log("pause button");
		main_menu.toggle_visibility();
		break;
	}
}


int main(int argc, char* argv[])
{
	if (init())
	{
		bool quit = false;
		SDL_Event e;

		Grid game_grid(DEFAULT_GRID_SIZE, DEFAULT_GRID_SIZE);
		Timer fps_cap_timer;
		Linpo linpo_logic(game_grid);
		ScoreBoardWPauseItem score_board(linpo_logic.get_player_array(), game_grid);
		MainMenu main_menu;

		SDL_SetRenderDrawBlendMode(main_renderer, SDL_BLENDMODE_BLEND);  // otherwise alpha value is ignored when using line and fill!

		while (!quit)
		{
			if (!VSYNC_ENABLED) fps_cap_timer.start();

			if (SDL_WaitEventTimeout(&e, TICKS_PER_FRAME) != 0)
			{
				do 
				{
					// unconditionally handle resizing event
					if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
					{
						main_menu.handle_event(e);
						game_grid.handle_event(e);
						score_board.handle_event(e);
						continue;
					}

					if (!main_menu.is_visible())
					{
						linpo_logic.handle_event(e);
						game_grid.handle_event(e);
						score_board.handle_event(e);
					}
					else
					{
						main_menu.handle_event(e);
					}

					if (e.type == SDL_KEYDOWN && 
					   (e.key.keysym.sym == SDLK_ESCAPE || e.key.keysym.sym == SDLK_AC_BACK) ||
					   score_board.item_clicked())
						main_menu.toggle_visibility();

					if (e.type == SDL_QUIT)
						quit = true;
				} while (SDL_PollEvent(&e) != 0);
			}

			// change base background drawing color based on the selected theme
			set_global_color_theme_render_color();
			SDL_RenderClear(main_renderer);

			// Code below
			if (main_menu.is_visible())
			{
				// the viewport needs to be reset
				SDL_RenderSetViewport(main_renderer, NULL);
				main_menu.render();

				handle_option(main_menu.get_selected_option(), linpo_logic, score_board, main_menu);
			}
			else 
			{
				linpo_logic.update();
				game_grid.update(linpo_logic.get_current_player());
				game_grid.render();
				score_board.render();
				linpo_logic.render();
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


// SETTINGS FILE FOR MENU OPTIONS

// pause item sometimes goes invisible when resizing ???????

// make use of gridboxstates in grid

// options: make a slider?

// global mouse_state?

// logical resolution?
// scaling instead of resizing?

// TODO: use std::set for grid_lines
// TODO: make textures be only of necessary size
// TODO: textures for everything (? maybe)

// OPTIONS-MENU:
//	ESC key or button on screen
//  board size
//	Reset game
//	N Players
//	(Font size)

// render pause button -> two vertical rects
