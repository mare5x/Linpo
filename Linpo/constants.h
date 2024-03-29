#pragma once

#include "SDL.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const char BASE_FONT_SIZE = 16;
const int TTF_DPI = 72;

const int FPS_CAP = 60;
const int TICKS_PER_FRAME = 1000 / FPS_CAP;

const bool VSYNC_ENABLED = false;

const int DEFAULT_GRID_SIZE = 8;

const int N_PLAYERS = 2;
const int MAX_PLAYERS = 4;
const int MIN_PLAYERS = 2;
const bool AI_ENABLED = true;


enum COLOR_NAME
{
	LIGHT_BLUE,
	BLOOD_RED,
	GREEN,
	ORANGE,
	WHITE,
	BLACK,
	BLUE,
	RED,
	GREY_YELLOW,
	DARK_GOLD,
	LIME,
	N_COLORS
};

const SDL_Color COLORS[COLOR_NAME::N_COLORS] = {
	{ 0, 162, 232, 255},	 // LIGHT BLUE
	{ 255, 50, 50, 255},	 // BLOOD RED
	{ 0, 255, 0, 255 },		 // GREEN
	{ 255, 140, 0, 255 },	 // ORANGE
	{ 255, 255, 255, 255 },  //	WHITE
	{ 0, 0, 0, 255 },		 // BLACK
	{ 0, 0, 255, 255 },		 // BLUE
	{ 255, 0, 0, 255 },		 // RED
	{ 239, 228, 176, 255 },	 // GREY YELLOW
	{ 210, 163, 0, 255 },	 // DARK GOLD
	{ 217, 245, 14, 255 }	 // LIME
};


enum class MENU_OPTION
{
	RESTART_GAME,
	N_PLAYER_CHANGE,
	GRID_SIZE,
	AI_TOGGLE,
	COLOR_THEME,
	PAUSE,
	_N_MENU_OPTIONS,
	NULL_OPTION,
	_N_OPTIONS
};

enum class COLOR_THEME
{
	DEFAULT,
	BLACK,
	N_THEMES
};