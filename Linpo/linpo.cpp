#include <numeric>
#include "linpo.h"


Linpo::Linpo(Grid &game_grid) : game_grid(game_grid)
{
	ai_logic = AI_ENABLED ? new AI_Logic(game_grid) : nullptr;

	players = {};
	players[0].color = { 0, 0, 255, 255 };
	players[1].color = { 255, 0, 0, 255 };
	//players[1].is_ai = true;
	//players[2].color = { 0, 255, 0, 255 };

	player_index = 0;
	last_score = 0;
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

	if (game_grid.new_line_placed())
	{
		if (!score_changed())
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

bool Linpo::score_changed()
{
	int new_score = calculate_score();
	if (last_score == new_score)
		return false;
	last_score = new_score;
	return true;
}

int Linpo::calculate_score()
{
	return std::accumulate(players.begin(), players.end(), 0, [](int a, const Player &player) { return a + player.score; });
}

bool Linpo::is_game_over()
{
	return game_grid.is_grid_full();
}
