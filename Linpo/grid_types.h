#pragma once

#include <vector>
#include <array>
#include "SDL.h"
#include "player_array.h"

class Line
{
public:
	const SDL_Point* start;
	const SDL_Point* end;
	Player* owner;
	
	bool has_owner() const { return owner != nullptr; }
	bool is_vertical() const { return start->x == end->x; }
	bool is_horizontal() const { return start->y == end->y; }
};

typedef SDL_Rect CollisionRect;

struct ScoreBox
{
	const SDL_Point* top_left;
	char score;
	Player* owner;

	bool has_owner() const { return owner != nullptr; }
};

class BoxState
{
public:

	/* Expects an array of box line indices in the following order: top, left, bottom, right. */
	void set_line_indices(const std::array<int, 4>& indices) { line_indices = indices; }

	void set_adjoining_boxes(const std::vector<BoxState*> box_states) { adjoining_boxes = box_states; }
	const std::vector<BoxState*>& get_adjoining_boxes() const { return adjoining_boxes; }

	const std::array<int, 4>& get_line_indices() const { return line_indices; }

	int top_line() const { return line_indices[0]; }
	int left_line() const { return line_indices[1]; }
	int bottom_line() const { return line_indices[2]; }
	int right_line() const { return line_indices[3]; }
private:
	/* Lines are in the following order: top, left, bottom, right. */
	std::array<int, 4> line_indices;
	std::vector<BoxState*> adjoining_boxes;
};

inline bool operator==(const BoxState &lhs, const BoxState &rhs) {
	return lhs.top_line() == rhs.top_line();
}

inline bool operator!=(const BoxState &lhs, const BoxState &rhs) {
	return !(lhs == rhs);
}

inline bool operator==(const Line &lhs, const Line &rhs) {
	return lhs.owner == rhs.owner && lhs.start == rhs.start && lhs.end == rhs.end;
}

inline bool operator!=(const Line &lhs, const Line &rhs) {
	return !(lhs == rhs);
}