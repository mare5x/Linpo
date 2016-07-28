#pragma once

#include "SDL.h"
#include "player.h"


struct Line
{
	SDL_Point* start;
	SDL_Point* end;
	Player* owner;
};

struct CollisionRect
{
	SDL_Rect collision_rect;
	SDL_Point* point_a;
	SDL_Point* point_b;
};

struct ScoreBox
{
	SDL_Point* top_left;
	int score;
	Player* owner;
};