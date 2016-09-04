#pragma once

#include <memory>
#include <array>
#include "player_array.h"
#include "constants.h"
#include "globals.h"
#include "text_texture.h"
#include "grid.h"
#include "menu_items.h"


class ScoreBoard
{
public:
	ScoreBoard(PlayerArray &players_array, Grid &game_grid);

	virtual void handle_event(SDL_Event &e);
	virtual void render();
	void resize_update();

	void clear();

	void update_scoreboard_textures();
	void update_texture_positions();

	virtual const int get_width() const;
	virtual const int get_height() const;
private:
	SDL_Rect viewport_rect;
	int _prev_score;

	Grid &game_grid;
	PlayerArray &players;

	std::array<std::unique_ptr<TextTexture>, MAX_PLAYERS> scoreboard_textures;
};

class ScoreBoardWPauseItem : public ScoreBoard
{
public:
	ScoreBoardWPauseItem(PlayerArray &players_array, Grid &game_grid);

	void handle_event(SDL_Event &e);
	void render();

	bool item_clicked();

	// get width of scoreboard area only
	const int get_width() const;
private:
	std::unique_ptr<PauseItem> pause_item;
};