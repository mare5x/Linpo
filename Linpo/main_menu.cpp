#include "main_menu.h"
#include "constants.h"
#include "globals.h"


MainMenu::MainMenu()
{
	visible = false;

	menu_items[(int)MENU_OPTION::RESTART_GAME] = std::make_unique<MenuItem>("Restart game", MENU_OPTION::RESTART_GAME);
	menu_items[(int)MENU_OPTION::N_PLAYER_CHANGE] = std::make_unique<IncrementerMenuItem>("Number of players:", MENU_OPTION::N_PLAYER_CHANGE);
	menu_items[(int)MENU_OPTION::AI_TOGGLE] = std::make_unique<BoolMenuItem>("AI player", MENU_OPTION::AI_TOGGLE);
	// TODO: font size selector

	resize_update();
}

void MainMenu::handle_event(SDL_Event & e)
{
	if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
	{
		resize_update();
	}
	else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
	{
		visible = visible ? false : true;
	}
	
	if (is_visible())
	{
		for (auto &menu_item : menu_items)
			menu_item->handle_event(e);
	}
}

void MainMenu::render()
{
	// White background
	SDL_SetRenderDrawColor(main_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(main_renderer);

	// center all menu_items
	int padding_y = height / menu_items.size() / 2;
	for (int i = 0; i < menu_items.size(); ++i)
	{
		menu_items[i]->render((width / 2) - (menu_items[i]->get_width() / 2), i * (height / menu_items.size()) + padding_y - (menu_items[i]->get_height() / 2));
	}
}

MENU_OPTION MainMenu::get_selected_option()
{
	for (auto &menu_item : menu_items)
	{
		if (menu_item->is_clicked())
			return menu_item->get_option_type();
	}
	return MENU_OPTION::NULL_OPTION;
}

bool MainMenu::is_visible()
{
	return visible;
}

void MainMenu::resize_update()
{
	SDL_GetRendererOutputSize(main_renderer, &this->width, &this->height);
}
