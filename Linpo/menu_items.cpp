#include "menu_items.h"
#include "globals.h"
#include "constants.h"


MenuItem::MenuItem(std::string name, MENU_OPTION option_type)
	:texture {std::make_unique<TextTexture>(main_renderer)},
	option_type{option_type}
{
	texture->write_text(name, COLORS[BLACK]);

	item_rect.x = 0;
	item_rect.y = 0;
	item_rect.w = texture->get_width();
	item_rect.h = texture->get_height();

	mouse_clicked = false;
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
	texture->render(x, y);
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
