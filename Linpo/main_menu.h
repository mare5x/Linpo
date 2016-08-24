#pragma once

#include "SDL.h"
#include <vector>
#include "menu_items.h"


class MainMenu
{
public:
	MainMenu();

	void handle_event(SDL_Event &e);
	void render();

	bool is_visible();
private:
	void resize_update();

	bool visible;
	int width, height;

	std::vector<std::unique_ptr<MenuItem>> menu_items;
};