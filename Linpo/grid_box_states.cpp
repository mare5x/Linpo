#include <unordered_set>
#include <queue>
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
	// using a queue traverse all paths in box chain
	if (get_taken_lines_size(box_state) == 4)
		return;

	std::vector<int> marked_lines;
	std::queue<const BoxState*> box_queue;
	box_queue.push(&box_state);

	while (!box_queue.empty())
	{
		const BoxState &next_box = *box_queue.front();
		box_queue.pop();

		const auto was_marked = marked_boxes.insert(next_box.top_line());
		if (!was_marked.second)  // if the box has already been marked
			continue;

		int chain_line_index = get_free_line(next_box);
		if (chain_line_index != -1)
		{
			grid.mark_line_taken(chain_line_index, true);
			marked_lines.push_back(chain_line_index);

			// if marking a line has completed another box (happens when 2 boxes are left in chain) also add it to the queue
			const BoxState &adjoining_box = get_adjoining_box_with_line(next_box, chain_line_index);
			if (adjoining_box != next_box && get_taken_lines_size(adjoining_box) == 4)
				box_queue.push(&adjoining_box);
		}

		for (const BoxState* const adjacent_box : next_box.get_adjoining_boxes())
		{
			if (get_free_lines_size(*adjacent_box) == 1)
			{
				box_queue.push(adjacent_box);
			}
		}
	}

	for (int marked_line : marked_lines)
		grid.mark_line_taken(marked_line, false);
}

void GridBoxStates::mark_possible_chain(const BoxState & box_state, std::unordered_set<int>& marked_boxes) const
{
	int chain_line_index = get_free_line(box_state);
	grid.mark_line_taken(chain_line_index, true);

	mark_box_chain(box_state, marked_boxes);

	grid.mark_line_taken(chain_line_index, false);
}

std::vector<const BoxState*> GridBoxStates::get_chain_part_origins(const BoxState & box_state) const
{
	std::vector<const BoxState*> chain_part_origins;
	if (get_free_lines_size(box_state) == 1)
	{
		chain_part_origins.push_back(&box_state);
		for (const BoxState* const box : box_state.get_adjoining_boxes())
		{
			if (get_free_lines_size(*box) == 1)
				chain_part_origins.push_back(box);
		}
	}
	return chain_part_origins;
}

int GridBoxStates::get_actual_safe_line(const BoxState & box_state) const
{
	if (get_taken_lines_size(box_state) < 2)
		return get_safe_line(box_state);
	return -1;
}

int GridBoxStates::calc_box_chain_length_part(const BoxState & box_state) const
{
	// FIX THIS (USE MARKING, PROBLEM WITH LENGTH 2 CHAINS)
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

int GridBoxStates::calc_box_chain_length(const BoxState & box_state, std::unordered_set<int>& marked_boxes) const
{
	int prev_size = marked_boxes.size();
	mark_box_chain(box_state, marked_boxes);
	return marked_boxes.size() - prev_size;
}

int GridBoxStates::calc_box_chain_length(const BoxState & box_state) const
{

	if (get_taken_lines_size(box_state) == 4)
		return 0;

	std::unordered_set<int> marked_boxes;

	return calc_box_chain_length(box_state, marked_boxes);
}

int GridBoxStates::calc_number_of_possible_chains() const
{
	std::unordered_set<int> marked_boxes(box_states.size());

	int number_of_chains = 0;
	for (const auto &box_state : box_states)
	{
		if (marked_boxes.find(box_state.top_line()) != marked_boxes.end())
			continue;

		int taken_lines_size = get_taken_lines_size(box_state);

		if (taken_lines_size == 3)
		{
			mark_box_chain(box_state, marked_boxes);
			number_of_chains++;
		}
		else if (taken_lines_size == 2)
		{
			mark_possible_chain(box_state, marked_boxes);
			number_of_chains++;
		}
	}
	return number_of_chains;
}

bool GridBoxStates::safe_box_available() const
{
	for (const auto &box_state : box_states)
	{
		if (is_box_safe(box_state))
			return true;
	}
	return false;
}

bool GridBoxStates::is_box_safe(const BoxState & box_state) const
{
	return get_actual_safe_line(box_state) != -1;
}

const BoxState* GridBoxStates::find_shortest_possible_chain() const
{
	int min_length = box_states.size() + 1;

	std::unordered_set<int> marked_boxes(box_states.size());

	const BoxState* box = nullptr;
	
	for (const auto &box_state : box_states)
	{
		// sometimes a chain is of different length depending on which line gets taken first -> no marking?

		int taken_lines_size = get_taken_lines_size(box_state);

		if (taken_lines_size == 3)
			return &box_state;

		// if box has already been marked
		if (marked_boxes.find(box_state.top_line()) != marked_boxes.end())
			continue;

		if (taken_lines_size < 2 || taken_lines_size == 4)
			continue;

		int prev_boxes_size = marked_boxes.size();

		if (taken_lines_size == 2)
			mark_possible_chain(box_state, marked_boxes);
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

const BoxState & GridBoxStates::get_shortest_part_of_chain(const BoxState & box_state) const
{
	const auto chain_part_origins = get_chain_part_origins(box_state);
	int shortest_chain_length = box_states.size() + 1;
	const BoxState* shortest_origin = nullptr;
	for (const auto box_state_origin : chain_part_origins)
	{
		int part_length = calc_box_chain_length_part(*box_state_origin);
		if (part_length != 0 && part_length < shortest_chain_length)
		{
			shortest_chain_length = part_length;
			shortest_origin = box_state_origin;
		}
	}

	if (shortest_origin != nullptr)
		return *shortest_origin;
	else
		return box_state;
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
