#include <sstream>
#include "pref_file.h"
#include "constants.h"

PrefFile::PrefFile(const char * file_name)
	:rw_ops(nullptr),
	preferences_data(static_cast<int>(PREF_OPTIONS::N_PREF_OPTIONS))
{
	char* dir_path = SDL_GetPrefPath("mare5", "linpo");

	std::stringstream full_path;
	full_path << dir_path << file_name;

	SDL_free(dir_path);
	SDL_Log("Preferences file path: %s\n", full_path.str().c_str());

	// try to open a file for reading and writing; if it doesn't exist, create it
	rw_ops = SDL_RWFromFile(full_path.str().c_str(), "rb+");
	if (rw_ops == NULL)
	{
		// create the file
		rw_ops = SDL_RWFromFile(full_path.str().c_str(), "wb+");

		// initialize it
		preferences_data[static_cast<int>(PREF_OPTIONS::WINDOW_X)] = SDL_WINDOWPOS_UNDEFINED;
		preferences_data[static_cast<int>(PREF_OPTIONS::WINDOW_Y)] = SDL_WINDOWPOS_UNDEFINED;
		preferences_data[static_cast<int>(PREF_OPTIONS::RESOLUTION_WIDTH)] = SCREEN_WIDTH;
		preferences_data[static_cast<int>(PREF_OPTIONS::RESOLUTION_HEIGHT)] = SCREEN_HEIGHT;
		preferences_data[static_cast<int>(PREF_OPTIONS::N_PLAYERS)] = N_PLAYERS;
		preferences_data[static_cast<int>(PREF_OPTIONS::GRID_SIZE)] = DEFAULT_GRID_SIZE;
		preferences_data[static_cast<int>(PREF_OPTIONS::AI_ENABLED)] = static_cast<int>(AI_ENABLED);
		preferences_data[static_cast<int>(PREF_OPTIONS::COLOR_THEME)] = static_cast<int>(COLOR_THEME::DEFAULT);
	}
	else
	{
		read_preferences_data();
	}

	if (rw_ops == NULL)
		SDL_Log("Error initializing preferences file! %s\n", SDL_GetError());
}

PrefFile::~PrefFile()
{
	SDL_RWclose(rw_ops);
}

int PrefFile::read_option(const PREF_OPTIONS & option) const
{
	int result = -1;
	int position = static_cast<int>(option) * sizeof(int);
	SDL_RWseek(rw_ops, position, RW_SEEK_SET);
	SDL_RWread(rw_ops, &result, sizeof(int), 1);
	return result;
}

void PrefFile::write_option(const PREF_OPTIONS & option, int data)
{
	int position = static_cast<int>(option) * sizeof(int);
	SDL_RWseek(rw_ops, position, RW_SEEK_SET);
	SDL_RWwrite(rw_ops, &data, sizeof(int), 1);
	preferences_data[static_cast<size_t>(option)] = data;
}

void PrefFile::write_preferences_data() const
{
	SDL_Log("Writing preferences data!");
	SDL_RWseek(rw_ops, 0, RW_SEEK_SET);  // seek to the start of the file
	for (int i = 0; i < preferences_data.size(); i++)
	{
		SDL_RWwrite(rw_ops, &preferences_data[i], sizeof(int), 1);
	}
}

void PrefFile::read_preferences_data()
{
	SDL_Log("Reading preferences data!");
	SDL_RWseek(rw_ops, 0, RW_SEEK_SET);  // seek to the start of the file
	for (int i = 0; i < preferences_data.size(); i++)
	{
		SDL_RWread(rw_ops, &preferences_data[i], sizeof(int), 1);
	}
}
