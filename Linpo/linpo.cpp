#include <numeric>
#include "linpo.h"


Linpo::Linpo(Grid &game_grid) : game_grid(game_grid)
{
	ai_logic = AI_ENABLED ? new AI_Logic(game_grid) : nullptr;

	players = {};
	for (int i = 0; i < players.size(); ++i)
	{
		players[i].color = &COLORS[i];
		players[i].id = i;
	}

	players[1].is_ai = AI_ENABLED ? true : false;

	player_index = 0;

	prev_n_lines = 0;
	prev_n_boxes = 0;
}

Linpo::~Linpo()
{
	delete ai_logic;
	ai_logic = nullptr;
}

void Linpo::update()
{
	if (is_game_over())
		return;

	if (ai_logic != nullptr && is_ai_turn())
	{
		ai_logic->make_move(get_current_player());
	}

	if (game_grid.new_line_placed(prev_n_lines))
	{
		if (!game_grid.score_changed(prev_n_boxes))
		{
			// next player's turn, since a move was made and the player didn't fill up a box
			player_index = (player_index + 1) % N_PLAYERS;
		}
	}
}

Player &Linpo::get_current_player()
{
	return players[player_index];
}

std::array<Player, N_PLAYERS>& Linpo::get_players()
{
	return players;
}

bool Linpo::is_ai_turn()
{
	if (get_current_player().is_ai)
		return true;
	return false;
}

bool Linpo::is_game_over()
{
	return game_grid.is_grid_full();
}
