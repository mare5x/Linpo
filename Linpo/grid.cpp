#include <algorithm>
#include "grid.h"
#include "globals.h"
#include "constants.h"
#include "render_functions.h"


Grid::Grid(int cols, int rows)
	:viewport_rect{ 0, static_cast<int>(0.05 * SCREEN_HEIGHT), SCREEN_WIDTH, SCREEN_HEIGHT - viewport_rect.y },
	cols(cols),
	rows(rows),
	n_edges(2 * rows * cols - rows - cols),
	point_radius(5),
	line_width(2 * point_radius),
	mouse_state{},
	_show_collision_boxes(false),
	hover_line{},
	prev_hover_line{},
	prev_n_lines(0),
	prev_n_boxes(0),
	lines_placed(0),
	grid_texture(std::make_unique<TextureWrapper>(main_renderer)),
	hover_line_texture(std::make_unique<TextureWrapper>(main_renderer))
{
	// grid and hover line textures will get properly resized later
	
	// initialize score_textures
	for (int score = 0; score < 4; ++score)
	{
		const char score_str[2] = { score + 1 + '0', '\0' };
		score_textures[score] = std::make_unique<TextTexture>(main_renderer);
		score_textures[score]->write_text(score_str, COLORS[WHITE]);
	}

	int n_points = rows * cols;
	int n_boxes = (rows - 1) * (cols - 1);
	grid_points.reserve(n_points);
	grid_score_boxes.reserve(n_boxes);

	init_grid_lines();

	resize_update();
}

void Grid::handle_event(SDL_Event &e)
{
	switch (e.type)
	{
	case SDL_WINDOWEVENT:
		if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
			resize_update();
		break;
	case SDL_MOUSEMOTION:
		SDL_GetMouseState(&mouse_state.pos.x, &mouse_state.pos.y);
		break;
	case SDL_MOUSEBUTTONDOWN:
		if (e.button.button == SDL_BUTTON_LEFT)
			if (SDL_PointInRect(&mouse_state.pos, &viewport_rect))
				mouse_state.pressed = true;
		break;
	case SDL_MOUSEBUTTONUP:
		if (e.button.button == SDL_BUTTON_LEFT)
		{
			if (mouse_state.pressed)
				mouse_clicked = true;
			mouse_state.pressed = false;
		}
	}
}

void Grid::render()
{
	// VIEWPORT CHANGES THE ORIGIN (0, 0)! BE CAREFUL!
	SDL_RenderSetViewport(main_renderer, &viewport_rect);

	grid_texture->render();

	if (hover_line.has_owner())
		if (hover_line.start->y == hover_line.end->y)  // horizontal
			hover_line_texture->render(hover_line.start->x, hover_line.start->y - line_width / 2);
		else
			hover_line_texture->render(hover_line.start->x - line_width / 2, hover_line.start->y + point_radius / 2);

	// (for debugging purposes below)
	//render_point(mouse_state.pos, point_radius, COLORS[RED]);
	//SDL_RenderDrawRect(main_renderer, NULL);
}

void Grid::update_textures()
{
	if (grid_texture_update_pending())
		update_grid_texture();
	if (hover_line_update_pending())
		update_hover_line_texture();
}

void Grid::update_grid_texture()
{
	grid_texture->clear();

	for (const auto &line : grid_lines)
		if (line.has_owner())
			render_line(*line.start, *line.end, line_width, *line.owner->color);

	for (const auto &grid_box_score : grid_score_boxes)
	{
		render_box_score(grid_box_score.score, *grid_box_score.top_left, *grid_box_score.owner);
	}

	render_points(grid_points, point_radius, COLORS[BLACK]);

	if (_show_collision_boxes)
	{
		set_render_draw_color(COLORS[BLACK]);
		for (const auto &rect : grid_collision_rects)
			SDL_RenderDrawRect(main_renderer, &rect);
	}

	grid_texture->reset_render_target();
}

void Grid::update_hover_line_texture()
{
	if (!hover_line.has_owner())
		return;

	if (hover_line.start->x == hover_line.end->x)  // is vertical
		hover_line_texture->resize(line_width, get_point_distance().y);  // resize() resizes only if it's necessary
	else  // horizontal
		hover_line_texture->resize(get_point_distance().x, line_width);

	SDL_Color hover_line_color = *hover_line.owner->color;
	hover_line_color.a = 60;
	hover_line_texture->clear(hover_line_color);

	hover_line_texture->reset_render_target();
}

bool Grid::hover_line_update_pending()
{
	bool ret_val = prev_hover_line != hover_line && hover_line.has_owner();
	prev_hover_line = hover_line;
	return ret_val;
}

bool Grid::grid_texture_update_pending()
{
	return new_line_placed(prev_n_lines) || score_changed(prev_n_boxes);
}

void Grid::update_grid_points()
{
	grid_points.clear();
	int padding_x = viewport_rect.w / cols / 2;
	int padding_y = viewport_rect.h / rows / 2;

	for (int row = 0; row < rows; ++row)
	{
		for (int col = 0; col < cols; ++col)
		{
			SDL_Point point = { padding_x + col * (viewport_rect.w / cols), padding_y + row * (viewport_rect.h / rows) };
			grid_points.push_back(point);
		}
	}
}

void Grid::init_grid_lines()
{
	update_grid_points();

	grid_lines.resize(n_edges);
	grid_lines.shrink_to_fit();

	taken_grid_lines = std::vector<bool>(n_edges, false);
	taken_grid_lines.shrink_to_fit();

	for (int row = 0; row < rows; ++row)
	{
		for (int col = 0; col < cols; ++col)
		{
			const auto line_index = get_grid_line_index(row, col);
			int start_index = get_grid_point_index(row, col);

			// horizontal line
			if (line_index[0] != -1)
			{
				Line& h_line = grid_lines[line_index[0]];
				h_line.start = &grid_points[start_index];
				h_line.end = &grid_points[start_index + 1];
				h_line.owner = nullptr;
			}

			// vertical line
			if (line_index[1] != -1)
			{
				Line& v_line = grid_lines[line_index[1]];
				v_line.start = &grid_points[start_index];
				v_line.end = &grid_points[get_grid_point_index(row + 1, col)];
				v_line.owner = nullptr;
			}
		}
	}
}

void Grid::add_grid_score_boxes(std::vector<ScoreBox>& score_boxes, Player &player)
{
	for (auto &box : score_boxes) 
	{
		grid_score_boxes.push_back(box);
		player.score += box.score;
	};
}

void Grid::set_grid_line(int index, Player &owner)
{
	grid_lines[index].owner = &owner;
	taken_grid_lines[index] = true;
	++lines_placed;
	//add_grid_score_boxes(get_boxes_around_line(index, owner), owner);
}

void Grid::remove_grid_line(int index)
{
	grid_lines[index].owner = nullptr;
	taken_grid_lines[index] = false;
	--lines_placed;
}

void Grid::update_grid_collision_rects()
{
	SDL_Point point_distance = get_point_distance();
	grid_collision_rects.resize(n_edges);
	grid_collision_rects.shrink_to_fit();
	for (int row = 0; row < rows; ++row)
	{
		for (int col = 0; col < cols; ++col)
		{
			const auto line_index = get_grid_line_index(row, col);
			int a_index = get_grid_point_index(row, col);
			SDL_Point point_a = grid_points[a_index];

			// make a collision_rect for the x direction
			if (line_index[0] != -1)
			{
				CollisionRect &rect = grid_collision_rects[line_index[0]];
				rect.x = point_a.x + (0.025 * point_distance.x) + point_radius;
				rect.y = point_a.y - (0.25 * point_distance.y);
				rect.w = point_distance.x * 0.95 - point_radius;
				rect.h = point_distance.y * 0.5;
			}

			// make a collision_rect for the y direction
			if (line_index[1] != -1)
			{
				CollisionRect &rect = grid_collision_rects[line_index[1]];
				rect.x = point_a.x - (0.25 * point_distance.x);
				rect.y = point_a.y + (0.025 * point_distance.y) + point_radius;
				rect.w = point_distance.x * 0.5;
				rect.h = point_distance.y * 0.95 - point_radius;
			}
		}
	}
}

void Grid::resize_update()
{
	SDL_GetRendererOutputSize(main_renderer, &viewport_rect.w, &viewport_rect.h);

	viewport_rect.y = 0.05 * viewport_rect.h;
	viewport_rect.h -= viewport_rect.y;

	grid_texture->resize(viewport_rect.w, viewport_rect.h);

	update_grid_points();
	update_grid_collision_rects();

	update_grid_texture();
	update_hover_line_texture();
}

void Grid::update(Player &player)
{
	// "global" mouse detection, later local
	if (SDL_PointInRect(&mouse_state.pos, &viewport_rect))
	{
		handle_mouse_hover(player);
	
		if (mouse_clicked)
		{
			handle_mouse_click(player);
		}
	}
	mouse_clicked = false;  // reset mouse_clicked since it has already been handled
	update_textures();
}

void Grid::handle_mouse_click(Player &player)
{
	if (hover_line.has_owner())
	{
		// mouse hover always happens before a click, so no need for anything else
		
		int hover_line_index = get_grid_line_index(hover_line);
		if (!taken_grid_lines[hover_line_index])
		{
			set_grid_line(hover_line_index, player);

			auto boxes = get_boxes_around_line(hover_line_index, player);

			add_grid_score_boxes(boxes, player);
		}
	}
}

void Grid::handle_mouse_hover(Player &player)
{
	// Note: necessary to undo the viewport offset on the mouse position since it's global,
	// and all grid coordinates are local to the current viewport (origin is at (x,y) of viewport_rect)

	Line new_line;
	SDL_Point local_mouse = { mouse_state.pos.x - viewport_rect.x, mouse_state.pos.y - viewport_rect.y };
	if (make_collision_line(new_line, local_mouse, player))
	{
		if (new_line != hover_line)
			hover_line = new_line;
	}
}

bool Grid::make_collision_line(Line &new_line, const SDL_Point &pos, Player &player)
{
	auto line_index = check_collision(pos);
	if (line_index != -1)
	{
		new_line = grid_lines[line_index];
		new_line.owner = &player;
	}
	return line_index != -1;
}

bool Grid::is_grid_full()
{
	return lines_placed == n_edges;
}

int Grid::get_grid_point_index(int row, int col) const
{
	if (row < rows && col < cols)
		return (row * rows) + col;
	return -1;
}

std::array<int, 2> Grid::get_grid_line_index(int row, int col) const
{
	std::array<int, 2> lines;

	int base = row * get_lines_in_row() + col;
	lines[0] = col + 1 < cols ? base : -1;						// horizontal
	lines[1] = row + 1 < rows ? base + (cols - 1) : -1;			// vertical

	return lines;
}

std::array<int, 4> Grid::get_grid_line_index(const SDL_Point & point) const
{
	auto point_distance = get_point_distance();
	int row = std::round((point.y - grid_points[0].y) / (float)point_distance.y);
	int col = std::round((point.x - grid_points[0].x) / (float)point_distance.x);

	if (row >= 0 && row < rows && col >= 0 && col < cols)
	{
		std::array<int, 4> indices;
		auto indices1 = get_grid_line_index(row, col);
		indices[0] = indices1[0];																				 // right horizontal
		indices[1] = indices1[1];																				 // down vertical
		indices[2] = indices[0] != -1 ? indices[0] - 1 : get_grid_line_index(row, col - 1)[0];					 // left horizontal
		indices[3] = indices[1] != -1 ? indices[1] - get_lines_in_row() : get_grid_line_index(row - 1, col)[1];  // up vertical
		return indices;
	}
	else
		return{ -1, -1, -1, -1 };
}

int Grid::get_grid_line_index(const Line & line) const
{
	auto indices = get_grid_line_index(*line.start);

	if (line.start->y == line.end->y)  // line is horizontal
		return indices[0];
	else
		return indices[1];
}

bool Grid::is_valid_top_line_index(int index) const
{
	int l_row = get_lines_in_row();
	return index >= 0 && index < (l_row * (rows - 1)) && index % l_row != (l_row - 1);
}

bool Grid::is_valid_box_indices(const std::array<int, 4>& box_indices) const
{
	if (!is_valid_top_line_index(box_indices[0]))
		return false;

	for (auto index : box_indices)
	{
		if (!taken_grid_lines[index]) 
			return false;
	}
	return true;
}

std::array<int, 4> Grid::get_box_indices(int top_index) const
{
	return {
		top_index,								// top
		top_index + (cols - 1),					// left
		top_index + get_lines_in_row(),			// bottom
		top_index + cols						// right
	};
}

ScoreBox Grid::make_box(const std::array<int, 4>& box_indices, Player & player) const
{
	ScoreBox new_box;
	new_box.top_left = grid_lines[box_indices[0]].start;
	new_box.score = calculate_box_score(box_indices, player);
	new_box.owner = &player;

	return new_box;
}

std::vector<ScoreBox> Grid::get_boxes_around_line(int line_index, Player &owner) const
{
	std::vector<ScoreBox> boxes;
	const Line &line = grid_lines[line_index];

	//if (!taken_grid_lines[line_index]) return boxes;  // leave it commented so the AI can test all possibilites

	int top_index;
	std::array<int, 4> indices;
	if (line.start->x == line.end->x)  // if vertical line: check left and right
	{
		int line_index_col = get_grid_line_index_col(line_index);
		// find the left box
		if (line_index_col > 0)
		{
			top_index = line_index - cols;
			indices = get_box_indices(top_index);
			if (is_valid_box_indices(indices))
				boxes.push_back(make_box(indices, owner));
		}

		// find the right box
		if (line_index_col < cols - 1)
		{
			top_index = (line_index - cols) + 1;
			indices = get_box_indices(top_index);
			if (is_valid_box_indices(indices))
				boxes.push_back(make_box(indices, owner));
		}
	}
	else  // if horizontal line: check top and bottom
	{
		int line_index_row = get_grid_line_index_row(line_index);
		// find the box on top
		if (line_index_row > 0)
		{
			top_index = line_index - get_lines_in_row();
			indices = get_box_indices(top_index);
			if (is_valid_box_indices(indices))
				boxes.push_back(make_box(indices, owner));
		}
		
		// find the bot box
		if (line_index_row < rows - 1)
		{
			top_index = line_index;
			indices = get_box_indices(top_index);
			if (is_valid_box_indices(indices))
				boxes.push_back(make_box(indices, owner));
		}
	}
	return boxes;
}

int Grid::calculate_box_score(const std::array<int, 4> &line_indices, const Player &last_player) const
{
	int score = 0;
	for (const auto index : line_indices)
		if (grid_lines[index].owner->id == last_player.id)
			++score;
	return score;
}

int Grid::check_collision(const SDL_Point & point) const
{
	auto indices = get_grid_line_index(point);
	for (int i = 0; i < indices.size(); ++i)
	{
		int index = indices[i];
		if (index >= 0 && SDL_PointInRect(&point, &grid_collision_rects[index]))
			return index;
	}
	return -1;
}

void Grid::render_box_score(const char score, const SDL_Point &top_left, const Player &player)
{
	const auto &point_distance = get_point_distance();

	const auto &score_texture = score_textures[score - 1];

	const int &text_width = score_texture->get_width();
	const int &text_height = score_texture->get_height();

	int x = top_left.x + (point_distance.x / 2) - (text_width / 2);
	int y = top_left.y + (point_distance.y / 2) - (text_height / 2) - (point_radius / 2);
	
	score_texture->set_color_mod(player.color->r, player.color->g, player.color->b);
	score_texture->render(x, y);
}

void Grid::clear_grid()
{
	init_grid_lines();
	grid_score_boxes.clear();

	resize_update();

	prev_n_lines = 0;
	prev_n_boxes = 0;
	lines_placed = 0;
}

void Grid::set_grid_size(int rows, int cols)
{
	this->rows = rows;
	this->cols = cols;
	n_edges = 2 * rows * cols - rows - cols;

	int n_points = rows * cols;
	int n_boxes = (rows - 1) * (cols - 1);
	grid_points.resize(n_points);
	grid_collision_rects.resize(n_edges);
	grid_score_boxes.resize(n_boxes);

	clear_grid();
}

bool Grid::new_line_placed(int &prev_lines)
{
	if (lines_placed > prev_lines)
	{
		prev_lines = lines_placed;
		return true;
	}
	return false;
}

bool Grid::score_changed(int &prev_boxes)
{
	if (grid_score_boxes.size() > prev_boxes)
	{
		prev_boxes = grid_score_boxes.size();
		return true;
	}
	return false;
}

SDL_Point Grid::get_point_distance() const
{
	SDL_Point point_distance = { 0, 0 };
	point_distance.x = (grid_points[1].x - grid_points[0].x);
	point_distance.y = (grid_points[cols].y - grid_points[0].y);

	return point_distance;
}