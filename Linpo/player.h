#pragma once

#include <SDL.h>

struct Player
{
	char id;
	int score;
	const SDL_Color* color;
	bool is_ai;
};