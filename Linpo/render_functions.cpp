#include "render_functions.h"
#include "globals.h"


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

void render_circle(const SDL_Point & center, int radius, const SDL_Color & color)
{
	set_render_draw_color(color);

	int x = radius;
	int y = 0;
	int err = 0;

	while (x >= y)
	{
		SDL_RenderDrawPoint(main_renderer, center.x + x, center.y + y);
		SDL_RenderDrawPoint(main_renderer, center.x + y, center.y + x);
		SDL_RenderDrawPoint(main_renderer, center.x - y, center.y + x);
		SDL_RenderDrawPoint(main_renderer, center.x - x, center.y + y);
		SDL_RenderDrawPoint(main_renderer, center.x - x, center.y - y);
		SDL_RenderDrawPoint(main_renderer, center.x - y, center.y - x);
		SDL_RenderDrawPoint(main_renderer, center.x + y, center.y - x);
		SDL_RenderDrawPoint(main_renderer, center.x + x, center.y - y);

		y += 1;
		err += 1 + 2 * y;
		if (2 * (err - x) + 1 > 0)
		{
			x -= 1;
			err += 1 - 2 * x;
		}
	}
}

void render_circle_filled(const SDL_Point & center, int radius, const SDL_Color & color)
{
	set_render_draw_color(color);

	int x = 0;
	for (int y = 0; y <= radius; y++)
	{
		x = sqrt(radius * radius - y * y);

		if (x <= radius)
		{
			SDL_RenderDrawLine(main_renderer, center.x - x, center.y + y, center.x + x, center.y + y);
			SDL_RenderDrawLine(main_renderer, center.x - x, center.y - y, center.x + x, center.y - y);
		}
	}
}

void render_circles_filled(const std::vector<SDL_Point>& points, int radius, const SDL_Color & color)
{
	for (auto const &point : points)
	{
		render_circle_filled(point, radius, color);
	}
}

void render_rect(const SDL_Rect & rect, const SDL_Color & color)
{
	set_render_draw_color(color);
	SDL_RenderFillRect(main_renderer, &rect);
}

inline void set_render_draw_color(const SDL_Color &color)
{
	SDL_SetRenderDrawColor(main_renderer, color.r, color.g, color.b, color.a);
}