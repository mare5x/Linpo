#include <unordered_set>
#include "grid_box_states.h"
#include "grid.h"


void GridBoxStates::update()
{
	int n_boxes = (grid.get_rows() - 1) * (grid.get_cols() - 1);
	box_states.resize(n_boxes);
	box_states.shrink_to_fit();

	for (int row = 0; row < grid.get_rows() - 1; row++)
	{
		for (int col = 0; col < grid.get_rows() - 1; col++)
		{
			BoxState &box_state = box_states[get_box_state_index(row, col)];
			// unnecessary to check for validity, since row and col are in valid bounds
			box_state.set_line_indices(grid.get_box_indices(grid.get_grid_line_index(row, col)[0]));

			std::vector<BoxState*> adjoining_boxes;
			if (col > 0) adjoining_boxes.push_back(&box_states[get_box_state_index(row, col - 1)]);						  // left
			if (row > 0) adjoining_boxes.push_back(&box_states[get_box_state_index(row - 1, col)]);						  // top
			if (col + 1 < grid.get_cols() - 1) adjoining_boxes.push_back(&box_states[get_box_state_index(row, col + 1)]);  // right
			if (row + 1 < grid.get_rows() - 1) adjoining_boxes.push_back(&box_states[get_box_state_index(row + 1, col)]);  // bot
			box_state.set_adjoining_boxes(adjoining_boxes);
		}
	}
}

inline int GridBoxStates::get_box_state_index(int row, int col) const
{
	return (grid.get_cols() - 1) * row + col;
}

const BoxState & GridBoxStates::get_next_box_in_chain(const BoxState & box_state) const
{
	if (get_free_lines_size(box_state) == 1)
	{
		int connecting_line = get_free_line(box_state);
		const BoxState &next_box = get_adjoining_box_with_free_line(box_state, connecting_line);
		if (get_taken_lines_size(next_box) >= 2)
			return next_box;
	}
	return box_state;
}

int GridBoxStates::get_safe_line(const BoxState & box_state) const
{
	for (int line : get_free_lines(box_state))
	{
		const BoxState & box = get_adjoining_box_with_free_line(box_state, line);
		if (get_taken_lines_size(box) <= 1)
			return line;
	}
	return -1;
}

int GridBoxStates::get_rand_safe_line(const BoxState & box_state) const
{
	// using reservoir sampling: increasingly smaller probability of picking index
	int line_index = -1;
	const auto &free_lines = get_free_lines(box_state);
	for (int line : free_lines)
	{
		const BoxState & box = get_adjoining_box_with_free_line(box_state, line);
		if (get_taken_lines_size(box) <= 1)
			if (line_index == -1 || (rand() % free_lines.size() == 0))
				line_index = line;
	}
	return line_index;
}

void GridBoxStates::mark_box_chain(const BoxState &box_state, std::unordered_set<int> &marked_boxes) const
{
	if (get_free_lines_size(box_state) == 1)
	{
		marked_boxes.insert(box_state.top_line());

		const auto &next_box = get_next_box_in_chain(box_state);

		int chain_line_index = get_free_line(box_state);
		grid.mark_line_taken(chain_line_index, true);

		mark_box_chain(next_box, marked_boxes);

		grid.mark_line_taken(chain_line_index, false);
	}
}

int GridBoxStates::calc_box_chain_length(const BoxState & box_state) const
{
	int chain_length = 0;
	if (get_free_lines_size(box_state) == 1)
	{
		chain_length++;

		const auto &next_box = get_next_box_in_chain(box_state);

		int chain_line_index = get_free_line(box_state);
		grid.mark_line_taken(chain_line_index, true);

		chain_length += calc_box_chain_length(next_box);

		grid.mark_line_taken(chain_line_index, false);
	}

	return chain_length;
}

const BoxState* GridBoxStates::find_shortest_possible_chain() const
{
	int min_length = box_states.size() + 1;

	std::unordered_set<int> marked_boxes(box_states.size());

	const BoxState* box = nullptr;
	
	for (const auto &box_state : box_states)
	{
		// if box has been marked already
		if (marked_boxes.find(box_state.top_line()) != marked_boxes.end())
			continue;

		int taken_lines_size = get_taken_lines_size(box_state);

		if (taken_lines_size < 2 || taken_lines_size == 4)
			continue;

		int prev_boxes_size = marked_boxes.size();

		if (taken_lines_size == 2)
		{
			int chain_line_index = get_free_line(box_state);
			grid.mark_line_taken(chain_line_index, true);
			// mark top & bot and left & right chains, since they are actually part of the same chain
			mark_box_chain(box_state, marked_boxes);
			auto next_box = get_adjoining_box_with_line(box_state, chain_line_index);
			if (next_box != box_state)
				mark_box_chain(next_box, marked_boxes);

			grid.mark_line_taken(chain_line_index, false);
		}
		else
			mark_box_chain(box_state, marked_boxes);

		int chain_length = marked_boxes.size() - prev_boxes_size;

		if (chain_length < min_length)
		{
			min_length = chain_length;
			box = &box_state;
		}

		if (min_length == 1)
			return box;
	}
	return box;
}

const BoxState & GridBoxStates::get_adjoining_box_with_free_line(const BoxState & box_state, int line_index) const
{
	for (const BoxState* adjoining_box : box_state.get_adjoining_boxes())
	{
		for (int box_line : get_free_lines(*adjoining_box))
		{
			if (box_line == line_index)
				return *adjoining_box;
		}
	}
	return box_state;
}

const BoxState & GridBoxStates::get_adjoining_box_with_line(const BoxState & box_state, int line_index) const
{
	for (const BoxState* adjoining_box : box_state.get_adjoining_boxes())
	{
		for (int line : adjoining_box->get_line_indices())
		{
			if (line == line_index)
				return *adjoining_box;
		}
	}
	return box_state;
}

const std::vector<const BoxState*> GridBoxStates::get_adjoining_boxes_with_line(const BoxState & box_state, int line_index) const
{
	std::vector<const BoxState*> adjoining_boxes;
	for (const BoxState* adjoining_box : box_state.get_adjoining_boxes())
	{
		for (int box_line : get_free_lines(*adjoining_box))
		{
			if (box_line == line_index)
				adjoining_boxes.push_back(adjoining_box);
		}
	}
	return adjoining_boxes;
}

int GridBoxStates::get_taken_lines_size(const BoxState & box_state) const
{
	int size = 0;
	for (int line_index : box_state.get_line_indices())
	{
		if (grid.is_line_taken(line_index))
		{
			size++;
		}
	}
	return size;
}

std::vector<int> GridBoxStates::get_free_lines(const BoxState & box_state) const
{
	std::vector<int> free_lines;

	for (int line_index : box_state.get_line_indices())
	{
		if (!grid.is_line_taken(line_index))
			free_lines.push_back(line_index);
	}

	return free_lines;
}

int GridBoxStates::get_free_line(const BoxState & box_state) const
{
	for (int line_index : box_state.get_line_indices())
	{
		if (!grid.is_line_taken(line_index))
			return line_index;
	}
	return -1;
}

int GridBoxStates::get_rand_free_line(const BoxState & box_state) const
{
	const auto &free_lines = get_free_lines(box_state);
	if (free_lines.size() > 0)
		return free_lines[rand() % free_lines.size()];
	return -1;
}
