#include <algorithm>
#include "ai_logic.h"


void AI_Logic::make_move(Player &player)
{
	Line new_line;
	std::vector<ScoreBox> score_boxes;
	make_greedy_line(new_line, score_boxes, player);
	if (new_line.owner != nullptr)
		game_grid.set_grid_line(new_line);
	// else game is over and do nothing

	game_grid.add_grid_score_boxes(score_boxes, player);
}


void AI_Logic::make_greedy_line(Line &new_line, std::vector<ScoreBox> &score_boxes, Player &player)
{
	// big O(rows * cols) AKA shit

	auto &grid_points = game_grid.get_grid_points();
	int max_score = -1;
	for (int row = 0; row < game_grid.get_rows(); ++row)
	{
		for (int col = 0; col < game_grid.get_cols(); ++col)
		{
			for (int end_line_decision = 0; end_line_decision < 2; ++end_line_decision)
			{
				if (end_line_decision == 0 && col + 1 >= game_grid.get_cols()) continue;
				else if (end_line_decision == 1 && row + 1 >= game_grid.get_rows()) continue;

				std::vector<ScoreBox> boxes;
				int line_points = 0;

				Line iter_line;
				iter_line.owner = &player;
				iter_line.start = &grid_points[game_grid.get_grid_point_index(row, col)];

				// 0 == right, 1 == bottom
				if (end_line_decision == 0 && col + 1 < game_grid.get_cols())
					iter_line.end = &grid_points[game_grid.get_grid_point_index(row, col + 1)];
				else 
					iter_line.end = &grid_points[game_grid.get_grid_point_index(row + 1, col)];

				if (!game_grid.is_line_taken(iter_line))
				{
					boxes = game_grid.get_boxes_around_line(iter_line);
					for (const auto &score_box : boxes) { line_points += score_box.score; }

					if (line_points > max_score)
					{
						max_score = line_points;
						new_line = iter_line;
						score_boxes = boxes;
					}
				}
			}
		}
	}
}