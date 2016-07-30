#pragma once

#include <vector>
#include <SDL.h>
#include "constants.h"
#include "texturewrapper.h"
#include "player.h"
#include "grid_types.h"


class Grid
{
public:
	Grid(int cols, int rows);
	~Grid();

	void handle_event(SDL_Event &e);

	void update(Player &player);

	void resize_update();

	void render();
	void render_box_score(const char score, const SDL_Point &top_left, const SDL_Color &color);

	std::vector<SDL_Point> &get_grid_points();
	std::vector<ScoreBox> get_boxes_around_line(Line &line);  // it is actually const Line &line
	Line &get_last_line_placed();
	int get_grid_point_index(int row, int col);
	const int &get_rows() const { return rows; }
	const int &get_cols() const { return cols; }
	SDL_Point get_point_distance();

	void add_grid_score_boxes(std::vector<ScoreBox> &score_boxes, Player &player);

	void set_grid_line(Line line);
	bool is_line_taken(Line &line);
	bool is_grid_full();
	bool new_line_placed(int &prev_lines);
	bool score_changed(int &prev_boxes);
private:
	void handle_mouse_click(Player &player);
	void handle_mouse_hover(Player &player);

	bool check_collision(int x, int y, CollisionRect &target_rect);

	void update_textures();
	void update_grid_texture();
	void update_hover_line_texture();

	bool hover_line_update_pending();
	bool grid_texture_update_pending();

	void update_grid_points();
	void update_grid_collision_rects();
	bool make_collision_line(Line &new_line, int x, int y, Player &player);

	ScoreBox make_box(const Line &top_line, const Line &right_line, const Line &bot_line, const Line &left_line, Player &player);
	bool find_box(const Line* base_line, Line* &top, Line* &right, Line* &bot, Line* &left);
	int calculate_box_score(const Line &top, const Line &right, const Line &bot, const Line &left, const Player &last_player);

	SDL_Rect viewport_rect;

	const int cols, const rows, const n_edges;
	int width, height;
	int point_radius, line_width;

	int mouse_x, mouse_y;
	bool mouse_clicked;

	Line hover_line, prev_hover_line;
	int prev_n_lines, prev_n_boxes;

	std::vector<SDL_Point> grid_points;
	std::vector<Line> grid_lines;
	std::vector<CollisionRect> grid_collision_rects;
	std::vector<ScoreBox> grid_score_boxes;
	
	TextureWrapper* grid_texture, *hover_line_texture;
};