#pragma once

#include <SDL.h>


class TextureWrapper
{
public:
	TextureWrapper(SDL_Renderer* &renderer, int w, int h, int access = SDL_TEXTUREACCESS_TARGET, SDL_Color background_color = { 0xFF, 0xFF, 0xFF, 0xFF });
	TextureWrapper(SDL_Renderer* &renderer, const char* file_path, int access = SDL_TEXTUREACCESS_TARGET, SDL_Color background_color = { 0xFF, 0xFF, 0xFF, 0xFF });
	TextureWrapper(SDL_Renderer* &renderer, int access = SDL_TEXTUREACCESS_TARGET, SDL_Color background_color = { 0xFF, 0xFF, 0xFF, 0xFF });
	~TextureWrapper();

	void resize(int new_width, int new_height);

	void set_as_render_target();
	void reset_render_target();

	/* Sets the background color, which is only applied when using TextureWrapper::clear(). */
	void set_background_color(const SDL_Color &background_color);

	/* Clears the texture to the base color and sets it as the current rendering target. */
	void clear();
	/* Clears the texture to the clear_color and sets it as the current rendering target. */
	void clear(const SDL_Color &clear_color);

	/* Scales the texture's alpha value based on the initial background_color's alpha value. 
	   alpha is a float between [0, 1]. Basically sets the textures opacity.*/
	void set_alpha_mod(const float alpha);

	void set_color_mod(Uint8 r, Uint8 g, Uint8 b);
	void set_color_mod(const SDL_Color &color_mod);

	void load_from_file(const char* path);

	void free();

	void render();
	void render(int x, int y);
	void render(const SDL_Rect &dest_rect);

	SDL_Texture* get_texture() { return texture; }
	const int &get_width() const { return width; }
	const int &get_height() const { return height; }
protected:
	void restore_texture_settings();

	SDL_Renderer* &renderer;
	SDL_Texture* texture;
	int width, height;
private:
	SDL_Color background_color;
	int access;
	Uint8 alpha_mod;
	SDL_Color color_mod;
};