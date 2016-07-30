#pragma once

#include "SDL2_gfxPrimitives.h"

#include <vector>
#include <string>


void render_line(const SDL_Point & start, const SDL_Point & end, int width, const SDL_Color & color);

void render_point(const SDL_Point & point, int radius, const SDL_Color & color);

void render_points(const std::vector<SDL_Point> &points, int radius, const SDL_Color &color);

void render_string(const std::string s, const SDL_Point &top_left, const SDL_Color &color);

void render_char(const char c, const SDL_Point &top_left, const SDL_Color &color);