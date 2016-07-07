#include <SDL.h>
#include <string>
#include <iostream>
#include "sharedfunctions.h"


void log(std::ostream& dest, const std::string& message, const std::string append_string)
{
	dest << message << append_string << std::endl;
}
