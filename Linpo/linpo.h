#pragma once

#include <array>
#include <SDL.h>
#include "player.h"
#include "grid.h"
#include "constants.h"


class Linpo
{
public:
	Linpo(Grid &game_grid);

	void update();

	bool score_changed();
	int calculate_score();

	Player &get_current_player();
	std::array<Player, N_PLAYERS> &get_players();
private:
	Grid &game_grid;

	std::array<Player, N_PLAYERS> players;
	int player_index;

	// last_score is the sum of all players' scores
	int last_score;
};