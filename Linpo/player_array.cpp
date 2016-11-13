#include "player_array.h"
#include "constants.h"


PlayerArray::PlayerArray()
	:players_in_game{N_PLAYERS},
	players{}
{
	for (int i = 0; i < players.size(); ++i)
	{
		players[i].id = i;
		players[i].color = &COLORS[i];
		players[i].last_moves.push_back({});
	}
}

void PlayerArray::set_players(int players)
{
	if (players > MAX_PLAYERS) players_in_game = MAX_PLAYERS;
	else if (players < MIN_PLAYERS) players_in_game = MIN_PLAYERS;
	else players_in_game = players;
}

void PlayerArray::add()
{
	players_in_game = players_in_game < MAX_PLAYERS ? players_in_game + 1 : MAX_PLAYERS;
}

void PlayerArray::remove()
{
	players_in_game = players_in_game > MIN_PLAYERS ? players_in_game - 1 : MIN_PLAYERS;
}

PlayerArray::iterator PlayerArray::begin()
{
	return players.begin();
}

const PlayerArray::const_iterator PlayerArray::begin() const
{
	return players.begin();
}

PlayerArray::iterator PlayerArray::end()
{
	return players.end();
}

const PlayerArray::const_iterator PlayerArray::end() const
{
	return players.end();
}

const Player & PlayerArray::operator[](size_t index) const
{
	return players[index];
}

Player & PlayerArray::operator[](size_t index)
{
	// call the const version of operator[]
	//return const_cast<Player &>(static_cast<const PlayerArray&>(*this)[index]);
	return players[index];
}
