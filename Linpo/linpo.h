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

	bool score_changed();
	int calculate_score();

	bool is_game_over();

	Player &get_current_player();
	std::array<Player, N_PLAYERS> &get_players();
private:
	bool is_ai_turn();

	Grid &game_grid;

	std::array<Player, N_PLAYERS> players;
	int player_index;

	AI_Logic* ai_logic;

	// last_score is the sum of all players' scores
	int last_score;
};