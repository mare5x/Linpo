#pragma once

#include <array>
#include <memory>
#include <SDL.h>
#include "player_array.h"
#include "grid.h"
#include "constants.h"
#include "ai_logic.h"


class Linpo
{
public:
	Linpo(Grid &game_grid);

	void update();

	void reset_game();

	void set_ai_enabled(bool decision);
	void set_player_number(int number);

	bool is_game_over();

	Player &get_current_player();
	PlayerArray &get_player_array();
private:
	bool is_ai_turn();
	void enable_ai(bool decision);

	Grid &game_grid;
	std::unique_ptr<AI_Logic> ai_logic;

	PlayerArray players;
	bool ai_enabled;
	int player_index;
	int prev_n_lines, prev_n_boxes;
};