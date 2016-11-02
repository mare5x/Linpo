#include "texture_wrapper.h"

TextureWrapper::TextureWrapper(SDL_Renderer* &renderer, int w, int h, int access, SDL_Color background_color)
	:renderer(renderer),
	width(w), 
	height(h), 
	access(access), 
	background_color(background_color),
	alpha_mod(255),
	color_mod(background_color)
{
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, access, w, h);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);  // otherwise alpha is ignored!
}

TextureWrapper::TextureWrapper(SDL_Renderer *& renderer, const char * file_path, int access, SDL_Color background_color)
	:renderer(renderer),
	texture(nullptr),
	width(0),
	height(0),
	background_color(background_color),
	access(access),
	alpha_mod(255),
	color_mod(background_color)
{
	load_from_file(file_path);
}

TextureWrapper::TextureWrapper(SDL_Renderer *& renderer, int access, SDL_Color background_color) 
	:renderer(renderer),
	texture{nullptr},
	width(0),
	height(0),
	access(access), 
	background_color(background_color),
	alpha_mod(255),
	color_mod(background_color)
{ }

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
	restore_texture_settings();
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

void TextureWrapper::set_background_color(const SDL_Color & background_color)
{
	this->background_color = background_color;
}

void TextureWrapper::clear()
{
	set_as_render_target();
	SDL_SetRenderDrawColor(renderer, background_color.r, background_color.g, background_color.b, background_color.a);
	SDL_RenderClear(renderer);
}

void TextureWrapper::clear(const SDL_Color &clear_color)
{
	set_as_render_target();
	SDL_SetRenderDrawColor(renderer, clear_color.r, clear_color.g, clear_color.b, clear_color.a);
	SDL_RenderClear(renderer);
}

void TextureWrapper::set_alpha_mod(const float alpha)
{
	alpha_mod = alpha * background_color.a;
	SDL_SetTextureAlphaMod(texture, alpha_mod);
}

void TextureWrapper::set_color_mod(Uint8 r, Uint8 g, Uint8 b)
{
	color_mod = { r, g, b, 0xff };
	SDL_SetTextureColorMod(texture, r, g, b);
}

void TextureWrapper::set_color_mod(const SDL_Color & color_mod)
{
	this->color_mod = color_mod;
	SDL_SetTextureColorMod(texture, color_mod.r, color_mod.g, color_mod.b);
}

void TextureWrapper::load_from_file(const char * path)
{
	if (texture != nullptr)
		free();

	SDL_Surface* bmp_surface = SDL_LoadBMP(path);
	if (bmp_surface != NULL)
	{
		// Set black as the color key -> black backgrounds are transparent.
		SDL_SetColorKey(bmp_surface, 1, SDL_MapRGB(bmp_surface->format, 0, 0, 0));
		texture = SDL_CreateTextureFromSurface(renderer, bmp_surface);
		if (texture != NULL)
		{
			width = bmp_surface->w;
			height = bmp_surface->h;
		}

		SDL_FreeSurface(bmp_surface);
	}
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

void TextureWrapper::restore_texture_settings()
{
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);  // otherwise alpha is ignored!
	SDL_SetTextureAlphaMod(texture, alpha_mod);
	set_color_mod(color_mod);
}
