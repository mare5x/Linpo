#pragma once

#include <string>
#include "texture_wrapper.h"


class TextTexture : public TextureWrapper
{
public:
	using TextureWrapper::TextureWrapper;
	TextTexture(SDL_Renderer* &renderer);
	~TextTexture();

	void write_text(const std::string &s, const SDL_Color &color, const char &font_size = GLOBAL_FONT_SIZE);
	void get_text_size(int &w, int &h);

	void set_render_pos(const SDL_Point &top_left);
	void render();
private:
	std::string text;
	SDL_Point top_left_pos;
	char font_size;
	TTF_Font* text_font;
};