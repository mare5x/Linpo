#pragma once

#include <array>
#include <memory>
#include <SDL.h>
#include "player_array.h"
#include "grid.h"
#include "constants.h"
#include "ai_logic.h"

class GameOverItem;

class Linpo
{
public:
	Linpo(Grid &game_grid);

	void update();

	/* Renders game_over_item when the game is over, otherwise it does nothing. */
	void render();
	void handle_event(const SDL_Event &e);

	void reset_game(bool clear_grid = true);

	void undo_last_move();

	void set_ai_enabled(bool decision);
	void set_player_number(int number);
	void set_grid_size(int size);
	void set_color_theme(const COLOR_THEME &color_theme);

	/* Displays the game over screen until the item is clicked/dismissed. */
	void show_game_winner_notification();

	bool is_game_over();

	/* Returns nullptr if there is no winner. If its a draw, it returns the first winning player based on the scoreboard order. 
	   Note: it assumes the game is over. */
	const Player* get_winner() const;

	Player &get_current_player() { return players[player_index]; }
	Player &get_previous_player() { return players[(player_index - 1) % players.size()]; }

	PlayerArray &get_player_array();
private:
	bool is_ai_turn();
	void enable_ai(bool decision);

	Grid &game_grid;
	std::unique_ptr<AI_Logic> ai_logic;

	PlayerArray players;
	bool ai_enabled;

	std::unique_ptr<GameOverItem> game_over_item;
	bool game_over_screen_was_shown;

	int player_index;
	int prev_n_lines, prev_n_boxes;

	bool undid_last_move;
};