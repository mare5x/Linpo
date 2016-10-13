#pragma once

#include "grid_types.h"
#include <unordered_set>

class Grid;

class GridBoxStates
{
public:
	GridBoxStates(Grid &grid) : grid(grid) { }

	void update();
	int size() const { return box_states.size(); }

	int get_box_state_index(int row, int col) const;

	const BoxState& get_box_state(int box_state_index) const { return box_states[box_state_index]; }

	/* Returns the parameter box_state if no box is left in chain.
	   The box must have 1 empty line for a chain to be found. */
	const BoxState& get_next_box_in_chain(const BoxState &box_state) const;

	int calc_box_chain_length(const BoxState &box_state) const;

	/* Returns a box that's part of the shortest chain, which still needs 2 lines to start itself. 
	   nullptr if no box is found. */
	const BoxState* find_shortest_possible_chain() const;

	/* Returns argument box_state if no adjoining box matches the criteria. */
	const BoxState& get_adjoining_box_with_free_line(const BoxState &box_state, int line_index) const;
	const BoxState& get_adjoining_box_with_line(const BoxState &box_state, int line_index) const;
	const std::vector<const BoxState*> get_adjoining_boxes_with_line(const BoxState &box_state, int line_index) const;

	int get_taken_lines_size(const BoxState &box_state) const;
	int get_free_lines_size(const BoxState &box_state) const { return 4 - get_taken_lines_size(box_state); }
	std::vector<int> get_free_lines(const BoxState &box_state) const;
	int get_free_line(const BoxState &box_state) const;
	int get_rand_free_line(const BoxState &box_state) const;

	/* Returns the line index of a line which when placed wouldn't give the enemy any easy points.
	   Returns -1 if no line is considered safe. */
	int get_safe_line(const BoxState &box_state) const;
	int get_rand_safe_line(const BoxState &box_state) const;

	bool is_box_full(const BoxState &box_state) const { return get_taken_lines_size(box_state) == 4; }
private:
	void mark_box_chain(const BoxState &box_state, std::unordered_set<int> &marked_boxes) const;

	Grid &grid;

	std::vector<BoxState> box_states;
};