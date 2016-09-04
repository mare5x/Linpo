#include <numeric>
#include "linpo.h"


Linpo::Linpo(Grid &game_grid)
	:game_grid(game_grid),
	ai_logic(std::make_unique<AI_Logic>(game_grid)),
	players{},
	ai_enabled(AI_ENABLED),
	player_index(0),
	prev_n_lines(0),
	prev_n_boxes(0)
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

	if (game_grid.new_line_placed(prev_n_lines))
	{
		if (!game_grid.score_changed(prev_n_boxes))
		{
			// next player's turn, since a move was made and the player didn't fill up a box
			player_index = (player_index + 1) % players.size();
		}
	}
}

void Linpo::reset_game()
{
	game_grid.clear_grid();

	player_index = 0;
	prev_n_lines = 0;
	prev_n_boxes = 0;

	for (auto &player : players)
		player.score = 0;
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

void Linpo::enable_ai(bool decision)
{
	for (int i = 1; i < players.size(); ++i)
		players[i].is_ai = decision;
}

Player &Linpo::get_current_player()
{
	return players[player_index];
}

PlayerArray& Linpo::get_player_array()
{
	return players;
}

bool Linpo::is_ai_turn()
{
	return get_current_player().is_ai;
}

bool Linpo::is_game_over()
{
	return game_grid.is_grid_full();
}
