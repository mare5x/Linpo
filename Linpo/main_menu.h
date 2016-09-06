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
	const AbstractMenuItem& get_option_item(const MENU_OPTION &option) const;

	void toggle_visibility();
	bool is_visible();
private:
	void resize_update();

	bool visible;
	int width, height;

	std::array<std::unique_ptr<AbstractMenuItem>, (int)MENU_OPTION::_N_MENU_OPTIONS> menu_items;
};