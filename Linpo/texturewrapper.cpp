#include "texturewrapper.h"

TextureWrapper::TextureWrapper(SDL_Renderer* renderer, int access, int w, int h)
{
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, access, w, h);
	this->renderer = renderer;
	this->access = access;
}

TextureWrapper::~TextureWrapper()
{
	SDL_DestroyTexture(texture);
	texture = nullptr;
	renderer = nullptr;
}

void TextureWrapper::resize(int new_width, int new_height)
{
	if (texture != nullptr)
		SDL_DestroyTexture(texture);

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, access, new_width, new_height);
}

void TextureWrapper::set_as_render_target()
{
	SDL_SetRenderTarget(renderer, texture);
}

void TextureWrapper::reset_render_target()
{
	SDL_SetRenderTarget(renderer, NULL);
}

void TextureWrapper::render()
{
	SDL_RenderCopy(renderer, texture, NULL, NULL);
}

SDL_Texture* TextureWrapper::get_texture()
{
	return texture;
}
