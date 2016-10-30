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
	virtual bool was_clicked();

	MENU_OPTION get_option_type() const;

	int get_texture_width() const;
	int get_texture_height() const;

	int get_width() const;
	int get_height() const;
protected:
	virtual void handle_hover();
	virtual void handle_item_click() {}
	virtual void update_full_texture() = 0;

	/*Resizes item_rect and item_texture to w, h.*/
	void resize(const int &w, const int &h);

	SDL_Rect item_rect;
	std::unique_ptr<TextureWrapper> item_texture;
private:
	void handle_mouse_click();
	bool is_item_hovered() const;

	MENU_OPTION option_type;
	MouseState mouse_state;
	bool item_hovered;
	bool item_clicked;
};

class TextMenuItem : public AbstractMenuItem
{
public:
	TextMenuItem(const std::string& name, const MENU_OPTION &option_type = MENU_OPTION::NULL_OPTION);
protected:
	virtual void update_full_texture() override;

	std::unique_ptr<TextTexture> text_texture;
};

class IncrementerMenuItem : public TextMenuItem
{
public:
	IncrementerMenuItem(const std::string& name, const MENU_OPTION &option_type, int min = MIN_PLAYERS, int max = MAX_PLAYERS, int cur = N_PLAYERS);

	void handle_event(SDL_Event &e) override;
	void render(int x, int y) override;

	bool was_clicked() override;

	const int get_cur_val() const;
protected:
	void handle_hover() override;
	void update_full_texture() override;

	virtual void adjust_item_size();

	int min_val, max_val, cur_val;

	std::unique_ptr<TextTexture> value_text_tex;
	std::unique_ptr<TextMenuItem> decrement_item;
	std::unique_ptr<TextMenuItem> increment_item;
};

class GridSizeMenuItem : public IncrementerMenuItem
{
public:
	GridSizeMenuItem(const std::string &name, const MENU_OPTION &option_type, int min = 3, int max = 20, int cur = DEFAULT_GRID_SIZE);
protected:
	void update_full_texture() override;
	void adjust_item_size() override;
private:
	void update_value_text();
};

class BoolMenuItem : public TextMenuItem
{
public:
	BoolMenuItem(const std::string &name, const MENU_OPTION &option_type);

	bool get_cur_val() const;
protected:
	void handle_item_click() override;
private:
	void update_full_texture() override;

	bool bool_val;

	std::unique_ptr<TextTexture> bool_text;
};

class ThemeMenuItem : public TextMenuItem
{
public:
	ThemeMenuItem(const std::string &name, const MENU_OPTION &option_type);

	COLOR_THEME get_cur_val() const { return color_theme; }
private:
	void handle_item_click() override;
	void update_full_texture() override;

	COLOR_THEME color_theme;
	std::unique_ptr<TextTexture> theme_text;
};

class PauseItem : public AbstractMenuItem
{
public:
	PauseItem(const int w = 64, const int h = 64);
protected:
	void update_full_texture() override;
};