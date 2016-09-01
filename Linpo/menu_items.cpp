#include <string>
#include "menu_items.h"
#include "globals.h"
#include "constants.h"
#include "render_functions.h"
#include "android-patch.h"


AbstractMenuItem::AbstractMenuItem(const MENU_OPTION &option_type)
	:item_rect{},
	item_texture{std::make_unique<TextureWrapper>(main_renderer)},
	option_type{option_type},
	mouse_state{},
	item_hovered(false)
{ }

void AbstractMenuItem::handle_event(SDL_Event & e)
{
	switch (e.type)
	{
	case SDL_MOUSEBUTTONDOWN:
		if (e.button.button == SDL_BUTTON_LEFT)
			mouse_state.clicked = true;
		break;
	case SDL_MOUSEMOTION:
		mouse_state.pos = { e.motion.x, e.motion.y };
		handle_hover();
		break;
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
	item_rect = dest;

	item_texture->render(dest);
}

void AbstractMenuItem::resize(const int & w, const int & h)
{
	item_rect.w = w;
	item_rect.h = h;
	item_texture->resize(item_rect.w, item_rect.h);
}

bool AbstractMenuItem::is_hovered() const
{
	return item_hovered;
}

bool AbstractMenuItem::is_clicked()
{
	if (mouse_state.clicked)
	{
		mouse_state.clicked = false;

		if (SDL_PointInRect(&mouse_state.pos, &item_rect))
			return true;
	}
	return false;
}

bool AbstractMenuItem::is_item_hovered() const
{
	if (SDL_PointInRect(&mouse_state.pos, &item_rect))
		return true;
	else
		return false;
}

MENU_OPTION AbstractMenuItem::get_option_type() const
{
	return option_type;
}

int AbstractMenuItem::get_texture_width() const
{
	return item_texture->get_width();
}

int AbstractMenuItem::get_texture_height() const
{
	return item_texture->get_height();
}

int AbstractMenuItem::get_width() const
{
	return item_rect.w;
}

int AbstractMenuItem::get_height() const
{
	return item_rect.h;
}

void AbstractMenuItem::handle_hover()
{
	// only update textures if the hover state just changed
	if (is_item_hovered())
	{
		if (!item_hovered)
		{
			item_hovered = true;
			update_full_texture();
		}
	}
	else
	{
		if (item_hovered)
		{
			item_hovered = false;
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
	if (is_hovered())
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
	value_text_tex(std::make_unique<TextTexture>(main_renderer, patch::to_string(cur), COLORS[BLACK])),
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

void IncrementerMenuItem::handle_hover() { }

void IncrementerMenuItem::render(int x, int y)
{
	AbstractMenuItem::render(x, y);

	SDL_Rect dec_rect;
	dec_rect.w = decrement_item->get_texture_width() * 0.9;
	dec_rect.h = decrement_item->get_texture_height() * 0.9;
	dec_rect.x = x + 10 + text_texture->get_width() + ((decrement_item->get_texture_width() - dec_rect.w) / 2);
	dec_rect.y = y + ((decrement_item->get_texture_height() - dec_rect.h) / 2);
	decrement_item->render(dec_rect);

	dec_rect.x += decrement_item->get_texture_width() + value_text_tex->get_width();
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

	value_text_tex->write_text(patch::to_string(cur_val), COLORS[BLACK]);
	value_text_tex->render(5 + text_texture->get_width() + 5 + decrement_item->get_texture_width(), 5);

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
	rect.x = get_texture_width() * 0.04 + 1;  // + 1 because the border takes 1 pixel
	rect.y = get_texture_height() * 0.04 + 1;
	rect.w = get_texture_width() * 0.38;
	rect.h = get_texture_height() * 0.92;
	
	if (is_hovered())
		render_rect(rect, { 255, 0, 0, 200 });
	else
		render_rect(rect, { 255, 0, 0, 100 });

	rect.x = rect.x + rect.w + (get_texture_width() * 0.16);

	if (is_hovered())
		render_rect(rect, { 255, 0, 0, 200 });
	else
		render_rect(rect, { 255, 0, 0, 100 });

	// draw border
	SDL_SetRenderDrawColor(main_renderer, 255, 0, 0, 255);
	SDL_RenderDrawRect(main_renderer, NULL);

	item_texture->reset_render_target();
}

