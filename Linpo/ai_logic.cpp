#include <algorithm>
#include "ai_logic.h"
#include "grid.h"
#include "grid_box_states.h"


void AI_Logic::make_move(Player &player)
{
	int line_index = get_smart_line(player);

	if (line_index >= 0)
	{
		game_grid.set_grid_line(line_index, player);
	}
	// else game is over and do nothing
}

int AI_Logic::get_greedy_line(Player &player)
{
	// big O(rows * cols) AKA shit

	int index = -1;

	const GridBoxStates &box_states = game_grid.get_box_states();
	for (int i = 0; i < box_states.size(); ++i)
	{
		const BoxState &box_state = box_states.get_box_state(i);
		auto free_lines = box_states.get_free_lines(box_state);
		if (free_lines.size() == 1)
			return free_lines[0];
	}

	return get_rand_index();
}

int AI_Logic::get_smart_line(Player & player)
{
	// O((rows - 1) * (cols - 1))

	int best_0_box_line = -1;
	int best_1_box_line = -1;
	int best_2_box = -1;
	int best_3_box = -1;
	int best_sacrifice_box_line = -1;

	const GridBoxStates &box_states = game_grid.get_box_states();
	for (int i = 0; i < box_states.size(); ++i)
	{
		const BoxState &box_state = box_states.get_box_state(i);
		int box_lines_taken = box_states.get_taken_lines_size(box_state);
		if (box_lines_taken != 4)
		{
			if (box_lines_taken == 3)
			{
				// TODO: add boxes in a chain to a queue for further moves, so I don't have to recalculate

				int sacrifice_length = 2;
				if (!box_states.is_chain_part_open_ended(box_state))
					sacrifice_length = 4;

				if (best_sacrifice_box_line == -1 && box_states.calc_box_chain_length(box_state) == sacrifice_length && game_grid.get_free_lines_size() > sacrifice_length && !box_states.safe_box_available()) // && box_states.calc_number_of_chains() > 1)
				{
					if (sacrifice_length == 2)
					{
						int line_index = box_states.get_free_line(box_state);

						auto other_box_state = box_states.get_next_box_in_chain(box_state);
						auto other_box_free_lines = box_states.get_free_lines(other_box_state);

						// sacrifice box
						// only sacrifice if its the last completable box
						if (other_box_free_lines.size() > 1)
						{
							if (other_box_free_lines[0] == line_index)
								best_sacrifice_box_line = other_box_free_lines[1];
							else
								best_sacrifice_box_line = other_box_free_lines[0];
						}
						else
						{	// no sacrifice
							best_3_box = i;
							break;
						}
					}
					else  // len = 4
					{
						const auto &boxes = box_states.get_box_chain_part(box_state);

						int prev_marked_line = -1;
						for (const auto box : boxes)
						{
							for (int free_line : box_states.get_free_lines(*box))
							{
								if (free_line == prev_marked_line)
									continue;

								game_grid.mark_line_taken(free_line, true);
								prev_marked_line = free_line;  // don't check lines we've already checked

								if (box_states.get_free_lines_size(*box) == 1)
								{
									best_sacrifice_box_line = free_line;
									game_grid.mark_line_taken(free_line, false);
									goto decision;
								}

								game_grid.mark_line_taken(free_line, false);
							}
						}

						// no sacrifice
						if (best_sacrifice_box_line == -1)
						{
							best_3_box = i;
							break;
						}
					}
				}
				else
				{
					// make box and end turn
					best_3_box = i;
					break;
				}
			}
			else if (box_lines_taken == 2)
				best_2_box = i;  // there is no safe line anyways
			else if (box_lines_taken == 1 && best_1_box_line == -1)
				best_1_box_line = box_states.get_rand_safe_line(box_state);
			else if (box_lines_taken == 0 && best_0_box_line == -1)
			{
				const int line = box_states.get_rand_safe_line(box_states.get_box_state(i));
				if (line != -1)
					best_0_box_line = line;
			}
		}
	}

	// TODO: if best_2_box is the only move pick the one that's part of the shortest chain
	// manage to sacrifice 4 boxes in a 2 x 2 box if there is a larger chain still left
	// sometimes doesn't sacrifice 
	// make better sacrificing algorithm

	// dont allow the enemy to make a sacrifice (in a size 2 chain)

decision:
	if (best_3_box != -1)
	{
		//// if chain is composed of multiple parts (2) then first start filling in the short one which gives us room to sacrifice
		const auto &box_state_origin = box_states.get_shortest_part_of_chain(box_states.get_box_state(best_3_box));
		return box_states.get_free_line(box_state_origin);
		//return box_states.get_free_line(box_states.get_box_state(best_3_box));
	}
	else if (best_sacrifice_box_line != -1) return best_sacrifice_box_line;
	else if (best_1_box_line != -1) return best_1_box_line;
	else if (best_0_box_line != -1) return best_0_box_line;

	// if we haven't returned yet it means that only unsafe lines are free
	auto box = box_states.find_shortest_possible_chain();
	if (box != nullptr)
		return box_states.get_rand_free_line(*box);

	// if nothing works, fuck it rng
	return get_rand_index();
}

int AI_Logic::get_rand_index() const
{
	// Reservoir sampling algorithm (kind of ...)

	int index = -1;
	int n_free_lines = 0;
	for (auto i = 0; i < game_grid.get_lines_size(); ++i)
	{
		if (!game_grid.is_line_taken(i))
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
