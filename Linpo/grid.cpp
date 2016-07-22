#include <algorithm>
#include "grid.h"
#include "globals.h"
#include "constants.h"
#include "sharedfunctions.h"
#include "render_functions.h"


Grid::Grid(int cols, int rows)
{
	this->cols = cols;
	this->rows = rows;
	point_radius = 5;
	line_width = 2 * point_radius;

	grid_texture = new TextureWrapper(main_renderer, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	mouse_x = 0;
	mouse_y = 0;
	mouse_clicked = false;

	hover_line = Line();
	prev_n_lines = 0;

	int n_points = rows * cols;
	int n_edges = 2 * rows * cols - rows - cols;
	int n_boxes = (rows - 1) * (cols - 1);
	grid_points.reserve(n_points);
	grid_collision_rects.reserve(n_edges);
	grid_lines.reserve(n_edges);
	grid_score_boxes.reserve(n_boxes);
	resize_update();
}

Grid::~Grid()
{
	delete grid_texture;
}

void Grid::handle_event(SDL_Event&e)
{
	if (e.type == SDL_WINDOWEVENT &&e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
		resize_update();
	else if (e.type == SDL_MOUSEBUTTONDOWN)
	{
		if (e.button.button == SDL_BUTTON_LEFT)
			mouse_clicked = true;
	}
}

void Grid::render()
{
	SDL_RenderSetViewport(main_renderer, &viewport_rect);

	grid_texture->set_as_render_target();

	SDL_SetRenderDrawColor(main_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(main_renderer);

	if (hover_line.start != nullptr &&hover_line.end != nullptr)
	{
		auto hover_line_color = hover_line.owner->color;
		hover_line_color.a = 60;
		render_line(*hover_line.start, *hover_line.end, line_width, hover_line_color);
	}

	for (const auto &line : grid_lines)
		render_line(*line.start, *line.end, line_width, line.owner->color);

	for (const auto &grid_box_score : grid_score_boxes)
	{
		render_box_score(grid_box_score.score, *grid_box_score.top_left, grid_box_score.owner->color);
	}

	render_points(grid_points, point_radius, { 0, 0, 0, 0xFF });

	grid_texture->reset_render_target();
	grid_texture->render();
}

void Grid::update_grid_points()
{
	grid_points.clear();
	int padding_x = 0.05 * width;
	int padding_y = 0.05 * height;

	for (int row = 0; row < rows; ++row)
	{
		for (int col = 0; col < cols; ++col)
		{
			SDL_Point point = { padding_x + col * (width / cols), padding_y + row * (height / rows) };
			grid_points.push_back(point);
		}
	}
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
			int a_index = row * cols + col;
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
	SDL_GetRendererOutputSize(main_renderer, &this->width, &this->height);

	viewport_rect.x = 0;
	viewport_rect.y = 20;
	viewport_rect.w = width;
	viewport_rect.h = height - viewport_rect.y;

	grid_texture->resize(width, height);
	update_grid_points();
	update_grid_collision_rects();
}

void Grid::update(Player &player)
{
	SDL_GetMouseState(&mouse_x, &mouse_y);

	handle_mouse_hover(player);
	
	if (mouse_clicked)
	{
		handle_mouse_click(player);
		mouse_clicked = false;
	}
}

void Grid::handle_mouse_click(Player &player)
{
	if (!is_line_taken(hover_line))
	{
		// mouse hover always happens before a click, so no need for anything else
		set_grid_line(hover_line);

		auto boxes = get_boxes_around_line(grid_lines.back());

		std::for_each(boxes.begin(), boxes.end(), [this, &player](auto &box) { 
			grid_score_boxes.push_back(box);
			player.score += box.score; 
		});
	}
}

void Grid::handle_mouse_hover(Player &player)
{
	Line new_line;
	if (make_collision_line(new_line, mouse_x, mouse_y, player))
		hover_line = new_line;
}

bool Grid::make_collision_line(Line &new_line, int x, int y, Player &player)
{
	CollisionRect target_rect;
	if (check_collision(x, y, target_rect))
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
		[&line](const Line &grid_line) -> bool { return (grid_line.start == line.start &&grid_line.end == line.end) || (grid_line.end == line.start &&grid_line.start == line.end); });

	if (it != grid_lines.end())
		return true;
	return false;
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

bool Grid::check_collision(int x, int y, CollisionRect &target_rect)
{
	SDL_Point mouse_point = { x, y };
	for (auto &grid_collision_rect : grid_collision_rects)
	{
		if (SDL_PointInRect(&mouse_point, &grid_collision_rect.collision_rect) == SDL_TRUE)
		{
			target_rect = grid_collision_rect;
			return true;
		}
	}
	return false;
}

void Grid::render_box_score(const int score, const SDL_Point &top_left, const SDL_Color &color)
{
	int font_width = 8;  // temp
	auto point_distance = get_point_distance();
	auto score_string = std::to_string(score);
	SDL_Point centered_top_left;
	centered_top_left.x = top_left.x + (point_distance.x / 2) - (score_string.length() * font_width) + point_radius;
	centered_top_left.y = top_left.y + (point_distance.y / 2) - (score_string.length() * font_width) + point_radius;

	render_string(std::to_string(score), centered_top_left, color);
}

bool Grid::new_line_placed()
{
	if (grid_lines.size() > prev_n_lines)
	{
		prev_n_lines = grid_lines.size();
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