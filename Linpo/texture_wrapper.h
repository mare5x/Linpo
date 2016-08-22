#pragma once

#include <SDL.h>


class TextureWrapper
{
public:
	TextureWrapper(SDL_Renderer* &renderer, int w, int h, int access = SDL_TEXTUREACCESS_TARGET, SDL_Color base_color = { 0xFF, 0xFF, 0xFF, 0xFF });
	TextureWrapper(SDL_Renderer* &renderer);
	~TextureWrapper();

	void resize(int new_width, int new_height);

	void set_as_render_target();
	void reset_render_target();

	/* Clears the texture to the base color and sets it as the current rendering target. */
	void clear();
	/* Clears the texture to the clear_color and sets it as the current rendering target. */
	void clear(const SDL_Color &clear_color);

	void free();

	void render();
	void render(int x, int y);
	void render(const SDL_Rect &dest_rect);

	SDL_Texture* get_texture() { return texture; }
	const int &get_width() const { return width; }
	const int &get_height() const { return height; }
protected:
	SDL_Renderer* &renderer;
	SDL_Texture* texture;
	int width, height;
private:
	SDL_Color base_color;
	int access;
};