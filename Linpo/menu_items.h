#pragma once

#include <memory>
#include <string>
#include "SDL.h"
#include "text_texture.h"


class MenuItem
{
public:
	MenuItem(std::string name);

	void handle_event(SDL_Event &e) { }
	void render(int x, int y);

	int get_width() const;
	int get_height() const;
protected:
	int width, height;

	std::unique_ptr<TextTexture> texture;
};

class IncrementerMenuItem : public MenuItem
{
public:
	IncrementerMenuItem(std::string name) : MenuItem::MenuItem(name) { }
};

class BoolMenuItem : public MenuItem
{
public:
	BoolMenuItem(std::string name) : MenuItem::MenuItem(name) { }
};