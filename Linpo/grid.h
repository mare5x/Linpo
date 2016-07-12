#pragma once

#include <SDL.h>
#include <vector>


struct Line
{
	SDL_Point start, end;
	int owner;
};

struct CollisionRect
{
	SDL_Rect collision_rect;
	SDL_Point* point_a;
	SDL_Point* point_b;
};


class Grid
{
public:
	Grid(int rows, int cols);

	void handle_event(SDL_Event& e);
	void handle_mouse_click(int x, int y);

	void update();

	bool check_collision(int x, int y);

	void render_line(SDL_Point& start, SDL_Point& end, const SDL_Color& color);
	void render_point(SDL_Point& point, const SDL_Color& color);
	void render_points(const std::vector<SDL_Point>& points, const SDL_Color& color);
	void render();
private:
	void resize_update();
	void update_grid_points();
	void update_grid_lines();
	void update_grid_collision_rects();
	SDL_Point get_point_distance();

	int cols, rows, width, height;
	int point_radius, line_width;

	std::vector<SDL_Point> grid_points;
	std::vector<Line> grid_lines;
	std::vector<CollisionRect> grid_collision_rects;
};