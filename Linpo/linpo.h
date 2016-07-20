#pragma once

#include <array>
#include <SDL.h>
#include "player.h"
#include "grid.h"


class Linpo
{
public:
	Linpo(Grid &game_grid);

	void update();

	bool score_changed();
	int calculate_score();

	Player &get_current_player();
private:
	Grid &game_grid;

	std::array<Player, 2> players;
	int player_index;

	// last_score is the sum of all players' scores
	int last_score;
};