#pragma once

#include <SDL.h>
#include <vector>
#include "constants.h"


struct Line
{
	SDL_Point* start;
	SDL_Point* end;
	Player owner;
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
	Grid(int cols, int rows);
	~Grid();

	void handle_event(SDL_Event & e);
	void handle_mouse_click(Player player);
	void handle_mouse_hover(Player player);

	void update();

	bool check_collision(int x, int y, CollisionRect & target_rect);

	void render_line(SDL_Point & start, SDL_Point & end, const SDL_Color & color);
	void render_point(const SDL_Point & point, const SDL_Color & color);
	void render_points(const std::vector<SDL_Point> & points, const SDL_Color & color);
	void render();
private:
	void resize_update();
	void update_grid_points();
	void update_grid_collision_rects();
	void set_grid_line(Line line);
	bool make_collision_line(Line & new_line, int x, int y, Player player);

	SDL_Point get_point_distance();

	int n_edges;
	int cols, rows, width, height;
	int point_radius, line_width;

	int mouse_x, mouse_y;
	bool mouse_clicked;

	Line hover_line;

	std::vector<SDL_Point> grid_points;
	std::vector<Line> grid_lines;
	std::vector<CollisionRect> grid_collision_rects;
	
	SDL_Texture* grid_texture;
};