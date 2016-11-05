#pragma once
#include <vector>
#include "SDL.h"

enum class PREF_OPTIONS
{
	WINDOW_X,
	WINDOW_Y,
	RESOLUTION_WIDTH,
	RESOLUTION_HEIGHT,
	N_PLAYERS,
	GRID_SIZE,
	AI_ENABLED,
	COLOR_THEME,
	N_PREF_OPTIONS
};

class PrefFile
{
public:
	PrefFile(const char* file_name);
	~PrefFile();

	const std::vector<int>& get_preferences_data() const { return preferences_data; }
	std::vector<int>& get_preferences_data() { return preferences_data; }
	void set_preferences_data(const std::vector<int>& data) { preferences_data = data; }

	int get_preference_data(const PREF_OPTIONS &option) const { return preferences_data[static_cast<size_t>(option)]; }
	void set_preference_data(const PREF_OPTIONS &option, int data) { preferences_data[static_cast<size_t>(option)] = data; }

	int read_option(const PREF_OPTIONS &option) const;
	void write_option(const PREF_OPTIONS &option, int data);

	/* Writes preferences_data into the preferences file. */
	void write_preferences_data() const;
private:
	/* Reads the preferences file data into preferences_data. */
	void read_preferences_data();

	SDL_RWops* rw_ops;
	std::vector<int> preferences_data;
};