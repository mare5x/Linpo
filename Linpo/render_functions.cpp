#include "render_functions.h"
#include "globals.h"


//void render_line(const SDL_Point & start, const SDL_Point & end, int width, const SDL_Color & color)
//{
//	thickLineRGBA(main_renderer, start.x, start.y, end.x, end.y, width, color.r, color.g, color.b, color.a);
//}

void render_line(const SDL_Point & start, const SDL_Point & end, int width, const SDL_Color & color)
{
	SDL_Rect draw_rect;
	if (start.y == end.y)
	{
		draw_rect.x = start.x;
		draw_rect.y = start.y - width / 2;
		draw_rect.w = end.x - start.x;
		draw_rect.h = width;
	}
	else
	{
		draw_rect.x = start.x - width / 2;
		draw_rect.y = start.y;
		draw_rect.w = width;
		draw_rect.h = end.y - start.y;
	}

	set_render_draw_color(color);
	SDL_RenderFillRect(main_renderer, &draw_rect);
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

void render_rect(const SDL_Rect & rect, const SDL_Color & color)
{
	set_render_draw_color(color);
	SDL_RenderFillRect(main_renderer, &rect);
}

void set_render_draw_color(const SDL_Color &color)
{
	SDL_SetRenderDrawColor(main_renderer, color.r, color.g, color.b, color.a);
}