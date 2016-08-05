#pragma once

#include <string>
#include "texture_wrapper.h"


class TextTexture : public TextureWrapper
{
public:
	using TextureWrapper::TextureWrapper;

	void write_text(const std::string &s, const SDL_Color &color);
	void get_text_size(int &w, int &h);

	void set_render_pos(const SDL_Point &top_left);
	void render();
private:
	std::string text;
	SDL_Point top_left_pos;
};