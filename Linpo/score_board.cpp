#include <sstream>
#include "score_board.h"
#include "render_functions.h"


ScoreBoard::ScoreBoard(PlayerArray &players_array, Grid &game_grid)
	:viewport_rect{ 0, 0, SCREEN_WIDTH, static_cast<int>(0.05 * SCREEN_HEIGHT) },
	_prev_score{ 0 },
	game_grid(game_grid), 
	players(players_array)
{
	for (int i = 0; i < players.max_size(); ++i)
		scoreboard_textures[i] = std::make_unique<TextTexture>(main_renderer);

	update_scoreboard_textures();
	update_texture_positions();
}

void ScoreBoard::handle_event(SDL_Event &e)
{
	if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
		resize_update();
}

void ScoreBoard::render()
{
	SDL_RenderSetViewport(main_renderer, &viewport_rect);

	if (game_grid.score_changed(_prev_score))
		update_scoreboard_textures();

	for (int i = 0; i < players.size(); ++i)
	{
		scoreboard_textures[i]->render();
	}
}

void ScoreBoard::resize_update()
{
	SDL_GetRendererOutputSize(main_renderer, &viewport_rect.w, &viewport_rect.h);
	viewport_rect.h = 0.05 * viewport_rect.h;

	update_scoreboard_textures();
	update_texture_positions();
}

void ScoreBoard::clear()
{
	_prev_score = 0;
	resize_update();
}

void ScoreBoard::update_scoreboard_textures()
{
	for (int i = 0; i < players.size(); ++i)
	{
		std::stringstream s;
		s << "Player " << i + 1 << ": " << players[i].score;
		scoreboard_textures[i]->write_text(s.str(), *players[i].color);
	}
}

void ScoreBoard::update_texture_positions()
{
	for (int i = 0; i < players.size(); ++i)
	{
		int text_h = scoreboard_textures[i]->get_height();
		int text_w = scoreboard_textures[i]->get_width();
		SDL_Point top_left_render_pos;
		top_left_render_pos.x = (get_width() / players.size() / 2) + (i * (get_width() / players.size())) - (text_w / 2);
		top_left_render_pos.y = (get_height() / 2) - (text_h / 2);
		scoreboard_textures[i]->set_render_pos(top_left_render_pos);
	}
}

const int ScoreBoard::get_width() const
{
	return viewport_rect.w;
}

const int ScoreBoard::get_height() const
{
	return viewport_rect.h;
}

ScoreBoardWPauseItem::ScoreBoardWPauseItem(PlayerArray& players_array, Grid & game_grid)
	:ScoreBoard::ScoreBoard(players_array, game_grid),
	pause_item(std::make_unique<PauseItem>())
{
	update_texture_positions();
}

void ScoreBoardWPauseItem::handle_event(SDL_Event & e)
{
	ScoreBoard::handle_event(e);
	pause_item->handle_event(e);
}

void ScoreBoardWPauseItem::render()
{
	ScoreBoard::render();
	SDL_Rect dest_rect;
	dest_rect.x = get_width();
	dest_rect.y = 0;
	dest_rect.w = get_height();
	dest_rect.h = get_height();
	pause_item->render(dest_rect);
}

bool ScoreBoardWPauseItem::item_clicked()
{
	return pause_item->was_clicked();
}

const int ScoreBoardWPauseItem::get_width() const
{
	return ScoreBoard::get_width() - 2 * get_height();
}
