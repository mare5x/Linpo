#include "render_functions.h"
#include "globals.h"


void render_line(const SDL_Point & start, const SDL_Point & end, int width, const SDL_Color & color)
{
	thickLineRGBA(main_renderer, start.x, start.y, end.x, end.y, width, color.r, color.g, color.b, color.a);
}

void render_point(const SDL_Point & point, int radius, const SDL_Color & color)
{
	filledCircleRGBA(main_renderer, point.x, point.y, radius, color.r, color.g, color.b, color.a);
}

void render_points(const std::vector<SDL_Point>& points, int radius, const SDL_Color & color)
{
	for (auto const &point : points)
	{
		render_point(point, radius, color);
	}
}

void render_string(const std::string s, const SDL_Point & top_left, const SDL_Color & color)
{
	stringRGBA(main_renderer, top_left.x, top_left.y, s.c_str(), color.r, color.g, color.b, color.a);
}

