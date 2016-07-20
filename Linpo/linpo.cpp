#include <numeric>
#include "linpo.h"
#include "sharedfunctions.h"


Linpo::Linpo(Grid &game_grid) : game_grid(game_grid)
{
	players[0] = {};
	players[1] = {};
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
		mlog("player1 score: ", players[0].score);
		mlog("player2 score: ", players[1].score);
	}
}

Player &Linpo::get_current_player()
{
	return players[player_index];
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