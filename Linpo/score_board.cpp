#include <sstream>
#include "score_board.h"
#include "render_functions.h"


ScoreBoardBase::ScoreBoardBase(PlayerArray &players_array, Grid &game_grid)
	:viewport_rect{ 0, 4, SCREEN_WIDTH, static_cast<int>(FONT_SIZE + 4) },
	_prev_score{ 0 },
	_base_score_texture_width{ 0 },
	game_grid(game_grid), 
	players(players_array)
{
	for (int i = 0; i < players.max_size(); ++i)
		scoreboard_textures[i] = std::make_unique<TextTexture>(main_renderer);

	update_scoreboard_textures_full();
	update_texture_positions();

	_base_score_texture_width = scoreboard_textures[0]->get_width();
}

void ScoreBoardBase::handle_event(SDL_Event &e)
{
	if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
		resize_update();
}

void ScoreBoardBase::render()
{
	SDL_RenderSetViewport(main_renderer, &viewport_rect);

	if (game_grid.score_changed(_prev_score))
		if (are_full_textures_overlapping())
			update_scoreboard_textures_score_only();
		else
			update_scoreboard_textures_full();

	for (int i = 0; i < players.size(); ++i)
	{
		scoreboard_textures[i]->render();
	}

	//set_render_draw_color(COLORS[BLUE]);
	//SDL_RenderDrawRect(main_renderer, NULL);
}

void ScoreBoardBase::resize_update()
{
	SDL_GetRendererOutputSize(main_renderer, &viewport_rect.w, NULL);
	//viewport_rect.h = 0.05 * viewport_rect.h;

	update_texture_positions();

	if (are_full_textures_overlapping())
		update_scoreboard_textures_score_only();
	else
		update_scoreboard_textures_full();

	update_texture_positions();
}

void ScoreBoardBase::clear()
{
	_prev_score = 0;
	resize_update();
}

void ScoreBoardBase::update_scoreboard_textures_full()
{
	for (int i = 0; i < players.size(); ++i)
	{
		std::stringstream s;
		s << "Player " << i + 1 << ": " << players[i].score;
		scoreboard_textures[i]->write_text(s.str(), *players[i].color);
	}
}

void ScoreBoardBase::update_scoreboard_textures_score_only()
{
	for (int i = 0; i < players.size(); ++i)
	{
		std::stringstream s;
		s << players[i].score;
		scoreboard_textures[i]->write_text(s.str(), *players[i].color);
	}
}

bool ScoreBoardBase::are_textures_overlapping() const
{
	SDL_Rect prev_rect{};
	for (int i = 0; i < players.size(); ++i)
	{
		SDL_Rect texture_rect = scoreboard_textures[i]->get_render_pos();
		if (SDL_HasIntersection(&prev_rect, &texture_rect) == SDL_TRUE)
			return true;
		prev_rect = texture_rect;
	}
	return false;
}

bool ScoreBoardBase::are_full_textures_overlapping() const
{
	return _calc_full_x(1) - (_calc_full_x(0) + _base_score_texture_width) < 4;
}

int ScoreBoardBase::_calc_full_x(int index) const
{
	return (get_width() / players.size() / 2) + (index * (get_width() / players.size())) - (_base_score_texture_width / 2);
}

void ScoreBoardBase::update_texture_positions()
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

const int ScoreBoardBase::get_width() const
{
	return viewport_rect.w;
}

const int ScoreBoardBase::get_height() const
{
	return viewport_rect.h;
}

ScoreBoard::ScoreBoard(PlayerArray& players_array, Grid & game_grid)
	:ScoreBoardBase::ScoreBoardBase(players_array, game_grid),
	pause_item(std::make_unique<PauseItem>(get_height(), get_height())),
	undo_item(std::make_unique<UndoItem>(get_height(), get_height()))
{
	update_texture_positions();
}

void ScoreBoard::handle_event(SDL_Event & e)
{
	ScoreBoardBase::handle_event(e);
	pause_item->handle_event(e);
	undo_item->handle_event(e);
}

void ScoreBoard::render()
{
	ScoreBoardBase::render();

	SDL_Rect undo_rect;
	undo_rect.x = get_width();
	undo_rect.y = 0;
	undo_rect.w = get_height();
	undo_rect.h = get_height();
	undo_item->render(undo_rect);

	SDL_Rect pause_rect;
	pause_rect.x = undo_rect.x + undo_rect.w + (0.5 * get_height());
	pause_rect.y = 0;
	pause_rect.w = undo_rect.w;
	pause_rect.h = undo_rect.h;
	pause_item->render(pause_rect);
}

bool ScoreBoard::pause_item_clicked()
{
	return pause_item->was_clicked();
}

bool ScoreBoard::undo_item_clicked()
{
	return undo_item->was_clicked();
}

const int ScoreBoard::get_width() const
{
	return ScoreBoardBase::get_width() - (3 * get_height());
}
