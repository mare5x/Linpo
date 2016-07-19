#include "player.h"

SDL_Color &Player::get_color()
{
	return color;
}

void Player::set_color(SDL_Color new_color)
{
	color = new_color;
}
