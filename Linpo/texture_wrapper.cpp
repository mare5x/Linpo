#include "texture_wrapper.h"

TextureWrapper::TextureWrapper(SDL_Renderer* &renderer, int w, int h, int access, SDL_Color base_color) : 
	renderer(renderer), width(w), height(h), access(access), base_color(base_color)
{
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, access, w, h);
}

TextureWrapper::TextureWrapper(SDL_Renderer *& renderer) : 
	renderer(renderer)
{
	texture = nullptr;
	width = 0;
	height = 0;
	// private variables are ignored
}

TextureWrapper::~TextureWrapper()
{
	free();
}

void TextureWrapper::resize(int new_width, int new_height)
{
	if (new_width == width && new_height == height)
		return;

	if (texture != nullptr)
		SDL_DestroyTexture(texture);

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, access, new_width, new_height);
	width = new_width;
	height = new_height;
}

void TextureWrapper::set_as_render_target()
{
	SDL_SetRenderTarget(renderer, texture);
}

void TextureWrapper::reset_render_target()
{
	SDL_SetRenderTarget(renderer, NULL);
}

void TextureWrapper::clear()
{
	set_as_render_target();
	SDL_SetRenderDrawColor(renderer, base_color.r, base_color.g, base_color.b, base_color.a);
	SDL_RenderClear(renderer);
}

void TextureWrapper::clear(const SDL_Color &clear_color)
{
	set_as_render_target();
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);  // otherwise alpha is ignored!
	SDL_SetRenderDrawColor(renderer, clear_color.r, clear_color.g, clear_color.b, clear_color.a);
	SDL_RenderClear(renderer);
}

void TextureWrapper::free()
{
	SDL_DestroyTexture(texture);
	texture = nullptr;
	width = 0;
	height = 0;
}

void TextureWrapper::render()
{
	SDL_RenderCopy(renderer, texture, NULL, NULL);
}

void TextureWrapper::render(int x, int y)
{
	SDL_Rect dest_rect;
	dest_rect.x = x;
	dest_rect.y = y;
	dest_rect.w = width;
	dest_rect.h = height;
	SDL_RenderCopy(renderer, texture, NULL, &dest_rect);
}

void TextureWrapper::render(const SDL_Rect &dest_rect)
{
	SDL_RenderCopy(renderer, texture, NULL, &dest_rect);
}
