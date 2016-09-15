#pragma once

#include "SDL.h"
#include "player_array.h"


class Line
{
public:
	SDL_Point* start;
	SDL_Point* end;
	Player* owner;
	
	bool has_owner() const { return owner != nullptr; }
};

typedef SDL_Rect CollisionRect;

struct ScoreBox
{
	SDL_Point* top_left;
	char score;
	Player* owner;
};


inline bool operator==(const Line &lhs, const Line &rhs) {
	return lhs.owner == rhs.owner && lhs.start == rhs.start && lhs.end == rhs.end;
}

inline bool operator!=(const Line &lhs, const Line &rhs) {
	return !(lhs == rhs);
}