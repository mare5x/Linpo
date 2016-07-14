#pragma once
#include "SDL2_gfxPrimitives.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int FPS_CAP = 60;
const int TICKS_PER_FRAME = 1000 / FPS_CAP;

const bool VSYNC_ENABLED = false;


enum Player
{
	PLAYER1 = 1,
	PLAYER2
};

const SDL_Color PLAYER1_COLOR = { 255, 0, 0, 255 };
const SDL_Color PLAYER2_COLOR = { 0, 0, 255, 255 };
