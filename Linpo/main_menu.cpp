#include "main_menu.h"
#include "constants.h"
#include "globals.h"


MainMenu::MainMenu()
	:visible(false),
	width(SCREEN_WIDTH),
	height(SCREEN_HEIGHT)
{
	menu_items[(int)MENU_OPTION::RESTART_GAME] = std::make_unique<TextMenuItem>("Restart game", MENU_OPTION::RESTART_GAME);
	menu_items[(int)MENU_OPTION::N_PLAYER_CHANGE] = std::make_unique<IncrementerMenuItem>("Number of players:", MENU_OPTION::N_PLAYER_CHANGE);
	menu_items[(int)MENU_OPTION::GRID_SIZE] = std::make_unique<GridSizeMenuItem>("Grid size: ", MENU_OPTION::GRID_SIZE);
	menu_items[(int)MENU_OPTION::AI_TOGGLE] = std::make_unique<BoolMenuItem>("AI player: ", MENU_OPTION::AI_TOGGLE);
	menu_items[(int)MENU_OPTION::COLOR_THEME] = std::make_unique<ThemeMenuItem>("Color theme: ", MENU_OPTION::COLOR_THEME);
	menu_items[(int)MENU_OPTION::PAUSE] = std::make_unique<PauseItem>();

	set_color_theme(GLOBAL_COLOR_THEME);

	resize_update();
}

void MainMenu::handle_event(SDL_Event & e)
{
	if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
	{
		resize_update();
	}
	
	if (is_visible())
	{
		for (auto &menu_item : menu_items)
			menu_item->handle_event(e);
	}
}

void MainMenu::render()
{
	// center all menu_items
	int padding_y = height / menu_items.size() / 2;
	for (size_t i = 0; i < menu_items.size(); ++i)
	{
		menu_items[i]->render((width / 2) - (menu_items[i]->get_width() / 2), i * (height / menu_items.size()) + padding_y - (menu_items[i]->get_height() / 2));
	}
}

MENU_OPTION MainMenu::get_selected_option()
{
	for (auto &menu_item : menu_items)
	{
		if (menu_item->was_clicked())
			return menu_item->get_option_type();
	}
	return MENU_OPTION::NULL_OPTION;
}

const AbstractMenuItem & MainMenu::get_option_item(const MENU_OPTION & option) const
{
	return *menu_items[static_cast<int>(option)];
}

void MainMenu::set_color_theme(const COLOR_THEME & color_theme) const
{
	auto font_color = get_font_color(color_theme);
	for (const auto &menu_item : menu_items)
		menu_item->set_font_color(font_color);
}

void MainMenu::toggle_visibility()
{
	visible = visible ? false : true;
}

bool MainMenu::is_visible()
{
	return visible;
}

void MainMenu::resize_update()
{
	SDL_GetRendererOutputSize(main_renderer, &this->width, &this->height);
}
