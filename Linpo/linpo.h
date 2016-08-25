#pragma once

#include <array>
#include <SDL.h>
#include "player.h"
#include "grid.h"
#include "constants.h"
#include "ai_logic.h"


class Linpo
{
public:
	Linpo(Grid &game_grid);
	~Linpo();

	void update();

	void reset_game();

	bool is_game_over();

	Player &get_current_player();
	std::array<Player, N_PLAYERS> &get_players();
private:
	bool is_ai_turn();

	Grid &game_grid;

	std::array<Player, N_PLAYERS> players;
	int player_index;

	AI_Logic* ai_logic;

	int prev_n_lines, prev_n_boxes;
};