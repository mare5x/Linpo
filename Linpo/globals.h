#pragma once
#include "SDL_ttf.h"
#include "SDL.h"
#include "constants.h"

extern SDL_Window* main_window;
extern SDL_Renderer* main_renderer;

extern TTF_Font* global_font;

extern int FONT_SIZE;
extern COLOR_THEME GLOBAL_COLOR_THEME;

// change base background drawing color based on the selected theme
void set_global_color_theme_render_color();
const SDL_Color& get_font_color(const COLOR_THEME &color_theme);