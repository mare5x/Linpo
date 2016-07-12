#pragma once
#include "SDL2_gfxPrimitives.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

enum Player
{
	PLAYER1 = 1,
	PLAYER2
};

const SDL_Color PLAYER1_COLOR = { 255, 0, 0, 255 };
const SDL_Color PLAYER2_COLOR = { 0, 0, 255, 255 };
