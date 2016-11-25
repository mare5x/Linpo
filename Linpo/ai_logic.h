#pragma once
#include <vector>

struct Player;
class Grid;
class BoxState;

class AI_Logic
{
public:
	AI_Logic(Grid &game_grid) : game_grid(game_grid), move_queue{}, move_queue_index(0) { }

	void make_move(Player &player);

	/* Resets the move queue. */
	void reset();
private:
	int get_greedy_line(Player &player);
	int get_smart_line(Player &player);
	int get_rand_index() const;

	int get_sacrifice_length(const BoxState &box_state) const;
	/* Get sacrifice length based on move_queue (box_chain_part) information. 
	   Note: the chain length of the move_queue MUST be more than 2 for a meaningful return value. */
	int get_sacrifice_length() const;
	/* Note: uses the move queue to make decision, if it's not empty. */
	bool sacrifice_possible(const BoxState &box_state, int sacrifice_length) const;
	/* Sacrifices only if its appropriate to do so (sacrifice_possible() == true). */
	int get_sacrifice_line(const BoxState &box_state, int sacrifice_length) const;

	Grid &game_grid;

	std::vector<const BoxState*> move_queue;  // actually a box chain
	size_t move_queue_index;
};


// brute force look ahead x moves

// use techniques like dynamic programming ... (store list of taken lines ...)