#pragma once
#include <deque>

struct Player;
class Grid;
class BoxState;

class AI_Logic
{
public:
	AI_Logic(Grid &game_grid) : game_grid(game_grid) { }

	void make_move(Player &player);

	/* Resets the move queue. */
	void reset() {}
private:
	int get_greedy_line(Player &player);
	int get_smart_line(Player &player);
	int get_rand_index() const;

	int get_sacrifice_length(const BoxState &box_state) const;
	bool sacrifice_possible(const BoxState &box_state, int sacrifice_length) const;
	int get_sacrifice_line(const BoxState &box_state, int sacrifice_length) const;

	Grid &game_grid;
};


// brute force look ahead x moves

// use techniques like dynamic programming ... (store list of taken lines ...)