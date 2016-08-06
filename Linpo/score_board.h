#pragma once

#include <memory>
#include <array>
#include "player.h"
#include "constants.h"
#include "globals.h"
#include "text_texture.h"
#include "grid.h"


class ScoreBoard
{
public:
	ScoreBoard(std::array<Player, N_PLAYERS> &players_array, Grid &game_grid);

	void handle_event(SDL_Event &e);
	void render();
	void resize();
	void update_scoreboard_textures();
	void update_texture_positions();
private:
	SDL_Rect viewport_rect;
	int _prev_score;

	Grid &game_grid;
	std::array<Player, N_PLAYERS> &players;

	std::array<std::unique_ptr<TextTexture>, N_PLAYERS> scoreboard_textures;
};