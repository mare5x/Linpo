#include <algorithm>
#include "ai_logic.h"


void AI_Logic::make_move(Player &player)
{
	make_greedy_line(player);
}

void AI_Logic::make_greedy_line(Player &player)
{
	// big O(rows * cols) AKA shit

	auto &taken_grid_lines = game_grid.get_taken_grid_lines();

	int index = -1;
	std::vector<ScoreBox> new_filled_boxes;
	for (int i = 0; i < taken_grid_lines.size(); ++i)
	{
		if (!taken_grid_lines[i])
		{
			game_grid.set_grid_line(i, player);
			new_filled_boxes = game_grid.get_boxes_around_line(i, player);
			game_grid.remove_grid_line(i);

			// stop on first box found, since the next move is then anyways yours
			if (!new_filled_boxes.empty())
			{
				index = i;
				break;
			}
		}
	}

	if (index == -1)
		index = get_rand_index();

	if (index >= 0)
	{
		game_grid.set_grid_line(index, player);
		game_grid.add_grid_score_boxes(new_filled_boxes, player);
	}
	// else game is over and do nothing
}

int AI_Logic::get_rand_index() const
{
	// Reservoir sampling algorithm (kind of ...)

	const auto &taken_grid_lines = game_grid.get_taken_grid_lines();
	int index = -1;
	int n_free_lines = 0;
	for (auto i = 0; i < taken_grid_lines.size(); ++i)
	{
		if (!taken_grid_lines[i])
		{
			n_free_lines++;
			if (index == -1)
			{
				index = i;
				continue;
			}
			// random number with decreasing probability to change the current index
			// 1 / n_free_lines
			if (rand() % n_free_lines == 0)
				index = i;
		}
	}

	return index;
}
