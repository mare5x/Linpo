#pragma once

#include <SDL.h>


class TextureWrapper
{
public:
	TextureWrapper(SDL_Renderer* &renderer, int w, int h, int access = SDL_TEXTUREACCESS_TARGET, SDL_Color base_color = { 0xFF, 0xFF, 0xFF, 0xFF });
	~TextureWrapper();

	void resize(int new_width, int new_height);

	void set_as_render_target();
	void reset_render_target();

	/* Clears the texture to the base color and sets it as the current rendering target. */
	void clear();
	/* Clears the texture to the clear_color and sets it as the current rendering target. */
	void clear(const SDL_Color &clear_color);

	void render();
	void render(int x, int y);

	SDL_Texture* get_texture() { return texture; }
	int get_width() { return width; }
	int get_height() { return height; }
private:
	SDL_Renderer* &renderer;

	SDL_Texture* texture;

	SDL_Color base_color;
	int access;

	int width, height;
};