#pragma once

#include <SDL.h>
#include <vector>
#include <string>


void render_line(const SDL_Point & start, const SDL_Point & end, int width, const SDL_Color &color);

void render_circle(const SDL_Point &center, int radius, const SDL_Color &color);

void render_circle_filled(const SDL_Point &center, int radius, const SDL_Color &color);

void render_circles_filled(const std::vector<SDL_Point> &points, int radius, const SDL_Color &color);

void render_rect(const SDL_Rect &rect, const SDL_Color &color);

inline void set_render_draw_color(const SDL_Color &color);