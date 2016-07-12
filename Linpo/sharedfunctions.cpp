#include <SDL.h>
#include <iostream>
#include "sharedfunctions.h"


void log(const std::string& message, const std::string append_string)
{
	std::cerr << message << append_string << std::endl;
}
