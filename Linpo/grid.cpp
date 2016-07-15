#include "SDL2_gfxPrimitives.h"

#include "grid.h"
#include "globals.h"
#include "constants.h"
#include "sharedfunctions.h"


Grid::Grid(int cols, int rows)
{
	this->cols = cols;
	this->rows = rows;
	point_radius = 5;
	line_width = 2 * point_radius;

	mouse_x = 0;
	mouse_y = 0;
	mouse_clicked = false;

	hover_line = Line();

	int n_points = rows * cols;
	n_edges = 2 * rows * cols - rows - cols;
	grid_points.reserve(n_points);
	grid_collision_rects.reserve(n_edges);
	grid_lines.reserve(n_edges);
	resize_update();
}

void Grid::handle_event(SDL_Event& e)
{
	if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
		resize_update();
	else if (e.type == SDL_MOUSEBUTTONDOWN)
	{
		if (e.button.button == SDL_BUTTON_LEFT)
			mouse_clicked = true;
	}
	else if (e.type == SDL_MOUSEMOTION)
	{
		mouse_x = e.motion.x;
		mouse_y = e.motion.y;
	}
}

void Grid::render()
{
	if (hover_line.start != nullptr && hover_line.end != nullptr)
		render_line(*hover_line.start, *hover_line.end, { 255, 0, 0, 64 });

	for (auto & line : grid_lines)
	{
		if (line.owner == PLAYER1)
			render_line(*line.start, *line.end, PLAYER1_COLOR);
		else if (line.owner == PLAYER2)
			render_line(*line.start, *line.end, PLAYER2_COLOR);
	}

	render_points(grid_points, { 0, 0, 0, 0xFF });
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
	if (grid_lines.size() < n_edges)
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
	SDL_GetRendererOutputSize(mainRenderer, &this->width, &this->height);
	update_grid_points();
	update_grid_collision_rects();
}

void Grid::update()
{
	handle_mouse_hover(PLAYER1);
	if (mouse_clicked)
	{
		handle_mouse_click(PLAYER1);
		mouse_clicked = false;
	}
}

void Grid::handle_mouse_click(Player player)
{
	set_grid_line(hover_line);
}

void Grid::handle_mouse_hover(Player player)
{
	Line new_line;
	if (make_collision_line(new_line, mouse_x, mouse_y, player))
		hover_line = new_line;
}

bool Grid::make_collision_line(Line & new_line, int x, int y, Player player)
{
	CollisionRect target_rect;
	if (check_collision(x, y, target_rect))
	{
		new_line.start = target_rect.point_a;
		new_line.end = target_rect.point_b;
		new_line.owner = player;
		return true;
	}
	return false;
}

bool Grid::check_collision(int x, int y, CollisionRect & target_rect)
{
	SDL_Point mouse_point = { x, y };
	for (auto & grid_collision_rect : grid_collision_rects)
	{
		if (SDL_PointInRect(&mouse_point, &grid_collision_rect.collision_rect) == SDL_TRUE)
		{
			target_rect = grid_collision_rect;
			return true;
		}
	}
	return false;
}

void Grid::render_line(SDL_Point& start, SDL_Point& end, const SDL_Color& color)
{
	thickLineRGBA(mainRenderer, start.x, start.y, end.x, end.y, line_width, color.r, color.g, color.b, color.a);
}

void Grid::render_point(const SDL_Point & point, const SDL_Color & color)
{
	filledCircleRGBA(mainRenderer, point.x, point.y, point_radius, color.r, color.g, color.b, color.a);
}

void Grid::render_points(const std::vector<SDL_Point>& points, const SDL_Color & color)
{
	for (auto const & point : points)
	{
		render_point(point, color);
	}
}

SDL_Point Grid::get_point_distance()
{
	SDL_Point point_distance = { 0, 0 };
	point_distance.x = (grid_points[1].x - grid_points[0].x);
	point_distance.y = (grid_points[cols].y - grid_points[0].y);

	return point_distance;
}