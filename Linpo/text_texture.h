#pragma once

#include <string>
#include "SDL_ttf.h"
#include "texture_wrapper.h"


class TextTexture : public TextureWrapper
{
public:
	using TextureWrapper::TextureWrapper;
	using TextureWrapper::render;

	TextTexture(SDL_Renderer* &renderer);
	~TextTexture();

	void write_text(const std::string &s, const SDL_Color &color);
	void get_text_size(int &w, int &h);
	void set_font_size(const int font_size);

	void set_render_pos(const SDL_Point &top_left);
	void render();
private:
	std::string text;
	SDL_Point top_left_pos;
	char font_size;
	TTF_Font* text_font;
};


int calculate_font_size();