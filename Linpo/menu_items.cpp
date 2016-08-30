#include "menu_items.h"
#include "globals.h"
#include "constants.h"
#include "render_functions.h"


AbstractMenuItem::AbstractMenuItem(const MENU_OPTION &option_type)
	:item_texture{std::make_unique<TextureWrapper>(main_renderer)},
	option_type{option_type}, 
	mouse_clicked(false), 
	mouse_hovered(false),
	item_rect{} 
{
}

void AbstractMenuItem::handle_event(SDL_Event & e)
{
	if (e.type == SDL_MOUSEBUTTONDOWN)
	{
		if (e.button.button == SDL_BUTTON_LEFT)
			mouse_clicked = true;
	}
}

void AbstractMenuItem::render(int x, int y)
{
	item_rect.x = x;
	item_rect.y = y;

	item_texture->render(x, y);
}

void AbstractMenuItem::render(const SDL_Rect & dest)
{
	item_rect.x = dest.x;
	item_rect.y = dest.y;

	item_texture->render(dest);
}

void AbstractMenuItem::resize(const int & w, const int & h)
{
	item_rect.w = w;
	item_rect.h = h;
	item_texture->resize(item_rect.w, item_rect.h);
}

bool AbstractMenuItem::is_clicked()
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

bool AbstractMenuItem::is_hovered() const
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	return is_hovered(x, y);
}

bool AbstractMenuItem::is_hovered(const int &x, const int &y) const
{
	SDL_Point mouse_pos = { x, y };
	if (SDL_PointInRect(&mouse_pos, &item_rect))
		return true;
	else
		return false;
}

MENU_OPTION AbstractMenuItem::get_option_type() const
{
	return option_type;
}

int AbstractMenuItem::get_width() const
{
	return item_rect.w;
}

int AbstractMenuItem::get_height() const
{
	return item_rect.h;
}

void AbstractMenuItem::handle_hover(int x, int y)
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


TextMenuItem::TextMenuItem(std::string name, const MENU_OPTION & option_type)
	:AbstractMenuItem::AbstractMenuItem(option_type), 
	text_texture(std::make_unique<TextTexture>(main_renderer, name, COLORS[BLACK]))
{
	resize(text_texture->get_width() + 10, text_texture->get_height() + 10);

	update_full_texture();
}

void TextMenuItem::update_full_texture()
{
	if (mouse_hovered)
		item_texture->clear({ 255, 0, 0, 100 });  // opaque red background
	else
		item_texture->clear({ 255, 0, 0, 50 });

	text_texture->set_render_pos({ 5, 5 });
	text_texture->render();

	// draw border
	SDL_SetRenderDrawColor(main_renderer, 255, 0, 0, 255);
	SDL_RenderDrawRect(main_renderer, NULL);

	item_texture->reset_render_target();
}


IncrementerMenuItem::IncrementerMenuItem(std::string name, MENU_OPTION option_type, int min, int max, int cur)
	:TextMenuItem::TextMenuItem(name, option_type), 
	min_val{ min }, max_val{ max }, cur_val(cur),
	value_text_tex(std::make_unique<TextTexture>(main_renderer, std::to_string(cur), COLORS[BLACK])),
	decrement_item(std::make_unique<TextMenuItem>("<", option_type)),
	increment_item(std::make_unique<TextMenuItem>(">", option_type))
{
	item_rect.w = text_texture->get_width() + decrement_item->get_width() + value_text_tex->get_width() + decrement_item->get_width() + 10;

	item_texture->resize(item_rect.w, item_rect.h);

	update_full_texture();  // initializes it
}

void IncrementerMenuItem::handle_event(SDL_Event & e)
{
	decrement_item->handle_event(e);
	increment_item->handle_event(e);
}

void IncrementerMenuItem::handle_hover(int x, int y)
{ 
	decrement_item->handle_hover(x, y);
	increment_item->handle_hover(x, y);
}

void IncrementerMenuItem::render(int x, int y)
{
	AbstractMenuItem::render(x, y);

	SDL_Rect dec_rect;
	dec_rect.w = decrement_item->get_width() * 0.9;
	dec_rect.h = decrement_item->get_height() * 0.9;
	dec_rect.x = x + 10 + text_texture->get_width() + ((decrement_item->get_width() - dec_rect.w) / 2);
	dec_rect.y = y + ((decrement_item->get_height() - dec_rect.h) / 2);
	decrement_item->render(dec_rect);

	dec_rect.x += decrement_item->get_width() + value_text_tex->get_width();
	increment_item->render(dec_rect);
}

bool IncrementerMenuItem::is_clicked()
{
	bool ret = false;
	if (decrement_item->is_clicked())
	{
		cur_val = cur_val <= min_val ? min_val : cur_val - 1;
		ret = true;
	}
	else if (increment_item->is_clicked())
	{
		cur_val = cur_val >= max_val ? max_val : cur_val + 1;
		ret = true;
	}

	if (ret) update_full_texture();

	return ret;
}

const int IncrementerMenuItem::get_cur_val() const
{
	return cur_val;
}

void IncrementerMenuItem::update_full_texture()
{
	item_texture->clear({ 255, 0, 0, 50 });

	text_texture->render(5, 5);

	value_text_tex->write_text(std::to_string(cur_val), COLORS[BLACK]);
	value_text_tex->render(5 + text_texture->get_width() + 5 + decrement_item->get_width(), 5);

	// draw border
	SDL_SetRenderDrawColor(main_renderer, 255, 0, 0, 255);
	SDL_RenderDrawRect(main_renderer, NULL);

	item_texture->reset_render_target();
}


PauseItem::PauseItem(const int w, const int h)
	:AbstractMenuItem::AbstractMenuItem(MENU_OPTION::PAUSE)
{
	// a 64 by 64 button by default
	resize(w, h);
	update_full_texture();
}

void PauseItem::update_full_texture()
{
	item_texture->clear({ 255, 0, 0, 50 });

	SDL_Rect rect;
	rect.x = get_width() * 0.04 + 1;  // + 1 because the border takes 1 pixel
	rect.y = get_height() * 0.04 + 1;
	rect.w = get_width() * 0.38;
	rect.h = get_height() * 0.92;
	
	if (mouse_hovered)
		render_rect(rect, { 255, 0, 0, 200 });
	else
		render_rect(rect, { 255, 0, 0, 100 });

	rect.x = rect.x + rect.w + (get_width() * 0.16);

	if (mouse_hovered)
		render_rect(rect, { 255, 0, 0, 200 });
	else
		render_rect(rect, { 255, 0, 0, 100 });

	// draw border
	SDL_SetRenderDrawColor(main_renderer, 255, 0, 0, 255);
	SDL_RenderDrawRect(main_renderer, NULL);

	item_texture->reset_render_target();
}

