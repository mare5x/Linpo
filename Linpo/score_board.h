#pragma once

#include <array>
#include "player.h"
#include "constants.h"
#include "globals.h"


class ScoreBoard
{
public:
	ScoreBoard(std::array<Player, N_PLAYERS> &players_array);

	void handle_event(SDL_Event &e);
	void render();
	void resize();
private:
	SDL_Rect viewport_rect;
	int width;

	std::array<Player, N_PLAYERS> &players;
};