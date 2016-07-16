#pragma once

#include <SDL.h>


class TextureWrapper
{
public:
	TextureWrapper(SDL_Renderer* renderer, int access, int w, int h);
	~TextureWrapper();

	void resize(int new_width, int new_height);

	void set_as_render_target();
	void reset_render_target();

	void render();

	SDL_Texture* get_texture();
private:
	SDL_Texture* texture;

	SDL_Renderer* renderer;
	int access;
};