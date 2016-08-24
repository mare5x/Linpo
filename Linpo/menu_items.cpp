#include "menu_items.h"
#include "globals.h"
#include "constants.h"


MenuItem::MenuItem(std::string name)
	:texture {std::make_unique<TextTexture>(main_renderer)}
{
	texture->write_text(name, COLORS[BLACK]);

	width = texture->get_width();
	height = texture->get_height();
}

void MenuItem::render(int x, int y)
{
	texture->render(x, y);
}

int MenuItem::get_width() const
{
	return width;
}

int MenuItem::get_height() const
{
	return height;
}
