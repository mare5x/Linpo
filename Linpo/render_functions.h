#pragma once

#include "SDL2_gfxPrimitives.h"

#include <vector>
#include <string>


void render_line(const SDL_Point & start, const SDL_Point & end, int width, const SDL_Color &color);

void render_point(const SDL_Point & point, int radius, const SDL_Color &color);

void render_points(const std::vector<SDL_Point> &points, int radius, const SDL_Color &color);

void render_rect(const SDL_Rect &rect, const SDL_Color &color);

void set_render_draw_color(const SDL_Color &color);