#pragma once

#include "player_array.h"
#include "grid.h"


class AI_Logic
{
public:
	AI_Logic(Grid &game_grid) : game_grid(game_grid) { }

	void make_move(Player &player);
	void make_greedy_line(Line &new_line, std::vector<ScoreBox> &score_boxes, Player &player);
private:
	Grid &game_grid;
};