#pragma once

#include <memory>
#include <string>
#include "SDL.h"
#include "text_texture.h"
#include "constants.h"


class MenuItem
{
public:
	MenuItem(std::string name, MENU_OPTION option_type);

	void handle_event(SDL_Event &e);
	void handle_hover(int x, int y);
	void render(int x, int y);

	bool is_clicked();
	bool is_hovered();
	bool is_hovered(const int &x, const int &y);

	MENU_OPTION get_option_type() const;

	virtual int get_width() const;
	virtual int get_height() const;
protected:
	virtual void update_full_texture();

	MENU_OPTION option_type;
	bool mouse_clicked;
	bool mouse_hovered;
	SDL_Rect item_rect;

	std::unique_ptr<TextTexture> text_texture;
	std::unique_ptr<TextureWrapper> item_texture;
};

class IncrementerMenuItem : public MenuItem
{
public:
	IncrementerMenuItem(std::string name, MENU_OPTION option_type, int min = 0, int max = 4);

	void handle_event(SDL_Event &e);
	void render(int x, int y);

	int get_width() const;
	int get_height() const;
protected:
	int min_val, max_val;

	std::unique_ptr<TextureWrapper> full_texture;
};

class BoolMenuItem : public MenuItem
{
public:
	BoolMenuItem(std::string name, MENU_OPTION option_type) : MenuItem::MenuItem(name, option_type) { }
};