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
	void render(int x, int y);

	bool is_clicked();

	MENU_OPTION get_option_type() const;

	int get_width() const;
	int get_height() const;
protected:
	MENU_OPTION option_type;
	bool mouse_clicked;
	SDL_Rect item_rect;

	std::unique_ptr<TextTexture> texture;
};

class IncrementerMenuItem : public MenuItem
{
public:
	IncrementerMenuItem(std::string name, MENU_OPTION option_type) : MenuItem::MenuItem(name, option_type) { }
};

class BoolMenuItem : public MenuItem
{
public:
	BoolMenuItem(std::string name, MENU_OPTION option_type) : MenuItem::MenuItem(name, option_type) { }
};