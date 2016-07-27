#pragma once

#include <SDL.h>

struct Player
{
	int score;
	SDL_Color color;
	bool is_ai;
};