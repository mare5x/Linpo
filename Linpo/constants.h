#pragma once

#include "SDL.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const char BASE_FONT_SIZE = 16;
const int TTF_DPI = 72;

const int FPS_CAP = 60;
const int TICKS_PER_FRAME = 1000 / FPS_CAP;

const bool VSYNC_ENABLED = false;

const int N_PLAYERS = 2;
const bool AI_ENABLED = true;


enum COLOR_NAME
{
	BLUE,
	RED,
	GREEN,
	WHITE,
	BLACK,
	N_COLORS
};

const SDL_Color COLORS[COLOR_NAME::N_COLORS] = {
	{ 0, 0, 255, 255 },		 // BLUE
	{ 255, 0, 0, 255 },		 // RED
	{ 0, 255, 0, 255 },		 // GREEN
	{ 255, 255, 255, 255 },  //	WHITE
	{ 0, 0, 0, 255 }		 // BLACK
};


enum class MENU_OPTION
{
	RESTART_GAME,
	N_PLAYER_CHANGE,
	AI_TOGGLE,
	PAUSE,
	_N_MENU_OPTIONS,
	NULL_OPTION,
	_N_OPTIONS
};
