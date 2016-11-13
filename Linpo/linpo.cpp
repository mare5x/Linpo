#include <numeric>
#include "globals.h"
#include "menu_items.h"
#include "linpo.h"


Linpo::Linpo(Grid &game_grid)
	:game_grid(game_grid),
	ai_logic(std::make_unique<AI_Logic>(game_grid)),
	players{},
	ai_enabled(AI_ENABLED),
	game_over_item(nullptr),
	game_over_screen_was_shown{ false },
	player_index(0),
	prev_n_lines(0),
	prev_n_boxes(0),
	undid_last_move(0)
{	
	enable_ai(ai_enabled);
}

void Linpo::update()
{
	if (is_game_over())
		return;

	if (is_ai_turn())
	{
		ai_logic->make_move(get_current_player());
	}

	if (game_grid.lines_placed_changed(prev_n_lines))
	{
		if (!game_grid.score_changed(prev_n_boxes))
		{
			if (!undid_last_move)
			{
				// next player's turn, since a move was made and the player didn't fill up a box
				player_index = (player_index + 1) % players.size();

				if (get_current_player().last_moves.empty() || !get_current_player().last_moves.back().empty())
					get_current_player().last_moves.push_back({});
			}
		}
		undid_last_move = false;
	}
}

void Linpo::render()
{
	if (is_game_over())
	{
		if (!game_over_screen_was_shown)
			show_game_winner_notification();
	}
}

void Linpo::handle_event(const SDL_Event & e)
{
	if (game_over_item)
		game_over_item->handle_event(e);
}

void Linpo::reset_game(bool clear_grid)
{
	if (clear_grid)
		game_grid.clear_grid();

	player_index = 0;
	prev_n_lines = 0;
	prev_n_boxes = 0;
	undid_last_move = false;

	game_over_item.reset(nullptr);
	game_over_screen_was_shown = false;

	for (auto &player : players)
	{
		player.score = 0;
		player.last_moves.clear();
		player.last_moves.push_back({});
	}
}

void Linpo::undo_last_move()
{
	SDL_Log("Undoing last move!\n");

	if (game_grid.is_grid_empty())
		return;

	if (!get_current_player().is_ai)
	{
		if (!get_current_player().last_moves.empty() && get_current_player().last_moves.back().empty())
		{
			get_current_player().last_moves.pop_back();
			player_index = (player_index - 1) % players.size();
		}
	}

	if (ai_enabled)
	{
		while (get_current_player().is_ai && !get_current_player().last_moves.empty())
		{
			for (int line : get_current_player().last_moves.back())
			{
				game_grid.remove_grid_line(line);
			}
			get_current_player().last_moves.pop_back();

			player_index = (player_index - 1) % players.size();
		}
	}

	if (!get_current_player().last_moves.empty() && get_current_player().last_moves.back().size() > 0)
	{
		game_grid.remove_grid_line(get_current_player().last_moves.back().back());
		get_current_player().last_moves.back().pop_back();
	}

	undid_last_move = true;
}

void Linpo::set_ai_enabled(bool decision)
{
	if (decision != ai_enabled)
	{
		ai_enabled = decision;
		enable_ai(decision);
	}
}

void Linpo::set_player_number(int number)
{
	players.set_players(number);
	reset_game();
	enable_ai(ai_enabled);
}

void Linpo::set_grid_size(int size)
{
	game_grid.set_grid_size(size, size);
	reset_game(false);
}

void Linpo::set_color_theme(const COLOR_THEME & color_theme)
{
	game_grid.set_color_theme(color_theme);

	// change Player colors
}

void Linpo::enable_ai(bool decision)
{
	for (int i = 1; i < players.size(); ++i)
		players[i].is_ai = decision;
}

const Player* Linpo::get_winner() const
{
	int best_score = -1;
	const Player* winner = nullptr;
	for (const Player &player : players)
	{
		if (player.score > best_score)
		{
			best_score = player.score;
			winner = &player;
		}
	}
	return winner;
}

PlayerArray& Linpo::get_player_array()
{
	return players;
}

bool Linpo::is_ai_turn()
{
	return get_current_player().is_ai;
}

void Linpo::show_game_winner_notification()
{
	if (!game_over_item)  // == nullptr
	{
		const Player* winner = get_winner();
		if (winner == nullptr)
			return;

		switch (GLOBAL_COLOR_THEME)
		{
		case COLOR_THEME::DEFAULT:
			game_over_item = std::make_unique<GameOverItem>(*winner, COLORS[BLACK]);
			break;
		case COLOR_THEME::BLACK:
			game_over_item = std::make_unique<GameOverItem>(*winner, COLORS[LIME]);
			break;
		}
	}

	if (!game_over_item->was_clicked())
	{
		// reset the viewport so event handling gets the correct coordinates
		SDL_RenderSetViewport(main_renderer, NULL);

		int width, height;
		SDL_GetRendererOutputSize(main_renderer, &width, &height);

		// center it
		SDL_Rect viewport_rect = { (width / 2) - (game_over_item->get_width() / 2), (height / 2) - (game_over_item->get_height() / 2), game_over_item->get_width(), game_over_item->get_height() };
		set_global_color_theme_render_color();
		SDL_RenderFillRect(main_renderer, &viewport_rect);
		game_over_item->apply_theme(GLOBAL_COLOR_THEME);
		game_over_item->render(viewport_rect);
	}
	else
	{
		game_over_item.reset(nullptr);  // delete the item
		game_over_screen_was_shown = true;
	}
}

bool Linpo::is_game_over()
{
	return game_grid.is_grid_full();
}
