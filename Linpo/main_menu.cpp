#include "main_menu.h"
#include "constants.h"
#include "globals.h"


MainMenu::MainMenu()
	:visible(false),
	width(SCREEN_WIDTH),
	height(SCREEN_HEIGHT),
	pref_file("preferences.dat")
{
	menu_items[(size_t)MENU_OPTION::RESTART_GAME] = std::make_unique<TextMenuItem>("Restart game", MENU_OPTION::RESTART_GAME);
	menu_items[(size_t)MENU_OPTION::N_PLAYER_CHANGE] = std::make_unique<IncrementerMenuItem>("Number of players:", MENU_OPTION::N_PLAYER_CHANGE);
	menu_items[(size_t)MENU_OPTION::GRID_SIZE] = std::make_unique<GridSizeMenuItem>("Grid size: ", MENU_OPTION::GRID_SIZE);
	menu_items[(size_t)MENU_OPTION::AI_TOGGLE] = std::make_unique<BoolMenuItem>("AI player: ", MENU_OPTION::AI_TOGGLE);
	menu_items[(size_t)MENU_OPTION::COLOR_THEME] = std::make_unique<ThemeMenuItem>("Color theme: ", MENU_OPTION::COLOR_THEME);
	menu_items[(size_t)MENU_OPTION::PAUSE] = std::make_unique<PauseItem>();

	set_color_theme(GLOBAL_COLOR_THEME);

	resize_update();
}

MainMenu::~MainMenu()
{
	update_pref_file();
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
	// the viewport needs to be reset
	SDL_RenderSetViewport(main_renderer, NULL);

	// center all menu_items
	int padding_y = height / menu_items.size() / 2;
	for (size_t i = 0; i < menu_items.size(); ++i)
	{
		menu_items[i]->render((width / 2) - (menu_items[i]->get_width() / 2), i * (height / menu_items.size()) + padding_y - (menu_items[i]->get_height() / 2));
	}
}

void MainMenu::update_from_pref_file()
{
	// update menu_items' values according the preferences file
	static_cast<IncrementerMenuItem&>(get_item(MENU_OPTION::N_PLAYER_CHANGE)).set_cur_val(pref_file.get_preference_data(PREF_OPTIONS::N_PLAYERS));
	static_cast<GridSizeMenuItem&>(get_item(MENU_OPTION::GRID_SIZE)).set_cur_val(pref_file.get_preference_data(PREF_OPTIONS::GRID_SIZE));
	static_cast<BoolMenuItem&>(get_item(MENU_OPTION::AI_TOGGLE)).set_cur_val(pref_file.get_preference_data(PREF_OPTIONS::AI_ENABLED));
	static_cast<ThemeMenuItem&>(get_item(MENU_OPTION::COLOR_THEME)).set_cur_val(static_cast<const COLOR_THEME>(pref_file.get_preference_data(PREF_OPTIONS::COLOR_THEME)));
}

void MainMenu::update_pref_file()
{
	int game_x, game_y;
	SDL_GetWindowPosition(main_window, &game_x, &game_y);
	pref_file.set_preference_data(PREF_OPTIONS::WINDOW_X, game_x);
	pref_file.set_preference_data(PREF_OPTIONS::WINDOW_Y, game_y);

	int game_width, game_height;
	SDL_GetRendererOutputSize(main_renderer, &game_width, &game_height);
	pref_file.set_preference_data(PREF_OPTIONS::RESOLUTION_WIDTH, game_width);
	pref_file.set_preference_data(PREF_OPTIONS::RESOLUTION_HEIGHT, game_height);

	int n_players = static_cast<const IncrementerMenuItem&>(get_item(MENU_OPTION::N_PLAYER_CHANGE)).get_cur_val();
	pref_file.set_preference_data(PREF_OPTIONS::N_PLAYERS, n_players);

	int grid_size = static_cast<const GridSizeMenuItem&>(get_item(MENU_OPTION::GRID_SIZE)).get_cur_val();
	pref_file.set_preference_data(PREF_OPTIONS::GRID_SIZE, grid_size);

	bool ai_enabled = static_cast<const BoolMenuItem&>(get_item(MENU_OPTION::AI_TOGGLE)).get_cur_val();
	pref_file.set_preference_data(PREF_OPTIONS::AI_ENABLED, ai_enabled);

	int color_theme = static_cast<int>(static_cast<const ThemeMenuItem&>(get_item(MENU_OPTION::COLOR_THEME)).get_cur_val());
	pref_file.set_preference_data(PREF_OPTIONS::COLOR_THEME, color_theme);

	pref_file.write_preferences_data();
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
	SDL_GetRendererOutputSize(main_renderer, &width, &height);
}
