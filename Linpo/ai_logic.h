#pragma once

#include "player_array.h"
#include "grid.h"


class AI_Logic
{
public:
	AI_Logic(Grid &game_grid) : game_grid(game_grid) { }

	void make_move(Player &player);
private:
	void make_greedy_line(Player &player);
	int get_rand_index() const;

	Grid &game_grid;
};


// brute force look ahead x moves

// use techniques like dynamic programming ... (store list of taken lines ...)