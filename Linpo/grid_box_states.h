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

	const std::vector<const BoxState*> get_box_chain_part(const BoxState &box_state) const;
	const std::vector<const BoxState*> get_box_chain(const BoxState &box_state) const;
	/* Return a vector of chain parts joined together into a single chain/array ordered by increasing length. 
	   Basically, shortest chain part first. */
	const std::vector<const BoxState*> get_box_chain_parts(const BoxState &box_state) const;

	/* Return an ordered vector of chains based on their length and sorted in such a way that allows seamless iteration. 
	   Time complexity: O(N). */
	const std::vector<const BoxState*> get_all_box_chains() const;

	/* Calculates not only the chain leading from box_state but also the surrounding chains around box_state.
	   Note: box_state must be part of an actual chain. */
	int calc_box_chain_length(const BoxState &box_state) const;
	int calc_number_of_possible_chains() const;

	bool safe_box_available() const;
	bool is_box_safe(const BoxState &box_state) const;

	/* Returns a box that's part of the shortest chain, which still needs 2 lines to start itself. 
	   nullptr if no box is found. */
	const BoxState* find_shortest_possible_chain() const;

	const BoxState& get_shortest_part_of_chain(const BoxState &box_state) const;

	/* Returns whether the last box in chain has 2 free lines. 
	   Note: make sure box_state is part of a chain. */
	bool is_chain_part_open_ended(const BoxState &box_state) const;

	/* Returns argument box_state if no adjoining box matches the criteria. */
	const BoxState& get_adjoining_box_with_free_line(const BoxState &box_state, int line_index) const;
	const BoxState& get_adjoining_box_with_line(const BoxState &box_state, int line_index) const;

	int get_taken_lines_size(const BoxState &box_state) const;
	int get_free_lines_size(const BoxState &box_state) const { return 4 - get_taken_lines_size(box_state); }
	std::vector<int> get_free_lines(const BoxState &box_state) const;
	int get_free_line(const BoxState &box_state) const;
	int get_rand_free_line(const BoxState &box_state) const;

	/* Returns the line index of a line which when placed wouldn't give the enemy any easy points.
	   Returns -1 if no line is considered 'safe'. */
	int get_safe_line(const BoxState &box_state) const;
	int get_rand_safe_line(const BoxState &box_state) const;

	bool is_box_full(const BoxState &box_state) const { return get_taken_lines_size(box_state) == 4; }
private:
	// box_chain optionally fills in the vector
	void mark_box_chain(const BoxState &box_state, std::unordered_set<const BoxState*> &marked_boxes, std::vector<const BoxState*> * const box_chain = nullptr) const;
	void mark_possible_chain(const BoxState &box_state, std::unordered_set<const BoxState*> &marked_boxes) const;
	
	/* Marks only if a box is strictly part of the chain, i.e. boxes must be connected by free lines. */
	void mark_box_chain_strict(const BoxState &box_state, std::unordered_set<const BoxState*> * const marked_boxes = nullptr, std::vector<const BoxState*> * const box_chain = nullptr) const;

	/* Strictly marks a box chain and optionally fills in a sorted vector of the chain. */
	void mark_box_chain_parts(const BoxState &box_state, std::unordered_set<const BoxState*> &marked_boxes, std::vector<const BoxState*> * const ordered_box_chain = nullptr) const;

	int calc_box_chain_length_part(const BoxState &box_state) const;
	int calc_box_chain_length(const BoxState &box_state, std::unordered_set<const BoxState*> &marked_boxes) const;

	/* Returns a vector of BoxStates adjacent to box_state that are part of a chain (have 1 free line). 
	   Note: this includes box_state. */
	std::vector<const BoxState*> get_chain_part_origins(const BoxState &box_state) const;

	/* Only returns a line index if that line yields ZERO points. (unlike get_safe_line()).*/
	int get_actual_safe_line(const BoxState &box_state) const;

	Grid &grid;

	std::vector<BoxState> box_states;
};