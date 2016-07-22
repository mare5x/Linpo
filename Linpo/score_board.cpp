#include <sstream>
#include "score_board.h"
#include "render_functions.h"


ScoreBoard::ScoreBoard(std::array<Player, N_PLAYERS>& players_array) : players(players_array)
{
	viewport_rect.x = 0;
	viewport_rect.y = 0;
	viewport_rect.w = SCREEN_WIDTH;
	viewport_rect.h = 20;
	
	width = SCREEN_WIDTH;
}

void ScoreBoard::handle_event(SDL_Event & e)
{
	if (e.type == SDL_WINDOWEVENT &&e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
		resize();
}

void ScoreBoard::render()
{
	SDL_RenderSetViewport(main_renderer, &viewport_rect);

	int prev_str_len = 0;
	for (int i = 0; i < N_PLAYERS; ++i)
	{
		SDL_Point top_left = { (width / 2 / (N_PLAYERS - i)) + (prev_str_len * 8), 4 };
		std::stringstream s;
		s << "Player " << i + 1 << ": " << players[i].score;
		auto score_str = s.str();
		prev_str_len += score_str.length();
		render_string(score_str, top_left, players[i].color);
	}
}

void ScoreBoard::resize()
{
	SDL_GetRendererOutputSize(main_renderer, &width, NULL);

	viewport_rect.w = width;
}
