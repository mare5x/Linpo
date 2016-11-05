#include <string>
#include "menu_items.h"
#include "globals.h"
#include "constants.h"
#include "render_functions.h"
#include "player_array.h"
#include "android-patch.h"
#include <sstream>


AbstractMenuItem::AbstractMenuItem(const MENU_OPTION &option_type)
	:item_rect{},
	item_texture{std::make_unique<TextureWrapper>(main_renderer)},
	option_type{option_type},
	mouse_state{},
	item_hovered(false),
	item_clicked(false)
{ }

void AbstractMenuItem::handle_event(const SDL_Event & e)
{
	switch (e.type)
	{
	case SDL_MOUSEBUTTONDOWN:
		if (e.button.button == SDL_BUTTON_LEFT)
			if (is_item_hovered())
				mouse_state.pressed = true;
		break;
	case SDL_MOUSEBUTTONUP:
		if (e.button.button == SDL_BUTTON_LEFT)
		{
			if (mouse_state.pressed)
				handle_mouse_click();

			mouse_state.pressed = false;

			if (item_clicked)
				handle_item_click();
		}
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

bool AbstractMenuItem::was_clicked()
{
	bool ret = item_clicked;
	item_clicked = false;
	return ret;
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

void AbstractMenuItem::handle_mouse_click()
{
	if (SDL_PointInRect(&mouse_state.pos, &item_rect))
		item_clicked = true;
	else
		item_clicked = false;
}


TextMenuItem::TextMenuItem(const std::string &name, const MENU_OPTION &option_type)
	:AbstractMenuItem::AbstractMenuItem(option_type), 
	base_font_color(COLORS[WHITE]),
	text_texture(std::make_unique<TextTexture>(main_renderer, name, base_font_color))
{
	resize(text_texture->get_width() + 10, text_texture->get_height() + 10);

	update_full_texture();
}

void TextMenuItem::set_font_color(const SDL_Color & color)
{
	text_texture->set_color_mod(color);

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


IncrementerMenuItem::IncrementerMenuItem(const std::string &name, const MENU_OPTION &option_type, int min, int max, int cur)
	:TextMenuItem::TextMenuItem(name, option_type), 
	min_val{ min }, max_val{ max }, cur_val(cur),
	value_text_tex(std::make_unique<TextTexture>(main_renderer, patch::to_string(cur), base_font_color)),
	decrement_item(std::make_unique<TextMenuItem>("<", option_type)),
	increment_item(std::make_unique<TextMenuItem>(">", option_type))
{
	adjust_item_size();
	update_full_texture();  // initializes it
}

void IncrementerMenuItem::handle_event(const SDL_Event & e)
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

	dec_rect.x = x + get_width() - increment_item->get_width() - 2;
	increment_item->render(dec_rect);
}

bool IncrementerMenuItem::was_clicked()
{
	bool ret = false;
	if (decrement_item->was_clicked())
	{
		cur_val = cur_val <= min_val ? min_val : cur_val - 1;
		ret = true;
	}
	else if (increment_item->was_clicked())
	{
		cur_val = cur_val >= max_val ? max_val : cur_val + 1;
		ret = true;
	}

	if (ret)
	{
		update_full_texture();
	}

	return ret;
}

void IncrementerMenuItem::set_font_color(const SDL_Color & color)
{
	TextMenuItem::set_font_color(color);
	value_text_tex->set_color_mod(color);
	decrement_item->set_font_color(color);
	increment_item->set_font_color(color);

	update_full_texture();
}

const int IncrementerMenuItem::get_cur_val() const
{
	return cur_val;
}

void IncrementerMenuItem::set_cur_val(int val)
{
	cur_val = val;
	update_full_texture();
}

void IncrementerMenuItem::update_full_texture()
{
	item_texture->clear({ 255, 0, 0, 50 });

	text_texture->render(5, 5);

	value_text_tex->write_text(patch::to_string(cur_val), base_font_color);
	value_text_tex->render(5 + text_texture->get_width() + 5 + decrement_item->get_texture_width(), 5);

	// draw border
	SDL_SetRenderDrawColor(main_renderer, 255, 0, 0, 255);
	SDL_RenderDrawRect(main_renderer, NULL);

	item_texture->reset_render_target();
}

void IncrementerMenuItem::adjust_item_size()
{
	int value_tex_size;
	calculate_text_size(patch::to_string(max_val).c_str(), &value_tex_size, NULL);
	item_rect.w = text_texture->get_width() + decrement_item->get_width() + value_tex_size + increment_item->get_width() + 10;

	item_texture->resize(item_rect.w, item_rect.h);
}


GridSizeMenuItem::GridSizeMenuItem(const std::string & name, const MENU_OPTION & option_type, int min, int max, int cur)
	:IncrementerMenuItem::IncrementerMenuItem(name, option_type, min, max, cur)
{
	update_value_text();
	adjust_item_size();
	update_full_texture();  // initializes it
}

void GridSizeMenuItem::update_full_texture()
{
	item_texture->clear({ 255, 0, 0, 50 });

	text_texture->render(5, 5);

	update_value_text();
	int left = 5 + text_texture->get_width() + 5 + decrement_item->get_texture_width();
	int right = get_width() - 2 - increment_item->get_texture_width();
	value_text_tex->render((left + right - value_text_tex->get_width()) / 2, 5);

	// draw border
	SDL_SetRenderDrawColor(main_renderer, 255, 0, 0, 255);
	SDL_RenderDrawRect(main_renderer, NULL);

	item_texture->reset_render_target();
}

void GridSizeMenuItem::adjust_item_size()
{
	std::ostringstream stream;
	stream << max_val << " X " << max_val;

	int value_tex_size;
	calculate_text_size(stream.str().c_str(), &value_tex_size, NULL);
	item_rect.w = text_texture->get_width() + decrement_item->get_width() + value_tex_size + increment_item->get_width() + 10;

	item_texture->resize(item_rect.w, item_rect.h);
}

void GridSizeMenuItem::update_value_text()
{
	std::ostringstream stream;
	stream << cur_val << " X " << cur_val;
	value_text_tex->write_text(stream.str(), base_font_color);
}


BoolMenuItem::BoolMenuItem(const std::string &name, const MENU_OPTION &option_type)
	:TextMenuItem::TextMenuItem(name, option_type),
	bool_val(AI_ENABLED),
	bool_text(std::make_unique<TextTexture>(main_renderer, AI_ENABLED ? "enabled" : "disabled", base_font_color))
{
	resize(text_texture->get_width() + bool_text->get_width() + 10, bool_text->get_height() + 10);
	update_full_texture();
}

void BoolMenuItem::set_font_color(const SDL_Color & color)
{
	TextMenuItem::set_font_color(color);
	bool_text->set_color_mod(color);

	update_full_texture();
}

bool BoolMenuItem::get_cur_val() const
{
	return bool_val;
}

void BoolMenuItem::set_cur_val(bool val)
{
	bool_val = val;
	update_full_texture();
}

void BoolMenuItem::handle_item_click()
{
	bool_val = !bool_val;
	update_full_texture();
}

void BoolMenuItem::update_full_texture()
{
	TextMenuItem::update_full_texture();

	if (bool_val)
		bool_text->write_text("enabled", base_font_color);
	else
		bool_text->write_text("disabled", base_font_color);

	item_texture->set_as_render_target();

	bool_text->render(text_texture->get_width() + 5, 5);

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

ThemeMenuItem::ThemeMenuItem(const std::string & name, const MENU_OPTION & option_type)
	:TextMenuItem::TextMenuItem(name, option_type),
	color_theme{COLOR_THEME::DEFAULT},
	theme_text{std::make_unique<TextTexture>(main_renderer, "default", base_font_color)}
{
	resize(text_texture->get_width() + theme_text->get_width() + 10, theme_text->get_height() + 10);
	update_full_texture();
}

void ThemeMenuItem::set_font_color(const SDL_Color & color)
{
	TextMenuItem::set_font_color(color);
	theme_text->set_color_mod(color);

	update_full_texture();
}

void ThemeMenuItem::set_cur_val(const COLOR_THEME & theme)
{
	color_theme = theme;
	update_full_texture();
}

void ThemeMenuItem::handle_item_click()
{
	color_theme = static_cast<COLOR_THEME>((static_cast<int>(color_theme) + 1) % static_cast<int>(COLOR_THEME::N_THEMES));
	update_full_texture();
}

void ThemeMenuItem::update_full_texture()
{
	TextMenuItem::update_full_texture();

	switch (color_theme)
	{
	case COLOR_THEME::DEFAULT:
		theme_text->write_text("default", base_font_color);
		break;
	case COLOR_THEME::BLACK:
		theme_text->write_text("black", base_font_color);
		break;
	};

	item_texture->set_as_render_target();

	theme_text->render(text_texture->get_width() + 5, 5);

	item_texture->reset_render_target();
}

GameOverItem::GameOverItem(const Player & winner, const SDL_Color & theme_font_color)
	:TextMenuItem::TextMenuItem("The winner is: "),
	current_theme(COLOR_THEME::DEFAULT),
	winner_text(std::make_unique<TextTexture>(main_renderer))
{
	std::stringstream message;
	message << "Player " << winner.id + 1;
	winner_text->write_text(message.str(), *winner.color);

	set_font_color(theme_font_color);

	resize(text_texture->get_width() + winner_text->get_width() + 10, winner_text->get_height() + 10);
	update_full_texture();
}

void GameOverItem::apply_theme(const COLOR_THEME & color_theme)
{
	if (static_cast<int>(current_theme) == static_cast<int>(color_theme))
		return;

	current_theme = color_theme;

	set_font_color(get_font_color(color_theme));
}

void GameOverItem::set_font_color(const SDL_Color & color)
{
	TextMenuItem::set_font_color(color);
}

void GameOverItem::update_full_texture()
{
	TextMenuItem::update_full_texture();

	item_texture->set_as_render_target();

	winner_text->render(text_texture->get_width() + 5, 5);

	item_texture->reset_render_target();
}
