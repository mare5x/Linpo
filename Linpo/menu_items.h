#pragma once

#include <memory>
#include <string>
#include "SDL.h"
#include "text_texture.h"
#include "constants.h"
#include "mouse_state.h"


class AbstractMenuItem
{
public:
	AbstractMenuItem(const MENU_OPTION &option_type = MENU_OPTION::NULL_OPTION);

	virtual void handle_event(SDL_Event &e);
	virtual void render(int x, int y);
	void render(const SDL_Rect &dest);

	bool is_hovered() const;
	virtual bool is_clicked();

	MENU_OPTION get_option_type() const;

	int get_width() const;
	int get_height() const;
protected:
	virtual void handle_hover();
	virtual void update_full_texture() = 0;

	/*Resizes item_rect and item_texture to w, h.*/
	void resize(const int &w, const int &h);

	SDL_Rect item_rect;
	std::unique_ptr<TextureWrapper> item_texture;
private:
	bool is_item_hovered() const;

	MENU_OPTION option_type;
	MouseState mouse_state;
	bool item_hovered;
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

	void handle_event(SDL_Event &e) override;
	void render(int x, int y) override;

	bool is_clicked() override;

	const int get_cur_val() const;
protected:
	void handle_hover() override;
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
	PauseItem(const int w = 64, const int h = 64);
protected:
	void update_full_texture() override;
};