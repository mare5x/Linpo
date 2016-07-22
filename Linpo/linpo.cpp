#include <numeric>
#include "linpo.h"


Linpo::Linpo(Grid &game_grid) : game_grid(game_grid)
{
	players = {};
	players[0].color = { 0, 0, 255, 255 };
	players[1].color = { 255, 0, 0, 255 };

	player_index = 0;
	last_score = 0;
}

void Linpo::update()
{
	if (game_grid.new_line_placed())
	{
		if (!score_changed())
		{
			// next player's turn, since a move was made and the player didn't fill up a box
			player_index = (player_index + 1) % players.size();
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