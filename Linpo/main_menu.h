#pragma once

#include "SDL.h"
#include <array>
#include "constants.h"
#include "menu_items.h"


class MainMenu
{
public:
	MainMenu();

	void handle_event(SDL_Event &e);
	void render();

	MENU_OPTION get_selected_option();

	void toggle_visibility();
	bool is_visible();
private:
	void resize_update();

	bool visible;
	int width, height;

	std::array<std::unique_ptr<MenuItem>, (int)MENU_OPTION::_N_OPTIONS - 1> menu_items;  // -1 because of NULL_OPTION
};