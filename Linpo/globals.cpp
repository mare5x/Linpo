#include "globals.h"

SDL_Window* main_window = nullptr;
SDL_Renderer* main_renderer = nullptr;

TTF_Font* global_font = nullptr;

int FONT_SIZE = 0;
COLOR_THEME GLOBAL_COLOR_THEME = COLOR_THEME::DEFAULT;

void set_global_color_theme_render_color()
{
	switch (GLOBAL_COLOR_THEME)
	{
	case COLOR_THEME::DEFAULT:
		SDL_SetRenderDrawColor(main_renderer, 255, 255, 255, 255);
		break;
	case COLOR_THEME::BLACK:
		SDL_SetRenderDrawColor(main_renderer, 0, 0, 0, 255);
		break;
	}
}

const SDL_Color & get_font_color(const COLOR_THEME & color_theme)
{
	switch (color_theme)
	{
	case COLOR_THEME::DEFAULT:
		return COLORS[BLACK];
	case COLOR_THEME::BLACK:
		return COLORS[LIME];
	}
}
