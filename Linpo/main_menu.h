#pragma once

#include "SDL.h"
#include <array>
#include "constants.h"
#include "menu_items.h"
#include "pref_file.h"


class MainMenu
{
public:
	MainMenu();
	~MainMenu();

	void handle_event(SDL_Event &e);
	void render();
	/* Updates MainMenu data with the preferences file. */
	void update_from_pref_file();

	/* Updates the preferences file with the currently selected options. */
	void update_pref_file();
	void write_pref_option(const PREF_OPTIONS &option, int data) { pref_file.write_option(option, data); }
	const PrefFile& get_pref_file() const { return pref_file; }
	const std::vector<int>& get_preferences_data() const { return pref_file.get_preferences_data(); }

	MENU_OPTION get_selected_option();
	const AbstractMenuItem& get_item(const MENU_OPTION& option) const { return *menu_items[static_cast<size_t>(option)]; }
	AbstractMenuItem& get_item(const MENU_OPTION& option) { return *menu_items[static_cast<size_t>(option)]; }

	void set_color_theme(const COLOR_THEME &color_theme) const;

	void toggle_visibility();
	bool is_visible();
private:
	void resize_update();

	bool visible;
	int width, height;

	PrefFile pref_file;
	std::array<std::unique_ptr<AbstractMenuItem>, (int)MENU_OPTION::_N_MENU_OPTIONS> menu_items;
};

