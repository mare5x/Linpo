#pragma once

#include <SDL.h>


class Player
{
public:
	Player(SDL_Color player_color) : score(0), color(player_color) { }

	SDL_Color &get_color();
	void set_color(SDL_Color new_color);

	int score;
private:
	SDL_Color color;
};