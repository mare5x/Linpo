#pragma once

#include <array>
#include "player.h"
#include "constants.h"
#include "globals.h"
#include "texturewrapper.h"
#include "grid.h"


class ScoreBoard
{
public:
	ScoreBoard(std::array<Player, N_PLAYERS> &players_array, Grid &game_grid);
	~ScoreBoard();

	void handle_event(SDL_Event &e);
	void render();
	void resize();
	void update_scoreboard_texture();
private:
	SDL_Rect viewport_rect;
	int _prev_score;

	Grid &game_grid;
	std::array<Player, N_PLAYERS> &players;

	TextureWrapper* scoreboard_texture;
};