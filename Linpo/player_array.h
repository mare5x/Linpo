#pragma once

#include <SDL.h>
#include <array>
#include "constants.h"


struct Player
{
	char id;
	int score;
	const SDL_Color* color;
	bool is_ai;
};


class PlayerArray
{
public:
	typedef std::array<Player, MAX_PLAYERS>::iterator iterator;
	typedef std::array<Player, MAX_PLAYERS>::const_iterator const_iterator;

	PlayerArray();

	void set_players(int players);
	void add();
	void remove();

	iterator begin();
	const const_iterator begin() const;
	iterator end();
	const const_iterator end() const;

	size_t size() const { return players_in_game; }
	size_t max_size() const { return MAX_PLAYERS; }

	const Player& operator[](size_t index) const;
	Player& operator[](size_t index);
private:
	int players_in_game;

	std::array<Player, MAX_PLAYERS> players;
};