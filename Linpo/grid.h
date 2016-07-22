#pragma once

#include <vector>
#include <SDL.h>
#include "constants.h"
#include "texturewrapper.h"
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

class Grid
{
public:
	Grid(int cols, int rows);
	~Grid();

	void handle_event(SDL_Event &e);

	void update(Player &player);

	void resize_update();

	void render();
	void render_box_score(const int score, const SDL_Point &top_left, const SDL_Color &color);

	bool new_line_placed();
private:
	void handle_mouse_click(Player &player);
	void handle_mouse_hover(Player &player);

	bool check_collision(int x, int y, CollisionRect &target_rect);

	SDL_Point get_point_distance();
	void update_grid_points();
	void update_grid_collision_rects();

	void set_grid_line(Line line);
	bool make_collision_line(Line &new_line, int x, int y, Player &player);
	bool is_line_taken(Line &line);

	ScoreBox make_box(const Line &top_line, const Line &right_line, const Line &bot_line, const Line &left_line, Player &player);
	std::vector<ScoreBox> get_boxes_around_line(Line &line);  // it is actually const Line &line
	bool find_box(const Line* base_line, Line* &top, Line* &right, Line* &bot, Line* &left);
	int calculate_box_score(const Line &top, const Line &right, const Line &bot, const Line &left, const Player &last_player);

	SDL_Rect viewport_rect;

	int cols, rows, width, height;
	int point_radius, line_width;

	int mouse_x, mouse_y;
	bool mouse_clicked;

	Line hover_line;
	int prev_n_lines;

	std::vector<SDL_Point> grid_points;
	std::vector<Line> grid_lines;
	std::vector<CollisionRect> grid_collision_rects;
	std::vector<ScoreBox> grid_score_boxes;
	
	TextureWrapper* grid_texture;
};