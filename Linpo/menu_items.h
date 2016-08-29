#pragma once

#include <memory>
#include <string>
#include "SDL.h"
#include "text_texture.h"
#include "constants.h"


class AbstractMenuItem
{
public:
	AbstractMenuItem(const MENU_OPTION &option_type = MENU_OPTION::NULL_OPTION);

	virtual void handle_event(SDL_Event &e);
	virtual void handle_hover(int x, int y);
	virtual void render(int x, int y);
	void render(const SDL_Rect &dest);

	virtual bool is_clicked();
	bool is_hovered() const;
	bool is_hovered(const int &x, const int &y) const;

	MENU_OPTION get_option_type() const;

	int get_width() const;
	int get_height() const;
protected:
	virtual void update_full_texture() = 0;

	/*Resizes item_rect and item_texture to w, h.*/
	void resize(const int &w, const int &h);

	MENU_OPTION option_type;
	bool mouse_clicked;
	bool mouse_hovered;
	SDL_Rect item_rect;

	std::unique_ptr<TextureWrapper> item_texture;
};

class TextMenuItem : public AbstractMenuItem
{
public:
	TextMenuItem(std::string name, const MENU_OPTION &option_type = MENU_OPTION::NULL_OPTION);
protected:
	virtual void update_full_texture() override;

	std::unique_ptr<TextTexture> text_texture;
};

class IncrementerMenuItem : public TextMenuItem
{
public:
	IncrementerMenuItem(std::string name, MENU_OPTION option_type, int min = 2, int max = 4, int cur = 2);

	void handle_event(SDL_Event &e);
	void handle_hover(int x, int y);
	void render(int x, int y);

	bool is_clicked();

	const int get_cur_val() const;
protected:
	void update_full_texture() override;

	int min_val, max_val, cur_val;

	std::unique_ptr<TextTexture> value_text_tex;
	std::unique_ptr<TextMenuItem> decrement_item;
	std::unique_ptr<TextMenuItem> increment_item;
};

class BoolMenuItem : public TextMenuItem
{
public:
	BoolMenuItem(std::string name, MENU_OPTION option_type) : TextMenuItem::TextMenuItem(name, option_type) { }
};

class PauseItem : public AbstractMenuItem
{
public:
	PauseItem() {}
};