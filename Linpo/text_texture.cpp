#include "globals.h"
#include "constants.h"
#include "text_texture.h"

TextTexture::TextTexture(SDL_Renderer* &renderer)
	:TextureWrapper(renderer)
{
	init();
}

TextTexture::TextTexture(SDL_Renderer *& renderer, const std::string & s, const SDL_Color & color)
	:TextureWrapper(renderer), text(s)
{
	init();
	write_text(s, color);
}

TextTexture::~TextTexture()
{
	if (text_font != nullptr && text_font != global_font)
		TTF_CloseFont(text_font);
}

void TextTexture::write_text(const std::string &s, const SDL_Color &color)
{
	free();

	SDL_Surface* text_surface = TTF_RenderText_Blended(text_font, s.c_str(), color);
	if (text_surface != NULL)
	{
		texture = SDL_CreateTextureFromSurface(renderer, text_surface);
		if (texture != NULL)
		{
			width = text_surface->w;
			height = text_surface->h;
		}

		SDL_FreeSurface(text_surface);

		text = s;

		restore_texture_settings();
	}
}

void TextTexture::get_text_size(int &w, int &h)
{
	TTF_SizeText(text_font, text.c_str(), &w, &h);
}

void TextTexture::set_font_size(const int font_size)
{
	if (this->font_size != font_size)
	{
		if (text_font != nullptr && text_font != global_font)
			TTF_CloseFont(text_font);
		text_font = TTF_OpenFont("resources/OpenSans-Regular.ttf", font_size);
		this->font_size = font_size;
	}
}

void TextTexture::set_render_pos(const SDL_Point &top_left)
{
	top_left_pos = top_left;
}

SDL_Rect TextTexture::get_render_pos() const
{
	SDL_Rect rect;
	rect.x = top_left_pos.x;
	rect.y = top_left_pos.y;
	rect.w = get_width();
	rect.h = get_height();
	return rect;
}

void TextTexture::render()
{
	TextureWrapper::render(top_left_pos.x, top_left_pos.y);
}

void TextTexture::init()
{
	text_font = global_font;
	font_size = FONT_SIZE;  // calculate_font_size();
	top_left_pos = { 0, 0 };
}


int calculate_font_size()
{
	float ddpi, hdpi, vdpi;
	
	if (SDL_GetDisplayDPI(SDL_GetWindowDisplayIndex(main_window), &ddpi, &hdpi, &vdpi) < 0)
	{
		SDL_Log("DPI Error: %s", SDL_GetError());
		int height, width;
		SDL_GetRendererOutputSize(main_renderer, &width, &height);
		return BASE_FONT_SIZE * ((float)(height / SCREEN_HEIGHT) + (float)(width / SCREEN_WIDTH) / 2);
	}

	SDL_Log("DPI: %f %f %f", ddpi, hdpi, vdpi);
	float dpi_scale_factor = vdpi / TTF_DPI;
	return dpi_scale_factor * BASE_FONT_SIZE;
}

void calculate_text_size(const char * text_str, int* w, int* h)
{
	TTF_SizeText(global_font, text_str, w, h);
}
