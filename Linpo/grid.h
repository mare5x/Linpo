#pragma once

#include <vector>
#include <array>
#include <memory>
#include <SDL.h>
#include "constants.h"
#include "texture_wrapper.h"
#include "text_texture.h"
#include "player_array.h"
#include "grid_types.h"
#include "mouse_state.h"


class Grid
{
public:
	Grid(int cols, int rows);

	void handle_event(SDL_Event &e);

	void update(Player &player);

	void resize_update();

	void render();

	/* Clears grid and makes it ready for a new game. */
	void clear_grid();
	void set_grid_size(int rows, int cols);

	const std::vector<bool>& get_taken_grid_lines() const { return taken_grid_lines; }
	std::vector<bool>& get_taken_grid_lines() { return taken_grid_lines; }

	/* Returns a ScoreBox vector of size [0, 2] of the adjoining two (or one) boxes,
	   if those boxes are full (lines on all 4 sides). */
	std::vector<ScoreBox> get_boxes_around_line(int line_index, Player &owner) const;

	int get_grid_point_index(int row, int col);

	/* Note: [0] = horizontal, [1] = vertical line and -1 if either line doesn't exist at (row, col). */
	std::array<int, 2> get_grid_line_index(int row, int col);

	int get_grid_line_index(const Line &line);

	/* Returns true if the line at index.row < rows - 1 && index.col < cols - 1. */
	bool is_valid_top_line_index(int index) const;

	/* Returns true if all indices are taken lines and if the top line index is valid. */
	bool is_valid_box_indices(const std::array<int, 4> &box_indices) const;

	/* Returns an array of box indices in the following order: top, left, bottom, right. */
	std::array<int, 4> get_box_indices(int top_index) const;

	/* Note: last row has "n_rows" lines. */
	int get_lines_in_row() const { return (2 * cols) - 1; }

	/* Note: last col has "n_cols" lines. */
	int get_lines_in_col() const { return (2 * rows) - 1; }

	//bool is_line_taken(const Line &line) const;

	int get_rows() const { return rows; }
	int get_cols() const { return cols; }
	SDL_Point get_point_distance() const;

	void add_grid_score_boxes(std::vector<ScoreBox> &score_boxes, Player &player);

	void set_grid_line(int index, Player &owner);
	bool is_grid_full();
	bool new_line_placed(int &prev_lines);
	bool score_changed(int &prev_boxes);
private:
	void handle_mouse_click(Player &player);
	void handle_mouse_hover(Player &player);

	bool check_collision(const SDL_Point &point, CollisionRect &target_rect);
	bool check_collision(const SDL_Point &point) const;

	void update_textures();
	void update_grid_texture();
	void update_hover_line_texture();

	bool hover_line_update_pending();
	bool grid_texture_update_pending();

	void update_grid_points();
	void init_grid_lines();
	void update_grid_collision_rects();
	bool make_collision_line(Line &new_line, const SDL_Point &pos, Player &player);

	/* Note: make sure box_indices are valid. */
	ScoreBox make_box(const std::array<int, 4>& box_indices, Player &player) const;
	int calculate_box_score(const std::array<int, 4> &line_indices, const Player &last_player) const;
	void render_box_score(const char score, const SDL_Point &top_left, const Player &player);

	SDL_Rect viewport_rect;

	int cols, rows, n_edges;
	int point_radius, line_width;

	MouseState mouse_state;
	bool mouse_clicked;

	// debuggin variables
	bool _show_collision_boxes;

	Line hover_line, prev_hover_line;
	int prev_n_lines, prev_n_boxes;

	std::vector<SDL_Point> grid_points;
	std::vector<Line> grid_lines;
	std::vector<bool> taken_grid_lines;
	std::vector<CollisionRect> grid_collision_rects;
	std::vector<ScoreBox> grid_score_boxes;
	
	std::array<std::unique_ptr<TextTexture>, 4> score_textures;

	std::unique_ptr<TextureWrapper> grid_texture, hover_line_texture;
};