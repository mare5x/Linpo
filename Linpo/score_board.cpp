#include <sstream>
#include "score_board.h"
#include "render_functions.h"


ScoreBoard::ScoreBoard(std::array<Player, N_PLAYERS>& players_array, Grid &game_grid) : players(players_array), game_grid(game_grid)
{
	viewport_rect.x = 0;
	viewport_rect.y = 0;
	viewport_rect.w = SCREEN_WIDTH;
	viewport_rect.h = 20;
	
	_prev_score = 0;

	scoreboard_texture = new TextureWrapper(main_renderer, viewport_rect.w, viewport_rect.h);
	update_scoreboard_texture();
}

ScoreBoard::~ScoreBoard()
{
	delete scoreboard_texture;
}

void ScoreBoard::handle_event(SDL_Event & e)
{
	if (e.type == SDL_WINDOWEVENT &&e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
		resize();
}

void ScoreBoard::render()
{
	SDL_RenderSetViewport(main_renderer, &viewport_rect);

	if (game_grid.score_changed(_prev_score))
		update_scoreboard_texture();

	scoreboard_texture->render();
}

void ScoreBoard::resize()
{
	SDL_GetRendererOutputSize(main_renderer, &viewport_rect.w, NULL);
	scoreboard_texture->resize(viewport_rect.w, viewport_rect.h);
	update_scoreboard_texture();
}

void ScoreBoard::update_scoreboard_texture()
{
	scoreboard_texture->clear();

	int prev_str_len = 0;
	for (int i = 0; i < N_PLAYERS; ++i)
	{
		SDL_Point top_left = { (0.05 * viewport_rect.w) + (8 * prev_str_len), 4 };
		std::stringstream s;
		s << "Player " << i + 1 << ": " << players[i].score;
		auto score_str = s.str();
		prev_str_len = score_str.length() + 2;
		render_string(score_str, top_left, players[i].color);
	}
	scoreboard_texture->reset_render_target();
}
