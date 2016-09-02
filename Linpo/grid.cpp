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
	grid_texture(std::make_unique<TextureWrapper>(main_renderer)),
	hover_line_texture(std::make_unique<TextureWrapper>(main_renderer))
{
	// grid and hover line textures will get properly resized later
	
	// initialize score_textures
	for (int i = 0; i < N_PLAYERS; ++i)
	{
		for (int score = 1; score <= 4; ++score)
		{
			const int index = (i * 4) + score - 1;
			const char score_str[2] = { score + '0', '\0' };
			score_textures[index] = std::make_unique<TextTexture>(main_renderer);
			score_textures[index]->write_text(score_str, COLORS[i]);
		}
	}

	int n_points = rows * cols;
	int n_boxes = (rows - 1) * (cols - 1);
	grid_points.reserve(n_points);
	grid_collision_rects.reserve(n_edges);
	grid_lines.reserve(n_edges);
	grid_score_boxes.reserve(n_boxes);
	resize_update();
}

void Grid::handle_event(SDL_Event &e)
{
	if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
		resize_update();
	else if (e.type == SDL_MOUSEBUTTONDOWN)
	{
		if (e.button.button == SDL_BUTTON_LEFT)
			mouse_state.clicked = true;
	}
}

void Grid::render()
{
	// VIEWPORT CHANGES THE ORIGIN (0, 0)! BE CAREFUL!
	SDL_RenderSetViewport(main_renderer, &viewport_rect);

	grid_texture->render();

	if (hover_line.owner != nullptr && !is_line_taken(hover_line))
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
			SDL_RenderDrawRect(main_renderer, &rect.collision_rect);
	}

	grid_texture->reset_render_target();
}

void Grid::update_hover_line_texture()
{
	if (hover_line.owner == nullptr)
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
	bool ret_val = prev_hover_line != hover_line && !is_line_taken(hover_line);
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

void Grid::add_grid_score_boxes(std::vector<ScoreBox>& score_boxes, Player &player)
{
	for (auto &box : score_boxes) 
	{
		grid_score_boxes.push_back(box);
		player.score += box.score;
	};
}

void Grid::set_grid_line(Line line)
{
	grid_lines.push_back(line);
}

void Grid::update_grid_collision_rects()
{
	SDL_Point point_distance = get_point_distance();
	grid_collision_rects.clear();
	for (int row = 0; row < rows; ++row)
	{
		for (int col = 0; col < cols; ++col)
		{
			int a_index = get_grid_point_index(row, col);
			SDL_Point point_a = grid_points[a_index];

			// make a collision_rect for the x direction
			if (col < cols - 1)
			{
				SDL_Rect collision_rect;
				collision_rect.x = point_a.x + (0.025 * point_distance.x) + point_radius;
				collision_rect.y = point_a.y - (0.25 * point_distance.y);
				collision_rect.w = point_distance.x * 0.95 - point_radius;
				collision_rect.h = point_distance.y * 0.5;
				
				CollisionRect rect;
				rect.collision_rect = collision_rect;
				rect.point_a = &grid_points[a_index];
				rect.point_b = &grid_points[a_index + 1];
				grid_collision_rects.push_back(rect);
			}

			// make a collision_rect for the y direction
			if (row < rows - 1)
			{
				SDL_Rect collision_rect;
				collision_rect.x = point_a.x - (0.25 * point_distance.x);
				collision_rect.y = point_a.y + (0.025 * point_distance.y) + point_radius;
				collision_rect.w = point_distance.x * 0.5;
				collision_rect.h = point_distance.y * 0.95 - point_radius;

				CollisionRect rect;
				rect.collision_rect = collision_rect;
				rect.point_a = &grid_points[a_index];
				rect.point_b = &grid_points[a_index + cols];
				grid_collision_rects.push_back(rect);
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
	SDL_GetMouseState(&mouse_state.pos.x, &mouse_state.pos.y);
	// "global" detection, later local
	if (SDL_PointInRect(&mouse_state.pos, &viewport_rect))
	{
		// necessary to undo the viewport offset on mouse position since other mouse
		// functions are "global" and all grid coordinates are local to the current viewport (origin is at (x,y) of viewport_rect)
		mouse_state.pos.x -= viewport_rect.x;
		mouse_state.pos.y -= viewport_rect.y;

		handle_mouse_hover(player);
	
		if (mouse_state.clicked)
		{
			handle_mouse_click(player);
		}
	}
	mouse_state.clicked = false;  // reset clicked state every update call
	update_textures();
}

void Grid::handle_mouse_click(Player &player)
{
	if (!is_line_taken(hover_line))
	{
		// mouse hover always happens before a click, so no need for anything else
		if (hover_line.owner != nullptr)
			set_grid_line(hover_line);

		auto boxes = get_boxes_around_line(grid_lines.back());

		add_grid_score_boxes(boxes, player);
	}
}

void Grid::handle_mouse_hover(Player &player)
{
	Line new_line;
	if (make_collision_line(new_line, mouse_state.pos, player))
	{
		if (new_line != hover_line)
			hover_line = new_line;
	}
}

bool Grid::make_collision_line(Line &new_line, const SDL_Point &pos, Player &player)
{
	CollisionRect target_rect;
	if (check_collision(pos, target_rect))
	{
		new_line.start = target_rect.point_a;
		new_line.end = target_rect.point_b;
		new_line.owner = &player;
		return true;
	}
	return false;
}

bool Grid::is_line_taken(Line &line)
{
	auto it = std::find_if(grid_lines.begin(), grid_lines.end(), 
		[&line](const Line &grid_line) -> bool { return (grid_line.start == line.start && grid_line.end == line.end) || (grid_line.end == line.start && grid_line.start == line.end); });

	if (it != grid_lines.end())
		return true;
	return false;
}

bool Grid::is_grid_full()
{
	return grid_lines.size() >= n_edges;
}

int Grid::get_grid_point_index(int row, int col)
{
	if (row < rows && col < cols)
		return (row * cols) + col;
	return -1;
}

ScoreBox Grid::make_box(const Line & top_line, const Line & right_line, const Line & bot_line, const Line & left_line, Player & player)
{
	ScoreBox new_box;
	new_box.top_left = left_line.start;
	new_box.score = calculate_box_score(top_line, right_line, bot_line, left_line, player);
	new_box.owner = &player;

	return new_box;
}

// returns a ScoreBox vector of size [0, 2] of the adjoining two (or one) boxes,
// if those boxes are full (lines on all 4 sides)
std::vector<ScoreBox> Grid::get_boxes_around_line(Line &line)
{
	std::vector<ScoreBox> boxes;
	if (line.start->x == line.end->x)  // if vertical line: check left and right
	{
		// find the left box
		{
			Line* l_top = nullptr, *l_bot = nullptr, *l_left = nullptr;
			Line* l_right = &line;
			if (find_box(l_right, l_top, l_right, l_bot, l_left))
			{
				ScoreBox left_box = make_box(*l_top, *l_right, *l_bot, *l_left, *line.owner);
				boxes.push_back(left_box);
			}
		}

		// find the right box
		{
			Line* l_top = nullptr, *l_bot = nullptr, *l_right = nullptr;
			Line* l_left = &line;
			if (find_box(l_left, l_top, l_right, l_bot, l_left))
			{
				ScoreBox right_box = make_box(*l_top, *l_right, *l_bot, *l_left, *line.owner);
				boxes.push_back(right_box);
			}
		}
	}
	else  // if horizontal line: check top and bottom
	{
		// find the box on top
		{
			Line* l_top = nullptr, *l_right = nullptr, *l_left = nullptr;
			Line* l_bot = &line;
			if (find_box(l_bot, l_top, l_right, l_bot, l_left))
			{
				ScoreBox top_box = make_box(*l_top, *l_right, *l_bot, *l_left, *line.owner);
				boxes.push_back(top_box);
			}
		}

		// find the bot box
		{
			Line* l_right = nullptr, *l_left = nullptr, *l_bot = nullptr;
			Line* l_top = &line;
			if (find_box(l_top, l_top, l_right, l_bot, l_left))
			{
				ScoreBox bot_box = make_box(*l_top, *l_right, *l_bot, *l_left, *line.owner);
				boxes.push_back(bot_box);
			}
		}
	}
	return boxes;
}

bool Grid::find_box(const Line* base_line, Line* &top, Line* &right, Line* &bot, Line* &left)
{
	auto point_distance = get_point_distance();
	if (base_line != nullptr)
	{
		// check bounds
		if (base_line == right && right->start->x - point_distance.x < grid_points[0].x)
			return false;
		else if (base_line == left && left->start->x + point_distance.x > grid_points[cols - 1].x)
			return false;
		else if (base_line == top && top->start->y + point_distance.y > grid_points.back().y)
			return false;
		else if (base_line == bot && bot->start->y - point_distance.y < grid_points[0].y)
			return false;

		for (auto &grid_line : grid_lines)
		{
			if (base_line->start == grid_line.start && base_line->end == grid_line.end)
				continue;

			if (top != nullptr && right != nullptr && bot != nullptr && left != nullptr)
				return true;

			if (base_line == left)
			{
				if (top == nullptr && left->start == grid_line.start)
					top = &grid_line;
				else if (bot == nullptr && left->end == grid_line.start && left->end->y == grid_line.end->y)
					bot = &grid_line;
				else if (right == nullptr && left->start->x + point_distance.x == grid_line.start->x && left->start->y == grid_line.start->y && grid_line.start->x == grid_line.end->x)
					right = &grid_line;
			}
			else if (base_line == right)
			{
				if (top == nullptr && right->start == grid_line.end && right->start->y == grid_line.start->y)
					top = &grid_line;
				else if (bot == nullptr && right->end == grid_line.end)
					bot = &grid_line;
				else if (left == nullptr && right->start->x - point_distance.x == grid_line.start->x && right->start->y == grid_line.start->y && grid_line.start->x == grid_line.end->x)
					left = &grid_line;
			}
			else if (base_line == top)
			{
				if (right == nullptr && top->end == grid_line.start && grid_line.start->x == grid_line.end->x)
					right = &grid_line;
				else if (left == nullptr && top->start == grid_line.start)
					left = &grid_line;
				else if (bot == nullptr && top->start->y + point_distance.y == grid_line.start->y && top->start->x == grid_line.start->x && grid_line.start->y == grid_line.end->y)
					bot = &grid_line;
			}
			else if (base_line == bot)
			{
				if (right == nullptr && bot->end == grid_line.end)
					right = &grid_line;
				else if (left == nullptr && bot->start == grid_line.end && grid_line.start->x == grid_line.end->x)
					left = &grid_line;
				else if (top == nullptr && bot->start->y - point_distance.y == grid_line.start->y && bot->start->x == grid_line.start->x && grid_line.start->y == grid_line.end->y)
					top = &grid_line;
			}
		}
	}

	if (top != nullptr && right != nullptr && bot != nullptr && left != nullptr)
		return true;

	return false;
}

int Grid::calculate_box_score(const Line & top, const Line & right, const Line & bot, const Line & left, const Player &last_player)
{
	int score = 0;
	if (top.owner == &last_player)
		++score;
	if (right.owner == &last_player)
		++score;
	if (bot.owner == &last_player)
		++score;
	if (left.owner == &last_player)
		++score;
	return score;
}

bool Grid::check_collision(const SDL_Point &point, CollisionRect &target_rect)
{
	for (const auto &grid_collision_rect : grid_collision_rects)
	{
		if (SDL_PointInRect(&point, &grid_collision_rect.collision_rect) == SDL_TRUE)
		{
			target_rect = grid_collision_rect;
			return true;
		}
	}
	return false;
}

void Grid::render_box_score(const char score, const SDL_Point &top_left, const Player &player)
{
	const auto &point_distance = get_point_distance();

	const auto &score_texture = score_textures[(player.id * 4) + score - 1];

	const int &text_width = score_texture->get_width();
	const int &text_height = score_texture->get_height();

	int x = top_left.x + (point_distance.x / 2) - (text_width / 2);
	int y = top_left.y + (point_distance.y / 2) - (text_height / 2) - (point_radius / 2);
	
	score_texture->render(x, y);
}

void Grid::clear_grid()
{
	grid_lines.clear();
	grid_score_boxes.clear();

	resize_update();

	prev_n_lines = 0;
	prev_n_boxes = 0;
}

std::vector<SDL_Point> &Grid::get_grid_points()
{
	return grid_points;
}

Line &Grid::get_last_line_placed()
{
	return grid_lines.back();
}

bool Grid::new_line_placed(int &prev_lines)
{
	if (grid_lines.size() > prev_lines)
	{
		prev_lines = grid_lines.size();
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

SDL_Point Grid::get_point_distance()
{
	SDL_Point point_distance = { 0, 0 };
	point_distance.x = (grid_points[1].x - grid_points[0].x);
	point_distance.y = (grid_points[cols].y - grid_points[0].y);

	return point_distance;
}