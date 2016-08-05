#include "SDL_ttf.h"
#include "globals.h"
#include "text_texture.h"

void TextTexture::write_text(const std::string & s, const SDL_Color & color)
{
	free();

	SDL_Surface* text_surface = TTF_RenderText_Solid(global_font, s.c_str(), color);
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
	}
}

void TextTexture::get_text_size(int &w, int &h)
{
	TTF_SizeText(global_font, text.c_str(), &w, &h);
}

void TextTexture::set_render_pos(const SDL_Point &top_left)
{
	top_left_pos = top_left;
}

void TextTexture::render()
{
	TextureWrapper::render(top_left_pos.x, top_left_pos.y);
}
