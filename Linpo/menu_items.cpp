#include "menu_items.h"
#include "globals.h"
#include "constants.h"


MenuItem::MenuItem(std::string name, MENU_OPTION option_type)
	:text_texture {std::make_unique<TextTexture>(main_renderer)},
	item_texture{std::make_unique<TextureWrapper>(main_renderer)},
	option_type{option_type}
{
	text_texture->write_text(name, COLORS[BLACK]);

	item_rect.x = 0;
	item_rect.y = 0;
	item_rect.w = text_texture->get_width() + 10;  // add rect padding width and height
	item_rect.h = text_texture->get_height() + 10;

	item_texture->resize(item_rect.w, item_rect.h);

	update_full_texture();  // initializes it

	mouse_clicked = false;
	mouse_hovered = false;
}

void MenuItem::handle_event(SDL_Event & e)
{
	if (e.type == SDL_MOUSEBUTTONDOWN)
	{
		if (e.button.button == SDL_BUTTON_LEFT)
			mouse_clicked = true;
	}
}

void MenuItem::render(int x, int y)
{
	item_rect.x = x;
	item_rect.y = y;

	item_texture->render(x, y);
}

bool MenuItem::is_clicked()
{
	if (mouse_clicked)
	{
		mouse_clicked = false;

		SDL_Point mouse_pos;
		SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
		if (SDL_PointInRect(&mouse_pos, &item_rect))
			return true;
	}
	return false;
}

bool MenuItem::is_hovered()
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	return is_hovered(x, y);
}

bool MenuItem::is_hovered(const int &x, const int &y)
{
	SDL_Point mouse_pos = { x, y };
	if (SDL_PointInRect(&mouse_pos, &item_rect))
		return true;
	else
		return false;
}

MENU_OPTION MenuItem::get_option_type() const
{
	return option_type;
}

int MenuItem::get_width() const
{
	return item_rect.w;
}

int MenuItem::get_height() const
{
	return item_rect.h;
}

void MenuItem::handle_hover(int x, int y)
{
	// only update textures if the hover state just changed
	if (is_hovered(x, y))
	{
		if (!mouse_hovered)
		{
			mouse_hovered = true;
			update_full_texture();
		}
	}
	else
	{
		if (mouse_hovered)
		{
			mouse_hovered = false;
			update_full_texture();
		}
	}
}

void MenuItem::update_full_texture()
{
	if (mouse_hovered)
		item_texture->clear( { 255, 0, 0, 100 } );  // opaque red background
	else
		item_texture->clear( { 255, 0, 0, 50 } );

	text_texture->set_render_pos({ 5, 5 });
	text_texture->render();

	// draw border
	SDL_SetRenderDrawColor(main_renderer, 255, 0, 0, 255);
	SDL_RenderDrawRect(main_renderer, NULL);

	item_texture->reset_render_target();
}
