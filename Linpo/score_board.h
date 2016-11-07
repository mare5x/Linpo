#pragma once

#include <memory>
#include <array>
#include "player_array.h"
#include "constants.h"
#include "globals.h"
#include "text_texture.h"
#include "grid.h"
#include "menu_items.h"


class ScoreBoardBase
{
public:
	ScoreBoardBase(PlayerArray &players_array, Grid &game_grid);

	virtual void handle_event(SDL_Event &e);
	virtual void render();
	void resize_update();

	void clear();

	//void set_color_theme(const COLOR_THEME &color_theme);

	virtual const int get_width() const;
	virtual const int get_height() const;
protected:
	void update_texture_positions();
private:
	void update_scoreboard_textures_full();
	void update_scoreboard_textures_score_only();

	bool are_textures_overlapping() const;
	/* Returns whether the textures of the full length score format ("Player 1: 0") are overlapping. */
	bool are_full_textures_overlapping() const;

	/* Calculates the the x coordinate of a full length score format texture at player index location index. */
	int _calc_full_x(int index) const;

	SDL_Rect viewport_rect;
	int _prev_score;
	int _base_score_texture_width;

	Grid &game_grid;
	PlayerArray &players;

	std::array<std::unique_ptr<TextTexture>, MAX_PLAYERS> scoreboard_textures;
};

class ScoreBoard : public ScoreBoardBase
{
public:
	ScoreBoard(PlayerArray &players_array, Grid &game_grid);

	void handle_event(SDL_Event &e);
	void render();

	bool pause_item_clicked();
	bool undo_item_clicked();

	// get width of scoreboard area only (excluding items)
	const int get_width() const override;
private:
	std::unique_ptr<PauseItem> pause_item;
	std::unique_ptr<UndoItem> undo_item;
};